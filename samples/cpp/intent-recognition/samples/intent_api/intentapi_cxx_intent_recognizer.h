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

    /// <summary>
    /// Creates an intent recognizer from the specified language.
    /// Users should use this function to create a new instance of an intent recognizer.
    /// </summary>
    /// <param name="language">Language tag in BCP 47 format.</param>
    /// <returns>Instance of intent recognizer.</returns>
    static std::shared_ptr<IntentRecognizer> FromLanguage(const std::string& language = "en-US");

    /// <summary>
    /// Internal constructor. Creates a new instance from the provided language.
    /// </summary>
    /// <param name="language">Language tag in BCP 47 format.</param>
    explicit IntentRecognizer(const std::string& language);

    IntentRecognizer(const IntentRecognizer&) = delete;
    IntentRecognizer& operator=(const IntentRecognizer&) = delete;
    IntentRecognizer(IntentRecognizer&&) = delete;
    IntentRecognizer& operator=(IntentRecognizer&&) = delete;

    /// <summary>
    /// Destructor.
    /// </summary>
    ~IntentRecognizer();

    /// <summary>
    /// Starts intent recognition, and generates a result from the text passed in.
    /// It is only valid for offline pattern matching or exact matching intents.
    /// </summary>
    /// <param name="text">The text to be evaluated.</param>
    /// <returns>Future containing result value (a shared pointer to IntentRecognitionResult)
    /// of the asynchronous intent recognition.
    /// </returns>
    std::future<std::shared_ptr<IntentRecognitionResult>> RecognizeOnceAsync(std::string text);

    /// <summary>
    /// Adds a simple phrase that may appear in the input text, indicating a specific user intent.
    /// This simple phrase can be a pattern including and enitity surrounded by braces. Such as "click the {checkboxName} checkbox".
    /// 
    /// </summary>
    /// <param name="simplePhrase">The phrase corresponding to the intent.</param>
    /// <remarks>Once recognized, the IntentRecognitionResult's IntentId property will match the simplePhrase specified here.
    /// If any entities are specified and matched, they will be available in the IntentResult->GetEntities() call.
    /// </remarks>
    void AddIntent(const std::string& simplePhrase);

    /// <summary>
    /// Adds a simple phrase that may appear in the input text, indicating a specific user intent.
    /// This simple phrase can be a pattern including and enitity surrounded by braces. Such as "click the {checkboxName} checkbox".
    /// </summary>
    /// <param name="simplePhrase">The phrase corresponding to the intent.</param>
    /// <param name="intentId">A custom id string to be returned in the IntentRecognitionResult's IntentId property.</param>
    /// <remarks>Once recognized, the result's intent id will match the id supplied here.
    /// If any entities are specified and matched, they will be available in the IntentResult->GetEntities() call.
    /// </remarks>
    void AddIntent(const std::string& simplePhrase, const std::string& intentId);

    /// <summary>
    /// Adds an intent by name from the specified Language Understanding Model.
    /// For PatternMatchingModel type, this will clear any existing models and add all intents from it. The intentName is ignored.
    /// </summary>
    /// <param name="model">The language understanding model containing the intent.</param>
    /// <param name="intentName">The name of the single intent to be included from the language understanding model.</param>
    void AddIntent(std::shared_ptr<LanguageUnderstandingModel> model, const std::string& intentName);

    /// <summary>
    /// Adds an intent by name from the specified Language Understanding Model.
    /// For PatternMatchingModel type, this will clear any existing models and add all intents from it. The intentName and intentId are ignored.
    /// </summary>
    /// <param name="model">The language understanding model containing the intent.</param>
    /// <param name="intentName">The name of the single intent to be included from the language understanding model.</param>
    /// <param name="intentId">A custom id string to be returned in the IntentRecognitionResult's IntentId property.</param>
    void AddIntent(std::shared_ptr<LanguageUnderstandingModel> model, const std::string& intentName, const std::string& intentId);

    /// <summary>
    /// Adds all intents from the specified Language Understanding Model.
    /// For PatternMatchingModel type, this will clear any existing models before enabling it.
    /// </summary>
    /// <param name="model">The language understanding model containing the intents.</param>
    void AddAllIntents(std::shared_ptr<LanguageUnderstandingModel> model);

    /// <summary>
    /// Adds all intents from the specified Language Understanding Model.
    /// For PatternMatchingModel type, this will clear any existing models before enabling it. The intentId is ignored.
    /// </summary>
    /// <param name="model">The language understanding model containing the intents.</param>
    /// <param name="intentId">A custom string id to be returned in the IntentRecognitionResult's IntentId property.</param>
    void AddAllIntents(std::shared_ptr<LanguageUnderstandingModel> model, const std::string& intentId);

    /// <summary>
    /// Takes a collection of language understanding models, makes a copy of them, and applies them to the recognizer.
    /// This replaces any previously applied models.
    /// </summary>
    /// <param name="collection">A vector of shared pointers to LanguageUnderstandingModels.</param>
    /// <returns>True if the application of the models takes effect immediately. Otherwise false.</returns>
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
