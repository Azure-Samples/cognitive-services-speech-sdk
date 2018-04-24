//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_c_result.h: Public API declarations for Result related C methods and enumerations
//

#pragma once
#include <speechapi_c_common.h>

SPXAPI Result_GetResultId(SPXRESULTHANDLE hresult, wchar_t* pszResultId, uint32_t cchResultId);

enum Result_RecognitionReason { Reason_Recognized, Reason_IntermediateResult, Reason_NoMatch, Reason_Canceled, Reason_OtherRecognizer };
typedef enum Result_RecognitionReason Result_RecognitionReason;
SPXAPI Result_GetRecognitionReason(SPXRESULTHANDLE hresult, Result_RecognitionReason* preason);

SPXAPI Result_GetText(SPXRESULTHANDLE hresult, wchar_t* pszText, uint32_t cchText);

enum Result_Property { ResultProperty_Json = 1,  ResultProperty_LanguageUnderstandingJson = 2, ResultProperty_ErrorDetails = 3 };
typedef enum Result_Property Result_Property;
SPXAPI Result_GetProperty_Name(Result_Property property, wchar_t* name, uint32_t cchName);

SPXAPI Result_GetProperty_String(SPXRESULTHANDLE hresult, const wchar_t* name, wchar_t* value, uint32_t cchValue, const wchar_t* defaultValue);
SPXAPI_(bool) Result_ContainsProperty_String(SPXRESULTHANDLE hresult, const wchar_t* name);

SPXAPI Result_GetProperty_Int32(SPXRESULTHANDLE hresult, const wchar_t* name, int32_t* pvalue, int32_t defaultValue);
SPXAPI_(bool) Result_ContainsProperty_Int32(SPXRESULTHANDLE hresult, const wchar_t* name);

SPXAPI Result_GetProperty_Bool(SPXRESULTHANDLE hresult, const wchar_t* name, bool* pvalue, bool defaultValue);
SPXAPI_(bool) Result_ContainsProperty_Bool(SPXRESULTHANDLE hresult, const wchar_t* name);
