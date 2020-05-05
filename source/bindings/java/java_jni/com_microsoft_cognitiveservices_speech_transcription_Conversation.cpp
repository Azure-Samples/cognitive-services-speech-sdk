//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
//
#include "com_microsoft_cognitiveservices_speech_transcription_Conversation.h"
#include "speechapi_c_conversation.h"
#include "jni_utils.h"

const size_t maxCharCount = 1024;

/*
 * Class:     com_microsoft_cognitiveservices_speech_transcription_Conversation
 * Method:    createConversationFromConfig
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/IntRef;Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_transcription_Conversation_createConversationFromConfig
  (JNIEnv *env, jclass cls, jobject conversationHandle, jobject speechConfigHandle, jstring conversationId)
{
    SPXCONVERSATIONHANDLE convHandle = SPXHANDLE_INVALID;
    jlong speechConfig = GetObjectHandle(env, speechConfigHandle);
    const char* id = GetStringUTFChars(env, conversationId);
    SPXHR hr = conversation_create_from_config(&convHandle, (SPXSPEECHCONFIGHANDLE)speechConfig, id);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, conversationHandle, (jlong)convHandle);
    }
    ReleaseStringUTFChars(env, conversationId, id);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_transcription_Conversation
 * Method:    getConversationId
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/StringRef;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_transcription_Conversation_getConversationId
  (JNIEnv *env, jobject obj, jobject conversationHandle, jobject conversationIdStr)
{
    char sz[maxCharCount + 1] = {};
    jlong convHandle = GetObjectHandle(env, conversationHandle);
    SPXHR hr = conversation_get_conversation_id((SPXCONVERSATIONHANDLE)convHandle, sz, maxCharCount);
    if (SPX_SUCCEEDED(hr))
    {
        hr = SetStringObjectHandle(env, conversationIdStr, sz);
    }
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_transcription_Conversation
 * Method:    addParticipant
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_transcription_Conversation_addParticipant
  (JNIEnv *env, jobject obj, jobject conversationHandle, jobject participantHandle)
{
    jlong convHandle = GetObjectHandle(env, conversationHandle);
    jlong partHandle = GetObjectHandle(env, participantHandle);
    SPXHR hr = conversation_update_participant((SPXCONVERSATIONHANDLE)convHandle, true, (SPXPARTICIPANTHANDLE)partHandle);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_transcription_Conversation
 * Method:    addParticipantByUser
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_transcription_Conversation_addParticipantByUser
  (JNIEnv *env, jobject obj, jobject conversationHandle, jobject userHandle)
{
    jlong convHandle = GetObjectHandle(env, conversationHandle);
    jlong usrHandle = GetObjectHandle(env, userHandle);
    SPXHR hr = conversation_update_participant_by_user((SPXCONVERSATIONHANDLE)convHandle, true, (SPXUSERHANDLE)usrHandle);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_transcription_Conversation
 * Method:    removeParticipant
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_transcription_Conversation_removeParticipant
  (JNIEnv *env, jobject obj, jobject conversationHandle, jobject participantHandle)
{
    jlong convHandle = GetObjectHandle(env, conversationHandle);
    jlong partHandle = GetObjectHandle(env, participantHandle);
    SPXHR hr = conversation_update_participant((SPXCONVERSATIONHANDLE)convHandle, false, (SPXPARTICIPANTHANDLE)partHandle);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_transcription_Conversation
 * Method:    removeParticipantByUser
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_transcription_Conversation_removeParticipantByUser
  (JNIEnv *env, jobject obj, jobject conversationHandle, jobject userHandle)
{
    jlong convHandle = GetObjectHandle(env, conversationHandle);
    jlong usrHandle = GetObjectHandle(env, userHandle);
    SPXHR hr = conversation_update_participant_by_user((SPXCONVERSATIONHANDLE)convHandle, false, (SPXUSERHANDLE)usrHandle);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_transcription_Conversation
 * Method:    removeParticipantByUserId
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_transcription_Conversation_removeParticipantByUserId
  (JNIEnv *env, jobject obj, jobject conversationHandle, jstring userId)
{
    jlong convHandle = GetObjectHandle(env, conversationHandle);
    const char* id = GetStringUTFChars(env, userId);
    SPXHR hr = conversation_update_participant_by_user_id((SPXCONVERSATIONHANDLE)convHandle, false, id);
    ReleaseStringUTFChars(env, userId, id);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_transcription_Conversation
 * Method:    endConversation
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_transcription_Conversation_endConversation
  (JNIEnv *env, jobject obj, jobject conversationHandle)
{
    jlong convHandle = GetObjectHandle(env, conversationHandle);
    SPXHR hr = conversation_end_conversation((SPXCONVERSATIONHANDLE)convHandle);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_transcription_Conversation
 * Method:    getPropertyBag
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/IntRef;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_transcription_Conversation_getPropertyBag
  (JNIEnv *env, jobject obj, jobject conversationHandle, jobject propertyHandle)
{
    SPXPROPERTYBAGHANDLE propHandle = SPXHANDLE_INVALID;
    jlong convHandle = GetObjectHandle(env, conversationHandle);
    SPXHR hr = conversation_get_property_bag((SPXCONVERSATIONHANDLE)convHandle, &propHandle);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, propertyHandle, (jlong)propHandle);
    }
    return (jlong)hr;
}
