//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "stdafx.h"
#include <map>
#include <queue>
#include "test_utils.h"
#include "file_utils.h"
#include "recognizer_utils.h"

// #define USEFIDDLERPROXY

TEST_CASE("RecognizeOnceAsync with compressed audio works", "[api][cxx]")
{
    SPXTEST_SECTION("English Speech Recognition works")
    {
        SPXTEST_REQUIRE(exists(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH)));

        auto sc = !DefaultSettingsMap[ENDPOINT].empty()
            ? SpeechConfig::FromEndpoint(DefaultSettingsMap[ENDPOINT], SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Key)
            : SpeechConfig::FromSubscription(SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Key, SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Region);
        sc->SetServiceProperty("TrafficType", SpxGetTestTrafficType(__FILE__, __LINE__), ServicePropertyChannel::UriQueryParameter);

        auto audioConfig = AudioConfig::FromWavFileInput(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH));
        EnableSilkAudioCompression(audioConfig);

#ifdef USEFIDDLERPROXY
        SetDefaultFiddlerProxy(audioConfig);
#endif

        auto recognizer = SpeechRecognizer::FromConfig(sc, "en-US", audioConfig);

        auto result = recognizer->RecognizeOnceAsync().get();
        SPXTEST_REQUIRE(result != nullptr);
        SPXTEST_REQUIRE(result->Reason == ResultReason::RecognizedSpeech);
        SPXTEST_REQUIRE(StringComparisions::AssertFuzzyMatch(result->Text, AudioUtterancesMap[SINGLE_UTTERANCE_ENGLISH].Utterances["en-US"][0].Text));
    }

    SPXTEST_SECTION("German Speech Recognition works")
    {
        SPXTEST_REQUIRE(exists(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_GERMAN)));

        auto sc = !DefaultSettingsMap[ENDPOINT].empty()
            ? SpeechConfig::FromEndpoint(DefaultSettingsMap[ENDPOINT], SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Key)
            : SpeechConfig::FromSubscription(SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Key, SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Region);
        sc->SetServiceProperty("TrafficType", SpxGetTestTrafficType(__FILE__, __LINE__), ServicePropertyChannel::UriQueryParameter);

        auto audioConfig = AudioConfig::FromWavFileInput(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_GERMAN));

        EnableSilkAudioCompression(audioConfig);

#ifdef USEFIDDLERPROXY
        SetDefaultFiddlerProxy(audioConfig);
#endif

        auto recognizer = SpeechRecognizer::FromConfig(sc, "de-DE", audioConfig);

        auto result = recognizer->RecognizeOnceAsync().get();
        SPXTEST_REQUIRE(result != nullptr);
        SPXTEST_REQUIRE(result->Reason == ResultReason::RecognizedSpeech);
        SPXTEST_REQUIRE(StringComparisions::AssertFuzzyMatch(result->Text, AudioUtterancesMap[SINGLE_UTTERANCE_GERMAN].Utterances["de"][0].Text));
    }

    SPXTEST_SECTION("Two recognizers in parallel work")
    {
        SPXTEST_REQUIRE(exists(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH)));

        auto sc = !DefaultSettingsMap[ENDPOINT].empty()
            ? SpeechConfig::FromEndpoint(DefaultSettingsMap[ENDPOINT], SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Key)
            : SpeechConfig::FromSubscription(SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Key, SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Region);
        sc->SetServiceProperty("TrafficType", SpxGetTestTrafficType(__FILE__, __LINE__), ServicePropertyChannel::UriQueryParameter);

        auto audioConfig = AudioConfig::FromWavFileInput(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH));

        EnableSilkAudioCompression(audioConfig);

#ifdef USEFIDDLERPROXY
        SetDefaultFiddlerProxy(audioConfig);
#endif

        auto recognizer1 = SpeechRecognizer::FromConfig(sc, "en-US", audioConfig);
        auto recognizer2 = SpeechRecognizer::FromConfig(sc, "en-US", audioConfig);

        auto result1Future = recognizer1->RecognizeOnceAsync();
        auto result2Future = recognizer2->RecognizeOnceAsync();

        auto result1 = result1Future.get();
        auto result2 = result2Future.get();

        SPXTEST_REQUIRE(result1 != nullptr);
        SPXTEST_REQUIRE(result2 != nullptr);
        SPXTEST_REQUIRE(result1->Reason == ResultReason::RecognizedSpeech);
        SPXTEST_REQUIRE(result2->Reason == ResultReason::RecognizedSpeech);

        SPXTEST_REQUIRE(StringComparisions::AssertFuzzyMatch(result1->Text, AudioUtterancesMap[SINGLE_UTTERANCE_ENGLISH].Utterances["en-US"][0].Text));
        SPXTEST_REQUIRE(StringComparisions::AssertFuzzyMatch(result2->Text, AudioUtterancesMap[SINGLE_UTTERANCE_ENGLISH].Utterances["en-US"][0].Text));
    }
}

