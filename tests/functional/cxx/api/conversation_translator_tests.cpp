//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "conversation_translator_utils.h"
#include "recognizer_utils.h"
#include "guid_utils.h"

using namespace std;
using namespace Microsoft::CognitiveServices::Speech::Impl;
using namespace Microsoft::CognitiveServices::Speech::IntegrationTests;
using namespace Microsoft::CognitiveServices::Speech::Transcription;


TEST_CASE("Conversation host without translations", "[api][cxx][conversation_translator][cxx_conversation][no_translate]")
{
    auto speechConfig = CreateConfig("en-US", {});
    auto conversation = Conversation::CreateConversationAsync(speechConfig).get();
    conversation->DeleteConversationAsync().get();
}

TEST_CASE("Conversation host with translations", "[api][cxx][conversation_translator][cxx_conversation][translate]")
{
    auto speechConfig = CreateConfig("en-US", { "fr" });
    auto conversation = Conversation::CreateConversationAsync(speechConfig).get();
    conversation->DeleteConversationAsync().get();
}

TEST_CASE("Conversation host destructor", "[api][cxx][conversation_translator][cxx_conversation][destructor]")
{
    auto speechConfig = CreateConfig("en-US", { "fr", "ar" });
    auto conversation = Conversation::CreateConversationAsync(speechConfig).get();

    conversation.reset();
}

TEST_CASE("Conversation destructor after start", "[api][cxx][conversation_translator][cxx_conversation][start_destructor]")
{
    auto speechConfig = CreateConfig("en-US", { "fr", "ar" });
    auto conversation = Conversation::CreateConversationAsync(speechConfig).get();
    conversation->StartConversationAsync().get();

    conversation.reset();
}

TEST_CASE("Conversation call while not joined", "[api][cxx][conversation_translator][cxx_conversation][not_started]")
{
    auto speechConfig = CreateConfig("en-US", { "fr", "ar" });
    auto conversation = Conversation::CreateConversationAsync(speechConfig).get();

    REQUIRE_THROWS_MATCHES(
        conversation->LockConversationAsync().get(),
        std::runtime_error,
        HasHR(SPXERR_INVALID_STATE));

    REQUIRE_THROWS_MATCHES(
        conversation->UnlockConversationAsync().get(),
        std::runtime_error,
        HasHR(SPXERR_INVALID_STATE));

    REQUIRE_THROWS_MATCHES(
        conversation->MuteParticipantAsync("userId").get(),
        std::runtime_error,
        HasHR(SPXERR_INVALID_STATE));

    REQUIRE_THROWS_MATCHES(
        conversation->UnmuteParticipantAsync("userId").get(),
        std::runtime_error,
        HasHR(SPXERR_INVALID_STATE));

    REQUIRE_THROWS_MATCHES(
        conversation->MuteAllParticipantsAsync().get(),
        std::runtime_error,
        HasHR(SPXERR_INVALID_STATE));

    REQUIRE_THROWS_MATCHES(
        conversation->UnmuteAllParticipantsAsync().get(),
        std::runtime_error,
        HasHR(SPXERR_INVALID_STATE));

    REQUIRE_THROWS_MATCHES(
        conversation->RemoveParticipantAsync("userId").get(),
        std::runtime_error,
        HasHR(SPXERR_INVALID_STATE));

    REQUIRE_NOTHROW(conversation->GetAuthorizationToken());
    REQUIRE_NOTHROW(conversation->GetConversationId());
    REQUIRE_NOTHROW(conversation->SetAuthorizationToken("random"));

    REQUIRE_NOTHROW(conversation->EndConversationAsync().get());
    REQUIRE_NOTHROW(conversation->DeleteConversationAsync().get());
}

