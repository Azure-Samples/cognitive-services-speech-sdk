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
* The status code of translation result.
*/
enum class TranslationStatus {
    Unknown, // Status is unknown
    IntermediateTextResult, // Intermediate text result.
    FinalTextResult, // Final text result.
    AudioResult, // The result contains a chunk of translation audio output. It might contain only a part of the complete audio output.
    FullResult, // The result contains full result, including both final text result and the complete audio output.
    SpeechNotRecognized, // Cannot recognize text from speech.
    TranslationNoMatch, // Cannot create translated text from the recognized text.
    Failed, // Failure during translaton.
    Cancelled // The translation request has been cancelled.
};

/*
* Represents translation result.
* Depending on TranslationStatus, it is possible that not all members contain valid value.
*/
class TranslationResult final
{
public:
    TranslationResult(SPXRESULTHANDLE hresult) :
        ResultId(m_resultId),
        RecognizedText(m_recognizedText),
        TranslationText(m_translationText),
        TranslationStatus(m_translationStatus),
        TranslationAudio(m_translationAudio),
        m_hresult(hresult)
    {
        // Todo: retrieves results from the hresult handle.
        SPX_THROW_ON_FAIL(SPXERR_NOT_IMPL);
    };

    ~TranslationResult() { };

    const std::wstring& ResultId;
    const std::wstring& RecognizedText;
    const std::wstring& TranslationText;
    const enum TranslationStatus& TranslationStatus;
    const std::vector<uint8_t>& TranslationAudio;

private:

    TranslationResult(const TranslationResult&) = delete;
    TranslationResult(const TranslationResult&&) = delete;

    TranslationResult& operator=(const TranslationResult&) = delete;

    SPXRESULTHANDLE m_hresult;

    std::wstring m_resultId;
    std::wstring m_recognizedText;
    std::wstring m_translationText;
    enum TranslationStatus m_translationStatus;
    std::vector<uint8_t> m_translationAudio;
};


} } } // CARBON_NAMESPACE_ROOT::Recognition::Translation
