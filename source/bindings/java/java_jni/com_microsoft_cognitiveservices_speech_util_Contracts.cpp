//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
//
#include "com_microsoft_cognitiveservices_speech_util_Contracts.h"
#include "speechapi_c_error.h"
#include "jni_utils.h"

/*
 * Class:     com_microsoft_cognitiveservices_speech_util_Contracts
 * Method:    getErrorMessage
 * Signature: (J)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_microsoft_cognitiveservices_speech_util_Contracts_getErrorMessage
  (JNIEnv *env, jclass cls, jlong objHandle)
{
    const char* message = error_get_message((SPXERRORHANDLE)objHandle);
    jstring result = message ? env->NewStringUTF(message) : env->NewStringUTF("");
    return result;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_util_Contracts
 * Method:    getErrorCallStack
 * Signature: (J)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_microsoft_cognitiveservices_speech_util_Contracts_getErrorCallStack
  (JNIEnv *env, jclass cls, jlong objHandle)
{
    const char* callstack = error_get_call_stack((SPXERRORHANDLE)objHandle);
    jstring result = callstack ? env->NewStringUTF(callstack) : env->NewStringUTF("");
    return result;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_util_Contracts
 * Method:    getErrorCode
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_com_microsoft_cognitiveservices_speech_util_Contracts_getErrorCode
  (JNIEnv *env, jclass cls, jlong objHandle)
{
    return (jint)error_get_error_code((SPXERRORHANDLE)objHandle);
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_util_Contracts
 * Method:    releaseErrorHandle
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_util_Contracts_releaseErrorHandle
  (JNIEnv *env, jclass cls, jlong objHandle)
{
    SPXHR hr = error_release((SPXERRORHANDLE)objHandle);
    return (jlong)hr;
}
