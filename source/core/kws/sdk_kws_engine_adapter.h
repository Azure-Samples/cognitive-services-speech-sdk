//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// sdk_kws_engine_adapter.h: Implementation declarations for CSpxSdkKwsEngineAdapter C++ class
//

#pragma once
#include <memory>
#include <queue>
#include "spxcore_common.h"
#include "ispxinterfaces.h"
#include "interface_helpers.h"
#include <speechapi.h>



namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


class CSpxSdkKwsEngineAdapter :
    public ISpxObjectWithSiteInitImpl<ISpxKwsEngineAdapterSite>,
    public ISpxKwsEngineAdapter
{
public:

    CSpxSdkKwsEngineAdapter();
    virtual ~CSpxSdkKwsEngineAdapter();

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectWithSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxKwsEngineAdapter)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioProcessor)
    SPX_INTERFACE_MAP_END()

    // --- ISpxObject
    void Init() override;
    void Term() override;

    // --- ISpxAudioProcessor
    void SetFormat(WAVEFORMATEX* pformat) override;
    void ProcessAudio(AudioData_Type data, uint32_t size) override;


private:

    void FireDoneProcessingAudioEvent();
    void FireKeywordDetectedEvent();

private:

    CSpxSdkKwsEngineAdapter(const CSpxSdkKwsEngineAdapter&) = delete;
    CSpxSdkKwsEngineAdapter(const CSpxSdkKwsEngineAdapter&&) = delete;
    CSpxSdkKwsEngineAdapter& operator=(const CSpxSdkKwsEngineAdapter&) = delete;
    CSpxSdkKwsEngineAdapter& operator=(const CSpxSdkKwsEngineAdapter&&) = delete;

private:

    class Impl; // forward declaration
    friend Impl;
    Impl* p_impl;
};


} } } } // Microsoft::CognitiveServices::Speech::Impl
