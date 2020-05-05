//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
//
#include "com_microsoft_cognitiveservices_speech_translation_SpeechTranslationConfig.h"
#include "speechapi_c_speech_translation_config.h"
#include "jni_utils.h"

/*
 * Class:     com_microsoft_cognitiveservices_speech_translation_SpeechTranslationConfig
 * Method:    fromSubscription
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/IntRef;Ljava/lang/String;Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_translation_SpeechTranslationConfig_fromSubscription
  (JNIEnv *env, jclass cls, jobject translationConfigHandle, jstring subscriptionKey, jstring serviceRegion)
{
    SPXSPEECHCONFIGHANDLE configHandle = SPXHANDLE_INVALID;
    const char* subscription = GetStringUTFChars(env, subscriptionKey);
    const char* region = GetStringUTFChars(env, serviceRegion);
    SPXHR hr = speech_translation_config_from_subscription(&configHandle, subscription, region);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, translationConfigHandle, (jlong)configHandle);
    }
    ReleaseStringUTFChars(env, subscriptionKey, subscription);
    ReleaseStringUTFChars(env, serviceRegion, region);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_translation_SpeechTranslationConfig
 * Method:    fromAuthorizationToken
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/IntRef;Ljava/lang/String;Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_translation_SpeechTranslationConfig_fromAuthorizationToken
  (JNIEnv *env, jclass cls, jobject translationConfigHandle, jstring authToken, jstring serviceRegion)
{
    SPXSPEECHCONFIGHANDLE configHandle = SPXHANDLE_INVALID;
    const char* authorizationToken = GetStringUTFChars(env, authToken);
    const char* region = GetStringUTFChars(env, serviceRegion);
    SPXHR hr = speech_translation_config_from_authorization_token(&configHandle, authorizationToken, region);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, translationConfigHandle, (jlong)configHandle);
    }
    ReleaseStringUTFChars(env, authToken, authorizationToken);
    ReleaseStringUTFChars(env, serviceRegion, region);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_translation_SpeechTranslationConfig
 * Method:    fromEndpoint
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/IntRef;Ljava/lang/String;Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_translation_SpeechTranslationConfig_fromEndpoint
  (JNIEnv *env, jclass cls, jobject translationConfigHandle, jstring serviceEndpoint, jstring subscriptionKey)
{
    SPXSPEECHCONFIGHANDLE configHandle = SPXHANDLE_INVALID;
    const char* subscription = GetStringUTFChars(env, subscriptionKey);
    const char* endpoint = GetStringUTFChars(env, serviceEndpoint);
    SPXHR hr = speech_translation_config_from_endpoint(&configHandle, endpoint, subscription);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, translationConfigHandle, (jlong)configHandle);
    }
    ReleaseStringUTFChars(env, subscriptionKey, subscription);
    ReleaseStringUTFChars(env, serviceEndpoint, endpoint);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_translation_SpeechTranslationConfig
 * Method:    fromHost
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/IntRef;Ljava/lang/String;Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_translation_SpeechTranslationConfig_fromHost
  (JNIEnv *env, jclass cls, jobject translationConfigHandle, jstring hostName, jstring subscriptionKey)
{
    SPXSPEECHCONFIGHANDLE configHandle = SPXHANDLE_INVALID;
    const char* subscription = GetStringUTFChars(env, subscriptionKey);
    const char* host = GetStringUTFChars(env, hostName);
    SPXHR hr = speech_translation_config_from_host(&configHandle, host, subscription);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, translationConfigHandle, (jlong)configHandle);
    }
    ReleaseStringUTFChars(env, subscriptionKey, subscription);
    ReleaseStringUTFChars(env, hostName, host);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_translation_SpeechTranslationConfig
 * Method:    addTargetLanguage
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_translation_SpeechTranslationConfig_addTargetLanguage
  (JNIEnv *env, jobject obj, jobject translationConfigHandle, jstring targetLanguage)
{
    jlong configHandle = GetObjectHandle(env, translationConfigHandle);
    const char* language = GetStringUTFChars(env, targetLanguage);
    SPXHR hr = speech_translation_config_add_target_language((SPXSPEECHCONFIGHANDLE)configHandle, language);
    ReleaseStringUTFChars(env, targetLanguage, language);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_translation_SpeechTranslationConfig
 * Method:    removeTargetLanguage
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_translation_SpeechTranslationConfig_removeTargetLanguage
  (JNIEnv *env, jobject obj, jobject translationConfigHandle, jstring targetLanguage)
{
    jlong configHandle = GetObjectHandle(env, translationConfigHandle);
    const char* language = GetStringUTFChars(env, targetLanguage);
    SPXHR hr = speech_translation_config_remove_target_language((SPXSPEECHCONFIGHANDLE)configHandle, language);
    ReleaseStringUTFChars(env, targetLanguage, language);
    return (jlong)hr;
}
