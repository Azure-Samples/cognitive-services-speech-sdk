//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// internal_audio_codec_adapter.h: Implementation declarations for CSpxInternalAudioCodecAdapter C++ class
//

#pragma once
#include "spxcore_common.h"
#include "ispxinterfaces.h"
#include "interface_helpers.h"
#include "speechapi_cxx_audio_stream_format.h"
#include "speechapi_c_ext_audiocompression.h"

using namespace Microsoft::CognitiveServices::Speech::Audio;

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class CSpxInternalAudioCodecAdapter :
    public ISpxObjectWithSiteInitImpl<ISpxGenericSite>,
    public ISpxInternalAudioCodecAdapter
{
public:

    CSpxInternalAudioCodecAdapter();
    virtual ~CSpxInternalAudioCodecAdapter();

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectWithSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxInternalAudioCodecAdapter)
    SPX_INTERFACE_MAP_END()

    // --- ISpxInternalAudioCodecAdapter
    SPXHR Load(const std::string& modulename, const std::string& codec, SPXCompressedDataCallback dataCallback) override;
    void InitCodec(const SPXWAVEFORMATEX* inputFormat) override;
    std::string GetContentType() override;
    void Encode(const uint8_t* buffer, size_t bufferSize) override;
    void Flush() override;
    void CloseEncodingStream() override;

    // --- ISpxObject
    void Term() override;

private:
    static SPXAPI_RESULTTYPE SPXAPI_CALLTYPE codec_client_get_property(const char* propertyId, char* buffer, uint64_t* buffersize, void* context);
    static void SPXAPI_CALLTYPE AudioEncoderOnEncodedDataCallback(const uint8_t* pBuffer, size_t bytesToWrite, uint64_t duration_100nanos, void* pContext);
    void AudioEncoderOnEncodedData(const uint8_t* pBuffer, size_t bytesToWrite, uint64_t duration_100nanos);

    CSpxInternalAudioCodecAdapter(const CSpxInternalAudioCodecAdapter&) = delete;
    CSpxInternalAudioCodecAdapter(const CSpxInternalAudioCodecAdapter&&) = delete;

    CSpxInternalAudioCodecAdapter& operator=(const CSpxInternalAudioCodecAdapter&) = delete;

    SPXHR GetProperty(const char* propertyId, char* buffer, uint64_t* buffersize);

    std::shared_ptr<SPXWAVEFORMATEX> m_format;
    SPXCODECCTYPE m_codec;
    SPXCompressedDataCallback m_dataCallback;
};

} } } } // Microsoft::CognitiveServices::Speech::Impl
