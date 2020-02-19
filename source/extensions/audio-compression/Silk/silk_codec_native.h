//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
// silk_codec_native.h: Implementation declarations for *silk_codec_c_interface_impl* C intarface
//

#pragma once

#ifdef _DEBUG
#define SPX_CONFIG_DBG_TRACE_ALL 1
#define SPX_CONFIG_TRACE_ALL 1
#else
#define SPX_CONFIG_TRACE_ALL 1
#endif

#include "spxcore_common.h"
#include <speechapi_c_ext_audiocompression.h>


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

// forward declaration of the actional object that handles the actual code.
class CSilkCodecImpl;

//
// functions implementing the C codec interface for codec_c_interface struct.
//
SPXAPI_RESULTTYPE SPXAPI_CALLTYPE codec_init(
                                    SPXCODECCTYPE codec,
                                    uint32_t inputSamplesPerSecond,
                                    uint8_t inputBitsPerSample,
                                    uint8_t inputChannels,
                                    AUDIO_ENCODER_ONENCODEDDATA datacallback, void* pContext);

SPXAPI_RESULTTYPE SPXAPI_CALLTYPE codec_get_format_type(SPXCODECCTYPE codec, char* buffer, uint64_t* buffersize);
SPXAPI_RESULTTYPE SPXAPI_CALLTYPE codec_encode(SPXCODECCTYPE codec, const uint8_t* pBuffer, size_t bytesToWrite);
SPXAPI_RESULTTYPE SPXAPI_CALLTYPE codec_flush(SPXCODECCTYPE codec);
SPXAPI_RESULTTYPE SPXAPI_CALLTYPE codec_endstream(SPXCODECCTYPE codec);
SPXAPI_RESULTTYPE SPXAPI_CALLTYPE codec_destroy(SPXCODECCTYPE codec);


struct silk_codec_c_interface_impl
{
    codec_c_interface interface;
    CSilkCodecImpl* codecImpl;
};

}}}}
