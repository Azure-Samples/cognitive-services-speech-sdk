//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include <map>
#include "test_utils.h"
#include "file_utils.h"
#include "recognizer_utils.h"
#include "property_id_2_name_map.h"

std::shared_ptr<SpeechTranslationConfig> CreateTranslationConfigWithLanguageId(
    const string& mode,
    const string& from,
    const vector<string>& to,
    string defaultLanguage = "",
    const vector<string>& voiceNames = {})
{
    if (defaultLanguage.empty())
    {
        defaultLanguage = PAL::split(from, CommaDelim)[0];
    }
    
    auto endPoint = "wss://" + Config::Region + ".sr.speech.microsoft.com/speech/translation/" + mode + "/mstranslator/v1?TrafficType=Test&language=" + defaultLanguage;
    auto config = SpeechTranslationConfig::FromEndpoint(endPoint, Keys::Speech);
    config->SetProperty(PropertyId::SpeechServiceConnection_AutoDetectSourceLanguages, from);
    for (auto item : to)
    {
        config->AddTargetLanguage(item);
    }
    if (voiceNames.size() > 0)
    {
        for (size_t i = 0; i < voiceNames.size(); i++)
        {
            auto propertyName = to[i] + GetPropertyName(PropertyId::SpeechServiceConnection_TranslationVoice); 
            config->SetProperty(propertyName, voiceNames[i]);
        }
    }

    return config;
}

std::shared_ptr<SpeechTranslationConfig> CreateTranslationConfig(
    const string& from,
    const vector<string>& to)
{
     auto config = CurrentTranslationConfig();
     config->SetSpeechRecognitionLanguage(from);
     for (auto item : to)
     {
         config->AddTargetLanguage(item);
     }
     return config;
}

static shared_ptr<TranslationRecognizer> CreateTranslationRecognizer(
    const string& filename,
    const string& from,
    const vector<string>& to)
{
    auto audioInput = AudioConfig::FromWavFileInput(filename);
    auto config = CreateTranslationConfig(from, to);
    return TranslationRecognizer::FromConfig(config, audioInput);
}

static shared_ptr<TranslationRecognizer> CreateTranslationRecognizer(
    const string& filename,
    std::shared_ptr<SpeechTranslationConfig> config)
{
    auto audioInput = AudioConfig::FromWavFileInput(filename);
    return TranslationRecognizer::FromConfig(config, audioInput);
}

