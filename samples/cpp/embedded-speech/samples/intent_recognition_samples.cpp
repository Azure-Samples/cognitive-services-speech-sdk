//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//

#include <iostream>
#include <thread>
#include <speechapi_cxx.h>

using namespace std;
using namespace Microsoft::CognitiveServices::Speech;
using namespace Microsoft::CognitiveServices::Speech::Audio;
using namespace Microsoft::CognitiveServices::Speech::Intent;

extern shared_ptr<EmbeddedSpeechConfig> CreateEmbeddedSpeechConfig();
extern const string GetKeywordModelFileName();
extern const string GetKeywordPhrase();


// Creates a pattern matching model with some example intents.
// For more examples on intent patterns, see
// https://github.com/Azure-Samples/cognitive-services-speech-sdk/blob/master/samples/cpp/windows/console/samples/intent_recognition_samples.cpp
shared_ptr<PatternMatchingModel> CreatePatternMatchingModel()
{
    // Create a pattern matching model. The id is necessary to identify this
    // model from others if there are several models in use at the same time.
    auto model = PatternMatchingModel::FromModelId("SomeUniqueIdForThisModel");

    // Make the keyword optional (surround with [ and ] in the pattern string)
    // in case the pattern matching model is used without keyword recognition.
    string patternKeywordOptional = "[" + GetKeywordPhrase() + "][{any}]";

    // Specify some intents to add. Example inputs:
    // - "Turn on the radio."
    // - "Switch off the lights in the bedroom."

    // Notes about patterns:
    // - surround entity names with { and }
    // - surround alternatives with ( and ) and separate with |
    // - surround optional parts with [ and ]
    // - default entity {any} matches any text
    string patternTurnOn = "(turn|switch) on";
    string patternTurnOff = "(turn|switch) off";
    string patternTarget = "[{any}] {targetName}";      // using {any} to match any text before {targetName}
    string patternLocation = "[{any}] {locationName}";  // using {any} to match any text before {locationName}

    // Define acceptable values for the entities. By default an entity can have any value.
    // If these are not defined, patterns with {any} before the entity name will not work.
    const vector<string> targetNames = { "lamp", "lights", "radio", "TV" };
    model->Entities.push_back(
        { "targetName",
        Intent::EntityType::List,
        Intent::EntityMatchMode::Strict,
        targetNames
        });

    const vector<string> locationNames = { "living room", "bedroom", "kitchen" };
    model->Entities.push_back(
        { "locationName",
        Intent::EntityType::List,
        Intent::EntityMatchMode::Strict,
        locationNames
        });

    // Add intents (id, phrase(s)) to the model.
    const vector<pair<string, vector<string>>> intents =
    {
        {
            "HomeAutomation.TurnOn",
            {
                patternKeywordOptional + " " + patternTurnOn + " " + patternTarget,
                patternKeywordOptional + " " + patternTurnOn + " " + patternTarget + " " + patternLocation
            }
        },
        {
            "HomeAutomation.TurnOff",
            {
                patternKeywordOptional + " " + patternTurnOff + " " + patternTarget,
                patternKeywordOptional + " " + patternTurnOff + " " + patternTarget + " " + patternLocation
            }
        }
    };

    cout << "Added intents" << endl;
    for (const auto& intent : intents)
    {
        model->Intents.push_back({
            {
                intent.second
            },
            intent.first });
        cout << "  id=\"" << intent.first << "\"" << endl;
        for (const auto& phrase : intent.second)
        {
            cout << "    phrase=\"" << phrase << "\"" << endl;
        }
    }
    cout << "where\n";
    cout << "  targetName can be one of ";
    for (const auto& targetName : targetNames)
    {
        cout << "\"" << targetName << "\" ";
    }
    cout << endl;
    cout << "  locationName can be one of ";
    for (const auto& locationName : locationNames)
    {
        cout << "\"" << locationName << "\" ";
    }
    cout << endl << endl;

    return model;
}


