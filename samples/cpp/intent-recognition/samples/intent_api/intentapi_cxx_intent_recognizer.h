//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//
// intentapi_cxx_intent_recognizer.h: Intent recognition API declarations for IntentRecognizer C++ class
//
#pragma once

#include <future>
#include <sstream>

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
class IntentRecognizer : public std::enable_shared_from_this<IntentRecognizer>
{
public:

    /// <summary>
    /// Creates an intent recognizer from the specified language.
    /// Users should use this function to create a new instance of an intent recognizer.
    /// </summary>
    /// <param name="language">Language tag in BCP 47 format.</param>
    /// <returns>Instance of intent recognizer.</returns>
    static std::shared_ptr<IntentRecognizer> FromLanguage(const std::string& language = "en-US")
    {
        return std::make_shared<IntentRecognizer>(language);
    }

    /// <summary>
    /// Internal constructor. Creates a new instance from the provided language.
    /// </summary>
    /// <param name="language">Language tag in BCP 47 format.</param>
    explicit IntentRecognizer(const std::string& language) :
        m_language(language)
    {
        m_recognizer = std::make_shared<CSpxIntentRecognizer>(language);
        m_recognizer->Init();
    }

    /// <summary>
    /// Destructor.
    /// </summary>
    ~IntentRecognizer()
    {
        m_recognizer->Term();
    }

    /// <summary>
    /// Starts intent recognition, and generates a result from the text passed in.
    /// It is only valid for offline pattern matching or exact matching intents.
    /// </summary>
    /// <param name="text">The text to be evaluated.</param>
    /// <returns>Future containing result value (a shared pointer to IntentRecognitionResult)
    /// of the asynchronous intent recognition.
    /// </returns>
    std::future<std::shared_ptr<IntentRecognitionResult>> RecognizeOnceAsync(std::string text)
    {
        auto keepAlive = this->shared_from_this();
        auto future = std::async(std::launch::async, [keepAlive, this, text]() -> std::shared_ptr<IntentRecognitionResult> {
            std::string intentId;
            std::string jsonResult;
            std::string detailedJsonResult;
            m_recognizer->ProcessText(text, intentId, jsonResult, detailedJsonResult);

            return std::make_shared<IntentRecognitionResult>(intentId, jsonResult, detailedJsonResult);
            });
        return future;
    }

    /// <summary>
    /// Adds a simple phrase that may appear in the input text, indicating a specific user intent.
    /// This simple phrase can be a pattern including and enitity surrounded by braces. Such as "click the {checkboxName} checkbox".
    /// 
    /// </summary>
    /// <param name="simplePhrase">The phrase corresponding to the intent.</param>
    /// <remarks>Once recognized, the IntentRecognitionResult's IntentId property will match the simplePhrase specified here.
    /// If any entities are specified and matched, they will be available in the IntentResult->GetEntities() call.
    /// </remarks>
    void AddIntent(const std::string& simplePhrase)
    {
        auto trigger = std::shared_ptr<ISpxTrigger>(new CSpxIntentTrigger());
        trigger->InitPhraseTrigger(simplePhrase);
        return AddIntent(trigger, simplePhrase);
    }

    /// <summary>
    /// Adds a simple phrase that may appear in the input text, indicating a specific user intent.
    /// This simple phrase can be a pattern including and enitity surrounded by braces. Such as "click the {checkboxName} checkbox".
    /// </summary>
    /// <param name="simplePhrase">The phrase corresponding to the intent.</param>
    /// <param name="intentId">A custom id string to be returned in the IntentRecognitionResult's IntentId property.</param>
    /// <remarks>Once recognized, the result's intent id will match the id supplied here.
    /// If any entities are specified and matched, they will be available in the IntentResult->GetEntities() call.
    /// </remarks>
    void AddIntent(const std::string& simplePhrase, const std::string& intentId)
    {
        auto trigger = std::shared_ptr<ISpxTrigger>(new CSpxIntentTrigger());
        trigger->InitPhraseTrigger(simplePhrase);
        return AddIntent(trigger, intentId);
    }

