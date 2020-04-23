//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
//
#include "com_microsoft_cognitiveservices_speech_dialog_CustomCommandsConfig.h"
#include "speechapi_c_dialog_service_config.h"
#include "jni_utils.h"

/*
 * Class:     com_microsoft_cognitiveservices_speech_dialog_CustomCommandsConfig
 * Method:    fromSubscription
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/IntRef;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_dialog_CustomCommandsConfig_fromSubscription
  (JNIEnv* env, jclass cls, jobject customConfigHandle, jstring appId, jstring subscriptionKey, jstring serviceRegion)
{
    SPXSPEECHCONFIGHANDLE configHandle = SPXHANDLE_INVALID;
    const char* app_id = env->GetStringUTFChars(appId, 0);
    const char* subscription = env->GetStringUTFChars(subscriptionKey, 0);
    const char* region = env->GetStringUTFChars(serviceRegion, 0);
    SPXHR hr = custom_commands_config_from_subscription(&configHandle, app_id, subscription, region);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, customConfigHandle, (jlong)configHandle);
    }
    env->ReleaseStringUTFChars(appId, app_id);
    env->ReleaseStringUTFChars(subscriptionKey, subscription);
    env->ReleaseStringUTFChars(serviceRegion, region);
    return (jlong)hr;
}
/*
 * Class:     com_microsoft_cognitiveservices_speech_dialog_CustomCommandsConfig
 * Method:    fromAuthorizationToken
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/IntRef;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_dialog_CustomCommandsConfig_fromAuthorizationToken
  (JNIEnv *env, jclass cls, jobject customConfigHandle, jstring appId, jstring authToken, jstring serviceRegion)
{
    SPXSPEECHCONFIGHANDLE configHandle = SPXHANDLE_INVALID;
    const char* app_id = env->GetStringUTFChars(appId, 0);
    const char* authorizationToken = env->GetStringUTFChars(authToken, 0);
    const char* region = env->GetStringUTFChars(serviceRegion, 0);
    SPXHR hr = custom_commands_config_from_authorization_token(&configHandle, app_id, authorizationToken, region);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, customConfigHandle, (jlong)configHandle);
    }
    env->ReleaseStringUTFChars(appId, app_id);
    env->ReleaseStringUTFChars(authToken, authorizationToken);
    env->ReleaseStringUTFChars(serviceRegion, region);
    return (jlong)hr;
}
