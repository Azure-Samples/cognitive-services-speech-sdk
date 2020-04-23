//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
//
#include "com_microsoft_cognitiveservices_speech_CancellationDetails.h"
#include "speechapi_c_result.h"
#include "jni_utils.h"
/*
 * Class:     com_microsoft_cognitiveservices_speech_CancellationDetails
 * Method:    getCanceledReason
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/IntRef;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_CancellationDetails_getCanceledReason
  (JNIEnv *env, jobject obj, jobject resultHandle, jobject canceledReasonIntRef)
{
    Result_CancellationReason reason;
    jlong handle = GetObjectHandle(env, resultHandle);
    SPXHR hr = result_get_reason_canceled((SPXRESULTHANDLE) handle, &reason);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, canceledReasonIntRef, (jlong)reason);
    }
    return (jlong)hr;
}
/*
 * Class:     com_microsoft_cognitiveservices_speech_CancellationDetails
 * Method:    getCanceledErrorCode
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/IntRef;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_CancellationDetails_getCanceledErrorCode
  (JNIEnv *env, jobject obj, jobject resultHandle, jobject canceledErrorCodeIntRef)
{
    Result_CancellationErrorCode errorCode;
    jlong handle = GetObjectHandle(env, resultHandle);
    SPXHR hr = result_get_canceled_error_code((SPXRESULTHANDLE)handle, &errorCode);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, canceledErrorCodeIntRef, (jlong)errorCode);
    }
    return (jlong)hr;
}
