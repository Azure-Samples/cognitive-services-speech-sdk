//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#pragma once

#include <iostream>
#include <sstream>
#include <random>
#include <signal.h>
#include <fstream>
#include <algorithm>
#include <string>

#ifdef _DEBUG
#define SPX_CONFIG_DBG_TRACE_ALL 1
#define SPX_CONFIG_TRACE_ALL 1
#else
#define SPX_CONFIG_TRACE_ALL 1
#endif

#include "trace_message.h"

#ifndef __SPX_DO_TRACE_IMPL
#define __SPX_DO_TRACE_IMPL SpxTraceMessage
#endif

#include "string_utils.h"
#include "debug_utils.h"
#include "json.h"
#include "exception.h"

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

#define TEST_AUDIOUTTERANCES_FILE "test.audio.utterances.json"
#define TEST_DEFAULTS_FILE "test.defaults.json"
#define TEST_SUBSCRIPTIONSREGIONS_FILE "test.subscriptions.regions.json"

namespace Config
{
    EXTERN std::string OfflineModelPathRoot;
    EXTERN std::string OfflineModelLanguage;
    EXTERN bool DoDiscover;
}

#define ROOT_RELATIVE_PATH(pathName) DefaultSettingsMap[INPUT_DIR] + "/" + AudioUtterancesMap[pathName].FilePath

// Subscriptions and regions keys
#define KEY "Key"
#define REGION "Region"
#define UNIFIED_SPEECH_SUBSCRIPTION "UnifiedSpeechSubscription"
#define DIALOG_SUBSCRIPTION "DialogSubscription"
#define LANGUAGE_UNDERSTANDING_SUBSCRIPTION "LanguageUnderstandingSubscription"
#define CONVERSATION_TRANSCRIPTION_PROD_SUBSCRIPTION "ConversationTranscriptionPRODSubscription"
#define CONVERSATION_TRANSCRIPTION_PPE_SUBSCRIPTION "ConversationTranscriptionPPESubscription"
#define CONVERSATION_TRANSLATOR_SUBSCRIPTION "ConversationTranslatorSubscription"

// Default settings keys
#define ENDPOINT "Endpoint"
#define DIALOG_FUNCTIONAL_TEST_BOT "DialogFunctionalTestBot"
#define LANGUAGE_UNDERSTANDING_HOME_AUTOMATION_APP_ID "LanguageUnderstandingHomeAutomationAppId"
#define INROOM_AUDIO_ENDPOINT "InRoomAudioEndpoint"
#define CONVERSATION_TRANSCRIPTION_ENDPOINT "ConversationTranscriptionEndpoint"
#define LONG_RUNNING "LongRunning"
#define DEPLOYMENT_ID "DeploymentId"
#define ONLINE_AUDIO_ENDPOINT "OnlineAudioEndpoint"
#define INPUT_DIR "InputDir"
#define CONVERSATION_TRANSLATOR_HOST "ConversationTranslatorHost"
#define CONVERSATION_TRANSLATOR_SPEECH_ENDPOINT "ConversationTranslatorSpeechEndpoint"
#define CONVERSATION_TRANSLATOR_CLIENTID "ConversationTranslatorClientId"

// Audio file and utterances keys
#define FILE_PATH "FilePath"
#define NATIVE_LANGUAGE "NativeLanguage"
#define UTTERANCES "Utterances"
#define UTTERANCE_TEXT "Text"
#define PROFANITY_RAW "ProfanityRaw"
#define PROFANITY_MASKED "ProfanityMasked"
#define PROFANITY_MASKED_PATTERN "ProfanityMaskedPattern"
#define PROFANITY_REMOVED "ProfanityRemoved"
#define PROFANITY_TAGGED "ProfanityTagged"
#define AUDIO_OFFSET "AudioOffset"
#define AUDIO_DURATION "AudioDuration"
#define TEXT_OFFSET "TextOffset"
#define SSML_OFFSET "SsmlOffse"

