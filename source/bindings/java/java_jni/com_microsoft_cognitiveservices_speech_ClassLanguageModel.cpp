//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
//
#include "com_microsoft_cognitiveservices_speech_ClassLanguageModel.h"
#include "speechapi_c_grammar.h"
#include "jni_utils.h"

/*
 * Class:     com_microsoft_cognitiveservices_speech_ClassLanguageModel
 * Method:    fromStorageId
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/IntRef;Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_ClassLanguageModel_fromStorageId
  (JNIEnv* env, jclass cls, jobject grammarHandle, jstring storageId)
{
    SPXGRAMMARHANDLE grammar = SPXHANDLE_INVALID;
    const char* id = env->GetStringUTFChars(storageId, 0);
    SPXHR hr = class_language_model_from_storage_id(&grammar, id);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, grammarHandle, (jlong)grammar);
    }
    env->ReleaseStringUTFChars(storageId, id);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_ClassLanguageModel
 * Method:    assignClass
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Ljava/lang/String;Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_ClassLanguageModel_assignClass
  (JNIEnv *env, jobject obj, jobject clmHandle, jstring className, jobject grammarToAssign)
{
    jlong clm = GetObjectHandle(env, clmHandle);
    jlong grammar = GetObjectHandle(env, grammarToAssign);
    const char* clsName = env->GetStringUTFChars(className, 0);
    SPXHR hr = class_language_model_assign_class((SPXGRAMMARHANDLE)clm, clsName, (SPXGRAMMARHANDLE)grammar);
    env->ReleaseStringUTFChars(className, clsName);
    return (jlong)hr;
}
