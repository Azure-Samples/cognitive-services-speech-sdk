#include "stdafx.h"
#include <future>
#include "recognizer.h"


namespace CARBON_IMPL_NAMESPACE() {


CSpxRecognizer::CSpxRecognizer()
{
    SPX_DBG_TRACE_FUNCTION();
}

CSpxRecognizer::CSpxRecognizer(const std::wstring& language)
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
    SPX_DBG_TRACE_FUNCTION();
    SPX_TRACE_ERROR("%s isn't implemented yet...", __FUNCTION__);
    SPX_THROW_HR(SPXERR_NOT_IMPL);

    throw SPXERR_NOT_IMPL;
}

void CSpxRecognizer::Enable()
{
    SPX_DBG_TRACE_FUNCTION();
    SPX_TRACE_ERROR("%s isn't implemented yet...", __FUNCTION__);
    SPX_THROW_HR(SPXERR_NOT_IMPL);
    throw SPXERR_NOT_IMPL;
}

void CSpxRecognizer::Disable()
{
    SPX_DBG_TRACE_FUNCTION();
    SPX_TRACE_ERROR("%s isn't implemented yet...", __FUNCTION__);
    SPX_THROW_HR(SPXERR_NOT_IMPL);
    throw SPXERR_NOT_IMPL;
}

std::future<std::shared_ptr<CSpxRecognitionResult>> CSpxRecognizer::RecognizeAsync()
{
    SPX_DBG_TRACE_FUNCTION();

    std::packaged_task<std::shared_ptr<CSpxRecognitionResult>()> taskHack([](){
        std::this_thread::sleep_for(std::chrono::seconds(2));
        return std::make_shared<CSpxRecognitionResult>();
    });
    auto futureHack = taskHack.get_future();

    std::thread task_td_hack(std::move(taskHack));
    task_td_hack.detach();

    return futureHack;
}

std::future<void> CSpxRecognizer::StartContinuousRecognitionAsync()
{
    SPX_DBG_TRACE_FUNCTION();
    SPX_TRACE_ERROR("%s isn't implemented yet...", __FUNCTION__);
    SPX_THROW_HR(SPXERR_NOT_IMPL);
    throw SPXERR_NOT_IMPL;
}

std::future<void> CSpxRecognizer::StopContinuousRecognitionAsync()
{
    SPX_DBG_TRACE_FUNCTION();
    SPX_TRACE_ERROR("%s isn't implemented yet...", __FUNCTION__);
    SPX_THROW_HR(SPXERR_NOT_IMPL);
    throw SPXERR_NOT_IMPL;
}


}; // CARBON_IMPL_NAMESPACE()
