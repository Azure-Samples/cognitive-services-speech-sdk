//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#pragma once

#include <iostream>
#include <random>
#include <signal.h>
#include <fstream>
#include <algorithm>
#include <string>

#ifdef _DEBUG
#define SPX_CONFIG_INCLUDE_ALL_DBG 1
#define SPX_CONFIG_INCLUDE_ALL 1
#else
#define SPX_CONFIG_INCLUDE_ALL 1
#endif

#include "trace_message.h"

#ifndef __SPX_DO_TRACE_IMPL
#define __SPX_DO_TRACE_IMPL SpxTraceMessage
#endif

#include "string_utils.h"
#include "debug_utils.h"
#include "json.h"

#ifdef _MSC_VER
#pragma warning( push )
// disable: (9754,94): error 6330:  : 'const char' passed as _Param_(1) when 'unsigned char' is required in call to 'isalnum'.
#pragma warning( disable : 6330 )
#include "catch.hpp"
#pragma warning( pop )
#else
#include "catch.hpp"
#endif

#if !defined(CATCH_CONFIG_RUNNER)
#define EXTERN extern
#else
#define EXTERN
#endif

#define TEST_SETTINGS_FILE "test.settings.json"

namespace Keys
{
    EXTERN std::string Speech;
    EXTERN std::string LUIS;
    EXTERN std::string Dialog;
    EXTERN std::string ConversationTranscriber;
}

namespace Config
{
    EXTERN std::string Endpoint;
    EXTERN std::string Region;
    EXTERN std::string LuisRegion;
    EXTERN std::string LuisAppId;
    EXTERN std::string InputDir;
    EXTERN std::string DialogRegion;
    EXTERN std::string DialogBotSecret;
    EXTERN std::string InRoomAudioEndpoint;
    EXTERN std::string OnlineAudioEndpoint;
    EXTERN std::string OfflineModelPathRoot;
    EXTERN std::string OfflineModelLanguage;
    EXTERN bool DoDiscover;
}

#define UNIFIED_SPEECH_SUBSCRIPTION_KEY "UnifiedSpeechSubscriptionKey"
#define LANGUAGE_UNDERSTANDING_SUBSCRIPTION_KEY "LanguageUnderstandingSubscriptionKey"
#define DIALOG_SUBSCRIPTION_KEY "DialogSubscriptionKey"
#define CONVERSATION_TRANSCRIPTION_SUBSCRIPTION_KEY "ConversationTranscriptionPPEKey"

#define CONVERSATION_TRANSCRIPTION_ENDPOINT "ConversationTranscriptionEndpoint"
#define REGION "Region"
#define LANGUAGE_UNDERSTANDING_SERVICE_REGION "LanguageUnderstandingServiceRegion"
#define LANGUAGE_UNDERSTANDING_HOME_AUTOMATION_APP_ID "LanguageUnderstandingHomeAutomationAppId"
#define SPEECH_REGION_FOR_CONVERSATION_TRANSCRIPTION "SpeechRegionForConversationTranscription"
#define IN_ROOM_AUDIO_ENDPOINT "InRoomAudioEndpoint"
#define ONLINE_AUDIO_ENDPOINT "OnlineAudioEndpoint"
#define INPUT_DIR "InputDir"
#define DIALOG_FUNCTIONAL_TEST_BOT "DialogFunctionalTestBot"
#define DIALOG_REGION "DialogRegion"

inline bool exists(const std::string& name) {
    return std::ifstream(name.c_str()).good();
}

inline std::ifstream get_stream(const std::string& name) {
    return std::ifstream(name.c_str(), std::ifstream::binary);
}

inline void GetKeyValue(const char* key, std::string &value, nlohmann::json &data, const char* file, int line)
{
    try
    {
        value = data.at(key).get<std::string>();
    }
    catch (nlohmann::json::type_error& e)
    {
        SpxConsoleLogger_Log(LOG_CATEGORY::AZ_LOG_ERROR, file, "LoadFromJsonFile", line, 0, "Error: exception in AT, %s.", e.what());
    }
}

