//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//
// intentapi_cxx_intent_recognizer.h: Intent recognition API declarations for IntentRecognizer C++ class
//
#pragma once

#include <future>
#include <memory>
#include <sstream>

#include <intentapi_cxx_exports.h>
#include <intentapi_cxx_intent_recognition_result.h>
#include <intentapi_cxx_pattern_matching_model.h>

#include <intent_recognizer.h>

namespace Microsoft {
namespace SpeechSDK {
namespace Standalone {
namespace Intent {

/// <summary>
/// IntentRecognizer extracts structured information about intents in text.
/// This standalone version of IntentRecognizer is valid only for offline pattern matching or exact matching intents.
/// It does not use or depend on cloud services and does not require the Speech SDK to run.
/// Supported languages are listed in https://learn.microsoft.com/azure/ai-services/speech-service/language-support?tabs=intent-recognizer-pattern-matcher
/// </summary>
class INTENT_API IntentRecognizer
{
public:

    static std::shared_ptr<IntentRecognizer> FromLanguage(const std::string& language = "en-US");

    explicit IntentRecognizer(const std::string& language);

    IntentRecognizer(const IntentRecognizer&) = delete;
    IntentRecognizer& operator=(const IntentRecognizer&) = delete;
    IntentRecognizer(IntentRecognizer&&) = delete;
    IntentRecognizer& operator=(IntentRecognizer&&) = delete;

    ~IntentRecognizer();

    std::future<std::shared_ptr<IntentRecognitionResult>> RecognizeOnceAsync(std::string text);

    void AddIntent(const std::string& simplePhrase);

    void AddIntent(const std::string& simplePhrase, const std::string& intentId);

    void AddIntent(std::shared_ptr<LanguageUnderstandingModel> model, const std::string& intentName);

    void AddIntent(std::shared_ptr<LanguageUnderstandingModel> model, const std::string& intentName, const std::string& intentId);

    void AddAllIntents(std::shared_ptr<LanguageUnderstandingModel> model);

    void AddAllIntents(std::shared_ptr<LanguageUnderstandingModel> model, const std::string& intentId);

    bool ApplyLanguageModels(const std::vector<std::shared_ptr<LanguageUnderstandingModel>>& collection);

private:

    void AddIntent(std::shared_ptr<ISpxTrigger> trigger, const std::string& intentId);

    enum class PhraseGetterHr
    {
        NoError = 0,
        InvalidArg = 1,
        OutOfRange = 2,
        UnhandledException = 3
    };

    void AddPatternMatchingModel(const std::shared_ptr<LanguageUnderstandingModel>& luModel) const;

    struct State
    {
        std::shared_ptr<CSpxIntentRecognizer> recognizer;
        std::string language;
    };

    State* m_state;
};

}}}}
