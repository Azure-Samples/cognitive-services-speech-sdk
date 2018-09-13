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


SPXAPI translator_synthesizing_audio_set_callback(SPXRECOHANDLE hreco, PTRANSLATIONSYNTHESIS_CALLBACK_FUNC pCallback, void* pvContext)
{
    return recognizer_recognition_set_event_callback(&ISpxRecognizerEvents::TranslationSynthesisResult, hreco, pCallback, pvContext);
}