TEST_CASE("Translation", "[api][cxx]")
{
    SPX_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    UseMocks(false);
    weather.UpdateFullFilename(Config::InputDir);
    REQUIRE(exists(weather.m_inputDataFilename));

    batman.UpdateFullFilename(Config::InputDir);
    SPXTEST_REQUIRE(exists(batman.m_inputDataFilename));

    SPXTEST_SECTION("TranslationContinuous")
    {
        promise<void> complete;
        shared_future<void> readyFuture(complete.get_future());
        vector<string> recognizingResults;
        vector<string> translatingResults;
        vector<string> recognizedResults;
        vector<string> translatedResults;
        vector<string> errorResults;
        
        auto recognizer = CreateTranslationRecognizer(weather.m_inputDataFilename, "en-US", { "de" });

        recognizer->Recognizing.DisconnectAll();
        recognizer->Recognizing.Connect([&recognizingResults, &translatingResults](const TranslationRecognitionEventArgs& e)
            {
                if (e.Result->Reason == ResultReason::TranslatingSpeech)
                {
                    recognizingResults.push_back(e.Result->Text);
                    translatingResults.push_back(e.Result->Translations.at("de"));
                }
            });

        recognizer->Recognized.DisconnectAll();
        recognizer->Recognized.Connect([&recognizedResults, &translatedResults, &errorResults, &complete, &readyFuture](const TranslationRecognitionEventArgs& e)
            {
                if (e.Result->Reason == ResultReason::TranslatedSpeech)
                {
                    recognizedResults.push_back(e.Result->Text);
                    translatedResults.push_back(e.Result->Translations.at("de"));
                }
                else
                {
                    if (e.Result->Reason == ResultReason::RecognizedSpeech)
                    {
                        SPX_TRACE_VERBOSE("cxx_api_Test Only speech is recognized, but no translation.");
                        errorResults.push_back("RecognizedSpeech Only");
                    }
                    else
                    {
                        SPX_TRACE_VERBOSE("cxx_api_Test Recognized failed: Reason:%d. json: %s", e.Result->Reason, e.Result->Properties.GetProperty(PropertyId::SpeechServiceResponse_JsonResult).c_str());
                        errorResults.push_back("Recognized error.");
                    }
                    if (readyFuture.wait_for(std::chrono::seconds(0)) != std::future_status::ready)
                    {
                        complete.set_value();
                    }
                }
            });
        recognizer->Canceled.DisconnectAll();
        recognizer->Canceled.Connect([&errorResults, &complete, &readyFuture](const TranslationRecognitionCanceledEventArgs& e)
            {
                if (e.Reason == CancellationReason::EndOfStream)
                {
                    SPX_TRACE_VERBOSE("CXX_API_TEST CANCELED: Reach the end of the file.");
                }
                else
                {
                    auto error = !e.ErrorDetails.empty() ? e.ErrorDetails : "Errors!";
                    SPX_TRACE_VERBOSE("CXX_API_TEST CANCELED: ErrorCode=%d, ErrorDetails=%s", (int)e.ErrorCode, e.ErrorDetails.c_str());
                    errorResults.push_back(error);
                    if (readyFuture.wait_for(std::chrono::seconds(0)) != std::future_status::ready)
                    {
                        complete.set_value();
                    }
                }
            });
        
        recognizer->SessionStopped.DisconnectAll();
        recognizer->SessionStopped.Connect([&complete, &readyFuture](const SessionEventArgs& e)
            {
                SPX_TRACE_VERBOSE("CXX_API_TEST SessionStopped: session id %s", e.SessionId.c_str());
                if (readyFuture.wait_for(std::chrono::seconds(0)) != std::future_status::ready)
                {
                    complete.set_value();
                }
            });

        recognizer->StartContinuousRecognitionAsync().get();
        auto status = readyFuture.wait_for(WAIT_FOR_RECO_RESULT_TIME);
        REQUIRE(status == future_status::ready);
        recognizer->StopContinuousRecognitionAsync().get();

        REQUIRE(errorResults.size() == 0);
        REQUIRE(recognizingResults.size() > 0);
        REQUIRE(translatingResults.size() == recognizingResults.size());
        REQUIRE(recognizedResults.size() > 0);
        REQUIRE(translatedResults.size() == recognizedResults.size());
        REQUIRE(recognizedResults[0] == weather.m_utterance);
        REQUIRE(translatedResults[0] == weatherGerman.m_utterance);
    }

    SPXTEST_SECTION("TranslationOnce")
    {
        string expectedLanguageDetected;
        string expectedTargetLanguage;
        shared_ptr<TranslationRecognizer> recognizer;

        SPXTEST_SECTION("Without Language Id")
        {
            recognizer = CreateTranslationRecognizer(weather.m_inputDataFilename, "en-US", { "de" });
            expectedTargetLanguage = "de";
        }

        SPXTEST_SECTION("With Language Id")
        {
            auto config = CreateTranslationConfigWithLanguageId(
                "interactive",
                "en-US,de-DE",
                { "en-US" , "de-DE" });
            recognizer = CreateTranslationRecognizer(weather.m_inputDataFilename, config);
            expectedLanguageDetected = "en-US";
            expectedTargetLanguage = "de-DE";
        }

        auto result = recognizer->RecognizeOnceAsync().get();
        REQUIRE(result != nullptr);
        REQUIRE(result->Reason == ResultReason::TranslatedSpeech);
        REQUIRE(!result->Text.compare(weather.m_utterance));
        REQUIRE(!result->Translations.at(expectedTargetLanguage).compare(weatherGerman.m_utterance));
        REQUIRE(result->Properties.GetProperty(PropertyId::SpeechServiceConnection_AutoDetectSourceLanguageResult) == expectedLanguageDetected);
    }

    SPXTEST_SECTION("Change languages in config, before turn and after turn.")
    {
        batman.UpdateFullFilename(Config::InputDir);
        SPXTEST_REQUIRE(exists(batman.m_inputDataFilename));
        auto config = SpeechTranslationConfig::FromSubscription(Keys::Speech, Config::Region);

        // Change languages in config
        config->SetSpeechRecognitionLanguage("en-US");
        config->AddTargetLanguage("de");
        auto targetLangs = config->GetProperty(PropertyId::SpeechServiceConnection_TranslationToLanguages);
        REQUIRE(!targetLangs.compare("de"));

        config->AddTargetLanguage("fr");
        targetLangs = config->GetProperty(PropertyId::SpeechServiceConnection_TranslationToLanguages);
        REQUIRE(!targetLangs.compare("de,fr"));

        config->RemoveTargetLanguage("de");
        targetLangs = config->GetProperty(PropertyId::SpeechServiceConnection_TranslationToLanguages);
        REQUIRE(!targetLangs.compare("fr"));

        config->AddTargetLanguage("de");
        targetLangs = config->GetProperty(PropertyId::SpeechServiceConnection_TranslationToLanguages);
        REQUIRE(!targetLangs.compare("fr,de"));

        auto recognizer = TranslationRecognizer::FromConfig(config, AudioConfig::FromWavFileInput(batman.m_inputDataFilename));

        // Change languages before turn.
        recognizer->AddTargetLanguage("es");
        targetLangs = recognizer->Properties.GetProperty(PropertyId::SpeechServiceConnection_TranslationToLanguages);
        REQUIRE(!targetLangs.compare("fr,de,es"));

        auto result = recognizer->RecognizeOnceAsync().get();
        REQUIRE(result != nullptr);
        REQUIRE(result->Reason == ResultReason::TranslatedSpeech);
        REQUIRE(result->Text.length() > 0);
        REQUIRE(result->Translations.at("de").length() > 0);
        REQUIRE(result->Translations.at("fr").length() > 0);
        REQUIRE(result->Translations.at("es").length() > 0);

        // Change languages after turn.
        recognizer->RemoveTargetLanguage("es");
        targetLangs = recognizer->Properties.GetProperty(PropertyId::SpeechServiceConnection_TranslationToLanguages);
        REQUIRE(!targetLangs.compare("fr,de"));

        result = recognizer->RecognizeOnceAsync().get();
        REQUIRE(result != nullptr);
        REQUIRE(result->Reason == ResultReason::TranslatedSpeech);
        REQUIRE(result->Text.length() > 0);
        REQUIRE(result->Translations.at("de").length() > 0);
        REQUIRE(result->Translations.at("fr").length() > 0);
        REQUIRE(result->Translations.find("es") == result->Translations.end());

        recognizer->RemoveTargetLanguage("de");
        recognizer->RemoveTargetLanguage("es");
        targetLangs = recognizer->Properties.GetProperty(PropertyId::SpeechServiceConnection_TranslationToLanguages);
        REQUIRE(!targetLangs.compare("fr"));

        recognizer->AddTargetLanguage("pt");
        targetLangs = recognizer->Properties.GetProperty(PropertyId::SpeechServiceConnection_TranslationToLanguages);
        REQUIRE(!targetLangs.compare("fr,pt"));

        result = recognizer->RecognizeOnceAsync().get();
        REQUIRE(result != nullptr);
        REQUIRE(result->Reason == ResultReason::TranslatedSpeech);
        REQUIRE(result->Text.length() > 0);
        REQUIRE(result->Translations.at("pt").length() > 0);
        REQUIRE(result->Translations.at("fr").length() > 0);
        REQUIRE(result->Translations.find("es") == result->Translations.end());
        REQUIRE(result->Translations.find("de") == result->Translations.end());
    }

    SPXTEST_SECTION("Change languages in turn.")
    {
        auto config = SpeechTranslationConfig::FromSubscription(Keys::Speech, Config::Region);

        // Change languages in config
        config->SetSpeechRecognitionLanguage("en-US");

        promise<void> complete;
        shared_future<void> readyFuture(complete.get_future());
        vector<string> recognizedResults;
        vector<string> translatedResultsDe;
        vector<string> translatedResultsEs;
        vector<string> translatedResultsFr;
        vector<string> errorResults;

        auto recognizer = TranslationRecognizer::FromConfig(config, AudioConfig::FromWavFileInput(batman.m_inputDataFilename));
        recognizer->AddTargetLanguage("de");

        recognizer->Recognized.DisconnectAll();
        recognizer->Recognized.Connect([&recognizer, &recognizedResults, &translatedResultsDe, &translatedResultsEs, &translatedResultsFr, &errorResults, &complete, &readyFuture](const TranslationRecognitionEventArgs& e)
        {
            if (e.Result->Reason == ResultReason::TranslatedSpeech)
            {
                recognizedResults.push_back(e.Result->Text);
                translatedResultsDe.push_back(e.Result->Translations.at("de"));
                if (translatedResultsDe.size() == 1)
                {
                    recognizer->AddTargetLanguage("es");
                }

                if (e.Result->Translations.find("es") != e.Result->Translations.end())
                {
                    translatedResultsEs.push_back(e.Result->Translations.at("es"));
                    if (translatedResultsEs.size() == 1)
                    {
                        recognizer->AddTargetLanguage("fr");
                    }
                }

                if (e.Result->Translations.find("fr") != e.Result->Translations.end())
                {
                    translatedResultsFr.push_back(e.Result->Translations.at("fr"));
                }
            }
            else
            {
                if (e.Result->Reason == ResultReason::RecognizedSpeech)
                {
                    SPX_TRACE_VERBOSE("cxx_api_Test Only speech is recognized, but no translation.");
                    errorResults.push_back("RecognizedSpeech Only");
                }
                else
                {
                    SPX_TRACE_VERBOSE("cxx_api_Test Recognized failed: Reason:%d.", e.Result->Reason);
                    errorResults.push_back("Recognized error.");
                }
                if (readyFuture.wait_for(std::chrono::seconds(0)) != std::future_status::ready)
                {
                    complete.set_value();
                }
            }
        });
        recognizer->Canceled.DisconnectAll();
        recognizer->Canceled.Connect([&errorResults, &complete, &readyFuture](const TranslationRecognitionCanceledEventArgs& e)
        {
            if (e.Reason == CancellationReason::EndOfStream)
            {
                SPX_TRACE_VERBOSE("CXX_API_TEST CANCELED: Reach the end of the file.");
            }
            else
            {
                auto error = !e.ErrorDetails.empty() ? e.ErrorDetails : "Errors!";
                SPX_TRACE_VERBOSE("CXX_API_TEST CANCELED: ErrorCode=%d, ErrorDetails=%s", (int)e.ErrorCode, e.ErrorDetails.c_str());
                errorResults.push_back(error);
                if (readyFuture.wait_for(std::chrono::seconds(0)) != std::future_status::ready)
                {
                    complete.set_value();
                }
            }
        });
        recognizer->SessionStopped.DisconnectAll();
        recognizer->SessionStopped.Connect([&complete, &readyFuture](const SessionEventArgs& e)
        {
            SPX_TRACE_VERBOSE("CXX_API_TEST SessionStopped: session id %s", e.SessionId.c_str());
            if (readyFuture.wait_for(std::chrono::seconds(0)) != std::future_status::ready)
            {
                complete.set_value();
            }
        });

        recognizer->StartContinuousRecognitionAsync().get();
        auto status = readyFuture.wait_for(300s);
        REQUIRE(status == future_status::ready);
        recognizer->StopContinuousRecognitionAsync().get();
        REQUIRE(recognizedResults.size() > 0);
        REQUIRE(translatedResultsFr.size() > 0);
        REQUIRE(translatedResultsEs.size() > translatedResultsFr.size());
        REQUIRE(translatedResultsDe.size() > translatedResultsEs.size());
        REQUIRE(errorResults.size() == 0);
    }
}

