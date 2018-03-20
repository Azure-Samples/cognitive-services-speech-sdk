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


namespace CARBON_IMPL_NAMESPACE() {


class CSpxMockLuEngineAdapter :
    public ISpxObjectWithSiteInitImpl<ISpxLuEngineAdapterSite>,
    public ISpxIntentTriggerService,
    public ISpxLuEngineAdapter
{
public:

    CSpxMockLuEngineAdapter();
    virtual ~CSpxMockLuEngineAdapter();

    // --- ISpxObject ---
    void Init() override;
    void Term() override;

    // --- ISpxIntentTriggerService ---
    void AddIntentTrigger(const wchar_t* id, std::shared_ptr<ISpxTrigger> trigger) override;

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


} // CARBON_IMPL_NAMESPACE
