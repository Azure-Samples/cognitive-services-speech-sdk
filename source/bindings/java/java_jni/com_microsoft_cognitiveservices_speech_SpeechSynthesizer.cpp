//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
//
#include "com_microsoft_cognitiveservices_speech_SpeechSynthesizer.h"
#include "speechapi_c_synthesizer.h"
#include "speechapi_c_factory.h"
#include "jni_utils.h"
#include <string.h>

void SynthesisStartedCallback(SPXSYNTHHANDLE recoHandle, SPXEVENTHANDLE eventHandle, void* context)
{
    CallbackEventMethod(context, "synthesisStartedEventCallback", eventHandle);
}

void SpeechSynthesizingCallback(SPXSYNTHHANDLE recoHandle, SPXEVENTHANDLE eventHandle, void* context)
{
    CallbackEventMethod(context, "synthesizingEventCallback", eventHandle);
}

void SynthesisCompletedCallback(SPXSYNTHHANDLE recoHandle, SPXEVENTHANDLE eventHandle, void* context)
{
    CallbackEventMethod(context, "synthesisCompletedEventCallback", eventHandle);
}

void SynthesisCanceledCallback(SPXSYNTHHANDLE recoHandle, SPXEVENTHANDLE eventHandle, void* context)
{
    CallbackEventMethod(context, "synthesisCanceledEventCallback", eventHandle);
}

