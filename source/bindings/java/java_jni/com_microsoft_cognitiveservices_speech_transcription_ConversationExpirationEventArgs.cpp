//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
//
#include "com_microsoft_cognitiveservices_speech_transcription_ConversationExpirationEventArgs.h"
#include "speechapi_c_conversation_translator.h"
#include "jni_utils.h"
/*
 * Class:     com_microsoft_cognitiveservices_speech_transcription_ConversationExpirationEventArgs
 * Method:    getExpirationTime
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_transcription_ConversationExpirationEventArgs_getExpirationTime
  (JNIEnv *env, jobject obj, jobject eventHandle, jobject hrIntRef)
{
    int32_t offset = 0;
    jlong handle = GetObjectHandle(env, eventHandle);
    SPXHR hr = conversation_translator_event_get_expiration_time((SPXEVENTHANDLE) handle, &offset);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, hrIntRef, (jlong)hr);
    }
    return (jlong)hr;
}
