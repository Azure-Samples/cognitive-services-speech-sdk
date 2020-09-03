//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// mock_tts_engine_adapter.h: Implementation declarations for CSpxMockTtsEngineAdapter C++ class
//

#pragma once
#include <memory>
#include "asyncop.h"
#include "spxcore_common.h"
#include "ispxinterfaces.h"
#include "interface_helpers.h"
#include "property_bag_impl.h"
#include <object_with_site_init_impl.h>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


class CSpxMockTtsEngineAdapter :
    public ISpxObjectWithSiteInitImpl<ISpxTtsEngineAdapterSite>,
    public ISpxTtsEngineAdapter,
    public ISpxPropertyBagImpl
{
public:

    CSpxMockTtsEngineAdapter();
    virtual ~CSpxMockTtsEngineAdapter();

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectWithSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxTtsEngineAdapter)
    SPX_INTERFACE_MAP_END()

    // --- ISpxTtsEngineAdapter
    void SetOutput(std::shared_ptr<ISpxAudioOutput> output) override;
    std::shared_ptr<ISpxSynthesisResult> Speak(const std::string& text, bool isSsml, const std::string& requestId, bool retry) override;
    void StopSpeaking() override;


protected:

    std::shared_ptr<ISpxNamedProperties> GetParentProperties() const override;


private:

    using SitePtr = std::shared_ptr<ISpxTtsEngineAdapterSite>;

    CSpxMockTtsEngineAdapter(const CSpxMockTtsEngineAdapter&) = delete;
    CSpxMockTtsEngineAdapter(const CSpxMockTtsEngineAdapter&&) = delete;

    CSpxMockTtsEngineAdapter& operator=(const CSpxMockTtsEngineAdapter&) = delete;

    SpxWAVEFORMATEX_Type GetOutputFormat(bool* hasHeader);


private:

    std::shared_ptr<ISpxAudioOutput> m_audioOutput;
};


} } } } // Microsoft::CognitiveServices::Speech::Impl
