//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_c_intent_result.cpp: Public API definitions for IntentResult related C methods
//

#include "stdafx.h"
#include "string_utils.h"
#include "handle_table.h"

using namespace Microsoft::CognitiveServices::Speech::Impl;

SPXAPI intent_result_get_intent_id(SPXRESULTHANDLE hresult, char* pszIntentId, uint32_t cchIntentId)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, cchIntentId == 0);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, pszIntentId == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto resulthandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognitionResult, SPXRESULTHANDLE>();
        auto result = (*resulthandles)[hresult];

        auto intentResult = SpxQueryInterface<ISpxIntentRecognitionResult>(result);

        auto strActual = PAL::ToString(intentResult->GetIntentId());
        auto pszActual = strActual.c_str();
        PAL::strcpy(pszIntentId, cchIntentId, pszActual, strActual.size(), true);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}
