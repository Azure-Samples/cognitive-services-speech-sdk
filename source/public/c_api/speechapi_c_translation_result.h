//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#pragma once
#include <speechapi_c_common.h>


enum Result_TranslationStatus { Result_TranslationSuccess, Result_TranslationError };

SPXAPI TranslationResult_GetTranslationText(SPXRESULTHANDLE handle, wchar_t* textBuffer, uint32_t length);
SPXAPI TranslationResult_GetSourceLanguage(SPXRESULTHANDLE handle, wchar_t* sourceLanguageBuffer, uint32_t length);
SPXAPI TranslationResult_GetTargetLanguage(SPXRESULTHANDLE handle, wchar_t* targetLanguageBuffer, uint32_t length);

// Todo: check how to expose translation status after it is defined in core.
// SPXAPI TranslationResult_GetTranslationStatus(SPXRESULTHANDLE handle, Result_TranslationStatus* statusPointer);

// Todo: API to get audio output.
