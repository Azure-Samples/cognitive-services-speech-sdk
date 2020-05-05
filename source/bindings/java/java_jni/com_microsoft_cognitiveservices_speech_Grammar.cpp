//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
//
#include "com_microsoft_cognitiveservices_speech_Grammar.h"
#include "speechapi_c_grammar.h"
#include "jni_utils.h"

/*
 * Class:     com_microsoft_cognitiveservices_speech_Grammar
 * Method:    fromStorageId
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/IntRef;Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_Grammar_fromStorageId
  (JNIEnv *env, jclass cls, jobject grammarHandle, jstring storageId)
{
    SPXGRAMMARHANDLE grammar = SPXHANDLE_INVALID;
    const char* id = GetStringUTFChars(env, storageId);
    SPXHR hr = grammar_create_from_storage_id(&grammar, id);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, grammarHandle, (jlong)grammar);
    }
    ReleaseStringUTFChars(env, storageId, id);
    return (jlong)hr;
}
