//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
//
#include <stdio.h>
#include <cstring>
#include "jni_utils.h"
#include <string>
#include <algorithm>
#include <array>
#include <vector>
#include <mutex>
#include <map>
#include <atomic>

JavaVM* g_jvm = nullptr;
static std::atomic<bool> processAttached = ATOMIC_VAR_INIT(false);
static std::map<SPXHANDLE, jobject> objectMap;
static std::mutex objectMutex;

/*
 * Method:    JNI_OnLoad
 * Description: JNI_OnLoad method is called by JNI when JNI dll is loaded using System.LoadLibrary.
 *              https://docs.oracle.com/javase/7/docs/technotes/guides/jni/spec/invocation.html#JNI_OnLoad
 *              Global variable g_jvm is saved here and it is used to get current java environment in callbacks
 */

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* jvm, void*)
{
    JNIEnv* env = nullptr;
    g_jvm = jvm;
    if (jvm->GetEnv((void**)&env, JNI_VERSION_1_6) != JNI_OK)
    {
        return -1;
    }
    return JNI_VERSION_1_6;
}

/*
 * Method:    JNI_OnUnload
 * Description: JNI_OnUnload method is called by JNI when JNI dll is unloaded.
 *              https://docs.oracle.com/javase/7/docs/technotes/guides/jni/spec/invocation.html#JNI_OnUnload
 *              Global variable g_jvm is cleared here
 */
JNIEXPORT void JNICALL JNI_OnUnload(JavaVM*, void*)
{
    g_jvm = nullptr;
}

/*
 * Method:    CheckException
 * Description: CheckException method is checks if JNI call has pending exception, reports and clears it.
 *              It is important to handle expections from JNI calls. Calling arbitrary JNI functions with
 *              a pending exception may lead to unexpected results.
 *              TODO: Consider throwing exception back to Java side.
 */
bool CheckException(JNIEnv* env)
{
    if (env->ExceptionCheck())
    {
        env->ExceptionDescribe();
        env->ExceptionClear();
        return true;
    }
    return false;
}

/*
 * Method:    GetJNIEnvAndAttach
 * Description: Gets Jni environment and attach to current thread if it is not already attached
 *
 */
JNIEnv* GetJNIEnvAndAttach(bool& needToDetach)
{
    JNIEnv* env = nullptr;
    bool attachedHere = false;
    if (g_jvm)
    {
        int getEnvStatus = g_jvm->GetEnv((void **)&env, JNI_VERSION_1_6);
        if (getEnvStatus == JNI_EDETACHED)
        {
#if defined(ANDROID) || defined(__ANDROID__)
            if (g_jvm->AttachCurrentThread(&env, NULL) != 0)
#else
            if (g_jvm->AttachCurrentThread((void **)&env, NULL) != 0)
#endif
            {
                return nullptr;
            }
            attachedHere = true;
        }
        else if (getEnvStatus == JNI_OK)
        {
            // Current thread already attached, do not attach 'again' (just to save the attachedHere flag)
            // We make sure to keep attachedHere = false
        }
        else if (getEnvStatus == JNI_EVERSION)
        {
            return nullptr;
        }
    }
    needToDetach = attachedHere;
    return env;
}

/*
 * Method:    DetachJNIEnv
 * Description: Detach current thread from java VM
 *
 */
void DetachJNIEnv(JNIEnv* env)
{
    if (g_jvm && env)
    {
        g_jvm->DetachCurrentThread();
    }
}

/*
 * Method:    CallbackEventMethod
 * Description: CallbackEventMethod handles callbacks from native side and makes a call to Java object
 *              which is registered for callbacks and matches to the SPXHANDLE given in the context.
 *
 */
void CallbackEventMethod(void *context, const char *method, SPXEVENTHANDLE eventHandle)
{
    jobject obj = GetObjectFromMap(static_cast<SPXHANDLE>(context));

    bool detach = false;
    JNIEnv* env = GetJNIEnvAndAttach(detach);
    if (env && obj)
    {
        jclass cl = env->GetObjectClass(obj);
        if (CheckException(env))
        {
            goto Exit;
        }
        jmethodID m = env->GetMethodID(cl, method, "(J)V");
        if (CheckException(env))
        {
            goto Exit;
        }
        env->CallVoidMethod(obj, m, (jlong)eventHandle);
        if (CheckException(env))
        {
            goto Exit;
        }
    }

Exit:
    if (detach)
    {
        DetachJNIEnv(env);
    }
    return;
}

