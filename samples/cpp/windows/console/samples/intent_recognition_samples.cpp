//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "stdafx.h"

// <toplevel>
#include <speechapi_cxx.h>

using namespace Microsoft::CognitiveServices::Speech;
using namespace Microsoft::CognitiveServices::Speech::Audio;
using namespace Microsoft::CognitiveServices::Speech::Intent;
// </toplevel>

// Intent recognition using microphone.
void IntentRecognitionWithMicrophone()
{
    // <IntentRecognitionWithMicrophone>
    // Creates an instance of a speech config with specified subscription key
    // and service region. Note that in contrast to other services supported by
    // the Cognitive Services Speech SDK, the Language Understanding service
    // requires a specific subscription key from https://www.luis.ai/.
    // The Language Understanding service calls the required key 'endpoint key'.
    // Once you've obtained it, replace with below with your own Language Understanding subscription key
    // and service region (e.g., "westus").
    // The default recognition language is "en-us".
    auto config = SpeechConfig::FromSubscription("YourLanguageUnderstandingSubscriptionKey", "YourLanguageUnderstandingServiceRegion");

    // Creates an intent recognizer using microphone as audio input.
    auto recognizer = IntentRecognizer::FromConfig(config);

    // Creates a Language Understanding model using the app id, and adds specific intents from your model
    auto model = LanguageUnderstandingModel::FromAppId("YourLanguageUnderstandingAppId");
    recognizer->AddIntent(model, "YourLanguageUnderstandingIntentName1", "id1");
    recognizer->AddIntent(model, "YourLanguageUnderstandingIntentName2", "id2");
    recognizer->AddIntent(model, "YourLanguageUnderstandingIntentName3", "any-IntentId-here");

    std::cout << "Say something..." << std::endl;

    // Starts intent recognition, and returns after a single utterance is recognized. The end of a
    // single utterance is determined by listening for silence at the end or until a maximum of 15
    // seconds of audio is processed.  The task returns the recognition text as result. 
    // Note: Since RecognizeOnceAsync() returns only a single utterance, it is suitable only for single
    // shot recognition like command or query. 
    // For long-running multi-utterance recognition, use StartContinuousRecognitionAsync() instead.
    auto result = recognizer->RecognizeOnceAsync().get();

    // Checks result.
    if (result->Reason == ResultReason::RecognizedIntent)
    {
        std::cout << "RECOGNIZED: Text=" << result->Text << std::endl;
        std::cout << "  Intent Id: " << result->IntentId << std::endl;
        std::cout << "  Intent Service JSON: " << result->Properties.GetProperty(PropertyId::LanguageUnderstandingServiceResponse_JsonResult) << std::endl;
    }
    else if (result->Reason == ResultReason::RecognizedSpeech)
    {
        std::cout << "RECOGNIZED: Text=" << result->Text << " (intent could not be recognized)" << std::endl;
    }
    else if (result->Reason == ResultReason::NoMatch)
    {
        std::cout << "NOMATCH: Speech could not be recognized." << std::endl;
    }
    else if (result->Reason == ResultReason::Canceled)
    {
        auto cancellation = CancellationDetails::FromResult(result);
        std::cout << "CANCELED: Reason=" << (int)cancellation->Reason << std::endl;

        if (cancellation->Reason == CancellationReason::Error)
        {
            std::cout << "CANCELED: ErrorCode=" << (int)cancellation->ErrorCode << std::endl;
            std::cout << "CANCELED: ErrorDetails=" << cancellation->ErrorDetails << std::endl;
            std::cout << "CANCELED: Did you update the subscription info?" << std::endl;
        }
    }
    // </IntentRecognitionWithMicrophone>
}

