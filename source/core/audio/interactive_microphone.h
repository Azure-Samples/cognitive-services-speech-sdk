//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// interactive_microphone.h: Implementation declarations for CSpxInteractiveMicrophone C++ class
//

#pragma once
#include "spxcore_common.h"
#include "delegate_audio_pump_impl.h"
#include "interface_helpers.h"
#include "service_helpers.h"
#include "property_bag_impl.h"
#include <object_with_site_init_impl.h>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


class CSpxInteractiveMicrophone :
        public ISpxObjectWithSiteInitImpl<ISpxGenericSite>,
        public ISpxServiceProvider,
        public ISpxGenericSite,
        public ISpxPropertyBagImpl,
        public ISpxDelegateAudioPumpImpl
{
public:

    CSpxInteractiveMicrophone();

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectWithSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxGenericSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxServiceProvider)
        SPX_INTERFACE_MAP_ENTRY(ISpxNamedProperties)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioPump)
    SPX_INTERFACE_MAP_END()

    // --- ISpxObjectInit
    void Init() override;
    void Term() override;

    // --- IServiceProvider
    SPX_SERVICE_MAP_BEGIN()
        SPX_SERVICE_MAP_ENTRY(ISpxNamedProperties)
        SPX_SERVICE_MAP_ENTRY_SITE(GetSite())
    SPX_SERVICE_MAP_END()

private:
    std::shared_ptr<ISpxNamedProperties> GetParentProperties() const override;
};


} } } } // Microsoft::CognitiveServices::Speech::Impl