TEST_CASE("Conversation call unsupported methods", "[api][cxx][conversation_translator][cxx_conversation][unsupported]")
{
    auto speechConfig = CreateConfig("en-US", { "fr", "ar" });

    SECTION("Conversation Translator")
    {
        auto conversation = Conversation::CreateConversationAsync(speechConfig).get();
        conversation->StartConversationAsync().get();

        auto u = User::FromUserId("userId");
        auto p = Participant::From("participant");

        REQUIRE_THROWS_MATCHES(
            conversation->AddParticipantAsync(u).get(),
            std::runtime_error,
            HasHR(SPXERR_UNSUPPORTED_API_ERROR));

        REQUIRE_THROWS_MATCHES(
            conversation->AddParticipantAsync(p).get(),
            std::runtime_error,
            HasHR(SPXERR_UNSUPPORTED_API_ERROR));

        REQUIRE_THROWS_MATCHES(
            conversation->AddParticipantAsync("userId").get(),
            std::runtime_error,
            HasHR(SPXERR_UNSUPPORTED_API_ERROR));

        conversation->EndConversationAsync().get();
        conversation->DeleteConversationAsync().get();
        conversation.reset();
    }

    SECTION("Conversation Transcriber")
    {
        speechConfig->SetProperty("ConversationTranscriptionInRoomAndOnline", "true");

        auto cts = Conversation::CreateConversationAsync(speechConfig, PAL::CreateGuidWithDashesUTF8()).get();

        REQUIRE_THROWS_MATCHES(
            cts->StartConversationAsync().get(),
            std::runtime_error,
            HasHR(SPXERR_UNSUPPORTED_API_ERROR));

        REQUIRE_THROWS_MATCHES(
            cts->LockConversationAsync().get(),
            std::runtime_error,
            HasHR(SPXERR_UNSUPPORTED_API_ERROR));

        REQUIRE_THROWS_MATCHES(
            cts->UnlockConversationAsync().get(),
            std::runtime_error,
            HasHR(SPXERR_UNSUPPORTED_API_ERROR));

        REQUIRE_THROWS_MATCHES(
            cts->MuteAllParticipantsAsync().get(),
            std::runtime_error,
            HasHR(SPXERR_UNSUPPORTED_API_ERROR));

        REQUIRE_THROWS_MATCHES(
            cts->UnmuteAllParticipantsAsync().get(),
            std::runtime_error,
            HasHR(SPXERR_UNSUPPORTED_API_ERROR));

        REQUIRE_THROWS_MATCHES(
            cts->MuteParticipantAsync("userId").get(),
            std::runtime_error,
            HasHR(SPXERR_UNSUPPORTED_API_ERROR));

        REQUIRE_THROWS_MATCHES(
            cts->UnmuteParticipantAsync("userId").get(),
            std::runtime_error,
            HasHR(SPXERR_UNSUPPORTED_API_ERROR));
    }
}

TEST_CASE("Conversation Translator Host Audio", "[api][cxx][conversation_translator][cxx_conversation_translator][audio][host]")
{
    UseMocks(false);
    REQUIRE(exists(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH)));

    string speechLang("en-US");
    string hostname("TheHost");
    vector<string> toLangs{ "fr", "de" };
    shared_ptr<SpeechTranslationConfig> speechConfig = CreateConfig(speechLang, toLangs);

    SPX_TRACE_INFO("Creating conversation");
    shared_ptr<Conversation> conversation = Conversation::CreateConversationAsync(speechConfig).get();
    SPX_TRACE_INFO("Starting conversation");
    conversation->StartConversationAsync().get();

    shared_ptr<AudioConfig> audioConfig = AudioConfig::FromWavFileInput(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH));
    shared_ptr<ConversationTranslator> conversationTranslator = ConversationTranslator::FromConfig(audioConfig);

    ConversationTranslatorCallbacks eventHandlers(conversationTranslator);

    SPX_TRACE_INFO("Joining conversation");
    conversationTranslator->JoinConversationAsync(conversation, hostname).get();
    SPX_TRACE_INFO("Start transcribing");
    conversationTranslator->StartTranscribingAsync().get();

    eventHandlers.WaitForAudioStreamCompletion(15000ms, 2000ms);


    SPX_TRACE_INFO("Stop Transcribing");
    conversationTranslator->StopTranscribingAsync().get();

    SPX_TRACE_INFO("Leave conversation");
    conversationTranslator->LeaveConversationAsync().get();

    SPX_TRACE_INFO("End conversation");
    conversation->EndConversationAsync().get();
    SPX_TRACE_INFO("Delete conversation");
    conversation->DeleteConversationAsync().get();

    SPX_TRACE_INFO("Verifying callbacks");

    std::string participantId;
    eventHandlers.VerifyBasicEvents(true, hostname, true, participantId);
    eventHandlers.VerifyTranscriptions(participantId,
    {
        ExpectedTranscription(participantId, AudioUtterancesMap[SINGLE_UTTERANCE_ENGLISH].Utterances["en-US"][0].Text, speechLang)
    });
}

