//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
//
#include "com_microsoft_cognitiveservices_speech_RecognitionResult.h"
#include "speechapi_c_result.h"
#include "jni_utils.h"

const size_t maxCharCount = 2048;

/*
 * Class:     com_microsoft_cognitiveservices_speech_RecognitionResult
 * Method:    getResultId
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/StringRef;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_RecognitionResult_getResultId
  (JNIEnv *env, jobject obj, jobject resultHandle, jobject resultIdStrRef)
{
    char sz[maxCharCount + 1] = {};
    jlong handle = GetObjectHandle(env, resultHandle);
    SPXHR hr = result_get_result_id((SPXRESULTHANDLE)handle, sz, maxCharCount);
    if (SPX_SUCCEEDED(hr))
    {
        std::string value = std::string(sz);
        SetStringObjectHandle(env, resultIdStrRef, value.c_str());
    }
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_RecognitionResult
 * Method:    getResultReason
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/IntRef;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_RecognitionResult_getResultReason
  (JNIEnv *env, jobject obj, jobject resultHandle, jobject resultReasonIntRef)
{
    Result_Reason resultReason;
    jlong handle = GetObjectHandle(env, resultHandle);
    SPXHR hr = result_get_reason((SPXRESULTHANDLE)handle, &resultReason);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, resultReasonIntRef, (jlong)resultReason);
    }
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_RecognitionResult
 * Method:    getResultText
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/StringRef;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_RecognitionResult_getResultText
  (JNIEnv *env, jobject obj, jobject resultHandle, jobject resultTextStrRef)
{
    char sz[maxCharCount + 1] = {};
    jlong handle = GetObjectHandle(env, resultHandle);
    SPXHR hr = result_get_text((SPXRESULTHANDLE)handle, sz, maxCharCount);
    if (SPX_SUCCEEDED(hr))
    {
        std::string value = std::string(sz);
        SetStringObjectHandle(env, resultTextStrRef, value.c_str());
    }
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_RecognitionResult
 * Method:    getResultDuration
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/IntRef;)Ljava/math/BigInteger;
 */
JNIEXPORT jobject JNICALL Java_com_microsoft_cognitiveservices_speech_RecognitionResult_getResultDuration
  (JNIEnv *env, jobject obj, jobject resultHandle, jobject resultHr)
{    
    uint64_t duration = 0;
    jlong handle = GetObjectHandle(env, resultHandle);
    SPXHR hr = result_get_duration((SPXRESULTHANDLE)handle, &duration);
    SetObjectHandle(env, resultHr, (jlong)hr);
    jobject jresult = AsBigInteger(env, duration);
    return jresult;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_RecognitionResult
 * Method:    getResultOffset
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/IntRef;)Ljava/math/BigInteger;
 */
JNIEXPORT jobject JNICALL Java_com_microsoft_cognitiveservices_speech_RecognitionResult_getResultOffset
  (JNIEnv *env, jobject obj, jobject resultHandle, jobject resultHr)
{
    uint64_t offset = 0;
    jlong handle = GetObjectHandle(env, resultHandle);
    SPXHR hr = result_get_offset((SPXRESULTHANDLE)handle, &offset);
    SetObjectHandle(env, resultHr, (jlong)hr);
    jobject jresult = AsBigInteger(env, offset);
    return jresult;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_RecognitionResult
 * Method:    getPropertyBagFromResult
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/IntRef;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_RecognitionResult_getPropertyBagFromResult
  (JNIEnv *env, jobject obj, jobject resultHandle, jobject propertyHandle)
{
    SPXPROPERTYBAGHANDLE progbag = SPXHANDLE_INVALID;
    jlong handle = GetObjectHandle(env, resultHandle);
    SPXHR hr = result_get_property_bag((SPXRESULTHANDLE) handle, &progbag);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, propertyHandle, (jlong)progbag);
    }
    return (jlong)hr;
}
