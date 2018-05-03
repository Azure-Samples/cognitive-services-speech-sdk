//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// mock_lu_engine_adapter.h: Implementation declarations for CSpxMockLuEngineAdapter C++ class
//

#pragma once
#include <memory>
#include <queue>
#include "spxcore_common.h"
#include "ispxinterfaces.h"
#include "interface_helpers.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


class CSpxMockLuEngineAdapter :
    public ISpxObjectWithSiteInitImpl<ISpxLuEngineAdapterSite>,
    public ISpxIntentTriggerService,
    public ISpxLuEngineAdapter
{
public:

    CSpxMockLuEngineAdapter();
    virtual ~CSpxMockLuEngineAdapter();

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectWithSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxIntentTriggerService)
        SPX_INTERFACE_MAP_ENTRY(ISpxRecognitionResultProcessor)
        SPX_INTERFACE_MAP_ENTRY(ISpxLuEngineAdapter)
    SPX_INTERFACE_MAP_END()

    // --- ISpxObject ---
    void Init() override;
    void Term() override;

    // --- ISpxIntentTriggerService ---
    void AddIntentTrigger(const wchar_t* id, std::shared_ptr<ISpxTrigger> trigger) override;

    std::list<std::string> GetListenForList() override;
    void GetIntentInfo(std::string& provider, std::string& id, std::string& key, std::string& region) override;

    // --- ISpxLuEngineAdapter / ISpxRecoResultProcessor ---
    void ProcessResult(std::shared_ptr<ISpxRecognitionResult> result) override;


private:

    CSpxMockLuEngineAdapter(const CSpxMockLuEngineAdapter&) = delete;
    CSpxMockLuEngineAdapter(const CSpxMockLuEngineAdapter&&) = delete;
    CSpxMockLuEngineAdapter& operator=(const CSpxMockLuEngineAdapter&) = delete;
    CSpxMockLuEngineAdapter& operator=(const CSpxMockLuEngineAdapter&&) = delete;

    std::mutex m_mutex;
    std::map<std::wstring, std::shared_ptr<ISpxTrigger>> m_triggerMap;
};


} } } } // Microsoft::CognitiveServices::Speech::Impl
