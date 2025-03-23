//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//

#pragma once

#include <mutex>
#include <set>

#include <ajv.h>

#include "intent_match_result.h"
#include "intent_trigger.h"
#include "pattern_matching_model.h"

namespace Microsoft {
namespace SpeechSDK {
namespace Standalone {
namespace Intent {

using namespace Microsoft::SpeechSDK::Standalone::Intent::Impl;

namespace Impl {

class CSpxIntentRecognizer
{
public:

    CSpxIntentRecognizer(const std::string& lang);
    ~CSpxIntentRecognizer();

    void AddIntentTrigger(const std::string& id, const ISpxTrigger::Ptr& trigger, const std::string& modelId);
    void ClearLanguageModels();

    void ProcessText(const std::string& text, std::string& intentId, std::string& jsonResult, std::string& detailedJsonResult);

    void Init();
    void Term();

private:

    std::string PrepareSimplePatternResults(std::set<std::shared_ptr<CSpxIntentMatchResult>, SpxIntentMatchResultCompare>& results, const std::string& inputText);

    std::string IntentIdFromPatternMatchingModels(const std::string& inputText);
    void AddToPatternMatchingJson(ajv::JsonBuilder::JsonWriter writer, std::shared_ptr<CSpxIntentMatchResult> matchResult) const;
    std::shared_ptr<CSpxPatternMatchingModel> GetOrCreateModel(const std::string& key);

    void InitIntentResult(const char* intentId = "", const char* jsonResult = "", const char* detailedJsonResult = "");

    std::string m_lang;
    std::mutex m_mutex;
    std::map<std::string, std::list<std::shared_ptr<ISpxTrigger>>> m_triggerMap;
    std::map<const std::string, std::shared_ptr<CSpxPatternMatchingModel>> m_patternMatchingModelMap;
    std::shared_ptr<CSpxPatternMatchingModel> m_defaultPatternMatchingModel;

    std::string m_intentId;
    std::string m_jsonResult;
    std::string m_detailedJsonResult;
};

}}}}}
