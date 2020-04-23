//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
//
#include "com_microsoft_cognitiveservices_speech_audio_AudioStreamFormat.h"
#include "speechapi_c_audio_stream_format.h"
#include "jni_utils.h"

/*
 * Class:     com_microsoft_cognitiveservices_speech_audio_AudioStreamFormat
 * Method:    createFromDefaultInput
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_audio_AudioStreamFormat_createFromDefaultInput
  (JNIEnv *env, jclass cls, jobject streamFormatHandle)
{
    SPXAUDIOSTREAMFORMATHANDLE formatHandle = SPXHANDLE_INVALID;
    SPXHR hr = audio_stream_format_create_from_default_input(&formatHandle);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, streamFormatHandle, (jlong)formatHandle);
    }
    return (jlong)hr;
}
/*
 * Class:     com_microsoft_cognitiveservices_speech_audio_AudioStreamFormat
 * Method:    createFromWaveFormatPCM
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;JSS)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_audio_AudioStreamFormat_createFromWaveFormatPCM
  (JNIEnv *env, jclass cls, jobject streamFormatHandle, jlong samplesPerSecond, jshort bitsPerSample, jshort channels)
{
    SPXAUDIOSTREAMFORMATHANDLE formatHandle = SPXHANDLE_INVALID;
    SPXHR hr = audio_stream_format_create_from_waveformat_pcm(&formatHandle, (uint32_t)samplesPerSecond, (uint8_t)bitsPerSample, (uint8_t)channels);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, streamFormatHandle, (jlong)formatHandle);
    }
    return (jlong)hr;
}
/*
 * Class:     com_microsoft_cognitiveservices_speech_audio_AudioStreamFormat
 * Method:    createFromCompressedFormat
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;I)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_audio_AudioStreamFormat_createFromCompressedFormat
  (JNIEnv *env, jclass cls, jobject streamFormatHandle, jint compressedFormat)
{
    SPXAUDIOSTREAMFORMATHANDLE formatHandle = SPXHANDLE_INVALID;
    SPXHR hr = audio_stream_format_create_from_compressed_format(&formatHandle, (Audio_Stream_Container_Format)compressedFormat);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, streamFormatHandle, (jlong)formatHandle);
    }
    return (jlong)hr;
}