    /// <summary>
    /// Adds an intent by name from the specified Language Understanding Model.
    /// For PatternMatchingModel type, this will clear any existing models and add all intents from it. The intentName is ignored.
    /// </summary>
    /// <param name="model">The language understanding model containing the intent.</param>
    /// <param name="intentName">The name of the single intent to be included from the language understanding model.</param>
    void AddIntent(std::shared_ptr<LanguageUnderstandingModel> model, const std::string& intentName)
    {
        switch (model->GetModelType())
        {
        case LanguageUnderstandingModel::LanguageUnderstandingModelType::PatternMatchingModel:
            m_recognizer->ClearLanguageModels();
            AddPatternMatchingModel(model);
            (void)intentName; // ignored
            break;
        default:
            // No other model types are supported.
            break;
        }
    }

    /// <summary>
    /// Adds an intent by name from the specified Language Understanding Model.
    /// For PatternMatchingModel type, this will clear any existing models and add all intents from it. The intentName and intentId are ignored.
    /// </summary>
    /// <param name="model">The language understanding model containing the intent.</param>
    /// <param name="intentName">The name of the single intent to be included from the language understanding model.</param>
    /// <param name="intentId">A custom id string to be returned in the IntentRecognitionResult's IntentId property.</param>
    void AddIntent(std::shared_ptr<LanguageUnderstandingModel> model, const std::string& intentName, const std::string& intentId)
    {
        switch (model->GetModelType())
        {
        case LanguageUnderstandingModel::LanguageUnderstandingModelType::PatternMatchingModel:
            m_recognizer->ClearLanguageModels();
            AddPatternMatchingModel(model);
            (void)intentName; // ignored
            (void)intentId;   // ignored
            break;
        default:
            // No other model types are supported.
            break;
        }
    }

    /// <summary>
    /// Adds all intents from the specified Language Understanding Model.
    /// For PatternMatchingModel type, this will clear any existing models before enabling it.
    /// </summary>
    /// <param name="model">The language understanding model containing the intents.</param>
    void AddAllIntents(std::shared_ptr<LanguageUnderstandingModel> model)
    {
        switch (model->GetModelType())
        {
        case LanguageUnderstandingModel::LanguageUnderstandingModelType::PatternMatchingModel:
            m_recognizer->ClearLanguageModels();
            AddPatternMatchingModel(model);
            break;
        default:
            // No other model types are supported.
            break;
        }
    }

    /// <summary>
    /// Adds all intents from the specified Language Understanding Model.
    /// For PatternMatchingModel type, this will clear any existing models before enabling it. The intentId is ignored.
    /// </summary>
    /// <param name="model">The language understanding model containing the intents.</param>
    /// <param name="intentId">A custom string id to be returned in the IntentRecognitionResult's IntentId property.</param>
    void AddAllIntents(std::shared_ptr<LanguageUnderstandingModel> model, const std::string& intentId)
    {
        switch (model->GetModelType())
        {
        case LanguageUnderstandingModel::LanguageUnderstandingModelType::PatternMatchingModel:
            m_recognizer->ClearLanguageModels();
            AddPatternMatchingModel(model);
            (void)intentId; // ignored
        default:
            // No other model types are supported.
            break;
        }
    }

    /// <summary>
    /// Takes a collection of language understanding models, makes a copy of them, and applies them to the recognizer.
    /// This replaces any previously applied models.
    /// </summary>
    /// <param name="collection">A vector of shared pointers to LanguageUnderstandingModels.</param>
    /// <returns>True if the application of the models takes effect immediately. Otherwise false.</returns>
    bool ApplyLanguageModels(const std::vector<std::shared_ptr<LanguageUnderstandingModel>>& collection)
    {
        bool result = true;

        // Clear existing language models.
        m_recognizer->ClearLanguageModels();

        // Add the new ones.
        for (auto model : collection)
        {
            switch (model->GetModelType())
            {
            case LanguageUnderstandingModel::LanguageUnderstandingModelType::PatternMatchingModel:
                AddPatternMatchingModel(model);
                break;
            default:
                // No other model types are supported.
                break;
            }
        }
        return result;
    }

private:

