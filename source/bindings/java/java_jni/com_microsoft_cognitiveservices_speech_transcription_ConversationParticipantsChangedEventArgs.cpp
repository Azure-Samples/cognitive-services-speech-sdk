//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
//
#include "com_microsoft_cognitiveservices_speech_transcription_ConversationParticipantsChangedEventArgs.h"
#include "speechapi_c_conversation_translator.h"
#include "speechapi_c_property_bag.h"
#include "jni_utils.h"
#include "spxdebug.h"
/*
 * Class:     com_microsoft_cognitiveservices_speech_transcription_ConversationParticipantsChangedEventArgs
 * Method:    getParticipantAt
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_transcription_ConversationParticipantsChangedEventArgs_getParticipantAt
  (JNIEnv *env, jobject obj, jobject eventHandle, jint jindex, jobject participantHandle)
{
    SPXRESULTHANDLE participant = SPXHANDLE_INVALID;
    jlong handle = GetObjectHandle(env, eventHandle);
    SPXHR hr = conversation_translator_event_get_participant_changed_at_index((SPXEVENTHANDLE) handle, jindex, &participant);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, participantHandle, (jlong)participant);
    }

    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_transcription_ConversationParticipantsChangedEventArgs
 * Method:    getReason
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/IntRef;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_transcription_ConversationParticipantsChangedEventArgs_getReason
(JNIEnv* env, jobject obj, jobject resultHandle , jobject resultReason)
{
    ParticipantChangedReason reason;
    jlong result = GetObjectHandle(env, resultHandle);
    SPXHR hr = conversation_translator_event_get_participant_changed_reason((SPXRESULTHANDLE)result, &reason);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, resultReason, (jlong)reason);
    }
    return (jlong)hr;
}
