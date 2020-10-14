//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#pragma once

#include <string>

#include "speechapi_cxx_properties.h"
#include "speechapi_cxx_string_helpers.h"
#include "speechapi_cxx_utils.h"
#include "speechapi_cxx_common.h"
#include "speechapi_cxx_enums.h"
#include <stdarg.h>
#include "speechapi_c_pronunciation_assessment_config.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {

/// <summary>
/// Class that defines pronunciation assessment configuration
/// Added in 1.14.0
/// </summary>
class PronunciationAssessmentConfig
{
public:
    /// <summary>
    /// Internal operator used to get underlying handle value.
    /// </summary>
    /// <returns>A handle.</returns>
    explicit operator SPXPRONUNCIATIONASSESSMENTCONFIGHANDLE() const { return m_hconfig; }

    /// <summary>
    /// Creates an instance of the PronunciationAssessmentConfig
    /// For the parameters details, see
    /// https://docs.microsoft.com/azure/cognitive-services/speech-service/rest-speech-to-text#pronunciation-assessment-parameters
    /// </summary>
    /// <param name="referenceText">The reference text</param>
    /// <param name="gradingSystem">The point system for score calibration</param>
    /// <param name="granularity">The evaluation granularity</param>
    /// <param name="enableMiscue">If enables miscue calculation</param>
    /// <returns>A shared pointer to the new PronunciationAssessmentConfig instance.</returns>
    static std::shared_ptr<PronunciationAssessmentConfig> Create(const std::string& referenceText,
                                                                 PronunciationAssessmentGradingSystem gradingSystem =
                                                                     PronunciationAssessmentGradingSystem::FivePoint,
                                                                 PronunciationAssessmentGranularity granularity =
                                                                     PronunciationAssessmentGranularity::Phoneme,
                                                                 bool enableMiscue = false)
    {
        SPXPRONUNCIATIONASSESSMENTCONFIGHANDLE hconfig = SPXHANDLE_INVALID;

        SPX_THROW_ON_FAIL(
            create_pronunciation_assessment_config(&hconfig, Utils::ToUTF8(referenceText).c_str(),
                static_cast<PronunciationAssessment_GradingSystem>(gradingSystem),
                static_cast<PronunciationAssessment_Granularity>(granularity),
                enableMiscue));
        const auto ptr = new PronunciationAssessmentConfig(hconfig);
        return std::shared_ptr<PronunciationAssessmentConfig>(ptr);
    }

#ifndef SWIG
    /// <summary>
    /// Creates an instance of the PronunciationAssessmentConfig
    /// For the parameters details, see
    /// https://docs.microsoft.com/azure/cognitive-services/speech-service/rest-speech-to-text#pronunciation-assessment-parameters
    /// </summary>
    /// <param name="referenceText">The reference text</param>
    /// <param name="gradingSystem">The point system for score calibration</param>
    /// <param name="granularity">The evaluation granularity</param>
    /// <param name="enableMiscue">If enables miscue calculation</param>
    /// <returns>A shared pointer to the new PronunciationAssessmentConfig instance.</returns>
    static std::shared_ptr<PronunciationAssessmentConfig> Create(const std::wstring& referenceText,
                                                                 PronunciationAssessmentGradingSystem gradingSystem =
                                                                     PronunciationAssessmentGradingSystem::FivePoint,
                                                                 PronunciationAssessmentGranularity granularity =
                                                                     PronunciationAssessmentGranularity::Phoneme,
                                                                 bool enableMiscue = false)
    {
        return Create(Utils::ToUTF8(referenceText), gradingSystem, granularity, enableMiscue);
    }
#endif

