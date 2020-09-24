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

#include "speechapi_c_diagnostics.h"
#ifndef __SPX_DO_TRACE_IMPL
#define __SPX_DO_TRACE_IMPL diagnostics_log_trace_message
#endif

#include "string_utils.h"
#include "debug_utils.h"
#include "json.h"
#include "exception.h"
#include "spxdebug.h"
#include "platform.h"

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
    EXTERN std::string RnntModelSpec;
    EXTERN std::string RnntTokens;
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
#define CUSTOM_VOICE_SUBSCRIPTION "CustomVoiceSubscription"
#define SPEAKER_RECOGNITION_SUBSCRIPTION "SpeakerRecognitionSubscription"
#define SPEECH_SUBSCRIPTION_WEST_US "SpeechSubscriptionWestUS"

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
#define CUSTOM_VOICE_DEPLOYMENT_ID "CustomVoiceDeploymentId"
#define CUSTOM_VOICE_VOICE_NAME "CustomVoiceVoiceName"
#define SPEAKER_RECOGNITION_ENDPOINT "SpeakerRecognitionEndpoint"
#define OFFLINE_VOICE_PATH "OfflineVoicePath"

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
#define UTTERANCE_SSML "Ssml"
#define VOICE_NAME "VoiceName"
#define AUDIO_OFFSETS "AudioOffsets"
#define AUDIO_DURATION "AudioDuration"
#define TEXT_OFFSETS "TextOffsets"
#define SSML_OFFSETS "SsmlOffsets"
#define WORD_LENGTHS "WordLengths"
#define OFFLINE_SYNTHESIZED_FILE_PATH "OfflineSynthesizedFilePath"

#define SPEAKER_VERIFICATION_ENGLISH "MyVoiceIsMyPassportVerifyMe"
#define SINGLE_UTTERANCE_ENGLISH "SingleUtteranceEnglish"
#define SINGLE_UTTERANCE_CHINESE "SingleUtteranceChinese"
#define MULTIPLE_UTTERANCE_CHINESE "MultipleUtteranceChinese"
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
#define PRONUNCIATION_ASSESSMENT_BAD_PRONUNCIATION "PronunciationAssessmentBadPronunciation"
#define PRONUNCIATION_ASSESSMENT_GOOD_PRONUNCIATION_CHINESE "PronunciationAssessmentGoodPronunciationChinese"

#define SYNTHESIS_WORD_BOUNDARY_UTTERANCE_CHINESE "SynthesisWordBoundaryUtteranceChinese"
#define SYNTHESIS_UTTERANCE_ENGLISH "SynthesisUtteranceEnglish"
#define SYNTHESIS_SHORT_UTTERANCE_CHINESE "SynthesisShortUtteranceChinese"
#define SYNTHESIS_UTTERANCE_CHINESE_1 "SynthesisUtteranceChinese1"
#define SYNTHESIS_UTTERANCE_CHINESE_2 "SynthesisUtteranceChinese2"
#define SYNTHESIS_LONG_UTTERANCE "SynthesisLongUtterance"

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
    std::string Ssml;
    std::string VoiceName;
    std::vector<uint64_t> AudioOffsets;
    int AudioDuration;
    std::vector<int> TextOffsets;
    std::vector<int> SsmlOffsets;
    std::vector<int> WordLengths;
    std::string OfflineSynthesizedFilePath;
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

inline void test_diagnostics_log_trace_message(int level, const char* pszTitle, const char* fileName, const int lineNumber, const char* pszFormat, ...)
{
    char sz[4096];
    va_list argptr;

    va_start(argptr, pszFormat);
    diagnostics_log_format_message(sz, 4096, level, pszTitle, fileName, lineNumber, pszFormat, argptr);
    va_end(argptr);

     fprintf(stderr, "TEST: %s", sz);

     va_start(argptr, pszFormat);
     diagnostics_log_trace_message2(level, pszTitle, fileName, lineNumber, pszFormat, argptr);
     va_end(argptr);
}

