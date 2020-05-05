//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
//
#include "com_microsoft_cognitiveservices_speech_transcription_User.h"
#include "speechapi_c_user.h"
#include "jni_utils.h"

const size_t maxCharCount = 1024;

/*
 * Class:     com_microsoft_cognitiveservices_speech_transcription_User
 * Method:    createFromUserId
 * Signature: (Ljava/lang/String;Lcom/microsoft/cognitiveservices/speech/util/IntRef;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_transcription_User_createFromUserId
  (JNIEnv *env, jclass cls, jstring userId, jobject userHandle)
{
    SPXUSERHANDLE user = SPXHANDLE_INVALID;
    const char* id = GetStringUTFChars(env, userId);
    SPXHR hr = user_create_from_id(id, &user);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, userHandle, (jlong)user);
    }
    ReleaseStringUTFChars(env, userId, id);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_transcription_User
 * Method:    getId
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/StringRef;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_transcription_User_getId
  (JNIEnv *env, jobject obj, jobject userHandle, jobject idRef)
{
    char sz[maxCharCount + 1] = {};
    jlong user = GetObjectHandle(env, userHandle);
    SPXHR hr = user_get_id((SPXUSERHANDLE)user, sz, maxCharCount);
    if (SPX_SUCCEEDED(hr))
    {
        hr = SetStringObjectHandle(env, idRef, sz);
    }
    return (jlong)hr;
}
