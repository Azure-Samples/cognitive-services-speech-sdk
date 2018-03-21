//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// luis_direct_lu_engine_adapter.cpp: Implementation definitions for CSpxLuisDirectEngineAdapter C++ class
//

#include "stdafx.h"
#include "http_helpers.h"
#include "luis_direct_lu_engine_adapter.h"
#include "string_utils.h"

#include "json.hpp"
using json = nlohmann::json;


namespace CARBON_IMPL_NAMESPACE() {


void CSpxLuisDirectEngineAdapter::Term()
{
    m_triggerMap.clear();
    m_intentNameToIdMap.clear();
}

void CSpxLuisDirectEngineAdapter::AddIntentTrigger(const wchar_t* id, std::shared_ptr<ISpxTrigger> trigger)
{
    // Luis Direct only works with luis models ... not phrase triggers ... 
    if (trigger->GetModel() != nullptr)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_triggerMap.emplace(id, trigger);

        auto intentName = trigger->GetModelIntentName();
        m_intentNameToIdMap[intentName] = id;

        if (!m_emptyIntentNameOk && intentName.empty())
        {
            m_emptyIntentNameOk = true;
        }
    }
}

void CSpxLuisDirectEngineAdapter::ProcessResult(std::shared_ptr<ISpxRecognitionResult> result)
{
    SPX_DBG_TRACE_FUNCTION();
    std::string query = PAL::ToString(result->GetText().c_str());

    // Get the connection information for this ONE (1!!) LUIS model reference
    std::string hostName, relativePath;
    GetConnectionInfo(query, &hostName, &relativePath);

    // If we found a set of connection information...
    if (!hostName.empty() && !relativePath.empty())
    {
        // Contact LUIS, asking it to return the JSON response for the LUIS model specified
        auto json = SpxHttpDownloadString(hostName.c_str(), relativePath.c_str());
        SPX_DBG_TRACE_VERBOSE("LUIS said this: '%s'", json.c_str());

        // Extract the intent from the JSON payload
        auto intentName = ExtractIntent(json);
        SPX_DBG_TRACE_VERBOSE("LUIS intent == '%S'", intentName.c_str());

        // Map the LUIS intent name in that payload to the specified "IntentId" specified when the devleoper-user called AddIntent("IntentId", ...)
        auto intentId = IntentIdFromIntentName(intentName);
        SPX_DBG_TRACE_VERBOSE("IntentRecognitionResult::IntentId == '%S'", intentId.c_str());

        // If we have a valid IntentId...
        bool validIntentResult = !intentId.empty() || (m_emptyIntentNameOk && !json.empty());
        if (validIntentResult)
        {
            // Update our result to be an "Intent" result, with the appropriate ID and JSON payload
            auto initIntentResult = std::dynamic_pointer_cast<ISpxIntentRecognitionResultInit>(result);
            initIntentResult->InitIntentResult(intentId.c_str(), PAL::ToWString(json).c_str());
        }
    }
}

void CSpxLuisDirectEngineAdapter::GetConnectionInfo(const std::string& query, std::string* phostName, std::string* prelativePath)
{
    GetConnectionInfoFromTriggers(query, phostName, prelativePath);
}

void CSpxLuisDirectEngineAdapter::GetConnectionInfoFromTriggers(const std::string& query, std::string* phostName, std::string* prelativePath)
{
    // The LUIS Direct LU Engine Adapter currently only allows for a single (1 !!!) LUIS model to be used. If the API developer-user specifies
    // more than a single LUIS model via AddIntent(), we'll fail this call with SPXERR_ABORT. However, specifying more than one intent, where
    // all of those intents are from teh same LUIS model, is supported. The code below iterates thru all the triggers, finding the 
    // "endpoint/hostname/subscription key/app id" ... It stores the first one it finds. It then continues iterating thru the triggers, ensuring
    // that all the other triggers have data that links them to the same LUIS model found initially... 

    std::string endpoint, host, key, appId;

    std::unique_lock<std::mutex> lock(m_mutex);
    for (auto item : m_triggerMap)
    {
        auto trigger = item.second;
        auto model = trigger->GetModel();
        if (model != nullptr)
        {
            auto str = PAL::ToString(model->GetEndpoint());
            SPX_IFTRUE_THROW_HR(!str.empty() && !endpoint.empty() && str != endpoint, SPXERR_ABORT);
            endpoint = str;

            str = PAL::ToString(model->GetHostName());
            SPX_IFTRUE_THROW_HR(!str.empty() && !host.empty() && str != host, SPXERR_ABORT);
            host = str;

            str = PAL::ToString(model->GetSubscriptionKey());
            SPX_IFTRUE_THROW_HR(!str.empty() && !key.empty() && str != key, SPXERR_ABORT);
            key = str;

            str = PAL::ToString(model->GetAppId());
            SPX_IFTRUE_THROW_HR(!str.empty() && !appId.empty() && str != appId, SPXERR_ABORT);
            appId = str;
        }
    }

    lock.unlock();
    GetConnectionInfo(endpoint, host, key, appId, query, phostName, prelativePath);
}

