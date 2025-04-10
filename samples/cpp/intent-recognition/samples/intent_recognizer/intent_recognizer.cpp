//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//

#include "stdafx.h"

#include <regex>
#include <set>

#include "intent_match_result.h"
#include "intent_trigger.h"
#include "intent_recognizer.h"
#include "pattern_matching_intent.h"
#include "pattern_matching_model.h"
#include "string_utils.h"

namespace Microsoft {
namespace SpeechSDK {
namespace Standalone {
namespace Intent {
namespace Impl {

CSpxIntentRecognizer::CSpxIntentRecognizer(const std::string& lang) :
    m_lang(lang)
{
    SPX_DBG_TRACE_FUNCTION();
}

CSpxIntentRecognizer::~CSpxIntentRecognizer()
{
    SPX_DBG_TRACE_FUNCTION();
}

void CSpxIntentRecognizer::Init()
{
    m_defaultPatternMatchingModel = std::make_shared<CSpxPatternMatchingModel>("");
    m_defaultPatternMatchingModel->Init(m_lang);
}

void CSpxIntentRecognizer::Term()
{
    m_triggerMap.clear();
    ClearLanguageModels();
}

void CSpxIntentRecognizer::ClearLanguageModels()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    m_patternMatchingModelMap.clear();
}

void CSpxIntentRecognizer::AddIntentTrigger(const std::string& id, const ISpxTrigger::Ptr& trigger, const std::string& modelId)
{
    auto modelIdStr = modelId;
    auto intentId = id;

    auto phrase = trigger->GetPhrase();

    // If this is a phrase without a model, meaning AddIntent() was called check if we have a pattern or exact match.
    if (!phrase.empty() && modelIdStr.empty())
    {
        auto intent = std::make_shared<CSpxPatternMatchingIntent>();
        auto speechOrthography = PAL::StringUtils::ToLower(m_lang);
        speechOrthography = speechOrthography.substr(0, speechOrthography.find_first_of('-'));

        intent->Init(intentId, 0, speechOrthography);
        intent->AddPhrase(phrase);

        m_defaultPatternMatchingModel->AddIntent(intent, intentId);
    }

    auto model = trigger->GetModel();
    if (model != nullptr)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_patternMatchingModelMap.emplace(model->GetId(), model);
    }
}

std::shared_ptr<CSpxPatternMatchingModel> CSpxIntentRecognizer::GetOrCreateModel(const std::string& key)
{
    if (m_patternMatchingModelMap.find(key) != m_patternMatchingModelMap.end())
    {
        return m_patternMatchingModelMap[key];
    }
    else
    {
        auto model = std::make_shared<CSpxPatternMatchingModel>("");
        model->Init(m_lang);
        std::unique_lock<std::mutex> lock(m_mutex);
        m_patternMatchingModelMap[key] = model;
        return model;
    }
}

void CSpxIntentRecognizer::ProcessText(const std::string& inputText, std::string& intentId, std::string& jsonResult, std::string& detailedJsonResult)
{
    SPX_DBG_TRACE_FUNCTION();

    // We only need to process the result when the input actually contains something...
    SPX_DBG_TRACE_VERBOSE("%s: text='%s'", __FUNCTION__, inputText.c_str());
    if (!inputText.empty())
    {
        InitIntentResult();

        intentId = IntentIdFromPatternMatchingModels(inputText);

        if (!intentId.empty())
        {
            jsonResult = m_jsonResult;
            detailedJsonResult = m_detailedJsonResult;
        }
    }
}

void CSpxIntentRecognizer::InitIntentResult(const char* intentId, const char* jsonResult, const char* detailedJsonResult)
{
    m_intentId = (intentId != nullptr) ? intentId : "";
    m_jsonResult = (jsonResult != nullptr) ? jsonResult : "";
    m_detailedJsonResult = (detailedJsonResult != nullptr) ? detailedJsonResult : "";
}

std::string CSpxIntentRecognizer::PrepareSimplePatternResults(std::set<std::shared_ptr<CSpxIntentMatchResult>, SpxIntentMatchResultCompare>& matchResults, const std::string& inputText)
{
    // Grab the first intent which should be the highest priority and return that. Pass other matches to the intent result
    // to be stored in the detailed property.
    auto& firstIntent = *matchResults.begin();
    auto& firstIntentId = firstIntent->GetIntentId();
    auto entitiesJson = ajv::json::Build();

    // Copy the entities into the json
    for (auto& entity : firstIntent->GetEntities())
    {
        entitiesJson[entity.first] = entity.second.Value;
    }

    // Build the detailed intent json
    auto intentsJson = ajv::json::Build();
    int index = 0;
    for (auto matchResult : matchResults)
    {
        AddToPatternMatchingJson(intentsJson[index], matchResult);
        index++;
    }

    // Update our result with the appropriate ID and json entities
    InitIntentResult(firstIntentId.c_str(), entitiesJson.AsJson().c_str(), intentsJson.AsJson().c_str());

    return firstIntentId;
}

void CSpxIntentRecognizer::AddToPatternMatchingJson(ajv::JsonBuilder::JsonWriter writer, std::shared_ptr<CSpxIntentMatchResult> matchResult) const
{
    writer["intentId"] = matchResult->GetIntentId();
    writer["pattern"] = matchResult->GetPattern();
    writer["priority"] = matchResult->GetPriority();

    for (auto& entity : matchResult->GetEntities())
    {
        writer["entities"][entity.first] = entity.second.Value;
    }
}

std::string CSpxIntentRecognizer::IntentIdFromPatternMatchingModels(const std::string& inputText)
{
    std::unique_lock<std::mutex> lock(m_mutex);

    auto phrase = CSpxIntentTrigger::NormalizeInput(inputText);

    if (phrase.empty())
    {
        return phrase;
    }

    std::set<std::shared_ptr<CSpxIntentMatchResult>, SpxIntentMatchResultCompare> intentResults{};

    for (const auto& model : m_patternMatchingModelMap)
    {
        const auto& patternModel = model.second;
        auto results = patternModel->FindMatches(phrase);
        if (results.size() != 0)
        {
            for (auto& matchResult : results)
            {
                intentResults.insert(matchResult);
            }
        }
    }

    // Don't forget the default model.
    if (m_defaultPatternMatchingModel)
    {
        auto results = m_defaultPatternMatchingModel->FindMatches(phrase);
        if (results.size() != 0)
        {
            for (auto& matchResult : results)
            {
                intentResults.insert(matchResult);
            }
        }
    }

    if (intentResults.size() == 0)
    {
        return "";
    }
    else
    {
        return PrepareSimplePatternResults(intentResults, inputText);
    }
}

}}}}}