// Intent recognition in the specified language, using microphone.
void IntentRecognitionWithLanguage()
{
    // <IntentRecognitionWithLanguage>
    // Creates an instance of a speech config with specified subscription key
    // and service region. Note that in contrast to other services supported by
    // the Cognitive Services Speech SDK, the Language Understanding service
    // requires a specific subscription key from https://www.luis.ai/.
    // The Language Understanding service calls the required key 'endpoint key'.
    // Once you've obtained it, replace with below with your own Language Understanding service subscription key
    // and service region (e.g., "westus").
    auto config = SpeechConfig::FromSubscription("YourLanguageUnderstandingSubscriptionKey", "YourLanguageUnderstandingServiceRegion");

    // Creates an intent recognizer in the specified language using microphone as audio input.
    auto lang = "de-de";
    config->SetSpeechRecognitionLanguage(lang);
    auto recognizer = IntentRecognizer::FromConfig(config);

    // Creates a Language Understanding model using the app id, and adds specific intents from your model
    auto model = LanguageUnderstandingModel::FromAppId("YourLanguageUnderstandingAppId");
    recognizer->AddIntent(model, "YourLanguageUnderstandingIntentName1", "id1");
    recognizer->AddIntent(model, "YourLanguageUnderstandingIntentName2", "id2");
    recognizer->AddIntent(model, "YourLanguageUnderstandingIntentName3", "any-IntentId-here");

    std::cout << "Say something in " << lang << "..." << std::endl;

    // Starts speech recognition, and returns after a single utterance is recognized. The end of a
    // single utterance is determined by listening for silence at the end, or until a maximum of 15
    // seconds of audio is processed.  The future returns the recognition text as result. 
    // Note: Since RecognizeOnceAsync() returns only a single utterance, it is suitable only for single
    // shot recognition like command or query. 
    // For long-running multi-utterance recognition, use StartContinuousRecognitionAsync() instead.
    auto result = recognizer->RecognizeOnceAsync().get();

    // Checks result.
    if (result->Reason == ResultReason::RecognizedIntent)
    {
        std::cout << "RECOGNIZED: Text=" << result->Text << std::endl;
        std::cout << "  Intent Id: " << result->IntentId << std::endl;
        std::cout << "  Intent Service JSON: " << result->Properties.GetProperty(PropertyId::LanguageUnderstandingServiceResponse_JsonResult) << std::endl;
    }
    else if (result->Reason == ResultReason::RecognizedSpeech)
    {
        std::cout << "RECOGNIZED: Text=" << result->Text << " (intent could not be recognized)" << std::endl;
    }
    else if (result->Reason == ResultReason::NoMatch)
    {
        std::cout << "NOMATCH: Speech could not be recognized." << std::endl;
    }
    else if (result->Reason == ResultReason::Canceled)
    {
        auto cancellation = CancellationDetails::FromResult(result);
        std::cout << "CANCELED: Reason=" << (int)cancellation->Reason << std::endl;

        if (cancellation->Reason == CancellationReason::Error)
        {
            std::cout << "CANCELED: ErrorCode=" << (int)cancellation->ErrorCode << std::endl;
            std::cout << "CANCELED: ErrorDetails=" << cancellation->ErrorDetails << std::endl;
            std::cout << "CANCELED: Did you update the subscription info?" << std::endl;
        }
    }
    // </IntentRecognitionWithLanguage>
}