void CSpxLuisDirectEngineAdapter::GetConnectionInfo(const std::string& endpoint, const std::string& host, const std::string& key, const std::string& appId, const std::string& query, std::string* phostName, std::string* prelativePath)
{
    if (!endpoint.empty())
    {
        GetEndpointConnectionInfo(endpoint, query, phostName, prelativePath);
    }
    else if (!key.empty() && !appId.empty())
    {
        GetSubscriptionConnectionInfo(host, key, appId, query, phostName, prelativePath);
    }
    else
    {
        GetDefaultConnectionInfo(query, phostName, prelativePath);
    }
}

void CSpxLuisDirectEngineAdapter::GetDefaultConnectionInfo(const std::string& query, std::string* phostName, std::string* prelativePath)
{
    // Get the default connection info, by having the overloaded GetSubscriptionConnectionInfo see "" strings and find the right defaults... 
    return GetSubscriptionConnectionInfo("", "", "", query, phostName, prelativePath);
}

void CSpxLuisDirectEngineAdapter::GetEndpointConnectionInfo(const std::string& endpoint, const std::string& query, std::string* phostName, std::string* prelativePath)
{
    UNUSED(endpoint);
    UNUSED(query);
    UNUSED(phostName);
    UNUSED(prelativePath);

    // TODO: RobCh: Intent: Tease apart the hostname and the relative path from the endpoint uri provided

    SPX_THROW_HR(SPXERR_NOT_IMPL);
}

void CSpxLuisDirectEngineAdapter::GetSubscriptionConnectionInfo(const std::string& host, const std::string& key, const std::string& appId, const std::string& query, std::string* phostName, std::string* prelativePath)
{
    // exampleUrl = R"(https://westus.api.cognitive.microsoft.com/luis/v2.0/apps/6ad2c77d-180b-45a2-88aa-8c442538c090?subscription-key=0415470b4d0d4ed7b736a7ccac71fa5d&verbose=true&timezoneOffset=0&q=what%20time%20is%20it%20in%20cincinnati)";
    // relativePath == R("/luis/v2.0/apps/{{{{app-id}}}}?subscription-key={{{{subscription-key}}}}&verbose=true&q={{{{query}}}}");
    // exampleHostName = R"(westus.api.cognitive.microsoft.com");

    constexpr auto pszDefaultHostName = R"(westus.api.cognitive.microsoft.com)";
    constexpr auto pszDefaultAppId = R"(52d44b10-3973-4b78-b5b5-4b2da0eaa6f5)";
    constexpr auto pszDefaultKey = R"(0415470b4d0d4ed7b736a7ccac71fa5d)";

    std::string relativePath { R"(/luis/v2.0/apps/)" };
    relativePath += !appId.empty() ? appId.c_str() : pszDefaultAppId;
    relativePath += R"(?subscription-key=)";
    relativePath += !key.empty() ? key.c_str() : pszDefaultKey;
    relativePath += R"(&verbose=true&q=)";
    relativePath += query;

    *phostName = !host.empty() ? host.c_str() : pszDefaultHostName;
    *prelativePath = std::move(relativePath);
}

std::wstring CSpxLuisDirectEngineAdapter::ExtractIntent(const std::string& str)
{
    std::wstring intent = L"";
    try
    {
        auto json = json::parse(str);
        intent = PAL::ToWString(json["topScoringIntent"]["intent"]);
    }
    catch (...)
    {
        SPX_DBG_TRACE_VERBOSE("ExtractIntent FAILED!!");
    }
    return intent;
}

std::wstring CSpxLuisDirectEngineAdapter::IntentIdFromIntentName(const std::wstring& intentName)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    if (m_intentNameToIdMap.find(intentName) != m_intentNameToIdMap.end())
    {
        return m_intentNameToIdMap[intentName];
    }
    return L"";
}


} // CARBON_IMPL_NAMESPACE