    /// <summary>
    /// Creates an instance of the PronunciationAssessmentConfig from json.
    /// see https://docs.microsoft.com/azure/cognitive-services/speech-service/rest-speech-to-text#pronunciation-assessment-parameters
    /// </summary>
    /// <param name="json">The json string containing the pronunciation assessment parameters.</param>
    /// <returns>A shared pointer to the new PronunciationAssessmentConfig instance.</returns>
    static std::shared_ptr<PronunciationAssessmentConfig> CreateFromJson(const SPXSTRING& json)
    {
        SPX_THROW_HR_IF(SPXERR_INVALID_ARG, json.empty());
        SPXAUTODETECTSOURCELANGCONFIGHANDLE hconfig = SPXHANDLE_INVALID;

        SPX_THROW_ON_FAIL(create_pronunciation_assessment_config_from_json(&hconfig, Utils::ToUTF8(json).c_str()));
        const auto ptr = new PronunciationAssessmentConfig(hconfig);
        return std::shared_ptr<PronunciationAssessmentConfig>(ptr);
    }

    /// <summary>
    /// Gets to json string of pronunciation assessment parameters.
    /// </summary>
    /// <returns>json string of pronunciation assessment parameters.</returns>
    SPXSTRING ToJson() const
    {
        const char* jsonCch = pronunciation_assessment_config_to_json(m_hconfig);
        return Utils::ToSPXString(Utils::CopyAndFreePropertyString(jsonCch));
    }

    /// <summary>
    /// Gets the reference text.
    /// </summary>
    /// <returns>The reference text.</returns>
    SPXSTRING GetReferenceText()
    {
        const char* value = property_bag_get_string(m_propertybag, static_cast<int>(PropertyId::PronunciationAssessment_ReferenceText), nullptr, "");
        return Utils::ToSPXString(Utils::CopyAndFreePropertyString(value));
    }

    /// <summary>
    /// Sets the reference text.
    /// </summary>
    /// <param name="referenceText">The reference text.</param>
    void SetReferenceText(const std::string& referenceText)
    {
        property_bag_set_string(m_propertybag, static_cast<int>(PropertyId::PronunciationAssessment_ReferenceText), nullptr, referenceText.c_str());
    }

#ifndef SWIG
    /// <summary>
    /// Sets the reference text.
    /// </summary>
    /// <param name="referenceText">The reference text.</param>
    void SetReferenceText(const std::wstring& referenceText)
    {
        SetReferenceText(Utils::ToUTF8(referenceText));
    }
#endif

    /// <summary>
    /// Applies the settings in this config to a Recognizer.
    /// </summary>
    /// <param name="recognizer">The target Recognizer.</param>
    void ApplyTo(std::shared_ptr<Recognizer> recognizer) const
    {
        SPX_INIT_HR(hr);
        SPX_IFTRUE_THROW_HR(recognizer == nullptr, SPXERR_INVALID_ARG);

        SPX_THROW_ON_FAIL(hr =::pronunciation_assessment_config_apply_to_recognizer(m_hconfig, recognizer->m_hreco));
    }

    /// <summary>
    /// Destructs the object.
    /// </summary>
    virtual ~PronunciationAssessmentConfig()
    {
        pronunciation_assessment_config_release(m_hconfig);
        property_bag_release(m_propertybag);
    }

private:

    /// <summary>
    /// Internal constructor. Creates a new instance using the provided handle.
    /// </summary>
    explicit PronunciationAssessmentConfig(SPXPRONUNCIATIONASSESSMENTCONFIGHANDLE hconfig)
        :m_hconfig(hconfig)
        {
            SPX_THROW_ON_FAIL(pronunciation_assessment_config_get_property_bag(hconfig, &m_propertybag));
        }

    /// <summary>
    /// Internal member variable that holds the config
    /// </summary>
    SPXPRONUNCIATIONASSESSMENTCONFIGHANDLE m_hconfig;

    /// <summary>
    /// Internal member variable that holds the properties of the speech config
    /// </summary>
    SPXPROPERTYBAGHANDLE m_propertybag;

    DISABLE_COPY_AND_MOVE(PronunciationAssessmentConfig);
};

}}}

