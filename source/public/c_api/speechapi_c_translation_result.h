//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#pragma once
#include <speechapi_c_common.h>

typedef enum { Result_Translation_Success, Result_Translation_Error } Result_TranslationStatus;
typedef enum { Result_Synthesis_Success, Result_Synthesis_End, Result_Synthesis_Error } Result_SynthesisStatus;

// Defines the header of the buffer that returns translation text results. The buffer starts with the header whose structure is
// defined below, and then follows the translation results for all required languages.
typedef struct _Result_TranslationTextBufferHeader {
    // The total size of the buffer, including translation results.
    size_t bufferSize;
    // The number of target languages. This indicates the array size of targetLanguages and translationTexts.
    size_t numberEntries;
    // Points to an array of target language names. The size of the array is numberEntries.
    wchar_t** targetLanguages;
    // Points to an array of translation texts. The element in the array is the translation text of the target language which has the same index in targetLanguages.
    wchar_t** translationTexts;
    // After that is the data buffer containing target language names and translation texts.
} Result_TranslationTextBufferHeader;

// textBuffer: point to the header of the buffer for storing translation result. The parameter lengthPointer points to the variable saving the size of buffer, which includes the textBuffer and
// the reserved space for storing results. On return, *lengthPointer is set to the size of the buffer returned. If textBuffer is nullptr or the length is smaller than
// the size required, the function returns SPXERR_BUFFER_TOO_SMALL.
SPXAPI TranslationTextResult_GetTranslationText(SPXRESULTHANDLE handle, Result_TranslationTextBufferHeader *textBuffer, size_t* lengthPointer);

SPXAPI TranslationTextResult_GetTranslationStatus(SPXRESULTHANDLE handle, Result_TranslationStatus* statusPointer);

SPXAPI TranslationTextResult_GetFailureReason(SPXRESULTHANDLE handle, wchar_t* buffer, size_t* bufferSizePointer);

// audioBuffer: point to the header for storing synthesis audio data. The parameter lengthPointer points to the variable saving the size of buffer. On return, *lengthPointer is set to the size of the buffer returned. 
// If textBuffer is nullptr or the length is smaller than the size required, the function returns SPXERR_BUFFER_TOO_SMALL.
SPXAPI TranslationSynthesisResult_GetSynthesisData(SPXRESULTHANDLE handle, uint8_t* audioBuffer, size_t* lengthPointer);

SPXAPI TranslationSynthesisResult_GetSynthesisStatus(SPXRESULTHANDLE handle, Result_SynthesisStatus* statusPointer);

SPXAPI TranslationSynthesisResult_GetFailureReason(SPXRESULTHANDLE handle, wchar_t* buffer, size_t* bufferSizePointer);
