//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
//
#include "com_microsoft_cognitiveservices_speech_SpeechConfig.h"
#include "speechapi_c_speech_config.h"
#include "jni_utils.h"

/*
 * Class:     com_microsoft_cognitiveservices_speech_SpeechConfig
 * Method:    fromSubscription
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/IntRef;Ljava/lang/String;Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_SpeechConfig_fromSubscription 
  (JNIEnv *env, jclass cls, jobject objHandle, jstring subsriptionKey, jstring serviceRegion)
{
    SPXSPEECHCONFIGHANDLE configHandle = SPXHANDLE_INVALID;
    const char *subscription = env->GetStringUTFChars(subsriptionKey, 0);
    const char *region = env->GetStringUTFChars(serviceRegion, 0);
    SPXHR hr = speech_config_from_subscription(&configHandle, subscription, region);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, objHandle, (jlong)configHandle);
    }
    env->ReleaseStringUTFChars(subsriptionKey, subscription);
    env->ReleaseStringUTFChars(serviceRegion, region);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_SpeechConfig
 * Method:    fromAuthorizationToken
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/IntRef;Ljava/lang/String;Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_SpeechConfig_fromAuthorizationToken
  (JNIEnv *env, jclass cls, jobject objHandle, jstring authToken, jstring serviceRegion)
{
    SPXSPEECHCONFIGHANDLE configHandle = SPXHANDLE_INVALID;
    const char *authorizationToken = env->GetStringUTFChars(authToken, 0);
    const char *region = env->GetStringUTFChars(serviceRegion, 0);
    SPXHR hr = speech_config_from_authorization_token(&configHandle, authorizationToken, region);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, objHandle, (jlong)configHandle);
    }
    env->ReleaseStringUTFChars(authToken, authorizationToken);
    env->ReleaseStringUTFChars(serviceRegion, region);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_SpeechConfig
 * Method:    fromEndpoint
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/IntRef;Ljava/lang/String;Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_SpeechConfig_fromEndpoint
  (JNIEnv *env, jclass cls, jobject objHandle, jstring serviceEndpoint, jstring subscriptionKey)
{
    SPXSPEECHCONFIGHANDLE configHandle = SPXHANDLE_INVALID;
    const char *subscription = NULL;
    if (subscriptionKey != NULL)
    {
        subscription = env->GetStringUTFChars(subscriptionKey, 0);
    }
    const char *endpoint = env->GetStringUTFChars(serviceEndpoint, 0);
    SPXHR hr = speech_config_from_endpoint(&configHandle, endpoint, subscription);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, objHandle, (jlong)configHandle);
    }
    if (subscriptionKey != NULL)
    {
        env->ReleaseStringUTFChars(subscriptionKey, subscription);
    }
    env->ReleaseStringUTFChars(serviceEndpoint, endpoint);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_SpeechConfig
 * Method:    fromHost
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/IntRef;Ljava/lang/String;Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_SpeechConfig_fromHost
  (JNIEnv *env, jclass cls, jobject objHandle, jstring hostName, jstring subscriptionKey)
{
    SPXSPEECHCONFIGHANDLE configHandle = SPXHANDLE_INVALID;
    const char *subscription = NULL;
    if (subscriptionKey != NULL)
    {
        subscription = env->GetStringUTFChars(subscriptionKey, 0);
    }
    const char *host = env->GetStringUTFChars(hostName, 0);
    SPXHR hr = speech_config_from_host(&configHandle, host, subscription);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, objHandle, (jlong)configHandle);
    }
    if (subscriptionKey != NULL)
    {
        env->ReleaseStringUTFChars(subscriptionKey, subscription);
    }
    env->ReleaseStringUTFChars(hostName, host);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_SpeechConfig
 * Method:    getPropertyBag
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/IntRef;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_SpeechConfig_getPropertyBag
  (JNIEnv *env, jobject obj, jobject confHandle, jobject propHandle)
{
    jlong configHandle = GetObjectHandle(env, confHandle);
    SPXPROPERTYBAGHANDLE propertyHandle = SPXHANDLE_INVALID;
    SPXHR hr = speech_config_get_property_bag((SPXSPEECHCONFIGHANDLE)configHandle, &propertyHandle);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, propHandle, (jlong)propertyHandle);
    }
    return (jlong)hr;
}
  
/*
 * Class:     com_microsoft_cognitiveservices_speech_SpeechConfig
 * Method:    setProfanity
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;I)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_SpeechConfig_setProfanity
  (JNIEnv *env, jobject obj, jobject objHandle, jint profanity)
{
    jlong configHandle = GetObjectHandle(env, objHandle);
    SPXHR hr = speech_config_set_profanity((SPXSPEECHCONFIGHANDLE)configHandle, (SpeechConfig_ProfanityOption)profanity);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_SpeechConfig
 * Method:    setServiceProperty
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Ljava/lang/String;Ljava/lang/String;I)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_SpeechConfig_setServiceProperty
  (JNIEnv *env, jobject obj, jobject objHandle, jstring propName, jstring propValue, jint channel)
{
    const char *propertyName = env->GetStringUTFChars(propName, 0);
    const char *propertyValue = env->GetStringUTFChars(propValue, 0);
    jlong configHandle = GetObjectHandle(env, objHandle);
    SPXHR hr = speech_config_set_service_property((SPXSPEECHCONFIGHANDLE)configHandle, propertyName, propertyValue, (SpeechConfig_ServicePropertyChannel)channel);   
    env->ReleaseStringUTFChars(propName, propertyName);
    env->ReleaseStringUTFChars(propValue, propertyValue);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_SpeechConfig
 * Method:    setAudioOutputFormat
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;I)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_SpeechConfig_setAudioOutputFormat
  (JNIEnv *env, jobject obj, jobject objHandle, jint value)
{
    jlong configHandle = GetObjectHandle(env, objHandle);
    SPXHR hr = speech_config_set_audio_output_format((SPXSPEECHCONFIGHANDLE)configHandle, (Speech_Synthesis_Output_Format)value);
    return (jlong)hr;
}
