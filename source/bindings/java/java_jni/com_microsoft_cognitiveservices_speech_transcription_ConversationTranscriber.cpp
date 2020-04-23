//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
//
#include "com_microsoft_cognitiveservices_speech_transcription_ConversationTranscriber.h"
#include "speechapi_c_factory.h"
#include "jni_utils.h"

/*
 * Class:     com_microsoft_cognitiveservices_speech_transcription_ConversationTranscriber
 * Method:    createConversationTranscriberFromConfig
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_transcription_ConversationTranscriber_createConversationTranscriberFromConfig
  (JNIEnv* env, jobject obj, jobject recognizerHandle, jobject audioConfigHandle)
{
    SPXRECOHANDLE recoHandle = SPXHANDLE_INVALID;
    jlong audioConfig = GetObjectHandle(env, audioConfigHandle);
    SPXHR hr = recognizer_create_conversation_transcriber_from_config(&recoHandle, (SPXAUDIOCONFIGHANDLE)audioConfig);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, recognizerHandle, (jlong)recoHandle);
    }
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_transcription_ConversationTranscriber
 * Method:    joinConversation
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_transcription_ConversationTranscriber_joinConversation
  (JNIEnv *env, jobject obj, jobject recognizerHandle, jobject conversationHandle)
{
    jlong recoHandle = GetObjectHandle(env, recognizerHandle);
    jlong convHandle = GetObjectHandle(env, conversationHandle);
    SPXHR hr = recognizer_join_conversation((SPXCONVERSATIONHANDLE)convHandle, (SPXRECOHANDLE)recoHandle);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_transcription_ConversationTranscriber
 * Method:    leaveConversation
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_transcription_ConversationTranscriber_leaveConversation
  (JNIEnv *env, jobject obj, jobject recognizerHandle)
{
    jlong recoHandle = GetObjectHandle(env, recognizerHandle);
    SPXHR hr = recognizer_leave_conversation((SPXRECOHANDLE)recoHandle);
    return (jlong)hr;
}