TEST_CASE("Custom translation endpoints", "[api][cxx]")
{
    SPX_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    callTheFirstOne.UpdateFullFilename(Config::InputDir);
    SPXTEST_REQUIRE(exists(callTheFirstOne.m_inputDataFilename));
    auto audioInput = AudioConfig::FromWavFileInput(callTheFirstOne.m_inputDataFilename);
    string speechEndpoint = "wss://" + Config::Region + ".s2s.speech.microsoft.com";

    SPXTEST_SECTION("Host only")
    {
        auto host = speechEndpoint;
        auto config = SpeechTranslationConfig::FromHost(host, Keys::Speech);
        config->SetServiceProperty("language", "de-DE", ServicePropertyChannel::UriQueryParameter);
        config->SetServiceProperty("from", "de-DE", ServicePropertyChannel::UriQueryParameter);
        config->SetServiceProperty("to", "en", ServicePropertyChannel::UriQueryParameter);
        auto recognizer = TranslationRecognizer::FromConfig(config, audioInput);

        auto result = recognizer->RecognizeOnceAsync().get();
        SPXTEST_REQUIRE(result != nullptr);
        SPXTEST_REQUIRE(result->Reason == ResultReason::TranslatedSpeech);
    }

    SPXTEST_SECTION("Host with parameters") // not allowed
    {
        auto host = speechEndpoint + "?from=de-DE&to=en";
        auto config = SpeechTranslationConfig::FromHost(host, Keys::Speech);
        config->SetServiceProperty("language", "de-DE", ServicePropertyChannel::UriQueryParameter);
        auto recognizer = TranslationRecognizer::FromConfig(config, audioInput);

        auto result = recognizer->RecognizeOnceAsync().get();
        SPXTEST_REQUIRE(result != nullptr);
        SPXTEST_REQUIRE(result->Reason == ResultReason::Canceled);
    }
}
