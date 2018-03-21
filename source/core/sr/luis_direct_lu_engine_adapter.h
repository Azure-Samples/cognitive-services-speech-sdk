//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// luis_direct_lu_engine_adapter.h: Implementation declarations for CSpxAudioStreamSession C++ class
//

#pragma once
#include "spxcore_common.h"
#include "ispxinterfaces.h"


namespace CARBON_IMPL_NAMESPACE() {


class CSpxLuisDirectEngineAdapter : 
    public ISpxObjectWithSiteInitImpl<ISpxLuEngineAdapterSite>,
    public ISpxIntentTriggerService,
    public ISpxLuEngineAdapter
{
public:

    CSpxLuisDirectEngineAdapter() : m_emptyIntentNameOk(false) { }

    // --- ISpxObjectInit overrides ---
    void Term() override;

    // --- ISpxIntentTriggerService ---
    void AddIntentTrigger(const wchar_t* id, std::shared_ptr<ISpxTrigger> trigger) override;

    // --- ISpxLuEngineAdapter / ISpxRecoResultProcessor ---
    void ProcessResult(std::shared_ptr<ISpxRecognitionResult> result) override;


private:

    CSpxLuisDirectEngineAdapter(CSpxLuisDirectEngineAdapter &&) = delete;
    CSpxLuisDirectEngineAdapter(const CSpxLuisDirectEngineAdapter&) = delete;
    CSpxLuisDirectEngineAdapter& operator=(CSpxLuisDirectEngineAdapter &&) = delete;
    CSpxLuisDirectEngineAdapter& operator=(const CSpxLuisDirectEngineAdapter&) = delete;

    void GetConnectionInfo(const std::string& query, std::string* phostName, std::string* prelativePath);
    void GetConnectionInfoFromTriggers(const std::string& query, std::string* phostName, std::string* prelativePath);

    void GetConnectionInfo(const std::string& endpoint, const std::string& host, const std::string& key, const std::string& appId, const std::string& query, std::string* phostName, std::string* prelativePath);
    void GetDefaultConnectionInfo(const std::string& query, std::string* phostName, std::string* prelativePath);
    void GetEndpointConnectionInfo(const std::string& endpoint, const std::string& query, std::string* phostName, std::string* prelativePath);
    void GetSubscriptionConnectionInfo(const std::string& host, const std::string& key, const std::string& appId, const std::string& query, std::string* phostName, std::string* prelativePath);

    std::wstring ExtractIntent(const std::string& json);
    std::wstring IntentIdFromIntentName(const std::wstring& intentName);

    std::mutex m_mutex;
    std::map<std::wstring, std::shared_ptr<ISpxTrigger>> m_triggerMap;
    std::map<std::wstring, std::wstring> m_intentNameToIdMap;
    bool m_emptyIntentNameOk;
};


} // CARBON_IMPL_NAMESPACE
