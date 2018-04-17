//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_c_intent_recognizer.cpp: Public API definitions for IntentRecognizer related C methods
//

#include "stdafx.h"
#include "event_helpers.h"
#include "handle_helpers.h"
#include "string_utils.h"


using namespace Microsoft::CognitiveServices::Speech::Impl;


SPXAPI TranslationRecognizer_TranslationSynthesis_SetEventCallback(SPXRECOHANDLE hreco, PTRANSLATIONSYNTHESIS_CALLBACK_FUNC pCallback, void* pvContext)
{
    return Recognizer_RecoEvent_SetCallback(&ISpxRecognizerEvents::TranslationSynthesisResult, hreco, pCallback, pvContext);
}
