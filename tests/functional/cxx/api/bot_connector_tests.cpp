//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include <iostream>
#include <atomic>
#include <functional>
#include <string>
#include <sstream>
#include <mutex>
#include <chrono>
#include <future>
#include <tuple>

#include "test_utils.h"
#include "file_utils.h"
#include "recognizer_utils.h"

#include "speechapi_cxx.h"

using namespace Microsoft::CognitiveServices::Speech;
using namespace Microsoft::CognitiveServices::Speech::Audio;
using namespace Microsoft::CognitiveServices::Speech::Dialog;

using namespace std::chrono_literals;

#define TEST_KWS_KEYWORD "Computer"
#define TEST_ACTIVITY_TYPE "message"


std::shared_ptr<BotConnectorConfig> BotConfigForTests()
{
    auto config = BotConnectorConfig::FromSecretKey("test", Keys::Bot, Config::BotRegion);
    config->SetSpeechRecognitionLanguage("en-us");
    config->SetProperty("Conversation_Communication_Type", "AutoReply");
    return config;
}

struct test_runner
{
    struct event_status
    {
        bool ok;
        int fired;
        int expected;
    };

    template<typename T>
    using test_function = std::function<bool(std::ostringstream&, const T&, int)>;

    template<typename T>
    using filter_function = std::function<bool(const T&)>;

    const filter_function<SpeechRecognitionEventArgs> isRecognizedSpeech = [](const SpeechRecognitionEventArgs& e)
    {
        return e.Result->Reason == ResultReason::RecognizedSpeech;
    };

    const filter_function<SpeechRecognitionEventArgs> isRecognizingSpeech = [](const SpeechRecognitionEventArgs& e)
    {
        return e.Result->Reason == ResultReason::RecognizingSpeech;
    };

    const filter_function<SpeechRecognitionEventArgs> isRecognizingKeyword = [](const SpeechRecognitionEventArgs& e)
    {
        return e.Result->Reason == ResultReason::RecognizingKeyword;
    };

    const filter_function<SpeechRecognitionEventArgs> isRecognizedKeyword = [](const SpeechRecognitionEventArgs& e)
    {
        return e.Result->Reason == ResultReason::RecognizedKeyword;
    };

    const filter_function<SpeechRecognitionEventArgs> isNotRecognizedKeyword = [](const SpeechRecognitionEventArgs& e)
    {
        return e.Result->Reason == ResultReason::NoMatch && NoMatchDetails::FromResult(e.Result)->Reason == NoMatchReason::KeywordNotRecognized;
    };

    const filter_function<ActivityReceivedEventArgs> isActivityReceived = [](const ActivityReceivedEventArgs&)
    {
        return true;
    };

    const filter_function<SpeechRecognitionCanceledEventArgs> isCanceledSpeech = [](const SpeechRecognitionCanceledEventArgs&)
    {
        return true;
    };

    test_runner(std::shared_ptr<SpeechBotConnector> connector, std::shared_ptr<KeywordRecognitionModel> model = nullptr): m_connector{connector}, m_model{model}
    {};

    template<typename T>
    void wire_event(EventSignal<const T&>& event, event_status& status, test_function<T> test_fn, filter_function<T> filter_fn, int expected)
    {
        event += [this, &status, test_fn, filter_fn](const T& e)
        {
            if (filter_fn(e)) {
                status.ok = test_fn(m_message, e, status.fired);
                status.fired++;
                m_cond.notify_one();
            }
        };
        status.expected = expected;
        status.ok = status.expected == 0;
    }

    void add_keyword_recognizing_test(test_function<SpeechRecognitionEventArgs> test, int expected = 1)
    {
        wire_event(m_connector->Recognizing, m_keyword_recognizing, test, isRecognizingKeyword, expected);
    }

    void add_keyword_recognized_test(test_function<SpeechRecognitionEventArgs> test, int expected = 1)
    {
        wire_event(m_connector->Recognized, m_keyword_recognized, test, isRecognizedKeyword, expected);
    }

    void add_keyword_not_recognized_test(test_function<SpeechRecognitionEventArgs> test, int expected = 0)
    {
        wire_event(m_connector->Recognized, m_keyword_not_recognized, test, isNotRecognizedKeyword, expected);
    }