#define SPX_TEST_TRACE_ERROR(file, title, line, options, msg, ...) __SPX_TRACE_ERROR((std::string("SPX_TRACE_ERROR: ") + std::string(title)).c_str(), file, line, msg, ##__VA_ARGS__)
#define SPX_TEST_TRACE_INFO(file, title, line, options, msg, ...)  __SPX_TRACE_INFO((std::string("SPX_TRACE_INFO: ") + std::string(title)).c_str(), file, line, msg, ##__VA_ARGS__)

inline void GetKeyValue(const char* key, std::string &value, nlohmann::json &data, const char* file, int line)
{
    try
    {
        value = data.at(key).get<std::string>();
    }
    catch (nlohmann::json::type_error& e)
    {
        SPX_TEST_TRACE_ERROR(file, "LoadFromJsonFile", line, 0, "Error: exception in AT, %s.", e.what());
    }
}

class StringComparisions
{
public:
    static bool AssertFuzzyMatch(const std::string recieved, const std::string expected, size_t deltaPercentage = 10, size_t minAllowedMismatchCount = 1)
    {
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

namespace Catch
{
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
        {}

        ExceptionWithCallStackMatcher(const std::string& contains)
            : m_expectedString(contains)
        {}

        virtual bool match(const std::exception& ex) const override
        {
            size_t pos = std::string(ex.what()).find(m_expectedString, 0);
            return pos != std::string::npos;
        }

        virtual std::string describe() const override
        {
            std::ostringstream ss;
            ss << "exception with HR " << m_expectedString;
            return ss.str();
        }
    };

    inline ExceptionWithCallStackMatcher HasHR(SPXHR hr)
    {
        return ExceptionWithCallStackMatcher(hr);
    }

    inline ExceptionWithCallStackMatcher MessageContains(const std::string& str)
    {
        return ExceptionWithCallStackMatcher(str);
    }

    /// <summary>
    /// Helper class to integrate fuzzy string matcher into the Catch framework. This integrates with the Catch
    /// framework and gives you richer error messages in case of assertion failures
    /// </summary>
    class FuzzyStringMatcher : public Catch::MatcherBase<std::string>
    {
    private:
        const std::string m_expectedString;
        const size_t m_deltaPercentage;
        const size_t m_minMismatachCount;

    public:
        FuzzyStringMatcher(const std::string& expected, size_t deltaPercentage, size_t minAllowedMismatchCount)
            : m_expectedString(expected), m_deltaPercentage(deltaPercentage), m_minMismatachCount(minAllowedMismatchCount)
        {}

        virtual bool match(const std::string& str) const override
        {
            return ::StringComparisions::AssertFuzzyMatch(str, m_expectedString, m_deltaPercentage, m_minMismatachCount);
        }

        virtual std::string describe() const override
        {
            std::ostringstream oss;
            oss << "fuzzy string match against '" << m_expectedString << "' "
                << "with " << m_deltaPercentage << "% and "
                << m_minMismatachCount << " allowed mismatch";
            return oss.str();
        }
    };

    /// <summary>
    /// Helper method to make working with the fuzzy matcher easier. This allows you do assert like this:
    /// REQUIRE_THAT(finalRecognition, Catch::FuzzyMatch("This is a short test"));
    /// Should this fail, the Catch framework will automatically show you a string describing the error
    /// </summary>
    /// <param name="expected">The expected string</param>
    /// <param name="deltaPercentage">(Optional) The percentage difference allowed (0-100)</param>
    /// <param name="minAllowedMismatchCount">(Optional) The minimum allowed mismatch count</param>
    /// <returns>An instance of the fuzzy matcher</returns>
    inline FuzzyStringMatcher FuzzyMatch(const std::string& expected, size_t deltaPercentage = 10, size_t minAllowedMismatchCount = 1)
    {
        return FuzzyStringMatcher(expected, deltaPercentage, minAllowedMismatchCount);
    }
}

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
    catch (std::exception& exception) {
        SPX_TEST_TRACE_ERROR(__FILE__, "from_json", __LINE__, 0, "SubscriptionRegion - failed to find key, %s", exception.what());
        throw;
    }
    try {
        jsonString.at(REGION).get_to(subscriptionRegion.Region);
    }
    catch (std::exception& exception) {
        SPX_TEST_TRACE_ERROR(__FILE__, "from_json", __LINE__, 0, "SubscriptionRegion - failed to find region, %s", exception.what());
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
    {UTTERANCE_SSML, utterance.Ssml},
    {VOICE_NAME, utterance.VoiceName},
    {AUDIO_OFFSETS, utterance.AudioOffsets},
    {AUDIO_DURATION, utterance.AudioDuration},
    {TEXT_OFFSETS, utterance.TextOffsets},
    {SSML_OFFSETS, utterance.SsmlOffsets},
    {WORD_LENGTHS, utterance.WordLengths}
    };
}

