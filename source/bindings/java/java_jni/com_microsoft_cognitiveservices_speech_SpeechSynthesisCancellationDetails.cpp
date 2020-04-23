//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
//
#include "com_microsoft_cognitiveservices_speech_SpeechSynthesisCancellationDetails.h"
#include "speechapi_c_result.h"
#include "speechapi_c_audio_stream.h"
#include "jni_utils.h"

/*
 * Class:     com_microsoft_cognitiveservices_speech_SpeechSynthesisCancellationDetails
 * Method:    getCanceledReasonFromSynthResult
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/IntRef;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_SpeechSynthesisCancellationDetails_getCanceledReasonFromSynthResult
  (JNIEnv *env, jobject obj, jobject resultHandle, jobject reasonHandle)
{
    Result_CancellationReason reason;
    jlong handle = GetObjectHandle(env, resultHandle);
    SPXHR hr = synth_result_get_reason_canceled((SPXRESULTHANDLE)handle, &reason);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, reasonHandle, (jlong)reason);
    }
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_SpeechSynthesisCancellationDetails
 * Method:    getCanceledErrorCodeFromSynthResult
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/IntRef;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_SpeechSynthesisCancellationDetails_getCanceledErrorCodeFromSynthResult
  (JNIEnv *env, jobject obj, jobject resultHandle, jobject errorHandle)
{
    Result_CancellationErrorCode errorCode;
    jlong handle = GetObjectHandle(env, resultHandle);
    SPXHR hr = synth_result_get_canceled_error_code((SPXRESULTHANDLE)handle, &errorCode);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, errorHandle, (jlong)errorCode);
    }
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_SpeechSynthesisCancellationDetails
 * Method:    getCanceledReasonFromStream
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/IntRef;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_SpeechSynthesisCancellationDetails_getCanceledReasonFromStream
  (JNIEnv *env, jobject obj, jobject streamHandle, jobject reasonHandle)
{
    Result_CancellationReason reason;
    jlong handle = GetObjectHandle(env, streamHandle);
    SPXHR hr = audio_data_stream_get_reason_canceled((SPXAUDIOSTREAMHANDLE)handle, &reason);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, reasonHandle, (jlong)reason);
    }
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_SpeechSynthesisCancellationDetails
 * Method:    getCanceledErrorCodeFromStream
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/IntRef;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_SpeechSynthesisCancellationDetails_getCanceledErrorCodeFromStream
  (JNIEnv* env, jobject obj, jobject streamHandle, jobject errorHandle)
{
    Result_CancellationErrorCode errorCode;
    jlong handle = GetObjectHandle(env, streamHandle);
    SPXHR hr = audio_data_stream_get_canceled_error_code((SPXAUDIOSTREAMHANDLE)handle, &errorCode);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, errorHandle, (jlong)errorCode);
    }
    return (jlong)hr;
}