TEST_CASE("Join a conversation with translation", "[api][cxx][conversation_translator][cxx_conversation_translator][audio][join]")
{
    UseMocks(false);
    REQUIRE(exists(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH)));
    REQUIRE(exists(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_CHINESE)));

    string hostLang("en-US");
    string hostName("TheHost");
    vector<string> hostToLangs{ "de" };
    string bobLang("zh-CN");
    string bobName("Bob");
    string hostId;
    string bobId;

    // Create room
    auto speechConfig = CreateConfig(hostLang, hostToLangs);

    auto conversation = Conversation::CreateConversationAsync(speechConfig).get();
    conversation->StartConversationAsync().get();

    auto audioConfig = AudioConfig::FromWavFileInput(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH));

    auto hostTranslator = ConversationTranslator::FromConfig(audioConfig);
    ConversationTranslatorCallbacks hostEvents(hostTranslator);
    hostTranslator->JoinConversationAsync(conversation, hostName).get();


    // Join room
    auto bobAudioConfig = AudioConfig::FromWavFileInput(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_CHINESE));
    SetParticipantConfig(bobAudioConfig);

    auto bobTranslator = ConversationTranslator::FromConfig(bobAudioConfig);
    ConversationTranslatorCallbacks bobEvents(bobTranslator);
    bobTranslator->JoinConversationAsync(conversation->GetConversationId(), bobName, bobLang).get();


    // do audio playback
    hostTranslator->StartTranscribingAsync().get();
    bobTranslator->StartTranscribingAsync().get();

    hostEvents.WaitForAudioStreamCompletion(15000ms);
    bobEvents.WaitForAudioStreamCompletion(15000ms, 2000ms);

    bobTranslator->StopTranscribingAsync().get();
    hostTranslator->StopTranscribingAsync().get();


    // bob leaves
    bobTranslator->LeaveConversationAsync().get();


    // host leaves
    this_thread::sleep_for(1s);
    hostTranslator->LeaveConversationAsync().get();
    conversation->EndConversationAsync().get();
    conversation->DeleteConversationAsync().get();

    // verify events
    bobEvents.VerifyBasicEvents(true, bobName, false, bobId);
    hostEvents.VerifyBasicEvents(true, hostName, true, hostId);
    bobEvents.VerifyTranscriptions(bobId,
    {
        ExpectedTranscription(bobId, AudioUtterancesMap[SINGLE_UTTERANCE_CHINESE].Utterances["zh-CN"][0].Text, bobLang),
        ExpectedTranscription(hostId, AudioUtterancesMap[SINGLE_UTTERANCE_ENGLISH].Utterances["en-US"][0].Text, hostLang)
    });

    hostEvents.VerifyTranscriptions(hostId,
    {
        ExpectedTranscription(bobId, AudioUtterancesMap[SINGLE_UTTERANCE_CHINESE].Utterances["zh-CN"][0].Text, bobLang, { { "en-US", "Weather." }, { "de", "wetter." } }),
        ExpectedTranscription(hostId, AudioUtterancesMap[SINGLE_UTTERANCE_ENGLISH].Utterances["en-US"][0].Text, hostLang, { { "de", "Wie ist das Wetter?" } }),
    });
}

TEST_CASE("Host sends an instant message", "[api][cxx][conversation_translator][cxx_conversation_translator][im][host]")
{
    UseMocks(false);
    REQUIRE(exists(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH)));

    auto speechConfig = CreateConfig("en-US", { "ja", "ar" });
    TestConversationParticipant host(speechConfig, "Host");

    auto audioConfig = AudioConfig::FromWavFileInput(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH));
    host.Join(audioConfig);

    host.ConvTrans->SendTextMessageAsync("This is a test").get(); // Translations for this are hard coded
    this_thread::sleep_for(3s);

    host.Leave();

    // verify events
    host.VerifyBasicEvents(false);
    host.VerifyIms(
    {
        ExpectedTranscription(host.ParticipantId, "This is a test", host.Lang, {{ "ja", "これはテストです" }, { "ar", "هذا اختبار" }})
    });
}