    void add_recognized_test(test_function<SpeechRecognitionEventArgs> test, int expected = 1)
    {
        wire_event(m_connector->Recognized, m_recognized, test, isRecognizedSpeech, expected);
    }

    void add_recognizing_test(test_function<SpeechRecognitionEventArgs> test, int expected = 1)
    {
        wire_event(m_connector->Recognizing, m_recognizing, test, isRecognizingSpeech, expected);
    }

    void add_activity_received_test(test_function<ActivityReceivedEventArgs> test, int expected = 1)
    {
        wire_event(m_connector->ActivityReceived, m_activity_received, test, isActivityReceived, expected);
    }

    void add_canceled_test(test_function<SpeechRecognitionCanceledEventArgs> test, int expected = 0)
    {
        wire_event(m_connector->Canceled, m_canceled, test, isCanceledSpeech, expected);
    }

    static bool ready(event_status e)
    {
        return e.expected == 0 || e.expected <= e.fired;
    }

    template<typename T>
    std::tuple<bool, std::string> run(std::function<std::future<T>(SpeechBotConnector&, std::shared_ptr<KeywordRecognitionModel>)> scenario, std::chrono::seconds timeout)
    {
        auto task = scenario(*m_connector, m_model);
        std::mutex m;
        std::unique_lock<std::mutex> lk{ m };
        auto res = m_cond.wait_for(lk, timeout, [&]()
        {
            if ((m_canceled.fired && !m_canceled.expected ) || (m_keyword_not_recognized.fired && !m_keyword_not_recognized.expected))
            {
                /* Unexpected cancel */
                return true;
            }
            if (ready(m_canceled) && ready(m_recognized) && ready(m_recognizing) && ready(m_activity_received) && ready(m_keyword_not_recognized) && ready(m_keyword_recognized) && ready(m_keyword_recognizing))
            {
                return true;
            }
            return false;
        });
        task.wait();
        if (!res)
        {
            return std::make_tuple<bool, std::string>(false, "Test timed out");
        }

        if (m_canceled.ok
            && m_recognized.ok
            && m_recognizing.ok
            && m_activity_received.ok
            && m_keyword_not_recognized.ok
            && m_keyword_recognized.ok
            && m_keyword_recognizing.ok)
        {
            return std::make_tuple<bool, std::string>(true, "Success");
        }
        return std::make_tuple<bool, std::string>(false, m_message.str());

    }

private:
    std::condition_variable m_cond;
    std::shared_ptr<SpeechBotConnector> m_connector;
    std::shared_ptr<KeywordRecognitionModel> m_model;
    std::ostringstream m_message{};
    event_status m_keyword_recognized{ true, 0, 0 };
    event_status m_keyword_recognizing{ true, 0, 0 };
    event_status m_keyword_not_recognized{ true, 0, 0 };
    event_status m_recognized{ true, 0, 0 };
    event_status m_recognizing{ true, 0, 0 };
    event_status m_activity_received{ true, 0, 0 };
    event_status m_canceled{ true, 0, 0 };
};

// Default verification lambdas
const auto verifyRecognizedSpeech = [](std::ostringstream& oss, const SpeechRecognitionEventArgs& e, int fired)
{
    // We should receive only one of these
    if (fired > 1)
    {
        oss << "Received more than one Recognized event" << std::endl;
        return false;
    }
    return !e.Result->Text.empty();
};

const auto verifyRecognizingSpeech = [](std::ostringstream&, const SpeechRecognitionEventArgs& e, int)
{
    // We should receive at least one of these
    return !e.Result->Text.empty();
};

const auto verifyActivityReceived = [](std::ostringstream&, const ActivityReceivedEventArgs& e, int)
{
    auto activity = e.GetActivity();
    return activity->Type == TEST_ACTIVITY_TYPE;
};

const auto verifyCanceledSpeech = [](std::ostringstream& oss, const SpeechRecognitionCanceledEventArgs& e, int)
{
    oss << "Received a cancel event " << e.ErrorDetails << std::endl;
    return false;
};

