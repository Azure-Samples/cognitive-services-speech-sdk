//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include <assert.h>
#include <cstring>
#include "stdafx.h"
#include "string_utils.h"
#include "speechapi_cxx_translation_result.h"

using namespace Microsoft::CognitiveServices::Speech::Impl;

SPXAPI translation_text_result_get_translation_text_buffer_header(SPXRESULTHANDLE handle, Result_TranslationTextBufferHeader* textBuffer, size_t* lengthPointer)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, lengthPointer == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto resulthandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognitionResult, SPXRESULTHANDLE>();
        auto result = (*resulthandles)[handle];

        auto textResult = SpxQueryInterface<ISpxTranslationTextResult>(result);
        auto translationPhrases = textResult->GetTranslationText();

        size_t entries = translationPhrases.size();
        size_t sizeInBytes = sizeof(Result_TranslationTextBufferHeader) + sizeof(char *) * entries * 2; /* space for targetLanguages and translationTexts array*/
        for (const auto& it : translationPhrases)
        {
            sizeInBytes += (PAL::ToString(it.first).size() + 1 + PAL::ToString(it.second).size() + 1) * sizeof(char);
        }

        if ((textBuffer == nullptr) || (*lengthPointer < sizeInBytes))
        {
            *lengthPointer = sizeInBytes;
            return SPXERR_BUFFER_TOO_SMALL;
        }

        textBuffer->bufferSize = sizeInBytes;
        textBuffer->numberEntries = entries;
        textBuffer->targetLanguages = reinterpret_cast<char**>(textBuffer + 1);
        textBuffer->translationTexts = textBuffer->targetLanguages + entries;
        auto data = reinterpret_cast<char*>(textBuffer->translationTexts + entries);
        size_t index = 0;
        for (const auto& it : translationPhrases)
        {
            std::string lang = PAL::ToString(it.first);
            std::string text = PAL::ToString(it.second);
            PAL::strcpy(data, lang.size() + 1, lang.c_str(), lang.size() + 1, true);
            textBuffer->targetLanguages[index] = data;
            data += lang.size() + 1;
            PAL::strcpy(data, text.size() + 1, text.c_str(), text.size() + 1, true);
            textBuffer->translationTexts[index] = data;
            data += text.size() + 1;
            index++;
        }
        *lengthPointer = sizeInBytes;
        if (reinterpret_cast<char *>(data) - reinterpret_cast<char *>(textBuffer) != (int)sizeInBytes)
        {
            SPX_THROW_HR(SPXERR_RUNTIME_ERROR);
        }
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI translation_synthesis_result_get_audio_data(SPXRESULTHANDLE handle, uint8_t* audioBuffer, size_t* lengthPointer)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, lengthPointer == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto resulthandle = CSpxSharedPtrHandleTableManager::Get<ISpxRecognitionResult, SPXRESULTHANDLE>();
        auto result = (*resulthandle)[handle];

        auto audioResult = SpxQueryInterface<ISpxTranslationSynthesisResult>(result);
        auto audioLength = audioResult->GetLength();

        if (audioLength == 0)
        {
            *lengthPointer = audioLength;
            SPX_RETURN_HR(hr);
        }

        if ((audioBuffer == nullptr) || (*lengthPointer < audioLength))
        {
            *lengthPointer = audioLength;
            return SPXERR_BUFFER_TOO_SMALL;
        }

        SPX_IFTRUE_THROW_HR(audioResult->GetAudio() == nullptr, SPXERR_RUNTIME_ERROR);
        std::memcpy(audioBuffer, audioResult->GetAudio(), audioLength);
        *lengthPointer = audioLength;
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}
