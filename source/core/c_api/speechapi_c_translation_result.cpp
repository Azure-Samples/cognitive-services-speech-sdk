//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include <assert.h>
#include <cstring>
#include "stdafx.h"
#include "string_utils.h"

using namespace Microsoft::CognitiveServices::Speech::Impl;

SPXAPI TranslationTextResult_GetTranslationStatus(SPXRESULTHANDLE handle, Result_TranslationStatus* statusPointer)
{
    static_assert((int)Result_Translation_Success == (int)TranslationStatus::Success, "Result_Translation* enum values == TranslationStatus::* enum values");
    static_assert((int)Result_Translation_Error == (int)TranslationStatus::Error, "Result_Translation* enum values == TranslationStatus::* enum values");
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, statusPointer == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto resulthandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognitionResult, SPXRESULTHANDLE>();
        auto result = (*resulthandles)[handle];

        auto textResult = SpxQueryInterface<ISpxTranslationTextResult>(result);
        *statusPointer = static_cast<Result_TranslationStatus>(textResult->GetTranslationStatus());
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI_RESULTTYPE SPXAPI_NOTHROW CheckAndCopyBuffer(const std::wstring& source, wchar_t* buffer, size_t* bufferSizePointer)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto len = source.size() + 1;

        if ((buffer == nullptr) || (len > *bufferSizePointer))
        {
            *bufferSizePointer = len;
            return SPXERR_BUFFER_TOO_SMALL;
        }

        PAL::wcscpy(buffer, *bufferSizePointer, source.c_str(), len, true);
        *bufferSizePointer = len;
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI TranslationTextResult_GetFailureReason(SPXRESULTHANDLE handle, wchar_t* buffer, size_t* bufferSizePointer)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, bufferSizePointer == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto resulthandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognitionResult, SPXRESULTHANDLE>();
        auto result = (*resulthandles)[handle];

        auto textResult = SpxQueryInterface<ISpxTranslationTextResult>(result);
        auto reason = textResult->GetTranslationFailureReason();

        hr = CheckAndCopyBuffer(reason, buffer, bufferSizePointer);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI TranslationTextResult_GetTranslationText(SPXRESULTHANDLE handle, Result_TranslationTextBufferHeader* textBuffer, size_t* lengthPointer)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, lengthPointer == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto resulthandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognitionResult, SPXRESULTHANDLE>();
        auto result = (*resulthandles)[handle];

        auto textResult = SpxQueryInterface<ISpxTranslationTextResult>(result);
        auto translationPhrases = textResult->GetTranslationText();

        size_t entries = translationPhrases.size();
        size_t sizeInBytes = sizeof(Result_TranslationTextBufferHeader) + sizeof(wchar_t *) * entries * 2; /* space for targetLanguages and translationTexts array*/
        for (const auto& it : translationPhrases)
        {
            sizeInBytes += (it.first.size() + 1 + it.second.size() + 1) * sizeof(wchar_t);
        }

        if ((textBuffer == nullptr) || (*lengthPointer < sizeInBytes))
        {
            *lengthPointer = sizeInBytes;
            return SPXERR_BUFFER_TOO_SMALL;
        }

        textBuffer->bufferSize = sizeInBytes;
        textBuffer->numberEntries = entries;
        textBuffer->targetLanguages = reinterpret_cast<wchar_t **>(textBuffer + 1);
        textBuffer->translationTexts = textBuffer->targetLanguages + entries;
        auto data = reinterpret_cast<wchar_t *>(textBuffer->translationTexts + entries);
        size_t index = 0;
        for (const auto& it : translationPhrases)
        {
            std::wstring lang, text;
            std::tie(lang, text) = it;
            PAL::wcscpy(data, lang.size() + 1, lang.c_str(), lang.size() + 1, true);
            textBuffer->targetLanguages[index] = data;
            data += lang.size() + 1;
            PAL::wcscpy(data, text.size() + 1, text.c_str(), text.size() + 1, true);
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


SPXAPI TranslationSynthesisResult_GetSynthesisStatus(SPXRESULTHANDLE handle, Result_SynthesisStatus* statusPointer)
{
    static_assert((int)Result_Synthesis_Success == (int)SynthesisStatus::Success, "Result_TranslationSynthesis* enum values == TranslationSynthesisStatus::* enum values");
    static_assert((int)Result_Synthesis_End == (int)SynthesisStatus::SynthesisEnd, "Result_TranslationSynthesis* enum values == TranslationSynthesisStatus::* enum values");
    static_assert((int)Result_Synthesis_Error == (int)SynthesisStatus::Error, "Result_TranslationSynthesis* enum values == TranslationSynthesisStatus::* enum values");
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, statusPointer == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto resulthandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognitionResult, SPXRESULTHANDLE>();
        auto result = (*resulthandles)[handle];

        auto synthesisResult = SpxQueryInterface<ISpxTranslationSynthesisResult>(result);
        *statusPointer = static_cast<Result_SynthesisStatus>(synthesisResult->GetSynthesisStatus());
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI TranslationSynthesisResult_GetFailureReason(SPXRESULTHANDLE handle, wchar_t* buffer, size_t* bufferSizePointer)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, bufferSizePointer == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto resulthandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognitionResult, SPXRESULTHANDLE>();
        auto result = (*resulthandles)[handle];

        auto synthesisResult = SpxQueryInterface<ISpxTranslationSynthesisResult>(result);
        auto reason = synthesisResult->GetSynthesisFailureReason();

        hr = CheckAndCopyBuffer(reason, buffer, bufferSizePointer);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI TranslationSynthesisResult_GetSynthesisData(SPXRESULTHANDLE handle, uint8_t* audioBuffer, size_t* lengthPointer)
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
