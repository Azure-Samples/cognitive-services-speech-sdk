//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// create_module_object.cpp: Implementation definitions for *CreateModuleObject* methods
//

#include "stdafx.h"

#include "audio_stream_session.h"
#include "factory_helpers.h"
#include "intent_recognizer.h"
#include "intent_trigger.h"
#include "translation_recognizer.h"
#include "keyword_spotter_model.h"
#include "luis_direct_lu_engine_adapter.h"
#include "language_understanding_model.h"
#include "recognition_event_args.h"
#include "recognition_result.h"
#include "recognizer.h"
#include "session_event_args.h"
#include "usp_reco_engine_adapter.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


SPX_EXTERN_C void* SRLib_CreateModuleObject(const char* className, const char* interfaceName)
{
    SPX_FACTORY_MAP_BEGIN();
    SPX_FACTORY_MAP_ENTRY(CSpxAudioStreamSession, ISpxSession);
    SPX_FACTORY_MAP_ENTRY(CSpxRecognitionEventArgs, ISpxRecognitionEventArgs);
    SPX_FACTORY_MAP_ENTRY(CSpxRecognitionResult, ISpxRecognitionResult);
    SPX_FACTORY_MAP_ENTRY(CSpxRecognizer, ISpxRecognizer);
    SPX_FACTORY_MAP_ENTRY(CSpxIntentRecognizer, ISpxRecognizer);
    SPX_FACTORY_MAP_ENTRY(CSpxIntentTrigger, ISpxTrigger);
    SPX_FACTORY_MAP_ENTRY(CSpxKwsModel, ISpxKwsModel);
    SPX_FACTORY_MAP_ENTRY(CSpxLanguageUnderstandingModel, ISpxLanguageUnderstandingModel);
    SPX_FACTORY_MAP_ENTRY(CSpxTranslationRecognizer, ISpxRecognizer);
    SPX_FACTORY_MAP_ENTRY(CSpxSessionEventArgs, ISpxSessionEventArgs);
    SPX_FACTORY_MAP_ENTRY(CSpxUspRecoEngineAdapter, ISpxRecoEngineAdapter);
    SPX_FACTORY_MAP_ENTRY(CSpxUspCallbackWrapper, ISpxUspCallbacks);
    SPX_FACTORY_MAP_ENTRY(CSpxLuisDirectEngineAdapter, ISpxLuEngineAdapter);
    SPX_FACTORY_MAP_END();
}


} } } } // Microsoft::CognitiveServices::Speech::Impl
