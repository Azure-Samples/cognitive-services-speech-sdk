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


SPXAPI IntentRecognizer_AddIntent(SPXRECOHANDLE hreco, const char* intentId, SPXTRIGGERHANDLE htrigger)
{
    if (intentId == nullptr)
        return SPXERR_INVALID_ARG;

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto triggerhandles = CSpxSharedPtrHandleTableManager::Get<ISpxTrigger, SPXTRIGGERHANDLE>();
        auto trigger = (*triggerhandles)[htrigger];

        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        auto recognizer = (*recohandles)[hreco];

        auto intentRecognizer = SpxQueryInterface<ISpxIntentRecognizer>(recognizer);
        SPX_IFTRUE_THROW_HR(intentRecognizer == nullptr, SPXERR_INVALID_ARG);

        intentRecognizer->AddIntentTrigger(PAL::ToWString(intentId).c_str(), trigger);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}
