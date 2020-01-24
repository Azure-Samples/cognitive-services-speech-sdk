//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_c_intent_trigger.cpp: Public API definitions for IntentTrigger related C methods
//
#include "stdafx.h"
#include "create_object_helpers.h"
#include "handle_helpers.h"
#include "resource_manager.h"

using namespace Microsoft::CognitiveServices::Speech::Impl;

SPXAPI_(bool) intent_trigger_handle_is_valid(SPXTRIGGERHANDLE htrigger)
{
     return Handle_IsValid<SPXTRIGGERHANDLE, ISpxTrigger>(htrigger);
}

SPXAPI intent_trigger_create_from_phrase(SPXTRIGGERHANDLE* htrigger, const char* phrase)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, phrase == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, htrigger == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        *htrigger = SPXHANDLE_INVALID;

        auto trigger = SpxCreateObjectWithSite<ISpxTrigger>("CSpxIntentTrigger", SpxGetRootSite());
        trigger->InitPhraseTrigger(PAL::ToWString(phrase).c_str());

        auto triggerhandles = CSpxSharedPtrHandleTableManager::Get<ISpxTrigger, SPXTRIGGERHANDLE>();
        *htrigger = triggerhandles->TrackHandle(trigger);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI intent_trigger_create_from_language_understanding_model(SPXTRIGGERHANDLE* htrigger, SPXLUMODELHANDLE hlumodel, const char* intentName)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, htrigger == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        *htrigger = SPXHANDLE_INVALID;

        auto languageUnderstandingModelHandles = CSpxSharedPtrHandleTableManager::Get<ISpxLanguageUnderstandingModel, SPXLUMODELHANDLE>();
        auto model = (*languageUnderstandingModelHandles)[hlumodel];

        auto trigger = SpxCreateObjectWithSite<ISpxTrigger>("CSpxIntentTrigger", SpxGetRootSite());
        trigger->InitLanguageUnderstandingModelTrigger(model, intentName != nullptr
            ? PAL::ToWString(intentName).c_str()
            : L"");

        auto triggerhandles = CSpxSharedPtrHandleTableManager::Get<ISpxTrigger, SPXTRIGGERHANDLE>();
        *htrigger = triggerhandles->TrackHandle(trigger);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI intent_trigger_handle_release(SPXTRIGGERHANDLE htrigger)
{
    return Handle_Close<SPXTRIGGERHANDLE, ISpxTrigger>(htrigger);
}

