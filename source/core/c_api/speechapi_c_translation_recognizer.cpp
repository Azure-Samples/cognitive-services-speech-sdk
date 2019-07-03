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

SPXAPI translator_add_target_language(SPXRECOHANDLE hreco, const char* language)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, language == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        auto recognizer = (*recohandles)[hreco];

        auto translationRecognizer = SpxQueryInterface<ISpxTranslationRecognizer>(recognizer);
        SPX_IFTRUE_THROW_HR(translationRecognizer == nullptr, SPXERR_INVALID_ARG);

        translationRecognizer->AddTargetLanguage(language);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI translator_remove_target_language(SPXRECOHANDLE hreco, const char* language)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, language == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        auto recognizer = (*recohandles)[hreco];

        auto translationRecognizer = SpxQueryInterface<ISpxTranslationRecognizer>(recognizer);
        SPX_IFTRUE_THROW_HR(translationRecognizer == nullptr, SPXERR_INVALID_ARG);

        translationRecognizer->RemoveTargetLanguage(language);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}