inline void from_json(const nlohmann::json& jsonString, Utterance& utterance)
{
    if (jsonString.contains(UTTERANCE_TEXT)) {
        jsonString.at(UTTERANCE_TEXT).get_to(utterance.Text);
    }
    else {
        utterance.Text = "";
    }

    if (jsonString.contains(PROFANITY_RAW)) {
        jsonString.at(PROFANITY_RAW).get_to(utterance.ProfanityRaw);
    }
    else {
        utterance.ProfanityRaw = "";
    }

    if (jsonString.contains(PROFANITY_MASKED)) {
        jsonString.at(PROFANITY_MASKED).get_to(utterance.ProfanityMasked);
    }
    else {
        utterance.ProfanityMasked = "";
    }

    if (jsonString.contains(PROFANITY_MASKED_PATTERN)) {
        jsonString.at(PROFANITY_MASKED_PATTERN).get_to(utterance.ProfanityMaskedPattern);
    }
    else {
        utterance.ProfanityMaskedPattern = "";
    }

    if (jsonString.contains(PROFANITY_REMOVED)) {
        jsonString.at(PROFANITY_REMOVED).get_to(utterance.ProfanityRemoved);
    }
    else {
        utterance.ProfanityRemoved = "";
    }

    if (jsonString.contains(PROFANITY_TAGGED)) {
        jsonString.at(PROFANITY_TAGGED).get_to(utterance.ProfanityTagged);
    }
    else {
        utterance.ProfanityTagged = "";
    }

    if (jsonString.contains(UTTERANCE_SSML)) {
        jsonString.at(UTTERANCE_SSML).get_to(utterance.Ssml);
    }
    else {
        utterance.Ssml = "";
    }

    if (jsonString.contains(VOICE_NAME)) {
        jsonString.at(VOICE_NAME).get_to(utterance.VoiceName);
    }
    else {
        utterance.VoiceName = "";
    }

    if (jsonString.contains(AUDIO_OFFSETS)) {
        jsonString.at(AUDIO_OFFSETS).get_to(utterance.AudioOffsets);
    }
    else {
        utterance.AudioOffsets = std::vector<uint64_t>();
    }

    if (jsonString.contains(AUDIO_DURATION)) {
        jsonString.at(AUDIO_DURATION).get_to(utterance.AudioDuration);
    }
    else {
        utterance.AudioDuration = -1;
    }

    if (jsonString.contains(TEXT_OFFSETS)) {
        jsonString.at(TEXT_OFFSETS).get_to(utterance.TextOffsets);
    }
    else {
        utterance.TextOffsets = std::vector<int>();
    }

    if (jsonString.contains(SSML_OFFSETS)) {
        jsonString.at(SSML_OFFSETS).get_to(utterance.SsmlOffsets);
    }
    else {
        utterance.SsmlOffsets = std::vector<int>();
    }

    if (jsonString.contains(WORD_LENGTHS)) {
        jsonString.at(WORD_LENGTHS).get_to(utterance.WordLengths);
    }
    else {
        utterance.WordLengths = std::vector<int>();
    }

    if (jsonString.contains(OFFLINE_SYNTHESIZED_FILE_PATH)) {
        jsonString.at(OFFLINE_SYNTHESIZED_FILE_PATH).get_to(utterance.OfflineSynthesizedFilePath);
    }
    else {
        utterance.OfflineSynthesizedFilePath = "";
    }
}

