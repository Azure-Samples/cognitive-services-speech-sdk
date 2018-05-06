//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_cxx_translation_result.h: Public API declarations for TranslationResult C++ class
//

#pragma once
#include <string>
#include <vector>
#include <map>
#include <new>
#include <speechapi_cxx_common.h>
#include <speechapi_c.h>
#include <speechapi_cxx_recognition_result.h>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Translation {

/// <summary>
/// Defines the status code of translation result.
/// </summary>
enum class TranslationStatusCode {
    /// <summary>
    /// The translation is successful.
    /// </summary>
    Success,

    /// <summary>
    /// An error occurred during translation.
    /// </summary>
    Error
};


/// <summary> 
/// Defines the translation text result.
/// </summary>
class TranslationTextResult final : public Microsoft::CognitiveServices::Speech::SpeechRecognitionResult
{

private:
    TranslationStatusCode m_translationStatus;
    std::map<std::wstring, std::wstring> m_translations;
    std::wstring m_failureReason;

public:

    /// <summary>
    /// Describes the status of translation result.
    /// </summary>
    const TranslationStatusCode& TranslationStatus;

    /// <summary>
    /// It is intended for internal use only. It creates an instance of <see cref="TranslationTextResult"/>.
    /// </summary>
    /// <param name="resultHandle">The handle of the result returned by recognizer in C-API.</param>
    explicit TranslationTextResult(SPXRESULTHANDLE resultHandle) :
        SpeechRecognitionResult(resultHandle),
        TranslationStatus(m_translationStatus),
        Translations(m_translations),
        FailureReason(m_failureReason)
    {
        PopulateResultFields(resultHandle);
        SPX_DBG_TRACE_VERBOSE("%s (this=0x%x, handle=0x%x) -- resultid=%ls; translation status=0x%x.", 
            __FUNCTION__, this, Handle, ResultId.c_str(), TranslationStatus);
    };

    /// <summary>
    /// Destructs the instance.
    /// </summary>
    virtual ~TranslationTextResult()
    {
        SPX_DBG_TRACE_VERBOSE("%s (this-0x%x, handle=0x%x)", __FUNCTION__, this, Handle);
    };

    /// <summary>
    /// Presents the translation results. Each item in the map is a key value pair, where key is the language tag of the translated text,
    /// and value is the translation text in that language.
    /// </summary>
    const std::map<std::wstring, std::wstring>& Translations;

    /// <summary>
    /// Contains failure reason if TranslationStatus indicates an error. Otherwise it is empty.
    /// </summary>
    const std::wstring& FailureReason;

private:

    void PopulateResultFields(SPXRESULTHANDLE resultHandle)
    {
        static_assert((int)Result_Translation_Success == (int)::Microsoft::CognitiveServices::Speech::Translation::TranslationStatusCode::Success, "Result_Translation* enum values == TranslationStatus::* enum values");
        static_assert((int)Result_Translation_Error == (int)::Microsoft::CognitiveServices::Speech::Translation::TranslationStatusCode::Error, "Result_Translation* enum values == TranslationStatus::* enum values");

        SPX_INIT_HR(hr);

        Result_TranslationStatus status;
        SPX_THROW_ON_FAIL(hr = TranslationTextResult_GetTranslationStatus(resultHandle, &status));
        m_translationStatus = static_cast<::Microsoft::CognitiveServices::Speech::Translation::TranslationStatusCode>(status);

        size_t bufLen = 0;
        std::unique_ptr<wchar_t[]> reasonBuffer;
        hr = TranslationTextResult_GetFailureReason(resultHandle, nullptr, &bufLen);
        if (hr == SPXERR_BUFFER_TOO_SMALL)
        {
            reasonBuffer = std::make_unique<wchar_t[]>(bufLen);
            hr = TranslationTextResult_GetFailureReason(resultHandle, reasonBuffer.get(), &bufLen);
        }
        SPX_THROW_ON_FAIL(hr);
        if (bufLen != 0)
        {
            m_failureReason = std::wstring(reasonBuffer.get());
        }

        bufLen = 0;
        std::unique_ptr<Result_TranslationTextBufferHeader> phraseBuffer;
        // retrieve the required buffer size first.
        hr = TranslationTextResult_GetTranslationText(resultHandle, nullptr, &bufLen);
        if (hr == SPXERR_BUFFER_TOO_SMALL)
        {
            phraseBuffer = std::unique_ptr<Result_TranslationTextBufferHeader>((Result_TranslationTextBufferHeader *)(new char[bufLen]));
            hr = TranslationTextResult_GetTranslationText(resultHandle, phraseBuffer.get(), &bufLen);
        }
        SPX_THROW_ON_FAIL(hr);

        if (phraseBuffer->bufferSize > bufLen)
        {
            SPX_THROW_HR(SPXERR_RUNTIME_ERROR);
        }

        for (size_t i = 0; i < phraseBuffer->numberEntries; i++)
        {
            m_translations[std::wstring(phraseBuffer->targetLanguages[i])] = std::wstring(phraseBuffer->translationTexts[i]);
        }

        SPX_DBG_TRACE_VERBOSE("Translation phrases: numberentries: %d", (int)m_translations.size());
#ifdef _DEBUG
        for (const auto& cf : m_translations)
        {
            (void)(cf); // prevent warning for cf when compiling release builds
            SPX_DBG_TRACE_VERBOSE(" phrase for %ls: %ls", cf.first.c_str(), cf.second.c_str());
        }
#endif
    };

