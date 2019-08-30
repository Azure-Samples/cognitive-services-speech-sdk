//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
// speechapi_cxx_keyword_recognition_model.h: Public API declarations for KeywordRecognitionModel C++ class
//

#pragma once
#include <speechapi_cxx_common.h>
#include <speechapi_cxx_string_helpers.h>
#include <speechapi_c_keyword_recognition_model.h>


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {

/// <summary>
/// Represents keyword recognition model used with StartKeywordRecognitionAsync methods.
/// </summary>
/// Note: Keyword spotting (KWS) functionality might work with any microphone type, official KWS support, however, is currently limited to the microphone arrays found in the Azure Kinect DK hardware or the Speech Devices SDK.
class KeywordRecognitionModel
{
public:

    /// <summary>
    /// Creates a keyword recognition model using the specified file.
    /// </summary>
    /// <param name="fileName">The file name of the keyword recognition model.</param>
    /// <returns>A shared pointer to keyword recognition model.</returns>
    static std::shared_ptr<KeywordRecognitionModel> FromFile(const SPXSTRING& fileName)
    {
        SPXKEYWORDHANDLE hkeyword = SPXHANDLE_INVALID;
        SPX_THROW_ON_FAIL(keyword_recognition_model_create_from_file(Utils::ToUTF8(fileName).c_str(), &hkeyword));
        return std::make_shared<KeywordRecognitionModel>(hkeyword);
    }

    /// <summary>
    /// Internal constructor. Creates a new instance using the provided handle.
    /// </summary>
    /// <param name="hkeyword">Keyword handle.</param>
    explicit KeywordRecognitionModel(SPXKEYWORDHANDLE hkeyword = SPXHANDLE_INVALID) : m_hkwmodel(hkeyword) { }

    /// <summary>
    /// Virtual destructor.
    /// </summary>
    virtual ~KeywordRecognitionModel() { keyword_recognition_model_handle_release(m_hkwmodel); }

    /// <summary>
    /// Internal. Explicit conversion operator.
    /// </summary>
    /// <returns>A handle.</returns>
    explicit operator SPXKEYWORDHANDLE() { return m_hkwmodel; }

private:

    DISABLE_COPY_AND_MOVE(KeywordRecognitionModel);

    SPXKEYWORDHANDLE m_hkwmodel;
};


} } } // Microsoft::CognitiveServices::Speech
