//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
// speechapi_cxx_pronunciation_assessment_result.h: Public API declarations for PronunciationAssessmentResult C++ class
//

#pragma once
#include <string>
#include <speechapi_cxx_common.h>
#include <speechapi_cxx_string_helpers.h>
#include <speechapi_cxx_recognition_result.h>


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {


/// <summary>
/// Base class for pronunciation assessment results.
/// </summary>
class PronunciationAssessmentResult
{
public:

    /// <summary>
    /// Creates a pronunciation assessment result object from recognition result
    /// If nullptr is returned, it means the assessment is failed.
    /// </summary>
    /// <param name="result">recognition result</param>
    /// <returns>A shared pointer to the created PronunciationAssessmentResult instance.</returns>
    static std::shared_ptr<PronunciationAssessmentResult> FromResult(std::shared_ptr<RecognitionResult> result)
    {
        SPX_THROW_HR_IF(SPXERR_INVALID_ARG, result == nullptr);
        if (result->Properties.GetProperty("AccuracyScore").empty())
        {
            return nullptr;
        }
        auto ptr = new PronunciationAssessmentResult(result->Properties);
        return std::shared_ptr<PronunciationAssessmentResult>(ptr);
    }

    /// <summary>
    /// The score indicating the pronunciation accuracy of the given speech, which indicates
    /// how closely the phonemes match a native speaker's pronunciation.
    /// If this is less 0, it means the pronunciation assessment failed.
    /// </summary>
    const double AccuracyScore;

    /// <summary>
    /// The overall score indicating the pronunciation quality of the given speech.
    /// This is calculated from AccuracyScore, FluencyScore and CompletenessScore with weight.
    /// If this is less 0, it means the pronunciation assessment failed.
    /// </summary>
    const double PronunciationScore;

    /// <summary>
    /// The score indicating the completeness of the given speech by calculating the ratio of pronounced words towards entire input.
    /// If this is less 0, it means the pronunciation assessment failed.
    /// </summary>
    const double CompletenessScore;

    /// <summary>
    /// The score indicating the fluency of the given speech.
    /// If this is less 0, it means the pronunciation assessment failed.
    /// </summary>
    const double FluencyScore;

protected:

    /*! \cond PROTECTED */


    explicit PronunciationAssessmentResult(const PropertyCollection& properties) :
        AccuracyScore(std::stod(properties.GetProperty("AccuracyScore", "-1"))),
        PronunciationScore(std::stod(properties.GetProperty("PronScore", "-1"))),
        CompletenessScore(std::stod(properties.GetProperty("CompletenessScore", "-1"))),
        FluencyScore(std::stod(properties.GetProperty("FluencyScore", "-1")))
    {
    }

    /*! \endcond */

private:

    DISABLE_DEFAULT_CTORS(PronunciationAssessmentResult);
};


} } } // Microsoft::CognitiveServices::Speech