inline void to_json(nlohmann::json& jsonString, const AudioEntry& audioEntry)
{
    jsonString = nlohmann::json{ {FILE_PATH, audioEntry.FilePath},
    {NATIVE_LANGUAGE, audioEntry.NativeLanguage} };
}

inline void from_json(const nlohmann::json& jsonString, AudioEntry& audioEntry)
{
    if (jsonString.contains(FILE_PATH)) {
        jsonString.at(FILE_PATH).get_to(audioEntry.FilePath);
    }
    else {
        audioEntry.FilePath = "";
    }

    if (jsonString.contains(NATIVE_LANGUAGE)) {
        jsonString.at(NATIVE_LANGUAGE).get_to(audioEntry.NativeLanguage);
    }
    else {
        audioEntry.NativeLanguage = "";
    }

    if (jsonString.contains(UTTERANCES)) {
        audioEntry.Utterances = jsonString.at(UTTERANCES).get<std::map<std::string, std::vector<Utterance>>>();
    }
    else {
        audioEntry.Utterances.clear();
    }
}

class ConfigSettings {
private:
    static nlohmann::json getJson(std::string path)
    {
        nlohmann::json nlohmanJson = nullptr;

        SPX_TEST_TRACE_INFO(__FILE__, "getJson", __LINE__, 0, "Loading json from %s", path.c_str());

        if (exists(path))
        {
            std::ifstream testSettingsFile(path);
            try {
                testSettingsFile >> nlohmanJson;
            }
            catch (nlohmann::json::parse_error& exception)
            {
                SPX_TEST_TRACE_ERROR(__FILE__, "getJson", __LINE__, 0, "json exception from %s - %s", path.c_str(), exception.what());
            }
        }
        else
        {
            SPX_TEST_TRACE_INFO(__FILE__, "getJson", __LINE__, 0, "json file %s cannot be found", path.c_str());
        }

        SPX_TEST_TRACE_INFO(__FILE__, "getJson", __LINE__, 0, "json loaded: %s", nlohmanJson.dump().c_str());

        return nlohmanJson;
    }

public:
    static void LoadFromJsonFile(std::string rootPathString)
    {
        SPX_TEST_TRACE_INFO(__FILE__, "LoadFromJsonFile", __LINE__, 0, "rootPathString is %s", rootPathString.c_str());

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
            SPX_TEST_TRACE_ERROR(__FILE__, "LoadFromJsonFile", __LINE__, 0, "JSON could not be loaded from %s", testSubscriptionsRegionsPath.c_str());
        }

        nlohmann::json defaultsData = getJson(testDefaultsPath);
        if (defaultsData != nullptr)
        {
            DefaultSettingsMap = defaultsData.get<std::map<std::string, std::string>>();
        }
        else
        {
            SPX_TEST_TRACE_ERROR(__FILE__, "LoadFromJsonFile", __LINE__, 0, "JSON could not be loaded from %s", testDefaultsPath.c_str());
        }

        SPX_TEST_TRACE_INFO(__FILE__, "LoadFromJsonFile", __LINE__, 0, "rootPathString is: %s", rootPathString.c_str());
        DefaultSettingsMap[INPUT_DIR] = rootPathString + DefaultSettingsMap[INPUT_DIR];

        if (!exists(DefaultSettingsMap[INPUT_DIR]))
        {
            SPX_TEST_TRACE_ERROR(__FILE__, "LoadFromJsonFile", __LINE__, 0, "input file path %s is invalid", DefaultSettingsMap[INPUT_DIR].c_str());
        }

        SPX_TEST_TRACE_INFO(__FILE__, "LoadFromJsonFile", __LINE__, 0, "Setting InputDir to %s", DefaultSettingsMap[INPUT_DIR].c_str());

