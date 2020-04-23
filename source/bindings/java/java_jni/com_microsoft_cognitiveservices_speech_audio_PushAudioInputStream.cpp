//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
//
#include "com_microsoft_cognitiveservices_speech_audio_PushAudioInputStream.h"
#include "speechapi_c_audio_stream.h"
#include "speechapi_c_audio_stream_format.h"
#include "jni_utils.h"

/*
 * Class:     com_microsoft_cognitiveservices_speech_audio_PushAudioInputStream
 * Method:    createPushAudioInputStream
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_audio_PushAudioInputStream_createPushAudioInputStream
  (JNIEnv *env, jclass cls, jobject streamHandle, jobject streamFormatHandle)
{
    SPXAUDIOSTREAMHANDLE audioStreamHandle = SPXHANDLE_INVALID;
    SPXAUDIOSTREAMFORMATHANDLE defaultFormat = SPXHANDLE_INVALID;
    jlong formatHandle = GetObjectHandle(env, streamFormatHandle);
    SPXHR hr = SPX_NOERROR;
    if (!formatHandle)
    {
        hr = audio_stream_format_create_from_default_input(&defaultFormat);
    }
    if (SPX_SUCCEEDED(hr))
    {
        hr = audio_stream_create_push_audio_input_stream(&audioStreamHandle, formatHandle ? (SPXAUDIOSTREAMFORMATHANDLE)formatHandle : defaultFormat);
    }
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, streamHandle, (jlong)audioStreamHandle);
    }
    if (audio_stream_format_is_handle_valid(defaultFormat))
    {
        audio_stream_format_release(defaultFormat);
    }
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_audio_PushAudioInputStream
 * Method:    pushAudioInputStreamWrite
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;[BI)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_audio_PushAudioInputStream_pushAudioInputStreamWrite
  (JNIEnv *env, jobject obj, jobject audioStreamHandle, jbyteArray byteArray, jint bufferSize)
{
    jlong streamHandle = GetObjectHandle(env, audioStreamHandle);
    uint8_t* buffer = (uint8_t *)env->GetByteArrayElements(byteArray, 0);
    uint32_t size = (uint32_t)env->GetArrayLength(byteArray);
    SPXHR hr = push_audio_input_stream_write((SPXAUDIOSTREAMHANDLE)streamHandle, buffer, size);
    if (byteArray)
    {
        env->ReleaseByteArrayElements(byteArray, (jbyte *)buffer, 0);
    }
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_audio_PushAudioInputStream
 * Method:    pushAudioInputStreamClose
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_audio_PushAudioInputStream_pushAudioInputStreamClose
  (JNIEnv *env, jobject obj, jobject audioStreamHandle)
{
    jlong streamHandle = GetObjectHandle(env, audioStreamHandle);
    SPXHR hr = push_audio_input_stream_close((SPXAUDIOSTREAMHANDLE) streamHandle);
    return (jlong)hr;
}
