//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// create_module_object.cpp: Implementation definitions for *CreateModuleObject* methods
//

#include "stdafx.h"

#include "activity_event_args.h"
#include "audio_stream_session.h"
#include "factory_helpers.h"
#include "intent_recognizer.h"
#include "conversation_transcriber.h"
#include "conversation.h"
#include "intent_trigger.h"
#include "translation_recognizer.h"
#include "keyword_spotter_model.h"
#include "luis_direct_lu_engine_adapter.h"
#include "language_understanding_model.h"
#include "phrase.h"
#include "phrase_list_grammar.h"
#include "recognition_event_args.h"
#include "recognition_result.h"
#include "recognizer.h"
#include "session_event_args.h"
#include "connection_event_args.h"
#include "connection_message.h"
#include "connection_message_event_args.h"
#include "usp_reco_engine_adapter.h"
#include "dialog_service_connector.h"
#include "connection.h"
#include "user.h"
#include "participant.h"
#include "stored_grammar.h"
#include "class_language_model.h"
#include "speech_config.h"
#include "speech_translation_config.h"
#include "auto_detect_source_lang_config.h"
#include "source_lang_config.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


SPX_EXTERN_C void* SRLib_CreateModuleObject(const char* className, const char* interfaceName)
{
    SPX_FACTORY_MAP_BEGIN();
    SPX_FACTORY_MAP_ENTRY(CSpxAudioStreamSession, ISpxSession);
    SPX_FACTORY_MAP_ENTRY(CSpxRecognitionEventArgs, ISpxRecognitionEventArgs);
    SPX_FACTORY_MAP_ENTRY(CSpxActivityEventArgs, ISpxActivityEventArgs);
    SPX_FACTORY_MAP_ENTRY(CSpxRecognitionResult, ISpxRecognitionResult);
    SPX_FACTORY_MAP_ENTRY(CSpxRecognizer, ISpxRecognizer);
    SPX_FACTORY_MAP_ENTRY(CSpxIntentRecognizer, ISpxRecognizer);
    SPX_FACTORY_MAP_ENTRY(CSpxDialogServiceConnector, ISpxRecognizer);
    SPX_FACTORY_MAP_ENTRY(CSpxConversationTranscriber, ISpxRecognizer);
    SPX_FACTORY_MAP_ENTRY(CSpxConversation, ISpxConversation);
    SPX_FACTORY_MAP_ENTRY(CSpxParticipant, ISpxParticipant);
    SPX_FACTORY_MAP_ENTRY(CSpxUser, ISpxUser);
    SPX_FACTORY_MAP_ENTRY(CSpxIntentTrigger, ISpxTrigger);
    SPX_FACTORY_MAP_ENTRY(CSpxKwsModel, ISpxKwsModel);
    SPX_FACTORY_MAP_ENTRY(CSpxLanguageUnderstandingModel, ISpxLanguageUnderstandingModel);
    SPX_FACTORY_MAP_ENTRY(CSpxPhrase, ISpxPhrase);
    SPX_FACTORY_MAP_ENTRY(CSpxPhraseListGrammar, ISpxPhraseList);
    SPX_FACTORY_MAP_ENTRY(CSpxTranslationRecognizer, ISpxRecognizer);
    SPX_FACTORY_MAP_ENTRY(CSpxSessionEventArgs, ISpxSessionEventArgs);
    SPX_FACTORY_MAP_ENTRY(CSpxUspRecoEngineAdapter, ISpxRecoEngineAdapter);
    SPX_FACTORY_MAP_ENTRY(CSpxUspCallbackWrapper, ISpxUspCallbacks);
    SPX_FACTORY_MAP_ENTRY(CSpxLuisDirectEngineAdapter, ISpxLuEngineAdapter);
    SPX_FACTORY_MAP_ENTRY(CSpxConnection, ISpxConnection);
    SPX_FACTORY_MAP_ENTRY(CSpxConnectionEventArgs, ISpxConnectionEventArgs);
    SPX_FACTORY_MAP_ENTRY(CSpxConnectionMessage, ISpxConnectionMessage);
    SPX_FACTORY_MAP_ENTRY(CSpxConnectionMessageEventArgs, ISpxConnectionMessageEventArgs);
    SPX_FACTORY_MAP_ENTRY(CSpxStoredGrammar, ISpxStoredGrammar);
    SPX_FACTORY_MAP_ENTRY(CSpxClassLanguageModel, ISpxClassLanguageModel);
    SPX_FACTORY_MAP_ENTRY(CSpxSpeechConfig, ISpxSpeechConfig);
    SPX_FACTORY_MAP_ENTRY(CSpxSpeechTranslationConfig, ISpxSpeechConfig);
    SPX_FACTORY_MAP_ENTRY(CSpxAutoDetectSourceLangConfig, ISpxAutoDetectSourceLangConfig);
    SPX_FACTORY_MAP_ENTRY(CSpxSourceLanguageConfig, ISpxSourceLanguageConfig);
    SPX_FACTORY_MAP_END();
}


} } } } // Microsoft::CognitiveServices::Speech::Impl