TEST_CASE("Host and participants send an instant messages", "[api][cxx][conversation_translator][cxx_conversation_translator][im][join]")
{
    UseMocks(false);
    REQUIRE(exists(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH)));
    REQUIRE(exists(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_CHINESE)));

    auto speechConfig = CreateConfig("en-US", {});
    TestConversationParticipant host(speechConfig, "Host");
    auto audioConfig = AudioConfig::FromWavFileInput(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH));
    host.Join(audioConfig);

    TestConversationParticipant alice("Alice", "fr", host);

    auto aliceAudioConfig = AudioConfig::FromWavFileInput(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_CHINESE));
    SetParticipantConfig(aliceAudioConfig);
    alice.Join(aliceAudioConfig);

    host.ConvTrans->SendTextMessageAsync("This is a test").get();
    this_thread::sleep_for(1s);
    alice.ConvTrans->SendTextMessageAsync("C'est un test").get();

    this_thread::sleep_for(2s);

    alice.Leave();
    host.Leave();

    // verify events
    alice.VerifyBasicEvents(false);
    host.VerifyBasicEvents(false);

    auto expectedIms =
    {
        ExpectedTranscription(host.ParticipantId, "This is a test", host.Lang, {{ alice.Lang, "C'est un test" }}),
        ExpectedTranscription(alice.ParticipantId, "C'est un test", alice.Lang, {{ host.Lang, "This is a test" }})
    };

    alice.VerifyIms(expectedIms);
    host.VerifyIms(expectedIms);
}

TEST_CASE("Join locked room", "[api][cxx][conversation_translator][cxx_conversation_translator][join_locked]")
{
    UseMocks(false);
    REQUIRE(exists(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH)));
    REQUIRE(exists(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_CHINESE)));

    auto speechConfig = CreateConfig("en-US", {});

    TestConversationParticipant host(speechConfig, "Host");
    auto audioConfig = AudioConfig::FromWavFileInput(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH));
    host.Join(audioConfig);
    host.Conv->LockConversationAsync().get();

    this_thread::sleep_for(1s);

    TestConversationParticipant alice("Alice", "fr", host);

    auto aliceAudioConfig = AudioConfig::FromWavFileInput(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_CHINESE));
    SetParticipantConfig(aliceAudioConfig);

    REQUIRE_THROWS_WITH(alice.Join(aliceAudioConfig), Catch::Contains("HTTP 400", Catch::CaseSensitive::No));

    host.Leave();
    host.VerifyBasicEvents(false);
}

TEST_CASE("ConversationTranslator Host disconnects room", "[api][cxx][conversation_translator][cxx_conversation_translator][host_disconnect]")
{
    UseMocks(false);
    REQUIRE(exists(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH)));
    REQUIRE(exists(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_CHINESE)));

    auto speechConfig = CreateConfig("en-US", {});
    TestConversationParticipant host(speechConfig, "Host");
    auto audioConfig = AudioConfig::FromWavFileInput(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH));
    host.Join(audioConfig);

    TestConversationParticipant alice("Alice", "zh-CN", host);
    auto aliceAudioConfig = AudioConfig::FromWavFileInput(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_CHINESE));
    SetParticipantConfig(aliceAudioConfig);
    alice.Join(aliceAudioConfig);

    host.StartAudio();
    alice.StartAudio();

    host.WaitForAudioToFinish();
    alice.WaitForAudioToFinish();

    host.StopAudio();
    host.Leave(); // should cause Alice to become disconnected

    this_thread::sleep_for(2s);

    host.VerifyBasicEvents(true);
    alice.VerifyBasicEvents(true);
}

TEST_CASE("Conversation Translator call methods when not joined", "[api][cxx][conversation_translator][cxx_conversation_translator][not_joined]")
{
    SECTION("Host")
    {
        auto speechConfig = CreateConfig("de-DE", {});
        auto conversation = Conversation::CreateConversationAsync(speechConfig).get();
        conversation->StartConversationAsync().get();

        auto translator = ConversationTranslator::FromConfig();

        REQUIRE_THROWS_MATCHES(
            translator->StartTranscribingAsync().get(),
            std::runtime_error,
            HasHR(SPXERR_UNINITIALIZED));

        REQUIRE_THROWS_MATCHES(
            translator->StopTranscribingAsync().get(),
            std::runtime_error,
            HasHR(SPXERR_UNINITIALIZED));

        REQUIRE_THROWS_MATCHES(
            translator->SendTextMessageAsync("This is a test").get(),
            std::runtime_error,
            HasHR(SPXERR_UNINITIALIZED));

        REQUIRE_NOTHROW(translator->LeaveConversationAsync().get());
    }

    SECTION("Participant")
    {
        auto translator = ConversationTranslator::FromConfig();

        REQUIRE_THROWS_MATCHES(
            translator->StartTranscribingAsync().get(),
            std::runtime_error,
            HasHR(SPXERR_UNINITIALIZED));

        REQUIRE_THROWS_MATCHES(
            translator->StopTranscribingAsync().get(),
            std::runtime_error,
            HasHR(SPXERR_UNINITIALIZED));

        REQUIRE_THROWS_MATCHES(
            translator->SendTextMessageAsync("This is a test").get(),
            std::runtime_error,
            HasHR(SPXERR_UNINITIALIZED));

        REQUIRE_NOTHROW(translator->LeaveConversationAsync().get());
    }
}

