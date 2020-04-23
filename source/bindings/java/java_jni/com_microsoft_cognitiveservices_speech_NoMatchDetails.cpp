//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
//
#include "com_microsoft_cognitiveservices_speech_NoMatchDetails.h"
#include "speechapi_c_result.h"
#include "jni_utils.h"

/*
 * Class:     com_microsoft_cognitiveservices_speech_NoMatchDetails
 * Method:    getResultReason
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/IntRef;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_NoMatchDetails_getResultReason
(JNIEnv* env, jobject obj, jobject resultHandle , jobject resultReason)
{
    Result_NoMatchReason reason;
    jlong result = GetObjectHandle(env, resultHandle);
    SPXHR hr = result_get_no_match_reason((SPXRESULTHANDLE)result, &reason);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, resultReason, (jlong)reason);
    }
    return (jlong)hr;
}