void RecognizeIntent(bool useKeyword)
{
    // Creates an instance of an embedded speech config.
    auto speechConfig = CreateEmbeddedSpeechConfig();

    // Creates an intent recognizer using microphone as audio input.
    auto audioConfig = AudioConfig::FromDefaultMicrophoneInput();
    auto recognizer = IntentRecognizer::FromConfig(speechConfig, audioConfig);

    // Creates a pattern matching model.
    auto patternMatchingModel = CreatePatternMatchingModel();
    // Adds the model to a new language model collection.
    vector<shared_ptr<LanguageUnderstandingModel>> modelCollection;
    modelCollection.push_back(patternMatchingModel);
    // Applies the language model collection to the recognizer.
    // This replaces all previously applied models.
    recognizer->ApplyLanguageModels(modelCollection);

    // Subscribes to events.
    recognizer->Recognizing += [](const IntentRecognitionEventArgs& e)
    {
        // Intermediate result (hypothesis).
        if (e.Result->Reason == ResultReason::RecognizingSpeech)
        {
            cout << "Recognizing:" << e.Result->Text << endl;
        }
        else if (e.Result->Reason == ResultReason::RecognizingKeyword)
        {
            // ignored
        }
    };

    recognizer->Recognized += [](const IntentRecognitionEventArgs& e)
    {
        if (e.Result->Reason == ResultReason::RecognizedKeyword)
        {
            // Keyword detected, speech recognition will start.
            cout << "KEYWORD: Text=" << e.Result->Text << endl;
        }
        else if (e.Result->Reason == ResultReason::RecognizedIntent)
        {
            // Final result (intent recognized). May differ from the last intermediate result.
            cout << "RECOGNIZED: Text=" << e.Result->Text << endl;
            cout << "  Intent Id: " << e.Result->IntentId << endl;

            if (e.Result->IntentId.find("HomeAutomation") != string::npos)
            {
                auto entities = e.Result->GetEntities();
                if (entities.find("targetName") != entities.end())
                {
                    cout << "     Target: " << entities["targetName"].c_str() << endl;
                    if (entities.find("locationName") != entities.end())
                    {
                        cout << "   Location: " << entities["locationName"].c_str() << endl;
                    }
                }
            }
        }
        else if (e.Result->Reason == ResultReason::RecognizedSpeech)
        {
            // Final result (no intent recognized). May differ from the last intermediate result.
            cout << "RECOGNIZED: Text=" << e.Result->Text << endl;
            cout << "  (intent not recognized)\n";
        }
        else if (e.Result->Reason == ResultReason::NoMatch)
        {
            // NoMatch occurs when no speech was recognized.
            auto reason = NoMatchDetails::FromResult(e.Result)->Reason;
            cout << "NO MATCH: Reason=";
            switch (reason)
            {
            case NoMatchReason::NotRecognized:
                // Input audio was not silent but contained no recognizable speech.
                cout << "NotRecognized" << endl;
                break;
            case NoMatchReason::InitialSilenceTimeout:
                // Input audio was silent and the silence timeout expired.
                // In continuous recognition this can happen multiple times during
                // a session, not just at the very beginning.
                cout << "SilenceTimeout" << endl;
                break;
            default:
                // Other reasons are not supported in embedded speech at the moment.
                cout << int(reason) << endl;
                break;
            }
        }
    };

    recognizer->Canceled += [](const IntentRecognitionCanceledEventArgs& e)
    {
        switch (e.Reason)
        {
        case CancellationReason::EndOfStream:
            // Input stream was closed or the end of an input file was reached.
            cout << "CANCELED: EndOfStream" << endl;
            break;

        case CancellationReason::Error:
            // NOTE: In case of an error, do not use the same recognizer for recognition anymore.
            cerr << "CANCELED: ErrorCode=" << int(e.ErrorCode) << endl;
            cerr << "CANCELED: ErrorDetails=\"" << e.ErrorDetails << "\"" << endl;
            break;

        default:
            cout << "CANCELED: Reason=" << int(e.Reason) << endl;
            break;
        }
    };

    recognizer->SessionStarted += [](const SessionEventArgs& e)
    {
        UNUSED(e);
        cout << "Session started." << endl;
    };

    recognizer->SessionStopped += [](const SessionEventArgs& e)
    {
        UNUSED(e);
        cout << "Session stopped." << endl;
    };

    if (useKeyword)
    {
        // Creates an instance of a keyword recognition model.
        auto keywordModel = KeywordRecognitionModel::FromFile(GetKeywordModelFileName());

        // Starts the following routine:
        // 1. Listen for a keyword in input audio. There is no timeout.
        //    Speech that does not start with the keyword is ignored.
        // 2. If the keyword is spotted, start normal speech recognition.
        // 3. After a recognition result (that always includes at least
        //    the keyword), go back to step 1.
        // Steps 1-3 repeat until StopKeywordRecognitionAsync() is called.
        recognizer->StartKeywordRecognitionAsync(keywordModel).get();

        cout << "Say something starting with \"" << GetKeywordPhrase() << "\" (press Enter to stop)...\n";
        cin.get();

        // Stops recognition.
        recognizer->StopKeywordRecognitionAsync().get();
    }
    else
    {
        // Starts continuous recognition.
        recognizer->StartContinuousRecognitionAsync().get();

        cout << "Say something (press Enter to stop)...\n";
        cin.get();

        // Stops recognition.
        recognizer->StopContinuousRecognitionAsync().get();
    }
}


// Recognizes intents using embedded speech config and the system default microphone device.
void EmbeddedIntentRecognitionFromMicrophone()
{
    bool useKeyword = false;
    RecognizeIntent(useKeyword);
}


// Recognizes intents using embedded speech config and the system default microphone device.
// Recognition is triggered with a keyword.
void EmbeddedIntentRecognitionWithKeywordFromMicrophone()
{
    bool useKeyword = true;
    RecognizeIntent(useKeyword);
}