const auto verifyRecognizingKeyword = [](std::ostringstream& oss, const SpeechRecognitionEventArgs& e, int fired)
{
    // We should receive only one of these
    if (fired > 1)
    {
        oss << "Received more than one Keyword Recognizing event" << std::endl;
        return false;
    }
    return e.Result->Text == TEST_KWS_KEYWORD;
};

const auto verifyRecognizedKeyword = [](std::ostringstream& oss, const SpeechRecognitionEventArgs& e, int fired)
{
    // We should receive only one of these
    if (fired > 1)
    {
        oss << "Received more than one Keyword Recognized event" << std::endl;
        return false;
    }
    return e.Result->Text == TEST_KWS_KEYWORD;
};

const auto verifyNotRecognizedKeyword = [](std::ostringstream& oss, const SpeechRecognitionEventArgs&, int)
{
    oss << "Received a Keyword Not Recognized event " << std::endl;
    return false;
};

TEST_CASE("Speech Bot Connector basics", "[api][cxx][bot_connector][!hide]")
{
    SECTION("Listen Once works")
    {
        turnOnLamp.UpdateFullFilename(Config::InputDir);
        REQUIRE(exists(turnOnLamp.m_inputDataFilename));
        auto config = BotConfigForTests();
        auto audioConfig = AudioConfig::FromWavFileInput(turnOnLamp.m_inputDataFilename);

        test_runner runner{ SpeechBotConnector::FromConfig(config, audioConfig) };

        runner.add_recognized_test(verifyRecognizedSpeech);
        runner.add_recognizing_test(verifyRecognizingSpeech);
        runner.add_activity_received_test(verifyActivityReceived);
        runner.add_canceled_test(verifyCanceledSpeech);

        auto result = runner.run<void>(
            [](SpeechBotConnector& connector, std::shared_ptr<KeywordRecognitionModel>)
            {
                return connector.ListenOnceAsync();
            },
            20s);

        bool success = std::get<0>(result);
        if (!success)
        {
            std::string message = std::move(std::get<1>(result));
            FAIL(message);
        }
    }

    SECTION("Send/Receive activity works (w/TTS)")
    {
        turnOnLamp.UpdateFullFilename(Config::InputDir);
        REQUIRE(exists(turnOnLamp.m_inputDataFilename));
        auto config = BotConfigForTests();
        auto audioConfig = AudioConfig::FromWavFileInput(turnOnLamp.m_inputDataFilename);

        test_runner runner{ SpeechBotConnector::FromConfig(config, audioConfig) };
;
        constexpr auto activity_text = "message";
        constexpr auto activity_speak = "Hello world";

        runner.add_activity_received_test(
            [&](std::ostringstream& oss, const ActivityReceivedEventArgs& e, int)
            {
                auto activity = e.GetActivity();
                bool success{ true };
                if (activity->Type != TEST_ACTIVITY_TYPE)
                {
                    oss << "activity->Type -> " << activity->Type << " != " << TEST_ACTIVITY_TYPE << std::endl;
                    success = false;
                }
                if (activity->Text != activity_text)
                {
                    oss << "activity->Text -> " << activity->Text << " != " << activity_text << std::endl;
                    success = false;
                }
                if (activity->Speak != activity_speak)
                {
                    oss << "activity->Speak -> " << activity->Speak << " != " << activity_speak << std::endl;
                    success = false;
                }

#if 0
                // Disabling as service is not returning audio consistently. bug #1758540
                if (e.HasAudio())
                {
                    auto audio = e.GetAudio();
                    auto buffer = std::make_unique<uint8_t[]>(800);
                    size_t total = 0;
                    while (auto count = audio->Read(buffer.get(), 800))
                    {
                        total += count;
                    }
                    if (total == 0)
                    {
                        oss << "Received empty stream" << std::endl;
                        success = false;
                    }
                }
                else
                {
                    oss << "No audio was received" << std::endl;
                    success = false;
                }
#endif

                return success;
        });

        runner.add_canceled_test(verifyCanceledSpeech);

        auto result = runner.run<std::string>(
            [&](SpeechBotConnector& connector, std::shared_ptr<KeywordRecognitionModel>)
            {
                auto activity = BotConnectorActivity::Create();
                activity->Type = TEST_ACTIVITY_TYPE;
                activity->Text = activity_text;
                activity->Speak = activity_speak;
                return connector.SendActivityAsync(activity);
            },
            20s);

        bool success = std::get<0>(result);
        if (!success)
        {
            std::string message = std::move(std::get<1>(result));
            FAIL(message);
        }

    }
}

