//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
//
#include "com_microsoft_cognitiveservices_speech_transcription_Participant.h"
#include "speechapi_c_participant.h"
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
