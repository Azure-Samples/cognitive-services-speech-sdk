//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
// kws_engine_adapter.cpp: Implementation definitions for CSpxSdkKwsEngineAdapter C++ class
//

#include "stdafx.h"
#include "kws_engine.h"
#include "service_helpers.h"
#include "file_utils.h"

// private kws artifact headers
#include <SpeechAPI.h>

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

    // needs to be atomic so the compiler does
    // not cache the value.
    std::atomic<bool> m_keywordDetectedInProgress;
    std::atomic<bool> m_keywordDetectedEnabled;
    std::atomic<bool> m_keywordDetected;
    std::atomic<bool> m_streamClosed;

    size_t       m_startSampleOffsetInBytes;
    size_t       m_endSampleOffsetInBytes;
    const char*  m_detectionBuffer;

    double       m_lastConfidence;
    const char  *m_lastKeyword;

    SpxWAVEFORMATEX_Type m_format;
    uint64_t m_cbAudioProcessed;

    void InitFormat(const SPXWAVEFORMATEX* pformat)
    {
        SPX_IFTRUE_THROW_HR(HasFormat(), SPXERR_ALREADY_INITIALIZED);

        auto sizeOfFormat = sizeof(SPXWAVEFORMATEX) + pformat->cbSize;
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


CSpxSdkKwsEngineAdapter::CSpxSdkKwsEngineAdapter() :
    p_impl(new CSpxSdkKwsEngineAdapter::Impl())
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    p_impl->m_speechHandle = (SPEECH_HANDLE)0;

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

    // note: we assume we can call this several times without side effects.
    static std::once_flag flag1;
    std::call_once(flag1, []() {
        SPEECH_RESULT ret = keyword_spotter_initialize();
        SPX_DBG_TRACE_ERROR_IF(ret < 0, "keyword_spotter_initialize FAILED: status %x\n\n", ret);
        SPX_IFTRUE_THROW_HR(ret < 0, SPXERR_INVALID_STATE);
    });

    // Init the kws
    p_impl->m_speechHandle = (SPEECH_HANDLE)0;
    p_impl->m_cbAudioProcessed = 0;
    p_impl->m_keywordDetected = false;
    p_impl->m_streamClosed = false;
    p_impl->m_keywordDetectedEnabled = true;
    p_impl->m_keywordDetectedInProgress = false;

    // Get the parameters needed for initialization
    auto namedProperties = SpxQueryService<ISpxNamedProperties>(GetSite());
    auto kwsModelPath = namedProperties->GetStringValue("KWSModelPath", "");
    SPX_DBG_TRACE_INFO("Loading KWS model: %s", kwsModelPath.c_str());
    SPX_IFTRUE_THROW_HR(kwsModelPath.length() < 1, SPXERR_INVALID_ARG);

    // open the keyword spotter, raising errors if this fails.
    SPEECH_RESULT ret = keyword_spotter_open(&p_impl->m_speechHandle, kwsModelPath.c_str());
    SPX_DBG_TRACE_ERROR_IF(ret < 0, "keyword_spotter_open FAILED: handle %p, status %x\n\n", (void*)p_impl->m_speechHandle, ret);
    SPX_IFTRUE_THROW_HR(ret == (SPEECH_RESULT)KWS_MODEL_VERSION_MISMATCH, SPXERR_UNSUPPORTED_FORMAT); // as per discussion, this indicates an incompatible model version
    SPX_IFTRUE_THROW_HR(ret < 0, SPXERR_INVALID_ARG);
    SPX_IFTRUE_THROW_HR(!p_impl->m_speechHandle, SPXERR_UNINITIALIZED);

    // Initializing the callbacks of the KWS
    p_impl->m_speechCallbacks.size = sizeof(p_impl->m_speechCallbacks);
    p_impl->m_speechCallbacks.version = KWS_CALLBACKS_VERSION;
    p_impl->m_speechCallbacks.OnStateChanged = p_impl->OnKwsStateChanged;
    p_impl->m_speechCallbacks.OnStatus = p_impl->OnKwsStatusChanged;

    // setting up callbacks, raising errors if this fails.
    ret = keyword_spotter_setcallbacks(
        p_impl->m_speechHandle,
        &p_impl->m_speechCallbacks,
        (void*)p_impl);
    SPX_DBG_TRACE_ERROR_IF(ret < 0, "keyword_spotter_setcallbacks FAILED: status %x\n\n", ret);
    SPX_IFTRUE_THROW_HR(ret < 0, SPXERR_INVALID_STATE);

#ifdef _DEBUG
    // set up audio dump
    auto properties = SpxQueryService<ISpxNamedProperties>(GetSite());
    auto audioDumpDir = namedProperties->GetStringValue("CARBON-INTERNAL-KwsDumpAudioToDir");
    SPX_DBG_TRACE_VERBOSE("CARBON-INTERNAL-KwsDumpAudioToDir : %s", audioDumpDir.c_str());
    if (!audioDumpDir.empty())
    {
        m_audioDumpDir = audioDumpDir;
        auto audioLength = namedProperties->GetStringValue("CARBON-INTERNAL-KwsDumpAudioCircularFileLengthMs");
        SPX_DBG_TRACE_VERBOSE("CARBON-INTERNAL-KwsDumpAudioCircularFileLengthMs : %s", audioLength.c_str());
        auto fileCount = namedProperties->GetStringValue("CARBON-INTERNAL-KwsDumpAudioCircularFileCount");
        SPX_DBG_TRACE_VERBOSE("CARBON-INTERNAL-KwsDumpAudioCircularFileCount : %s", fileCount.c_str());
        if (!audioLength.empty())
        {
            m_audioDumpFileLengthMs = std::stoul(audioLength);
        }
        if (!fileCount.empty())
        {
            m_audioDumpMaxCount = std::stoul(fileCount);
        }
        InitAudioDumpFile();
    }
#endif
}

