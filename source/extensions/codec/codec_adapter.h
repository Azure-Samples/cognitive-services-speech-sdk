//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// codec_adapter.h: Implementation declarations for CSpxCodecAdapter C++ class
//

#pragma once
#include "spxcore_common.h"
#include "ispxinterfaces.h"
#include "interface_helpers.h"
#include "base_gstreamer.h"
#include "speechapi_cxx_audio_stream_format.h"

using namespace Microsoft::CognitiveServices::Speech::Audio;

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class CSpxCodecAdapter :
    public ISpxGenericSite,
    public ISpxAudioStreamInitFormat,
    public ISpxAudioStream,
    public ISpxAudioStreamReader,
    public ISpxAudioStreamReaderInitCallbacks
{
public:

    CSpxCodecAdapter();
    virtual ~CSpxCodecAdapter();

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioStreamInitFormat)
        SPX_INTERFACE_MAP_ENTRY(ISpxGenericSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioStream)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioStreamReader)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioStreamReaderInitCallbacks)
    SPX_INTERFACE_MAP_END()

    // --- ISpxAudioStreamReader ---
    uint16_t GetFormat(SPXWAVEFORMATEX* format, uint16_t formatSize) override;
    uint32_t Read(uint8_t* pbuffer, uint32_t cbBuffer) override;
    SPXSTRING GetProperty(PropertyId propertyId) override;
    void Close() override;
    // --- ISpxAudioStreamInitFormat ---
    void SetFormat(SPXWAVEFORMATEX* format) override;
    // --- ISpxAudioStreamReaderInitCallbacks ---
    void SetCallbacks(ISpxAudioStreamReaderInitCallbacks::ReadCallbackFunction_Type readCallback, ISpxAudioStreamReaderInitCallbacks::CloseCallbackFunction_Type closeCallback) override;
    void SetPropertyCallback2(GetPropertyCallbackFunction_Type2 getPropertyCallBack) override;

private:
    CSpxCodecAdapter(const CSpxCodecAdapter&) = delete;
    CSpxCodecAdapter(const CSpxCodecAdapter&&) = delete;

    CSpxCodecAdapter& operator=(const CSpxCodecAdapter&) = delete;
    void Open(AudioStreamContainerFormat containerFormat, uint16_t bitsPerSample, uint16_t numChannels, uint32_t sampleRate);
    std::shared_ptr<BaseGstreamer> m_gstObject = nullptr;
    std::shared_ptr<SPXWAVEFORMATEX> m_format;

    ReadCallbackFunction_Type m_readCallback;
    CloseCallbackFunction_Type m_closeCallback;
    GetPropertyCallbackFunction_Type2 m_getPropertyCallback;
    std::mutex m_mutex;
    std::shared_ptr<ISpxAudioStreamReader> m_reader;
    bool m_streamStarted = false;
};

} } } } // Microsoft::CognitiveServices::Speech::Impl
