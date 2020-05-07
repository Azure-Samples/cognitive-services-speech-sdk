//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
//
#include "com_microsoft_cognitiveservices_speech_transcription_Participant.h"
#include "speechapi_c_participant.h"
#include "speechapi_c_conversation_translator.h"
#include "jni_utils.h"

/*
 * Class:     com_microsoft_cognitiveservices_speech_transcription_Participant
 * Method:    createParticipantHandle
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/IntRef;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_transcription_Participant_createParticipantHandle
  (JNIEnv *env, jclass cls, jobject participantHandle, jstring userId, jstring preferredLanguage, jstring voiceSignature)
{
    SPXPARTICIPANTHANDLE participant = SPXHANDLE_INVALID;
    const char* id = GetStringUTFChars(env, userId);
    const char* language = GetStringUTFChars(env, preferredLanguage);
    const char* voice_signature = GetStringUTFChars(env, voiceSignature);
    SPXHR hr = participant_create_handle(&participant, id, language, voice_signature);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, participantHandle, (jlong)participant);
    }
    ReleaseStringUTFChars(env, userId, id);
    ReleaseStringUTFChars(env, preferredLanguage, language);
    ReleaseStringUTFChars(env, voiceSignature, voice_signature);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_transcription_Participant
 * Method:    getPropertyBag
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/IntRef;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_transcription_Participant_getPropertyBag
  (JNIEnv *env, jobject obj, jobject participantHandle, jobject propertyHandle)
{
    SPXPROPERTYBAGHANDLE propbag = SPXHANDLE_INVALID;
    jlong participant = GetObjectHandle(env, participantHandle);
    SPXHR hr = participant_get_property_bag((SPXPARTICIPANTHANDLE)participant, &propbag);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, propertyHandle, (jlong)propbag);
    }
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_transcription_Participant
 * Method:    setPreferredLanguage
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_transcription_Participant_setPreferredLanguage
  (JNIEnv *env, jobject obj, jobject participantHandle, jstring preferredLanguage)
{
    jlong participant = GetObjectHandle(env, participantHandle);
    const char* language = GetStringUTFChars(env, preferredLanguage);
    SPXHR hr = participant_set_preferred_langugage((SPXPARTICIPANTHANDLE)participant, language);
    ReleaseStringUTFChars(env, preferredLanguage, language);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_transcription_Participant
 * Method:    setVoiceSignature
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_transcription_Participant_setVoiceSignature
  (JNIEnv *env, jobject obj, jobject participantHandle, jstring voiceSignature)
{
    jlong participant = GetObjectHandle(env, participantHandle);
    const char* voice_signature = GetStringUTFChars(env, voiceSignature);
    SPXHR hr = participant_set_voice_signature((SPXPARTICIPANTHANDLE) participant, voice_signature);
    ReleaseStringUTFChars(env, voiceSignature, voice_signature);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_transcription_Participant
 * Method:    getUserId
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/StringRef;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_transcription_Participant_getUserId
  (JNIEnv *env, jobject obj, jobject participantHandle, jobject userIdRef)
{
    jlong handle = GetObjectHandle(env, participantHandle);
    uint32_t length = 0;
    SPXHR hr = conversation_translator_participant_get_id((SPXRESULTHANDLE)handle, nullptr, &length);
    if (SPX_SUCCEEDED(hr))
    {
        char * sz = (char *) calloc(length, sizeof(char));
        if (sz == NULL) {
            return (jlong)SPXERR_OUT_OF_MEMORY;
        }
        hr = conversation_translator_participant_get_id((SPXRESULTHANDLE)handle, sz, &length);
        if (SPX_SUCCEEDED(hr))
        {
            hr = SetStringObjectHandle(env, userIdRef, sz);
        }
        free(sz);
    }
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_transcription_Participant
 * Method:    getDisplayName
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/StringRef;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_transcription_Participant_getDisplayName
  (JNIEnv *env, jobject obj, jobject participantHandle, jobject userIdRef)
{
    jlong handle = GetObjectHandle(env, participantHandle);
    uint32_t length = 0;
    SPXHR hr = conversation_translator_participant_get_displayname((SPXRESULTHANDLE)handle, nullptr, &length);
    if (SPX_SUCCEEDED(hr))
    {
        char * sz = (char *) calloc(length, sizeof(char));
        if (sz == NULL) {
            return (jlong)SPXERR_OUT_OF_MEMORY;
        }
        hr = conversation_translator_participant_get_displayname((SPXRESULTHANDLE)handle, sz, &length);
        if (SPX_SUCCEEDED(hr))
        {
            hr = SetStringObjectHandle(env, userIdRef, sz);
        }
        free(sz);
    }
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_transcription_Participant
 * Method:    getAvatar
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/StringRef;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_transcription_Participant_getAvatar
  (JNIEnv *env, jobject obj, jobject participantHandle, jobject userIdRef)
{
    jlong handle = GetObjectHandle(env, participantHandle);
    uint32_t length = 0;
    SPXHR hr = conversation_translator_participant_get_avatar((SPXRESULTHANDLE)handle, nullptr, &length);
    if (SPX_SUCCEEDED(hr))
    {
        char * sz = (char *) calloc(length, sizeof(char));
        if (sz == NULL) {
            return (jlong)SPXERR_OUT_OF_MEMORY;
        }
        hr = conversation_translator_participant_get_avatar((SPXRESULTHANDLE)handle, sz, &length);
        if (SPX_SUCCEEDED(hr))
        {
            hr = SetStringObjectHandle(env, userIdRef, sz);
        }
        free(sz);
    }
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_transcription_Participant
 * Method:    getIsHost
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_microsoft_cognitiveservices_speech_transcription_Participant_getIsHost
  (JNIEnv *env, jobject obj, jobject handle)
{
    jlong event = GetObjectHandle(env, handle);
    bool booleanValue;
    conversation_translator_participant_get_is_host((SPXEVENTHANDLE)event, &booleanValue);
    return (jboolean)booleanValue;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_transcription_Participant
 * Method:    getIsMuted
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_microsoft_cognitiveservices_speech_transcription_Participant_getIsMuted
  (JNIEnv *env, jobject obj, jobject handle)
{
    jlong event = GetObjectHandle(env, handle);
    bool booleanValue;
    conversation_translator_participant_get_is_muted((SPXEVENTHANDLE)event, &booleanValue);
    return (jboolean)booleanValue;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_transcription_Participant
 * Method:    getIsUsingTts
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_microsoft_cognitiveservices_speech_transcription_Participant_getIsUsingTts
  (JNIEnv *env, jobject obj, jobject handle)
{
    jlong event = GetObjectHandle(env, handle);
    bool booleanValue;
    conversation_translator_participant_get_is_using_tts((SPXEVENTHANDLE)event, &booleanValue);
    return (jboolean)booleanValue;
}
