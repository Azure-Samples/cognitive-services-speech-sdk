//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
//
#include "com_microsoft_cognitiveservices_speech_translation_TranslationRecognizer.h"
#include "speechapi_c_factory.h"
#include "speechapi_c_translation_recognizer.h"
#include "jni_utils.h"

void SynthesizingCallback(SPXRECOHANDLE recoHandle, SPXEVENTHANDLE eventHandle, void* context)
{
    CallbackEventMethod(context, "synthesizingEventCallback", eventHandle);
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_translation_TranslationRecognizer
 * Method:    createTranslationRecognizerFromConfig
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_translation_TranslationRecognizer_createTranslationRecognizerFromConfig
  (JNIEnv *env, jobject obj, jobject recognizerHandle, jobject speechConfigHandle, jobject audioConfigHandle)
{
    SPXRECOHANDLE recoHandle = SPXHANDLE_INVALID;
    jlong speechConfig = GetObjectHandle(env, speechConfigHandle);
    jlong audioConfig = GetObjectHandle(env, audioConfigHandle);
    SPXHR hr = recognizer_create_translation_recognizer_from_config(&recoHandle, (SPXSPEECHCONFIGHANDLE)speechConfig, (SPXAUDIOCONFIGHANDLE)audioConfig);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, recognizerHandle, (jlong)recoHandle);
    }
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_translation_TranslationRecognizer
 * Method:    synthesizingSetCallback
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_translation_TranslationRecognizer_synthesizingSetCallback
  (JNIEnv *env, jobject obj, jlong recognizerHandle)
{
    AddGlobalReferenceForHandle(env, obj, (SPXHANDLE)recognizerHandle);
    SPXHR hr = translator_synthesizing_audio_set_callback((SPXRECOHANDLE)recognizerHandle, SynthesizingCallback, (SPXHANDLE)recognizerHandle);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_translation_TranslationRecognizer
 * Method:    addTargetLanguage
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_translation_TranslationRecognizer_addTargetLanguage
  (JNIEnv* env, jobject obj, jobject recognizerHandle, jstring value)
{
    jlong recoHandle = GetObjectHandle(env, recognizerHandle);
    const char* language = env->GetStringUTFChars(value, 0);
    SPXHR hr = translator_add_target_language((SPXRECOHANDLE) recoHandle, language);
    env->ReleaseStringUTFChars(value, language);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_translation_TranslationRecognizer
 * Method:    removeTargetLanguage
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_translation_TranslationRecognizer_removeTargetLanguage
  (JNIEnv *env, jobject obj, jobject recognizerHandle, jstring value)
{
    jlong recoHandle = GetObjectHandle(env, recognizerHandle);
    const char* language = env->GetStringUTFChars(value, 0);
    SPXHR hr = translator_remove_target_language((SPXRECOHANDLE)recoHandle, language);
    env->ReleaseStringUTFChars(value, language);
    return (jlong)hr;
}
