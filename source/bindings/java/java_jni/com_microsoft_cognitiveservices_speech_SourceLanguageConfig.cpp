//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
//
#include "com_microsoft_cognitiveservices_speech_SourceLanguageConfig.h"
#include "speechapi_c_source_lang_config.h"
#include "jni_utils.h"

/*
 * Class:     com_microsoft_cognitiveservices_speech_SourceLanguageConfig
 * Method:    fromLanguage
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/IntRef;Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_SourceLanguageConfig_fromLanguage
  (JNIEnv*env, jclass cls, jobject sourceLangConfigHandle, jstring language)
{
    SPXSOURCELANGCONFIGHANDLE configHandle = SPXHANDLE_INVALID;
    const char* lang = GetStringUTFChars(env, language);
    SPXHR hr = source_lang_config_from_language(&configHandle, lang);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, sourceLangConfigHandle, (jlong)configHandle);
    }
    ReleaseStringUTFChars(env, language, lang);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_SourceLanguageConfig
 * Method:    fromLanguageAndEndpointId
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/IntRef;Ljava/lang/String;Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_SourceLanguageConfig_fromLanguageAndEndpointId
  (JNIEnv *env, jclass cls, jobject sourceLangConfigHandle, jstring language, jstring endpointId)
{
    SPXSOURCELANGCONFIGHANDLE configHandle = SPXHANDLE_INVALID;
    const char* lang = GetStringUTFChars(env, language);
    const char* endpoint = GetStringUTFChars(env, endpointId);
    SPXHR hr = source_lang_config_from_language_and_endpointId(&configHandle, lang, endpoint);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, sourceLangConfigHandle, (jlong)configHandle);
    }
    ReleaseStringUTFChars(env, language, lang);
    ReleaseStringUTFChars(env, endpointId, endpoint);
    return (jlong)hr;
}
