//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
//
#include "com_microsoft_cognitiveservices_speech_Recognizer.h"
#include "speechapi_c_recognizer.h"
#include "jni_utils.h"

/*
 * Class:     com_microsoft_cognitiveservices_speech_Recognizer
 * Method:    startContinuousRecognition
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_Recognizer_startContinuousRecognition
  (JNIEnv* env, jobject obj, jobject recognizerHandle)
{
    jlong recoHandle = GetObjectHandle(env, recognizerHandle);
    jlong hr = (jlong) recognizer_start_continuous_recognition((SPXRECOHANDLE) recoHandle);
    return hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_Recognizer
 * Method:    stopContinuousRecognition
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_Recognizer_stopContinuousRecognition
  (JNIEnv *env, jobject obj, jobject recognizerHandle)
{
    jlong recoHandle = GetObjectHandle(env, recognizerHandle);
    jlong hr = (jlong)recognizer_stop_continuous_recognition((SPXRECOHANDLE)recoHandle);
    return hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_Recognizer
 * Method:    startKeywordRecognition
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_Recognizer_startKeywordRecognition
  (JNIEnv* env, jobject obj, jobject recognizerHandle, jobject modelHandle)
{
    jlong recoHandle = GetObjectHandle(env, recognizerHandle);
    jlong moHandle = GetObjectHandle(env, modelHandle);
    jlong hr = (jlong)recognizer_start_keyword_recognition((SPXRECOHANDLE)recoHandle, (SPXKEYWORDHANDLE)moHandle);
    return hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_Recognizer
 * Method:    stopKeywordRecognition
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_Recognizer_stopKeywordRecognition
  (JNIEnv *env, jobject obj, jobject recognizerHandle)
{
    jlong recoHandle = GetObjectHandle(env, recognizerHandle);
    jlong hr = (jlong)recognizer_stop_keyword_recognition((SPXRECOHANDLE)recoHandle);
    return hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_Recognizer
 * Method:    recognizingSetCallback
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_Recognizer_recognizingSetCallback
  (JNIEnv *env, jobject obj, jlong recoHandle)
{
    AddGlobalReferenceForHandle(env, obj, (SPXHANDLE)recoHandle);
    SPXHR hr = recognizer_recognizing_set_callback((SPXRECOHANDLE)recoHandle, RecognizingCallback, (SPXHANDLE)recoHandle);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_Recognizer
 * Method:    recognizedSetCallback
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_Recognizer_recognizedSetCallback
  (JNIEnv *env, jobject obj, jlong recoHandle)
{
    AddGlobalReferenceForHandle(env, obj, (SPXHANDLE)recoHandle);
    SPXHR hr = recognizer_recognized_set_callback((SPXRECOHANDLE)recoHandle, RecognizedCallback, (SPXHANDLE)recoHandle);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_Recognizer
 * Method:    canceledSetCallback
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_Recognizer_canceledSetCallback
  (JNIEnv *env, jobject obj, jlong recoHandle)
{
    AddGlobalReferenceForHandle(env, obj, (SPXHANDLE)recoHandle);
    SPXHR hr = recognizer_canceled_set_callback((SPXRECOHANDLE)recoHandle, CanceledCallback, (SPXHANDLE)recoHandle);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_Recognizer
 * Method:    sessionStartedSetCallback
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_Recognizer_sessionStartedSetCallback
  (JNIEnv *env, jobject obj, jlong recoHandle)
{
    AddGlobalReferenceForHandle(env, obj, (SPXHANDLE)recoHandle);
    SPXHR hr = recognizer_session_started_set_callback((SPXRECOHANDLE)recoHandle, SessionStartedCallback, (SPXHANDLE)recoHandle);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_Recognizer
 * Method:    sessionStoppedSetCallback
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_Recognizer_sessionStoppedSetCallback
  (JNIEnv *env, jobject obj, jlong recoHandle)
{
    AddGlobalReferenceForHandle(env, obj, (SPXHANDLE)recoHandle);
    SPXHR hr = recognizer_session_stopped_set_callback((SPXRECOHANDLE)recoHandle, SessionStoppedCallback, (SPXHANDLE)recoHandle);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_Recognizer
 * Method:    speechStartDetectedSetCallback
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_Recognizer_speechStartDetectedSetCallback
  (JNIEnv *env, jobject obj, jlong recoHandle)
{
    AddGlobalReferenceForHandle(env, obj, (SPXHANDLE)recoHandle);
    SPXHR hr = recognizer_speech_start_detected_set_callback((SPXRECOHANDLE)recoHandle, SpeechStartDetectedCallback, (SPXHANDLE)recoHandle);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_Recognizer
 * Method:    speechEndDetectedSetCallback
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_Recognizer_speechEndDetectedSetCallback
  (JNIEnv *env, jobject obj, jlong recoHandle)
{
    AddGlobalReferenceForHandle(env, obj, (SPXHANDLE)recoHandle);
    SPXHR hr = recognizer_speech_end_detected_set_callback((SPXRECOHANDLE)recoHandle, SpeechEndDetectedCallback, (SPXHANDLE)recoHandle);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_Recognizer
 * Method:    getPropertyBagFromRecognizerHandle
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/IntRef;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_Recognizer_getPropertyBagFromRecognizerHandle
  (JNIEnv* env, jobject obj, jobject recognizerHandle, jobject propertyHandle)
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

/*
 * Class:     com_microsoft_cognitiveservices_speech_Recognizer
 * Method:    recognizeOnce
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/IntRef;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_Recognizer_recognizeOnce
  (JNIEnv* env, jobject obj, jobject recoHandle, jobject resultHandle)
{
    SPXRESULTHANDLE result = SPXHANDLE_INVALID;
    jlong handle = GetObjectHandle(env, recoHandle);
    SPXHR hr = recognizer_recognize_once((SPXRECOHANDLE)handle, &result);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, resultHandle, (jlong)result);
    }
    return (jlong)hr;
}