// Continuous intent recognition.
void IntentContinuousRecognitionWithFile()
{
    // <IntentContinuousRecognitionWithFile>
    // Creates an instance of a speech config with specified subscription key
    // and service region. Note that in contrast to other services supported by
    // the Cognitive Services Speech SDK, the Language Understanding service
    // requires a specific subscription key from https://www.luis.ai/.
    // The Language Understanding service calls the required key 'endpoint key'.
    // Once you've obtained it, replace with below with your own Language Understanding subscription key
    // and service region (e.g., "westus").
    auto config = SpeechConfig::FromSubscription("YourLanguageUnderstandingSubscriptionKey", "YourLanguageUnderstandingServiceRegion");

    // Creates an intent recognizer using file as audio input.
    // Replace with your own audio file name.
    auto audioInput = AudioConfig::FromWavFileInput("whatstheweatherlike.wav");
    auto recognizer = IntentRecognizer::FromConfig(config, audioInput);

    // std::promise for synchronization of recognition end.
    std::promise<void> recognitionEnd;

    // Creates a Language Understanding model using the app id, and adds specific intents from your model
    auto model = LanguageUnderstandingModel::FromAppId("YourLanguageUnderstandingAppId");
    recognizer->AddIntent(model, "YourLanguageUnderstandingIntentName1", "id1");
    recognizer->AddIntent(model, "YourLanguageUnderstandingIntentName2", "id2");
    recognizer->AddIntent(model, "YourLanguageUnderstandingIntentName3", "any-IntentId-here");

    // Subscribes to events.
    recognizer->Recognizing.Connect([](const IntentRecognitionEventArgs& e)
        {
            std::cout << "Recognizing:" << e.Result->Text << std::endl;
        });

    recognizer->Recognized.Connect([](const IntentRecognitionEventArgs& e)
        {
            if (e.Result->Reason == ResultReason::RecognizedIntent)
            {
                std::cout << "RECOGNIZED: Text=" << e.Result->Text << std::endl;
                std::cout << "  Intent Id: " << e.Result->IntentId << std::endl;
                std::cout << "  Intent Service JSON: " << e.Result->Properties.GetProperty(PropertyId::LanguageUnderstandingServiceResponse_JsonResult) << std::endl;
            }
            else if (e.Result->Reason == ResultReason::RecognizedSpeech)
            {
                std::cout << "RECOGNIZED: Text=" << e.Result->Text << " (intent could not be recognized)" << std::endl;
            }
            else if (e.Result->Reason == ResultReason::NoMatch)
            {
                std::cout << "NOMATCH: Speech could not be recognized." << std::endl;
            }
        });

    recognizer->Canceled.Connect([&recognitionEnd](const IntentRecognitionCanceledEventArgs& e)
        {
            std::cout << "CANCELED: Reason=" << (int)e.Reason << std::endl;

            if (e.Reason == CancellationReason::Error)
            {
                std::cout << "CANCELED: ErrorCode=" << (int)e.ErrorCode << std::endl;
                std::cout << "CANCELED: ErrorDetails=" << e.ErrorDetails << std::endl;
                std::cout << "CANCELED: Did you update the subscription info?" << std::endl;
            }

            recognitionEnd.set_value(); // Notify to stop recognition.
        });

    recognizer->SessionStopped.Connect([&recognitionEnd](const SessionEventArgs& e)
        {
            std::cout << "Session stopped.";
            recognitionEnd.set_value(); // Notify to stop recognition.
        });

    // Starts continuous recognition. Uses StopContinuousRecognitionAsync() to stop recognition.
    recognizer->StartContinuousRecognitionAsync().get();

    // Waits for recognition end.
    recognitionEnd.get_future().get();

    // Stops recognition.
    recognizer->StopContinuousRecognitionAsync().get();
    // </IntentContinuousRecognitionWithFile>
}