#if defined(SPEECHSDK_KWS_ENABLED)

// The following tests currently do only work with a real KWS engine (not the
// KWS mock). Consider extending the mock:
// 1) Being able to specify a custom number of keyword detection
// 2) Being able to specify the audio that will be streamed to the service for keyword verification.
// 3) Being able to specify the keyword text in keyword detection (Done)
// 4) Being able to specify the audio for the latter utterance 

TEST_CASE("Speech Bot Connector KWS basics", "[api][cxx][bot_connector][!hide]") {

    SECTION("Listen once with KWS only works")
    {
        UseMocks(false);
        kwvAccept.UpdateFullFilename(Config::InputDir);
        REQUIRE(exists(kwvAccept.m_inputDataFilename));
        auto config = BotConfigForTests();
        // Turn off keyword verification
        config->SetProperty("KeywordConfig_EnableKeywordVerification", "false");
        auto audioConfig = AudioConfig::FromWavFileInput(kwvAccept.m_inputDataFilename);
        auto model = KeywordRecognitionModel::FromFile(Config::InputDir + "/kws/Computer/kws.table");

        test_runner runner{ SpeechBotConnector::FromConfig(config, audioConfig), model };

        runner.add_keyword_recognizing_test(verifyRecognizingKeyword);
        runner.add_keyword_recognized_test(verifyRecognizedKeyword);
        runner.add_keyword_not_recognized_test(verifyNotRecognizedKeyword);
        runner.add_recognized_test(verifyRecognizedSpeech);
        runner.add_recognizing_test(verifyRecognizingSpeech);
        runner.add_activity_received_test(verifyActivityReceived);
        runner.add_canceled_test(verifyCanceledSpeech);

        auto result = runner.run<void>(
            [](SpeechBotConnector& connector, shared_ptr<KeywordRecognitionModel> model)
            {
                return connector.StartKeywordRecognitionAsync(model);
            },
            60s);

        bool success = std::get<0>(result);
        if (!success)
        {
            std::string message = std::move(std::get<1>(result));
            FAIL(message);
        }
    }
}

TEST_CASE("Speech Bot Connector KWV basics", "[api][cxx][bot_connector][!hide]")
{

    SECTION("Listen once with KWS + KWV accept works")
    {
        UseMocks(false);
        kwvAccept.UpdateFullFilename(Config::InputDir);
        REQUIRE(exists(kwvAccept.m_inputDataFilename));
        auto config = BotConfigForTests();
        auto audioConfig = AudioConfig::FromWavFileInput(kwvAccept.m_inputDataFilename);
        auto model = KeywordRecognitionModel::FromFile(Config::InputDir + "/kws/Computer/kws.table");

        test_runner runner{ SpeechBotConnector::FromConfig(config, audioConfig), model};

        runner.add_keyword_recognizing_test(verifyRecognizingKeyword);
        runner.add_keyword_recognized_test(verifyRecognizedKeyword);
        runner.add_keyword_not_recognized_test(verifyNotRecognizedKeyword);
        runner.add_recognized_test(verifyRecognizedSpeech);
        runner.add_recognizing_test(verifyRecognizingSpeech);
        runner.add_activity_received_test(verifyActivityReceived);
        runner.add_canceled_test(verifyCanceledSpeech);;

        auto result = runner.run<void>(
            [](SpeechBotConnector& connector, shared_ptr<KeywordRecognitionModel> model)
            {
                return connector.StartKeywordRecognitionAsync(model);
            },
            60s);

        bool success = std::get<0>(result);
        if (!success)
        {
            std::string message = std::move(std::get<1>(result));
            FAIL(message);
        }
    }

    SECTION("Listen once with KWS + KWV reject works")
    {
        UseMocks(false);
        kwvReject.UpdateFullFilename(Config::InputDir);
        REQUIRE(exists(kwvReject.m_inputDataFilename));
        auto config = BotConfigForTests();
        auto audioConfig = AudioConfig::FromWavFileInput(kwvReject.m_inputDataFilename);
        auto model = KeywordRecognitionModel::FromFile(Config::InputDir + "/kws/Computer/kws.table");

        test_runner runner{ SpeechBotConnector::FromConfig(config, audioConfig), model };

        runner.add_keyword_recognizing_test(verifyRecognizingKeyword);
        runner.add_canceled_test(verifyCanceledSpeech);

        runner.add_keyword_recognized_test(
            [](std::ostringstream& oss, const SpeechRecognitionEventArgs&, int)
            {
                oss << "Received a Keyword Recognized event " << std::endl;
                return false;
            },
            0);

        runner.add_keyword_not_recognized_test(
            [](std::ostringstream& oss, const SpeechRecognitionEventArgs&, int fired)
            {
                // We should receive only one of these
                if (fired > 1)
                {
                    oss << "Received more than one Keyword Not Recognized event" << std::endl;
                    return false;
                }
                return true;
            },
            1);

        runner.add_recognized_test(
            [](std::ostringstream& oss, const SpeechRecognitionEventArgs&, int)
            {
                oss << "Received a Recognized event " << std::endl;
                return false;
            },
            0);

        runner.add_activity_received_test(
            [](std::ostringstream& oss, const ActivityReceivedEventArgs&, int)
            {
                oss << "Received a an unexpected activity " << std::endl;
                return false;
            },
            0);

        auto result = runner.run<void>(
            [](SpeechBotConnector& connector, shared_ptr<KeywordRecognitionModel> model)
            {
                return connector.StartKeywordRecognitionAsync(model);
            },
            60s);

        bool success = std::get<0>(result);
        if (!success)
        {
            std::string message = std::move(std::get<1>(result));
            FAIL(message);
        }
    }
}

