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
        bool fired;
        bool ok;
        bool expected;

        void fire(bool success)
        {
            fired = true;
            ok = success;
        }
    };

    template<typename T>
    using test_function = std::function<bool(std::ostringstream&, bool, const T&)>;

    test_runner(std::shared_ptr<SpeechBotConnector> connector): m_connector{connector}
    {};

    template<typename T>
    void wire_event(EventSignal<const T&>& event, test_function<T> test_fn, event_status& status, bool expected)
    {
        event += [this, &status, test_fn](const T& e)
        {
            auto success = test_fn(m_message, !status.fired, e);
            status.fire(success);
            m_cond.notify_one();
        };
        status.expected = expected;
    }

    void add_recognized_test(test_function<SpeechRecognitionEventArgs> test, bool expected)
    {
        wire_event(m_connector->Recognized, test, m_recognized, expected);
    }

    void add_recognizing_test(test_function<SpeechRecognitionEventArgs> test, bool expected)
    {
        wire_event(m_connector->Recognizing, test, m_recognizing, expected);
    }

    void add_activity_received_test(test_function<ActivityReceivedEventArgs> test, bool expected)
    {
        wire_event(m_connector->ActivityReceived, test, m_activity_received, expected);
    }

    void add_canceled_test(test_function<SpeechRecognitionCanceledEventArgs> test, bool expected)
    {
        wire_event(m_connector->Canceled, test, m_canceled, expected);
    }

    static bool ready(event_status e)
    {
        return !e.expected || e.fired;
    }

    static bool ok(event_status e)
    {
        return !e.expected || e.ok;
    }

    template<typename T>
    std::tuple<bool, std::string> run(std::function<std::future<T>(SpeechBotConnector&)> scenario, std::chrono::seconds timeout)
    {
        auto task = scenario(*m_connector);
        std::mutex m;
        std::unique_lock<std::mutex> lk{ m };
        auto res = m_cond.wait_for(lk, timeout, [&]()
        {
            if (m_canceled.fired && !m_canceled.expected)
            {
                /* Unexpected cancel */
                return true;
            }
            if (ready(m_canceled) && ready(m_recognized) && ready(m_recognizing) && ready(m_activity_received))
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

        if (ok(m_canceled) && ok(m_recognized) && ok(m_recognizing) && ok(m_activity_received))
        {
            return std::make_tuple<bool, std::string>(true, "Success");
        }
        return std::make_tuple<bool, std::string>(false, m_message.str());

    }
private:
    std::condition_variable m_cond;
    std::shared_ptr<SpeechBotConnector> m_connector;
    std::ostringstream m_message{};
    event_status m_recognized{false, false, false};
    event_status m_recognizing{false, false, false};
    event_status m_activity_received{false, false, false};
    event_status m_canceled{false, false, false};
};

TEST_CASE("Speech Bot Connector basics", "[api][cxx][bot_connector]")
{
    SECTION("Listen Once works")
    {
        turnOnLamp.UpdateFullFilename(Config::InputDir);
        REQUIRE(exists(turnOnLamp.m_inputDataFilename));

        auto config = BotConfigForTests();
        auto audioConfig = AudioConfig::FromWavFileInput(turnOnLamp.m_inputDataFilename);

        test_runner runner{SpeechBotConnector::FromConfig(config, audioConfig)};

        runner.add_recognized_test([&](std::ostringstream& oss, bool first, const SpeechRecognitionEventArgs& e)
        {
            /* We should received one of these */
            if (!first)
            {
                oss << "Received more than one Recognized event" << std::endl;
                return false;
            }
            return e.Result->Reason == ResultReason::RecognizedSpeech && !e.Result->Text.empty();
        }, true);

        runner.add_recognizing_test([&](std::ostringstream&, bool, const SpeechRecognitionEventArgs& e)
        {
            /* We should receive at least one of these */
            return e.Result->Reason == ResultReason::RecognizingSpeech && !e.Result->Text.empty();
        }, true);

        runner.add_activity_received_test([&](std::ostringstream&, bool, const ActivityReceivedEventArgs& e)
        {
            auto activity = e.GetActivity();
            return activity->Type == "message";
        }, true);

        runner.add_canceled_test([&](std::ostringstream& oss, bool, const SpeechRecognitionCanceledEventArgs& e)
        {
            oss << "Received a cancel event " << e.ErrorDetails << std::endl;
            return false;
        }, false);

        auto result = runner.run<void>([](SpeechBotConnector& connector)
        {
            return connector.ListenOnceAsync();
        }, 15s);
        bool success = std::get<0>(result);
        if (!success)
        {
            std::string message = std::move(std::get<1>(result));
            FAIL(message);
        }
        SUCCEED("Success");
    }

    SECTION("Send/Receive activity works (w/TTS)")
    {
        turnOnLamp.UpdateFullFilename(Config::InputDir);
        REQUIRE(exists(turnOnLamp.m_inputDataFilename));

        auto config = BotConfigForTests();
        auto audioConfig = AudioConfig::FromWavFileInput(turnOnLamp.m_inputDataFilename);

        test_runner runner{SpeechBotConnector::FromConfig(config, audioConfig)};

        constexpr auto activity_type = "message";
        constexpr auto activity_text = "message";
        constexpr auto activity_speak = "Hello world";

        runner.add_activity_received_test([&](std::ostringstream& oss, bool, const ActivityReceivedEventArgs& e)
        {
            auto activity = e.GetActivity();
            bool success{true};
            if (activity->Type != activity_type)
            {
                oss << "activity->Type -> " << activity->Type << " != " << activity_type << std::endl;
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
                while(auto count = audio->Read(buffer.get(), 800))
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
        }, true);

        runner.add_canceled_test([&](std::ostringstream& oss, bool, const SpeechRecognitionCanceledEventArgs& e)
        {
            oss << "Received a cancel event " << e.ErrorDetails << std::endl;
            return false;
        }, false);

        auto result = runner.run<std::string>([&](SpeechBotConnector& connector)
        {
            auto activity = BotConnectorActivity::Create();
            activity->Type = activity_type;
            activity->Text = activity_text;
            activity->Speak = activity_speak;
            return connector.SendActivityAsync(activity);
        }, 5s);
        bool success = std::get<0>(result);
        if (!success)
        {
            std::string message = std::move(std::get<1>(result));
            FAIL(message);
        }
        SUCCEED("Succeeded");

    }
}
