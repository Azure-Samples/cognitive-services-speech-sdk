//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
// silk_codec_native.cpp: Implementation definitions for *silk_codec_c_interface_impl* C intarface
//

#define SPX_CONFIG_EXPORTAPIS
#include "stdafx.h"

#include "speechapi_c_ext_audiocompression.h"
#include "silk_codec_native.h"
#include "silk_codec_impl.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

SPX_EXTERN_C SPXDLL_EXPORT SPXCODECCTYPE codec_create(const char* /*codecid*/, void* /*context*/, SPX_CODEC_CLIENT_GET_PROPERTY /*property_read_func*/)
{
    silk_codec_c_interface_impl* codecImpl = reinterpret_cast<silk_codec_c_interface_impl*>(malloc(sizeof(silk_codec_c_interface_impl)));
    if (codecImpl != nullptr)
    {
        codecImpl->codecImpl = new CSilkCodecImpl();
        codecImpl->interface.init = codec_init;
        codecImpl->interface.get_format_type = codec_get_format_type;
        codecImpl->interface.encode = codec_encode;
        codecImpl->interface.flush = codec_flush;
        codecImpl->interface.endstream = codec_endstream;
        codecImpl->interface.destroy = codec_destroy;
    }

    return reinterpret_cast<SPXCODECCTYPE>(codecImpl);
}

SPXAPI_RESULTTYPE SPXAPI_CALLTYPE codec_init(
                                    SPXCODECCTYPE codec,
                                    uint32_t inputSamplesPerSecond,
                                    uint8_t inputBitsPerSample,
                                    uint8_t inputChannels,
                                    AUDIO_ENCODER_ONENCODEDDATA datacallback,
                                    void* pContext)
{
    auto codecImpl = (reinterpret_cast<silk_codec_c_interface_impl*>(codec))->codecImpl;
    return codecImpl->Init(
                inputSamplesPerSecond,
                inputBitsPerSample,
                inputChannels,
                [datacallback](const uint8_t* buffer, size_t dataSize, uint64_t duration_100ns, void* context)
                {
                    datacallback(buffer, dataSize, duration_100ns, context);
                },
                pContext);
}

SPXAPI_RESULTTYPE SPXAPI_CALLTYPE codec_get_format_type(SPXCODECCTYPE codec, char* buffer, uint64_t* buffersize)
{
    auto codecImpl = (reinterpret_cast<silk_codec_c_interface_impl*>(codec))->codecImpl;
    return codecImpl->GetFormatType(buffer, buffersize);
}

SPXAPI_RESULTTYPE SPXAPI_CALLTYPE codec_encode(SPXCODECCTYPE codec, const uint8_t* pBuffer, size_t bytesToWrite)
{
    auto codecImpl = (reinterpret_cast<silk_codec_c_interface_impl*>(codec))->codecImpl;
    return codecImpl->Encode(pBuffer, bytesToWrite);
}

SPXAPI_RESULTTYPE SPXAPI_CALLTYPE codec_flush(SPXCODECCTYPE codec)
{
    auto codecImpl = (reinterpret_cast<silk_codec_c_interface_impl*>(codec))->codecImpl;
    return codecImpl->Flush();
}

SPXAPI_RESULTTYPE SPXAPI_CALLTYPE codec_endstream(SPXCODECCTYPE codec)
{
    auto codecImpl = (reinterpret_cast<silk_codec_c_interface_impl*>(codec))->codecImpl;
    return codecImpl->EndStream();
}

SPXAPI_RESULTTYPE SPXAPI_CALLTYPE codec_destroy(SPXCODECCTYPE codec)
{
    silk_codec_c_interface_impl* codecImpl = reinterpret_cast<silk_codec_c_interface_impl*>(codec);

    // Destroy the impl object as well.
    delete codecImpl->codecImpl;
    free(codecImpl);

    return SPX_NOERROR;
}


}}}}