    DISABLE_DEFAULT_CTORS(TranslationTextResult);
};

/// <summary>
/// Defines the status code of synthesis result.
/// </summary>
enum class SynthesisStatusCode {
    /// <summary>
    /// The audio data contained in the message is valid.
    /// </summary>
    Success,

    /// <summary>
    /// Indicates the end of audio data. No audio data is included in this message.
    /// </summary>
    SynthesisEnd,

    /// <summary>
    /// An error occurred during translation.
    /// </summary>
    Error
};

/// <summary>
/// Defines the translation synthesis result, i.e. the voice output of the translated text in the target language.
/// </summary>
class TranslationSynthesisResult
{
private:
    SynthesisStatusCode m_synthesisStatus;
    std::vector<uint8_t> m_audioData;
    std::wstring m_failureReason;

public:
    /// <summary>
    /// It is intended for internal use only. It creates an instance of <see cref="TranslationSynthesisResult"/>
    /// </summary>
    /// <param name="resultHandle">The handle of the result returned by recognizer in C-API.</param>
    explicit TranslationSynthesisResult(SPXRESULTHANDLE resultHandle) :
        SynthesisStatus(m_synthesisStatus),
        Audio(m_audioData),
        FailureReason(m_failureReason)
    {
        PopulateResultFields(resultHandle);
        SPX_DBG_TRACE_VERBOSE("%s (this=0x%x) -- synthesis status=0x%x.", __FUNCTION__, this, SynthesisStatus);
    };

    /// <summary>
    /// Deconstructs the instance.
    /// </summary>
    virtual ~TranslationSynthesisResult()
    {
        SPX_DBG_TRACE_VERBOSE("%s (this-0x%x)", __FUNCTION__, this);
    };

    /// <summary>
    /// Describes the status of translation synthesis status.
    /// </summary>
    const SynthesisStatusCode& SynthesisStatus;

    /// <summary>
    /// The voice output of the translated text in the target language.
    /// </summary>
    const std::vector<uint8_t>& Audio;

    /// <summary>
    /// Contains failure reason if SynthesisStatus indicates an error. Otherwise it is empty.
    /// </summary>
    const std::wstring& FailureReason;


private:

    void PopulateResultFields(SPXRESULTHANDLE resultHandle)
    {
        static_assert((int)Result_Synthesis_Success == (int)::Microsoft::CognitiveServices::Speech::Translation::SynthesisStatusCode::Success, "Result_Synthesis* enum values == SynthesisStatus::* enum values");
        static_assert((int)Result_Synthesis_End == (int)::Microsoft::CognitiveServices::Speech::Translation::SynthesisStatusCode::SynthesisEnd, "Result_Synthesis* enum values == SynthesisStatus::* enum values");
        static_assert((int)Result_Synthesis_Error == (int)::Microsoft::CognitiveServices::Speech::Translation::SynthesisStatusCode::Error, "Result_Synthesis* enum values == SynthesisStatus::* enum values");

        SPX_INIT_HR(hr);

        Result_SynthesisStatus status;
        SPX_THROW_ON_FAIL(hr = TranslationSynthesisResult_GetSynthesisStatus(resultHandle, &status));
        m_synthesisStatus = static_cast<::Microsoft::CognitiveServices::Speech::Translation::SynthesisStatusCode>(status);

        size_t bufLen = 0;
        std::unique_ptr<wchar_t[]> reasonBuffer;
        hr = TranslationSynthesisResult_GetFailureReason(resultHandle, nullptr, &bufLen);
        if (hr == SPXERR_BUFFER_TOO_SMALL)
        {
            reasonBuffer = std::make_unique<wchar_t[]>(bufLen);
            hr = TranslationSynthesisResult_GetFailureReason(resultHandle, reasonBuffer.get(), &bufLen);
        }
        SPX_THROW_ON_FAIL(hr);
        if (bufLen > 0)
        {
            m_failureReason = std::wstring(reasonBuffer.get());
        }

        bufLen = 0;
        // retrieve the required buffer size first.
        hr = TranslationSynthesisResult_GetSynthesisData(resultHandle, nullptr, &bufLen);
        if (hr == SPXERR_BUFFER_TOO_SMALL)
        {
            m_audioData.resize(bufLen);
            hr = TranslationSynthesisResult_GetSynthesisData(resultHandle, m_audioData.data(), &bufLen);
        }
        SPX_THROW_ON_FAIL(hr);

        SPX_DBG_TRACE_VERBOSE("Translation synthesis: audio length: %d, verctor size:", bufLen, m_audioData.size());
    };

    DISABLE_DEFAULT_CTORS(TranslationSynthesisResult);
};


} } } } // Microsoft::CognitiveServices::Speech::Translation
