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


SPXAPI intent_recognizer_add_intent(SPXRECOHANDLE hreco, const char* intentId, SPXTRIGGERHANDLE htrigger)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto triggerhandles = CSpxSharedPtrHandleTableManager::Get<ISpxTrigger, SPXTRIGGERHANDLE>();
        auto trigger = (*triggerhandles)[htrigger];

        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        auto recognizer = (*recohandles)[hreco];

        auto intentRecognizer = SpxQueryInterface<ISpxIntentRecognizer>(recognizer);
        SPX_IFTRUE_THROW_HR(intentRecognizer == nullptr, SPXERR_INVALID_ARG);

        intentRecognizer->AddIntentTrigger(intentId == nullptr ? nullptr : PAL::ToWString(intentId).c_str(), trigger);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}
