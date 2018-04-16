//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "stdafx.h"
#include "string_utils.h"


using namespace CARBON_IMPL_NAMESPACE();

SPXAPI TranslationResult_GetTranslationText(SPXRESULTHANDLE handle, wchar_t* textBuffer, uint32_t length)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, length == 0);

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto resulthandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognitionResult, SPXRESULTHANDLE>();
        auto result = (*resulthandles)[handle];

        auto textResult = SpxQueryInterface<ISpxTranslationTextResult>(result);

        (void)textBuffer;
        // Todo: get text out.
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

