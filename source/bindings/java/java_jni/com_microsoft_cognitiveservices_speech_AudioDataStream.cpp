//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
//
#include "com_microsoft_cognitiveservices_speech_AudioDataStream.h"
#include "speechapi_c_audio_stream.h"
#include "jni_utils.h"

/*
 * Class:     com_microsoft_cognitiveservices_speech_AudioDataStream
 * Method:    createFromResult
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/IntRef;Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_AudioDataStream_createFromResult
  (JNIEnv *env, jclass cls, jobject streamHandle, jobject resultHandle)
{
    SPXAUDIOSTREAMHANDLE audioStream = SPXHANDLE_INVALID;
    jlong result = GetObjectHandle(env, resultHandle);
    SPXHR hr = audio_data_stream_create_from_result(&audioStream, (SPXRESULTHANDLE)result);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, streamHandle, (jlong)audioStream);
    }
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_AudioDataStream
 * Method:    getStatus
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/IntRef;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_AudioDataStream_getStatus
  (JNIEnv *env, jobject obj, jobject streamHandle, jobject streamStatus)
{
    Stream_Status status;
    jlong audioStream = GetObjectHandle(env, streamHandle);
    SPXHR hr = audio_data_stream_get_status((SPXAUDIOSTREAMHANDLE)audioStream, &status);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, streamStatus, (jlong)status);
    }
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_AudioDataStream
 * Method:    canReadData
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;J)Z
 */
JNIEXPORT jboolean JNICALL Java_com_microsoft_cognitiveservices_speech_AudioDataStream_canReadData__Lcom_microsoft_cognitiveservices_speech_util_SafeHandle_2J
  (JNIEnv *env, jobject obj, jobject streamHandle, jlong bytesRequested)
{
    jlong audioStream = GetObjectHandle(env, streamHandle);
    bool canRead = audio_data_stream_can_read_data((SPXAUDIOSTREAMHANDLE)audioStream, (uint32_t)bytesRequested);
    return canRead;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_AudioDataStream
 * Method:    canReadData
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;JJ)Z
 */
JNIEXPORT jboolean JNICALL Java_com_microsoft_cognitiveservices_speech_AudioDataStream_canReadData__Lcom_microsoft_cognitiveservices_speech_util_SafeHandle_2JJ
  (JNIEnv *env, jobject obj, jobject streamHandle, jlong pos, jlong bytesRequested)
{
    jlong audioStream = GetObjectHandle(env, streamHandle);
    bool canRead = audio_data_stream_can_read_data_from_position((SPXAUDIOSTREAMHANDLE)audioStream, (uint32_t)bytesRequested, (uint32_t)pos);
    return canRead;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_AudioDataStream
 * Method:    readData
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;[BLcom/microsoft/cognitiveservices/speech/util/IntRef;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_AudioDataStream_readData__Lcom_microsoft_cognitiveservices_speech_util_SafeHandle_2_3BLcom_microsoft_cognitiveservices_speech_util_IntRef_2
  (JNIEnv *env, jobject obj, jobject streamHandle, jbyteArray dataBuffer, jobject filledSize)
{
    jlong audioStream = GetObjectHandle(env, streamHandle);
    uint8_t* buffer = (uint8_t*)env->GetByteArrayElements(dataBuffer, 0);
    uint32_t bufferSize = (uint32_t)env->GetArrayLength(dataBuffer);
    uint32_t filled = 0;
    SPXHR hr = audio_data_stream_read((SPXAUDIOSTREAMHANDLE)audioStream, buffer, bufferSize, &filled);
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

/*
 * Class:     com_microsoft_cognitiveservices_speech_AudioDataStream
 * Method:    readData
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;[BJLcom/microsoft/cognitiveservices/speech/util/IntRef;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_AudioDataStream_readData__Lcom_microsoft_cognitiveservices_speech_util_SafeHandle_2_3BJLcom_microsoft_cognitiveservices_speech_util_IntRef_2
(JNIEnv* env, jobject obj, jobject streamHandle, jbyteArray dataBuffer, jlong position, jobject filledSize)
{
    jlong audioStream = GetObjectHandle(env, streamHandle);
    uint8_t* buffer = (uint8_t*)env->GetByteArrayElements(dataBuffer, 0);
    uint32_t bufferSize = (uint32_t)env->GetArrayLength(dataBuffer);
    uint32_t filled = 0;
    SPXHR hr = audio_data_stream_read_from_position((SPXAUDIOSTREAMHANDLE)audioStream, buffer, bufferSize, (uint32_t) position, &filled);
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

/*
 * Class:     com_microsoft_cognitiveservices_speech_AudioDataStream
 * Method:    saveToWaveFile
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_AudioDataStream_saveToWaveFile
  (JNIEnv *env, jobject obj, jobject streamHandle, jstring fileName)
{
    jlong audioStream = GetObjectHandle(env, streamHandle);
    const char* file = GetStringUTFChars(env, fileName);
    SPXHR hr = audio_data_stream_save_to_wave_file((SPXAUDIOSTREAMHANDLE)audioStream, file);
    ReleaseStringUTFChars(env, fileName, file);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_AudioDataStream
 * Method:    getPosition
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/IntRef;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_AudioDataStream_getPosition
  (JNIEnv *env, jobject obj, jobject streamHandle, jobject position)
{
    jlong audioStream = GetObjectHandle(env, streamHandle);
    uint32_t pos = 0;
    SPXHR hr = audio_data_stream_get_position((SPXAUDIOSTREAMHANDLE)audioStream, &pos);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, position, (jlong)pos);
    }
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_AudioDataStream
 * Method:    setPosition
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;J)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_AudioDataStream_setPosition
  (JNIEnv *env, jobject obj, jobject streamHandle, jlong position)
{
    jlong audioStream = GetObjectHandle(env, streamHandle);
    SPXHR hr = audio_data_stream_set_position((SPXAUDIOSTREAMHANDLE)audioStream, (uint32_t)position);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_AudioDataStream
 * Method:    getPropertyBagFromStreamHandle
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/IntRef;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_AudioDataStream_getPropertyBagFromStreamHandle
  (JNIEnv *env, jobject obj, jobject streamHandle, jobject propertyHandle)
{
    SPXPROPERTYBAGHANDLE propHandle = SPXHANDLE_INVALID;
    jlong audioStream = GetObjectHandle(env, streamHandle);
    SPXHR hr = audio_data_stream_get_property_bag((SPXAUDIOSTREAMHANDLE)audioStream, &propHandle);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, propertyHandle, (jlong)propHandle);
    }
    return (jlong)hr;
}
