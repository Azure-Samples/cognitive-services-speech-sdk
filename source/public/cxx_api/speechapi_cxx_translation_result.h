//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_cxx_translation_result.h: Public API declarations for TranslationResult C++ class
//

#pragma once
#include <string>
#include <vector>
#include <speechapi_c.h>
#include <speechapi_cxx_common.h>
#include <speechapi_cxx_recognition_result.h>

namespace CARBON_NAMESPACE_ROOT {
namespace Recognition {
namespace Translation {

/*
* Represents translation text result.
*/
class TranslationTextResult
{
public:
    TranslationTextResult(SPXRESULTHANDLE resultHandle) :
        ResultId(m_resultId),
        RecognitionText(m_recognizedText),
        TranslationText(m_translationText),
        SourceLanguage(m_sourceLanguage),
        TargetLanguage(m_targetLanguage),
        m_hresult(resultHandle)
    {
        PopulateResultFields(resultHandle, m_resultId, m_recognizedText, m_translationText, m_sourceLanguage, m_targetLanguage);
    };

    virtual ~TranslationTextResult() { };

    const std::wstring& ResultId;
    const std::wstring& RecognitionText;
    const std::wstring& TranslationText;
    const std::wstring& SourceLanguage;
    const std::wstring& TargetLanguage;

private:

    void PopulateResultFields(
        SPXRESULTHANDLE resultHandle,
        std::wstring& resultId,
        std::wstring& recognitionText,
        std::wstring& translationText,
        std::wstring& sourceLanguage,
        std::wstring& targetLanguage)
    {
        SPX_INIT_HR(hr);

        const size_t maxCharCount = 1024;
        wchar_t sz[maxCharCount + 1];

        SPX_THROW_ON_FAIL(hr = Result_GetResultId(resultHandle, sz, maxCharCount));
        resultId = sz;

        SPX_THROW_ON_FAIL(hr = Result_GetText(resultHandle, sz, maxCharCount));
        recognitionText = sz;

        SPX_THROW_ON_FAIL(hr = TranslationResult_GetTranslationText(resultHandle, sz, maxCharCount));
        translationText = sz;

        SPX_THROW_ON_FAIL(hr = TranslationResult_GetSourceLanguage(resultHandle, sz, maxCharCount));
        sourceLanguage = sz;

        SPX_THROW_ON_FAIL(hr = TranslationResult_GetTargetLanguage(resultHandle, sz, maxCharCount));
        targetLanguage = sz;
    };

    TranslationTextResult() = delete;
    TranslationTextResult(TranslationTextResult&&) = delete;
    TranslationTextResult(const TranslationTextResult&) = delete;
    TranslationTextResult& operator=(TranslationTextResult&&) = delete;
    TranslationTextResult& operator=(const TranslationTextResult&) = delete;

    SPXRESULTHANDLE m_hresult;

    std::wstring m_resultId;
    std::wstring m_recognizedText;
    std::wstring m_translationText;
    std::wstring m_sourceLanguage;
    std::wstring m_targetLanguage;
    bool m_isFinalResult;
};

/*
* Represents synthesized audio data returned as result from service.
* TODO: This might be unified with the TTS response.
*/
class TranslationSynthesisResult
{
public:
    TranslationSynthesisResult(SPXRESULTHANDLE resultHandle) :
        ResultId(m_resultId),
        AudioData(m_audioData),
        m_hresult(resultHandle)
    {
        SPX_INIT_HR(hr);

        const size_t maxCharCount = 1024;
        wchar_t sz[maxCharCount + 1];

        SPX_THROW_ON_FAIL(hr = Result_GetResultId(resultHandle, sz, maxCharCount));
        m_resultId = sz;

        // Todo: get audio data from result.
    };

    virtual ~TranslationSynthesisResult() { };

    const std::wstring& ResultId;
    const std::vector<uint8_t>& AudioData;

private:

    TranslationSynthesisResult(const TranslationSynthesisResult&) = delete;
    TranslationSynthesisResult(const TranslationSynthesisResult&&) = delete;

    TranslationSynthesisResult& operator=(const TranslationSynthesisResult&) = delete;

    SPXRESULTHANDLE m_hresult;

    std::wstring m_resultId;
    std::wstring m_recognizedText;
    std::vector<uint8_t> m_audioData;
};

/*
* The status code of translation result.
*/
enum class TranslationStatus {
    Success,
    SpeechNotRecognized, // Cannot recognize text from speech.
    TranslationNoMatch, // Cannot create translated text from the recognized text.
    VoiceSynthesisError, // Cannot generate voice from the transcribed text.
    Error, // service encountered an internal error and could not continue.
    Cancelled // The translation request has been cancelled.
};

/*
* Represents the translation result containing both text and audio
*/
class TranslationResult final : public TranslationTextResult, public TranslationSynthesisResult
{
public:
    TranslationResult(SPXRESULTHANDLE hresult) :
        TranslationTextResult(hresult),
        TranslationSynthesisResult(hresult),
        ResultStatus(m_resultStatus),
        m_hresult(hresult)
    {
        // Todo: get result status from hresult
        m_resultStatus = TranslationStatus::Success;

        // Todo: resolve both TranslationTextResult and TranslationSynthesisResult have ResultId. Check whether they are same.
        SPX_DBG_TRACE_VERBOSE("%s (this=0x%x, handle=0x%x) -- resultid=%S; recogniton text='%S' translation text='%S'",
            __FUNCTION__, this, m_hresult, TranslationTextResult::ResultId.c_str(), RecognitionText.c_str(), TranslationText.c_str());
    };

    virtual ~TranslationResult() { };

    const TranslationStatus& ResultStatus;

private:

    TranslationResult(const TranslationResult&) = delete;
    TranslationResult(const TranslationResult&&) = delete;

    TranslationResult& operator=(const TranslationResult&) = delete;

    SPXRESULTHANDLE m_hresult;

    TranslationStatus m_resultStatus;
};

} } } // CARBON_NAMESPACE_ROOT::Recognition::Translation
