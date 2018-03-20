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


using namespace CARBON_IMPL_NAMESPACE();


SPXAPI_(bool) IntentTrigger_Handle_IsValid(SPXTRIGGERHANDLE htrigger)
{
     return Handle_IsValid<SPXTRIGGERHANDLE, ISpxTrigger>(htrigger);
}

SPXAPI IntentTrigger_Handle_Close(SPXTRIGGERHANDLE htrigger)
{
    return Handle_Close<SPXTRIGGERHANDLE, ISpxTrigger>(htrigger);
}

SPXAPI IntentTrigger_Create_From_Phrase(const wchar_t* phrase, SPXTRIGGERHANDLE* phtrigger)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        *phtrigger = SPXHANDLE_INVALID;

        auto trigger = SpxCreateObjectWithSite<ISpxTrigger>("CSpxIntentTrigger", SpxGetRootSite());
        trigger->InitPhraseTrigger(phrase);
        
        auto triggerhandles = CSpxSharedPtrHandleTableManager::Get<ISpxTrigger, SPXTRIGGERHANDLE>();
        *phtrigger = triggerhandles->TrackHandle(trigger);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI IntentTrigger_Create_From_LuisModel(SPXLUISHANDLE hluis, SPXTRIGGERHANDLE* phtrigger)
{
    return IntentTrigger_Create_From_LuisModel_Intent(hluis, nullptr, phtrigger);
}

SPXAPI IntentTrigger_Create_From_LuisModel_Intent(SPXLUISHANDLE hluis, const wchar_t* intentName, SPXTRIGGERHANDLE* phtrigger)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        *phtrigger = SPXHANDLE_INVALID;

        auto luishandles = CSpxSharedPtrHandleTableManager::Get<ISpxLuisModel, SPXLUISHANDLE>();
        auto model = (*luishandles)[hluis];

        auto trigger = SpxCreateObjectWithSite<ISpxTrigger>("CSpxIntentTrigger", SpxGetRootSite());
        trigger->InitLuisModelTrigger(model, intentName);

        auto triggerhandles = CSpxSharedPtrHandleTableManager::Get<ISpxTrigger, SPXTRIGGERHANDLE>();
        *phtrigger = triggerhandles->TrackHandle(trigger);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}
