//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_c_intent_result.h: Public API declarations for IntentResult related C methods and enumerations
//

#pragma once
#include <speechapi_c_common.h>

SPXAPI intent_result_get_intent_id(SPXRESULTHANDLE hresult, char* pszIntentId, uint32_t cchIntentId);
