//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
//
#include "com_microsoft_cognitiveservices_speech_audio_PullAudioOutputStream.h"
#include "speechapi_c_audio_stream.h"
#include "jni_utils.h"

/*
 * Class:     com_microsoft_cognitiveservices_speech_audio_PullAudioOutputStream
 * Method:    createPullAudioOutputStream
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/IntRef;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_audio_PullAudioOutputStream_createPullAudioOutputStream
  (JNIEnv *env, jclass cls, jobject audioStreamHandle)
{
    SPXAUDIOSTREAMHANDLE audioStream = SPXHANDLE_INVALID;
    SPXHR hr = audio_stream_create_pull_audio_output_stream(&audioStream);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, audioStreamHandle, (jlong)audioStream);
    }
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_audio_PullAudioOutputStream
 * Method:    pullAudioOutputStreamRead
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;[BLcom/microsoft/cognitiveservices/speech/util/IntRef;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_audio_PullAudioOutputStream_pullAudioOutputStreamRead
  (JNIEnv *env, jobject obj, jobject streamHandle, jbyteArray dataBuffer, jobject filledSize)
{
    jlong audioStream = GetObjectHandle(env, streamHandle);
    uint8_t* buffer = (uint8_t*)env->GetByteArrayElements(dataBuffer, 0);
    uint32_t bufferSize = (uint32_t)env->GetArrayLength(dataBuffer);
    uint32_t filled = 0;
    SPXHR hr = pull_audio_output_stream_read((SPXAUDIOSTREAMHANDLE)audioStream, buffer, bufferSize, &filled);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, filledSize, (jlong)filled);
    }
    if (dataBuffer)
    {
        env->ReleaseByteArrayElements(dataBuffer, (jbyte*)buffer, 0);
    }
    return (jlong)hr;
}