void WordBoundaryCallback(SPXSYNTHHANDLE recoHandle, SPXEVENTHANDLE eventHandle, void* context)
{
    CallbackEventMethod(context, "wordBoundaryEventCallback", eventHandle);
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_SpeechSynthesizer
 * Method:    createSpeechSynthesizerFromConfig
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_SpeechSynthesizer_createSpeechSynthesizerFromConfig
  (JNIEnv *env, jobject obj, jobject synthesizerHandle, jobject speechConfigHandle, jobject audioConfigHandle)
{
    SPXSYNTHHANDLE synthHandle = SPXHANDLE_INVALID;
    jlong speechConfig = GetObjectHandle(env, speechConfigHandle);
    jlong audioConfig = GetObjectHandle(env, audioConfigHandle);
    SPXHR hr = synthesizer_create_speech_synthesizer_from_config(&synthHandle, (SPXSPEECHCONFIGHANDLE)speechConfig, (SPXAUDIOCONFIGHANDLE)audioConfig);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, synthesizerHandle, (jlong)synthHandle);
    }
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_SpeechSynthesizer
 * Method:    createSpeechSynthesizerFromAutoDetectSourceLangConfig
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_SpeechSynthesizer_createSpeechSynthesizerFromAutoDetectSourceLangConfig
  (JNIEnv *env, jobject obj, jobject synthesizerHandle, jobject speechConfigHandle, jobject autoDetectHandle, jobject audioConfigHandle)
{
    SPXSYNTHHANDLE synthHandle = SPXHANDLE_INVALID;
    jlong speechConfig = GetObjectHandle(env, speechConfigHandle);
    jlong autoDetectLangConfig = GetObjectHandle(env, autoDetectHandle);
    jlong audioConfig = GetObjectHandle(env, audioConfigHandle);
    SPXHR hr = synthesizer_create_speech_synthesizer_from_auto_detect_source_lang_config(&synthHandle, (SPXSPEECHCONFIGHANDLE)speechConfig, (SPXAUTODETECTSOURCELANGCONFIGHANDLE)autoDetectLangConfig, (SPXAUDIOCONFIGHANDLE)audioConfig);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, synthesizerHandle, (jlong)synthHandle);
    }
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_SpeechSynthesizer
 * Method:    speakText
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Ljava/lang/String;Lcom/microsoft/cognitiveservices/speech/util/IntRef;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_SpeechSynthesizer_speakText
  (JNIEnv *env, jobject obj, jobject synthesizerHandle, jstring textStr, jobject resultRef)
{
    jlong synthHandle = GetObjectHandle(env, synthesizerHandle);
    const char* text = GetStringUTFChars(env, textStr);
    SPXRESULTHANDLE result = SPXHANDLE_INVALID;
    uint32_t textLength = (uint32_t) std::string(text).length();
    SPXHR hr = synthesizer_speak_text((SPXSYNTHHANDLE)synthHandle, text, textLength, &result);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, resultRef, (jlong)result);
    }
    ReleaseStringUTFChars(env, textStr, text);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_SpeechSynthesizer
 * Method:    speakSsml
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Ljava/lang/String;Lcom/microsoft/cognitiveservices/speech/util/IntRef;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_SpeechSynthesizer_speakSsml
  (JNIEnv *env, jobject obj, jobject synthesizerHandle, jstring ssmlStr, jobject resultRef)
{
    jlong synthHandle = GetObjectHandle(env, synthesizerHandle);
    const char* ssml = GetStringUTFChars(env, ssmlStr);
    SPXRESULTHANDLE result = SPXHANDLE_INVALID;
    uint32_t ssmlLength = (uint32_t)std::string(ssml).length();
    SPXHR hr = synthesizer_speak_ssml((SPXSYNTHHANDLE)synthHandle, ssml, ssmlLength, &result);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, resultRef, (jlong)result);
    }
    ReleaseStringUTFChars(env, ssmlStr, ssml);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_SpeechSynthesizer
 * Method:    startSpeakingText
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Ljava/lang/String;Lcom/microsoft/cognitiveservices/speech/util/IntRef;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_SpeechSynthesizer_startSpeakingText
  (JNIEnv *env, jobject obj, jobject synthesizerHandle, jstring textStr, jobject resultRef)
{
    jlong synthHandle = GetObjectHandle(env, synthesizerHandle);
    const char* text = GetStringUTFChars(env, textStr);
    SPXRESULTHANDLE result = SPXHANDLE_INVALID;
    uint32_t textLength = (uint32_t)std::string(text).length();
    SPXHR hr = synthesizer_start_speaking_text((SPXSYNTHHANDLE)synthHandle, text, textLength, &result);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, resultRef, (jlong)result);
    }
    ReleaseStringUTFChars(env, textStr, text);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_SpeechSynthesizer
 * Method:    startSpeakingSsml
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Ljava/lang/String;Lcom/microsoft/cognitiveservices/speech/util/IntRef;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_SpeechSynthesizer_startSpeakingSsml
  (JNIEnv *env, jobject obj, jobject synthesizerHandle, jstring ssmlStr, jobject resultRef)
{
    jlong synthHandle = GetObjectHandle(env, synthesizerHandle);
    const char* ssml = GetStringUTFChars(env, ssmlStr);
    SPXRESULTHANDLE result = SPXHANDLE_INVALID;
    uint32_t ssmlLength = (uint32_t)std::string(ssml).length();
    SPXHR hr = synthesizer_start_speaking_ssml((SPXSYNTHHANDLE)synthHandle, ssml, ssmlLength, &result);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, resultRef, (jlong)result);
    }
    ReleaseStringUTFChars(env, ssmlStr, ssml);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_SpeechSynthesizer
 * Method:    stopSpeaking
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_SpeechSynthesizer_stopSpeaking
  (JNIEnv *env, jobject obj, jobject synthesizerHandle)
{
    jlong synthHandle = GetObjectHandle(env, synthesizerHandle);
    SPXHR hr = synthesizer_stop_speaking((SPXSYNTHHANDLE)synthHandle);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_SpeechSynthesizer
 * Method:    synthesisStartedSetCallback
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_SpeechSynthesizer_synthesisStartedSetCallback
  (JNIEnv *env, jobject obj, jobject synthesizerHandle)
{
    jlong handle = GetObjectHandle(env, synthesizerHandle);
    AddGlobalReferenceForHandle(env, obj, (SPXHANDLE)handle);
    SPXHR hr = synthesizer_started_set_callback((SPXSYNTHHANDLE)handle, SynthesisStartedCallback, (SPXHANDLE)handle);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_SpeechSynthesizer
 * Method:    synthesizingSetCallback
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_SpeechSynthesizer_synthesizingSetCallback
  (JNIEnv* env, jobject obj, jobject synthesizerHandle)
{
    jlong handle = GetObjectHandle(env, synthesizerHandle);
    AddGlobalReferenceForHandle(env, obj, (SPXHANDLE)handle);
    SPXHR hr = synthesizer_synthesizing_set_callback((SPXSYNTHHANDLE)handle, SpeechSynthesizingCallback, (SPXHANDLE)handle);
    return (jlong)hr;
}
/*
 * Class:     com_microsoft_cognitiveservices_speech_SpeechSynthesizer
 * Method:    synthesisCompletedSetCallback
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_SpeechSynthesizer_synthesisCompletedSetCallback
  (JNIEnv* env, jobject obj, jobject synthesizerHandle)
{
    jlong handle = GetObjectHandle(env, synthesizerHandle);
    AddGlobalReferenceForHandle(env, obj, (SPXHANDLE)handle);
    SPXHR hr = synthesizer_completed_set_callback((SPXSYNTHHANDLE)handle, SynthesisCompletedCallback, (SPXHANDLE)handle);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_SpeechSynthesizer
 * Method:    synthesisCanceledSetCallback
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_SpeechSynthesizer_synthesisCanceledSetCallback
  (JNIEnv* env, jobject obj, jobject synthesizerHandle)
{
    jlong handle = GetObjectHandle(env, synthesizerHandle);
    AddGlobalReferenceForHandle(env, obj, (SPXHANDLE)handle);
    SPXHR hr = synthesizer_canceled_set_callback((SPXSYNTHHANDLE)handle, SynthesisCanceledCallback, (SPXHANDLE)handle);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_SpeechSynthesizer
 * Method:    wordBoundarySetCallback
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_SpeechSynthesizer_wordBoundarySetCallback
  (JNIEnv* env, jobject obj, jobject synthesizerHandle)
{
    jlong handle = GetObjectHandle(env, synthesizerHandle);
    AddGlobalReferenceForHandle(env, obj, (SPXHANDLE)handle);
    SPXHR hr = synthesizer_word_boundary_set_callback((SPXSYNTHHANDLE)handle, WordBoundaryCallback, (SPXHANDLE)handle);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_SpeechSynthesizer
 * Method:    getPropertyBagFromSynthesizerHandle
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Lcom/microsoft/cognitiveservices/speech/util/IntRef;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_SpeechSynthesizer_getPropertyBagFromSynthesizerHandle
  (JNIEnv* env, jobject obj, jobject synthHandle, jobject propertyHandle)
{
    SPXPROPERTYBAGHANDLE propHandle = SPXHANDLE_INVALID;
    jlong handle = GetObjectHandle(env, synthHandle);
    SPXHR hr = synthesizer_get_property_bag((SPXSYNTHHANDLE)handle, &propHandle);
    if (SPX_SUCCEEDED(hr))
    {
        SetObjectHandle(env, propertyHandle, (jlong)propHandle);
    }
    return (jlong)hr;
}