TEST_CASE("ContinuousRecognitionAsync with compressed audio works", "[api][cxx]")
{
    SPX_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    UseMocks(false);
    SPXTEST_REQUIRE(exists(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH)));

    auto recognizer = CreateRecognizers<SpeechRecognizer>(SpxGetTestTrafficType(__FILE__, __LINE__), ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH), true);

    // a normal case.
    SPXTEST_SECTION("start and stop once")
    {
        auto result = make_shared<RecoPhrases>();
        recognizer->StartContinuousRecognitionAsync().get();

        ConnectCallbacks(recognizer.get(), result);
        WaitForResult(result->ready.get_future(), WAIT_FOR_RECO_RESULT_TIME * 20);
        recognizer->StopContinuousRecognitionAsync().get();
        SPXTEST_REQUIRE(!result->phrases.empty());
        SPXTEST_REQUIRE(StringComparisions::AssertFuzzyMatch(result->phrases[0].Text, AudioUtterancesMap[SINGLE_UTTERANCE_ENGLISH].Utterances["en-US"][0].Text));
    }

    // Another normal case. no stop is a valid user case.
    SPXTEST_SECTION("start without stop")
    {
        auto result = make_shared<RecoPhrases>();
        recognizer->StartContinuousRecognitionAsync().get();

        ConnectCallbacks(recognizer.get(), result);
        WaitForResult(result->ready.get_future(), WAIT_FOR_RECO_RESULT_TIME);
        SPXTEST_REQUIRE(!result->phrases.empty());
        SPXTEST_REQUIRE(StringComparisions::AssertFuzzyMatch(result->phrases[0].Text, AudioUtterancesMap[SINGLE_UTTERANCE_ENGLISH].Utterances["en-US"][0].Text));
    }

}

#include <chrono>
TEST_CASE("ContinuousRecognitionAsync with compressed audio - multiple recognizers in parallel", "[api][cxx]")
{
    SPX_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    UseMocks(false);
    const auto c_utteranceId = SINGLE_UTTERANCE_ENGLISH;
    auto audioFile = ROOT_RELATIVE_PATH(c_utteranceId);

    SPXTEST_REQUIRE(exists(audioFile));

    auto recognizer1 = CreateRecognizers<SpeechRecognizer>(SpxGetTestTrafficType(__FILE__, __LINE__), audioFile, true);
    auto recognizer2 = CreateRecognizers<SpeechRecognizer>(SpxGetTestTrafficType(__FILE__, __LINE__), audioFile, true);
    auto recognizer3 = CreateRecognizers<SpeechRecognizer>(SpxGetTestTrafficType(__FILE__, __LINE__), audioFile, true);
    auto recognizer4 = CreateRecognizers<SpeechRecognizer>(SpxGetTestTrafficType(__FILE__, __LINE__), audioFile, true);

    auto result1 = make_shared<RecoPhrases>();
    auto result2 = make_shared<RecoPhrases>();
    auto result3 = make_shared<RecoPhrases>();
    auto result4 = make_shared<RecoPhrases>();

    auto expectedUtterances = AudioUtterancesMap[c_utteranceId].Utterances["en-US"];

    SPXTEST_SECTION("two parallel runs")
    {
        recognizer1->StartContinuousRecognitionAsync().get();
        recognizer2->StartContinuousRecognitionAsync().get();

        ConnectCallbacks(recognizer1.get(), result1);
        ConnectCallbacks(recognizer2.get(), result2);

        WaitForResult(result1->ready.get_future(), WAIT_FOR_RECO_RESULT_TIME);
        WaitForResult(result2->ready.get_future(), WAIT_FOR_RECO_RESULT_TIME);

        SPXTEST_REQUIRE(!result1->phrases.empty());
        SPXTEST_REQUIRE(!result2->phrases.empty());

        SPXTEST_REQUIRE(StringComparisions::AssertFuzzyMatch(result1->phrases[0].Text, expectedUtterances[0].Text));
        SPXTEST_REQUIRE(StringComparisions::AssertFuzzyMatch(result2->phrases[0].Text, expectedUtterances[0].Text));
    }

    SPXTEST_SECTION("four parallel runs")
    {
        recognizer1->StartContinuousRecognitionAsync().get();
        recognizer2->StartContinuousRecognitionAsync().get();
        recognizer3->StartContinuousRecognitionAsync().get();
        recognizer4->StartContinuousRecognitionAsync().get();

        ConnectCallbacks(recognizer1.get(), result1);
        ConnectCallbacks(recognizer2.get(), result2);
        ConnectCallbacks(recognizer3.get(), result3);
        ConnectCallbacks(recognizer4.get(), result4);

        WaitForResult(result1->ready.get_future(), WAIT_FOR_RECO_RESULT_TIME);
        WaitForResult(result2->ready.get_future(), WAIT_FOR_RECO_RESULT_TIME);
        WaitForResult(result3->ready.get_future(), WAIT_FOR_RECO_RESULT_TIME);
        WaitForResult(result4->ready.get_future(), WAIT_FOR_RECO_RESULT_TIME);

        SPXTEST_REQUIRE(!result1->phrases.empty());
        SPXTEST_REQUIRE(!result2->phrases.empty());
        SPXTEST_REQUIRE(!result3->phrases.empty());
        SPXTEST_REQUIRE(!result4->phrases.empty());

        SPXTEST_REQUIRE(StringComparisions::AssertFuzzyMatch(result1->phrases[0].Text, expectedUtterances[0].Text));
        SPXTEST_REQUIRE(StringComparisions::AssertFuzzyMatch(result2->phrases[0].Text, expectedUtterances[0].Text));
        SPXTEST_REQUIRE(StringComparisions::AssertFuzzyMatch(result3->phrases[0].Text, expectedUtterances[0].Text));
        SPXTEST_REQUIRE(StringComparisions::AssertFuzzyMatch(result4->phrases[0].Text, expectedUtterances[0].Text));
    }
}

