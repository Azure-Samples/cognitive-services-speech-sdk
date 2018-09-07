//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_c_result.h: Public API declarations for Result related C methods and enumerations
//

#pragma once
#include <speechapi_c_common.h>

SPXAPI Result_GetResultId(SPXRESULTHANDLE hresult, char* pszResultId, uint32_t cchResultId);

enum Result_RecognitionReason { Reason_Recognized, Reason_IntermediateResult, Reason_NoMatch, Reason_InitialSilenceTimeout, Reason_InitialBabbleTimeout, Reason_Canceled };
typedef enum Result_RecognitionReason Result_RecognitionReason;
SPXAPI Result_GetRecognitionReason(SPXRESULTHANDLE hresult, Result_RecognitionReason* preason);

SPXAPI Result_GetText(SPXRESULTHANDLE hresult, char* pszText, uint32_t cchText);
SPXAPI Result_GetOffset(SPXRESULTHANDLE hresult, uint64_t* offset);
SPXAPI Result_GetDuration(SPXRESULTHANDLE hresult, uint64_t* duration);

SPXAPI result_get_property_bag(SPXRESULTHANDLE hresult, SPXPROPERTYBAGHANDLE* hpropbag);
