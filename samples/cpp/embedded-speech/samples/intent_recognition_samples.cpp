//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//

#include <iostream>
#include <thread>
#include <speechapi_cxx.h>

using namespace Microsoft::CognitiveServices::Speech;
using namespace Microsoft::CognitiveServices::Speech::Audio;
using namespace Microsoft::CognitiveServices::Speech::Intent;

extern std::shared_ptr<EmbeddedSpeechConfig> CreateEmbeddedSpeechConfig();
extern const std::string GetKeywordModelFileName();
extern const std::string GetKeywordPhrase();


// Creates a pattern matching model with some example intents.
// For more examples on intent patterns, see
// https://github.com/Azure-Samples/cognitive-services-speech-sdk/blob/master/samples/cpp/windows/console/samples/intent_recognition_samples.cpp
std::shared_ptr<PatternMatchingModel> CreatePatternMatchingModel()
{
    // Create a pattern matching model. The id is necessary to identify this
    // model from others if there are several models in use at the same time.
    auto model = PatternMatchingModel::FromModelId("SomeUniqueIdForThisModel");

    // Make the keyword optional (surround with [ and ] in the pattern string)
    // in case the pattern matching model is used without keyword recognition.
    std::string patternKeywordOptional = "[" + GetKeywordPhrase() + "][{any}]";

    // Specify some intents to add. Example inputs:
    // - "Turn on the radio."
    // - "Switch off the lights in the bedroom."

    // Notes about patterns:
    // - surround entity names with { and }
    // - surround alternatives with ( and ) and separate with |
    // - surround optional parts with [ and ]
    // - default entity {any} matches any text
    std::string patternTurnOn = "(turn|switch) on";
    std::string patternTurnOff = "(turn|switch) off";
    std::string patternTarget = "[{any}] {targetName}";      // using {any} to match any text before {targetName}
    std::string patternLocation = "[{any}] {locationName}";  // using {any} to match any text before {locationName}

    // Define acceptable values for the entities. By default an entity can have any value.
    // If these are not defined, patterns with {any} before the entity name will not work.
    const std::vector<std::string> targetNames = { "lamp", "lights", "radio", "TV" };
    model->Entities.push_back(
        { "targetName",
        Intent::EntityType::List,
        Intent::EntityMatchMode::Strict,
        targetNames
        });

    const std::vector<std::string> locationNames = { "living room", "bedroom", "kitchen" };
    model->Entities.push_back(
        { "locationName",
        Intent::EntityType::List,
        Intent::EntityMatchMode::Strict,
        locationNames
        });

    // Add intents (id, phrase(s)) to the model.
    const std::vector<std::pair<std::string, std::vector<std::string>>> intents =
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

    std::cout << "Added intents" << std::endl;
    for (const auto& intent : intents)
    {
        model->Intents.push_back({
            {
                intent.second
            },
            intent.first });
        std::cout << "  id=\"" << intent.first << "\"" << std::endl;
        for (const auto& phrase : intent.second)
        {
            std::cout << "    phrase=\"" << phrase << "\"" << std::endl;
        }
    }
    std::cout << "where" << std::endl;
    std::cout << "  targetName can be one of ";
    for (const auto& targetName : targetNames)
    {
        std::cout << "\"" << targetName << "\" ";
    }
    std::cout << std::endl;
    std::cout << "  locationName can be one of ";
    for (const auto& locationName : locationNames)
    {
        std::cout << "\"" << locationName << "\" ";
    }
    std::cout << std::endl << std::endl;

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
    std::vector<std::shared_ptr<LanguageUnderstandingModel>> modelCollection;
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
            std::cout << "Recognizing:" << e.Result->Text << std::endl;
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
            std::cout << "KEYWORD: Text=" << e.Result->Text << std::endl;
        }
        else if (e.Result->Reason == ResultReason::RecognizedIntent)
        {
            // Final result (intent recognized). May differ from the last intermediate result.
            std::cout << "RECOGNIZED: Text=" << e.Result->Text << std::endl;
            std::cout << "  Intent Id: " << e.Result->IntentId << std::endl;

            if (e.Result->IntentId.find("HomeAutomation") != std::string::npos)
            {
                auto entities = e.Result->GetEntities();
                if (entities.find("targetName") != entities.end())
                {
                    std::cout << "     Target: " << entities["targetName"].c_str() << std::endl;
                    if (entities.find("locationName") != entities.end())
                    {
                        std::cout << "   Location: " << entities["locationName"].c_str() << std::endl;
                    }
                }
            }
        }
        else if (e.Result->Reason == ResultReason::RecognizedSpeech)
        {
            // Final result (no intent recognized). May differ from the last intermediate result.
            std::cout << "RECOGNIZED: Text=" << e.Result->Text << std::endl;
            std::cout << "  (intent not recognized)" << std::endl;
        }
        else if (e.Result->Reason == ResultReason::NoMatch)
        {
            // NoMatch occurs when no speech was recognized.
            auto reason = NoMatchDetails::FromResult(e.Result)->Reason;
            std::cout << "NO MATCH: Reason=";
            switch (reason)
            {
            case NoMatchReason::NotRecognized:
                // Input audio was not silent but contained no recognizable speech.
                std::cout << "NotRecognized" << std::endl;
                break;
            case NoMatchReason::InitialSilenceTimeout:
                // Input audio was silent and the silence timeout expired.
                // In continuous recognition this can happen multiple times during
                // a session, not just at the very beginning.
                std::cout << "SilenceTimeout" << std::endl;
                break;
            default:
                // Other reasons are not supported in embedded speech at the moment.
                std::cout << int(reason) << std::endl;
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
            std::cout << "CANCELED: EndOfStream" << std::endl;
            break;

        case CancellationReason::Error:
            // NOTE: In case of an error, do not use the same recognizer for recognition anymore.
            std::cerr << "CANCELED: ErrorCode=" << int(e.ErrorCode) << std::endl;
            std::cerr << "CANCELED: ErrorDetails=\"" << e.ErrorDetails << "\"" << std::endl;
            break;

        default:
            std::cout << "CANCELED: Reason=" << int(e.Reason) << std::endl;
            break;
        }
    };

    recognizer->SessionStarted += [](const SessionEventArgs& e)
    {
        UNUSED(e);
        std::cout << "Session started." << std::endl;
    };

    recognizer->SessionStopped += [](const SessionEventArgs& e)
    {
        UNUSED(e);
        std::cout << "Session stopped." << std::endl;
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

        std::cout << "Say something starting with \"" << GetKeywordPhrase() << "\" (press Enter to stop)..." << std::endl;
        std::cin.get();

        // Stops recognition.
        recognizer->StopKeywordRecognitionAsync().get();
    }
    else
    {
        // Starts continuous recognition.
        recognizer->StartContinuousRecognitionAsync().get();

        std::cout << "Say something (press Enter to stop)..." << std::endl;
        std::cin.get();

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
