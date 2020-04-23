//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
//
#include "com_microsoft_cognitiveservices_speech_GrammarList.h"
#include "speechapi_c_grammar.h"
#include "jni_utils.h"

/*
 * Class:     com_microsoft_cognitiveservices_speech_GrammarList
 * Method:    fromRecognizer
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/IntRef;Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_GrammarList_fromRecognizer
  (JNIEnv *env, jclass cls, jobject grammarHandle, jobject recognizerHandle)
{
    SPXGRAMMARHANDLE grammarList = SPXHANDLE_INVALID;
    jlong recoHandle = GetObjectHandle(env, recognizerHandle);
    SPXHR hr = grammar_list_from_recognizer(&grammarList, (SPXRECOHANDLE)recoHandle);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, grammarHandle, (jlong)grammarList);
    }
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_GrammarList
 * Method:    add
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_GrammarList_add
  (JNIEnv *env, jobject obj, jobject grammarHandle, jobject grammarToAdd)
{
    jlong grammarList = GetObjectHandle(env, grammarHandle);
    jlong grammar = GetObjectHandle(env, grammarToAdd);
    SPXHR hr = grammar_list_add_grammar((SPXGRAMMARHANDLE)grammarList, (SPXGRAMMARHANDLE)grammar);
    return (jlong)hr;
}
