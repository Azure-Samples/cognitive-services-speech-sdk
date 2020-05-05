//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
//
#include "com_microsoft_cognitiveservices_speech_AutoDetectSourceLanguageConfig.h"
#include "speechapi_c_auto_detect_source_lang_config.h"
#include "jni_utils.h"

/*
 * Class:     com_microsoft_cognitiveservices_speech_AutoDetectSourceLanguageConfig
 * Method:    fromLanguages
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/IntRef;Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_AutoDetectSourceLanguageConfig_fromLanguages
  (JNIEnv *env, jclass cls, jobject autoDetectSourceLanguageConfigHandle, jstring languages)
{
    SPXAUTODETECTSOURCELANGCONFIGHANDLE configHandle = SPXHANDLE_INVALID;
    const char* langs = GetStringUTFChars(env, languages);
    SPXHR hr = create_auto_detect_source_lang_config_from_languages(&configHandle, langs);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, autoDetectSourceLanguageConfigHandle, (jlong)configHandle);
    }
    ReleaseStringUTFChars(env, languages, langs);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_AutoDetectSourceLanguageConfig
 * Method:    createFromSourceLangConfig
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/IntRef;Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_AutoDetectSourceLanguageConfig_createFromSourceLangConfig
  (JNIEnv *env, jclass cls, jobject autoDetectSourceLanguageConfigHandle, jobject sourceLanguageConfig)
{
    SPXAUTODETECTSOURCELANGCONFIGHANDLE autoDetectConfig = SPXHANDLE_INVALID;
    jlong sourceLangConfig = GetObjectHandle(env, sourceLanguageConfig);
    SPXHR hr = create_auto_detect_source_lang_config_from_source_lang_config(&autoDetectConfig, (SPXSOURCELANGCONFIGHANDLE)sourceLangConfig);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, autoDetectSourceLanguageConfigHandle, (jlong)autoDetectConfig);
    }
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_AutoDetectSourceLanguageConfig
 * Method:    addSourceLangConfigToAutoDetectSourceLangConfig
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/IntRef;Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_AutoDetectSourceLanguageConfig_addSourceLangConfigToAutoDetectSourceLangConfig
  (JNIEnv* env, jclass cls, jobject autoDetectSourceLanguageConfigHandle, jobject sourceLanguageConfig)
{
    jlong autoDetectConfig = GetObjectHandle(env, autoDetectSourceLanguageConfigHandle);
    jlong sourceLangConfig = GetObjectHandle(env, sourceLanguageConfig);
    SPXHR hr = add_source_lang_config_to_auto_detect_source_lang_config((SPXAUTODETECTSOURCELANGCONFIGHANDLE)autoDetectConfig, (SPXSOURCELANGCONFIGHANDLE)sourceLangConfig);
    return (jlong)hr;
}
