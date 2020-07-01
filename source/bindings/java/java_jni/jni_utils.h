//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
//
#pragma once
#include <jni.h>
#include "speechapi_c_common.h"
#include <string>

#if defined(ANDROID) || defined(__ANDROID__)
#include "azure_c_shared_utility/xlogging.h"
#endif

jint SetObjectHandle(JNIEnv *env, jobject objHandle, jlong handleValue);

jlong GetObjectHandle(JNIEnv *env, jobject objHandle);

jstring GetStringObjectHandle(JNIEnv *env, jobject objHandle);

SPXHR SetStringObjectHandle(JNIEnv *env, jobject objHandle, const char* value);

SPXHR SetStringMapObjectHandle(JNIEnv *env, jobject objHandle, const char *key, const char *value);

const char* GetStringUTFChars(JNIEnv *env, jstring jString);

void ReleaseStringUTFChars(JNIEnv *env, jstring jString, const char *cString);

jobject AsBigInteger(JNIEnv *env, uint64_t value);

jobject GetObjectFromMap(SPXHANDLE handle);

void AddGlobalReferenceForHandle(JNIEnv *env, jobject obj, SPXHANDLE handle);

void RemoveGlobalReferenceFromHandle(JNIEnv *env, SPXHANDLE handle);

void CallbackEventMethod(void* context, const char *method, SPXEVENTHANDLE eventHandle);

JNIEnv* GetJNIEnvAndAttach(bool& needToDetach);

void DetachJNIEnv(JNIEnv* env);

bool CheckException(JNIEnv* env);

void RecognizingCallback(SPXRECOHANDLE recoHandle, SPXEVENTHANDLE eventHandle, void* context);

void RecognizedCallback(SPXRECOHANDLE recoHandle, SPXEVENTHANDLE eventHandle, void* context);

void CanceledCallback(SPXRECOHANDLE recoHandle, SPXEVENTHANDLE eventHandle, void* context);

void SessionStartedCallback(SPXRECOHANDLE recoHandle, SPXEVENTHANDLE eventHandle, void* context);

void SessionStoppedCallback(SPXRECOHANDLE recoHandle, SPXEVENTHANDLE eventHandle, void* context);

void SpeechStartDetectedCallback(SPXRECOHANDLE recoHandle, SPXEVENTHANDLE eventHandle, void* context);

void SpeechEndDetectedCallback(SPXRECOHANDLE recoHandle, SPXEVENTHANDLE eventHandle, void* context);

void ParticipantsChangedCallback(SPXCONVERSATIONTRANSLATORHANDLE conversationTranslatorHandle, SPXEVENTHANDLE eventHandle, void* context);

void ConversationExpireCallback(SPXCONVERSATIONTRANSLATORHANDLE conversationTranslatorHandle, SPXEVENTHANDLE eventHandle, void* context);

void TranscribingCallback(SPXCONVERSATIONTRANSLATORHANDLE conversationTranslatorHandle, SPXEVENTHANDLE eventHandle, void* context);

void TranscribedCallback(SPXCONVERSATIONTRANSLATORHANDLE conversationTranslatorHandle, SPXEVENTHANDLE eventHandle, void* context);

void TextMessageCallback(SPXCONVERSATIONTRANSLATORHANDLE conversationTranslatorHandle, SPXEVENTHANDLE eventHandle, void* context);

void SetProcessState(bool state);

/*! \cond PRIVATE */
struct LocalRefGuard
{
    inline LocalRefGuard(JNIEnv* env, jobject obj): m_env{env}, m_obj{obj}
    {}

    inline ~LocalRefGuard()
    {
        if (m_obj != nullptr)
        {
            m_env->DeleteLocalRef(m_obj);
        }
    }
private:
    JNIEnv* m_env;
    jobject m_obj;
};
/*! \endcond */
