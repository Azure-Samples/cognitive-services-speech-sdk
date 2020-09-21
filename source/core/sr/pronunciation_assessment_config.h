//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#pragma once

#include "spxcore_common.h"
#include "interface_helpers.h"
#include "service_helpers.h"
#include "property_bag_impl.h"
#include "ispxinterfaces.h"
#include <object_with_site_init_impl.h>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class CSpxPronunciationAssessmentConfig :
    public ISpxObjectWithSiteInitImpl<ISpxGenericSite>,
    public ISpxServiceProvider,
    public ISpxGenericSite,
    public ISpxPropertyBagImpl,
    public ISpxPronunciationAssessmentConfig
{
public:
    CSpxPronunciationAssessmentConfig() {}

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectWithSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxServiceProvider)
        SPX_INTERFACE_MAP_ENTRY(ISpxNamedProperties)
        SPX_INTERFACE_MAP_ENTRY(ISpxPronunciationAssessmentConfig)
        SPX_INTERFACE_MAP_ENTRY(ISpxGenericSite)
    SPX_INTERFACE_MAP_END()

    // --- ISpxPronunciationAssessmentConfig ---
    void InitWithParameters(const char* referenceText, PronunciationAssessmentGradingSystem gradingSystem,
              PronunciationAssessmentGranularity granularity, bool enableMiscue) override;
    void InitFromJson(const char* json) override;
    void UpdateJson() override;

    // --- IServiceProvider
    SPX_SERVICE_MAP_BEGIN()
        SPX_SERVICE_MAP_ENTRY(ISpxNamedProperties)
        SPX_SERVICE_MAP_ENTRY_SITE(GetSite())
    SPX_SERVICE_MAP_END()

private:
    bool m_init{ false };
    DISABLE_COPY_AND_MOVE(CSpxPronunciationAssessmentConfig);
};

namespace PronunciationAssessment {

static const std::map<PronunciationAssessmentGradingSystem, const std::string> pronunciationAssessmentGradingSystemToString = {
    {PronunciationAssessmentGradingSystem::FivePoint, "FivePoint"},
    {PronunciationAssessmentGradingSystem::HundredMark, "HundredMark"}
};

static const std::map<PronunciationAssessmentGranularity, const std::string> pronunciationAssessmentGranularityToString = {
    {PronunciationAssessmentGranularity::Phoneme, "Phoneme"},
    {PronunciationAssessmentGranularity::Word, "Word"},
    {PronunciationAssessmentGranularity::FullText, "FullText"}
};

}

}}}} // Microsoft::CognitiveServices::Speech::Impl