void CSpxSdkKwsEngineAdapter::Term()
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

#ifdef _DEBUG
    if (m_audioDumpFile)
    {
        SPX_DBG_TRACE_VERBOSE("Closing kws audio dump file");
        fflush(m_audioDumpFile);
        fclose(m_audioDumpFile);
        m_audioDumpFile = nullptr;
    }
#endif

    if (p_impl->m_speechHandle)
    {
        SPEECH_RESULT ret = keyword_spotter_close(p_impl->m_speechHandle);

        SPX_DBG_TRACE_WARNING_IF(ret < 0, "keyword_spotter_close FAILED: status %x\n\n", ret);
        SPX_IFTRUE_THROW_HR(ret < 0, SPXERR_INVALID_STATE);
    }

    p_impl->m_speechHandle = nullptr;
}

void CSpxSdkKwsEngineAdapter::SetFormat(const SPXWAVEFORMATEX* pformat)
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
    SPX_IFTRUE_THROW_HR(p_impl->m_keywordDetectedInProgress == true, SPXERR_INVALID_STATE);

    if (pformat != nullptr)
    {
        p_impl->InitFormat(pformat);
        p_impl->m_keywordDetectedEnabled = true;
    }
    else
    {
        p_impl->m_keywordDetectedEnabled = false;
        p_impl->TermFormat();

        SPEECH_RESULT ret = keyword_spotter_reset(p_impl->m_speechHandle);
        (void)ret; // release builds
        SPX_DBG_TRACE_WARNING_IF(ret < 0, "keyword_spotter_reset FAILED: status %x\n\n", ret);

        FireDoneProcessingAudioEvent();
    }

#if _DEBUG
    if (pformat != nullptr)
    {
        m_audioDumpBytesMax = uint32_t(((double)m_audioDumpFileLengthMs) * pformat->nAvgBytesPerSec / 1000);
    }
#endif
}

