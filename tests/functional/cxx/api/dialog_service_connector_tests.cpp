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

#include <json.hpp>

using namespace Microsoft::CognitiveServices::Speech;
using namespace Microsoft::CognitiveServices::Speech::Audio;
using namespace Microsoft::CognitiveServices::Speech::Dialog;

using namespace std::chrono_literals;

constexpr auto TEST_KWS_KEYWORD = "Computer";
constexpr auto TEST_ACTIVITY_TYPE = "message";

std::shared_ptr<DialogServiceConfig> DialogServiceConfigForTests(bool isBot = true)
{
    std::shared_ptr<DialogServiceConfig> config;
    if (isBot)
    {
        config = BotFrameworkConfig::FromSubscription(Keys::Dialog, Config::DialogRegion);
    }
    else
    {
        config = SpeechCommandsConfig::FromSubscription(Config::DialogBotSecret, Keys::Dialog, Config::DialogRegion);
    }
    config->SetLanguage("en-us");
    config->SetProperty("Conversation_Communication_Type", "AutoReply");
    config->SetProperty(PropertyId::Conversation_ApplicationId, Config::DialogBotSecret);
    return config;
}

struct scope_exit
{
    scope_exit(std::function<void()> fn): m_fn{fn}
    {}

    ~scope_exit()
    {
        m_fn();
    }
private:
    std::function<void()> m_fn;
};

struct test_runner
{
    struct event_status
    {
        event_status(): fired{0}, expected{0}, ok{true}
        {
        }

        void reset()
        {
            fired = 0;
            ok = true;
        }

        uint32_t fired;
        uint32_t expected;
        bool ok;
    };

    template<typename T>
    using test_function = std::function<bool(std::ostringstream&, const T&, int)>;

    static bool isSpeech(const SpeechRecognitionEventArgs& e)
    {
        return e.Result->Reason == ResultReason::RecognizedSpeech ||
               e.Result->Reason == ResultReason::RecognizingSpeech;
    };

    static bool isKeyword(const SpeechRecognitionEventArgs& e)
    {
        return e.Result->Reason == ResultReason::RecognizedKeyword ||
               e.Result->Reason == ResultReason::RecognizingKeyword;
    };

    static bool isKeywordNotRecognized(const SpeechRecognitionEventArgs& e)
    {
        return e.Result->Reason == ResultReason::NoMatch && NoMatchDetails::FromResult(e.Result)->Reason == NoMatchReason::KeywordNotRecognized;
    };

    test_runner(std::shared_ptr<DialogServiceConfig> config, std::shared_ptr<AudioConfig> audio): m_config{config}, m_audio{audio}
    {};

    template<typename T>
    std::function<void(const T&)> wire_event(event_status& status, test_function<T> test_fn, int expected)
    {
        status.expected = expected;
        status.ok = status.expected == 0;
        return [this, &status, test_fn](const T& e)
        {
            status.ok = test_fn(m_log, e, status.fired);
            status.fired++;
            m_cond.notify_one();
        };
    }

    void add_keyword_recognizing_test(test_function<SpeechRecognitionEventArgs> test, int expected = 1)
    {
        m_keyword_recognizing_handler = wire_event(m_keyword_recognizing, test, expected);
    }

    void add_keyword_recognized_test(test_function<SpeechRecognitionEventArgs> test, int expected = 1)
    {
        m_keyword_recognized_handler = wire_event(m_keyword_recognized, test, expected);
    }

    void add_keyword_not_recognized_test(test_function<SpeechRecognitionEventArgs> test, int expected = 0)
    {
        m_keyword_not_recognized_handler = wire_event(m_keyword_not_recognized, test, expected);
    }

    void add_recognized_test(test_function<SpeechRecognitionEventArgs> test, int expected = 1)
    {
        m_recognized_handler = wire_event(m_recognized, test, expected);
    }

    void add_recognizing_test(test_function<SpeechRecognitionEventArgs> test, int expected = 1)
    {
        m_recognizing_handler = wire_event(m_recognizing, test, expected);
    }

    void add_activity_received_test(test_function<ActivityReceivedEventArgs> test, int expected = 1)
    {
        m_activity_received_handler = wire_event(m_activity_received, test, expected);
    }

    void add_canceled_test(test_function<SpeechRecognitionCanceledEventArgs> test, int expected = 0)
    {
        m_canceled_handler = wire_event(m_canceled, test, expected);
    }

    static bool ready(event_status e)
    {
        return e.expected == 0 || e.expected <= e.fired;
    }

