//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#include "stdafx.h"
#include "common.h"
#include "speechapi_c_pronunciation_assessment_config.h"
#include "create_object_helpers.h"
#include "event_helpers.h"
#include "handle_helpers.h"
#include "platform.h"
#include "site_helpers.h"
#include "handle_table.h"
#include "property_id_2_name_map.h"

using namespace Microsoft::CognitiveServices::Speech::Impl;
using namespace Microsoft::CognitiveServices::Speech;

static_assert(static_cast<int>(PronunciationAssessmentGradingSystem_FivePoint) ==
        static_cast<int>(PronunciationAssessmentGradingSystem::FivePoint),
    "PronunciationAssessmentGradingSystem_* enum values == PronunciationAssessmentGradingSystem::* enum values");
static_assert(static_cast<int>(PronunciationAssessmentGradingSystem_HundredMark) ==
    static_cast<int>(PronunciationAssessmentGradingSystem::HundredMark),
    "PronunciationAssessmentGradingSystem_* enum values == PronunciationAssessmentGradingSystem::* enum values");

static_assert(static_cast<int>(PronunciationAssessmentGranularity_Phoneme) ==
    static_cast<int>(PronunciationAssessmentGranularity::Phoneme),
    "PronunciationAssessmentGranularity_* enum values == PronunciationAssessmentGranularity::* enum values");
static_assert(static_cast<int>(PronunciationAssessmentGranularity_Word) ==
    static_cast<int>(PronunciationAssessmentGranularity::Word),
    "PronunciationAssessmentGranularity_* enum values == PronunciationAssessmentGranularity::* enum values");
static_assert(static_cast<int>(PronunciationAssessmentGranularity_FullText) ==
    static_cast<int>(PronunciationAssessmentGranularity::FullText),
    "PronunciationAssessmentGranularity_* enum values == PronunciationAssessmentGranularity::* enum values");

SPXAPI create_pronunciation_assessment_config(SPXPRONUNCIATIONASSESSMENTCONFIGHANDLE* hPronunciationAssessmentConfig,
                                              const char* referenceText,
                                              PronunciationAssessment_GradingSystem gradingSystem,
                                              PronunciationAssessment_Granularity granularity,
                                              bool enableMiscue,
                                              const char* scenarioId)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, hPronunciationAssessmentConfig == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        *hPronunciationAssessmentConfig = SPXHANDLE_INVALID;

        auto pronunciationAssessmentConfig = SpxCreateObjectWithSite<ISpxPronunciationAssessmentConfig>("CSpxPronunciationAssessmentConfig", SpxGetRootSite());
        pronunciationAssessmentConfig->InitWithParameters(referenceText,
                                                         static_cast<PronunciationAssessmentGradingSystem>(gradingSystem),
                                                         static_cast<PronunciationAssessmentGranularity>(granularity),
                                                         enableMiscue,
                                                         scenarioId);

        auto pronunciationAssessmentConfigs = CSpxSharedPtrHandleTableManager::Get<ISpxPronunciationAssessmentConfig, SPXPRONUNCIATIONASSESSMENTCONFIGHANDLE>();
        *hPronunciationAssessmentConfig = pronunciationAssessmentConfigs->TrackHandle(pronunciationAssessmentConfig);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI create_pronunciation_assessment_config_from_json(SPXPRONUNCIATIONASSESSMENTCONFIGHANDLE* hPronunciationAssessmentConfig, const char* json)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, json == nullptr || !(*json));
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, hPronunciationAssessmentConfig == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        *hPronunciationAssessmentConfig = SPXHANDLE_INVALID;

        auto pronunciationAssessmentConfig = SpxCreateObjectWithSite<ISpxPronunciationAssessmentConfig>("CSpxPronunciationAssessmentConfig", SpxGetRootSite());
        pronunciationAssessmentConfig->InitFromJson(json);

        auto pronunciationAssessmentConfigs = CSpxSharedPtrHandleTableManager::Get<ISpxPronunciationAssessmentConfig, SPXPRONUNCIATIONASSESSMENTCONFIGHANDLE>();
        *hPronunciationAssessmentConfig = pronunciationAssessmentConfigs->TrackHandle(pronunciationAssessmentConfig);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI_(bool) pronunciation_assessment_config_is_handle_valid(SPXPRONUNCIATIONASSESSMENTCONFIGHANDLE hPronunciationAssessmentConfig)
{
    return Handle_IsValid<SPXPRONUNCIATIONASSESSMENTCONFIGHANDLE, ISpxPronunciationAssessmentConfig>(hPronunciationAssessmentConfig);
}

SPXAPI pronunciation_assessment_config_release(SPXPRONUNCIATIONASSESSMENTCONFIGHANDLE hPronunciationAssessmentConfig)
{
    return Handle_Close<SPXPRONUNCIATIONASSESSMENTCONFIGHANDLE, ISpxPronunciationAssessmentConfig>(hPronunciationAssessmentConfig);
}

SPXAPI pronunciation_assessment_config_get_property_bag(SPXPRONUNCIATIONASSESSMENTCONFIGHANDLE hPronunciationAssessmentConfig, SPXPROPERTYBAGHANDLE* hpropbag)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, !pronunciation_assessment_config_is_handle_valid(hPronunciationAssessmentConfig));
    return GetTargetObjectByService<ISpxPronunciationAssessmentConfig, ISpxNamedProperties>(hPronunciationAssessmentConfig, hpropbag);
}

SPXAPI__(const char*) pronunciation_assessment_config_to_json(SPXPRONUNCIATIONASSESSMENTCONFIGHANDLE hPronunciationAssessmentConfig)
{
    char* result = nullptr;

    if (hPronunciationAssessmentConfig == nullptr)
    {
        return result;
    }

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto config = GetInstance<ISpxPronunciationAssessmentConfig>(hPronunciationAssessmentConfig);

        config->UpdateJson();
        const auto namedProperties = SpxQueryInterface<ISpxNamedProperties>(config);

        const auto tempValue = namedProperties->GetStringValue(GetPropertyName(PropertyId::PronunciationAssessment_Params));
        const auto size = tempValue.size() + 1;

        result = new char[size];
        PAL::strcpy(result, size, tempValue.c_str(), size, true);
    }
    SPXAPI_CATCH_AND_RETURN(hr, result);
}

SPXAPI pronunciation_assessment_config_apply_to_recognizer(SPXPRONUNCIATIONASSESSMENTCONFIGHANDLE hPronunciationAssessmentConfig, SPXRECOHANDLE hreco)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, !pronunciation_assessment_config_is_handle_valid(hPronunciationAssessmentConfig));
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, !recognizer_handle_is_valid(hreco));

    SPXAPI_INIT_HR_TRY(hr)
    {
        const auto recognizer = GetInstance<ISpxRecognizer>(hreco);
        auto config = GetInstance<ISpxPronunciationAssessmentConfig>(hPronunciationAssessmentConfig);
        config->UpdateJson();
        auto recognizerProperties = SpxQueryInterface<ISpxNamedProperties>(recognizer);
        auto paramsPropertyName = GetPropertyName(PropertyId::PronunciationAssessment_Params);
        const auto configProperties = SpxQueryInterface<ISpxNamedProperties>(config);
        recognizerProperties->SetStringValue(paramsPropertyName, configProperties->GetStringValue(paramsPropertyName).c_str());
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}