void CSpxSdkKwsEngineAdapter::ProcessAudio(const DataChunkPtr& audioChunk)
{

    SPX_IFTRUE_THROW_HR(!p_impl->HasFormat(), SPXERR_UNINITIALIZED);
    SPX_IFTRUE_THROW_HR(!p_impl->m_speechHandle, SPXERR_UNINITIALIZED);
    SPX_IFTRUE_THROW_HR(p_impl->m_keywordDetectedInProgress == true, SPXERR_INVALID_STATE);

    SPX_DBG_TRACE_INFO("data %p, size %d\n\n", (void*)audioChunk->data.get(), audioChunk->size);

    if (p_impl->m_keywordDetectedEnabled == true)
    {
        p_impl->m_keywordDetectedInProgress = true;
        SPEECH_RESULT ret = keyword_spotter_write(p_impl->m_speechHandle, audioChunk->data.get(), audioChunk->size);
        p_impl->m_keywordDetectedInProgress = false;

        SPX_DBG_TRACE_ERROR_IF(ret < 0, "keyword_spotter_write FAILED: status %x\n\n", ret);
        SPX_IFTRUE_THROW_HR(ret < 0, SPXERR_INVALID_STATE);

        if (p_impl->m_keywordDetected)
        {
            p_impl->m_keywordDetectedEnabled = false;
            ret = keyword_spotter_reset(p_impl->m_speechHandle);
            SPX_DBG_TRACE_WARNING_IF(ret < 0, "keyword_spotter_reset FAILED: status %x\n\n", ret);

            FireKeywordDetectedEvent(audioChunk);
        }

#ifdef _DEBUG
        if (m_audioDumpFile)
        {
            fwrite(audioChunk->data.get(), 1, audioChunk->size, m_audioDumpFile);
            m_audioDumpBytesWritten += audioChunk->size;
            if (m_audioDumpBytesMax > 0 && m_audioDumpBytesWritten > m_audioDumpBytesMax)
            {
                CycleAudioDumpFile();
            }
        }
#endif

        p_impl->m_cbAudioProcessed += audioChunk->size;
    }
    else
    {
        SPX_DBG_ASSERT_WITH_MESSAGE(!audioChunk->data, "called with data but not active");
    }
}


void CSpxSdkKwsEngineAdapter::FireDoneProcessingAudioEvent()
{
    SPX_DBG_ASSERT(GetSite());
    GetSite()->AdapterCompletedSetFormatStop(this);
}

void CSpxSdkKwsEngineAdapter::FireKeywordDetectedEvent(const DataChunkPtr& audioChunk)
{
    SPX_DBG_TRACE_FUNCTION();

    // get the data we need to fire the event to our site (including a copy of the audio data)
    auto size = p_impl->m_endSampleOffsetInBytes - p_impl->m_startSampleOffsetInBytes;
    SPX_DBG_TRACE_WARNING_IF(size < (size_t)1, "size illegal?");

    auto audioData = SpxAllocSharedAudioBuffer(size);
    memcpy(audioData.get(), p_impl->m_detectionBuffer + p_impl->m_startSampleOffsetInBytes, size);

    // now ... tell our site we found it ...
    auto site = GetSite();

    // convert byte offsets to ticks of 100nsec
    auto confidence = p_impl->m_lastConfidence;
    std::string keyword = p_impl->m_lastKeyword ? p_impl->m_lastKeyword : "";

    // Remove special characters from the keyword
    // TODO: move this logic to the actual keyword detector
    replace_if(keyword.begin(), keyword.end(), ::ispunct, ' ');

    auto ticksPerSecond = 1000 * 1000 * 10; // 1000 == to_msec, 1000 == to_usec, 10 == to_100nsec

    auto offset = (p_impl->m_cbAudioProcessed + p_impl->m_startSampleOffsetInBytes);
    offset = (ticksPerSecond * offset) / p_impl->m_format->nAvgBytesPerSec;

    auto duration = (uint64_t)(p_impl->m_endSampleOffsetInBytes - p_impl->m_startSampleOffsetInBytes);
    duration = (ticksPerSecond * duration) / p_impl->m_format->nAvgBytesPerSec;

    // reset the flag now that we have the data
    p_impl->m_keywordDetected = false;

    std::shared_ptr<ISpxAudioProcessor> keepAlive = SpxSharedPtrFromThis<ISpxAudioProcessor>(this);
    auto keywordAudio = std::make_shared<DataChunk>(audioData, (uint32_t)size, audioChunk->receivedTime);
    site->KeywordDetected(this, offset, duration, confidence, keyword, keywordAudio);
}

