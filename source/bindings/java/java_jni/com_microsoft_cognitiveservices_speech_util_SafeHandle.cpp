//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
//
#include "com_microsoft_cognitiveservices_speech_util_SafeHandle.h"
#include "speechapi_c_speech_config.h"
#include "speechapi_c_audio_config.h"
#include "speechapi_c_property_bag.h"
#include "speechapi_c_recognizer.h"
#include "speechapi_c_connection.h"
#include "speechapi_c_audio_stream.h"
#include "speechapi_c_audio_stream_format.h"
#include "speechapi_c_keyword_recognition_model.h"
#include "speechapi_c_language_understanding_model.h"
#include "speechapi_c_intent_trigger.h"
#include "speechapi_c_user.h"
#include "speechapi_c_participant.h"
#include "speechapi_c_conversation.h"
#include "speechapi_c_dialog_service_connector.h"
#include "speechapi_c_auto_detect_source_lang_config.h"
#include "speechapi_c_source_lang_config.h"
#include "speechapi_c_synthesizer.h"
#include "speechapi_c_audio_stream.h"
#include "speechapi_c_grammar.h"
#include "speechapi_c_translation_recognizer.h"
#include "jni_utils.h"

/*
 * Class:     com_microsoft_cognitiveservices_speech_util_SafeHandle
 * Method:    releaseEventHandle
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_util_SafeHandle_releaseEventHandle
(JNIEnv *env, jobject obj, jlong handle)
{
    SPXHR hr = recognizer_event_handle_release((SPXEVENTHANDLE)handle);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_util_SafeHandle
 * Method:    releaseRecognizerHandle
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_util_SafeHandle_releaseRecognizerHandle
(JNIEnv *env, jobject obj, jlong handle)
{
    recognizer_recognizing_set_callback((SPXRECOHANDLE)handle, nullptr, nullptr);
    recognizer_recognized_set_callback((SPXRECOHANDLE)handle, nullptr, nullptr);
    recognizer_canceled_set_callback((SPXRECOHANDLE)handle, nullptr, nullptr);
    recognizer_session_started_set_callback((SPXRECOHANDLE)handle, nullptr, nullptr);
    recognizer_session_stopped_set_callback((SPXRECOHANDLE)handle, nullptr, nullptr);
    recognizer_speech_start_detected_set_callback((SPXRECOHANDLE)handle, nullptr, nullptr);
    recognizer_speech_end_detected_set_callback((SPXRECOHANDLE)handle, nullptr, nullptr);
    SPXHR hr = recognizer_handle_release((SPXRECOHANDLE)handle);
    RemoveGlobalReferenceFromHandle(env, (SPXHANDLE)handle);
    return (jlong)hr;    
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_util_SafeHandle
 * Method:    releaseSpeechConfigHandle
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_util_SafeHandle_releaseSpeechConfigHandle
  (JNIEnv *env, jobject obj, jlong handle)
{
    SPXHR hr = speech_config_release((SPXSPEECHCONFIGHANDLE)handle);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_util_SafeHandle
 * Method:    releaseAudioConfigHandle
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_util_SafeHandle_releaseAudioConfigHandle
  (JNIEnv *env, jobject obj, jlong handle)
{
    SPXHR hr = audio_config_release((SPXEVENTHANDLE)handle);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_util_SafeHandle
 * Method:    releasePropertyHandle
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_util_SafeHandle_releasePropertyHandle
  (JNIEnv *env, jobject obj, jlong handle)
{
    SPXHR hr = property_bag_release((SPXPROPERTYBAGHANDLE)handle);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_util_SafeHandle
 * Method:    releaseRecognizerResultHandle
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_util_SafeHandle_releaseRecognizerResultHandle
(JNIEnv *env, jobject obj, jlong handle)
{
    SPXHR hr = recognizer_result_handle_release((SPXRESULTHANDLE)handle);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_util_SafeHandle
 * Method:    releaseConnectionHandle
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_util_SafeHandle_releaseConnectionHandle
(JNIEnv *env, jobject obj, jlong handle)
{
    connection_connected_set_callback((SPXCONNECTIONHANDLE)handle, nullptr, nullptr);
    connection_disconnected_set_callback((SPXCONNECTIONHANDLE)handle, nullptr, nullptr);
    SPXHR hr = connection_handle_release((SPXCONNECTIONHANDLE)handle);
    RemoveGlobalReferenceFromHandle(env, (SPXHANDLE)handle);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_util_SafeHandle
 * Method:    releaseAudioInputStreamHandle
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_util_SafeHandle_releaseAudioInputStreamHandle
  (JNIEnv *env, jobject obj, jlong handle)
{
    SPXHR hr = audio_stream_release((SPXAUDIOSTREAMHANDLE)handle);
    RemoveGlobalReferenceFromHandle(env, (SPXHANDLE)handle);
    return (jlong)hr;
}
/*
 * Class:     com_microsoft_cognitiveservices_speech_util_SafeHandle
 * Method:    releaseAudioOutputStreamHandle
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_util_SafeHandle_releaseAudioOutputStreamHandle
  (JNIEnv *env, jobject obj, jlong handle)
{
    SPXHR hr = audio_stream_release((SPXAUDIOSTREAMHANDLE)handle);
    RemoveGlobalReferenceFromHandle(env, (SPXHANDLE)handle);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_util_SafeHandle
 * Method:    releaseAudioStreamFormatHandle
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_util_SafeHandle_releaseAudioStreamFormatHandle
  (JNIEnv *env, jobject obj, jlong handle)
{
    SPXHR hr = audio_stream_format_release((SPXAUDIOSTREAMFORMATHANDLE) handle);
    return (jlong)hr;
}
/*
 * Class:     com_microsoft_cognitiveservices_speech_util_SafeHandle
 * Method:    releaseKeywordModelHandle
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_util_SafeHandle_releaseKeywordModelHandle
  (JNIEnv* env, jobject obj, jlong handle)
{
    SPXHR hr = keyword_recognition_model_handle_release((SPXKEYWORDHANDLE)handle);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_util_SafeHandle
 * Method:    releaseLanguageUnderstandingModelHandle
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_util_SafeHandle_releaseLanguageUnderstandingModelHandle
  (JNIEnv* env, jobject obj, jlong handle)
{
    SPXHR hr = language_understanding_model__handle_release((SPXLUMODELHANDLE)handle);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_util_SafeHandle
 * Method:    releaseIntentTriggerHandle
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_util_SafeHandle_releaseIntentTriggerHandle
  (JNIEnv* env, jobject obj, jlong handle)
{
    SPXHR hr = intent_trigger_handle_release((SPXAUDIOSTREAMFORMATHANDLE)handle);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_util_SafeHandle
 * Method:    releaseUserHandle
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_util_SafeHandle_releaseUserHandle
  (JNIEnv* env, jobject obj, jlong handle)
{
    SPXHR hr = user_release_handle((SPXUSERHANDLE)handle);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_util_SafeHandle
 * Method:    releaseParticipantHandle
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_util_SafeHandle_releaseParticipantHandle
  (JNIEnv* env, jobject obj, jlong handle)
{
    SPXHR hr = participant_release_handle((SPXPARTICIPANTHANDLE)handle);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_util_SafeHandle
 * Method:    releaseConversationHandle
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_util_SafeHandle_releaseConversationHandle
  (JNIEnv* env, jobject obj, jlong handle)
{
    SPXHR hr = conversation_release_handle((SPXHANDLE)handle);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_util_SafeHandle
 * Method:    releaseDialogServiceConnectorHandle
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_util_SafeHandle_releaseDialogServiceConnectorHandle
  (JNIEnv* env, jobject obj, jlong handle)
{
    dialog_service_connector_session_started_set_callback((SPXRECOHANDLE)handle, nullptr, nullptr);
    dialog_service_connector_session_stopped_set_callback((SPXRECOHANDLE)handle, nullptr, nullptr);
    dialog_service_connector_recognized_set_callback((SPXRECOHANDLE)handle, nullptr, nullptr);
    dialog_service_connector_recognizing_set_callback((SPXRECOHANDLE)handle, nullptr, nullptr);
    dialog_service_connector_canceled_set_callback((SPXRECOHANDLE)handle, nullptr, nullptr);
    dialog_service_connector_activity_received_set_callback((SPXRECOHANDLE)handle, nullptr, nullptr);
    SPXHR hr = dialog_service_connector_handle_release((SPXRECOHANDLE)handle);
    RemoveGlobalReferenceFromHandle(env, (SPXHANDLE)handle);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_util_SafeHandle
 * Method:    releaseDialogServiceConnectorEventHandle
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_util_SafeHandle_releaseDialogServiceConnectorEventHandle
  (JNIEnv* env, jobject obj, jlong handle)
{
    SPXHR hr = dialog_service_connector_activity_received_event_release((SPXEVENTHANDLE)handle);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_util_SafeHandle
 * Method:    releaseAutoDetectSourceLanguageConfigHandle
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_util_SafeHandle_releaseAutoDetectSourceLanguageConfigHandle
  (JNIEnv*env, jobject obj, jlong handle)
{
    SPXHR hr = auto_detect_source_lang_config_release((SPXAUTODETECTSOURCELANGCONFIGHANDLE)handle);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_util_SafeHandle
 * Method:    releaseSourceLanguageConfigHandle
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_util_SafeHandle_releaseSourceLanguageConfigHandle
  (JNIEnv* env, jobject obj, jlong handle)
{
    SPXHR hr = source_lang_config_release((SPXSOURCELANGCONFIGHANDLE)handle);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_util_SafeHandle
 * Method:    releaseSynthesisEventHandle
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_util_SafeHandle_releaseSynthesisEventHandle
  (JNIEnv* env, jobject obj, jlong handle)
{
    SPXHR hr = synthesizer_event_handle_release((SPXEVENTHANDLE)handle);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_util_SafeHandle
 * Method:    releaseSynthesisResultHandle
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_util_SafeHandle_releaseSynthesisResultHandle
  (JNIEnv* env, jobject obj, jlong handle)
{
    SPXHR hr = synthesizer_result_handle_release((SPXRESULTHANDLE)handle);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_util_SafeHandle
 * Method:    releaseAudioDataStreamHandle
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_util_SafeHandle_releaseAudioDataStreamHandle
  (JNIEnv* env, jobject obj, jlong handle)
{
    SPXHR hr = audio_data_stream_release((SPXAUDIOSTREAMHANDLE)handle);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_util_SafeHandle
 * Method:    releaseSynthesizerHandle
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_util_SafeHandle_releaseSynthesizerHandle
  (JNIEnv* env, jobject obj, jlong handle)
{
    synthesizer_word_boundary_set_callback((SPXSYNTHHANDLE)handle, nullptr, nullptr);
    synthesizer_canceled_set_callback((SPXSYNTHHANDLE)handle, nullptr, nullptr);
    synthesizer_completed_set_callback((SPXSYNTHHANDLE)handle, nullptr, nullptr);
    synthesizer_synthesizing_set_callback((SPXSYNTHHANDLE)handle, nullptr, nullptr);
    synthesizer_started_set_callback((SPXSYNTHHANDLE)handle, nullptr, nullptr);
    SPXHR hr = synthesizer_handle_release((SPXSYNTHHANDLE)handle);
    RemoveGlobalReferenceFromHandle(env, (SPXHANDLE)handle);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_util_SafeHandle
 * Method:    releaseGrammarHandle
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_util_SafeHandle_releaseGrammarHandle
  (JNIEnv* env, jobject obj, jlong handle)
{
    SPXHR hr = grammar_handle_release((SPXGRAMMARHANDLE)handle);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_util_SafeHandle
 * Method:    releaseTranslationSynthesisHandle
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_util_SafeHandle_releaseTranslationSynthesisHandle
(JNIEnv* env, jobject obj, jlong handle)
{
    SPXHR hr = translator_synthesizing_audio_set_callback((SPXRECOHANDLE)handle, nullptr, nullptr);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_util_SafeHandle
 * Method:    releaseKeywordRecognizerHandle
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_util_SafeHandle_releaseKeywordRecognizerHandle
(JNIEnv* env, jobject obj, jlong handle)
{
    recognizer_recognized_set_callback((SPXRECOHANDLE)handle, nullptr, nullptr);
    recognizer_canceled_set_callback((SPXRECOHANDLE)handle, nullptr, nullptr);
    SPXHR hr = recognizer_handle_release((SPXRECOHANDLE)handle);
    RemoveGlobalReferenceFromHandle(env, (SPXHANDLE)handle);
    return (jlong)hr;
}
