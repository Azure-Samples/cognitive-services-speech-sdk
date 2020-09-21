//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
//

#include "com_microsoft_cognitiveservices_speech_PronunciationAssessmentConfig.h"
#include "speechapi_c_pronunciation_assessment_config.h"
#include "speechapi_c_property_bag.h"
#include "jni_utils.h"

/*
 * Class:     com_microsoft_cognitiveservices_speech_PronunciationAssessmentConfig
 * Method:    create
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/IntRef;Ljava/lang/String;IIZ)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_PronunciationAssessmentConfig_create
  (JNIEnv *env, jclass cls, jobject pronAssessmentConfigHandle, jstring referenceText, jint gradingSystem, jint granularity, jboolean enableMiscue)
{
    SPXPRONUNCIATIONASSESSMENTCONFIGHANDLE configHandle = SPXHANDLE_INVALID;
    const char* referenceTextPtr = GetStringUTFChars(env, referenceText);
    SPXHR hr = create_pronunciation_assessment_config(&configHandle, referenceTextPtr, (PronunciationAssessment_GradingSystem) gradingSystem, (PronunciationAssessment_Granularity) granularity, (bool) enableMiscue);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, pronAssessmentConfigHandle, (jlong)configHandle);
    }
    ReleaseStringUTFChars(env, referenceText, referenceTextPtr);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_PronunciationAssessmentConfig
 * Method:    fromJson
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/IntRef;Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_PronunciationAssessmentConfig_fromJson
  (JNIEnv *env, jclass cls, jobject pronAssessmentConfigHandle, jstring json)
{
    SPXPRONUNCIATIONASSESSMENTCONFIGHANDLE configHandle = SPXHANDLE_INVALID;
    const char* jsonPtr = GetStringUTFChars(env, json);
    SPXHR hr = create_pronunciation_assessment_config_from_json(&configHandle, jsonPtr);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, pronAssessmentConfigHandle, (jlong)configHandle);
    }
    ReleaseStringUTFChars(env, json, jsonPtr);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_PronunciationAssessmentConfig
 * Method:    getPropertyBag
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/IntRef;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_PronunciationAssessmentConfig_getPropertyBag
  (JNIEnv *env, jobject obj, jobject pronAssessmentConfigHandle, jobject propHandle)
{
    jlong configHandle = GetObjectHandle(env, pronAssessmentConfigHandle);
    SPXPROPERTYBAGHANDLE propertyHandle = SPXHANDLE_INVALID;
    SPXHR hr = pronunciation_assessment_config_get_property_bag((SPXPRONUNCIATIONASSESSMENTCONFIGHANDLE)configHandle, &propertyHandle);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, propHandle, (jlong)propertyHandle);
    }
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_PronunciationAssessmentConfig
 * Method:    applyTo
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_PronunciationAssessmentConfig_applyTo
  (JNIEnv *env, jobject obj, jobject pronAssessmentConfigHandle, jobject recoHandle)
{
    jlong configHandle = GetObjectHandle(env, pronAssessmentConfigHandle);
    jlong recognizerHandle = GetObjectHandle(env, recoHandle);
    SPXHR hr = pronunciation_assessment_config_apply_to_recognizer((SPXPRONUNCIATIONASSESSMENTCONFIGHANDLE)configHandle, (SPXRECOHANDLE)recognizerHandle);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_PronunciationAssessmentConfig
 * Method:    toJson
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_microsoft_cognitiveservices_speech_PronunciationAssessmentConfig_toJson
  (JNIEnv *env, jobject obj, jobject pronAssessmentConfigHandle)
{
    jstring result;
    jlong configHandle = GetObjectHandle(env, pronAssessmentConfigHandle);
    const char* str = pronunciation_assessment_config_to_json((SPXPRONUNCIATIONASSESSMENTCONFIGHANDLE)configHandle);
    result = str ? env->NewStringUTF(str) : env->NewStringUTF("");
    if (str != nullptr)
    {
        property_bag_free_string(str);
    }
    return result;
}
