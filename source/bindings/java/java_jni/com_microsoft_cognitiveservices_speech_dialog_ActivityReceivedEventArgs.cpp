//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
//
#include "com_microsoft_cognitiveservices_speech_dialog_ActivityReceivedEventArgs.h"
#include "speechapi_c_dialog_service_connector.h"
#include "jni_utils.h"
#include <memory>

/*
 * Class:     com_microsoft_cognitiveservices_speech_dialog_ActivityReceivedEventArgs
 * Method:    getAudio
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/IntRef;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_dialog_ActivityReceivedEventArgs_getAudio
  (JNIEnv *env, jobject obj, jobject activityReceivedEventHandle, jobject audioStreamHandle)
{
    SPXAUDIOSTREAMHANDLE audioStream = SPXHANDLE_INVALID;
    jlong event = GetObjectHandle(env, activityReceivedEventHandle);
    SPXHR hr = dialog_service_connector_activity_received_event_get_audio((SPXEVENTHANDLE)event, &audioStream);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, audioStreamHandle, (jlong)audioStream);
    }
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_dialog_ActivityReceivedEventArgs
 * Method:    getActivity
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/StringRef;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_dialog_ActivityReceivedEventArgs_getActivity
  (JNIEnv *env, jobject obj, jobject activityReceivedEventHandle, jobject activityStr)
{
    jlong event = GetObjectHandle(env, activityReceivedEventHandle);
    size_t size = 0;
    SPXHR hr = dialog_service_connector_activity_received_event_get_activity_size((SPXEVENTHANDLE)event, &size);
    auto activity = std::make_unique<char[]>(size + 1);
    if (SPX_SUCCEEDED(hr))
    {
        hr = dialog_service_connector_activity_received_event_get_activity((SPXEVENTHANDLE)event, activity.get(), size + 1);
    }
    if (SPX_SUCCEEDED(hr))
    {
        SetStringObjectHandle(env, activityStr, activity.get());
    }
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_dialog_ActivityReceivedEventArgs
 * Method:    hasAudio
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_microsoft_cognitiveservices_speech_dialog_ActivityReceivedEventArgs_hasAudio
  (JNIEnv *env, jobject obj, jobject activityReceivedEventHandle)
{
    jlong event = GetObjectHandle(env, activityReceivedEventHandle);
    bool hasAudio = dialog_service_connector_activity_received_event_has_audio((SPXEVENTHANDLE)event);
    return (jboolean)hasAudio;
}