// Intent recognition using pattern matching and microphone.
void IntentRecognitionWithPatternMatchingAndMicrophone()
{
    // <IntentRecognitionWithPatternMatchingAndMicrophone>
    // Creates an instance of a speech config with specified subscription key
    // and service region. Note that in contrast to the other samples this DOES NOT require a
    // LUIS application.
    // The default recognition language is "en-us".
    auto config = SpeechConfig::FromSubscription("YourLanguageUnderstandingSubscriptionKey", "YourLanguageUnderstandingServiceRegion");

    // Creates an intent recognizer using microphone as audio input.
    auto recognizer = IntentRecognizer::FromConfig(config);

    // Creates a Pattern Matching model and adds specific intents from your model. 
    // The Id is used to identify this model from others in the collection.
    auto model = PatternMatchingModel::FromModelId("YourPatternMatchingModelId");

    // Creates a std::string with a pattern that uses groups of optional words. "[Go | Take me]" will match either "Go", "Take me", or "".
    std::string patternWithOptionalWords = "[Go | Take me] to [floor|level] {floorName}";
    // Creates a std::string with a pattern that uses an optional entity and group that could be used to tie commands together.
    std::string patternWithOptionalEntity = "Go to parking [{parkingLevel}]";

    // You can also have multiple entities of the same name in a single pattern by adding appending a unique identifier
    // to distinguish between the instances. For example:
    std::string patternWithTwoOfTheSameEntity = "Go to floor {floorName:1} [and then go to floor {floorName:2}]";
    // NOTE: Both floorName:1 and floorName:2 are tied to the same list of entries. The identifier can be a string
    //       and is separated from the entity name by a ':'

    // Adds some intents to look for specific patterns.
    model->Intents.push_back({ {patternWithOptionalWords, patternWithOptionalEntity, patternWithTwoOfTheSameEntity, "{floorName}"}, "ChangeFloors" });
    model->Intents.push_back({ {"{action} the doors", "{action} doors", "{action} the door", "{action} door"}, "DoorControl" });

    // Creates the "floorName" entity and set it to type list.
    // Adds acceptable values. NOTE the default entity type is Any and so we do not need
    // to declare the "action" entity.
    model->Entities.push_back({ "floorName" , Intent::EntityType::List, Intent::EntityMatchMode::Strict, {"ground floor", "lobby", "1st", "first", "one", "1", "2nd", "second", "two", "2"}});

    // Creates the "parkingLevel" entity as a prebuilt Integer
    model->Entities.push_back({ "parkingLevel" , Intent::EntityType::PrebuiltInteger});

    // Add the model to a model std::vector.
    std::vector<std::shared_ptr<LanguageUnderstandingModel>> modelCollection;
    modelCollection.push_back(model);

    // Apply the language model collection to the recognizer.
    recognizer->ApplyLanguageModels(modelCollection);

    std::cout << "Say something..." << std::endl;

    // Starts intent recognition, and returns after a single utterance is recognized. The end of a
    // single utterance is determined by listening for silence at the end, or until a maximum of 15
    // seconds of audio is processed.  The future returns the recognition text as result. 
    // Note: Since RecognizeOnceAsync() returns only a single utterance, it is suitable only for single
    // shot recognition like command or query. 
    // For long-running multi-utterance recognition, use StartContinuousRecognitionAsync() instead.
    auto result = recognizer->RecognizeOnceAsync().get();

    // Checks result.
    if (result->Reason == ResultReason::RecognizedIntent)
    {
        std::cout << "RECOGNIZED: Text=" << result->Text << std::endl;
        std::cout << "  Intent Id: " << result->IntentId << std::endl;
        auto entities = result->GetEntities();
        if (result->IntentId == "ChangeFloors")
        {

            if (entities.find("floorName") != entities.end())
            {
                std::cout << "     Floor name: = " << entities["floorName"].c_str() << std::endl;
            }

            if (entities.find("floorName:1") != entities.end())
            {
                std::cout << "   Floor name 1: = " << entities["floorName"].c_str() << std::endl;
            }

            if (entities.find("floorName:2") != entities.end())
            {
                std::cout << "   Floor name 2: = " << entities["floorName"].c_str() << std::endl;
            }

            if (entities.find("parkingLevel") != entities.end())
            {
                std::cout << "  Parking Level: = " << entities["floorName"].c_str() << std::endl;
            }
        }
        else if (result->IntentId == "DoorControl")
        {

            if (entities.find("action") != entities.end())
            {
                std::cout << "         Action: = " << entities["action"].c_str() << std::endl;
            }
        }
    }
    else if (result->Reason == ResultReason::RecognizedSpeech)
    {
        std::cout << "RECOGNIZED: Text=" << result->Text << " (intent could not be recognized)" << std::endl;
    }
    else if (result->Reason == ResultReason::NoMatch)
    {
        std::cout << "NOMATCH: Speech could not be recognized." << std::endl;
    }
    else if (result->Reason == ResultReason::Canceled)
    {
        auto cancellation = CancellationDetails::FromResult(result);
        std::cout << "CANCELED: Reason=" << (int)cancellation->Reason << std::endl;

        if (cancellation->Reason == CancellationReason::Error)
        {
            std::cout << "CANCELED: ErrorCode=" << (int)cancellation->ErrorCode << std::endl;
            std::cout << "CANCELED: ErrorDetails=" << cancellation->ErrorDetails << std::endl;
            std::cout << "CANCELED: Did you update the subscription info?" << std::endl;
        }
    }
    // </IntentRecognitionWithPatternMatchingAndMicrophone>
}
