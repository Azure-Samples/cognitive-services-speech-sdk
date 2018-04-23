//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_c_result.cpp: Public API definitions for Result related C methods
//

#include "stdafx.h"
#include "string_utils.h"
#include "named_properties_constants.h"


using namespace Microsoft::CognitiveServices::Speech::Impl;


SPXAPI Result_GetResultId(SPXRESULTHANDLE hresult, wchar_t* pszResultId, uint32_t cchResultId)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, cchResultId == 0);

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto resulthandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognitionResult, SPXRESULTHANDLE>();
        auto result = (*resulthandles)[hresult];

        auto strActual = result->GetResultId();
        auto pszActual = strActual.c_str();
        PAL::wcscpy(pszResultId, cchResultId, pszActual, strActual.size(), true);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI Result_GetRecognitionReason(SPXRESULTHANDLE hresult, Result_RecognitionReason* preason)
{
    static_assert((int)Reason_NoMatch == (int)Reason::NoMatch, "Reason_* enum values == Reason::* enum values");
    static_assert((int)Reason_Canceled == (int)Reason::Canceled, "Reason_* enum values == Reason::* enum values");
    static_assert((int)Reason_Recognized == (int)Reason::Recognized, "Reason_* enum values == Reason::* enum values");
    static_assert((int)Reason_OtherRecognizer == (int)Reason::OtherRecognizer, "Reason_* enum values == Reason::* enum values");
    static_assert((int)Reason_IntermediateResult == (int)Reason::IntermediateResult, "Reason_* enum values == Reason::* enum values");

    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, preason == nullptr);

    SPXAPI_INIT_HR_TRY(hr)    
    {
        auto resulthandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognitionResult, SPXRESULTHANDLE>();
        auto result = (*resulthandles)[hresult];
        *preason = (Result_RecognitionReason)result->GetReason();
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI Result_GetText(SPXRESULTHANDLE hresult, wchar_t* pszText, uint32_t cchText)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, cchText == 0);

    SPXAPI_INIT_HR_TRY(hr)    
    {
        auto resulthandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognitionResult, SPXRESULTHANDLE>();
        auto result = (*resulthandles)[hresult];

        auto strActual = result->GetText();
        auto pszActual = strActual.c_str();
        PAL::wcscpy(pszText, cchText, pszActual, strActual.size(), true);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI Result_GetProperty_Name(Result_Property property, wchar_t* name, uint32_t cchName)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        const wchar_t* propertyName = L"";
        switch (property)
        {
            case ResultProperty_Json:
                propertyName = g_RESULT_Json;
                break;
            
            case ResultProperty_LanguageUnderstandingJson:
                propertyName = g_RESULT_LanguageUnderstandingJson;
                break;

            default:
                hr = SPXERR_INVALID_ARG;
                break;
        }

        PAL::wcscpy(name, cchName, propertyName, wcslen(propertyName), true);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI Result_GetProperty_String(SPXRESULTHANDLE hresult, const wchar_t* name, wchar_t* value, uint32_t cchValue, const wchar_t* defaultValue)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto resulthandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognitionResult, SPXRESULTHANDLE>();
        auto result = (*resulthandles)[hresult];

        auto namedProperties = SpxQueryInterface<ISpxNamedProperties>(result);
        auto tempValue = namedProperties->GetStringValue(name, defaultValue);

        PAL::wcscpy(value, cchValue, tempValue.c_str(), tempValue.size(), true);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI_(bool) Result_ContainsProperty_String(SPXRESULTHANDLE hresult, const wchar_t* name)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto resulthandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognitionResult, SPXRESULTHANDLE>();
        auto result = (*resulthandles)[hresult];

        auto namedProperties = SpxQueryInterface<ISpxNamedProperties>(result);
        return namedProperties->HasStringValue(name);
    }
    SPXAPI_CATCH_AND_RETURN(hr, false);
}

SPXAPI Result_GetProperty_Int32(SPXRESULTHANDLE hresult, const wchar_t* name, int32_t* pvalue, int32_t defaultValue)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto resulthandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognitionResult, SPXRESULTHANDLE>();
        auto result = (*resulthandles)[hresult];

        auto namedProperties = SpxQueryInterface<ISpxNamedProperties>(result);
        auto tempValue = namedProperties->GetNumberValue(name, defaultValue);

        *pvalue = (int32_t)tempValue;
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI_(bool) Result_ContainsProperty_Int32(SPXRESULTHANDLE hresult, const wchar_t* name)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto resulthandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognitionResult, SPXRESULTHANDLE>();
        auto result = (*resulthandles)[hresult];

        auto namedProperties = SpxQueryInterface<ISpxNamedProperties>(result);
        return namedProperties->HasNumberValue(name);
    }
    SPXAPI_CATCH_AND_RETURN(hr, false);
}

SPXAPI Result_GetProperty_Bool(SPXRESULTHANDLE hresult, const wchar_t* name, bool* pvalue, bool defaultValue)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto resulthandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognitionResult, SPXRESULTHANDLE>();
        auto result = (*resulthandles)[hresult];

        auto namedProperties = SpxQueryInterface<ISpxNamedProperties>(result);
        auto tempValue = namedProperties->GetBooleanValue(name, defaultValue);

        *pvalue = !!tempValue;
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI_(bool) Result_ContainsProperty_Bool(SPXRESULTHANDLE hresult, const wchar_t* name)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto resulthandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognitionResult, SPXRESULTHANDLE>();
        auto result = (*resulthandles)[hresult];

        auto namedProperties = SpxQueryInterface<ISpxNamedProperties>(result);
        return namedProperties->HasBooleanValue(name);
    }
    SPXAPI_CATCH_AND_RETURN(hr, false);
}
