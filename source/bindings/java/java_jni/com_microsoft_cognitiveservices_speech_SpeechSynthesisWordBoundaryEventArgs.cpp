//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
//
#include "com_microsoft_cognitiveservices_speech_SpeechSynthesisWordBoundaryEventArgs.h"
#include "speechapi_c_synthesizer.h"
#include "jni_utils.h"

/*
 * Class:     com_microsoft_cognitiveservices_speech_SpeechSynthesisWordBoundaryEventArgs
 * Method:    getWordBoundaryEventValues
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/IntRef;Lcom/microsoft/cognitiveservices/speech/util/IntRef;Lcom/microsoft/cognitiveservices/speech/util/IntRef;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_SpeechSynthesisWordBoundaryEventArgs_getWordBoundaryEventValues
  (JNIEnv *env, jobject obj, jobject eventHandle, jobject audioOffsetRef, jobject textOffsetRef, jobject wordLengthRef)
{
    jlong event = GetObjectHandle(env, eventHandle);
    uint64_t audioOffset = 0;
    uint32_t textOffset = 0;
    uint32_t wordLength = 0;
    SPXHR hr = synthesizer_word_boundary_event_get_values((SPXEVENTHANDLE)event, &audioOffset, &textOffset, &wordLength);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, audioOffsetRef, (jlong)audioOffset);
        SetObjectHandle(env, textOffsetRef, (jlong)textOffset);
        SetObjectHandle(env, wordLengthRef, (jlong)wordLength);
    }
    return (jlong)hr;
}
