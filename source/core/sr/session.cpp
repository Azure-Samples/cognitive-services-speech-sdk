#include "stdafx.h"
#include <future>
#include "session.h"
#include "guid_utils.h"
#include "service_helpers.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


CSpxSession::CSpxSession() :
    m_recoAsyncWaiting(false),
    m_sessionId(PAL::CreateGuid())
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

    std::shared_ptr<ISpxSession> keepAlive = SpxSharedPtrFromThis<ISpxSession>(this);
    std::packaged_task<std::shared_ptr<ISpxRecognitionResult>()> task([=](){

        SPX_DBG_TRACE_SCOPE("*** CSpxSession::RecognizeAsync kicked-off THREAD started ***", "*** CSpxSession::RecognizeAsync kicked-off THREAD stopped ***");
        auto keepAliveCopy = keepAlive;

        // Keep track of the fact that we have a thread pending waiting to hear
        // what the final recognition result is, and then stop recognizing...
        m_recoAsyncWaiting = true;
        this->StartRecognizing(RecognitionKind::SingleShot);

        // Wait for the recognition result, and then stop recognizing
        auto result = this->WaitForRecognition();
        this->StopRecognizing(RecognitionKind::SingleShot);

        // Return our result back to the future/caller
        return result;
    });

    auto taskFuture = task.get_future();
    std::thread taskThread(std::move(task));
    taskThread.detach();

    return CSpxAsyncOp<std::shared_ptr<ISpxRecognitionResult>>(
        std::forward<std::future<std::shared_ptr<ISpxRecognitionResult>>>(taskFuture),
        AOS_Started);
}

CSpxAsyncOp<void> CSpxSession::StartContinuousRecognitionAsync()
{
    return StartRecognitionAsync(RecognitionKind::Continuous);
}

CSpxAsyncOp<void> CSpxSession::StopContinuousRecognitionAsync()
{
    return StopRecognitionAsync(RecognitionKind::Continuous);
}

CSpxAsyncOp<void> CSpxSession::StartKeywordRecognitionAsync(const std::wstring& keyword)
{
    return StartRecognitionAsync(RecognitionKind::Keyword, keyword);
}

CSpxAsyncOp<void> CSpxSession::StopKeywordRecognitionAsync()
{
    return StopRecognitionAsync(RecognitionKind::Keyword);
}

CSpxAsyncOp<void> CSpxSession::StartRecognitionAsync(RecognitionKind startKind, std::wstring keyword)
{
    SPX_DBG_TRACE_FUNCTION();

    std::shared_ptr<ISpxSession> keepAlive = SpxSharedPtrFromThis<ISpxSession>(this);
    std::packaged_task<void()> task([=](){

        SPX_DBG_TRACE_SCOPE("*** CSpxSession::StartRecognitionAsync kicked-off THREAD started ***", "*** CSpxSession::StartRecognitionAsync kicked-off THREAD stopped ***");
        auto keepAliveCopy = keepAlive;

        this->StartRecognizing(startKind, keyword);
    });

    auto taskFuture = task.get_future();
    std::thread taskThread(std::move(task));
    taskThread.detach();

    return CSpxAsyncOp<void>(
        std::forward<std::future<void>>(taskFuture),
        AOS_Started);    
}

CSpxAsyncOp<void> CSpxSession::StopRecognitionAsync(RecognitionKind stopKind)
{
    SPX_DBG_TRACE_FUNCTION();

    std::shared_ptr<ISpxSession> keepAlive = SpxSharedPtrFromThis<ISpxSession>(this);
    std::packaged_task<void()> task([=](){
        
        SPX_DBG_TRACE_SCOPE("*** CSpxSession::StopRecognitionAsync kicked-off THREAD started ***", "*** CSpxSession::StopRecognitionAsync kicked-off THREAD stopped ***");
        auto keepAliveCopy = keepAlive;

        this->StopRecognizing(stopKind);
    });

    auto taskFuture = task.get_future();
    std::thread taskThread(std::move(task));
    taskThread.detach();

    return CSpxAsyncOp<void>(
        std::forward<std::future<void>>(taskFuture),
        AOS_Started);    
}

