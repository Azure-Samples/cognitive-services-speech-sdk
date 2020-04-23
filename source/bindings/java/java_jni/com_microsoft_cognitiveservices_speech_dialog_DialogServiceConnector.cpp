//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
//
#include "com_microsoft_cognitiveservices_speech_dialog_DialogServiceConnector.h"
#include "speechapi_c_dialog_service_connector.h"
#include "speechapi_c_factory.h"
#include "jni_utils.h"

const size_t maxCharCount = 50;

void ActivityReceivedCallback(SPXRECOHANDLE recoHandle, SPXEVENTHANDLE eventHandle, void* context)
{
    CallbackEventMethod(context, "activityReceivedEventCallback", eventHandle);
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_dialog_DialogServiceConnector
 * Method:    createDialogServiceConnectorFomConfig
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_dialog_DialogServiceConnector_createDialogServiceConnectorFomConfig
(JNIEnv* env, jclass cls, jobject dialogServiceHandle, jobject speechConfigHandle , jobject audioConfigHandle)
{
    SPXRECOHANDLE recoHandle = SPXHANDLE_INVALID;
    jlong speechConfig = GetObjectHandle(env, speechConfigHandle);
    jlong audioConfig = GetObjectHandle(env, audioConfigHandle);
    SPXHR hr = dialog_service_connector_create_dialog_service_connector_from_config(&recoHandle, (SPXSPEECHCONFIGHANDLE)speechConfig, (SPXAUDIOCONFIGHANDLE)audioConfig);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, dialogServiceHandle, (jlong)recoHandle);
    }
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_dialog_DialogServiceConnector
 * Method:    connect
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_dialog_DialogServiceConnector_connect
  (JNIEnv* env, jobject obj, jobject dialogServiceHandle)
{
    jlong connector = GetObjectHandle(env, dialogServiceHandle);
    SPXHR hr = dialog_service_connector_connect((SPXRECOHANDLE) connector);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_dialog_DialogServiceConnector
 * Method:    disconnect
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_dialog_DialogServiceConnector_disconnect
  (JNIEnv* env, jobject obj, jobject dialogServiceHandle)
{
    jlong connector = GetObjectHandle(env, dialogServiceHandle);
    SPXHR hr = dialog_service_connector_disconnect((SPXRECOHANDLE)connector);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_dialog_DialogServiceConnector
 * Method:    sendActivity
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/StringRef;Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_dialog_DialogServiceConnector_sendActivity
  (JNIEnv *env, jobject obj, jobject dialogServiceHandle, jobject interactionId, jstring activityStr)
{
    char interaction_id[maxCharCount + 1] = {};
    jlong connector = GetObjectHandle(env, dialogServiceHandle);
    const char* activity = env->GetStringUTFChars(activityStr, 0);
    SPXHR hr = dialog_service_connector_send_activity((SPXRECOHANDLE)connector, activity, interaction_id);
    if (SPX_SUCCEEDED(hr))
    {
        std::string value = std::string(interaction_id);
        SetStringObjectHandle(env, interactionId, value.c_str());
    }
    env->ReleaseStringUTFChars(activityStr, activity);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_dialog_DialogServiceConnector
 * Method:    listenOnce
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/IntRef;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_dialog_DialogServiceConnector_listenOnce
  (JNIEnv *env, jobject obj, jobject dialogServiceHandle, jobject resultHandle)
{
    jlong connector = GetObjectHandle(env, dialogServiceHandle);
    SPXRESULTHANDLE result = SPXHANDLE_INVALID;
    SPXHR hr = dialog_service_connector_listen_once((SPXRECOHANDLE)connector, &result);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, resultHandle, (jlong)result);
    }
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_dialog_DialogServiceConnector
 * Method:    startKeywordRecognition
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_dialog_DialogServiceConnector_startKeywordRecognition
  (JNIEnv *env, jobject obj, jobject dialogServiceHandle, jobject keywordModelHandle)
{
    jlong connector = GetObjectHandle(env, dialogServiceHandle);
    jlong keyword = GetObjectHandle(env, keywordModelHandle);
    SPXHR hr = dialog_service_connector_start_keyword_recognition((SPXRECOHANDLE)connector, (SPXKEYWORDHANDLE)keyword);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_dialog_DialogServiceConnector
 * Method:    stopKeywordRecognition
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_dialog_DialogServiceConnector_stopKeywordRecognition
  (JNIEnv *env, jobject obj, jobject dialogServiceHandle)
{
    jlong connector = GetObjectHandle(env, dialogServiceHandle);
    SPXHR hr = dialog_service_connector_stop_keyword_recognition((SPXRECOHANDLE)connector);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_dialog_DialogServiceConnector
 * Method:    recognizingSetCallback
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_dialog_DialogServiceConnector_recognizingSetCallback
  (JNIEnv *env, jobject obj, jlong recoHandle)
{
    AddGlobalReferenceForHandle(env, obj, (SPXHANDLE)recoHandle);
    SPXHR hr = dialog_service_connector_recognizing_set_callback((SPXRECOHANDLE)recoHandle, RecognizingCallback, (SPXHANDLE)recoHandle);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_dialog_DialogServiceConnector
 * Method:    recognizedSetCallback
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_dialog_DialogServiceConnector_recognizedSetCallback
  (JNIEnv* env, jobject obj, jlong recoHandle)
{
    AddGlobalReferenceForHandle(env, obj, (SPXHANDLE)recoHandle);
    SPXHR hr = dialog_service_connector_recognized_set_callback((SPXRECOHANDLE)recoHandle, RecognizedCallback, (SPXHANDLE)recoHandle);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_dialog_DialogServiceConnector
 * Method:    sessionStartedSetCallback
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_dialog_DialogServiceConnector_sessionStartedSetCallback
  (JNIEnv* env, jobject obj, jlong recoHandle)
{
    AddGlobalReferenceForHandle(env, obj, (SPXHANDLE)recoHandle);
    SPXHR hr = dialog_service_connector_session_started_set_callback((SPXRECOHANDLE)recoHandle, SessionStartedCallback, (SPXHANDLE)recoHandle);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_dialog_DialogServiceConnector
 * Method:    sessionStoppedSetCallback
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_dialog_DialogServiceConnector_sessionStoppedSetCallback
  (JNIEnv* env, jobject obj, jlong recoHandle)
{
    AddGlobalReferenceForHandle(env, obj, (SPXHANDLE)recoHandle);
    SPXHR hr = dialog_service_connector_session_stopped_set_callback((SPXRECOHANDLE)recoHandle, SessionStoppedCallback, (SPXHANDLE)recoHandle);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_dialog_DialogServiceConnector
 * Method:    canceledSetCallback
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_dialog_DialogServiceConnector_canceledSetCallback
  (JNIEnv* env, jobject obj, jlong recoHandle)
{
    AddGlobalReferenceForHandle(env, obj, (SPXHANDLE)recoHandle);
    SPXHR hr = dialog_service_connector_canceled_set_callback((SPXRECOHANDLE)recoHandle, CanceledCallback, (SPXHANDLE)recoHandle);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_dialog_DialogServiceConnector
 * Method:    activityReceivedSetCallback
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_dialog_DialogServiceConnector_activityReceivedSetCallback
  (JNIEnv* env, jobject obj, jlong recoHandle)
{
    AddGlobalReferenceForHandle(env, obj, (SPXHANDLE)recoHandle);
    SPXHR hr = dialog_service_connector_activity_received_set_callback((SPXRECOHANDLE)recoHandle, ActivityReceivedCallback, (SPXHANDLE)recoHandle);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_dialog_DialogServiceConnector
 * Method:    getPropertyBagFromDialogServiceConnectorHandle
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/IntRef;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_dialog_DialogServiceConnector_getPropertyBagFromDialogServiceConnectorHandle
  (JNIEnv *env, jobject obj, jobject recognizerHandle, jobject propertyHandle)
{
    SPXPROPERTYBAGHANDLE propHandle = SPXHANDLE_INVALID;
    jlong recoHandle = GetObjectHandle(env, recognizerHandle);
    SPXHR hr = dialog_service_connector_get_property_bag((SPXRECOHANDLE)recoHandle, &propHandle);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, propertyHandle, (jlong)propHandle);
    }
    return (jlong)hr;
}
