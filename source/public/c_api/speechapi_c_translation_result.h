//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#pragma once
#include <speechapi_c_common.h>

/// <summary>
/// Defines the header of the buffer that returns translation text results. The buffer starts with the header whose structure is
/// defined below, and then follows the translation results for all required languages.
/// </summary>
typedef struct _Result_TranslationTextBufferHeader {
    /// <summary>
    /// The total size of the buffer, including translation results.
    /// </summary>
    size_t bufferSize;
    /// <summary>
    /// The number of target languages. This indicates the array size of targetLanguages and translationTexts.
    /// </summary>
    size_t numberEntries;
    /// <summary>
    /// Points to an array of target language names. The size of the array is numberEntries.
    /// </summary>
    char** targetLanguages;
    /// <summary>
    /// Points to an array of translation texts. The element in the array is the translation text of the target language which has the same index in targetLanguages.
    /// </summary>
    char** translationTexts;
    // After that is the data buffer containing target language names and translation texts.
} Result_TranslationTextBufferHeader;

// textBuffer: point to the header of the buffer for storing translation result. The parameter lengthPointer points to the variable saving the size of buffer, which includes the textBuffer and
// the reserved space for storing results. On return, *lengthPointer is set to the size of the buffer returned. If textBuffer is nullptr or the length is smaller than
// the size required, the function returns SPXERR_BUFFER_TOO_SMALL.
SPXAPI translation_text_result_get_translation_text_buffer_header(SPXRESULTHANDLE handle, Result_TranslationTextBufferHeader *textBuffer, size_t* lengthPointer);

// audioBuffer: point to the header for storing synthesis audio data. The parameter lengthPointer points to the variable saving the size of buffer. On return, *lengthPointer is set to the size of the buffer returned. 
// If textBuffer is nullptr or the length is smaller than the size required, the function returns SPXERR_BUFFER_TOO_SMALL.
SPXAPI translation_synthesis_result_get_audio_data(SPXRESULTHANDLE handle, uint8_t* audioBuffer, size_t* lengthPointer);
