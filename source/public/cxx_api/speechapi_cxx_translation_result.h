//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_cxx_translation_result.h: Public API declarations for TranslationResult C++ class
//

#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <speechapi_c.h>
#include <speechapi_cxx_common.h>
#include <speechapi_cxx_recognition_result.h>

namespace CARBON_NAMESPACE_ROOT {
namespace Recognition {
namespace Translation {

/// <summary>
/// Defines the status code of translation result.
/// </summary>
enum class TranslationStatus {
    /// <summary>
    /// The translation is successful.
    /// </summary>
    Success,

    /// <summary>
    /// Cannot get recognized text from speech.
    /// </summary>
    SpeechNotRecognized, 

    /// <summary>
    /// Cannot create translated text from the recognized text.
    /// </summary>
    TranslationNoMatch,

    /// <summary>
    /// Cannot generate voice output from the transcribed text.
    /// </summary>
    VoiceSynthesisError,

    /// <summary>
    /// An error occurred during translation.
    /// </summary>
    Error, 

    /// <summary>
    /// The translation request has been cancelled.
    /// </summary>
    Cancelled
};


/// <summary> 
/// Defines the translation text result.
/// </summary>
class TranslationTextResult final : public CARBON_NAMESPACE_ROOT::Recognition::Speech::SpeechRecognitionResult
{

private:
    enum TranslationStatus m_translationStatus;
    ::std::unordered_map<::std::wstring, ::std::wstring> m_translations;

public:

    /// <summary>
    /// It is intended for internal use only. It creates an instance of <see cref="TranslationTextResult">.
    /// </summary>
    /// <param name="resultHandle">The handle of the result returned by recognizer in C-API.</param>
    TranslationTextResult(SPXRESULTHANDLE resultHandle) :
        SpeechRecognitionResult(resultHandle),
        TranslationStatus(m_translationStatus),
        Translations(m_translations)
    {
        PopulateResultFields(resultHandle);
    };

    /// <summary>
    /// Destructs the instance.
    /// </summary>
    virtual ~TranslationTextResult() { };

    /// <summary>
    /// Describes the status of translation result.
    /// </summary>
    const enum TranslationStatus& TranslationStatus;

    /// <summary>
    /// Presents the translation results. Each item in the map is a key value pair, where key is the language tag of the translated text,
    /// and value is the translation text in that language.
    /// </summary>
    const ::std::unordered_map<::std::wstring, ::std::wstring>& Translations;

private:

    void PopulateResultFields(SPXRESULTHANDLE resultHandle)
    {
        (void)resultHandle;
        SPX_THROW_ON_FAIL(SPXERR_NOT_IMPL);
    };

    TranslationTextResult() = delete;
    TranslationTextResult(TranslationTextResult&&) = delete;
    TranslationTextResult(const TranslationTextResult&) = delete;
    TranslationTextResult& operator=(TranslationTextResult&&) = delete;
    TranslationTextResult& operator=(const TranslationTextResult&) = delete;

    SPXRESULTHANDLE m_hresult;
    bool m_isFinalResult;
};

/// <summary>
/// Defines the translation synthesis result, i.e. the voice output of the translated text in the target language.
/// </summary>
class TranslationSynthesisResult
{
private:
    std::vector<uint8_t> m_audioData;

public:
    /// <summary>
    /// It is intended for internal use only. It creates an instance of <see cref="TranslationSynthesisResult">
    /// </summary>
    /// <param name="resultHandle">The handle of the result returned by recognizer in C-API.</param>
    TranslationSynthesisResult(SPXRESULTHANDLE resultHandle) :
        AudioData(m_audioData),
        m_hresult(resultHandle)
    {
        // Todo: get audio data from result.
    };

    /// <summary>
    /// Deconstructs the instance.
    /// </summary>
    virtual ~TranslationSynthesisResult() { };

    /// <summary>
    /// The voice output of the translated text in the target language.
    const std::vector<uint8_t>& AudioData;

private:

    TranslationSynthesisResult(const TranslationSynthesisResult&) = delete;
    TranslationSynthesisResult(const TranslationSynthesisResult&&) = delete;
    TranslationSynthesisResult& operator=(const TranslationSynthesisResult&) = delete;

    SPXRESULTHANDLE m_hresult;

};


} } } // CARBON_NAMESPACE_ROOT::Recognition::Translation
