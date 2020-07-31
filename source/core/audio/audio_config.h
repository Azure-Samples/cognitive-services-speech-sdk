//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// audio_config.h: Implementation declarations for CSpxAudioConfig C++ class
//

#pragma once
#include "spxcore_common.h"
#include "interface_helpers.h"
#include "service_helpers.h"
#include "property_bag_impl.h"
#include <object_with_site_init_impl.h>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


class CSpxAudioConfig :
    public ISpxObjectWithSiteInitImpl<ISpxGenericSite>,
    public ISpxServiceProvider,
    public ISpxPropertyBagImpl,
    public ISpxAudioConfig
{
public:

    CSpxAudioConfig();

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectWithSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxServiceProvider)
        SPX_INTERFACE_MAP_ENTRY(ISpxNamedProperties)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioConfig)
    SPX_INTERFACE_MAP_END()

    // --- ISpxAudioConfig ---
    void InitFromDefaultDevice() override;
    void InitFromFile(const wchar_t* pszFileName) override;
    void InitFromStream(std::shared_ptr<ISpxAudioStream> stream) override;

    std::wstring GetFileName() const override { return m_fileName; }
    std::shared_ptr<ISpxAudioStream> GetStream() override { return m_stream; }

    // --- IServiceProvider
    SPX_SERVICE_MAP_BEGIN()
        SPX_SERVICE_MAP_ENTRY(ISpxNamedProperties)
        SPX_SERVICE_MAP_ENTRY_SITE(GetSite())
    SPX_SERVICE_MAP_END()

private:

    DISABLE_COPY_AND_MOVE(CSpxAudioConfig);

    bool m_init { false };
    std::wstring m_fileName;
    std::shared_ptr<ISpxAudioStream> m_stream;
};


} } } } // Microsoft::CognitiveServices::Speech::Impl