    template<typename T>
    std::tuple<bool, std::string> run(std::function<std::future<T>(DialogServiceConnector&)> scenario, std::chrono::seconds timeout, uint32_t retries)
    {
        for(auto i = 0u; i < retries; i++)
        {
            scope_exit at_exit{[&]()
            {
                m_keyword_recognized.reset();
                m_keyword_recognizing.reset();
                m_keyword_not_recognized.reset();
                m_recognized.reset();
                m_recognizing.reset();
                m_activity_received.reset();
                m_canceled.reset();
            }};

            auto connector = DialogServiceConnector::FromConfig(m_config, m_audio);
            connector->Recognized += [&](const SpeechRecognitionEventArgs& e)
            {
                if (isKeyword(e) && m_keyword_recognized_handler)
                {
                    return m_keyword_recognized_handler(e);
                }
                else if (isKeywordNotRecognized(e) && m_keyword_not_recognized_handler)
                {
                    return m_keyword_not_recognized_handler(e);
                }
                else if (isSpeech(e) && m_recognized_handler)
                {
                    return m_recognized_handler(e);
                }
            };
            connector->Recognizing += [&](const SpeechRecognitionEventArgs& e)
            {
                if (isKeyword(e) && m_keyword_recognizing_handler)
                {
                    return m_keyword_recognizing_handler(e);
                }
                else if (isSpeech(e) && m_recognizing_handler)
                {
                    return m_recognizing_handler(e);
                }
            };
            connector->ActivityReceived += [&](const ActivityReceivedEventArgs& e)
            {
                if (m_activity_received_handler)
                {
                    return m_activity_received_handler(e);
                }
            };
            connector->Canceled += [&](const SpeechRecognitionCanceledEventArgs& e)
            {
                if (m_canceled_handler)
                {
                    return m_canceled_handler(e);
                }
            };

            auto task = scenario(*connector);

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
                m_log << "Attempt " << i << " timed out." << std::endl;
                continue;
            }

            if (m_canceled.ok &&
                m_recognized.ok &&
                m_recognizing.ok &&
                m_activity_received.ok &&
                m_keyword_not_recognized.ok &&
                m_keyword_recognized.ok &&
                m_keyword_recognizing.ok)
            {
                return std::make_tuple<bool, std::string>(true, m_log.str());
            }
            m_log << "Attempt " << i << " failed." << std::endl;
        }

        return std::make_tuple<bool, std::string>(false, m_log.str());
    }

private:
    std::shared_ptr<DialogServiceConfig> m_config;
    std::shared_ptr<AudioConfig> m_audio;

    std::condition_variable m_cond;

    std::ostringstream m_log{};

    std::function<void(const SpeechRecognitionEventArgs&)> m_keyword_recognizing_handler;
    std::function<void(const SpeechRecognitionEventArgs&)> m_keyword_recognized_handler;
    std::function<void(const SpeechRecognitionEventArgs&)> m_keyword_not_recognized_handler;
    std::function<void(const SpeechRecognitionEventArgs&)> m_recognized_handler;
    std::function<void(const SpeechRecognitionEventArgs&)> m_recognizing_handler;
    std::function<void(const ActivityReceivedEventArgs&)> m_activity_received_handler;
    std::function<void(const SpeechRecognitionCanceledEventArgs&)> m_canceled_handler;

    event_status m_keyword_recognized;
    event_status m_keyword_recognizing;
    event_status m_keyword_not_recognized;
    event_status m_recognized;
    event_status m_recognizing;
    event_status m_activity_received;
    event_status m_canceled;
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
    auto activityStr = e.GetActivity();
    auto activity = nlohmann::json::parse(activityStr);
    return activity["type"].get<std::string>() == TEST_ACTIVITY_TYPE;
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

