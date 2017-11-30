#include "stdafx.h"
#include <future>
#include "session.h"
#include "recognition_result.h"


namespace CARBON_IMPL_NAMESPACE() {


CSpxSession::CSpxSession()
{
    SPX_DBG_TRACE_FUNCTION();
}

CSpxSession::~CSpxSession()
{
    SPX_DBG_TRACE_FUNCTION();
}

std::wstring CSpxSession::GetSessionId() const
{
    return m_sessionId;
}

void CSpxSession::AddRecognizer(std::shared_ptr<ISpxRecognizer> recognizer)
{
     m_recognizers.push_back(recognizer);
}

void CSpxSession::RemoveRecognizer(ISpxRecognizer* precognzier)
{
     m_recognizers.remove_if([&](std::weak_ptr<ISpxRecognizer>& item) {
         std::shared_ptr<ISpxRecognizer> recognizer = item.lock();
         return recognizer.get() == precognzier; 
     });
}

CSpxAsyncOp<std::shared_ptr<ISpxRecognitionResult>> CSpxSession::RecognizeAsync()
{
    SPX_DBG_TRACE_FUNCTION();

    std::packaged_task<std::shared_ptr<ISpxRecognitionResult>()> taskHack([=](){

        this->StartRecognizing();
        std::this_thread::sleep_for(std::chrono::milliseconds(1500));
        this->StopRecognizing();

        auto result = std::make_shared<CSpxRecognitionResult>();
        auto pISpxRecognitionResult = std::dynamic_pointer_cast<ISpxRecognitionResult>(result);

        return pISpxRecognitionResult;
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
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
}

void CSpxSession::StopRecognizing()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}


void CSpxAudioSession::SetFormat(WAVEFORMATEX* pformat)
{
    SPX_DBG_TRACE_VERBOSE_IF(pformat == nullptr, "%s - pformat == nullptr", __FUNCTION__);
    SPX_DBG_TRACE_VERBOSE_IF(pformat != nullptr, "%s\n  wFormatTag:      %s\n  nChannels:       %d\n  nSamplesPerSec:  %d\n  nAvgBytesPerSec: %d\n  nBlockAlign:     %d\n  wBitsPerSample:  %d\n  cbSize:          %d",
        __FUNCTION__,
        pformat->wFormatTag == WAVE_FORMAT_PCM ? "PCM" : "non-PCM",
        pformat->nChannels,
        pformat->nSamplesPerSec,
        pformat->nAvgBytesPerSec,
        pformat->nBlockAlign,
        pformat->wBitsPerSample,
        pformat->cbSize);
}

void CSpxAudioSession::ProcessAudio(AudioData_Type data, uint32_t size)
{
    SPX_DBG_TRACE_VERBOSE("%s - size=%d", __FUNCTION__, size);
}

void CSpxAudioSession::StartRecognizing()
{
    Base_Type::StartRecognizing();

    m_audioPump->StartPump(static_cast<ISpxAudioProcessor*>(this)->shared_from_this());
}

void CSpxAudioSession::StopRecognizing()
{
    m_audioPump->StopPump();

    Base_Type::StopRecognizing();
}

}; // CARBON_IMPL_NAMESPACE()
