//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
//
#include "com_microsoft_cognitiveservices_speech_Connection.h"
#include "speechapi_c_connection.h"
#include "jni_utils.h"

void ConnectedCallback(SPXEVENTHANDLE event, void* context)
{
    CallbackEventMethod(context, "connectedEventCallback", event);
}

void DisconnectedCallback(SPXEVENTHANDLE event, void* context)
{
    CallbackEventMethod(context, "disconnectedEventCallback", event);
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_Connection
 * Method:    connectionFromRecognizer
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/IntRef;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_Connection_connectionFromRecognizer
  (JNIEnv *env, jclass cls, jobject recoHandle, jobject connectionHandle)
{
    SPXCONNECTIONHANDLE result = SPXHANDLE_INVALID;
    jlong handle = GetObjectHandle(env, recoHandle);
    SPXHR hr = connection_from_recognizer((SPXRECOHANDLE)handle, &result);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, connectionHandle, (jlong)result);
    }
    return hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_Connection
 * Method:    connectionFromConversationTranslator
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/IntRef;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_Connection_connectionFromConversationTranslator
  (JNIEnv *env, jclass cls, jobject convTransHandle, jobject connectionHandle)
{
    SPXCONNECTIONHANDLE result = SPXHANDLE_INVALID;
    jlong handle = GetObjectHandle(env, convTransHandle);
    SPXHR hr = connection_from_conversation_translator((SPXCONVERSATIONTRANSLATORHANDLE)handle, &result);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, connectionHandle, (jlong)result);
    }
    return hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_Connection
 * Method:    connectionFromDialogServiceConnector
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/IntRef;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_Connection_connectionFromDialogServiceConnector
  (JNIEnv *env, jclass cls, jobject dialogServiceConnectorHandle, jobject connectionHandle)
{
    SPXCONNECTIONHANDLE result = SPXHANDLE_INVALID;
    jlong handle = GetObjectHandle(env, dialogServiceConnectorHandle);
    SPXHR hr = connection_from_dialog_service_connector((SPXRECOHANDLE)handle, &result);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, connectionHandle, (jlong)result);
    }
    return hr;
}
/*
 * Class:     com_microsoft_cognitiveservices_speech_Connection
 * Method:    openConnection
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Z)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_Connection_openConnection
  (JNIEnv *env, jobject obj, jobject connectionHandle, jboolean forContinuousRecognition)
{
    jlong handle = GetObjectHandle(env, connectionHandle);
    SPXHR hr = connection_open((SPXCONNECTIONHANDLE)handle, (bool) forContinuousRecognition);
    return hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_Connection
 * Method:    closeConnection
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_Connection_closeConnection
  (JNIEnv *env, jobject obj, jobject connectionHandle)
{
    jlong handle = GetObjectHandle(env, connectionHandle);
    SPXHR hr = connection_close((SPXCONNECTIONHANDLE)handle);
    return hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_Connection
 * Method:    connectionSendMessage
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Ljava/lang/String;Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_Connection_connectionSendMessage
  (JNIEnv *env, jobject obj, jobject connectionHandle, jstring path, jstring payload)
{
    jlong handle = GetObjectHandle(env, connectionHandle);
    const char *pathCStr = GetStringUTFChars(env, path);
    const char *payloadCStr = GetStringUTFChars(env, payload);
    SPXHR hr = connection_send_message((SPXCONNECTIONHANDLE)handle, pathCStr, payloadCStr);
    ReleaseStringUTFChars(env, path, pathCStr);
    ReleaseStringUTFChars(env, payload, payloadCStr);
    return hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_Connection
 * Method:    connectionSetMessageProperty
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_Connection_connectionSetMessageProperty
(JNIEnv *env, jobject obj, jobject connectionHandle, jstring path, jstring propertyName, jstring propertyValue)
{
    jlong handle = GetObjectHandle(env, connectionHandle);
    const char *pathCStr = GetStringUTFChars(env, path);
    const char *propertyNameCStr = GetStringUTFChars(env, propertyName);
    const char *propertyValueCStr = GetStringUTFChars(env, propertyValue);
    SPXHR hr = connection_set_message_property((SPXCONNECTIONHANDLE)handle, pathCStr, propertyNameCStr, propertyValueCStr);
    ReleaseStringUTFChars(env, path, pathCStr);
    ReleaseStringUTFChars(env, propertyName, propertyNameCStr);
    ReleaseStringUTFChars(env, propertyValue, propertyValueCStr);
    return hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_Connection
 * Method:    connectionConnectedSetCallback
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_Connection_connectionConnectedSetCallback
  (JNIEnv *env, jobject obj, jlong connectionHandle)
{
    AddGlobalReferenceForHandle(env, obj, (SPXHANDLE)connectionHandle);
    SPXHR hr = connection_connected_set_callback((SPXCONNECTIONHANDLE)connectionHandle, ConnectedCallback, (SPXHANDLE)connectionHandle);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_Connection
 * Method:    connectionDisconnectedSetCallback
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_Connection_connectionDisconnectedSetCallback
  (JNIEnv *env, jobject obj, jlong connectionHandle)
{
    AddGlobalReferenceForHandle(env, obj, (SPXHANDLE)connectionHandle);
    SPXHR hr = connection_disconnected_set_callback((SPXCONNECTIONHANDLE)connectionHandle, DisconnectedCallback, (SPXHANDLE)connectionHandle);
    return (jlong)hr;
}