class StringComparisions {
public:
    static bool AssertFuzzyMatch(const std::string recieved, const std::string expected, size_t deltaPercentage = 10, size_t minAllowedMismatchCount = 1) {
        size_t errors, totalWords;

        CalculateWordErrorRate(recieved, expected, &errors, &totalWords);

        // Find the max number of errors to allow.
        size_t allowedEdits = std::max(minAllowedMismatchCount, (deltaPercentage * totalWords) / 100);

        return (allowedEdits >= errors);
    }

private:
    template <class T>
    static unsigned int Levenshtein(const T& s1, const T& s2)
    {
        const std::size_t l1 = s1.size(), l2 = s2.size();
        std::vector<unsigned int> d1(l2 + 1), d2(l2 + 1);

        for (unsigned int i = 0; i < d2.size(); i++) d2[i] = i;
        for (unsigned int i = 0; i < l1; i++) {
            d1[0] = i + 1;
            for (unsigned int j = 0; j < l2; j++) {
                d1[j + 1] = std::min({ d2[1 + j] + 1, d1[j] + 1, d2[j] + (s1[i] == s2[j] ? 0 : 1) });
            }
            d1.swap(d2);
        }
        return d2[l2];
    }

    static void CalculateWordErrorRate(const std::string& text1, const std::string& text2, size_t* errors, size_t* words)
    {
        auto words1 = PAL::split(text1, ' ');
        auto words2 = PAL::split(text2, ' ');

        *errors = Levenshtein(words1, words2);
        *words = words1.size();
       }
};

class ConfigSettings {
private:
    static nlohmann::json getJson(std::string path)
    {
        nlohmann::json nlohmanJson = nullptr;

        if (exists(path))
        {
            std::ifstream testSettingsFile(path);
            testSettingsFile >> nlohmanJson;
        }

        return nlohmanJson;
    }

public:
    static void LoadFromJsonFile(std::string rootPathString)
    {
        std::string testSettingsPath = rootPathString.append(TEST_SETTINGS_FILE);
        SpxConsoleLogger_Log(LOG_CATEGORY::AZ_LOG_INFO, __FILE__, "LoadFromJsonFile", __LINE__, 0, "Attempting to read file at %s", testSettingsPath.c_str());

        nlohmann::json data = getJson(testSettingsPath);

        if (data != nullptr)
        {
            GetKeyValue(UNIFIED_SPEECH_SUBSCRIPTION_KEY, Keys::Speech, data, __FILE__, __LINE__);
            GetKeyValue(LANGUAGE_UNDERSTANDING_SUBSCRIPTION_KEY, Keys::LUIS, data, __FILE__, __LINE__);
            GetKeyValue(DIALOG_SUBSCRIPTION_KEY, Keys::Dialog, data, __FILE__, __LINE__);
            GetKeyValue(DIALOG_FUNCTIONAL_TEST_BOT, Config::DialogBotSecret, data, __FILE__, __LINE__);
            GetKeyValue(CONVERSATION_TRANSCRIPTION_SUBSCRIPTION_KEY, Keys::ConversationTranscriber, data, __FILE__, __LINE__);
            GetKeyValue(DIALOG_REGION, Config::DialogRegion, data, __FILE__, __LINE__),
            GetKeyValue(REGION, Config::Region, data, __FILE__, __LINE__);
            GetKeyValue(LANGUAGE_UNDERSTANDING_SERVICE_REGION, Config::LuisRegion, data, __FILE__, __LINE__);
            GetKeyValue(LANGUAGE_UNDERSTANDING_HOME_AUTOMATION_APP_ID, Config::LuisAppId, data, __FILE__, __LINE__);
            GetKeyValue(IN_ROOM_AUDIO_ENDPOINT, Config::InRoomAudioEndpoint, data, __FILE__, __LINE__);
            GetKeyValue(ONLINE_AUDIO_ENDPOINT, Config::OnlineAudioEndpoint, data, __FILE__, __LINE__);
            GetKeyValue(INPUT_DIR, Config::InputDir, data, __FILE__, __LINE__);
        }
        else
        {
            SpxConsoleLogger_Log(LOG_CATEGORY::AZ_LOG_ERROR, __FILE__, "LoadFromJsonFile", __LINE__, 0, "JSON could not be loaded from %s", testSettingsPath.c_str());
        }
    }
};

inline bool checkForDiscovery(int argc, char*argv[])
{
    for (int index = 0; index < argc; index++)
    {
        if (!strcmp(argv[index], "--discovery"))
        {
            return true;
        }
    }

    return false;
}

typedef std::linear_congruential_engine<uint_fast32_t, 1664525, 1013904223, UINT_FAST32_MAX> random_engine;

