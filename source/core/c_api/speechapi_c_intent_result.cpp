//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_c_intent_result.cpp: Public API definitions for IntentResult related C methods
//

#include "stdafx.h"
#include "string_utils.h"


using namespace CARBON_IMPL_NAMESPACE();


SPXAPI IntentResult_GetIntentId(SPXRESULTHANDLE hresult, wchar_t* pszIntentId, uint32_t cchIntentId)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, cchIntentId == 0);

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto resulthandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognitionResult, SPXRESULTHANDLE>();
        auto result = (*resulthandles)[hresult];

        auto intentResult = SpxQueryInterface<ISpxIntentRecognitionResult>(result);

        auto strActual = intentResult->GetIntentId();
        auto pszActual = strActual.c_str();
        PAL::wcscpy(pszIntentId, cchIntentId, pszActual, strActual.size(), true);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}
