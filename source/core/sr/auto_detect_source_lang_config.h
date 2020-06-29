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

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class CSpxAutoDetectSourceLangConfig :
    public ISpxObjectWithSiteInitImpl<ISpxGenericSite>,
    public ISpxServiceProvider,
    public ISpxGenericSite,
    public ISpxPropertyBagImpl,
    public ISpxAutoDetectSourceLangConfig
{
public:
    CSpxAutoDetectSourceLangConfig() {}

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectWithSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxServiceProvider)
        SPX_INTERFACE_MAP_ENTRY(ISpxNamedProperties)
        SPX_INTERFACE_MAP_ENTRY(ISpxAutoDetectSourceLangConfig)
        SPX_INTERFACE_MAP_ENTRY(ISpxGenericSite)
    SPX_INTERFACE_MAP_END()

    // --- ISpxAutoDetectSourceLangConfig ---
    virtual void InitFromOpenRange() override;
    virtual void InitFromLanguages(const char* languages) override;
    virtual void AddSourceLanguageConfig(std::shared_ptr<ISpxSourceLanguageConfig> sourceLanguageConfigs) override;

    // --- IServiceProvider
    SPX_SERVICE_MAP_BEGIN()
        SPX_SERVICE_MAP_ENTRY(ISpxNamedProperties)
        SPX_SERVICE_MAP_ENTRY_SITE(GetSite())
    SPX_SERVICE_MAP_END()

private:
    bool m_init{ false };
    DISABLE_COPY_AND_MOVE(CSpxAutoDetectSourceLangConfig);
};

}}}} // Microsoft::CognitiveServices::Speech::Impl
