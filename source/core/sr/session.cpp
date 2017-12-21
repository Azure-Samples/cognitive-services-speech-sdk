#include "stdafx.h"
#include <future>
#include "session.h"
#include "recognition_result.h"
#include "guid_utils.h"


namespace CARBON_IMPL_NAMESPACE() {


CSpxSession::CSpxSession() :
    m_fRecoAsyncWaiting(false),
    m_sessionId(PAL_CreateGuid())
{
    SPX_DBG_TRACE_FUNCTION();
}

CSpxSession::~CSpxSession()
{
    SPX_DBG_TRACE_FUNCTION();
}

const std::wstring& CSpxSession::GetSessionId() const
{
    return m_sessionId;
}

void CSpxSession::AddRecognizer(std::shared_ptr<ISpxRecognizer> recognizer)
{
     m_recognizers.push_back(recognizer);
}

void CSpxSession::RemoveRecognizer(ISpxRecognizer* recognizer)
{
     m_recognizers.remove_if([&](std::weak_ptr<ISpxRecognizer>& item) {
         std::shared_ptr<ISpxRecognizer> sharedPtr = item.lock();
         return sharedPtr.get() == recognizer;
     });
}

CSpxAsyncOp<std::shared_ptr<ISpxRecognitionResult>> CSpxSession::RecognizeAsync()
{
    SPX_DBG_TRACE_FUNCTION();

    std::packaged_task<std::shared_ptr<ISpxRecognitionResult>()> taskHack([=](){

        this->StartRecognizing();

        auto result = this->WaitForRecognition();
        this->StopRecognizing();

        return result;
    });

    auto futureHack = taskHack.get_future();
    std::thread task_td_hack(std::move(taskHack));
    task_td_hack.detach();

    return CSpxAsyncOp<std::shared_ptr<ISpxRecognitionResult>>(
        std::forward<std::future<std::shared_ptr<ISpxRecognitionResult>>>(futureHack),
        AOS_Started);
}

CSpxAsyncOp<void> CSpxSession::StartContinuousRecognitionAsync()
{
    SPX_DBG_TRACE_FUNCTION();
    
    std::packaged_task<void()> taskHack([=](){
        this->StartRecognizing();
    });

    auto futureHack = taskHack.get_future();
    std::thread task_td_hack(std::move(taskHack));
    task_td_hack.detach();

    return CSpxAsyncOp<void>(
        std::forward<std::future<void>>(futureHack),
        AOS_Started);    
}

CSpxAsyncOp<void> CSpxSession::StopContinuousRecognitionAsync()
{
    SPX_DBG_TRACE_FUNCTION();
    
    std::packaged_task<void()> taskHack([=](){
        this->StopRecognizing();
    });

    auto futureHack = taskHack.get_future();
    std::thread task_td_hack(std::move(taskHack));
    task_td_hack.detach();

    return CSpxAsyncOp<void>(
        std::forward<std::future<void>>(futureHack),
        AOS_Started);    
}

void CSpxSession::StartRecognizing()
{
    SPX_DBG_TRACE_SCOPE("Sleeping for 500ms...", "Sleeping for 500ms... Done!");
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
}

void CSpxSession::StopRecognizing()
{
    SPX_DBG_TRACE_SCOPE("Sleeping for 1000ms...", "Sleeping for 1000ms... Done!");
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

std::shared_ptr<ISpxRecognitionResult> CSpxSession::WaitForRecognition()
{
    std::unique_lock<std::mutex> lock(m_mutex);

    m_fRecoAsyncWaiting = true;
    m_cv.wait_for(lock, std::chrono::seconds(m_recoAsyncTimeout), [&] { return !m_fRecoAsyncWaiting; });

    if (!m_recoAsyncResult) // If we don't have a result, make a 'NoMatch' result
    {
        lock.unlock();
        EnsureFireResultEvent();
    }

    return std::move(m_recoAsyncResult);
}

void CSpxSession::WaitForRecognition_Complete(std::shared_ptr<ISpxRecognitionResult> result)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    m_fRecoAsyncWaiting = false;
    m_recoAsyncResult = result;

    m_cv.notify_all();
    lock.unlock();

    FireResultEvent(GetSessionId(), result);
}

void CSpxSession::FireSessionStartedEvent()
{
    // Make a copy of the recognizers (under lock), to use to send events; 
    // otherwise the underlying list could be modified while we're sending events...

    std::unique_lock<std::mutex> lock(m_mutex);
    decltype(m_recognizers) weakRecognizers(m_recognizers.begin(), m_recognizers.end());
    lock.unlock();

    for (auto weakRecognizer : weakRecognizers)
    {
        auto recognizer = weakRecognizer.lock();
        auto ptr = std::dynamic_pointer_cast<ISpxRecognizerEvents>(recognizer);
        if (recognizer)
        {
            ptr->FireSessionStarted(m_sessionId);
        }
    }
}

void CSpxSession::FireSessionStoppedEvent()
{
    EnsureFireResultEvent();

    // Make a copy of the recognizers (under lock), to use to send events; 
    // otherwise the underlying list could be modified while we're sending events...

    std::unique_lock<std::mutex> lock(m_mutex);
    decltype(m_recognizers) weakRecognizers(m_recognizers.begin(), m_recognizers.end());
    lock.unlock();

    for (auto weakRecognizer : weakRecognizers)
    {
        auto recognizer = weakRecognizer.lock();
        auto ptr = std::dynamic_pointer_cast<ISpxRecognizerEvents>(recognizer);
        if (recognizer)
        {
            ptr->FireSessionStopped(m_sessionId);
        }
    }
}

void CSpxSession::FireResultEvent(const std::wstring& sessionId, std::shared_ptr<ISpxRecognitionResult> result)
{
    // Make a copy of the recognizers (under lock), to use to send events; 
    // otherwise the underlying list could be modified while we're sending events...

    std::unique_lock<std::mutex> lock(m_mutex);
    decltype(m_recognizers) weakRecognizers(m_recognizers.begin(), m_recognizers.end());
    lock.unlock();

    for (auto weakRecognizer : weakRecognizers)
    {
        auto recognizer = weakRecognizer.lock();
        auto ptr = std::dynamic_pointer_cast<ISpxRecognizerEvents>(recognizer);
        if (recognizer)
        {
            ptr->FireResultEvent(sessionId, result);
        }
    }
}

void CSpxSession::EnsureFireResultEvent()
{
    if (m_fRecoAsyncWaiting)
    {
        auto noMatchResult = SpxMakeShared<CSpxRecognitionResult, ISpxRecognitionResult>(CSpxRecognitionResult::NoMatch);
        WaitForRecognition_Complete(noMatchResult);
    }
}


}; // CARBON_IMPL_NAMESPACE()