inline void add_signal_handlers()
{
    Debug::HookSignalHandlers();
}

#if defined(CATCH_CONFIG_RUNNER)
inline int parse_cli_args(Catch::Session& session, int argc, char* argv[])
{
    // Build a new parser on top of Catch's
    using namespace Catch::clara;

    auto cli
        = session.cli() // Get Catch's composite command line parser
        | Opt(Keys::Speech, "SpeechSubscriptionKey") // bind variable to a new option, with a hint string
        ["--keySpeech"]    // the option names it will respond to
    ("The subscription key for speech")
        | Opt(Keys::LUIS, "LuisSubscriptionKey")
        ["--keyLUIS"]
    ("The subscription key for language understanding")
        | Opt(Keys::Dialog, "keyDialog")
        ["--keyDialog"]
    ("The subscription key for the Speech Channel")
        | Opt(Config::Endpoint, "endpoint")
        ["--endpoint"]
    ("The endpoint url to test against.")
        | Opt(Config::Region, "Region")
        ["--region"]
    ("The region id to be used for subscription and authorization requests")
        | Opt(Config::LuisRegion, "LuisRegionId")
        ["--regionIdLUIS"]
    ("The region id to be used for language understanding subscription and authorization requests")
        | Opt(Config::LuisAppId, "LuisAppId")
        ["--luisAppId"]
    ("The language understanding app id to be used intent recognition tests")
        | Opt(Config::InRoomAudioEndpoint, "InRoomAudioEndpoint")
        ["--InRoomAudioEndpoint"]
    ("The endpoint that in-room tests in intelligent meeting recognizer talks to")
        | Opt(Config::OnlineAudioEndpoint, "OnlineAudioEndpoint")
        ["--OnlineAudioEndpoint"]
    ("The endpoint that on-line tests in intelligent meeting recognizer talks to")
        | Opt(Keys::ConversationTranscriber, "ConversationTranscriber")
        ["--keyConversationTranscriberPPE"]
    ("The conversation transcriber key")
        | Opt(Config::InputDir, "InputDir")
        ["--inputDir"]
    ("The directory where test input files are placed")
        | Opt(Config::DialogRegion, "DialogRegion")
        ["--dialogRegionId"]
    ("The region id to be used for the Speech Channel Service")
        | Opt(Config::DialogBotSecret, "DialogBotSecret")
        ["--dialogBotSecret"]
    ("Secret for the functional test bot")
        | Opt(Config::OfflineModelPathRoot, "OfflineModelPathRoot")
        ["--offlineModelPathRoot"]
    ("The root path under which offline speech recognition models are located.")
        | Opt(Config::OfflineModelLanguage, "OfflineModelLanguage")
        ["--offlineModelLanguage"]
    ("The language code of the offline speech recognition model used in tests.")
        | Opt(Config::DoDiscover)
        ["--discovery"]
    ("Perform VS Test Adaptor discovery");
    ;

    // Now pass the new composite back to Catch so it uses that
    session.cli(cli);

    // Let Catch (using Clara) parse the command line
    return session.applyCommandLine(argc, argv);
}
#endif

#define SPXTEST_SECTION(msg) SECTION(msg) if ([=](){ \
    SPX_TRACE_INFO("SPXTEST_SECTION('%s') %s(%d)", msg, __FILE__, __LINE__);  \
    return 1; }())

#define SPXTEST_GIVEN(msg) GIVEN(msg) if ([=](){ \
    SPX_TRACE_INFO("SPXTEST_GIVEN('%s') %s(%d):", msg, __FILE__, __LINE__); \
    return 1; }())

#define SPXTEST_WHEN(msg) WHEN(msg) if ([=](){ \
    SPX_TRACE_INFO("SPXTEST_WHEN('%s') %s(%d):", msg, __FILE__, __LINE__); \
    return 1; }())

#define SPXTEST_REQUIRE(expr) \
    SPX_TRACE_INFO("SPXTEST_REQUIRE('%s'): %s(%d):", __SPX_EXPR_AS_STRING(expr), __FILE__, __LINE__); \
    SPX_TRACE_ERROR_IF(!(expr), "SPXTEST_REQUIRE('%s') FAILED: %s(%d):", __SPX_EXPR_AS_STRING(expr), __FILE__, __LINE__); \
    REQUIRE(expr)
