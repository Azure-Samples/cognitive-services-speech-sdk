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
    const char* subscription = env->GetStringUTFChars(subscriptionKey, 0);
    const char* region = env->GetStringUTFChars(serviceRegion, 0);
    SPXHR hr = speech_translation_config_from_subscription(&configHandle, subscription, region);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, translationConfigHandle, (jlong)configHandle);
    }
    env->ReleaseStringUTFChars(subscriptionKey, subscription);
    env->ReleaseStringUTFChars(serviceRegion, region);
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
    const char* authorizationToken = env->GetStringUTFChars(authToken, 0);
    const char* region = env->GetStringUTFChars(serviceRegion, 0);
    SPXHR hr = speech_translation_config_from_authorization_token(&configHandle, authorizationToken, region);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, translationConfigHandle, (jlong)configHandle);
    }
    env->ReleaseStringUTFChars(authToken, authorizationToken);
    env->ReleaseStringUTFChars(serviceRegion, region);
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
    const char* subscription = NULL;
    if (subscriptionKey != NULL)
    {
        subscription = env->GetStringUTFChars(subscriptionKey, 0);
    }
    const char* endpoint = env->GetStringUTFChars(serviceEndpoint, 0);
    SPXHR hr = speech_translation_config_from_endpoint(&configHandle, endpoint, subscription);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, translationConfigHandle, (jlong)configHandle);
    }
    if (subscriptionKey != NULL)
    {
        env->ReleaseStringUTFChars(subscriptionKey, subscription);
    }
    env->ReleaseStringUTFChars(serviceEndpoint, endpoint);
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
    const char* subscription = NULL;
    if (subscriptionKey != NULL)
    {
        subscription = env->GetStringUTFChars(subscriptionKey, 0);
    }
    const char* host = env->GetStringUTFChars(hostName, 0);
    SPXHR hr = speech_translation_config_from_host(&configHandle, host, subscription);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, translationConfigHandle, (jlong)configHandle);
    }
    if (subscriptionKey != NULL)
    {
        env->ReleaseStringUTFChars(subscriptionKey, subscription);
    }
    env->ReleaseStringUTFChars(hostName, host);
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
    const char* language = env->GetStringUTFChars(targetLanguage, 0);
    SPXHR hr = speech_translation_config_add_target_language((SPXSPEECHCONFIGHANDLE)configHandle, language);
    env->ReleaseStringUTFChars(targetLanguage, language);
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
    const char* language = env->GetStringUTFChars(targetLanguage, 0);
    SPXHR hr = speech_translation_config_remove_target_language((SPXSPEECHCONFIGHANDLE)configHandle, language);
    env->ReleaseStringUTFChars(targetLanguage, language);
    return (jlong)hr;
}
