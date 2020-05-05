//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
//
#include "com_microsoft_cognitiveservices_speech_intent_IntentTrigger.h"
#include "speechapi_c_intent_trigger.h"
#include "jni_utils.h"

/*
 * Class:     com_microsoft_cognitiveservices_speech_intent_IntentTrigger
 * Method:    createFromPhrase
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/IntRef;Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_intent_IntentTrigger_createFromPhrase
  (JNIEnv *env, jclass cls, jobject triggerHandle, jstring phraseStr)
{
    SPXTRIGGERHANDLE trigger = SPXHANDLE_INVALID;
    const char* phrase = GetStringUTFChars(env, phraseStr);
    SPXHR hr = intent_trigger_create_from_phrase(&trigger, phrase);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, triggerHandle, (jlong)trigger);
    }
    ReleaseStringUTFChars(env, phraseStr, phrase);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_intent_IntentTrigger
 * Method:    createFromLanguageUnderstandingModel
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/IntRef;Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_intent_IntentTrigger_createFromLanguageUnderstandingModel
  (JNIEnv *env, jclass cls, jobject triggerHandle, jobject luModelHandle, jstring intentNameStr)
{
    SPXTRIGGERHANDLE trigger = SPXHANDLE_INVALID;
    jlong luModel = GetObjectHandle(env, luModelHandle);
    const char* intentName = GetStringUTFChars(env, intentNameStr);
    SPXHR hr = intent_trigger_create_from_language_understanding_model(&trigger, (SPXLUMODELHANDLE)luModel, intentName);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, triggerHandle, (jlong)trigger);
    }
    ReleaseStringUTFChars(env, intentNameStr, intentName);
    return (jlong)hr;
}
