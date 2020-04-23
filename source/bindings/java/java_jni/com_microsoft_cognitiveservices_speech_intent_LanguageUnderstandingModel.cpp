//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
//
#include "com_microsoft_cognitiveservices_speech_intent_LanguageUnderstandingModel.h"
#include "speechapi_c_language_understanding_model.h"
#include "jni_utils.h"

/*
 * Class:     com_microsoft_cognitiveservices_speech_intent_LanguageUnderstandingModel
 * Method:    createModelFromUri
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/IntRef;Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_intent_LanguageUnderstandingModel_createModelFromUri
  (JNIEnv *env, jclass cls, jobject luModelHandle, jstring uriStr)
{
    SPXLUMODELHANDLE luModel = SPXHANDLE_INVALID;
    const char* uri = env->GetStringUTFChars(uriStr, 0);
    SPXHR hr = language_understanding_model_create_from_uri(&luModel, uri);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, luModelHandle, (jlong)luModel);
    }
    env->ReleaseStringUTFChars(uriStr, uri);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_intent_LanguageUnderstandingModel
 * Method:    createModelFromAppId
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/IntRef;Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_intent_LanguageUnderstandingModel_createModelFromAppId
  (JNIEnv *env, jclass cls, jobject luModelHandle, jstring appId)
{
    SPXLUMODELHANDLE luModel = SPXHANDLE_INVALID;
    const char* app_id = env->GetStringUTFChars(appId, 0);
    SPXHR hr = language_understanding_model_create_from_app_id(&luModel, app_id);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, luModelHandle, (jlong)luModel);
    }
    env->ReleaseStringUTFChars(appId, app_id);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_intent_LanguageUnderstandingModel
 * Method:    createModelFromSubscription
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/IntRef;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_intent_LanguageUnderstandingModel_createModelFromSubscription
  (JNIEnv *env, jclass cls, jobject luModelHandle, jstring subscriptionKey, jstring appId, jstring region)
{
    SPXLUMODELHANDLE luModel = SPXHANDLE_INVALID;
    const char* key = env->GetStringUTFChars(subscriptionKey, 0);
    const char* app_id = env->GetStringUTFChars(appId, 0);
    const char* reg = env->GetStringUTFChars(region, 0);
    SPXHR hr = language_understanding_model_create_from_subscription(&luModel, key, app_id, reg);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, luModelHandle, (jlong)luModel);
    }
    env->ReleaseStringUTFChars(subscriptionKey, key);
    env->ReleaseStringUTFChars(appId, app_id);
    env->ReleaseStringUTFChars(region, reg);
    return (jlong)hr;
}
