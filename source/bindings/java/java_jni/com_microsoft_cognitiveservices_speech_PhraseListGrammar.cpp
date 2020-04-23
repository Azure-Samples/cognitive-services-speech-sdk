//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
//
#include "com_microsoft_cognitiveservices_speech_PhraseListGrammar.h"
#include "speechapi_c_grammar.h"
#include "jni_utils.h"

/*
 * Class:     com_microsoft_cognitiveservices_speech_PhraseListGrammar
 * Method:    fromRecognizer
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/IntRef;Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_PhraseListGrammar_fromRecognizer
  (JNIEnv *env, jclass cls, jobject grammarHandle, jobject recognizerHandle)
{
    SPXGRAMMARHANDLE phraseListGrammar = SPXHANDLE_INVALID;
    jlong recoHandle = GetObjectHandle(env, recognizerHandle);
    SPXHR hr = phrase_list_grammar_from_recognizer_by_name(&phraseListGrammar, (SPXRECOHANDLE)recoHandle, "");
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, grammarHandle, (jlong)phraseListGrammar);
    }
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_PhraseListGrammar
 * Method:    clear
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_PhraseListGrammar_clear
  (JNIEnv *env, jobject obj, jobject grammarHandle)
{
    jlong grammar = GetObjectHandle(env, grammarHandle);
    SPXHR hr = phrase_list_grammar_clear((SPXGRAMMARHANDLE)grammar);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_PhraseListGrammar
 * Method:    addPhrase
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_PhraseListGrammar_addPhrase
  (JNIEnv *env, jobject obj, jobject grammarHandle, jstring phrase)
{
    SPXPHRASEHANDLE phraseHandle = SPXHANDLE_INVALID;
    const char* phraseText = env->GetStringUTFChars(phrase, 0);
    SPXHR hr = grammar_phrase_create_from_text(&phraseHandle, phraseText);
    if (SPX_SUCCEEDED(hr))
    {
        jlong grammar = GetObjectHandle(env, grammarHandle);
        hr = phrase_list_grammar_add_phrase((SPXGRAMMARHANDLE)grammar, phraseHandle);
    }
    env->ReleaseStringUTFChars(phrase, phraseText);
    return (jlong)hr;
}