/*
 * Method:    GetObjectFromMap
 * Description: GetObjectFromMap gets Java object reference from the objectMap registered in the callback setting phase using SPXHANDLE as key.
 *              If process is not attached (Java object is being disposed from context NULL is returned.
 */
jobject GetObjectFromMap(SPXHANDLE handle)
{
    std::lock_guard<std::mutex> lock(objectMutex);

    if (!processAttached) return NULL;

    auto it = objectMap.find(handle);
    if (it != objectMap.end())
    {
        jobject objFound = it->second;
        return objFound;
    }
    return nullptr;
}

/*
 * Method:    AddGlobalReferenceForHandle
 * Description: AddGlobalReferenceForHandle adds global weak reference to Java object (requiring callback registeration) to objectMap
 *              This is needed for callbacks from native to managed. Without global reference jobject is valid only during each jni call.
 *
 */
void AddGlobalReferenceForHandle(JNIEnv *env, jobject obj, SPXHANDLE handle)
{
    std::lock_guard<std::mutex> lock(objectMutex);
    // if there is no object reference for handle, add it
    if (objectMap.find(handle) == objectMap.end())
    {
        jobject objRef = env->NewWeakGlobalRef(obj);
        objectMap.insert(std::pair<SPXHANDLE, jobject>(handle, objRef));
    }
}

/*
 * Method:    RemoveGlobalReferenceFromHandle
 * Description: RemoveGlobalReferenceFromHandle removes the global weak reference from the objectMap.
 *              This is done in closing phase when resources are disposed.
 *
 */
void RemoveGlobalReferenceFromHandle(JNIEnv *env, SPXHANDLE handle)
{
    std::lock_guard<std::mutex> lock(objectMutex);
    auto it = objectMap.find(handle);
    if (it != objectMap.end())
    {
        env->DeleteWeakGlobalRef(it->second);
        objectMap.erase(handle);
    }
}

/*
 * Method:    SetProcessState
 * Description: SetProcessState tells if process is attached (true) or detached (false)
 *
 */
void SetProcessState(bool state)
{
    std::lock_guard<std::mutex> lock(objectMutex);
    processAttached = state;
}

/*
 * Method:    SetObjectHandle
 * Description: SetObjectHandle is used in case of IntRef or SafeHandle objects to set value for Java object from jni.
 *
 */
jint SetObjectHandle(JNIEnv *env, jobject objHandle, jlong handleValue)
{
    jclass cl = env->GetObjectClass(objHandle);
    if (CheckException(env))
    {
        return -1;
    }
    jmethodID m = env->GetMethodID(cl, "setValue", "(J)V");
    if (CheckException(env))
    {
        return -1;
    }
    env->CallVoidMethod(objHandle, m, handleValue);
    if (CheckException(env))
    {
        return -1;
    }
    return 0;
}

/*
 * Method:    GetObjectHandle
 * Description: GetObjectHandle gets the value (long) of IntRef or SafeHandle Java object.
 *
 */
jlong GetObjectHandle(JNIEnv *env, jobject objHandle)
{
    if (objHandle == NULL)
    {
        return 0;
    }
    jclass cl = env->GetObjectClass(objHandle);
    if (CheckException(env))
    {
        return 0;
    }
    jmethodID m = env->GetMethodID(cl, "getValue", "()J");
    if (CheckException(env))
    {
        return 0;
    }
    jlong handle = env->CallLongMethod(objHandle, m);
    if (CheckException(env))
    {
        return 0;
    }
    return handle;
}

/*
 * Method:    GetStringObjectHandle
 * Description: GetStringObjectHandle gets the value (string) of StringRef Java object.
 *
 */
jstring GetStringObjectHandle(JNIEnv *env, jobject objHandle)
{
    std::string result = "";
    jclass cl = env->GetObjectClass(objHandle);
    if (CheckException(env))
    {
        return env->NewStringUTF(result.c_str());
    }
    jmethodID m = env->GetMethodID(cl, "getValue", "()Ljava/lang/String;");
    if (CheckException(env))
    {
        return env->NewStringUTF(result.c_str());
    }
    jstring handle = (jstring)env->CallObjectMethod(objHandle, m);
    if (CheckException(env))
    {
        return env->NewStringUTF(result.c_str());
    }
    return handle;
}

