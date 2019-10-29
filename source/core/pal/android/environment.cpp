//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
#include "environment.h"

#include <memory>
#include <exception>

static JavaVM* g_vm = nullptr;

thread_local std::shared_ptr<JNIEnvironment> t_env{ nullptr };

JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void*)
{
    JNIEnv* env;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK)
    {
        return -1;
    }
    g_vm = vm;
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

JNIEnvironment::JNIEnvironment(JavaVM* vm)
{
    JNIEnv* e;
    if (vm->AttachCurrentThread(&e, nullptr) != JNI_OK)
    {
        throw std::runtime_error{ "Cannot initialize JNI Environment." };
    }
    env = e;
}

JNIEnvironment::~JNIEnvironment()
{
    if (g_vm != nullptr)
    {
        g_vm->DetachCurrentThread();
    }
}

std::shared_ptr<JNIEnvironment> GetEnvironment()
{
    if (g_vm == nullptr)
    {
        throw std::runtime_error { "Java environment not initialized" };
    }
    if (t_env == nullptr)
    {
        t_env = std::make_shared<JNIEnvironment>(g_vm);
    }
    return t_env;
}