#include "stdafx.h"
#include <future>
#include "recognizer.h"


namespace CARBON_IMPL_NAMESPACE() {


CSpxRecognizer::CSpxRecognizer() :
    m_fEnabled(true)
{
    SPX_DBG_TRACE_FUNCTION();
}

CSpxRecognizer::CSpxRecognizer(const std::wstring& language) :
    m_fEnabled(true)
{
    SPX_DBG_TRACE_FUNCTION();
    SPX_TRACE_ERROR("%s isn't implemented yet...", __FUNCTION__);
    SPX_THROW_HR(SPXERR_NOT_IMPL);
}

CSpxRecognizer::~CSpxRecognizer()
{
    SPX_DBG_TRACE_FUNCTION();
}

bool CSpxRecognizer::IsEnabled()
{
    return m_fEnabled;
}

void CSpxRecognizer::Enable()
{
    if (m_fEnabled.exchange(true) != true)
    {
        OnIsEnabledChanged();
    }
}

void CSpxRecognizer::Disable()
{
    if (m_fEnabled.exchange(false) != false)
    {
        OnIsEnabledChanged();
    }
}

CSpxAsyncOp<std::shared_ptr<CSpxRecognitionResult>> CSpxRecognizer::RecognizeAsync()
{
    SPX_DBG_TRACE_FUNCTION();

    std::packaged_task<std::shared_ptr<CSpxRecognitionResult>()> taskHack([](){
        std::this_thread::sleep_for(std::chrono::seconds(2));
        return std::make_shared<CSpxRecognitionResult>();
    });
    auto futureHack = taskHack.get_future();

    std::thread task_td_hack(std::move(taskHack));
    task_td_hack.detach();

    return CSpxAsyncOp<std::shared_ptr<CSpxRecognitionResult>>(
        std::forward<std::future<std::shared_ptr<CSpxRecognitionResult>>>(futureHack),
        AOS_Started);
}

CSpxAsyncOp<void> CSpxRecognizer::StartContinuousRecognitionAsync()
{
    SPX_DBG_TRACE_FUNCTION();
    
    std::packaged_task<void()> taskHack([](){
        std::this_thread::sleep_for(std::chrono::seconds(2));
    });
    auto futureHack = taskHack.get_future();

    std::thread task_td_hack(std::move(taskHack));
    task_td_hack.detach();

    return CSpxAsyncOp<void>(
        std::forward<std::future<void>>(futureHack),
        AOS_Started);    
}

CSpxAsyncOp<void> CSpxRecognizer::StopContinuousRecognitionAsync()
{
    SPX_DBG_TRACE_FUNCTION();
    
    std::packaged_task<void()> taskHack([](){
        std::this_thread::sleep_for(std::chrono::seconds(2));
    });
    auto futureHack = taskHack.get_future();

    std::thread task_td_hack(std::move(taskHack));
    task_td_hack.detach();

    return CSpxAsyncOp<void>(
        std::forward<std::future<void>>(futureHack),
        AOS_Started);    
}

void CSpxRecognizer::OnIsEnabledChanged()
{
    // no op currently
}


}; // CARBON_IMPL_NAMESPACE()