/*
 * Method:    SetStringObjectHandle
 * Description: SetStringObjectHandle sets the value (string) of StringRef Java object.
 *
 */
SPXHR SetStringObjectHandle(JNIEnv *env, jobject objHandle, const char *value)
{
    jclass cl = env->GetObjectClass(objHandle);
    if (CheckException(env))
    {
        return SPXERR_RUNTIME_ERROR;
    }
    jmethodID m = env->GetMethodID(cl, "setValue", "(Ljava/lang/String;)V");
    if (CheckException(env))
    {
        return SPXERR_RUNTIME_ERROR;
    }
    jstring val = env->NewStringUTF(value);
    if (val == NULL)
    {
        return SPXERR_OUT_OF_MEMORY;
    }
    env->CallVoidMethod(objHandle, m, val);
    if (CheckException(env))
    {
        return SPXERR_RUNTIME_ERROR;
    }
    return SPX_NOERROR;
}

/*
 * Method:    SetStringMapObjectHandle
 * Description: SetStringMapObjectHandle sets the value (Map<string key, string value>) of Java object.
 *
 */
SPXHR SetStringMapObjectHandle(JNIEnv* env, jobject objHandle, const char* key, const char* value)
{
    jclass cl = env->GetObjectClass(objHandle);
    if (CheckException(env))
    {
        return SPXERR_RUNTIME_ERROR;
    }
    jmethodID m = env->GetMethodID(cl, "setValue", "(Ljava/lang/String;Ljava/lang/String;)V");
    if (CheckException(env))
    {
        return SPXERR_RUNTIME_ERROR;
    }
    jstring k = env->NewStringUTF(key);
    if (k == NULL)
    {
        return SPXERR_OUT_OF_MEMORY;
    }
    jstring val = env->NewStringUTF(value);
    if (val == NULL)
    {
        return SPXERR_OUT_OF_MEMORY;
    }
    env->CallVoidMethod(objHandle, m, k, val);
    if (CheckException(env))
    {
        return SPXERR_RUNTIME_ERROR;
    }
    return SPX_NOERROR;
}

/*
 * Method:    GetStringUTFChars
 * Description: GetStringUTFChars gets the const char string from jstring object.
 *              ReleaseStringUTFChars needs to be called to release the string.
 *
 */
const char* GetStringUTFChars(JNIEnv* env, jstring jString)
{
    const char* result = NULL;
    if (jString != NULL)
    {
        result = env->GetStringUTFChars(jString, 0);
        if (CheckException(env))
        {
            if (result != NULL)
            {
                env->ReleaseStringUTFChars(jString, result);
            }
            return NULL;
        }
    }
    return result;
}

/*
 * Method:    ReleaseStringUTFChars
 * Description: ReleaseStringUTFChars releases the string.
 *
 */
void ReleaseStringUTFChars(JNIEnv* env, jstring jString, const char *cString)
{
    if (jString != NULL && cString != NULL)
    {
        env->ReleaseStringUTFChars(jString, cString);
    }
}

/*
 * Method:    AsBigInteger
 * Description: AsBigInteger converts uint64_t value to Java BigInteger object.
 *
 */
jobject AsBigInteger(JNIEnv *env, uint64_t value)
{
    auto ba = env->NewByteArray(sizeof(uint64_t));
    auto guard = LocalRefGuard(env, ba);
    if (CheckException(env))
    {
        return nullptr;
    }
    std::array<jbyte, sizeof(uint64_t)> buffer{};
    std::memcpy(buffer.data(), reinterpret_cast<int8_t*>(&value), sizeof(uint64_t));
    std::reverse(buffer.begin(), buffer.end());
    env->SetByteArrayRegion(ba, 0, sizeof(uint64_t), buffer.data());
    if (CheckException(env))
    {
        return nullptr;
    }
    auto clazz = env->FindClass("java/math/BigInteger");
    if (CheckException(env))
    {
        return nullptr;
    }
    auto mid = env->GetMethodID(clazz, "<init>", "([B)V");
    if (CheckException(env))
    {
        return nullptr;
    }
    auto jresult = env->NewObject(clazz, mid, ba);
    if (CheckException(env))
    {
        return nullptr;
    }
    return jresult;
}

