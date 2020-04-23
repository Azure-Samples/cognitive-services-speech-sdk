//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
//
#include "com_microsoft_cognitiveservices_speech_transcription_ConversationTranscriptionResult.h"
#include "speechapi_c_conversation_transcription_result.h"
#include "jni_utils.h"

const size_t maxCharCount = 2048;

/*
 * Class:     com_microsoft_cognitiveservices_speech_transcription_ConversationTranscriptionResult
 * Method:    getUserId
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/StringRef;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_transcription_ConversationTranscriptionResult_getUserId
  (JNIEnv *env, jobject obj, jobject resultHandle, jobject userIdStr)
{
    jlong result = GetObjectHandle(env, resultHandle);
    char sz[maxCharCount + 1] = {};
    SPXHR hr = conversation_transcription_result_get_user_id((SPXRESULTHANDLE)result, sz, maxCharCount);
    if (SPX_SUCCEEDED(hr))
    {
        std::string value = std::string(sz);
        SetStringObjectHandle(env, userIdStr, value.c_str());
    }
    return (jlong)hr;
}
