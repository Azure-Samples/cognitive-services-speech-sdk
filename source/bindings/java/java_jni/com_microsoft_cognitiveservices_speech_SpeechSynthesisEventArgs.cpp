//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
//
#include "com_microsoft_cognitiveservices_speech_SpeechSynthesisEventArgs.h"
#include "speechapi_c_synthesizer.h"
#include "jni_utils.h"

/*
 * Class:     com_microsoft_cognitiveservices_speech_SpeechSynthesisEventArgs
 * Method:    getSynthesisResult
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/IntRef;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_SpeechSynthesisEventArgs_getSynthesisResult
  (JNIEnv *env, jobject obj, jobject eventHandle, jobject resultHandle)
{
    SPXRESULTHANDLE result = SPXHANDLE_INVALID;
    jlong handle = GetObjectHandle(env, eventHandle);
    SPXHR hr = synthesizer_synthesis_event_get_result((SPXEVENTHANDLE)handle, &result);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, resultHandle, (jlong)result);
    }
    return (jlong)hr;
}
