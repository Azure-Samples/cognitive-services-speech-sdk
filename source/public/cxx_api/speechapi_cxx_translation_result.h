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
    TranslationTextResult(SPXRESULTHANDLE hresult) :
        ResultId(m_resultId),
        RecognizedText(m_recognizedText),
        TranslationText(m_translationText),
        IsFinalResult(m_isFinalResult),
        m_hresult(hresult)
    {
        // Todo: retrieves results from the hresult handle.
    };

    virtual ~TranslationTextResult() { };

    const std::wstring& ResultId;
    const std::wstring& RecognizedText;
    const std::wstring& TranslationText;
    const bool& IsFinalResult;

private:

    TranslationTextResult(const TranslationTextResult&) = delete;
    TranslationTextResult(const TranslationTextResult&&) = delete;

    TranslationTextResult& operator=(const TranslationTextResult&) = delete;

    SPXRESULTHANDLE m_hresult;

    std::wstring m_resultId;
    std::wstring m_recognizedText;
    std::wstring m_translationText;
    bool m_isFinalResult;
};

/*
* Represents synthesized audio data returned as result from service.
* TODO: This might be unified with the TTS response.
*/
class AudioResult
{
public:
    AudioResult(SPXRESULTHANDLE hresult) :
        ResultId(m_resultId),
        AudioData(m_audioData),
        m_hresult(hresult)
    {
        // Todo: retrieves results from the hresult handle.
    };

    virtual ~AudioResult() { };

    const std::wstring& ResultId;
    const std::vector<uint8_t>& AudioData;

private:

    AudioResult(const AudioResult&) = delete;
    AudioResult(const AudioResult&&) = delete;

    AudioResult& operator=(const AudioResult&) = delete;

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
class TranslationResult final : public TranslationTextResult, public AudioResult
{
public:
    TranslationResult(SPXRESULTHANDLE hresult) :
        TranslationTextResult(hresult),
        AudioResult(hresult),
        ResultStatus(m_resultStatus),
        m_hresult(hresult)
    {
        // Todo: retrieves results from the hresult handle.
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