        nlohmann::json audioUtterancesData = getJson(testAudioUtterancesPath);
        if (audioUtterancesData != nullptr)
        {
            AudioUtterancesMap = audioUtterancesData.get<std::map<std::string, AudioEntry>>();
        }
        else
        {
            SPX_TEST_TRACE_ERROR(__FILE__, "LoadFromJsonFile", __LINE__, 0, "JSON could not be loaded from %s", testAudioUtterancesPath.c_str());
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
        | Opt(SubscriptionsRegionsMap[CONVERSATION_TRANSCRIPTION_PPE_SUBSCRIPTION].Key, "ConversationTranscriberPPEKey")
            ["--keyConversationTranscriberPPE"]
            ("The conversation transcriber PPE key")
        | Opt(SubscriptionsRegionsMap[CONVERSATION_TRANSCRIPTION_PPE_SUBSCRIPTION].Region, "ConversationTranscriberPPERegion")
            ["--regionConversationTranscriberPPE"]
            ("The conversation transcriber PPE key")
        | Opt(SubscriptionsRegionsMap[CONVERSATION_TRANSCRIPTION_PROD_SUBSCRIPTION].Key, "ConversationTranscriberProdKey")
            ["--keyConversationTranscriberProd"]
            ("The conversation transcriber PROD key")
        | Opt(SubscriptionsRegionsMap[CONVERSATION_TRANSCRIPTION_PROD_SUBSCRIPTION].Region, "ConversationTranscriberProdRegion")
            ["--regionConversationTranscriberProd"]
            ("The conversation transcriber PROD region")
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
        | Opt(Config::RnntModelSpec, "RnntModelSpec")
            ["--rnntModelSpec"]
            ("The specification for RNN-T model used in tests.")
        | Opt(Config::RnntTokens, "RnntTokens")
            ["--rnntTokens"]
            ("The path to tokens file used for RNN-T engine in tests.")
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

//This is how you set up tests for carbon, always use SPXTEST_CASE_BEGIN / SPXTEST_CASE_END.
//This ensures no exceptions are thrown. In case there is exception, it fails the test and do a memory dump of all the accumulated logging to stderr.
#define SPXTEST_CASE_BEGIN(...)              \
        INTERNAL_CATCH_TESTCASE(__VA_ARGS__) \
        { try      {                         \

#define SPXTEST_CASE_END(...)                                        \
} catch (const std::exception& e)                                    \
{                                                                    \
    SPX_TRACE_ERROR("SPXTEST_CASE_END %s(%d):", __FILE__, __LINE__); \
    diagnostics_log_memory_dump_to_file(nullptr, 0);                 \
    std::string msg{"Failed due to exception '"};                    \
    msg += e.what();                                                 \
    msg += "'";                                                      \
    FAIL(msg.c_str());                                               \
} }

#define SPXTEST_SECTION(msg) SECTION(msg) if ([=](){ \
    SPX_TRACE_INFO("SPXTEST_SECTION('%s') %s(%d)", msg, __FILE__, __LINE__);  \
    return 1; }())

#define SPXTEST_GIVEN(msg) GIVEN(msg) if ([=](){ \
    SPX_TRACE_INFO("SPXTEST_GIVEN('%s') %s(%d):", msg, __FILE__, __LINE__); \
    return 1; }())

#define SPXTEST_WHEN(msg) WHEN(msg) if ([=](){ \
    SPX_TRACE_INFO("SPXTEST_WHEN('%s') %s(%d):", msg, __FILE__, __LINE__); \
    return 1; }())