#ifdef _DEBUG
void CSpxSdkKwsEngineAdapter::InitAudioDumpFile()
{
    if (m_audioDumpDir.empty())
    {
        return;
    }
    m_audioDumpInstCount %= m_audioDumpMaxCount;
    std::string fileName = PAL::AppendPath(m_audioDumpDir, "continuous_kws_" + std::to_string(m_audioDumpInstCount) + ".wav");
    m_audioDumpInstCount++;
    m_audioDumpBytesWritten = 0;
    auto err = PAL::fopen_s(&m_audioDumpFile, fileName.c_str(), "wb");
    if (err != 0)
    {
        SPX_DBG_TRACE_WARNING("%s: (0x%8p) FAILED to open audio dump file %s for write (Error=%d)", __FUNCTION__, (void*)this, fileName.c_str(), err);
    }
    else
    {
        SPX_DBG_TRACE_VERBOSE("%s: (0x%8p) Writing audio dump to file %s", __FUNCTION__, (void*)this, fileName.c_str());
    }
}

void CSpxSdkKwsEngineAdapter::CycleAudioDumpFile()
{
    if (m_audioDumpFile)
    {
        fflush(m_audioDumpFile);
        fclose(m_audioDumpFile);
    }
    InitAudioDumpFile();
}
#endif

// ***************************************************************************************
// *** Callback implementations
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
        //
        // Note: we assume that this code is called from within keyword_spotter_write().
        //
        SPX_DBG_ASSERT_WITH_MESSAGE(pimpl->m_keywordDetectedInProgress == true,
                                    "must be called from keyword_spotter_write()");

        SPX_DBG_TRACE_WARNING_IF(pStatus->endSampleOffset > 0 || pStatus->startSampleOffset > 0,
                                 "offsets are reported positive");

        //
        // Note: we assume that the audio within the offsets start from the detected keyword
        //       and include any audio that comes after the keyword has ended.
        //       In particular, we assume that the buffer contains ALL audio starting from
        //       the keyword.
        //
        pimpl->m_detectionBuffer = (char *)pStatus->detectionBuffer;
        pimpl->m_startSampleOffsetInBytes = 0 - (pStatus->endSampleOffset * sizeof(uint16_t));
        pimpl->m_endSampleOffsetInBytes = 0 - (pStatus->startSampleOffset * sizeof(uint16_t));

        // validating that offsets are within boundaries.
        SPX_DBG_TRACE_WARNING_IF(pimpl->m_startSampleOffsetInBytes > pimpl->m_endSampleOffsetInBytes,
                                 "end reported as smaller than start?!");
        SPX_DBG_TRACE_WARNING_IF(pimpl->m_startSampleOffsetInBytes >= pStatus->detectionBufferSize * sizeof(uint16_t),
                                 "m_startSampleOffsetInBytes offset out of bounds?!");
        SPX_DBG_TRACE_WARNING_IF(pimpl->m_endSampleOffsetInBytes >= pStatus->detectionBufferSize * sizeof(uint16_t),
                                 "m_endSampleOffsetInBytes offset out of bounds?!");

        pimpl->m_lastConfidence = pStatus->confidence;
        pimpl->m_lastKeyword = pStatus->keyword;

        // set the flag
        pimpl->m_keywordDetected = true;
    }
}

}}}}
