//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// local_tts_engine_adapter.h: Implementation declarations for CSpxLocalTtsEngineAdapter C++ class
//

#pragma once
#include <memory>
#include <queue>
#include "asyncop.h"
#include "spxcore_common.h"
#include "ispxinterfaces.h"
#include "interface_helpers.h"
#include "property_bag_impl.h"
#include <object_with_site_init_impl.h>
#include "mstts.h"

#define AUDIO_OUTPUT_CHUNK_BUFFER_SIZE 0x10000
#define AUDIO_OUTPUT_BUFFER_SIZE 0x1000000


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


class CSpxLocalTtsEngineAdapter :
    public ISpxObjectWithSiteInitImpl<ISpxTtsEngineAdapterSite>,
    public ISpxTtsEngineAdapter,
    public ISpxPropertyBagImpl
{
public:

    CSpxLocalTtsEngineAdapter();
    virtual ~CSpxLocalTtsEngineAdapter();

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectWithSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxTtsEngineAdapter)
    SPX_INTERFACE_MAP_END()

    // --- ISpxObjectInit
    void Init() override;
    void Term() override;

    // --- ISpxTtsEngineAdapter
    void SetOutput(std::shared_ptr<ISpxAudioOutput> output) override;
    std::shared_ptr<ISpxSynthesisResult> Speak(const std::string& text, bool isSsml, const std::wstring& requestId, bool retry) override;
    void StopSpeaking() override;


protected:

    std::shared_ptr<ISpxNamedProperties> GetParentProperties() const override;


private:

    using SitePtr = std::shared_ptr<ISpxTtsEngineAdapterSite>;

    CSpxLocalTtsEngineAdapter(const CSpxLocalTtsEngineAdapter&) = delete;
    CSpxLocalTtsEngineAdapter(const CSpxLocalTtsEngineAdapter&&) = delete;

    CSpxLocalTtsEngineAdapter& operator=(const CSpxLocalTtsEngineAdapter&) = delete;

    SpxWAVEFORMATEX_Type GetOutputFormat(bool* hasHeader);

private:

    IMSSpeechSynthesize* m_pMsTtsEngine;
    MSTTSERROR m_engineError;
    std::string m_voiceName;
    std::shared_ptr<ISpxAudioOutput> m_audioOutput;
};


} } } } // Microsoft::CognitiveServices::Speech::Impl
