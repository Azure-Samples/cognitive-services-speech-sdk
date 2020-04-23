//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
//
#include "com_microsoft_cognitiveservices_speech_dialog_BotFrameworkConfig.h"
#include "speechapi_c_dialog_service_config.h"
#include "jni_utils.h"

/*
 * Class:     com_microsoft_cognitiveservices_speech_dialog_BotFrameworkConfig
 * Method:    fromSubscription
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/IntRef;Ljava/lang/String;Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_dialog_BotFrameworkConfig_fromSubscription
  (JNIEnv *env, jclass cls, jobject botConfigHandle, jstring subscriptionKey, jstring serviceRegion, jstring botIdStr)
{
    SPXSPEECHCONFIGHANDLE configHandle = SPXHANDLE_INVALID;
    const char* subscription = env->GetStringUTFChars(subscriptionKey, 0);
    const char* region = env->GetStringUTFChars(serviceRegion, 0);
    const char* botId = NULL;
    if (botIdStr != NULL)
    {
        botId = env->GetStringUTFChars(botIdStr, 0);
    }
    SPXHR hr = bot_framework_config_from_subscription(&configHandle, subscription, region, botId);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, botConfigHandle, (jlong)configHandle);
    }
    env->ReleaseStringUTFChars(subscriptionKey, subscription);
    env->ReleaseStringUTFChars(serviceRegion, region);
    if (botIdStr != NULL)
    {
        env->ReleaseStringUTFChars(botIdStr, botId);
    }
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_dialog_BotFrameworkConfig
 * Method:    fromAuthorizationToken
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/IntRef;Ljava/lang/String;Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_dialog_BotFrameworkConfig_fromAuthorizationToken
  (JNIEnv* env, jclass cls, jobject botConfigHandle, jstring authToken, jstring serviceRegion, jstring botIdStr)
{
    SPXSPEECHCONFIGHANDLE configHandle = SPXHANDLE_INVALID;
    const char* authorizationToken = env->GetStringUTFChars(authToken, 0);
    const char* region = env->GetStringUTFChars(serviceRegion, 0);
    const char* botId = NULL;
    if (botIdStr != NULL)
    {
        botId = env->GetStringUTFChars(botIdStr, 0);
    }
    SPXHR hr = bot_framework_config_from_authorization_token(&configHandle, authorizationToken, region, botId);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, botConfigHandle, (jlong)configHandle);
    }
    env->ReleaseStringUTFChars(authToken, authorizationToken);
    env->ReleaseStringUTFChars(serviceRegion, region);
    if (botIdStr != NULL)
    {
        env->ReleaseStringUTFChars(botIdStr, botId);
    }
    return (jlong)hr;
}
