//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// luis_direct_lu_engine_adapter.cpp: Implementation definitions for CSpxLuisDirectEngineAdapter C++ class
//

#include "stdafx.h"
#include "http_helpers.h"
#include "urlencode_helpers.h"
#include "luis_direct_lu_engine_adapter.h"
#include "string_utils.h"
#include "service_helpers.h"
#include "named_properties_constants.h"

#ifdef _MSC_VER
#pragma warning( push )
// disable: (8300,27): error 28020:  : The expression '0&lt;=_Param_(1)&amp;&amp;_Param_(1)&lt;=64-1' is not true at this call.
#pragma warning( disable : 28020 )
#include "json.hpp"
#pragma warning( pop )
#else
#include "json.hpp"
#endif
using json = nlohmann::json;


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


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

std::list<std::string> CSpxLuisDirectEngineAdapter::GetListenForList()
{
    std::list<std::string> listenForList;

    // Let's loop thru each trigger we have...
    std::unique_lock<std::mutex> lock(m_mutex);
    for (auto item : m_triggerMap)
    {
        auto trigger = item.second;

        // If it's a simple phrase trigger, add it 'naked' as a ListenFor element
        auto phrase = trigger->GetPhrase();
        if (!phrase.empty())
        {
            std::string listenFor = PAL::ToString(phrase);
            listenForList.push_back(listenFor);
        }

        // If it's a language understanding model...
        auto model = trigger->GetModel();
        if (model != nullptr)
        {
            // Get the app id and the intent name...
            auto appId = model->GetAppId();
            auto intentName = trigger->GetModelIntentName();

            // Format the ListenFor element...
            std::string listenFor;
            listenFor += "{luis:";
            listenFor += PAL::ToString(appId);
            if (!intentName.empty())
            {
                listenFor += "#";
                listenFor += PAL::ToString(intentName);
            }
            listenFor += "}";

            // And add it to the list...
            listenForList.push_back(listenFor);
        }
    }

    return listenForList;
}

void CSpxLuisDirectEngineAdapter::GetIntentInfo(std::string& provider, std::string& id, std::string& key)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    for (auto item : m_triggerMap)
    {
        auto trigger = item.second;
        auto model = trigger->GetModel();
        if (model != nullptr)
        {
            auto str = PAL::ToString(model->GetAppId());
            SPX_IFTRUE_THROW_HR(!str.empty() && !id.empty() && str != id, SPXERR_ABORT);
            id = str;

            str = PAL::ToString(model->GetSubscriptionKey());
            SPX_IFTRUE_THROW_HR(!str.empty() && !key.empty() && str != key, SPXERR_ABORT);
            key = str;
        }
    }

    if (!id.empty() && !key.empty())
    {
        provider = "LUIS";
    }
}

void CSpxLuisDirectEngineAdapter::ProcessResult(std::shared_ptr<ISpxRecognitionResult> result)
{
    SPX_DBG_TRACE_FUNCTION();

    // We only need to process the result when the user actually said something...
    std::string resultText = PAL::ToString(result->GetText().c_str());
    SPX_DBG_TRACE_VERBOSE("%s: text='%s'", __FUNCTION__, resultText.c_str());
    if (!resultText.empty())
    {
        // Check to see if we already have the JSON payload (from the speech service)
        auto properties = SpxQueryInterface<ISpxNamedProperties>(result);
        auto json = PAL::ToString(properties->GetStringValue(g_RESULT_LanguageUnderstandingJson));
        SPX_DBG_TRACE_VERBOSE("%s: text='%s'; already-existing-IntentResultJson='%s'", __FUNCTION__, resultText.c_str(), json.c_str());

        // If we don't already have the LUIS json, fetch it from LUIS now...
        if (json.empty())
        {
            // Get the connection information for this ONE (1!!) language understanding model reference
            std::string hostName, relativePath;
            GetConnectionInfo(resultText, &hostName, &relativePath);

            // If we found a set of connection information...
            if (!hostName.empty() && !relativePath.empty())
            {
                // Contact LUIS, asking it to return the JSON response for the language understanding model specified
                json = SpxHttpDownloadString(hostName.c_str(), relativePath.c_str());
                SPX_DBG_TRACE_VERBOSE("LUIS said this: '%s'", json.c_str());
            }
        }

        if (!json.empty())
        {
            // Extract the intent from the JSON payload
            auto intentName = ExtractIntent(json);
            SPX_DBG_TRACE_VERBOSE("LUIS intent == '%ls'", intentName.c_str());

            // Map the LUIS intent name in that payload to the specified "IntentId" specified when the devleoper-user called AddIntent("IntentId", ...)
            auto intentId = IntentIdFromIntentName(intentName);
            SPX_DBG_TRACE_VERBOSE("IntentRecognitionResult::IntentId == '%ls'", intentId.c_str());

            // If we have a valid IntentId...
            bool validIntentResult = !intentId.empty() || (m_emptyIntentNameOk && !json.empty());
            if (validIntentResult)
            {
                // Update our result to be an "Intent" result, with the appropriate ID and JSON payload
                auto initIntentResult = SpxQueryInterface<ISpxIntentRecognitionResultInit>(result);
                initIntentResult->InitIntentResult(intentId.c_str(), PAL::ToWString(json).c_str());
            }
        }
    }
}

void CSpxLuisDirectEngineAdapter::GetConnectionInfo(const std::string& query, std::string* phostName, std::string* prelativePath)
{
    GetConnectionInfoFromTriggers(query, phostName, prelativePath);
}

void CSpxLuisDirectEngineAdapter::GetConnectionInfoFromTriggers(const std::string& query, std::string* phostName, std::string* prelativePath)
{
    // The LUIS Direct LU Engine Adapter currently only allows for a single (1 !!!) language understanding model to be used. If the API developer-user specifies
    // more than a single language understanding model via AddIntent(), we'll fail this call with SPXERR_ABORT. However, specifying more than one intent, where
    // all of those intents are from teh same language understanding model, is supported. The code below iterates thru all the triggers, finding the 
    // "endpoint/hostname/subscription key/app id" ... It stores the first one it finds. It then continues iterating thru the triggers, ensuring
    // that all the other triggers have data that links them to the same language understanding model found initially... 

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
    constexpr auto pszDefaultHostName = R"(westus2.api.cognitive.microsoft.com)";

    std::string relativePath { R"(/luis/v2.0/apps/)" };
    relativePath += appId;
    relativePath += R"(?subscription-key=)";
    relativePath += key;
    relativePath += R"(&q=)";
    relativePath += Impl::UrlEncode(query);

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
    return L"X-" + intentName;
}


} } } } // Microsoft::CognitiveServices::Speech::Impl
