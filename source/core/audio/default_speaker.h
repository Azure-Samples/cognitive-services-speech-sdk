//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// default_speaker.h: Implementation declarations for CSpxDefaultSpeaker C++ class
//

#pragma once
#include "audio_sys.h"
#include "null_audio_output.h"
#include "pull_audio_output_stream.h"
#include "spxcore_common.h"
#include "interface_helpers.h"
#include "service_helpers.h"
#include "property_bag_impl.h"

#ifdef WIN32
#include "windows\com_init_and_uninit.h"
#undef min
#endif

// Currently audio output device is only available on Windows Desktop / Linux Desktop / Android
#if defined(_MSC_VER) // Windows
#if !defined(WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP) // Desktop
#define AUDIO_OUTPUT_DEVICE_AVAILABLE
#endif
#elif defined(__linux__) && !defined(ANDROID) && !defined(__ANDROID__) // Linux
#define AUDIO_OUTPUT_DEVICE_AVAILABLE
#endif


#define CHUNK_SIZE 32000

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


class CSpxDefaultSpeaker :
    public ISpxObjectWithSiteInitImpl<ISpxGenericSite>,
    public ISpxAudioRender,
    public CSpxNullAudioOutput
{
public:

    CSpxDefaultSpeaker();
    ~CSpxDefaultSpeaker();

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectWithSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioRender)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioOutput)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioStream)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioStreamInitFormat)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioOutputFormat)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioOutputInitFormat)
    SPX_INTERFACE_MAP_END()

    // --- ISpxObjectInit ---

    void Init() override;
    void Term() override;

    // --- ISpxAudioRender ---

    void StartPlayback() override;
    void PausePlayback() override;
    void StopPlayback() override;

    // --- ISpxAudioOutput ---

    uint32_t Write(uint8_t* buffer, uint32_t size) override;
    void WaitUntilDone() override;
    void Close() override;

    // --- ISpxAudioStream ---

    uint16_t GetFormat(SPXWAVEFORMATEX* pformat, uint16_t cbFormat) override;

    // --- ISpxAudioStreamInitFormat ---

    void SetFormat(SPXWAVEFORMATEX* pformat) override;


private:

    void InitializeAudio();

    static int PlayAudioReadCallback(void* pContext, uint8_t* pBuffer, uint32_t size);
    static void AudioCompleteCallback(void* pContext);
    static void BufferUnderRunCallback(void* pContext);

#ifdef AUDIO_OUTPUT_DEVICE_AVAILABLE
    AUDIO_SETTINGS_HANDLE m_hsetting = nullptr;
    AUDIO_SYS_HANDLE m_haudio = nullptr;
    std::atomic<bool> m_playing { false };
    std::mutex m_mutex;
    std::condition_variable m_cv;
#endif

    bool m_audioInitialized = false;

    std::shared_ptr<AUDIO_WAVEFORMAT> m_audioFormat;

    std::shared_ptr<CSpxPullAudioOutputStream> m_audioStream; // Use this to buffer the audio data

#ifdef WIN32
    ComInitAndUnInit m_com;
#endif
};


} } } } // Microsoft::CognitiveServices::Speech::Impl
