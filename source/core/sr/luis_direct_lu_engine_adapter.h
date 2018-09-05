//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#pragma once
#include "spxcore_common.h"
#include "ispxinterfaces.h"
#include "interface_helpers.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


class CSpxLuisDirectEngineAdapter : 
    public ISpxObjectWithSiteInitImpl<ISpxLuEngineAdapterSite>,
    public ISpxIntentTriggerService,
    public ISpxLuEngineAdapter
{
public:

    CSpxLuisDirectEngineAdapter() : m_emptyIntentNameOk(false) { }

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectWithSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxIntentTriggerService)
        SPX_INTERFACE_MAP_ENTRY(ISpxLuEngineAdapter)
        SPX_INTERFACE_MAP_ENTRY(ISpxRecognitionResultProcessor)
    SPX_INTERFACE_MAP_END()

    // --- ISpxObjectInit overrides ---
    void Term() override;

    // --- ISpxIntentTriggerService ---
    void AddIntentTrigger(const wchar_t* id, std::shared_ptr<ISpxTrigger> trigger) override;

    std::list<std::string> GetListenForList() override;
    void GetIntentInfo(std::string& provider, std::string& id, std::string& key, std::string& region) override;

    // --- ISpxLuEngineAdapter / ISpxRecoResultProcessor ---
    void ProcessResult(std::shared_ptr<ISpxRecognitionResult> result) override;


private:

    CSpxLuisDirectEngineAdapter(CSpxLuisDirectEngineAdapter &&) = delete;
    CSpxLuisDirectEngineAdapter(const CSpxLuisDirectEngineAdapter&) = delete;
    CSpxLuisDirectEngineAdapter& operator=(CSpxLuisDirectEngineAdapter &&) = delete;
    CSpxLuisDirectEngineAdapter& operator=(const CSpxLuisDirectEngineAdapter&) = delete;

    void GetConnectionInfoFromTriggers(const std::string& query, std::string* phostName, std::string* prelativePath);

    std::wstring ExtractIntent(const std::string& json);
    std::wstring IntentIdFromIntentName(const std::wstring& intentName);

    std::mutex m_mutex;
    std::map<std::wstring, std::shared_ptr<ISpxTrigger>> m_triggerMap;
    std::map<std::wstring, std::wstring> m_intentNameToIdMap;
    bool m_emptyIntentNameOk;
};


} } } } // Microsoft::CognitiveServices::Speech::Impl
