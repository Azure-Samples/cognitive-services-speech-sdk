//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
//
#include "com_microsoft_cognitiveservices_speech_transcription_ConversationTranslationResult.h"
#include "speechapi_c_conversation_transcription_result.h"
#include "speechapi_c_conversation_translator.h"
#include "jni_utils.h"

const uint32_t maxCharCount = 2048;

/*
 * Class:     com_microsoft_cognitiveservices_speech_transcription_ConversationTranslationResult
 * Method:    getOriginalLang
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/StringRef;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_transcription_ConversationTranslationResult_getOriginalLang
  (JNIEnv *env, jobject obj, jobject resultHandle, jobject originalLangStrRef)
{
    jlong handle = GetObjectHandle(env, resultHandle);
    uint32_t length = 0;
    SPXHR hr = conversation_translator_result_get_original_lang((SPXRESULTHANDLE)handle, nullptr, &length);
    if (SPX_SUCCEEDED(hr))
    {
        char * sz = (char *) calloc(length, sizeof(char));
        if (sz == NULL) {
            return (jlong)SPXERR_OUT_OF_MEMORY;
        }
        hr = conversation_translator_result_get_original_lang((SPXRESULTHANDLE)handle, sz, &length);
        if (SPX_SUCCEEDED(hr))
        {
            hr = SetStringObjectHandle(env, originalLangStrRef, sz);
        }
        free(sz);
    }
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_transcription_ConversationTranslationResult
 * Method:    getParticipantId
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/StringRef;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_transcription_ConversationTranslationResult_getParticipantId
  (JNIEnv *env, jobject obj, jobject resultHandle, jobject participantIdStrRef)
{
    char sz[maxCharCount + 1] = {};
    jlong handle = GetObjectHandle(env, resultHandle);
    SPXHR hr = conversation_transcription_result_get_user_id((SPXRESULTHANDLE)handle, sz, maxCharCount);
    if (SPX_SUCCEEDED(hr))
    {
        hr = SetStringObjectHandle(env, participantIdStrRef, sz);
    }
    return (jlong)hr;
}
