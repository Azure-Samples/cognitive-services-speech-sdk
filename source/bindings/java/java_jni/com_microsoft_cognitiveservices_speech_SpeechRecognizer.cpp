//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
//
#include "com_microsoft_cognitiveservices_speech_SpeechRecognizer.h"
#include "speechapi_c_factory.h"
#include "speechapi_c_recognizer.h"
#include "jni_utils.h"

/*
 * Class:     com_microsoft_cognitiveservices_speech_SpeechRecognizer
 * Method:    createSpeechRecognizerFromConfig
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_SpeechRecognizer_createSpeechRecognizerFromConfig
  (JNIEnv *env, jobject obj, jobject recognizerHandle, jobject speechConfigHandle, jobject audioConfigHandle)
{
    SPXRECOHANDLE recoHandle = SPXHANDLE_INVALID;
    jlong speechConfig = GetObjectHandle(env, speechConfigHandle);
    jlong audioConfig = GetObjectHandle(env, audioConfigHandle);
    SPXHR hr = recognizer_create_speech_recognizer_from_config(&recoHandle, (SPXSPEECHCONFIGHANDLE)speechConfig, (SPXAUDIOCONFIGHANDLE)audioConfig);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, recognizerHandle, (jlong)recoHandle);
    }
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_SpeechRecognizer
 * Method:    createSpeechRecognizerFromAutoDetectSourceLangConfig
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_SpeechRecognizer_createSpeechRecognizerFromAutoDetectSourceLangConfig
  (JNIEnv *env, jobject obj, jobject recognizerHandle, jobject speechConfigHandle, jobject autoDetectHandle, jobject audioConfigHandle)
{
    SPXRECOHANDLE recoHandle = SPXHANDLE_INVALID;
    jlong speechConfig = GetObjectHandle(env, speechConfigHandle);
    jlong autoDetectLangConfig = GetObjectHandle(env, autoDetectHandle);
    jlong audioConfig = GetObjectHandle(env, audioConfigHandle);
    SPXHR hr = recognizer_create_speech_recognizer_from_auto_detect_source_lang_config(&recoHandle, (SPXSPEECHCONFIGHANDLE)speechConfig, (SPXAUTODETECTSOURCELANGCONFIGHANDLE)autoDetectLangConfig, (SPXAUDIOCONFIGHANDLE)audioConfig);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, recognizerHandle, (jlong)recoHandle);
    }
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_SpeechRecognizer
 * Method:    createSpeechRecognizerFromSourceLangConfig
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_SpeechRecognizer_createSpeechRecognizerFromSourceLangConfig
  (JNIEnv *env, jobject obj, jobject recognizerHandle, jobject speechConfigHandle, jobject sourceLangConfigHandle, jobject audioConfigHandle)
{
    SPXRECOHANDLE recoHandle = SPXHANDLE_INVALID;
    jlong speechConfig = GetObjectHandle(env, speechConfigHandle);
    jlong sourceLangConfig = GetObjectHandle(env, sourceLangConfigHandle);
    jlong audioConfig = GetObjectHandle(env, audioConfigHandle);
    SPXHR hr = recognizer_create_speech_recognizer_from_source_lang_config(&recoHandle, (SPXSPEECHCONFIGHANDLE)speechConfig, (SPXSOURCELANGCONFIGHANDLE)sourceLangConfig, (SPXAUDIOCONFIGHANDLE)audioConfig);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, recognizerHandle, (jlong)recoHandle);
    }
    return (jlong)hr;
}