TEST_CASE("Dialog Service Connector basics", "[api][cxx][dialog_service_connector]")
{
    SECTION("Listen Once works")
    {
        turnOnLamp.UpdateFullFilename(Config::InputDir);
        REQUIRE(exists(turnOnLamp.m_inputDataFilename));
        auto config = DialogServiceConfigForTests();
        auto audioConfig = AudioConfig::FromWavFileInput(turnOnLamp.m_inputDataFilename);

        test_runner runner{ config, audioConfig };

        runner.add_recognized_test(verifyRecognizedSpeech);
        runner.add_recognizing_test(verifyRecognizingSpeech);
        runner.add_activity_received_test(verifyActivityReceived);
        runner.add_canceled_test(verifyCanceledSpeech);

        auto result = runner.run<void>(
            [](DialogServiceConnector& connector)
            {
                return connector.ListenOnceAsync();
            },
            20s, 3u);

        auto success = std::get<0>(result);
        auto message = std::move(std::get<1>(result));
        INFO(message);
        REQUIRE(success);
    }

    SECTION("Send/Receive activity works (w/TTS)")
    {
        turnOnLamp.UpdateFullFilename(Config::InputDir);
        REQUIRE(exists(turnOnLamp.m_inputDataFilename));
        auto config = DialogServiceConfigForTests();
        auto audioConfig = AudioConfig::FromWavFileInput(turnOnLamp.m_inputDataFilename);

        test_runner runner{ config, audioConfig };

        constexpr auto activity_text = "message";
        constexpr auto activity_speak = "Hello world";

        runner.add_activity_received_test(
            [&](std::ostringstream& oss, const ActivityReceivedEventArgs& e, int)
            {
                auto activityStr = e.GetActivity();
                auto activity = nlohmann::json::parse(activityStr);
                bool success{ true };
                if (activity["type"].get<std::string>() != TEST_ACTIVITY_TYPE)
                {
                    oss << "activity[\"type\"] -> " << activity["type"] << " != " << TEST_ACTIVITY_TYPE << std::endl;
                    success = false;
                }
                if (activity["text"] != activity_text)
                {
                    oss << "activity[\"text\"] -> " << activity["text"] << " != " << activity_text << std::endl;
                    success = false;
                }
                if (activity["speak"] != activity_speak)
                {
                    oss << "activity[\"speak\"] -> " << activity["speak"] << " != " << activity_speak << std::endl;
                    success = false;
                }

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

                return success;
        });

        runner.add_canceled_test(verifyCanceledSpeech);

        auto result = runner.run<std::string>(
            [&](DialogServiceConnector& connector)
            {
                nlohmann::json act{
                    { "type", TEST_ACTIVITY_TYPE },
                    { "text", activity_text },
                    { "speak", activity_speak }
                };
                auto activity = act.dump();
                return connector.SendActivityAsync(activity);
            },
            20s, 3);

        auto success = std::get<0>(result);
        auto message = std::move(std::get<1>(result));
        INFO(message);
        REQUIRE(success);

    }
}

TEST_CASE("Dialog Service Connector extended", "[api][cxx][dialog_service_connector][adv][!hide]")
{
    SECTION("Interleaving speech and activities.")
    {
        turnOnLamp.UpdateFullFilename(Config::InputDir);
        REQUIRE(exists(turnOnLamp.m_inputDataFilename));
        auto config = DialogServiceConfigForTests();
        auto audioConfig = AudioConfig::FromWavFileInput(turnOnLamp.m_inputDataFilename);

        test_runner runner{ config, audioConfig };

        constexpr auto activity_text = "message";

        runner.add_recognized_test(verifyRecognizedSpeech, 1);
        runner.add_activity_received_test([](std::ostringstream&, const ActivityReceivedEventArgs&, int)
        {
            /* Just check that we are receiving it */
            return true;
        }, 2);
        runner.add_canceled_test(verifyCanceledSpeech);

        auto result = runner.run<void>(
            [&](DialogServiceConnector& connector)
            {
                auto f1 = connector.ListenOnceAsync();
                nlohmann::json act{
                    { "type", TEST_ACTIVITY_TYPE },
                    { "text", activity_text }
                };
                auto activity = act.dump();
                auto f2 = connector.SendActivityAsync(activity);
                return std::async([f1 = std::move(f1), f2 = std::move(f2)]()
                {
                    f1.wait();
                    f2.wait();
                    return;
                });
            },
            20s, 3);

        auto success = std::get<0>(result);
        auto message = std::move(std::get<1>(result));
        INFO(message);
        REQUIRE(success);
    }
}

TEST_CASE("Dialog Service Connector SpeechCommands", "[api][cxx][dialog_service_connector][tahiti][!hide]")
{
    SECTION("Send/receive activities.")
    {
        turnOnLamp.UpdateFullFilename(Config::InputDir);
        REQUIRE(exists(turnOnLamp.m_inputDataFilename));
        auto config = DialogServiceConfigForTests(false);
        auto audioConfig = AudioConfig::FromWavFileInput(turnOnLamp.m_inputDataFilename);

        test_runner runner{ config, audioConfig };

        constexpr auto activity_text = "message";

        runner.add_activity_received_test([](std::ostringstream&, const ActivityReceivedEventArgs&, int)
        {
            /* Just check that we are receiving it */
            return true;
        }, 1);
        runner.add_canceled_test(verifyCanceledSpeech);

        auto result = runner.run<std::string>(
            [&](DialogServiceConnector& connector)
            {
                nlohmann::json act{
                        { "type", TEST_ACTIVITY_TYPE },
                        { "text", activity_text }
                };
                auto activity = act.dump();
                return connector.SendActivityAsync(activity);
            },
            20s, 3);

        auto success = std::get<0>(result);
        auto message = std::move(std::get<1>(result));
        INFO(message);
        REQUIRE(success);
    }
}

