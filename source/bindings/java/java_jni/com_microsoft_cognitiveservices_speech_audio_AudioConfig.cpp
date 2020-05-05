//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
//
#include "com_microsoft_cognitiveservices_speech_audio_AudioConfig.h"
#include "speechapi_c_audio_config.h"
#include "jni_utils.h"

/*
 * Class:     com_microsoft_cognitiveservices_speech_audio_AudioConfig
 * Method:    createAudioInputFromWavFileName
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/IntRef;Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_audio_AudioConfig_createAudioInputFromWavFileName
  (JNIEnv *env, jclass cls, jobject audioConfigHandle, jstring fileName)
{
    SPXAUDIOCONFIGHANDLE configHandle = SPXHANDLE_INVALID;
    const char *filename = GetStringUTFChars(env, fileName);
    SPXHR hr = audio_config_create_audio_input_from_wav_file_name(&configHandle, filename);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, audioConfigHandle, (jlong)configHandle);
    }
    ReleaseStringUTFChars(env, fileName, filename);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_audio_AudioConfig
 * Method:    createAudioInputFromDefaultMicrophone
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/IntRef;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_audio_AudioConfig_createAudioInputFromDefaultMicrophone
  (JNIEnv *env, jclass cls, jobject audioConfigHandle)
{
    SPXAUDIOCONFIGHANDLE configHandle = SPXHANDLE_INVALID;
    SPXHR hr = audio_config_create_audio_input_from_default_microphone(&configHandle);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, audioConfigHandle, (jlong)configHandle);
    }
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_audio_AudioConfig
 * Method:    createAudioInputFromMicrophoneInput
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/IntRef;Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_audio_AudioConfig_createAudioInputFromMicrophoneInput
  (JNIEnv *env, jclass cls, jobject audioConfigHandle, jstring micDeviceName)
{
    SPXAUDIOCONFIGHANDLE configHandle = SPXHANDLE_INVALID;
    const char *deviceName = GetStringUTFChars(env, micDeviceName);
    SPXHR hr = audio_config_create_audio_input_from_a_microphone(&configHandle, deviceName);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, audioConfigHandle, (jlong)configHandle);
    }
    ReleaseStringUTFChars(env, micDeviceName, deviceName);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_audio_AudioConfig
 * Method:    createAudioInputFromStream
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/IntRef;Lcom/microsoft/cognitiveservices/speech/util/SafeHandle)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_audio_AudioConfig_createAudioInputFromStream
  (JNIEnv *env, jclass cls, jobject audioConfigHandle, jobject audioStreamHandle)
{
    SPXAUDIOCONFIGHANDLE configHandle = SPXHANDLE_INVALID;
    jlong streamHandle = GetObjectHandle(env, audioStreamHandle);
    SPXHR hr = audio_config_create_audio_input_from_stream(&configHandle, (SPXAUDIOSTREAMHANDLE)streamHandle);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, audioConfigHandle, (jlong)configHandle);
    }
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_audio_AudioConfig
 * Method:    createAudioOutputFromDefaultSpeaker
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/IntRef;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_audio_AudioConfig_createAudioOutputFromDefaultSpeaker
  (JNIEnv *env, jclass cls, jobject audioConfigHandle)
{
    SPXAUDIOCONFIGHANDLE configHandle = SPXHANDLE_INVALID;
    SPXHR hr = audio_config_create_audio_output_from_default_speaker(&configHandle);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, audioConfigHandle, (jlong)configHandle);
    }
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_audio_AudioConfig
 * Method:    createAudioOutputFromWavFileName
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/IntRef;Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_audio_AudioConfig_createAudioOutputFromWavFileName
  (JNIEnv *env, jclass cls, jobject audioConfigHandle, jstring fileName)
{
    SPXAUDIOCONFIGHANDLE configHandle = SPXHANDLE_INVALID;
    const char* file = GetStringUTFChars(env, fileName);
    SPXHR hr = audio_config_create_audio_output_from_wav_file_name(&configHandle, file);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, audioConfigHandle, (jlong)configHandle);
    }
    ReleaseStringUTFChars(env, fileName, file);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_audio_AudioConfig
 * Method:    createAudioOutputFromStream
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/IntRef;Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_audio_AudioConfig_createAudioOutputFromStream
  (JNIEnv *env, jclass cls, jobject audioConfigHandle, jobject audioOutputStreamHandle)
{
    SPXAUDIOCONFIGHANDLE configHandle = SPXHANDLE_INVALID;
    jlong handle = GetObjectHandle(env, audioOutputStreamHandle);
    SPXHR hr = audio_config_create_audio_output_from_stream(&configHandle, (SPXAUDIOSTREAMHANDLE)handle);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, audioConfigHandle, (jlong)configHandle);
    }
    return (jlong)hr;
}
