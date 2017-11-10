#include "stdafx.h"


using namespace CARBON_IMPL_NAMESPACE();


SPXAPI Result_GetResultId(SPXRESULTHANDLE hresult, wchar_t* pszResultId, uint32_t cchResultId)
{
    return SPXERR_NOT_IMPL;
}

SPXAPI Result_GetRecognitionReason(SPXRESULTHANDLE hresult, Result_RecognitionReason* preason)
{
    return SPXERR_NOT_IMPL;
}

SPXAPI Result_GetText(SPXRESULTHANDLE hresult, wchar_t* pszText, uint32_t cchText)
{
    return SPXERR_NOT_IMPL;
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
