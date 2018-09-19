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
#include "property_id_2_name_map.h"

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
    auto model = trigger->GetModel();
    if (model != nullptr)
    {
        if (model->GetSubscriptionKey().empty() && model->GetRegion().empty())
        {
            auto properties = SpxQueryInterface<ISpxNamedProperties>(GetSite());
            auto region = properties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_Region));
            auto key = properties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_Key));
            model->UpdateSubscription(PAL::ToWString(key).c_str(), PAL::ToWString(region).c_str());
        }

        auto intentId = (id == nullptr || id[0] == '\0') ? std::wstring() : id;
        auto intentName = trigger->GetModelIntentName();

        std::unique_lock<std::mutex> lock(m_mutex);
        m_triggerMap.emplace(intentId, trigger);

        if (!intentName.empty() && !intentId.empty())
        {
            // We've got both intentName and intentId ... add to the map
            m_intentNameToIdMap[intentName] = intentId;
        }
        else if (!intentName.empty() && intentId.empty())
        {
            // We've got the intentName, but no intentId ... use intentName as the id
            m_intentNameToIdMap[intentName] = intentName;
        }
        else if (intentName.empty() && intentId.empty())
        {
            // We've got neither intentName nor IntentId
            m_matchAllIntents = true;
            m_useIntentNameAsIdWhenNotFoundInMap = true;
        }
        else if (intentName.empty() && !intentId.empty())
        {
            // We've got an intentId but no intentName
            m_matchAllIntents = true;
            m_useIntentNameAsIdWhenNotFoundInMap = false;
            m_useThisIdWhenNameNotFoundInMap = intentId;
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
            listenFor += PAL::ToString(appId) + "-PRODUCTION";
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

void CSpxLuisDirectEngineAdapter::GetIntentInfo(std::string& provider, std::string& id, std::string& key, std::string& region)
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

            str = PAL::ToString(model->GetRegion());
            SPX_IFTRUE_THROW_HR(!str.empty() && !region.empty() && str != region, SPXERR_ABORT);
            region = str;
        }
    }

    if (!id.empty() && !key.empty() && !region.empty())
    {
        provider = "LUIS";
    }

    SPX_DBG_TRACE_VERBOSE("%s: provider=%s; id=%s; key=%s; region=%s", __FUNCTION__, provider.c_str(), id.c_str(), key.c_str(), region.c_str());
}

void CSpxLuisDirectEngineAdapter::ProcessResult(std::shared_ptr<ISpxRecognitionResult> result)
{
    SPX_DBG_TRACE_FUNCTION();

    // We only need to process the result when the user actually said something...
    std::string resultText = PAL::ToString(result->GetText());
    SPX_DBG_TRACE_VERBOSE("%s: text='%s'", __FUNCTION__, resultText.c_str());
    if (!resultText.empty())
    {
        // Check to see if we already have the JSON payload (from the speech service)
        auto properties = SpxQueryInterface<ISpxNamedProperties>(result);
        auto json = properties->GetStringValue(GetPropertyName(PropertyId::LanguageUnderstandingServiceResponse_JsonResult));
        SPX_DBG_TRACE_VERBOSE("%s: text='%s'; already-existing-IntentResultJson='%s'", __FUNCTION__, resultText.c_str(), json.c_str());

        // If we don't already have the LUIS json, fetch it from LUIS now...
        if (json.empty())
        {
            // Get the connection information for this ONE (1!!) language understanding model reference
            std::string hostName, relativePath;
            GetConnectionInfoFromTriggers(resultText, &hostName, &relativePath);

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

            // Map the LUIS intent name in that payload to the specified "IntentId" specified when the developer-user called AddIntent("IntentId", ...)
            auto intentId = IntentIdFromIntentName(intentName);
            SPX_DBG_TRACE_VERBOSE("IntentRecognitionResult::IntentId == '%ls'", intentId.c_str());

            // If we have a valid IntentId...
            bool validIntentResult = !intentId.empty();
            if (validIntentResult)
            {
                // Update our result to be an "Intent" result, with the appropriate ID and JSON payload
                auto initIntentResult = SpxQueryInterface<ISpxIntentRecognitionResultInit>(result);
                initIntentResult->InitIntentResult(intentId.c_str(), PAL::ToWString(json).c_str());
            }
        }
    }
}

void CSpxLuisDirectEngineAdapter::GetConnectionInfoFromTriggers(const std::string& query, std::string* phostName, std::string* prelativePath)
{
    // The LUIS Direct LU Engine Adapter currently only allows for a single (1 !!!) language understanding model to be used. If the API developer-user specifies
    // more than a single language understanding model via AddIntent(), we'll fail this call with SPXERR_ABORT. However, specifying more than one intent, where
    // all of those intents are from the same language understanding model, is supported. The code below iterates thru all the triggers, finding the
    // "hostName/relativePath" ... It stores the first one it finds. It then continues iterating thru the triggers, ensuring
    // that all the other triggers have data that links them to the same language understanding model found initially... 

    std::string hostName, relativePath, id, key, region;

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

            str = PAL::ToString(model->GetRegion());
            SPX_IFTRUE_THROW_HR(!str.empty() && !region.empty() && str != region, SPXERR_ABORT);
            region = str;

            str = PAL::ToString(model->GetHostName());
            SPX_IFTRUE_THROW_HR(!str.empty() && !hostName.empty() && str != hostName, SPXERR_INVALID_URL);
            hostName = str;

            str = PAL::ToString(model->GetPathAndQuery());
            SPX_IFTRUE_THROW_HR(!str.empty() && !relativePath.empty() && str != relativePath, SPXERR_INVALID_URL);
            relativePath = str;
        }
    }

    *phostName = hostName;
    *prelativePath = relativePath + Impl::UrlEncode(query);
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
    else if (!m_matchAllIntents)
    {
        return L""; // blank intentId indicating we didn't find it...
    }

    return m_useIntentNameAsIdWhenNotFoundInMap
        ? intentName
        : m_useThisIdWhenNameNotFoundInMap;
}


} } } } // Microsoft::CognitiveServices::Speech::Impl