//TEST_CASE("Tyler's test case", "[tyler_test]")
//{
//    auto subscriptionKey = SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Key;
//    auto subscriptionRegion = SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Region;
//
//    std::string speechLang("en-US");
//    std::string endpointUrl;
//    {
//        int numChannels = 1;
//        std::string host("its-loadbalancer-devb9503283r42485a4.princetondev.customspeech.ai");
//        std::ostringstream oss;
//
//        oss << "wss://" << host << "/speech/recognition/dynamicaudio?&format=simple"
//            << "&channelCount=" << numChannels;
//
//        endpointUrl = oss.str();
//    }
//
//    auto speechConfig = SpeechConfig::FromEndpoint(endpointUrl, subscriptionKey);
//    speechConfig->SetSpeechRecognitionLanguage(speechLang);
//    speechConfig->SetProperty("ConversationTranslator_Region", subscriptionRegion);
//    speechConfig->SetProperty("ConversationTranslator_MultiChannelAudio", "true");
//    speechConfig->SetProxy("127.0.0.1", 8888);
//
//    shared_ptr<Conversation> conversation = Conversation::CreateConversationAsync(speechConfig).get();
//    SPX_TRACE_INFO("Starting conversation");
//    conversation->StartConversationAsync().get();
//
//    shared_ptr<AudioConfig> audioConfig = AudioConfig::FromWavFileInput(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH));
//    shared_ptr<ConversationTranslator> conversationTranslator = ConversationTranslator::FromConfig(audioConfig);
//
//    ConversationTranslatorCallbacks eventHandlers(conversationTranslator);
//
//    SPX_TRACE_INFO("Joining conversation");
//    conversationTranslator->JoinConversationAsync(conversation, "TheHost").get();
//    SPX_TRACE_INFO("Start transcribing");
//    conversationTranslator->StartTranscribingAsync().get();
//
//    //eventHandlers.WaitForAudioStreamCompletion(15000ms, 2000ms);
//    std::this_thread::sleep_for(15s);
//
//    SPX_TRACE_INFO("Stop Transcribing");
//    conversationTranslator->StopTranscribingAsync().get();
//
//    SPX_TRACE_INFO("Leave conversation");
//    conversationTranslator->LeaveConversationAsync().get();
//
//    SPX_TRACE_INFO("End conversation");
//    conversation->EndConversationAsync().get();
//    SPX_TRACE_INFO("Delete conversation");
//    conversation->DeleteConversationAsync().get();
//
//    //auto speechConfig = SpeechTranslationConfig::FromEndpoint(endpointUrl, subscriptionKey);
//    //speechConfig->SetProxy("127.0.0.1", 8888);
//    //speechConfig->SetSpeechRecognitionLanguage(speechLang);
//    //speechConfig->AddTargetLanguage("fr");
//    ////speechConfig->SetProperty("ConversationTranslator_Region", "westus2");
//
//    //shared_ptr<AudioConfig> audioConfig = AudioConfig::FromWavFileInput(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH));
//    //auto from = SourceLanguageConfig::FromLanguage(speechLang);
//
//    //auto reco = TranslationRecognizer::FromConfig(speechConfig, audioConfig);
//
//    //reco->StartContinuousRecognitionAsync().get();
//    //std::this_thread::sleep_for(900s);
//    //reco->StopContinuousRecognitionAsync().get();
//}

