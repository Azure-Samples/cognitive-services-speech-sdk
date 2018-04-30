//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// sdk_kws_engine_adapter.cpp: Implementation definitions for CSpxSdkKwsEngineAdapter C++ class
//

#include "stdafx.h"
#include "sdk_kws_engine_adapter.h"
#include "service_helpers.h"

// private kws artifact headers
extern "C"
{
#include <speechapi.h>
}


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


// ***************************************************************************************
// *** Implementation specific details hidden from public class interface
// ***************************************************************************************

class CSpxSdkKwsEngineAdapter::Impl
{
public:

    friend CSpxSdkKwsEngineAdapter;

    // speech kws api
    SPEECH_HANDLE m_speechHandle;
    KWS_CALLBACKS m_speechCallbacks;

    // needs to be volatile so the compiler does
    // not cache the value.
    volatile bool m_keywordDetected;
    volatile bool m_streamClosed;

    int          m_startSampleOffsetInBytes;
    unsigned int m_sampleLengthInBytes;
    const char*  m_detectionBuffer;

    SpxWAVEFORMATEX_Type m_format;
    uint64_t m_cbAudioProcessed;

    Impl()
    {
        SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    }

    ~Impl()
    {
        SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    }

    void InitFormat(WAVEFORMATEX* pformat)
    {
        SPX_IFTRUE_THROW_HR(HasFormat(), SPXERR_ALREADY_INITIALIZED);

        auto sizeOfFormat = sizeof(WAVEFORMATEX) + pformat->cbSize;
        m_format = SpxAllocWAVEFORMATEX(sizeOfFormat);
        memcpy(m_format.get(), pformat, sizeOfFormat);

        m_cbAudioProcessed = 0;
        m_keywordDetected = false;
    }

    bool HasFormat()
    {
        return m_format.get() != nullptr;
    }

    void TermFormat()
    {
        SPX_DBG_TRACE_FUNCTION();
        m_format = nullptr;
    }

private:
    static void OnKwsStateChanged(void* pContext, SPEECH_STATE state);
    static void OnKwsStatusChanged(void* pContext, const KWS_STATUS* pStatus);
};


CSpxSdkKwsEngineAdapter::CSpxSdkKwsEngineAdapter()
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    p_impl = new CSpxSdkKwsEngineAdapter::Impl();
    p_impl->m_cbAudioProcessed = 0;

    p_impl->m_keywordDetected = false;
    p_impl->m_streamClosed = false;
}

CSpxSdkKwsEngineAdapter::~CSpxSdkKwsEngineAdapter()
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    delete p_impl;
}

void CSpxSdkKwsEngineAdapter::Init()
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    SPX_IFTRUE_THROW_HR(GetSite() == nullptr, SPXERR_UNINITIALIZED);

    // TODO: can we call this several times?
    keyword_spotter_initialize();


    // Get the parameters needed for initialization
    auto namedProperties = SpxQueryService<ISpxNamedProperties>(GetSite());

    auto kwsModelPathW = namedProperties->GetStringValue(L"KWSModelPath", L"/data/carbon/heycortana_en-US.table");
    auto kwsModelPath = std::string(kwsModelPathW.begin(), kwsModelPathW.end());

    // TODO: handle bad return values
    //       missing definitions in header file.
    keyword_spotter_open(&p_impl->m_speechHandle, kwsModelPath.c_str());

    // Initializing the callbacks of the KWS
    p_impl->m_speechCallbacks.size = sizeof(p_impl->m_speechCallbacks);
    p_impl->m_speechCallbacks.version = KWS_CALLBACKS_VERSION;
    p_impl->m_speechCallbacks.OnStateChanged = p_impl->OnKwsStateChanged;
    p_impl->m_speechCallbacks.OnStatus = p_impl->OnKwsStatusChanged;

    // TODO: handle bad return values
    //       missing definitions in header file.
    keyword_spotter_setcallbacks(
        p_impl->m_speechHandle,
        &p_impl->m_speechCallbacks,
        (void*)p_impl);
}

void CSpxSdkKwsEngineAdapter::Term()
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    keyword_spotter_close(p_impl->m_speechHandle);
}

