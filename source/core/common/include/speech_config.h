//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speech_config.h: Implementation declarations for CSpxSpeechConfig C++ class
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

class CSpxSpeechConfig :
    public ISpxObjectWithSiteInitImpl<ISpxGenericSite>,
    public ISpxServiceProvider,
    public ISpxGenericSite,
    public ISpxPropertyBagImpl,
    public ISpxSpeechConfig
{
public:
    CSpxSpeechConfig() {}

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectWithSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxServiceProvider)
        SPX_INTERFACE_MAP_ENTRY(ISpxNamedProperties)
        SPX_INTERFACE_MAP_ENTRY(ISpxSpeechConfig)
        SPX_INTERFACE_MAP_ENTRY(ISpxGenericSite)
    SPX_INTERFACE_MAP_END()

    // --- ISpxSpeechConfig ---
    void InitAuthorizationToken(const char* authToken, const char* region) override;
    void InitFromEndpoint(const char* endpoint, const char* subscription) override;
    void InitFromSubscription(const char* subscription, const char* region) override;
    void SetServiceProperty(std::string name, std::string, ServicePropertyChannel channel) override;

    // --- IServiceProvider
    SPX_SERVICE_MAP_BEGIN()
        SPX_SERVICE_MAP_ENTRY(ISpxNamedProperties)
        SPX_SERVICE_MAP_ENTRY_SITE(GetSite())
    SPX_SERVICE_MAP_END()

private:
    bool m_init{ false };
    DISABLE_COPY_AND_MOVE(CSpxSpeechConfig);
};

}}}} // Microsoft::CognitiveServices::Speech::Impl
