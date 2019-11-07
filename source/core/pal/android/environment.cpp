//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
#include "environment.h"

#include <memory>
#include <exception>

#include <pthread.h>

static JavaVM* g_vm = nullptr;

pthread_key_t t_curr_env;

static void detach_current_thread(void*)
{
    g_vm->DetachCurrentThread();
}

static JNIEnv* attach_current_thread()
{
    JNIEnv* env = NULL;
    g_vm->AttachCurrentThread(&env, NULL);
    return env;
}

JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void*)
{
    JNIEnv* env;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK)
    {
        return -1;
    }
    g_vm = vm;
    pthread_key_create(&t_curr_env, detach_current_thread);
    return JNI_VERSION_1_6;
}

JNIEXPORT void JNI_OnUnload(JavaVM*, void*)
{
    g_vm = nullptr;
}

JavaVM* GetVM() noexcept
{
    return g_vm;
}

JNIEnv* GetEnvironment()
{
    if (g_vm == nullptr)
    {
        throw std::runtime_error { "Java environment not initialized" };
    }
    JNIEnv* env = nullptr;
    if ((env = (JNIEnv *)pthread_getspecific(t_curr_env)) == nullptr)
    {
        env = attach_current_thread();
        pthread_setspecific(t_curr_env, env);
    }
    return env;
}