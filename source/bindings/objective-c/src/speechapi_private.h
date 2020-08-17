//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
#ifdef _DEBUG
#define LogDebug( ... ) NSLog( __VA_ARGS__ )
#else
#define LogDebug( ... )
#endif

#import "common_private.h"

#import "util_private.h"

#import "audio_stream_format_private.h"
#import "audio_stream_private.h"
#import "audio_config_private.h"

#import "speech_config_private.h"
#import "speech_translation_config_private.h"

#import "language_understanding_model_private.h"

#import "recognition_result_property_collection_private.h"
#import "recognition_result_private.h"
#import "speech_recognition_result_private.h"
#import "intent_recognition_result_private.h"
#import "translation_text_result_private.h"
#import "translation_synthesis_result_private.h"
#import "speech_synthesis_result_property_collection_private.h"
#import "speech_synthesis_result_private.h"
#import "audio_data_stream_private.h"

#import "session_event_args_private.h"
#import "connection_event_args_private.h"
#import "recognition_event_args_private.h"
#import "speech_recognition_event_args_private.h"
#import "intent_recognition_event_args_private.h"
#import "translation_text_result_event_args_private.h"
#import "translation_synthesis_result_event_args_private.h"
#import "speech_synthesis_event_args_private.h"
#import "speech_synthesis_cancellation_details_private.h"

#import "property_collection_private.h"
#import "recognizer_private.h"
#import "speech_recognizer_private.h"
#import "speech_synthesizer_private.h"
#import "intent_recognizer_private.h"
#import "translation_recognizer_private.h"
#import "connection_private.h"

#import "grammar_private.h"
#import "grammar_phrase_private.h"
#import "phrase_list_grammar_private.h"

#import "auto_detect_source_language_config_private.h"
#import "source_language_config_private.h"

#import "pronunciation_assessment_config_private.h"

#import "conversation_private.h"
#import "conversation_event_args_private.h"
#import "conversation_transcriber_private.h"
#import "conversation_transcription_event_args_private.h"
#import "conversation_transcription_result_private.h"
#import "conversation_translation_event_args_private.h"
#import "conversation_translation_result_private.h"
#import "conversation_translator_private.h"
#import "participant_private.h"
#import "user_private.h"
