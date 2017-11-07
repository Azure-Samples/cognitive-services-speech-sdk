#pragma once
#include <speechapi_c_common.h>

SPXAPI Result_GetResultId(SPXRESULTHANDLE hresult, wchar_t* pszResultId, uint32_t cchResultId);

enum Result_RecognitionReason { Reason_Recognized, Reason_NoMatch, Reason_Canceled, Reason_OtherRecognizer };
SPXAPI Result_GetRecognitionReason(SPXRESULTHANDLE hresult, Result_RecognitionReason* preason);

SPXAPI Result_GetText(SPXRESULTHANDLE hresult, wchar_t* pszText, uint32_t cchText);

SPXAPI Result_Payload_GetCount(SPXRESULTHANDLE hresult, uint32_t* pcount);
SPXAPI Result_Payload_GetKey(SPXRESULTHANDLE hresult, uint32_t i, wchar_t* pkey, uint32_t cchKey);

SPXAPI Result_Payload_GetByIndex_String(SPXRESULTHANDLE hresult, uint32_t index, wchar_t* pvalue, uint32_t cchValue);
SPXAPI Result_Payload_GetByIndex_Int32(SPXRESULTHANDLE hresult, uint32_t index, uint32_t* pvalue);

SPXAPI_(bool) Result_Payload_TryGetByIndex_String(SPXRESULTHANDLE hresult, uint32_t index, wchar_t* pvalue, uint32_t cchValue);
SPXAPI_(bool) Result_Payload_TryGetByIndex_Int32(SPXRESULTHANDLE hresult, uint32_t index, uint32_t* pvalue);

SPXAPI Result_Payload_ContainsKey(SPXRESULTHANDLE hresult, const wchar_t* name);

SPXAPI Result_Payload_GetByName_String(SPXRESULTHANDLE hresult, const wchar_t* name, wchar_t* pvalue, uint32_t cchValue);
SPXAPI Result_Payload_GetByName_Int32(SPXRESULTHANDLE hresult, const wchar_t* name, uint32_t* pvalue);

SPXAPI_(bool) Result_Payload_TryGetByName_String(SPXRESULTHANDLE hresult, const wchar_t* name, wchar_t* pvalue, uint32_t cchValue);
SPXAPI_(bool) Result_Payload_TryGetByName_Int32(SPXRESULTHANDLE hresult, const wchar_t* name, uint32_t* pvalue);

enum Payload_Item { Payload_NormalizedText, Payload_InverseNormalizedText, Payload_DictationSpacingBefore, Payload_DictationSpacingAfter, Payload_SpeakerId, Payload_LanguageId };

SPXAPI Result_Payload_GetByEnum_String(SPXRESULTHANDLE hresult, Payload_Item item, wchar_t* pvalue, uint32_t cchValue);
SPXAPI Result_Payload_GetByEnum_Int32(SPXRESULTHANDLE hresult, Payload_Item item, uint32_t* pvalue);

SPXAPI_(bool) Result_Payload_TryGetByEnum_String(SPXRESULTHANDLE hresult, Payload_Item item, wchar_t* pvalue, uint32_t cchValue);
SPXAPI_(bool) Result_Payload_TryGetByEnum_Int32(SPXRESULTHANDLE hresult, Payload_Item item, uint32_t* pvalue);
