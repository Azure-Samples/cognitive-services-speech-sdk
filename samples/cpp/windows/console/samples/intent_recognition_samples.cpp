//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "stdafx.h"

// <toplevel>
#include <speechapi_cxx.h>

using namespace std;
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

    cout << "Say something...\n";

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
        cout << "RECOGNIZED: Text=" << result->Text << endl;
        cout << "  Intent Id: " << result->IntentId << endl;
        cout << "  Intent Service JSON: " << result->Properties.GetProperty(PropertyId::LanguageUnderstandingServiceResponse_JsonResult) << endl;
    }
    else if (result->Reason == ResultReason::RecognizedSpeech)
    {
        cout << "RECOGNIZED: Text=" << result->Text << " (intent could not be recognized)" << endl;
    }
    else if (result->Reason == ResultReason::NoMatch)
    {
        cout << "NOMATCH: Speech could not be recognized." << endl;
    }
    else if (result->Reason == ResultReason::Canceled)
    {
        auto cancellation = CancellationDetails::FromResult(result);
        cout << "CANCELED: Reason=" << (int)cancellation->Reason << endl;

        if (cancellation->Reason == CancellationReason::Error)
        {
            cout << "CANCELED: ErrorCode=" << (int)cancellation->ErrorCode << endl;
            cout << "CANCELED: ErrorDetails=" << cancellation->ErrorDetails << endl;
            cout << "CANCELED: Did you update the subscription info?" << endl;
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

    cout << "Say something in " << lang << "..." << endl;

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
        cout << "RECOGNIZED: Text=" << result->Text << endl;
        cout << "  Intent Id: " << result->IntentId << endl;
        cout << "  Intent Service JSON: " << result->Properties.GetProperty(PropertyId::LanguageUnderstandingServiceResponse_JsonResult) << endl;
    }
    else if (result->Reason == ResultReason::RecognizedSpeech)
    {
        cout << "RECOGNIZED: Text=" << result->Text << " (intent could not be recognized)" << endl;
    }
    else if (result->Reason == ResultReason::NoMatch)
    {
        cout << "NOMATCH: Speech could not be recognized." << endl;
    }
    else if (result->Reason == ResultReason::Canceled)
    {
        auto cancellation = CancellationDetails::FromResult(result);
        cout << "CANCELED: Reason=" << (int)cancellation->Reason << endl;

        if (cancellation->Reason == CancellationReason::Error)
        {
            cout << "CANCELED: ErrorCode=" << (int)cancellation->ErrorCode << endl;
            cout << "CANCELED: ErrorDetails=" << cancellation->ErrorDetails << endl;
            cout << "CANCELED: Did you update the subscription info?" << endl;
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

    // promise for synchronization of recognition end.
    promise<void> recognitionEnd;

    // Creates a Language Understanding model using the app id, and adds specific intents from your model
    auto model = LanguageUnderstandingModel::FromAppId("YourLanguageUnderstandingAppId");
    recognizer->AddIntent(model, "YourLanguageUnderstandingIntentName1", "id1");
    recognizer->AddIntent(model, "YourLanguageUnderstandingIntentName2", "id2");
    recognizer->AddIntent(model, "YourLanguageUnderstandingIntentName3", "any-IntentId-here");

    // Subscribes to events.
    recognizer->Recognizing.Connect([](const IntentRecognitionEventArgs& e)
        {
            cout << "Recognizing:" << e.Result->Text << endl;
        });

    recognizer->Recognized.Connect([](const IntentRecognitionEventArgs& e)
        {
            if (e.Result->Reason == ResultReason::RecognizedIntent)
            {
                cout << "RECOGNIZED: Text=" << e.Result->Text << endl;
                cout << "  Intent Id: " << e.Result->IntentId << endl;
                cout << "  Intent Service JSON: " << e.Result->Properties.GetProperty(PropertyId::LanguageUnderstandingServiceResponse_JsonResult) << endl;
            }
            else if (e.Result->Reason == ResultReason::RecognizedSpeech)
            {
                cout << "RECOGNIZED: Text=" << e.Result->Text << " (intent could not be recognized)" << endl;
            }
            else if (e.Result->Reason == ResultReason::NoMatch)
            {
                cout << "NOMATCH: Speech could not be recognized." << endl;
            }
        });

    recognizer->Canceled.Connect([&recognitionEnd](const IntentRecognitionCanceledEventArgs& e)
        {
            cout << "CANCELED: Reason=" << (int)e.Reason << endl;

            if (e.Reason == CancellationReason::Error)
            {
                cout << "CANCELED: ErrorCode=" << (int)e.ErrorCode << endl;
                cout << "CANCELED: ErrorDetails=" << e.ErrorDetails << endl;
                cout << "CANCELED: Did you update the subscription info?" << endl;
            }

            recognitionEnd.set_value(); // Notify to stop recognition.
        });

    recognizer->SessionStopped.Connect([&recognitionEnd](const SessionEventArgs& e)
        {
            cout << "Session stopped.";
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

    // Adds some intents to look for specific patterns.
    model->Intents.push_back({ {"Take me to floor {floorName}", "Go to floor {floorName}", "Take me to {floorName}", "Go to floor {floorName}", "Go to {floorName}", "{floorName}"}, "ChangeFloors" });
    model->Intents.push_back({ {"{action} the doors", "{action} doors", "{action} the door", "{action} door"}, "DoorControl" });

    // Creates the "floorName" entity and set it to type list.
    // Adds acceptable values. NOTE the default entity type is Any and so we do not need
    // to declare the "action" entity.
    model->Entities.push_back({ "floorName" , Intent::EntityType::List, Intent::EntityMatchMode::Strict, {"ground floor", "lobby", "1st", "first", "one", "2nd", "second", "two"} });

    // Add the model to a model vector.
    vector<shared_ptr<LanguageUnderstandingModel>> modelCollection;
    modelCollection.push_back(model);

    // Apply the language model collection to the recognizer.
    recognizer->ApplyLanguageModels(modelCollection);

    cout << "Say something...\n";

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
        cout << "RECOGNIZED: Text=" << result->Text << endl;
        cout << "  Intent Id: " << result->IntentId << endl;

        auto entities = result->GetEntities();
        if (entities.find("floorName") != entities.end())
        {
            cout << "  Floor name: = " << entities["floorName"].c_str() << endl;
        }

        if (entities.find("action") != entities.end())
        {
            cout << "  Action: = " << entities["action"].c_str() << endl;
        }
    }
    else if (result->Reason == ResultReason::RecognizedSpeech)
    {
        cout << "RECOGNIZED: Text=" << result->Text << " (intent could not be recognized)" << endl;
    }
    else if (result->Reason == ResultReason::NoMatch)
    {
        cout << "NOMATCH: Speech could not be recognized." << endl;
    }
    else if (result->Reason == ResultReason::Canceled)
    {
        auto cancellation = CancellationDetails::FromResult(result);
        cout << "CANCELED: Reason=" << (int)cancellation->Reason << endl;

        if (cancellation->Reason == CancellationReason::Error)
        {
            cout << "CANCELED: ErrorCode=" << (int)cancellation->ErrorCode << endl;
            cout << "CANCELED: ErrorDetails=" << cancellation->ErrorDetails << endl;
            cout << "CANCELED: Did you update the subscription info?" << endl;
        }
    }
    // </IntentRecognitionWithPatternMatchingAndMicrophone>
}
