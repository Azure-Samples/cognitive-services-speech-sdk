//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
//
#include "com_microsoft_cognitiveservices_speech_KeywordRecognitionModel.h"
#include "speechapi_c_keyword_recognition_model.h"
#include "jni_utils.h"

/*
 * Class:     com_microsoft_cognitiveservices_speech_KeywordRecognitionModel
 * Method:    createKeywordRecognitionModelFromFile
 * Signature: (Ljava/lang/String;Lcom/microsoft/cognitiveservices/speech/util/IntRef;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_KeywordRecognitionModel_createKeywordRecognitionModelFromFile
  (JNIEnv *env, jclass cls, jstring modelFile, jobject keywordModelHandle)
{
    SPXKEYWORDHANDLE kwmodel = SPXHANDLE_INVALID;
    const char* fileName = env->GetStringUTFChars(modelFile, 0);
    SPXHR hr = keyword_recognition_model_create_from_file(fileName, &kwmodel);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, keywordModelHandle, (jlong)kwmodel);
    }
    env->ReleaseStringUTFChars(modelFile, fileName);
    return (jlong)hr;
}