/*
TEST_CASE("Sample host code", "[sample_code][host]")
{
    UseMocks(false);
    REQUIRE(exists(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH)));

    // For the host
    auto config = SpeechTranslationConfig::FromSubscription("<Cognitive speech subscription key>", "<subscription key region>");
    config->SetSpeechRecognitionLanguage("en-US");
    config->AddTargetLanguage("fr");

    // Create and start the conversation
    auto conversation = Conversation::CreateConversationAsync(config).get();
    conversation->StartConversationAsync().get();

    // Create the conversation translator using the default microphone

    auto audioConfig = AudioConfig::FromWavFileInput(weather.m_inputDataFilename);
    auto conversationTranslator = ConversationTranslator::FromConfig();

    // hook up the event handlers
    conversationTranslator->Canceled += [](const ConversationTranslationCanceledEventArgs& args)
    {
        std::cout << "Canceled Reason:" << to_string((int)args.Reason) << ", Error code: " << to_string((int)args.ErrorCode) << endl;
    };
    conversationTranslator->ConversationExpiration += [](const ConversationExpirationEventArgs& args)
    {
        std::cout << "Conversation will expire in " << args.ExpirationTime.count() << " minute(s)" << endl;
    };
    conversationTranslator->ParticipantsChanged += [](const ConversationParticipantsChangedEventArgs& args)
    {
        std::cout << "Participants(s) ";
        switch (args.Reason)
        {
            case ParticipantChangedReason::JoinedConversation:
                std::cout << "joined";
                break;
            case ParticipantChangedReason::Updated:
                std::cout << "updated";
                break;
            case ParticipantChangedReason::LeftConversation:
                std::cout << "left";
                break;
        }

        for (std::shared_ptr<Participant> participant : args.Participants)
        {
            std::cout << ", " << participant->DisplayName << "[" << participant->Id << "]";
        }

        std::cout << endl;
    };
    conversationTranslator->SessionStarted += [](const SessionEventArgs& args)
    {
        std::cout << "Session has started " << args.SessionId << endl;
    };
    conversationTranslator->SessionStopped += [](const SessionEventArgs& args)
    {
        std::cout << "Session has stopped " << args.SessionId << endl;
    };
    conversationTranslator->TextMessageReceived += [](const ConversationTranslationEventArgs& args)
    {
        std::cout << "Text message received from " << args.Result->ParticipantId << ": '" << args.Result->Text << "'" << endl;
        for (const auto& translation : args.Result->Translations)
        {
            std::cout << "\t" << translation.first << ": '" << translation.second << "'" << endl;
        }
    };
    conversationTranslator->Transcribing += [](const ConversationTranslationEventArgs& args)
    {
        std::cout << "Partial transcription received from " << args.Result->ParticipantId << ": '" << args.Result->Text << "'" << endl;
        for (const auto& translation : args.Result->Translations)
        {
            std::cout << "\t" << translation.first << ": '" << translation.second << "'" << endl;
        }
    };
    conversationTranslator->Transcribed += [](const ConversationTranslationEventArgs& args)
    {
        std::cout << "Transcription received from " << args.Result->ParticipantId << ": '" << args.Result->Text << "'" << endl;
        for (const auto& translation : args.Result->Translations)
        {
            std::cout << "\t" << translation.first << ": '" << translation.second << "'" << endl;
        }
    };

    // join the conversation. After this you still start receiving events
    conversationTranslator->JoinConversationAsync(conversation, "<the display name to use>").get();

    // as the host you can manage the room
    conversation->MuteAllParticipantsAsync().get(); // mute everyone other than yourself
    conversation->LockConversationAsync().get(); // prevent new participants from joining the room

    // start and stop capturing audio from the microphone
    conversationTranslator->StartTranscribingAsync().get();
    this_thread::sleep_for(30s);
    conversationTranslator->StopTranscribingAsync().get();

    // exit and cleanup
    conversationTranslator->LeaveConversationAsync().get(); // after this you will not receive events

    conversation->EndConversationAsync().get(); // all other current participants will be removed from the room
    conversation->DeleteConversationAsync().get();
}

TEST_CASE("Sample participant code", "[sample_code][participant]")
{
    // For a participant joining you don't need a conversation object

    // play back audio from a wave file
    auto audioConfig = AudioConfig::FromWavFileInput("<full path to wave file>");
    auto conversationTranslator = ConversationTranslator::FromConfig(audioConfig);

    // hook up events

    // join the existing conversation, after this you will receive events
    conversationTranslator->JoinConversationAsync("<conversation id>", "<the display name to use>", "en-US").get();

    // start sending the audio, then stop
    conversationTranslator->StartTranscribingAsync().get();
    this_thread::sleep_for(15s);
    conversationTranslator->StopTranscribingAsync().get();

    // you can send text messages at any time
    conversationTranslator->SendTextMessageAsync("This is a test message").get();

    // stop sending audio and cleanup
    conversationTranslator->LeaveConversationAsync().get();
}
*/
