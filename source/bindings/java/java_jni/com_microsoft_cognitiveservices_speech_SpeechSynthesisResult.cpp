//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
//
#include "com_microsoft_cognitiveservices_speech_SpeechSynthesisResult.h"
#include "speechapi_c_synthesizer.h"
#include "speechapi_c_result.h"
#include "jni_utils.h"
#include <vector>

const size_t maxCharCount = 1024;

/*
 * Class:     com_microsoft_cognitiveservices_speech_SpeechSynthesisResult
 * Method:    getResultId
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/StringRef;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_SpeechSynthesisResult_getResultId
  (JNIEnv *env, jobject obj, jobject resultHandle, jobject resultIdRef)
{
    char sz[maxCharCount + 1] = {};
    jlong handle = GetObjectHandle(env, resultHandle);
    SPXHR hr = synth_result_get_result_id((SPXRESULTHANDLE)handle, sz, maxCharCount);
    if (SPX_SUCCEEDED(hr))
    {
        std::string value = std::string(sz);
        SetStringObjectHandle(env, resultIdRef, value.c_str());
    }
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_SpeechSynthesisResult
 * Method:    getResultReason
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/IntRef;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_SpeechSynthesisResult_getResultReason
  (JNIEnv *env, jobject obj, jobject resultHandle, jobject reasonRef)
{
    Result_Reason resultReason;
    jlong handle = GetObjectHandle(env, resultHandle);
    SPXHR hr = synth_result_get_reason((SPXRESULTHANDLE)handle, &resultReason);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, reasonRef, (jlong)resultReason);
    }
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_SpeechSynthesisResult
 * Method:    getPropertyBagFromResult
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/IntRef;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_SpeechSynthesisResult_getPropertyBagFromResult
  (JNIEnv *env, jobject obj, jobject resultHandle, jobject propertyHandle)
{
    SPXPROPERTYBAGHANDLE progbag = SPXHANDLE_INVALID;
    jlong handle = GetObjectHandle(env, resultHandle);
    SPXHR hr = synth_result_get_property_bag((SPXRESULTHANDLE)handle, &progbag);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, propertyHandle, (jlong)progbag);
    }
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_SpeechSynthesisResult
 * Method:    getAudioLength
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/IntRef;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_SpeechSynthesisResult_getAudioLength
  (JNIEnv *env, jobject obj, jobject resultHandle, jobject audioLengthRef)
{
    uint32_t audioLength = 0;
    jlong handle = GetObjectHandle(env, resultHandle);
    SPXHR hr = synth_result_get_audio_length((SPXRESULTHANDLE)handle, &audioLength);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, audioLengthRef, (jlong)audioLength);
    }
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_SpeechSynthesisResult
 * Method:    getAudio
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/IntRef;)[B
 */
JNIEXPORT jbyteArray JNICALL Java_com_microsoft_cognitiveservices_speech_SpeechSynthesisResult_getAudio
  (JNIEnv *env, jobject obj, jobject resultHandle, jobject resultHr)
{
    uint32_t audioLength = 0;
    std::vector<uint8_t> audiodata;
    jbyteArray jdataBuffer = 0;
    jlong handle = GetObjectHandle(env, resultHandle);

    SPXHR hr = synth_result_get_audio_length((SPXRESULTHANDLE)handle, &audioLength);

    if (SPX_SUCCEEDED(hr) && audioLength > 0)
    {
        audiodata.resize(audioLength);
        uint32_t filledSize = 0;
        hr = synth_result_get_audio_data((SPXRESULTHANDLE)handle, audiodata.data(), audioLength, &filledSize);
    }

    if (SPX_SUCCEEDED(hr))
    {
        jdataBuffer = env->NewByteArray((jsize)audioLength);
        if (!jdataBuffer) hr = SPXERR_ABORT;
        if (SPX_SUCCEEDED(hr))
        {
            env->SetByteArrayRegion(jdataBuffer, 0, (jsize)audioLength, (jbyte*)audiodata.data());
        }
    }
    SetObjectHandle(env, resultHr, (jlong)hr);
    return jdataBuffer;
}
