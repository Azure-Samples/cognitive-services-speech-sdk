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
    const char* uri = GetStringUTFChars(env, uriStr);
    SPXHR hr = language_understanding_model_create_from_uri(&luModel, uri);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, luModelHandle, (jlong)luModel);
    }
    ReleaseStringUTFChars(env, uriStr, uri);
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
    const char* app_id = GetStringUTFChars(env, appId);
    SPXHR hr = language_understanding_model_create_from_app_id(&luModel, app_id);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, luModelHandle, (jlong)luModel);
    }
    ReleaseStringUTFChars(env, appId, app_id);
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
    const char* key = GetStringUTFChars(env, subscriptionKey);
    const char* app_id = GetStringUTFChars(env, appId);
    const char* reg = GetStringUTFChars(env, region);
    SPXHR hr = language_understanding_model_create_from_subscription(&luModel, key, app_id, reg);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, luModelHandle, (jlong)luModel);
    }
    ReleaseStringUTFChars(env, subscriptionKey, key);
    ReleaseStringUTFChars(env, appId, app_id);
    ReleaseStringUTFChars(env, region, reg);
    return (jlong)hr;
}