void CSpxSdkKwsEngineAdapter::SetFormat(WAVEFORMATEX* pformat)
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    SPX_DBG_TRACE_VERBOSE_IF(pformat == nullptr, "%s - pformat == nullptr", __FUNCTION__);
    SPX_DBG_TRACE_VERBOSE_IF(pformat != nullptr, "%s\n  wFormatTag:      %s\n  nChannels:       %d\n  nSamplesPerSec:  %d\n  nAvgBytesPerSec: %d\n  nBlockAlign:     %d\n  wBitsPerSample:  %d\n  cbSize:          %d",
        __FUNCTION__,
        pformat->wFormatTag == WAVE_FORMAT_PCM ? "PCM" : std::to_string(pformat->wFormatTag).c_str(),
        pformat->nChannels,
        pformat->nSamplesPerSec,
        pformat->nAvgBytesPerSec,
        pformat->nBlockAlign,
        pformat->wBitsPerSample,
        pformat->cbSize);

    SPX_IFTRUE_THROW_HR(pformat != nullptr && p_impl->HasFormat(), SPXERR_ALREADY_INITIALIZED);

    if (pformat != nullptr)
    {
        p_impl->InitFormat(pformat);
    }
    else
    {
        p_impl->TermFormat();
        FireDoneProcessingAudioEvent();
        keyword_spotter_reset(p_impl->m_speechHandle);
    }
}

void CSpxSdkKwsEngineAdapter::ProcessAudio(AudioData_Type data, uint32_t size)
{
    SPX_IFTRUE_THROW_HR(!p_impl->HasFormat(), SPXERR_UNINITIALIZED);

    keyword_spotter_write(p_impl->m_speechHandle, data.get(), size);

    p_impl->m_cbAudioProcessed += size;
    if (p_impl->m_keywordDetected)
    {
        FireKeywordDetectedEvent();
    }
}


void CSpxSdkKwsEngineAdapter::FireDoneProcessingAudioEvent()
{
    SPX_DBG_ASSERT(GetSite());
    GetSite()->AdapterCompletedSetFormatStop(this);
}

void CSpxSdkKwsEngineAdapter::FireKeywordDetectedEvent()
{
    SPX_DBG_TRACE_FUNCTION();

    // get the data we need to fire the event to our site (including a copy of the audio data)
    auto offset = p_impl->m_cbAudioProcessed;
    auto size = p_impl->m_sampleLengthInBytes;

    auto audioData = SpxAllocSharedAudioBuffer(size);
    memcpy(audioData.get(), p_impl->m_detectionBuffer, size);

    // reset the flag now that we have the data
    p_impl->m_keywordDetected = false;

    // now ... tell our site we found it ...
    auto site = GetSite();

    std::shared_ptr<ISpxAudioProcessor> keepAlive = SpxSharedPtrFromThis<ISpxAudioProcessor>(this);
    std::packaged_task<void()> task([=](){

        auto keepAliveCopy = keepAlive;
        site->KeywordDetected(this, offset, size, audioData);
    });

    auto taskFuture = task.get_future();
    std::thread taskThread(std::move(task));
    taskThread.detach();
}

// ***************************************************************************************
// *** Implementation class details
// ***************************************************************************************

/*static */void CSpxSdkKwsEngineAdapter::Impl::OnKwsStateChanged(void* pContext, SPEECH_STATE state)
{
    CSpxSdkKwsEngineAdapter::Impl* pimpl = (CSpxSdkKwsEngineAdapter::Impl*)pContext;
    if (state == SPEECH_STATE_STOPPED)
    {
        pimpl->m_streamClosed = true;
    }
}

/*static */void CSpxSdkKwsEngineAdapter::Impl::OnKwsStatusChanged(void* pContext, const KWS_STATUS* pStatus)
{
    CSpxSdkKwsEngineAdapter::Impl* pimpl = (CSpxSdkKwsEngineAdapter::Impl*)pContext;
    if (pStatus->result == KEYWORD_DETECTED && !pimpl->m_keywordDetected)
    {
        pimpl->m_detectionBuffer = (char *)pStatus->detectionBuffer;
        pimpl->m_startSampleOffsetInBytes = pStatus->startSampleOffset * sizeof(uint16_t);
        pimpl->m_sampleLengthInBytes = (unsigned int)pStatus->detectionBufferSize;

        // set the flag
        pimpl->m_keywordDetected = true;
    }
}


} } } } // Microsoft::CognitiveServices::Speech::Impl
