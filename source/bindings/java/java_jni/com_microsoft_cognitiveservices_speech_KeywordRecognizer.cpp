//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
//
#include "com_microsoft_cognitiveservices_speech_KeywordRecognizer.h"
#include "speechapi_c_factory.h"
#include "speechapi_c_recognizer.h"
#include "jni_utils.h"

/*
 * Class:     com_microsoft_cognitiveservices_speech_KeywordRecognizer
 * Method:    createKeywordRecognizerFromConfig
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_KeywordRecognizer_createKeywordRecognizerFromConfig
  (JNIEnv *env, jobject obj, jobject recognizerHandle, jobject audioConfig)
{
    SPXRECOHANDLE recoHandle = SPXHANDLE_INVALID;
    jlong audioConfigHandle = GetObjectHandle(env, audioConfig);
    SPXHR hr = recognizer_create_keyword_recognizer_from_audio_config(&recoHandle, (SPXAUDIOCONFIGHANDLE)audioConfigHandle);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, recognizerHandle, (jlong)recoHandle);
    }
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_KeywordRecognizer
 * Method:    recognizeOnce
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/IntRef;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_KeywordRecognizer_recognizeOnce
  (JNIEnv *env, jobject obj, jobject recognizerHandle, jobject keywordModel, jobject resultHandle)
{
    SPXRESULTHANDLE result = SPXHANDLE_INVALID;
    jlong recoHandle = GetObjectHandle(env, recognizerHandle);
    jlong keywordModelHandle = GetObjectHandle(env, keywordModel);
    SPXHR hr = recognizer_recognize_keyword_once((SPXRECOHANDLE)recoHandle, (SPXKEYWORDHANDLE)keywordModelHandle, &result);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, resultHandle, (jlong)result);
    }
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_KeywordRecognizer
 * Method:    stopRecognition
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_KeywordRecognizer_stopRecognition
  (JNIEnv *env, jobject obj, jobject recognizerHandle)
{
    jlong recoHandle = GetObjectHandle(env, recognizerHandle);
    SPXHR hr = recognizer_stop_keyword_recognition((SPXRECOHANDLE)recoHandle);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_KeywordRecognizer
 * Method:    recognizedSetCallback
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_KeywordRecognizer_recognizedSetCallback
  (JNIEnv *env, jobject obj, jlong recoHandle)
{
    AddGlobalReferenceForHandle(env, obj, (SPXHANDLE)recoHandle);
    SPXHR hr = recognizer_recognized_set_callback((SPXRECOHANDLE)recoHandle, RecognizedCallback, (SPXHANDLE)recoHandle);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_KeywordRecognizer
 * Method:    canceledSetCallback
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_KeywordRecognizer_canceledSetCallback
  (JNIEnv *env, jobject obj, jlong recoHandle)
{
    AddGlobalReferenceForHandle(env, obj, (SPXHANDLE)recoHandle);
    SPXHR hr = recognizer_canceled_set_callback((SPXRECOHANDLE)recoHandle, CanceledCallback, (SPXHANDLE)recoHandle);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_KeywordRecognizer
 * Method:    getPropertyBagFromRecognizerHandle
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/IntRef;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_KeywordRecognizer_getPropertyBagFromRecognizerHandle
  (JNIEnv *env, jobject obj, jobject recognizerHandle, jobject propertyHandle)
{
    SPXPROPERTYBAGHANDLE propHandle = SPXHANDLE_INVALID;
    jlong recoHandle = GetObjectHandle(env, recognizerHandle);
    SPXHR hr = recognizer_get_property_bag((SPXRECOHANDLE)recoHandle, &propHandle);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, propertyHandle, (jlong)propHandle);
    }
    return (jlong)hr;
}
