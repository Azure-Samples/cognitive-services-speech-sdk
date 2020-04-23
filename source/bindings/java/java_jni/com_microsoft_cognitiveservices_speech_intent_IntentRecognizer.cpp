//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
//
#include "com_microsoft_cognitiveservices_speech_intent_IntentRecognizer.h"
#include "speechapi_c_factory.h"
#include "speechapi_c_intent_recognizer.h"
#include "jni_utils.h"

/*
 * Class:     com_microsoft_cognitiveservices_speech_intent_IntentRecognizer
 * Method:    createIntentRecognizerFromConfig
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_intent_IntentRecognizer_createIntentRecognizerFromConfig
  (JNIEnv *env, jobject obj, jobject recognizerHandle, jobject speechConfigHandle, jobject audioConfigHandle)
{
    SPXRECOHANDLE recoHandle = SPXHANDLE_INVALID;
    jlong speechConfig = GetObjectHandle(env, speechConfigHandle);
    jlong audioConfig = GetObjectHandle(env, audioConfigHandle);
    SPXHR hr = recognizer_create_intent_recognizer_from_config(&recoHandle, (SPXSPEECHCONFIGHANDLE)speechConfig, (SPXAUDIOCONFIGHANDLE)audioConfig);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, recognizerHandle, (jlong)recoHandle);
    }
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_intent_IntentRecognizer
 * Method:    addIntent
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Ljava/lang/String;Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_intent_IntentRecognizer_addIntent
  (JNIEnv *env, jobject obj, jobject recognizerHandle, jstring intentId, jobject triggerHandle)
{
    jlong recoHandle = GetObjectHandle(env, recognizerHandle);
    jlong trigger = GetObjectHandle(env, triggerHandle);
    const char* id = env->GetStringUTFChars(intentId, 0);
    SPXHR hr = intent_recognizer_add_intent((SPXRECOHANDLE) recoHandle, id, (SPXTRIGGERHANDLE)trigger);
    env->ReleaseStringUTFChars(intentId, id);
    return (jlong)hr;
}