    void AddIntent(std::shared_ptr<ISpxTrigger> trigger, const std::string& intentId)
    {
        m_recognizer->AddIntentTrigger(intentId, trigger, "");
    }

    enum class PhraseGetterHr
    {
        NoError = 0,
        InvalidArg = 1,
        OutOfRange = 2,
        UnhandledException = 3
    };

    void AddPatternMatchingModel(const std::shared_ptr<LanguageUnderstandingModel>& luModel) const
    {
        auto model = static_cast<const PatternMatchingModel*>(luModel.get());
        auto modelId = model->GetModelId();

        auto pmModel = std::make_shared<CSpxPatternMatchingModel>(modelId);
        auto pmFactory = std::make_shared<CSpxPatternMatchingFactory>();
        pmFactory->Init(pmModel, m_language);

        typedef PhraseGetterHr(__stdcall* INTENT_PATTERN_MATCHING_MODEL_GET_STR_FROM_INDEX)(void* context, size_t index, const char** str, size_t* size);
        INTENT_PATTERN_MATCHING_MODEL_GET_STR_FROM_INDEX phraseGetter = [](void* context, size_t index, const char** phrase, size_t* phraseLen) -> PhraseGetterHr
            {
                try
                {
                    if (context == nullptr || phrase == nullptr || phraseLen == nullptr)
                    {
                        return PhraseGetterHr::InvalidArg;
                    }

                    auto phrases = static_cast<std::vector<std::string>*>(context);
                    if (index >= phrases->size())
                    {
                        return PhraseGetterHr::OutOfRange;
                    }

                    *phrase = phrases->at(index).c_str();
                    *phraseLen = phrases->at(index).length();
                    return PhraseGetterHr::NoError;
                }
                catch (...)
                {
                    return PhraseGetterHr::UnhandledException;
                }
            };

        for (const auto& entity : model->Entities)
        {
            auto pmEntity = pmFactory->CreateEntity(entity.Type);
            pmEntity->Init(entity.Id, pmModel->GetOrthographyInfo());
            pmEntity->SetMode(entity.Mode);

            auto phraseContext = (void*)&entity.Phrases;
            for (size_t i = 0; i < entity.Phrases.size(); i++)
            {
                const char* phrase = nullptr;
                size_t phraseLen = 0;
                auto hr = phraseGetter(phraseContext, i, &phrase, &phraseLen);
                if (hr != PhraseGetterHr::NoError)
                {
                    std::ostringstream msg;
                    msg << "phraseGetter error " << (int)hr << " with modelId " << modelId << " entity.Phrases[" << i << "] " << entity.Phrases[i];
                    throw std::runtime_error(msg.str());
                }
                pmEntity->AddPhrase(std::string(phrase, phraseLen));
            }
            pmFactory->AddEntity(pmEntity);
        }

        for (const auto& intent : model->Intents)
        {
            auto pmIntent = pmFactory->CreateIntent();
            auto priority = 0; // no priority at the moment so set to 0
            pmIntent->Init(intent.Id, priority, pmModel->GetOrthographyInfo().Name);

            auto phraseContext = (void*)&intent.Phrases;
            for (size_t i = 0; i < intent.Phrases.size(); i++)
            {
                const char* phrase = nullptr;
                size_t phraseLen = 0;
                auto hr = phraseGetter(phraseContext, i, &phrase, &phraseLen);
                if (hr != PhraseGetterHr::NoError)
                {
                    std::ostringstream msg;
                    msg << "phraseGetter error " << (int)hr << " with modelId " << modelId << " intent.Phrases[" << i << "] " << intent.Phrases[i];
                    throw std::runtime_error(msg.str());
                }
                pmIntent->AddPhrase(std::string(phrase, phraseLen));
            }
            pmFactory->AddIntent(pmIntent);
        }

        auto pmTrigger = std::shared_ptr<ISpxTrigger>(new CSpxIntentTrigger());
        pmTrigger->InitPatternMatchingModelTrigger(pmModel);

        m_recognizer->AddIntentTrigger("", pmTrigger, modelId);
    }

    std::shared_ptr<CSpxIntentRecognizer> m_recognizer;
    std::string m_language;
};

}}}}
