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

class CSpxSourceLanguageConfig :
    public ISpxObjectWithSiteInitImpl<ISpxGenericSite>,
    public ISpxServiceProvider,
    public ISpxGenericSite,
    public ISpxPropertyBagImpl,
    public ISpxSourceLanguageConfig
{
public:
    CSpxSourceLanguageConfig() {}

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectWithSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxServiceProvider)
        SPX_INTERFACE_MAP_ENTRY(ISpxNamedProperties)
        SPX_INTERFACE_MAP_ENTRY(ISpxSourceLanguageConfig)
        SPX_INTERFACE_MAP_ENTRY(ISpxGenericSite)
    SPX_INTERFACE_MAP_END()

    // --- ISpxSourceLanguageConfig ---
    virtual void InitFromLanguage(const char* languages) override;
    virtual void InitFromLanguageAndEndpointId(const char* language, const char* endpointId) override;
    virtual std::string GetLanguage() override;
    virtual std::string GetEndpointId() override;

    // --- IServiceProvider
    SPX_SERVICE_MAP_BEGIN()
        SPX_SERVICE_MAP_ENTRY(ISpxNamedProperties)
        SPX_SERVICE_MAP_ENTRY_SITE(GetSite())
    SPX_SERVICE_MAP_END()

private:
    bool m_init{ false };
    DISABLE_COPY_AND_MOVE(CSpxSourceLanguageConfig);
};

}}}} // Microsoft::CognitiveServices::Speech::Impl
