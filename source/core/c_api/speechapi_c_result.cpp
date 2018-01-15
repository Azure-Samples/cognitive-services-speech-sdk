#include "stdafx.h"
#include "string_utils.h"


using namespace CARBON_IMPL_NAMESPACE();


SPXAPI Result_GetResultId(SPXRESULTHANDLE hresult, wchar_t* pszResultId, uint32_t cchResultId)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, cchResultId == 0);

    SPXAPI_INIT_TRY(hr)
    {
        auto resulthandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognitionResult, SPXRESULTHANDLE>();
        auto result = (*resulthandles)[hresult];

        auto strActual = result->GetResultId();
        auto pszActual = strActual.c_str();
        PAL::wcscpy(pszResultId, cchResultId, pszActual, strActual.size(), true);
    }
    SPXAPI_CATCH_AND_RETURN(hr);
}

SPXAPI Result_GetRecognitionReason(SPXRESULTHANDLE hresult, Result_RecognitionReason* preason)
{
    static_assert((int)Reason_NoMatch == (int)Reason::NoMatch, "Reason_* enum values == Reason::* enum values");
    static_assert((int)Reason_Canceled == (int)Reason::Canceled, "Reason_* enum values == Reason::* enum values");
    static_assert((int)Reason_Recognized == (int)Reason::Recognized, "Reason_* enum values == Reason::* enum values");
    static_assert((int)Reason_OtherRecognizer == (int)Reason::OtherRecognizer, "Reason_* enum values == Reason::* enum values");
    static_assert((int)Reason_IntermediateResult == (int)Reason::IntermediateResult, "Reason_* enum values == Reason::* enum values");

    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, preason == nullptr);

    SPXAPI_INIT_TRY(hr)    
    {
        auto resulthandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognitionResult, SPXRESULTHANDLE>();
        auto result = (*resulthandles)[hresult];
        *preason = (Result_RecognitionReason)result->GetReason();
    }
    SPXAPI_CATCH_AND_RETURN(hr);
}

SPXAPI Result_GetText(SPXRESULTHANDLE hresult, wchar_t* pszText, uint32_t cchText)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, cchText == 0);

    SPXAPI_INIT_TRY(hr)    
    {
        auto resulthandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognitionResult, SPXRESULTHANDLE>();
        auto result = (*resulthandles)[hresult];

        auto strActual = result->GetText();
        auto pszActual = strActual.c_str();
        PAL::wcscpy(pszText, cchText, pszActual, strActual.size(), true);
    }
    SPXAPI_CATCH_AND_RETURN(hr);
}

SPXAPI Result_Payload_GetCount(SPXRESULTHANDLE hresult, uint32_t* pcount)
{
    return SPXERR_NOT_IMPL;
}

SPXAPI Result_Payload_GetKey(SPXRESULTHANDLE hresult, uint32_t i, wchar_t* pkey, uint32_t cchKey)
{
    return SPXERR_NOT_IMPL;
}

SPXAPI Result_Payload_GetByIndex_String(SPXRESULTHANDLE hresult, uint32_t index, wchar_t* pvalue, uint32_t cchValue)
{
    return SPXERR_NOT_IMPL;
}

SPXAPI Result_Payload_GetByIndex_Int32(SPXRESULTHANDLE hresult, uint32_t index, uint32_t* pvalue)
{
    return SPXERR_NOT_IMPL;
}

SPXAPI_(bool) Result_Payload_TryGetByIndex_String(SPXRESULTHANDLE hresult, uint32_t index, wchar_t* pvalue, uint32_t cchValue)
{
    return false;
}

SPXAPI_(bool) Result_Payload_TryGetByIndex_Int32(SPXRESULTHANDLE hresult, uint32_t index, uint32_t* pvalue)
{
    return false;
}

SPXAPI Result_Payload_ContainsKey(SPXRESULTHANDLE hresult, const wchar_t* name)
{
    return SPXERR_NOT_IMPL;
}

SPXAPI Result_Payload_GetByName_String(SPXRESULTHANDLE hresult, const wchar_t* name, wchar_t* pvalue, uint32_t cchValue)
{
    return SPXERR_NOT_IMPL;
}

SPXAPI Result_Payload_GetByName_Int32(SPXRESULTHANDLE hresult, const wchar_t* name, uint32_t* pvalue)
{
    return SPXERR_NOT_IMPL;
}

SPXAPI_(bool) Result_Payload_TryGetByName_String(SPXRESULTHANDLE hresult, const wchar_t* name, wchar_t* pvalue, uint32_t cchValue)
{
    return false;
}

SPXAPI_(bool) Result_Payload_TryGetByName_Int32(SPXRESULTHANDLE hresult, const wchar_t* name, uint32_t* pvalue)
{
    return false;
}

SPXAPI Result_Payload_GetByEnum_String(SPXRESULTHANDLE hresult, Payload_Item item, wchar_t* pvalue, uint32_t cchValue)
{
    return SPXERR_NOT_IMPL;
}

SPXAPI Result_Payload_GetByEnum_Int32(SPXRESULTHANDLE hresult, Payload_Item item, uint32_t* pvalue)
{
    return SPXERR_NOT_IMPL;
}

SPXAPI_(bool) Result_Payload_TryGetByEnum_String(SPXRESULTHANDLE hresult, Payload_Item item, wchar_t* pvalue, uint32_t cchValue)
{
    return false;
}

SPXAPI_(bool) Result_Payload_TryGetByEnum_Int32(SPXRESULTHANDLE hresult, Payload_Item item, uint32_t* pvalue)
{
    return false;
}
