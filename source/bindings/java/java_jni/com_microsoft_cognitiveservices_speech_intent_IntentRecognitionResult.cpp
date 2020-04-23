//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
//
#include "com_microsoft_cognitiveservices_speech_intent_IntentRecognitionResult.h"
#include "speechapi_c_intent_result.h"
#include "jni_utils.h"

const size_t maxCharCount = 2048;

/*
 * Class:     com_microsoft_cognitiveservices_speech_intent_IntentRecognitionResult
 * Method:    getIntentId
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/StringRef;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_intent_IntentRecognitionResult_getIntentId
  (JNIEnv *env, jobject obj, jobject resultHandle, jobject intentIdStr)
{
    char sz[maxCharCount + 1] = {};
    jlong handle = GetObjectHandle(env, resultHandle);
    SPXHR hr = intent_result_get_intent_id((SPXRESULTHANDLE)handle, sz, maxCharCount);
    if (SPX_SUCCEEDED(hr))
    {
        std::string value = std::string(sz);
        SetStringObjectHandle(env, intentIdStr, value.c_str());
    }
    return (jlong)hr;
}
