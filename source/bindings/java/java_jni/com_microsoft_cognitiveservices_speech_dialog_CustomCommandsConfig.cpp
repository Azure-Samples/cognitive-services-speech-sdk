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
    const char* app_id = GetStringUTFChars(env, appId);
    const char* subscription = GetStringUTFChars(env, subscriptionKey);
    const char* region = GetStringUTFChars(env, serviceRegion);
    SPXHR hr = custom_commands_config_from_subscription(&configHandle, app_id, subscription, region);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, customConfigHandle, (jlong)configHandle);
    }
    ReleaseStringUTFChars(env, appId, app_id);
    ReleaseStringUTFChars(env, subscriptionKey, subscription);
    ReleaseStringUTFChars(env, serviceRegion, region);
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
    const char* app_id = GetStringUTFChars(env, appId);
    const char* authorizationToken = GetStringUTFChars(env, authToken);
    const char* region = GetStringUTFChars(env, serviceRegion);
    SPXHR hr = custom_commands_config_from_authorization_token(&configHandle, app_id, authorizationToken, region);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, customConfigHandle, (jlong)configHandle);
    }
    ReleaseStringUTFChars(env, appId, app_id);
    ReleaseStringUTFChars(env, authToken, authorizationToken);
    ReleaseStringUTFChars(env, serviceRegion, region);
    return (jlong)hr;
}
