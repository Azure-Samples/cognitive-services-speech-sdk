//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_c_language_understanding_model.h: Public API declarations for LanguageUnderstandingModel related C methods and typedefs
//

#pragma once
#include <speechapi_c_common.h>


SPXAPI_(bool) LanguageUnderstandingModel_Handle_IsValid(SPXLUMODELHANDLE hlumodel);
SPXAPI LanguageUnderstandingModel_Handle_Close(SPXLUMODELHANDLE hlumodel);

SPXAPI LanguageUnderstandingModel_Create_From_Uri(const char* uri, SPXLUMODELHANDLE* phlumodel);
SPXAPI LanguageUnderstandingModel_Create_From_AppId(const char* appId, SPXLUMODELHANDLE* phlumodel);
SPXAPI LanguageUnderstandingModel_Create_From_Subscription(const char* subscriptionKey, const char* appId, const char* region, SPXLUMODELHANDLE* phlumodel);
