//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
//
#include "com_microsoft_cognitiveservices_speech_audio_PushAudioOutputStream.h"
#include "speechapi_c_audio_stream.h"
#include "jni_utils.h"

jobject GetPushAudioOutputStreamCallbackObject(JNIEnv* env, jobject streamObject)
{
    // Get PushAudioOutputStreamCallback object from stream handle
    jclass cl = env->GetObjectClass(streamObject);
    if (CheckException(env))
    {
        return NULL;
    }
    jmethodID m = env->GetMethodID(cl, "getCallbackHandle", "()Lcom/microsoft/cognitiveservices/speech/audio/PushAudioOutputStreamCallback;");
    if (CheckException(env))
    {
        return NULL;
    }
    jobject callbackObj = env->CallObjectMethod(streamObject, m);
    if (CheckException(env))
    {
        return NULL;
    }
    return callbackObj;
}

int OutputStreamWriteCallback(void* context, uint8_t* buffer, uint32_t size)
{
    jint result = 0;
    jobject streamObject = GetObjectFromMap(static_cast<SPXHANDLE>(context));
    if (!streamObject) return 0;

    bool detach = false;
    JNIEnv* env = GetJNIEnvAndAttach(detach);
    if (!env) return 0;

    jbyteArray jdataBuffer = 0;
    if (buffer)
    {
        jdataBuffer = env->NewByteArray((jsize)size);
        if (!jdataBuffer) return 0;
        env->SetByteArrayRegion(jdataBuffer, 0, (jsize)size, (jbyte*)buffer);
        if (CheckException(env))
        {
            return 0;
        }
    }

    jobject callbackObj = GetPushAudioOutputStreamCallbackObject(env, streamObject);
    if (!callbackObj) return 0;

    // Call method write on PushAudioOutputStreamCallback object
    jclass cl = env->GetObjectClass(callbackObj);
    if (CheckException(env))
    {
        return 0;
    }
    jmethodID m = env->GetMethodID(cl, "write", "([B)I");
    if (CheckException(env))
    {
        return 0;
    }
    result = env->CallIntMethod(callbackObj, m, jdataBuffer);
    if (CheckException(env))
    {
        return 0;
    }
    if (detach)
    {
        DetachJNIEnv(env);
    }
    return (int)result;
}

void OutputStreamCloseCallback(void* context)
{
    jobject streamObject = GetObjectFromMap(static_cast<SPXHANDLE>(context));
    if (!streamObject) return;

    bool detach = false;
    JNIEnv* env = GetJNIEnvAndAttach(detach);
    if (!env) return;

    jobject callbackObj = GetPushAudioOutputStreamCallbackObject(env, streamObject);
    if (!callbackObj) return;

    jclass cl = env->GetObjectClass(callbackObj);
    if (CheckException(env))
    {
        return;
    }
    jmethodID m = env->GetMethodID(cl, "close", "()V");
    if (CheckException(env))
    {
        return;
    }
    env->CallVoidMethod(callbackObj, m);
    if (CheckException(env))
    {
        return;
    }
    if (detach)
    {
        DetachJNIEnv(env);
    }
    return;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_audio_PushAudioOutputStream
 * Method:    createPushAudioOutputStream
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/IntRef;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_audio_PushAudioOutputStream_createPushAudioOutputStream
  (JNIEnv* env, jclass cls, jobject audioStreamHandle)
{
    SPXAUDIOSTREAMHANDLE audioStream = SPXHANDLE_INVALID;
    SPXHR hr = audio_stream_create_push_audio_output_stream(&audioStream);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, audioStreamHandle, (jlong)audioStream);
    }
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_audio_PushAudioOutputStream
 * Method:    setStreamCallbacks
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/audio/PushAudioOutputStreamCallback;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_audio_PushAudioOutputStream_setStreamCallbacks
  (JNIEnv *env, jobject obj, jobject audioStreamHandle)
{
    jlong streamHandle = GetObjectHandle(env, audioStreamHandle);
    AddGlobalReferenceForHandle(env, obj, (SPXHANDLE)streamHandle);
    SPXHR hr = push_audio_output_stream_set_callbacks((SPXAUDIOSTREAMHANDLE)streamHandle, (SPXHANDLE)streamHandle, OutputStreamWriteCallback, OutputStreamCloseCallback);
    return (jlong)hr;
}