#define SPXTEST_CHECK( ... ) ([&](){ \
    try { auto isTrue = !!(__VA_ARGS__); \
          SPX_TRACE_INFO_IF(isTrue, "SPXTEST_CHECK('%s'): %s(%d): PASSED:", __SPX_EXPR_AS_STRING(__VA_ARGS__), __FILE__, __LINE__); \
          SPX_TRACE_ERROR_IF(!isTrue, "SPXTEST_CHECK('%s'): %s(%d): FAILED:", __SPX_EXPR_AS_STRING(__VA_ARGS__), __FILE__, __LINE__); \
          SPX_IFTRUE(!isTrue, diagnostics_log_memory_dump_to_file(nullptr, 0)); \
    } catch (...) { \
          SPX_TRACE_ERROR("SPXTEST_CHECK('%s'): FAILED: %s(%d) w/Exception:", __SPX_EXPR_AS_STRING(__VA_ARGS__), __FILE__, __LINE__); \
    } CHECK(__VA_ARGS__); }())

// ISSUE: need to try and not evaluate __VA_ARGS__ multiple times...
// ISSUE: also ... need to figure out why memory log didn't emit when cxx_api_tests failed last run
#define SPXTEST_REQUIRE( ... ) ([&](){ \
    try { auto isTrue = !!(__VA_ARGS__); \
          SPX_TRACE_INFO_IF(isTrue, "SPXTEST_REQUIRE('%s'): %s(%d): PASSED:", __SPX_EXPR_AS_STRING(__VA_ARGS__), __FILE__, __LINE__); \
          SPX_TRACE_ERROR_IF(!isTrue, "SPXTEST_REQUIRE('%s'): %s(%d): FAILED:", __SPX_EXPR_AS_STRING(__VA_ARGS__), __FILE__, __LINE__); \
          SPX_IFTRUE(!isTrue, diagnostics_log_memory_dump_to_file(nullptr, 0)); \
    } catch (...) { \
          SPX_TRACE_ERROR("SPXTEST_REQUIRE('%s'): %s(%d): FAILED: w/Exception:", __SPX_EXPR_AS_STRING(__VA_ARGS__), __FILE__, __LINE__); \
    } REQUIRE(__VA_ARGS__); }())

#define SPXTEST_CHECK_FALSE( ... ) ([&](){ \
    try { auto isFalse = !(__VA_ARGS__); \
          SPX_TRACE_INFO_IF(isFalse, "SPXTEST_CHECK_FALSE('%s'): %s(%d): PASSED:", __SPX_EXPR_AS_STRING(__VA_ARGS__), __FILE__, __LINE__); \
          SPX_TRACE_ERROR_IF(!isFalse, "SPXTEST_CHECK_FALSE('%s'): %s(%d): FAILED:", __SPX_EXPR_AS_STRING(__VA_ARGS__), __FILE__, __LINE__); \
          SPX_IFTRUE(!isFalse, diagnostics_log_memory_dump_to_file(nullptr, 0)); \
    } catch (...) { \
          SPX_TRACE_ERROR("SPXTEST_CHECK_FALSE('%s'): %s(%d): FAILED: w/Exception:", __SPX_EXPR_AS_STRING(__VA_ARGS__), __FILE__, __LINE__); \
    } CHECK_FALSE(__VA_ARGS__); }())

#define SPXTEST_REQUIRE_FALSE( ... ) ([&](){ \
    try { auto isFalse = !(__VA_ARGS__); \
          SPX_TRACE_INFO_IF(isFalse, "SPXTEST_REQUIRE_FALSE('%s'): %s(%d): PASSED:", __SPX_EXPR_AS_STRING(__VA_ARGS__), __FILE__, __LINE__); \
          SPX_TRACE_ERROR_IF(!isFalse, "SPXTEST_REQUIRE_FALSE('%s'): %s(%d): FAILED:", __SPX_EXPR_AS_STRING(__VA_ARGS__), __FILE__, __LINE__); \
          SPX_IFTRUE(!isFalse, diagnostics_log_memory_dump_to_file(nullptr, 0)); \
    } catch (...) { \
          SPX_TRACE_ERROR("SPXTEST_REQUIRE_FALSE('%s'): %s(%d): FAILED: w/Exception:", __SPX_EXPR_AS_STRING(__VA_ARGS__), __FILE__, __LINE__); \
    } REQUIRE_FALSE(__VA_ARGS__); }())

