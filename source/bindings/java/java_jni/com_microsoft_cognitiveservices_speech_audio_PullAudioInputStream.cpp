//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
//
#include "com_microsoft_cognitiveservices_speech_audio_PullAudioInputStream.h"
#include "speechapi_c_audio_stream.h"
#include "speechapi_c_audio_stream_format.h"
#include "jni_utils.h"
#include <string.h>
#include <cstring>

jobject GetPullAudioInputStreamCallbackObject(JNIEnv* env, jobject streamObject)
{
    // Get PullAudioInputStreamCallback object from stream handle
    jclass cl = env->GetObjectClass(streamObject);
    if (cl == NULL)
    {
        return NULL;
    }
    jmethodID m = env->GetMethodID(cl, "getCallbackHandle", "()Lcom/microsoft/cognitiveservices/speech/audio/PullAudioInputStreamCallback;");
    if (m == NULL)
    {
        return NULL;
    }
    jobject callbackObj = env->CallObjectMethod(streamObject, m);
    return callbackObj;
}

int StreamReadCallback(void* context, uint8_t* buffer, uint32_t size)
{
    jint result = 0;
    jobject streamObject = GetObjectFromMap(static_cast<SPXHANDLE>(context));
    if (!streamObject) return 0;

    bool detach = false;   
    JNIEnv* env = GetJNIEnvAndAttach(detach);
    if (!env) return 0;

    jobject callbackObj = GetPullAudioInputStreamCallbackObject(env, streamObject);
    if (!callbackObj) return 0;

    jbyteArray jdataBuffer = 0;
    if (buffer)
    {
        jdataBuffer = env->NewByteArray((jsize)size);
        if (!jdataBuffer) return 0;
        //env->SetByteArrayRegion(jdataBuffer, 0, (jsize)size, (jbyte *)buffer);
    }

    jclass cl = env->GetObjectClass(callbackObj);
    if (cl == NULL)
    {
        return 0;
    }
    jmethodID m = env->GetMethodID(cl, "read", "([B)I");
    if (m == NULL)
    {
        return 0;
    }
    result = env->CallIntMethod(callbackObj, m, jdataBuffer);
    if (jdataBuffer && buffer)
    {
        env->GetByteArrayRegion(jdataBuffer, 0, (jsize)size, (jbyte *)buffer);
    }
    if (detach)
    {
        DetachJNIEnv(env);
    }
    return (int)result;
}

void StreamCloseCallback(void* context)
{
    jobject streamObject = GetObjectFromMap(static_cast<SPXHANDLE>(context));
    if (!streamObject) return;

    bool detach = false;
    JNIEnv* env = GetJNIEnvAndAttach(detach);
    if (!env) return;

    jobject callbackObj = GetPullAudioInputStreamCallbackObject(env, streamObject);
    if (!callbackObj) return;

    jclass cl = env->GetObjectClass(callbackObj);
    if (cl == NULL)
    {
        return;
    }
    jmethodID m = env->GetMethodID(cl, "close", "()V");
    if (m == NULL)
    {
        return;
    }
    env->CallVoidMethod(callbackObj, m);

    if (detach)
    {
        DetachJNIEnv(env);
    }

    return;
}

void StreamGetPropertyCallback(void* context, int id, uint8_t* result, uint32_t size)
{
    jobject streamObject = GetObjectFromMap(static_cast<SPXHANDLE>(context));
    if (!streamObject) return;

    bool detach = false;
    JNIEnv* env = GetJNIEnvAndAttach(detach);
    if (!env) return;

    jobject callbackObj = GetPullAudioInputStreamCallbackObject(env, streamObject);
    if (!callbackObj) return;

    // Get propertyId from PullAudioInputStreamCallback
    jclass cl = env->GetObjectClass(callbackObj);
    if (cl == NULL)
    {
        return;
    }

    jmethodID m = env->GetMethodID(cl, "getPropertyId", "(I)Lcom/microsoft/cognitiveservices/speech/PropertyId;");
    if (m == NULL)
    {
        return;
    }

    jobject propertyId = env->CallObjectMethod(callbackObj, m, id);
    if (propertyId == NULL)
    {
        return;
    }

    m = env->GetMethodID(cl, "getProperty", "(Lcom/microsoft/cognitiveservices/speech/PropertyId;)Ljava/lang/String;");
    if (m == NULL)
    {
        return;
    }

    jstring propertyJstring = (jstring)(env->CallObjectMethod(callbackObj, m, propertyId));
    const char* propertyString = env->GetStringUTFChars(propertyJstring, 0);
    auto propertyStringSize = strlen(propertyString) + 1;
    if (propertyStringSize <= size)
    {
        std::memcpy(result, propertyString, propertyStringSize);
    }
    env->ReleaseStringUTFChars(propertyJstring, propertyString);

    if (detach)
    {
        DetachJNIEnv(env);
    }

    return;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_audio_PullAudioInputStream
 * Method:    createPullAudioInputStream
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_audio_PullAudioInputStream_createPullAudioInputStream
  (JNIEnv *env, jclass cls, jobject streamHandle, jobject streamFormatHandle)
{
    SPXAUDIOSTREAMHANDLE audioStreamHandle = SPXHANDLE_INVALID;
    SPXAUDIOSTREAMFORMATHANDLE defaultFormat = SPXHANDLE_INVALID;
    jlong formatHandle = GetObjectHandle(env, streamFormatHandle);
    SPXHR hr = SPX_NOERROR;
    if (!formatHandle)
    {
        hr = audio_stream_format_create_from_default_input(&defaultFormat);
    }
    if (SPX_SUCCEEDED(hr))
    {
        hr = audio_stream_create_pull_audio_input_stream(&audioStreamHandle, formatHandle ? (SPXAUDIOSTREAMFORMATHANDLE)formatHandle: defaultFormat);
    }
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, streamHandle, (jlong)audioStreamHandle);
    }
    if (audio_stream_format_is_handle_valid(defaultFormat))
    {
        audio_stream_format_release(defaultFormat);
    }
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_audio_PullAudioInputStream
 * Method:    setStreamCallbacks
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_audio_PullAudioInputStream_setStreamCallbacks
  (JNIEnv *env, jobject obj, jobject audioStreamHandle)
{
    jlong streamHandle = GetObjectHandle(env, audioStreamHandle);
    AddGlobalReferenceForHandle(env, obj, (SPXHANDLE)streamHandle);
    SPXHR hr = pull_audio_input_stream_set_callbacks((SPXAUDIOSTREAMHANDLE)streamHandle, (SPXHANDLE)streamHandle, StreamReadCallback, StreamCloseCallback);
    if (SPX_SUCCEEDED(hr))
    {
        hr = pull_audio_input_stream_set_getproperty_callback((SPXAUDIOSTREAMHANDLE)streamHandle, (SPXHANDLE)streamHandle, StreamGetPropertyCallback);
    }
    return (jlong)hr;
}