#ifdef SPEECHSDK_KWS_ENABLED

// The following tests currently do only work with a real KWS engine (not the
// KWS mock). Consider extending the mock:
// 1) Being able to specify a custom number of keyword detection
// 2) Being able to specify the audio that will be streamed to the service for keyword verification.
// 3) Being able to specify the keyword text in keyword detection (Done)
// 4) Being able to specify the audio for the latter utterance

TEST_CASE("Dialog Service Connector KWS basics", "[api][cxx][dialog_service_connector]") {

    SECTION("Listen once with KWS only works")
    {
        UseMocks(false);
        kwvAccept.UpdateFullFilename(Config::InputDir);
        REQUIRE(exists(kwvAccept.m_inputDataFilename));
        auto config = DialogServiceConfigForTests();
        // Turn off keyword verification
        config->SetProperty("KeywordConfig_EnableKeywordVerification", "false");
        auto audioConfig = AudioConfig::FromWavFileInput(kwvAccept.m_inputDataFilename);
        auto model = KeywordRecognitionModel::FromFile(Config::InputDir + "/kws/Computer/kws.table");

        test_runner runner{ config, audioConfig };

        runner.add_keyword_recognizing_test(verifyRecognizingKeyword);
        runner.add_keyword_recognized_test(verifyRecognizedKeyword);
        runner.add_keyword_not_recognized_test(verifyNotRecognizedKeyword);
        runner.add_recognized_test(verifyRecognizedSpeech);
        runner.add_recognizing_test(verifyRecognizingSpeech);
        runner.add_activity_received_test(verifyActivityReceived);
        runner.add_canceled_test(verifyCanceledSpeech);

        auto result = runner.run<void>(
            [model](DialogServiceConnector& connector)
            {
                return connector.StartKeywordRecognitionAsync(model);
            },
            60s, 3);

        bool success = std::get<0>(result);
        if (!success)
        {
            std::string message = std::move(std::get<1>(result));
            FAIL(message);
        }
    }
}

TEST_CASE("Dialog Service Connector KWV basics", "[api][cxx][dialog_service_connector][!hide]")
{

    SECTION("Listen once with KWS + KWV accept works")
    {
        UseMocks(false);
        kwvAccept.UpdateFullFilename(Config::InputDir);
        REQUIRE(exists(kwvAccept.m_inputDataFilename));
        auto config = DialogServiceConfigForTests();
        auto audioConfig = AudioConfig::FromWavFileInput(kwvAccept.m_inputDataFilename);
        auto model = KeywordRecognitionModel::FromFile(Config::InputDir + "/kws/Computer/kws.table");

        test_runner runner{ config, audioConfig };

        runner.add_keyword_recognizing_test(verifyRecognizingKeyword);
        runner.add_keyword_recognized_test(verifyRecognizedKeyword);
        runner.add_keyword_not_recognized_test(verifyNotRecognizedKeyword);
        runner.add_recognized_test(verifyRecognizedSpeech);
        runner.add_recognizing_test(verifyRecognizingSpeech);
        runner.add_activity_received_test(verifyActivityReceived);
        runner.add_canceled_test(verifyCanceledSpeech);;

        auto result = runner.run<void>(
            [model](DialogServiceConnector& connector)
            {
                return connector.StartKeywordRecognitionAsync(model);
            },
            60s, 3);

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
        auto config = DialogServiceConfigForTests();
        auto audioConfig = AudioConfig::FromWavFileInput(kwvReject.m_inputDataFilename);
        auto model = KeywordRecognitionModel::FromFile(Config::InputDir + "/kws/Computer/kws.table");

        test_runner runner{ config, audioConfig };

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
            [model](DialogServiceConnector& connector)
            {
                return connector.StartKeywordRecognitionAsync(model);
            },
            60s, 3);

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
TEST_CASE("Dialog Service Connector KWV multi-turn tests", "[api][cxx][dialog_service_connector]")
{

    SECTION("Later utterances should not be polluted by KWV during multi turn recognition")
    {
        UseMocks(false);
        kwvMultiturn.UpdateFullFilename(Config::InputDir);
        REQUIRE(exists(kwvMultiturn.m_inputDataFilename));
        auto config = DialogServiceConfigForTests();
        auto audioConfig = AudioConfig::FromWavFileInput(kwvMultiturn.m_inputDataFilename);
        auto model = KeywordRecognitionModel::FromFile(Config::InputDir + "/kws/Computer/kws.table");

        test_runner runner{ DialogServiceConnector::FromConfig(config, audioConfig), model };

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
            [](DialogServiceConnector& connector, shared_ptr<KeywordRecognitionModel> model)
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