#define SPXTEST_CHECK_THAT(arg, matcher) ([&](){ \
    try { auto isTrue = !!(matcher.match(arg)); \
          SPX_TRACE_INFO_IF(isTrue, "SPXTEST_CHECK_THAT('%s', '%s'): %s(%d): PASSED:", __SPX_EXPR_AS_STRING(arg), __SPX_EXPR_AS_STRING(matcher), __FILE__, __LINE__); \
          SPX_TRACE_ERROR_IF(!isTrue, "SPXTEST_CHECK_THAT('%s', '%s'): %s(%d): FAILED:", __SPX_EXPR_AS_STRING(arg), __SPX_EXPR_AS_STRING(matcher), __FILE__, __LINE__); \
          SPX_IFTRUE(!isTrue, diagnostics_log_memory_dump_to_file(nullptr, 0)); \
    } catch (...) { \
          SPX_TRACE_ERROR("SPXTEST_CHECK_THAT('%s', '%s'): %s(%d): FAILED: w/Exception:", __SPX_EXPR_AS_STRING(arg), __SPX_EXPR_AS_STRING(matcher), __FILE__, __LINE__); \
    } CHECK_THAT(arg, matcher); }())

#define SPXTEST_REQUIRE_THAT(arg, matcher) ([&](){ \
    try { auto isTrue = !!(matcher.match(arg)); \
          SPX_TRACE_INFO_IF(isTrue, "SPXTEST_REQUIRE_THAT('%s', '%s'): %s(%d): PASSED:", __SPX_EXPR_AS_STRING(arg), __SPX_EXPR_AS_STRING(matcher), __FILE__, __LINE__); \
          SPX_TRACE_ERROR_IF(!isTrue, "SPXTEST_REQUIRE_THAT('%s', '%s'): %s(%d): FAILED:", __SPX_EXPR_AS_STRING(arg), __SPX_EXPR_AS_STRING(matcher), __FILE__, __LINE__); \
          SPX_IFTRUE(!isTrue, diagnostics_log_memory_dump_to_file(nullptr, 0)); \
    } catch (...) { \
          SPX_TRACE_INFO("SPXTEST_REQUIRE_THAT('%s', '%s'): %s(%d): FAILED: w/Exception:", __SPX_EXPR_AS_STRING(arg), __SPX_EXPR_AS_STRING(matcher), __FILE__, __LINE__); \
    } REQUIRE_THAT(arg, matcher); }())

#define SPXTEST_CAPTURE(msg) ([&](){ \
        SPX_TRACE_INFO("SPXTEST_CAPTURE(%s): %s", #msg, ::Catch::Detail::stringify(msg).c_str()); \
    }())

#define SPXTEST_FAIL(...) ([&](){ \
        SPXTEST_CAPTURE(__VA_ARGS__); \
        FAIL(__VA_ARGS__); \
    }())

#define SPXTEST_CHECK_NOTHROW( ... ) \
    SPX_TRACE_INFO("SPXTEST_CHECK_NOTHROW('%s'): %s(%d):", __SPX_EXPR_AS_STRING(__VA_ARGS__), __FILE__, __LINE__); \
    CHECK_NOTHROW(__VA_ARGS__)

#define SPXTEST_REQUIRE_NOTHROW( ... ) \
    SPX_TRACE_INFO("SPXTEST_REQUIRE_NOTHROW('%s'): %s(%d):", __SPX_EXPR_AS_STRING(__VA_ARGS__), __FILE__, __LINE__); \
    REQUIRE_NOTHROW(__VA_ARGS__)

inline std::string SpxGetTestTrafficType(const char* file, int line)
{
    char trafficType[1000];
    if (strrchr(file, '/') != nullptr) file = strrchr(file, '/') + 1;
    if (strrchr(file, '\\') != nullptr) file = strrchr(file, '\\') + 1;

    auto buildid = PAL::SpxGetEnv("BUILD_BUILDID", "dev");

    snprintf(trafficType, sizeof(trafficType), "%s(%d)%%20bld(%s)", file, line, buildid.c_str());
    return trafficType;
}
