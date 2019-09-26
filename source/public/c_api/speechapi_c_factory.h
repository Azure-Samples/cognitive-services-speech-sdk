//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#pragma once
#include <speechapi_c_common.h>

SPXAPI recognizer_create_speech_recognizer_from_config(SPXRECOHANDLE* phreco, SPXSPEECHCONFIGHANDLE hspeechconfig, SPXAUDIOCONFIGHANDLE haudioInput);
SPXAPI recognizer_create_translation_recognizer_from_config(SPXRECOHANDLE* phreco, SPXSPEECHCONFIGHANDLE hspeechconfig, SPXAUDIOCONFIGHANDLE haudioInput);
SPXAPI recognizer_create_intent_recognizer_from_config(SPXRECOHANDLE* phreco, SPXSPEECHCONFIGHANDLE hspeechconfig, SPXAUDIOCONFIGHANDLE haudioInput);
SPXAPI synthesizer_create_speech_synthesizer_from_config(SPXSYNTHHANDLE* phsynth, SPXSPEECHCONFIGHANDLE hspeechconfig, SPXAUDIOCONFIGHANDLE haudioconfig);
SPXAPI dialog_service_connector_create_dialog_service_connector_from_config(SPXRECOHANDLE* phreco, SPXSPEECHCONFIGHANDLE hspeechconfig, SPXAUDIOCONFIGHANDLE haudioInput);
SPXAPI recognizer_create_conversation_transcriber_from_config(SPXRECOHANDLE* phreco, SPXSPEECHCONFIGHANDLE hspeechconfig, SPXAUDIOCONFIGHANDLE haudioInput);
