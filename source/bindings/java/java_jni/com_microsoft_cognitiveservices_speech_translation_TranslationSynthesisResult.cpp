//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
//
#include "com_microsoft_cognitiveservices_speech_translation_TranslationSynthesisResult.h"
#include "speechapi_c_translation_result.h"
#include "speechapi_c_result.h"
#include "jni_utils.h"
#include <vector>

/*
 * Class:     com_microsoft_cognitiveservices_speech_translation_TranslationSynthesisResult
 * Method:    getResultReason
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/IntRef;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_translation_TranslationSynthesisResult_getResultReason
  (JNIEnv *env, jobject obj, jobject resultHandle, jobject resultReasonRef)
{
    Result_Reason resultReason;
    jlong handle = GetObjectHandle(env, resultHandle);
    SPXHR hr = result_get_reason((SPXRESULTHANDLE)handle, &resultReason);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, resultReasonRef, (jlong)resultReason);
    }
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_translation_TranslationSynthesisResult
 * Method:    getAudio
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/IntRef;)[B
 */
JNIEXPORT jbyteArray JNICALL Java_com_microsoft_cognitiveservices_speech_translation_TranslationSynthesisResult_getAudio
  (JNIEnv *env, jobject obj, jobject resultHandle, jobject resultHr)
{
    size_t bufLen = 0;
    std::vector<uint8_t> audioData;
    jbyteArray jdataBuffer = NULL;

    jlong handle = GetObjectHandle(env, resultHandle);
    SPXHR hr = translation_synthesis_result_get_audio_data((SPXRESULTHANDLE)handle, nullptr, &bufLen);
    if (hr == SPXERR_BUFFER_TOO_SMALL)
    {
        audioData.resize(bufLen);
        hr = translation_synthesis_result_get_audio_data((SPXRESULTHANDLE)handle, audioData.data(), &bufLen);
    }

    if (SPX_SUCCEEDED(hr))
    {
        jdataBuffer = env->NewByteArray((jsize)bufLen);
        if (!jdataBuffer) hr = SPXERR_ABORT;
        if (SPX_SUCCEEDED(hr))
        {
            env->SetByteArrayRegion(jdataBuffer, 0, (jsize)bufLen, (jbyte*)audioData.data());
        }
    }
    SetObjectHandle(env, resultHr, (jlong)hr);
    return jdataBuffer;
}