TEST_CASE("ContinuousRecognitionAsync with compressed audio - long dictation", "[!hide][api][cxx]")
{
    // This works as a semi - manual test for reconnecting after a disconnect :
    // This test takes about 1 minute to complete.
    //    1. Test : Change the call to create recognizer with a proxy :
    //      auto recognizer = CreateRecognizers<SpeechRecognizer>(ROOT_RELATIVE_PATH(c_utteranceId), true, __true__);
    //    2. Fiddler: Start Fiddler(optional : press F12 to stop automatic tracking of applications and clear the window)
    //    3. Test : Run the test
    //    4. Fiddler : After ~15s right click on the session showing in fiddler and pick "Abort Session"
    //    5. Fiddler : Check a new session was created and it keeps going.
    //    6. Test : test passes.Some times it may fail especially if ran under debugger but it is the same behavior as in the case without audio compression

    SPX_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    UseMocks(false);
    const auto c_utteranceId = MULTIPLE_UTTERANCE_ENGLISH;

    SPXTEST_REQUIRE(exists(ROOT_RELATIVE_PATH(c_utteranceId)));

    auto recognizer = CreateRecognizers<SpeechRecognizer>(SpxGetTestTrafficType(__FILE__, __LINE__), ROOT_RELATIVE_PATH(c_utteranceId), true);
    auto expectedUtterances = AudioUtterancesMap[c_utteranceId].Utterances["en-US"];
    recognizer->StartContinuousRecognitionAsync().get();

    auto result = make_shared<RecoPhrases>();
    ConnectCallbacks(recognizer.get(), result);

    // If running under debugger increase this to 20, it takes about 350s to run.
    WaitForResult(result->ready.get_future(), 5 * WAIT_FOR_RECO_RESULT_TIME);

    SPXTEST_REQUIRE(!result->phrases.empty());
    SPXTEST_REQUIRE(StringComparisions::AssertFuzzyMatch(result->phrases[0].Text, expectedUtterances[0].Text));

    auto lastResultIdx = result->phrases.size() - 2;
    auto lastExpectedResultIdx = expectedUtterances.size() - 1;

    SPXTEST_REQUIRE(lastResultIdx == lastExpectedResultIdx);
    for (int idx = 1; idx <= (int)lastResultIdx; idx++)
    {
        SPXTEST_REQUIRE(StringComparisions::AssertFuzzyMatch(result->phrases[idx].Text, expectedUtterances[idx].Text, 20));
    }

}
