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

        auto translationText = textResult->GetTranslationText();
        auto textStr = translationText.c_str();
        PAL::wcscpy(textBuffer, length, textStr, translationText.size(), true);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI TranslationResult_GetSourceLanguage(SPXRESULTHANDLE handle, wchar_t* sourceLanguageBuffer, uint32_t length)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, length == 0);

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto resulthandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognitionResult, SPXRESULTHANDLE>();
        auto result = (*resulthandles)[handle];

        auto textResult = SpxQueryInterface<ISpxTranslationTextResult>(result);

        auto lang = textResult->GetSourceLanguage();
        auto langStr = lang.c_str();
        PAL::wcscpy(sourceLanguageBuffer, length, langStr, lang.size(), true);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI TranslationResult_GetTargetLanguage(SPXRESULTHANDLE handle, wchar_t* targetLanguageBuffer, uint32_t length)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, length == 0);

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto resulthandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognitionResult, SPXRESULTHANDLE>();
        auto result = (*resulthandles)[handle];

        auto textResult = SpxQueryInterface<ISpxTranslationTextResult>(result);

        auto lang = textResult->GetTargetLanguage();
        auto langStr = lang.c_str();
        PAL::wcscpy(targetLanguageBuffer, length, langStr, lang.size(), true);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}


