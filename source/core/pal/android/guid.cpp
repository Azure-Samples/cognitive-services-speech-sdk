//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
#include "guid.h"
#include <array>
#include <random>

#include "environment.h"

namespace PAL = Microsoft::CognititveServices::Speech::PAL;

template<typename T>
T* throw_if_null(T* ptr, const char* error)
{
    if (ptr == nullptr)
    {
        throw std::runtime_error{ error };
    }
    return ptr;
}

constexpr size_t UUID_LENGTH = 36;

std::string PAL::GenerateGUID()
{
    if (GetVM() == nullptr)
    {
        /**
         * This is a workaround for when the library is used through pinvoke as JNI_OnLoad doesn't get called.
         * The proper fix is to use something like
         * https://docs.microsoft.com/dotnet/api/java.lang.javasystem.loadlibrary?view=xamarin-android-sdk-9
         * to force the interp layer to initialized the JNI environment. Until we have that we are falling back to the
         * insecure guid generation
         */
        std::array<char, UUID_LENGTH + 1> buffer;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<unsigned int> dist(std::numeric_limits<unsigned int>::min(), std::numeric_limits<unsigned int>::max());
        srand(dist(gen));
        UniqueId_Generate(buffer.data(), UUID_LENGTH + 1);
        return std::string{ buffer.data() };
    }
    return RunOnEnv([](JNIEnv* env)
    {
        /**
         *  This is equivalent to the following java code:
         *
         *  var uuid = java.util.UUID.randomUUID();
         *  return uuid.toString();
         *
         *  With code added to do the UTF-16 -> UTF-8 conversion.
         */
        auto cl = throw_if_null(env->FindClass("java/util/UUID"), "Can't find UUID class.");
        auto randomUUID = throw_if_null(env->GetStaticMethodID(cl, "randomUUID", "()Ljava/util/UUID;"), "Can't find static method \"UUID.randomUUID()\"");
        auto toString = throw_if_null(env->GetMethodID(cl, "toString", "()Ljava/lang/String;"), "Can't find method \"UUID.toString()\"");
        auto uuidObj = throw_if_null(env->CallStaticObjectMethod(cl, randomUUID), "Problem calling \"UUID.randomUUID()\"");
        auto uuidStr = static_cast<jstring>(throw_if_null(env->CallObjectMethod(uuidObj, toString), "Problem calling \"UUID.toString()\""));
        auto uuidUTFStr = env->GetStringUTFChars(uuidStr, 0);
        std::string uuid{ uuidUTFStr };
        env->DeleteLocalRef(uuidObj);
        env->ReleaseStringUTFChars(uuidStr, uuidUTFStr);
        return uuid;
    });
}