void CSpxSession::WaitForRecognition_Complete(std::shared_ptr<ISpxRecognitionResult> result)
{
    FireResultEvent(GetSessionId(), result);

    std::unique_lock<std::mutex> lock(m_mutex);
    if (m_recoAsyncWaiting)
    {
        m_recoAsyncWaiting = false;
        m_recoAsyncResult = result;

        m_cv.notify_all();
    }
}

void CSpxSession::FireSessionStartedEvent()
{
    SPX_DBG_TRACE_FUNCTION();

    FireEvent(EventType::SessionStart);
}

void CSpxSession::FireSpeechStartDetectedEvent(uint64_t offset)
{
    SPX_DBG_TRACE_FUNCTION();
    
    FireEvent(EventType::SpeechStart, nullptr, nullptr, offset);
}

void CSpxSession::FireSpeechEndDetectedEvent(uint64_t offset)
{
    SPX_DBG_TRACE_FUNCTION();

    FireEvent(EventType::SpeechEnd, nullptr, nullptr, offset);
}

void CSpxSession::FireSessionStoppedEvent()
{
    SPX_DBG_TRACE_FUNCTION();
    EnsureFireResultEvent();

    FireEvent(EventType::SessionStop);
}

void CSpxSession::FireResultEvent(const std::wstring& sessionId, std::shared_ptr<ISpxRecognitionResult> result)
{
    SPX_DBG_TRACE_FUNCTION();

    FireEvent(EventType::RecoResultEvent, result, const_cast<wchar_t*>(sessionId.c_str()));
}

void CSpxSession::FireEvent(EventType sessionType, std::shared_ptr<ISpxRecognitionResult> result, wchar_t* sessionId, uint64_t offset)
{
    // Make a copy of the recognizers (under lock), to use to send events; 
    // otherwise the underlying list could be modified while we're sending events...
    
    std::unique_lock<std::mutex> lock(m_mutex);
    decltype(m_recognizers) weakRecognizers(m_recognizers.begin(), m_recognizers.end());
    lock.unlock();

    auto sessionId_local = (sessionId != nullptr) ? sessionId : m_sessionId;

    for (auto weakRecognizer : weakRecognizers)
    {
        auto recognizer = weakRecognizer.lock();
        auto ptr = SpxQueryInterface<ISpxRecognizerEvents>(recognizer);
        if (recognizer)
        {
            switch (sessionType)
            {
                case EventType::SessionStart:
                    ptr->FireSessionStarted(sessionId_local);
                break;

                case EventType::SessionStop:
                    ptr->FireSessionStopped(sessionId_local);
                break;

                case EventType::SpeechStart:
                    ptr->FireSpeechStartDetected(sessionId_local, offset);
                break;

                case EventType::SpeechEnd:
                    ptr->FireSpeechEndDetected(sessionId_local, offset);
                break;

                case EventType::RecoResultEvent:
                    ptr->FireResultEvent(sessionId_local, result);
                break;
            }
        }
    }
}

void CSpxSession::EnsureFireResultEvent()
{
    // Since we're not holding a lock throughout this "ensure" method, a conflict is still possible.
    // That said, the conflict is benign, in the worst case we just created a throw away no-match result.
    if (m_recoAsyncWaiting)
    {
        auto factory = SpxQueryService<ISpxRecoResultFactory>(SpxSharedPtrFromThis<ISpxSession>(this));
        auto noMatchResult = factory->CreateNoMatchResult(ResultType::Speech);
        WaitForRecognition_Complete(noMatchResult);
    }
}


} } } } // Microsoft::CognitiveServices::Speech::Impl
