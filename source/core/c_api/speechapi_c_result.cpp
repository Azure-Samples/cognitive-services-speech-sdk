//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_c_result.cpp: Public API definitions for Result related C methods
//

#include "stdafx.h"
#include "string_utils.h"

using namespace Microsoft::CognitiveServices::Speech::Impl;

static_assert((int)Reason_Recognized == (int)Reason::Recognized, "Reason_* enum values == Reason::* enum values");
static_assert((int)Reason_IntermediateResult == (int)Reason::IntermediateResult, "Reason_* enum values == Reason::* enum values");
static_assert((int)Reason_NoMatch == (int)Reason::NoMatch, "Reason_* enum values == Reason::* enum values");
static_assert((int)Reason_InitialSilenceTimeout == (int)Reason::InitialSilenceTimeout, "Reason_* enum values == Reason::* enum values");
static_assert((int)Reason_InitialBabbleTimeout == (int)Reason::InitialBabbleTimeout, "Reason_* enum values == Reason::* enum values");
static_assert((int)Reason_Canceled == (int)Reason::Canceled, "Reason_* enum values == Reason::* enum values");

static_assert((int)ResultProperty_Json == (int)ResultProperty::Json, "ResultProperty_* enum values == ResultProperty::* enum values");
static_assert((int)ResultProperty_LanguageUnderstandingJson == (int)ResultProperty::LanguageUnderstandingJson, "ResultProperty_* enum values == ResultProperty::* enum values");
static_assert((int)ResultProperty_ErrorDetails == (int)ResultProperty::ErrorDetails, "ResultProperty_* enum values == ResultProperty::* enum values");

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

SPXAPI Result_GetOffset(SPXRESULTHANDLE hresult, uint64_t* offset)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, offset == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto resulthandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognitionResult, SPXRESULTHANDLE>();
        auto result = (*resulthandles)[hresult];
        *offset = result->GetOffset();
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI Result_GetDuration(SPXRESULTHANDLE hresult, uint64_t* duration)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, duration == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto resulthandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognitionResult, SPXRESULTHANDLE>();
        auto result = (*resulthandles)[hresult];
        *duration = result->GetDuration();
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

            case ResultProperty_ErrorDetails:
                propertyName = g_RESULT_ErrorDetails;
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
