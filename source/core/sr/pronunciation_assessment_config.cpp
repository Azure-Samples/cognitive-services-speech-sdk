//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#include "stdafx.h"
#include "pronunciation_assessment_config.h"
#include "property_id_2_name_map.h"
#include "usp.h"
#include "json.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

using namespace std;
using namespace PronunciationAssessment;

void CSpxPronunciationAssessmentConfig::InitWithParameters(const char* referenceText, PronunciationAssessmentGradingSystem gradingSystem,
                          PronunciationAssessmentGranularity granularity, bool enableMiscue, const char* scenarioId)
{
    SPX_IFTRUE_THROW_HR(m_init, SPXERR_ALREADY_INITIALIZED);
    m_init = true;
    SetStringValue(GetPropertyName(PropertyId::PronunciationAssessment_ReferenceText), referenceText);

    try
    {
        SetStringValue(GetPropertyName(PropertyId::PronunciationAssessment_GradingSystem), pronunciationAssessmentGradingSystemToString.at(gradingSystem).c_str());
        SetStringValue(GetPropertyName(PropertyId::PronunciationAssessment_Granularity), pronunciationAssessmentGranularityToString.at(granularity).c_str());
    }
    catch(const std::exception& e)
    {
        SPX_TRACE_ERROR("%s", e.what());
        SPX_THROW_HR(SPXERR_INVALID_ARG);
    }
    
    if (enableMiscue)
    {
        SetStringValue(GetPropertyName(PropertyId::PronunciationAssessment_EnableMiscue), TrueString);
    }
    
    SetStringValue(GetPropertyName(PropertyId::PronunciationAssessment_ScenarioId), scenarioId);
}

void CSpxPronunciationAssessmentConfig::InitFromJson(const char* json)
{
    SPX_IFTRUE_THROW_HR(m_init, SPXERR_ALREADY_INITIALIZED);
    m_init = true;

    try
    {
        auto j = nlohmann::json::parse(json);
    }
    catch (nlohmann::json::parse_error& e)
    {
        UNUSED(e);
        SPX_TRACE_ERROR("invalid json");
        SPX_THROW_HR(SPXERR_INVALID_ARG);
    }
    
    SetStringValue(GetPropertyName(PropertyId::PronunciationAssessment_Json), json);
}

void CSpxPronunciationAssessmentConfig::UpdateJson()
{
    const auto jsonString = GetStringValue(GetPropertyName(PropertyId::PronunciationAssessment_Json));
    auto paramsJson = jsonString.empty() ? nlohmann::json() : nlohmann::json::parse(jsonString);

    auto referenceText = GetStringValue(GetPropertyName(PropertyId::PronunciationAssessment_ReferenceText));
    if (!referenceText.empty())
    {
        paramsJson["referenceText"] = referenceText;
    }

    auto gradingSystem = GetStringValue(GetPropertyName(PropertyId::PronunciationAssessment_GradingSystem));
    if (!gradingSystem.empty())
    {
        paramsJson["gradingSystem"] = gradingSystem;
    }

    auto granularity = GetStringValue(GetPropertyName(PropertyId::PronunciationAssessment_Granularity));
    if (!granularity.empty())
    {
        paramsJson["granularity"] = granularity;
    }

    // always set dimension to Comprehensive
    paramsJson["dimension"] = "Comprehensive";

    const auto enableMiscueString = GetStringValue(GetPropertyName(PropertyId::PronunciationAssessment_EnableMiscue));
    if (enableMiscueString == TrueString)
    {
        paramsJson["enableMiscue"] = true;
    }
    else if (enableMiscueString == FalseString)
    {
        paramsJson["enableMiscue"] = false;
    }

    auto scenarioId = GetStringValue(GetPropertyName(PropertyId::PronunciationAssessment_ScenarioId));
    if (!scenarioId.empty())
    {
        paramsJson["scenarioId"] = scenarioId;
    }

    SetStringValue(GetPropertyName(PropertyId::PronunciationAssessment_Params), paramsJson.dump().c_str());
}

} } } } // Microsoft::CognitiveServices::Speech::Impl
