//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_c_luis_model.h: Public API declarations for LuisModel related C methods and typedefs
//

#pragma once
#include <speechapi_c_common.h>


SPXAPI_(bool) LuisModel_Handle_IsValid(SPXLUISHANDLE hluis);
SPXAPI LuisModel_Handle_Close(SPXLUISHANDLE hluis);

SPXAPI LuisModel_Create_From_Uri(const wchar_t* uri, SPXLUISHANDLE* phluis);
SPXAPI LuisModel_Create_From_Subscription(const wchar_t* hostName, const wchar_t* subscriptionKey, const wchar_t* appId, SPXLUISHANDLE* phluis);
