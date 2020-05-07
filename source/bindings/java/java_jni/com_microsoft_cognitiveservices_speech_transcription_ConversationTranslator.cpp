//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
//
#include "com_microsoft_cognitiveservices_speech_transcription_ConversationTranslator.h"
#include "speechapi_c_factory.h"
#include "speechapi_c_conversation_translator.h"
#include "jni_utils.h"

/*
 * Class:     com_microsoft_cognitiveservices_speech_transcription_ConversationTranslator
 * Method:    createConversationTranslatorFromConfig
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_transcription_ConversationTranslator_createConversationTranslatorFromConfig
  (JNIEnv* env, jobject jobj, jobject conversationTranslatorHandle, jobject audioConfigHandle)
{
    SPXCONVERSATIONTRANSLATORHANDLE convTransHandle = SPXHANDLE_INVALID;
    jlong audioConfig = GetObjectHandle(env, audioConfigHandle);
    SPXHR hr = conversation_translator_create_from_config(&convTransHandle, (SPXAUDIOCONFIGHANDLE)audioConfig);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, conversationTranslatorHandle, (jlong)convTransHandle);
    }
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_transcription_ConversationTranslator
 * Method:    getPropertyBagFromHandle
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/IntRef;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_transcription_ConversationTranslator_getPropertyBagFromHandle
  (JNIEnv* env, jobject obj, jobject conversationTranslatorHandle, jobject propertyHandle)
{
    SPXPROPERTYBAGHANDLE propHandle = SPXHANDLE_INVALID;
    jlong convTransHandle = GetObjectHandle(env, conversationTranslatorHandle);
    SPXHR hr = conversation_translator_get_property_bag((SPXRECOHANDLE)convTransHandle, &propHandle);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, propertyHandle, (jlong)propHandle);
    }
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_transcription_ConversationTranslator
 * Method:    joinConversation
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_transcription_ConversationTranslator_joinConversation
  (JNIEnv *env, jobject obj, jobject conversationTranslatorHandle, jobject conversationHandle, jstring value)
{
    jlong convTransHandle = GetObjectHandle(env, conversationTranslatorHandle);
    jlong convHandle = GetObjectHandle(env, conversationHandle);
    const char* nickname = GetStringUTFChars(env, value);
    SPXHR hr = conversation_translator_join((SPXCONVERSATIONTRANSLATORHANDLE)convTransHandle, (SPXCONVERSATIONHANDLE)convHandle, nickname);
    ReleaseStringUTFChars(env, value, nickname);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_transcription_ConversationTranslator
 * Method:    joinConversationId
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_transcription_ConversationTranslator_joinConversationId
  (JNIEnv *env, jobject obj, jobject conversationTranslatorHandle, jstring conversationIdValue, jstring nameValue, jstring langValue)
{
    jlong convTransHandle = GetObjectHandle(env, conversationTranslatorHandle);
    const char* conversationId = GetStringUTFChars(env, conversationIdValue);
    const char* nickname = GetStringUTFChars(env, nameValue);
    const char* language = GetStringUTFChars(env, langValue);
    SPXHR hr = conversation_translator_join_with_id((SPXCONVERSATIONTRANSLATORHANDLE)convTransHandle, conversationId, nickname, language);
    ReleaseStringUTFChars(env, conversationIdValue, conversationId);
    ReleaseStringUTFChars(env, nameValue, nickname);
    ReleaseStringUTFChars(env, langValue, language);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_transcription_ConversationTranslator
 * Method:    leaveConversation
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_transcription_ConversationTranslator_leaveConversation
  (JNIEnv *env, jobject obj, jobject conversationTranslatorHandle)
{
    jlong convTransHandle = GetObjectHandle(env, conversationTranslatorHandle);
    SPXHR hr = conversation_translator_leave((SPXCONVERSATIONHANDLE)convTransHandle);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_transcription_ConversationTranslator
 * Method:    sendText
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_transcription_ConversationTranslator_sendText
  (JNIEnv *env, jobject obj, jobject conversationTranslatorHandle, jstring value)
{
    jlong convTransHandle = GetObjectHandle(env, conversationTranslatorHandle);
    const char* text = GetStringUTFChars(env, value);
    SPXHR hr = conversation_translator_send_text_message((SPXCONVERSATIONHANDLE)convTransHandle, text);
    ReleaseStringUTFChars(env, value, text);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_transcription_ConversationTranslator
 * Method:    startTranscribing
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_transcription_ConversationTranslator_startTranscribing
  (JNIEnv* env, jobject obj, jobject conversationTranslatorHandle)
{
    jlong convTransHandle = GetObjectHandle(env, conversationTranslatorHandle);
    jlong hr = (jlong) conversation_translator_start_transcribing((SPXCONVERSATIONTRANSLATORHANDLE) convTransHandle);
    return hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_transcription_ConversationTranslator
 * Method:    stopTranscribing
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_transcription_ConversationTranslator_stopTranscribing
  (JNIEnv *env, jobject obj, jobject conversationTranslatorHandle)
{
    jlong convTransHandle = GetObjectHandle(env, conversationTranslatorHandle);
    jlong hr = (jlong) conversation_translator_stop_transcribing((SPXCONVERSATIONTRANSLATORHANDLE)convTransHandle);
    return hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_transcription_ConversationTranslator
 * Method:    sessionStartedSetCallback
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_transcription_ConversationTranslator_sessionStartedSetCallback
  (JNIEnv *env, jobject obj, jlong conversationTranslatorHandle)
{
    AddGlobalReferenceForHandle(env, obj, (SPXHANDLE)conversationTranslatorHandle);
    SPXHR hr = conversation_translator_session_started_set_callback((SPXCONVERSATIONTRANSLATORHANDLE)conversationTranslatorHandle, SessionStartedCallback, (SPXHANDLE)conversationTranslatorHandle);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_transcription_ConversationTranslator
 * Method:    sessionStoppedSetCallback
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_transcription_ConversationTranslator_sessionStoppedSetCallback
  (JNIEnv *env, jobject obj, jlong conversationTranslatorHandle)
{
    AddGlobalReferenceForHandle(env, obj, (SPXHANDLE)conversationTranslatorHandle);
    SPXHR hr = conversation_translator_session_stopped_set_callback((SPXRECOHANDLE)conversationTranslatorHandle, SessionStoppedCallback, (SPXHANDLE)conversationTranslatorHandle);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_transcription_ConversationTranslator
 * Method:    canceledSetCallback
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_transcription_ConversationTranslator_canceledSetCallback
  (JNIEnv *env, jobject obj, jlong conversationTranslatorHandle)
{
    AddGlobalReferenceForHandle(env, obj, (SPXHANDLE)conversationTranslatorHandle);
    SPXHR hr = conversation_translator_canceled_set_callback((SPXCONVERSATIONTRANSLATORHANDLE)conversationTranslatorHandle, CanceledCallback, (SPXHANDLE)conversationTranslatorHandle);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_transcription_ConversationTranslator
 * Method:    participantsChangedSetCallback
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_transcription_ConversationTranslator_participantsChangedSetCallback
  (JNIEnv *env, jobject obj, jlong conversationTranslatorHandle)
{
    AddGlobalReferenceForHandle(env, obj, (SPXHANDLE)conversationTranslatorHandle);
    SPXHR hr = conversation_translator_participants_changed_set_callback((SPXCONVERSATIONTRANSLATORHANDLE)conversationTranslatorHandle, ParticipantsChangedCallback, (SPXHANDLE)conversationTranslatorHandle);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_transcription_ConversationTranslator
 * Method:    conversationExpireSetCallback
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_transcription_ConversationTranslator_conversationExpireSetCallback
  (JNIEnv *env, jobject obj, jlong conversationTranslatorHandle)
{
    AddGlobalReferenceForHandle(env, obj, (SPXHANDLE)conversationTranslatorHandle);
    SPXHR hr = conversation_translator_conversation_expiration_set_callback((SPXCONVERSATIONTRANSLATORHANDLE)conversationTranslatorHandle, ConversationExpireCallback, (SPXHANDLE)conversationTranslatorHandle);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_transcription_ConversationTranslator
 * Method:    transcribingSetCallback
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_transcription_ConversationTranslator_transcribingSetCallback
  (JNIEnv *env, jobject obj, jlong conversationTranslatorHandle)
{
    AddGlobalReferenceForHandle(env, obj, (SPXHANDLE)conversationTranslatorHandle);
    SPXHR hr = conversation_translator_transcribing_set_callback((SPXCONVERSATIONTRANSLATORHANDLE)conversationTranslatorHandle, TranscribingCallback, (SPXHANDLE)conversationTranslatorHandle);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_transcription_ConversationTranslator
 * Method:    transcribedSetCallback
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_transcription_ConversationTranslator_transcribedSetCallback
  (JNIEnv *env, jobject obj, jlong conversationTranslatorHandle)
{
    AddGlobalReferenceForHandle(env, obj, (SPXHANDLE)conversationTranslatorHandle);
    SPXHR hr = conversation_translator_transcribed_set_callback((SPXCONVERSATIONTRANSLATORHANDLE)conversationTranslatorHandle, TranscribedCallback, (SPXHANDLE)conversationTranslatorHandle);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_transcription_ConversationTranslator
 * Method:    textMessageSetCallback
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_transcription_ConversationTranslator_textMessageSetCallback
  (JNIEnv *env, jobject obj, jlong conversationTranslatorHandle)
{
    AddGlobalReferenceForHandle(env, obj, (SPXHANDLE)conversationTranslatorHandle);
    SPXHR hr = conversation_translator_text_message_recevied_set_callback((SPXCONVERSATIONTRANSLATORHANDLE)conversationTranslatorHandle, TextMessageCallback, (SPXHANDLE)conversationTranslatorHandle);
    return (jlong)hr;
}