#define SINGLE_UTTERANCE_ENGLISH "SingleUtteranceEnglish"
#define SINGLE_UTTERANCE_CHINESE "SingleUtteranceChinese"
#define SINGLE_UTTERANCE_MP3 "SingleUtteranceMP3"
#define SINGLE_UTTERANCE_OPUS "SingleUtteranceOPUS"
#define SINGLE_UTTERANCE_A_LAW "SingleUtteranceALaw"
#define SINGLE_UTTERANCE_MU_LAW "SingleUtteranceMULaw"
#define SINGLE_UTTERANCE_FLAC "SingleUtteranceFLAC"
#define SINGLE_UTTERANCE_3X "SingleUtterance3x"
#define SINGLE_UTTERANCE_MULTIPLE_TURNS "SingleUtteranceMultipleTurns"
#define SINGLE_UTTERANCE_CATALAN "SingleUtteranceCatalan"
#define MULTIPLE_UTTERANCE_ENGLISH "MultipleUtteranceEnglish"
#define AUDIO_44_1KHZ "Audio441Khz"
#define AUDIO_11_KHZ "Audio11Khz"
#define HEY_CORTANA "HeyCortana"
#define SINGLE_UTTERANCE_GERMAN "SingleUtteranceGerman"
#define INTENT_UTTERANCE "IntentUtterance"
#define AMBIGUOUS_SPEECH "AmbiguousSpeech"
#define COMPUTER_KEYWORD_WITH_SINGLE_UTTERANCE_1 "ComputerKeywordWithSingleUtterance1" // Used to be accept
#define COMPUTER_KEYWORD_WITH_SINGLE_UTTERANCE_2 "ComputerKeywordWithSingleUtterance2"
#define COMPUTER_KEYWORD_WITH_SINGLE_UTTERANCE_2X "ComputerKeywordWithSingleUtterance2x" // Used to be acceptx2
#define COMPUTER_KEYWORD_WITH_SINGLE_UTTERANCE_3 "ComputerKeywordWithSingleUtterance3"
#define COMPUTER_KEYWORD_WITH_MULTIPLE_TURNS_1 "ComputerKeywordWithMultipleTurns"
#define SECRET_KEYWORDS "SecretKeywords"
#define CONVERSATION_BETWEEN_TWO_PERSONS_ENGLISH "ConversationBetweenTwoPersonsEnglish"
#define PERSON_ENROLLMENT_ENGLISH_1 "PersonEnrollmentEnglish1"
#define PERSON_ENROLLMENT_ENGLISH_2 "PersonEnrollmentEnglish2"
#define SINGLE_UTTERANCE_WITH_SPECIAL_CHARACTER "SingleUtteranceWithSpecialCharacter"
#define SHORT_SILENCE "ShortSilence"
#define SINGLE_UTTERANCE_WITH_PUNCTUATION "SingleUtteranceWithPunctuation"
#define PROFANTITY_SINGLE_UTTERANCE_ENGLISH_1 "ProfanitySingleUtteranceEnglish1"
#define PROFANITY_SINGLE_UTTERANCE_ENGLISH_2 "ProfanitySingleUtteranceEnglish2"

struct SubscriptionRegion
{
    std::string Key;
    std::string Region;
};

struct Utterance
{
    std::string Text;
    std::string ProfanityRaw;
    std::string ProfanityMasked;
    std::string ProfanityMaskedPattern;
    std::string ProfanityRemoved;
    std::string ProfanityTagged;
    int AudioOffset;
    int AudioDuration;
    int TextOffset;
    int SsmlOffset;
};

struct AudioEntry
{
    std::string FilePath;
    std::string NativeLanguage;
    std::map<std::string, std::vector<Utterance>> Utterances;
};

EXTERN std::map<std::string, AudioEntry> AudioUtterancesMap;
EXTERN std::map<std::string, std::string> DefaultSettingsMap;
EXTERN std::map<std::string, SubscriptionRegion> SubscriptionsRegionsMap;

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

/// <summary>
/// Helper class to match ExceptionWithCallStack for testing using Catch framework
/// </summary>
class ExceptionWithCallStackMatcher : public Catch::MatcherBase<std::exception>
{
private:
    std::string m_expectedString;

public:
    ExceptionWithCallStackMatcher(SPXHR hr)
        : m_expectedString(Microsoft::CognitiveServices::Speech::Impl::stringify(hr))
    {
    }

    bool match(const std::exception& ex) const override
    {
        size_t pos = std::string(ex.what()).find(m_expectedString, 0);
        return pos != std::string::npos;
    }

    virtual std::string describe() const override
    {
        std::ostringstream ss;
        ss << "Exception with HR " << m_expectedString;
        return ss.str();
    }
};

