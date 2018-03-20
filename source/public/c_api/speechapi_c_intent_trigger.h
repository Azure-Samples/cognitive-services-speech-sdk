//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_c_intent_trigger.h: Public API declarations for IntentTrigger related C methods and typedefs
//

#pragma once
#include <speechapi_c_common.h>


SPXAPI_(bool) IntentTrigger_Handle_IsValid(SPXTRIGGERHANDLE htrigger);
SPXAPI IntentTrigger_Handle_Close(SPXTRIGGERHANDLE htrigger);

SPXAPI IntentTrigger_Create_From_Phrase(const wchar_t* phrase, SPXTRIGGERHANDLE* phtrigger);
SPXAPI IntentTrigger_Create_From_LuisModel(SPXLUISHANDLE hluis, SPXTRIGGERHANDLE* phtrigger);
SPXAPI IntentTrigger_Create_From_LuisModel_Intent(SPXLUISHANDLE hluis, const wchar_t* intentName, SPXTRIGGERHANDLE* phtrigger);
