//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// internal_audio_codec_adapter.cpp: Implementation definitions for CSpxInternalAudioCodecAdapter C++ class
//


#include "stdafx.h"
#include "dynamic_module.h"
#include "internal_audio_codec_adapter.h"
#include "speechapi_c_ext_audiocompression.h"
#include <service_helpers.h>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

CSpxInternalAudioCodecAdapter::CSpxInternalAudioCodecAdapter()
    : m_codec(nullptr),
      m_dataCallback(nullptr)
{

}

CSpxInternalAudioCodecAdapter::~CSpxInternalAudioCodecAdapter()
{
    Term();
}

void CSpxInternalAudioCodecAdapter::Term()
{
    if (m_codec != nullptr)
    {
        m_codec->destroy(m_codec);
        m_codec = nullptr;
    }

    m_dataCallback = nullptr;
}

SPXHR CSpxInternalAudioCodecAdapter::GetProperty(const char* propertyId, char* buffer, uint64_t* buffersize)
{
    auto properties = SpxQueryService<ISpxNamedProperties>(GetSite());
    auto propValue = properties->GetStringValue(propertyId);

    if (buffer == nullptr)
    {
        *buffersize = (uint64_t)propValue.length() + 1;
        return SPXERR_BUFFER_TOO_SMALL;
    }
    else
    {
        auto maxSize = *buffersize;
        *buffersize = (uint64_t)propValue.length() + 1;

        PAL::strcpy(buffer, (size_t)maxSize, propValue.c_str(), (size_t)*buffersize, false);
        if (maxSize <= *buffersize)
        {
            return SPXERR_BUFFER_TOO_SMALL;
        }
    }

    return SPX_NOERROR;
}

SPXHR CSpxInternalAudioCodecAdapter::Load(const std::string& modulename, const std::string& codecname, SPXCompressedDataCallback dataCallback)
{
    SPX_RETURN_HR_IF(SPXERR_ALREADY_INITIALIZED, m_codec != nullptr);

    auto dynamicModule = CSpxDynamicModule::Get(modulename);
    SPX_RETURN_HR_IF(SPXERR_NOT_FOUND, dynamicModule == nullptr);

    auto codecCreateFunc = (PCODEC_CREATE_FUNC)dynamicModule->GetModuleProcAddress(CODECCREATEEXPORTNAME);
    SPX_RETURN_HR_IF(SPXERR_NOT_FOUND, codecCreateFunc == nullptr);

    m_codec = codecCreateFunc(codecname.c_str(), this, CSpxInternalAudioCodecAdapter::codec_client_get_property);
    SPX_RETURN_HR_IF(SPXERR_COMPRESS_AUDIO_CODEC_INITIFAILED, m_codec == nullptr);

    m_dataCallback = dataCallback;

    return SPX_NOERROR;
}

void CSpxInternalAudioCodecAdapter::Encode(const uint8_t* buffer, size_t bufferSize)
{
    SPX_THROW_HR_IF(SPXERR_UNINITIALIZED, m_codec == nullptr);

    SPX_THROW_ON_FAIL(m_codec->encode(m_codec, buffer, bufferSize));
}

void CSpxInternalAudioCodecAdapter::Flush()
{
    SPX_THROW_ON_FAIL(m_codec->flush(m_codec));
}

void CSpxInternalAudioCodecAdapter::CloseEncodingStream()
{
    SPX_THROW_ON_FAIL(m_codec->endstream(m_codec));
}

void CSpxInternalAudioCodecAdapter::InitCodec(const SPXWAVEFORMATEX* inputFormat)
{

    SPX_THROW_ON_FAIL(m_codec->init(
                                m_codec,
                                inputFormat->nSamplesPerSec,
                                (uint8_t)inputFormat->wBitsPerSample,
                                (uint8_t)inputFormat->nChannels,
                                CSpxInternalAudioCodecAdapter::AudioEncoderOnEncodedDataCallback,
                                this));
}

std::string CSpxInternalAudioCodecAdapter::GetContentType()
{
    uint64_t bufferSize = 0;

    m_codec->get_format_type(m_codec, nullptr, &bufferSize);
    std::unique_ptr<char[]> buffer { new char[(size_t)bufferSize]};
    m_codec->get_format_type(m_codec, buffer.get(), &bufferSize);
    std::string contentType = buffer.get();
    return contentType;
}

void CSpxInternalAudioCodecAdapter::AudioEncoderOnEncodedData(const uint8_t* pBuffer, size_t bytesToWrite, uint64_t duration_100nanos)
{
    // The duration is not needed right now but we should keep it as part of the information as it will help when refactoring the audio pipeline.
    UNUSED(duration_100nanos);
    if (m_dataCallback != nullptr)
    {
        m_dataCallback(pBuffer, bytesToWrite);
    }
}


/*static*/ SPXAPI_RESULTTYPE CSpxInternalAudioCodecAdapter::codec_client_get_property(
    const char* propertyId,
    char* buffer,
    uint64_t* buffersize,
    void* context)
{
    SPX_IFTRUE_RETURN_HR(context == nullptr, SPXERR_INVALID_ARG);
    SPX_IFTRUE_RETURN_HR(propertyId == nullptr, SPXERR_INVALID_ARG);
    SPX_IFTRUE_RETURN_HR(buffersize == nullptr, SPXERR_INVALID_ARG);
    SPX_IFTRUE_RETURN_HR(buffer == nullptr || *buffersize > 0, SPXERR_INVALID_ARG);

    auto codecAdapter = (CSpxInternalAudioCodecAdapter*)context;
    return codecAdapter->GetProperty(propertyId, buffer, buffersize);
}

/*static*/ void SPXAPI_CALLTYPE CSpxInternalAudioCodecAdapter::AudioEncoderOnEncodedDataCallback(const uint8_t* pBuffer, size_t bytesToWrite, uint64_t duration_100nanos, void* pContext)
{
    auto thisAudioCodecAdapter = (CSpxInternalAudioCodecAdapter*)pContext;
    thisAudioCodecAdapter->AudioEncoderOnEncodedData(pBuffer, bytesToWrite, duration_100nanos);
}



}}}}