/*
 * Method:    RecognizingCallback
 * Description: RecognizingCallback method which calls recognizingEventCallback method on Java object matching to context.
 *
 */
void RecognizingCallback(SPXRECOHANDLE recoHandle, SPXEVENTHANDLE eventHandle, void* context)
{
    CallbackEventMethod(context, "recognizingEventCallback", eventHandle);
}

/*
 * Method:    RecognizedCallback
 * Description: RecognizedCallback method which calls recognizedEventCallback method on Java object matching to context.
 *
 */
void RecognizedCallback(SPXRECOHANDLE recoHandle, SPXEVENTHANDLE eventHandle, void* context)
{
    CallbackEventMethod(context, "recognizedEventCallback", eventHandle);
}

/*
 * Method:    CanceledCallback
 * Description: CanceledCallback method which calls canceledEventCallback method on Java object matching to context.
 *
 */
void CanceledCallback(SPXRECOHANDLE recoHandle, SPXEVENTHANDLE eventHandle, void* context)
{
    CallbackEventMethod(context, "canceledEventCallback", eventHandle);
}

/*
 * Method:    SessionStartedCallback
 * Description: SessionStartedCallback method which calls sessionStartedEventCallback method on Java object matching to context.
 *
 */
void SessionStartedCallback(SPXRECOHANDLE recoHandle, SPXEVENTHANDLE eventHandle, void* context)
{
    CallbackEventMethod(context, "sessionStartedEventCallback", eventHandle);
}

/*
 * Method:    SessionStoppedCallback
 * Description: SessionStoppedCallback method which calls sessionStoppedEventCallback method on Java object matching to context.
 *
 */
void SessionStoppedCallback(SPXRECOHANDLE recoHandle, SPXEVENTHANDLE eventHandle, void* context)
{
    CallbackEventMethod(context, "sessionStoppedEventCallback", eventHandle);
}

/*
 * Method:    SpeechStartDetectedCallback
 * Description: SpeechStartDetectedCallback method which calls speechStartDetectedEventCallback method on Java object matching to context.
 *
 */
void SpeechStartDetectedCallback(SPXRECOHANDLE recoHandle, SPXEVENTHANDLE eventHandle, void* context)
{
    CallbackEventMethod(context, "speechStartDetectedEventCallback", eventHandle);
}

/*
 * Method:    SpeechEndDetectedCallback
 * Description: SpeechEndDetectedCallback method which calls speechEndDetectedEventCallback method on Java object matching to context.
 *
 */
void SpeechEndDetectedCallback(SPXRECOHANDLE recoHandle, SPXEVENTHANDLE eventHandle, void* context)
{
    CallbackEventMethod(context, "speechEndDetectedEventCallback", eventHandle);
}

void ParticipantsChangedCallback(SPXCONVERSATIONTRANSLATORHANDLE conversationTranslatorHandle, SPXEVENTHANDLE eventHandle, void* context)
{
    CallbackEventMethod(context, "participantsChangedEventCallback", eventHandle);
}

void ConversationExpireCallback(SPXCONVERSATIONTRANSLATORHANDLE conversationTranslatorHandle, SPXEVENTHANDLE eventHandle, void* context)
{
    CallbackEventMethod(context, "conversationExpireEventCallback", eventHandle);
}

void TranscribingCallback(SPXCONVERSATIONTRANSLATORHANDLE conversationTranslatorHandle, SPXEVENTHANDLE eventHandle, void* context)
{
    CallbackEventMethod(context, "transcribingEventCallback", eventHandle);
}

void TranscribedCallback(SPXCONVERSATIONTRANSLATORHANDLE conversationTranslatorHandle, SPXEVENTHANDLE eventHandle, void* context)
{
    CallbackEventMethod(context, "transcribedEventCallback", eventHandle);
}

void TextMessageCallback(SPXCONVERSATIONTRANSLATORHANDLE conversationTranslatorHandle, SPXEVENTHANDLE eventHandle, void* context)
{
    CallbackEventMethod(context, "textMessageEventCallback", eventHandle);
}
