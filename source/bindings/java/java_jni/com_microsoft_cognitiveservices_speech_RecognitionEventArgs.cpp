//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
//
#include "com_microsoft_cognitiveservices_speech_RecognitionEventArgs.h"
#include "speechapi_c_recognizer.h"
#include "jni_utils.h"
/*
 * Class:     com_microsoft_cognitiveservices_speech_RecognitionEventArgs
 * Method:    getOffset
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/IntRef;)Ljava/math/BigInteger;
 */
JNIEXPORT jobject JNICALL Java_com_microsoft_cognitiveservices_speech_RecognitionEventArgs_getOffset
  (JNIEnv *env, jobject obj, jobject eventHandle, jobject hrIntRef)
{
    uint64_t offset = 0;
    jlong handle = GetObjectHandle(env, eventHandle);
    SPXHR hr = recognizer_recognition_event_get_offset((SPXEVENTHANDLE) handle, &offset);
    SetObjectHandle(env, hrIntRef, (jlong)hr);
    jobject jresult = AsBigInteger(env, offset);
    return jresult;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_RecognitionEventArgs
 * Method:    getRecognitionResult
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/IntRef;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_RecognitionEventArgs_getRecognitionResult
  (JNIEnv *env, jobject obj, jobject eventHandle, jobject resultHandle)
{
    SPXRESULTHANDLE result = SPXHANDLE_INVALID;
    jlong handle = GetObjectHandle(env, eventHandle);
    SPXHR hr = recognizer_recognition_event_get_result((SPXEVENTHANDLE) handle, &result);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, resultHandle, (jlong)result);
    }
    return (jlong)hr;
}