inline ExceptionWithCallStackMatcher HasHR(SPXHR hr)
{
    return ExceptionWithCallStackMatcher(hr);
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

inline void to_json(nlohmann::json& jsonString, const SubscriptionRegion& subscriptionRegion)
{
    jsonString = nlohmann::json{ {KEY, subscriptionRegion.Key},
    {REGION, subscriptionRegion.Region}
    };
}

inline void from_json(const nlohmann::json& jsonString, SubscriptionRegion& subscriptionRegion) {
    try {
        jsonString.at(KEY).get_to(subscriptionRegion.Key);
    }
    catch (std::exception exception) {
        SpxConsoleLogger_Log(LOG_CATEGORY::AZ_LOG_TRACE, __FILE__, "from_json", __LINE__, 0, "SubscriptionRegion - failed to find key");
        throw;
    }
    try {
        jsonString.at(REGION).get_to(subscriptionRegion.Region);
    }
    catch (std::exception exception) {
        SpxConsoleLogger_Log(LOG_CATEGORY::AZ_LOG_TRACE, __FILE__, "from_json", __LINE__, 0, "SubscriptionRegion - failed to find region");
        throw;
    }
}

inline void to_json(nlohmann::json& jsonString, const Utterance& utterance)
{
    jsonString = nlohmann::json{ {UTTERANCE_TEXT,utterance.Text},
    {PROFANITY_RAW, utterance.ProfanityRaw},
    {PROFANITY_MASKED, utterance.ProfanityMasked},
    {PROFANITY_MASKED_PATTERN, utterance.ProfanityMaskedPattern},
    {PROFANITY_REMOVED, utterance.ProfanityRemoved},
    {PROFANITY_TAGGED, utterance.ProfanityTagged},
    {AUDIO_OFFSET, utterance.AudioOffset},
    {AUDIO_DURATION, utterance.AudioDuration},
    {TEXT_OFFSET, utterance.TextOffset},
    {SSML_OFFSET, utterance.SsmlOffset}
    };
}

inline void from_json(const nlohmann::json& jsonString, Utterance& utterance)
{
    try {
        jsonString.at(UTTERANCE_TEXT).get_to(utterance.Text);
    }
    catch (std::exception exception)
    {
        utterance.Text = "";
    }
    try {
        jsonString.at(PROFANITY_RAW).get_to(utterance.ProfanityRaw);
    }
    catch (std::exception exception)
    {
        utterance.ProfanityRaw = "";
    }
    try {
        jsonString.at(PROFANITY_MASKED).get_to(utterance.ProfanityMasked);
    }
    catch (std::exception exception)
    {
        utterance.ProfanityMasked = "";
    }
    try {
        jsonString.at(PROFANITY_MASKED_PATTERN).get_to(utterance.ProfanityMaskedPattern);
    }
    catch (std::exception exception)
    {
        utterance.ProfanityMaskedPattern = "";
    }
    try {
        jsonString.at(PROFANITY_REMOVED).get_to(utterance.ProfanityRemoved);
    }
    catch (std::exception exception)
    {
        utterance.ProfanityRemoved = "";
    }
    try {
        jsonString.at(PROFANITY_TAGGED).get_to(utterance.ProfanityTagged);
    }
    catch (std::exception exception)
    {
        utterance.ProfanityTagged = "";
    }
    try {
        jsonString.at(AUDIO_OFFSET).get_to(utterance.AudioOffset);
    }
    catch (std::exception exception)
    {
        utterance.AudioOffset = -1;
    }
    try {
        jsonString.at(AUDIO_DURATION).get_to(utterance.AudioDuration);
    }
    catch (std::exception exception)
    {
        utterance.AudioDuration = -1;
    }
    try {
        jsonString.at(TEXT_OFFSET).get_to(utterance.TextOffset);
    }
    catch (std::exception exception)
    {
        utterance.TextOffset = -1;
    }
    try {
        jsonString.at(SSML_OFFSET).get_to(utterance.SsmlOffset);
    }
    catch (std::exception exception)
    {
        utterance.SsmlOffset = -1;
    }
}

inline void to_json(nlohmann::json& jsonString, const AudioEntry& audioEntry)
{
    jsonString = nlohmann::json{ {FILE_PATH, audioEntry.FilePath},
    {NATIVE_LANGUAGE, audioEntry.NativeLanguage} };
}

inline void from_json(const nlohmann::json& jsonString, AudioEntry& audioEntry)
{
    try {
        jsonString.at(FILE_PATH).get_to(audioEntry.FilePath);
    }
    catch (nlohmann::json::exception exception)
    {
        audioEntry.FilePath = "";
    }
    try {
        jsonString.at(NATIVE_LANGUAGE).get_to(audioEntry.NativeLanguage);
    }
    catch (nlohmann::json::exception exception)
    {
        audioEntry.NativeLanguage = "";
    }
    try {
        audioEntry.Utterances = jsonString.at(UTTERANCES).get<std::map<std::string, std::vector<Utterance>>>();
    }
    catch (nlohmann::json::exception exception)
    {
        audioEntry.Utterances.clear();
    }
}

class ConfigSettings {
private:
    static nlohmann::json getJson(std::string path)
    {
        nlohmann::json nlohmanJson = nullptr;

        SpxConsoleLogger_Log(LOG_CATEGORY::AZ_LOG_INFO, __FILE__, "getJson", __LINE__, 0, "Loading json from %s", path.c_str());

        if (exists(path))
        {
            std::ifstream testSettingsFile(path);
            try {
                testSettingsFile >> nlohmanJson;
            }
            catch (nlohmann::json::parse_error exception)
            {
                SpxConsoleLogger_Log(LOG_CATEGORY::AZ_LOG_ERROR, __FILE__, "getJson", __LINE__, 0, "json exception from %s - %s", path.c_str(), exception.what());
            }
        }
        else
        {
            SpxConsoleLogger_Log(LOG_CATEGORY::AZ_LOG_INFO, __FILE__, "getJson", __LINE__, 0, "json file %s cannot be found", path.c_str());
        }

        SpxConsoleLogger_Log(LOG_CATEGORY::AZ_LOG_INFO, __FILE__, "getJson", __LINE__, 0, "json loaded: %s", nlohmanJson.dump().c_str());

        return nlohmanJson;
    }

public:
    static void LoadFromJsonFile(std::string rootPathString)
    {
        SpxConsoleLogger_Log(LOG_CATEGORY::AZ_LOG_INFO, __FILE__, "LoadFromJsonFile", __LINE__, 0, "rootPathString is %s", rootPathString.c_str());

        std::string testSubscriptionsRegionsPath = rootPathString + TEST_SUBSCRIPTIONSREGIONS_FILE;
        std::string testAudioUtterancesPath = rootPathString + TEST_AUDIOUTTERANCES_FILE;
        std::string testDefaultsPath = rootPathString + TEST_DEFAULTS_FILE;

        nlohmann::json subscriptionRegionData = getJson(testSubscriptionsRegionsPath);
        if (subscriptionRegionData != nullptr)
        {
            SubscriptionsRegionsMap = subscriptionRegionData.get<std::map<std::string, SubscriptionRegion>>();
        }
        else
        {
            SpxConsoleLogger_Log(LOG_CATEGORY::AZ_LOG_ERROR, __FILE__, "LoadFromJsonFile", __LINE__, 0, "JSON could not be loaded from %s", testSubscriptionsRegionsPath.c_str());
        }

        nlohmann::json defaultsData = getJson(testDefaultsPath);
        if (defaultsData != nullptr)
        {
            DefaultSettingsMap = defaultsData.get<std::map<std::string, std::string>>();
        }
        else
        {
            SpxConsoleLogger_Log(LOG_CATEGORY::AZ_LOG_ERROR, __FILE__, "LoadFromJsonFile", __LINE__, 0, "JSON could not be loaded from %s", testDefaultsPath.c_str());
        }

        SpxConsoleLogger_Log(LOG_CATEGORY::AZ_LOG_INFO, __FILE__, "LoadFromJsonFile", __LINE__, 0, "rootPathString is: %s", rootPathString.c_str());
        DefaultSettingsMap[INPUT_DIR] = rootPathString + DefaultSettingsMap[INPUT_DIR];

        if (!exists(DefaultSettingsMap[INPUT_DIR]))
        {
            SpxConsoleLogger_Log(LOG_CATEGORY::AZ_LOG_ERROR, __FILE__, "LoadFromJsonFile", __LINE__, 0, "input file path %s is invalid", DefaultSettingsMap[INPUT_DIR].c_str());
        }

        SpxConsoleLogger_Log(LOG_CATEGORY::AZ_LOG_INFO, __FILE__, "LoadFromJsonFile", __LINE__, 0, "Setting InputDir to %s", DefaultSettingsMap[INPUT_DIR].c_str());

        nlohmann::json audioUtterancesData = getJson(testAudioUtterancesPath);
        if (audioUtterancesData != nullptr)
        {
            AudioUtterancesMap = audioUtterancesData.get<std::map<std::string, AudioEntry>>();
        }
        else
        {
            SpxConsoleLogger_Log(LOG_CATEGORY::AZ_LOG_ERROR, __FILE__, "LoadFromJsonFile", __LINE__, 0, "JSON could not be loaded from %s", testAudioUtterancesPath.c_str());
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
        | Opt(SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Key, "SpeechSubscriptionKey") // bind variable to a new option, with a hint string
        ["--keySpeech"]    // the option names it will respond to
    ("The subscription key for speech")
        | Opt(SubscriptionsRegionsMap[LANGUAGE_UNDERSTANDING_SUBSCRIPTION].Key, "LuisSubscriptionKey")
        ["--keyLUIS"]
    ("The subscription key for language understanding")
        | Opt(SubscriptionsRegionsMap[DIALOG_SUBSCRIPTION].Key, "keyDialog")
        ["--keyDialog"]
    ("The subscription key for the Speech Channel")
        | Opt(DefaultSettingsMap[ENDPOINT], "endpoint")
        ["--endpoint"]
    ("The endpoint url to test against.")
        | Opt(SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Region, "Region")
        ["--region"]
    ("The region id to be used for subscription and authorization requests")
        | Opt(SubscriptionsRegionsMap[LANGUAGE_UNDERSTANDING_SUBSCRIPTION].Region, "LuisRegionId")
        ["--regionIdLUIS"]
    ("The region id to be used for language understanding subscription and authorization requests")
        | Opt(DefaultSettingsMap[LANGUAGE_UNDERSTANDING_HOME_AUTOMATION_APP_ID], "LuisAppId")
        ["--luisAppId"]
    ("The language understanding app id to be used intent recognition tests")
        | Opt(DefaultSettingsMap[INROOM_AUDIO_ENDPOINT], "InRoomAudioEndpoint")
        ["--InRoomAudioEndpoint"]
    ("The endpoint that in-room tests in intelligent meeting recognizer talks to")
        | Opt(DefaultSettingsMap[ONLINE_AUDIO_ENDPOINT], "OnlineAudioEndpoint")
        ["--OnlineAudioEndpoint"]
    ("The endpoint that on-line tests in intelligent meeting recognizer talks to")
        | Opt(SubscriptionsRegionsMap[CONVERSATION_TRANSCRIPTION_PROD_SUBSCRIPTION].Key, "ConversationTranscriber")
        ["--keyConversationTranscriberPPE"]
    ("The conversation transcriber key")
        | Opt(DefaultSettingsMap[INPUT_DIR], "InputDir")
        ["--inputDir"]
    ("The directory where test input files are placed")
        | Opt(SubscriptionsRegionsMap[DIALOG_SUBSCRIPTION].Region, "DialogRegion")
        ["--dialogRegionId"]
    ("The region id to be used for the Speech Channel Service")
        | Opt(DefaultSettingsMap[DIALOG_FUNCTIONAL_TEST_BOT], "DialogBotSecret")
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
    ("Perform VS Test Adaptor discovery")
        | Opt(SubscriptionsRegionsMap[CONVERSATION_TRANSLATOR_SUBSCRIPTION].Key, "ConversationTranslatorSubscriptionKey")
        ["--keyConversationTranslator"]
    ("The subscription key to use for the conversation translator service")
        | Opt(DefaultSettingsMap[CONVERSATION_TRANSLATOR_HOST], "ConversationTranslatorHost")
        ["--conversationTranslatorHost"]
    ("The conversation host to use")
        | Opt(SubscriptionsRegionsMap[CONVERSATION_TRANSLATOR_SUBSCRIPTION].Region, "ConversationTranslatorRegion")
        ["--conversationTranslatorRegion"]
    ("The conversation region to use")
        | Opt(DefaultSettingsMap[CONVERSATION_TRANSLATOR_SPEECH_ENDPOINT], "ConversationTranslatorSpeechEndpoint")
        ["--conversationTranslatorEndpoint"]
    ("The speech endpoint to use for the conversation translator")
        | Opt(DefaultSettingsMap[CONVERSATION_TRANSLATOR_CLIENTID], "ConversationTranslatorClientId")
        ["--conversationTranslatorClientId"]
    ("The client ID to use for the conversation translator")
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

#define SPXTEST_REQUIRE_NOTHROW( ... ) \
    SPX_TRACE_INFO("SPXTEST_REQUIRE_NOTHROW('%s'): %s(%d):", __SPX_EXPR_AS_STRING(__VA_ARGS__), __FILE__, __LINE__); \
    REQUIRE_NOTHROW(__VA_ARGS__)

#define SPXTEST_REQUIRE_THAT(arg, matcher) \
    SPX_TRACE_INFO("SPXTEST_REQUIRE_THAT('%s', '%s'): %s(%d):", __SPX_EXPR_AS_STRING(arg), __SPX_EXPR_AS_STRING(matcher), __FILE__, __LINE__); \
    REQUIRE_THAT(arg, matcher)
