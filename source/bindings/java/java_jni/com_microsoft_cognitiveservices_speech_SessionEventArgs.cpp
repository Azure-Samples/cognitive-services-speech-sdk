//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
//
#include "com_microsoft_cognitiveservices_speech_SessionEventArgs.h"
#include "speechapi_c_recognizer.h"
#include "jni_utils.h"

static const auto cchMaxUUID = 36;
static const auto cchMaxSessionId = cchMaxUUID + 1;

/*
 * Class:     com_microsoft_cognitiveservices_speech_SessionEventArgs
 * Method:    getSessionId
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/StringRef;I)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_SessionEventArgs_getSessionId
(JNIEnv *env, jobject obj, jobject eventHandle, jobject sessionIdStrRef, jint maxSessionIdCharLength)
{
    char sessionId[cchMaxSessionId] = {};
    jlong handle = GetObjectHandle(env, eventHandle);
    SPXHR hr = recognizer_session_event_get_session_id((SPXEVENTHANDLE) handle, sessionId, cchMaxSessionId);
    if (SPX_SUCCEEDED(hr))
    {
        std::string value = std::string(sessionId);
        SetStringObjectHandle(env, sessionIdStrRef, value.c_str());
    }
    return (jlong)hr;
}