#if 0
// Disable multi-turn tests for now as we don't have an easy way of loading audio in stages to simulate a microphone scenario
TEST_CASE("Speech Bot Connector KWV multi-turn tests", "[api][cxx][bot_connector]")
{

    SECTION("Later utterances should not be polluted by KWV during multi turn recognition")
    {
        UseMocks(false);
        kwvMultiturn.UpdateFullFilename(Config::InputDir);
        REQUIRE(exists(kwvMultiturn.m_inputDataFilename));
        auto config = BotConfigForTests();
        auto audioConfig = AudioConfig::FromWavFileInput(kwvMultiturn.m_inputDataFilename);
        auto model = KeywordRecognitionModel::FromFile(Config::InputDir + "/kws/Computer/kws.table");

        test_runner runner{ SpeechBotConnector::FromConfig(config, audioConfig), model };

        runner.add_keyword_recognizing_test(verifyRecognizingKeyword);
        runner.add_keyword_recognized_test(verifyRecognizedKeyword);
        runner.add_keyword_not_recognized_test(verifyNotRecognizedKeyword);
        runner.add_recognizing_test(verifyRecognizingSpeech);
        runner.add_canceled_test(verifyCanceledSpeech);

        runner.add_recognized_test(
            [](std::ostringstream& oss, const SpeechRecognitionEventArgs& e, int fired)
            {
                // Note the first turn result should contain the keyword, but the rest shouldn't.
                if (fired == 0)
                {
                    return e.Result->Text == "Computer what's the weather like?";
                } else if (fired == 1) {
                    return e.Result->Text == "Turn on the radio.";
                } else {
                    oss << "Received unexpected utterance" << std::endl;
                    return false;
                }
            },
            2);

        runner.add_activity_received_test(
            [](std::ostringstream& oss, const ActivityReceivedEventArgs& e, int fired)
            {
                if (fired > 2) {
                    oss << "Received unexpected activites" << std::endl;
                    return false;
                }
                auto activity = e.GetActivity();
                return activity->Type == TEST_ACTIVITY_TYPE;
            },
            2);

        auto result = runner.run<void>(
            [](SpeechBotConnector& connector, shared_ptr<KeywordRecognitionModel> model)
            {
                connector.StartKeywordRecognitionAsync(model);
                std::this_thread::sleep_for(5s);
                return connector.ListenOnceAsync();
            },
            60s);

        bool success = std::get<0>(result);
        if (!success)
        {
            std::string message = std::move(std::get<1>(result));
            FAIL(message);
        }
    }
}
#endif
#endif
