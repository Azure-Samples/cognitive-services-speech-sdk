//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

// Dump logs to the output stream in release builds
#if !defined(DEBUG) && !defined(_DEBUG)
#define INTEGRATION_TEST_LOG_AND_DUMP
#endif

#include "stdafx.h"
#include <speechapi_cxx_connection.h>
#include "recognizer_utils.h"
#include "guid_utils.h"
#include "dump_log_files_at_end.h"
#include "conversation_translator_utils.h"

#define CT_INTEGRATION_TEST_INIT \
    UseMocks(false); \
    LogToFileAndDumpAtEnd("ct_integration_test_log.txt")

using namespace std;
using namespace Microsoft::CognitiveServices::Speech::Impl;
using namespace Microsoft::CognitiveServices::Speech::IntegrationTests;
using namespace Microsoft::CognitiveServices::Speech::Transcription;

TEST_CASE("[CT] Conversation host without translations", "[api][cxx][conversation_translator][cxx_conversation][no_translate]")
{
    CT_INTEGRATION_TEST_INIT;
    auto speechConfig = CreateConfig("en-US", {});
    auto conversation = Conversation::CreateConversationAsync(speechConfig).get();
    conversation->DeleteConversationAsync().get();
}

TEST_CASE("[CT] Conversation host with translations", "[api][cxx][conversation_translator][cxx_conversation][translate]")
{
    CT_INTEGRATION_TEST_INIT;
    auto speechConfig = CreateConfig("en-US", { "fr" });
    auto conversation = Conversation::CreateConversationAsync(speechConfig).get();
    conversation->DeleteConversationAsync().get();
}

TEST_CASE("[CT] Conversation host destructor", "[api][cxx][conversation_translator][cxx_conversation][destructor]")
{
    CT_INTEGRATION_TEST_INIT;
    auto speechConfig = CreateConfig("en-US", { "fr", "ar" });
    auto conversation = Conversation::CreateConversationAsync(speechConfig).get();

    conversation.reset();
}

TEST_CASE("[CT] Conversation destructor after start", "[api][cxx][conversation_translator][cxx_conversation][start_destructor]")
{
    CT_INTEGRATION_TEST_INIT;
    auto speechConfig = CreateConfig("en-US", { "fr", "ar" });
    auto conversation = Conversation::CreateConversationAsync(speechConfig).get();
    conversation->StartConversationAsync().get();

    conversation.reset();
}

TEST_CASE("[CT] Conversation call while not joined", "[api][cxx][conversation_translator][cxx_conversation][not_started]")
{
    CT_INTEGRATION_TEST_INIT;
    auto speechConfig = CreateConfig("en-US", { "fr", "ar" });
    auto conversation = Conversation::CreateConversationAsync(speechConfig).get();

    REQUIRE_THROWS_MATCHES(
        conversation->LockConversationAsync().get(),
        std::runtime_error,
        Catch::HasHR(SPXERR_INVALID_STATE));

    REQUIRE_THROWS_MATCHES(
        conversation->UnlockConversationAsync().get(),
        std::runtime_error,
        Catch::HasHR(SPXERR_INVALID_STATE));

    REQUIRE_THROWS_MATCHES(
        conversation->MuteParticipantAsync("userId").get(),
        std::runtime_error,
        Catch::HasHR(SPXERR_INVALID_STATE));

    REQUIRE_THROWS_MATCHES(
        conversation->UnmuteParticipantAsync("userId").get(),
        std::runtime_error,
        Catch::HasHR(SPXERR_INVALID_STATE));

    REQUIRE_THROWS_MATCHES(
        conversation->MuteAllParticipantsAsync().get(),
        std::runtime_error,
        Catch::HasHR(SPXERR_INVALID_STATE));

    REQUIRE_THROWS_MATCHES(
        conversation->UnmuteAllParticipantsAsync().get(),
        std::runtime_error,
        Catch::HasHR(SPXERR_INVALID_STATE));

    REQUIRE_THROWS_MATCHES(
        conversation->RemoveParticipantAsync("userId").get(),
        std::runtime_error,
        Catch::HasHR(SPXERR_INVALID_STATE));

    REQUIRE_NOTHROW(conversation->GetAuthorizationToken());
    REQUIRE_NOTHROW(conversation->GetConversationId());
    REQUIRE_NOTHROW(conversation->SetAuthorizationToken("random"));

    REQUIRE_NOTHROW(conversation->EndConversationAsync().get());
    REQUIRE_NOTHROW(conversation->DeleteConversationAsync().get());
}

TEST_CASE("[CT] Conversation call unsupported methods", "[api][cxx][conversation_translator][cxx_conversation][unsupported]")
{
    CT_INTEGRATION_TEST_INIT;
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
            Catch::HasHR(SPXERR_UNSUPPORTED_API_ERROR));

        REQUIRE_THROWS_MATCHES(
            conversation->AddParticipantAsync(p).get(),
            std::runtime_error,
            Catch::HasHR(SPXERR_UNSUPPORTED_API_ERROR));

        REQUIRE_THROWS_MATCHES(
            conversation->AddParticipantAsync("userId").get(),
            std::runtime_error,
            Catch::HasHR(SPXERR_UNSUPPORTED_API_ERROR));

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
            Catch::HasHR(SPXERR_UNSUPPORTED_API_ERROR));

        REQUIRE_THROWS_MATCHES(
            cts->LockConversationAsync().get(),
            std::runtime_error,
            Catch::HasHR(SPXERR_UNSUPPORTED_API_ERROR));

        REQUIRE_THROWS_MATCHES(
            cts->UnlockConversationAsync().get(),
            std::runtime_error,
            Catch::HasHR(SPXERR_UNSUPPORTED_API_ERROR));

        REQUIRE_THROWS_MATCHES(
            cts->MuteAllParticipantsAsync().get(),
            std::runtime_error,
            Catch::HasHR(SPXERR_UNSUPPORTED_API_ERROR));

        REQUIRE_THROWS_MATCHES(
            cts->UnmuteAllParticipantsAsync().get(),
            std::runtime_error,
            Catch::HasHR(SPXERR_UNSUPPORTED_API_ERROR));

        REQUIRE_THROWS_MATCHES(
            cts->MuteParticipantAsync("userId").get(),
            std::runtime_error,
            Catch::HasHR(SPXERR_UNSUPPORTED_API_ERROR));

        REQUIRE_THROWS_MATCHES(
            cts->UnmuteParticipantAsync("userId").get(),
            std::runtime_error,
            Catch::HasHR(SPXERR_UNSUPPORTED_API_ERROR));
    }
}

TEST_CASE("[CT] Conversation Translator Host Audio", "[api][cxx][conversation_translator][cxx_conversation_translator][audio][host]")
{
    CT_INTEGRATION_TEST_INIT;
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

    auto eventHandlers = ConversationTranslatorCallbacks::From(conversationTranslator);

    SPX_TRACE_INFO("Joining conversation");
    conversationTranslator->JoinConversationAsync(conversation, hostname).get();
    SPX_TRACE_INFO("Start transcribing");
    conversationTranslator->StartTranscribingAsync().get();

    eventHandlers->WaitForAudioStreamCompletion(MAX_WAIT_FOR_AUDIO_TO_COMPLETE, WAIT_AFTER_AUDIO_COMPLETE);

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
    eventHandlers->VerifyBasicEvents(true, hostname, true, participantId);
    eventHandlers->VerifyTranscriptions(participantId,
    {
        ExpectedTranscription(participantId, AudioUtterancesMap[SINGLE_UTTERANCE_ENGLISH].Utterances["en-US"][0].Text, speechLang)
    });
}

TEST_CASE("[CT] Join a conversation with translation", "[api][cxx][conversation_translator][cxx_conversation_translator][audio][join]")
{
    CT_INTEGRATION_TEST_INIT;
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
    auto hostEvents = ConversationTranslatorCallbacks::From(hostTranslator);
    hostTranslator->JoinConversationAsync(conversation, hostName).get();


    // Join room
    auto bobAudioConfig = AudioConfig::FromWavFileInput(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_CHINESE));
    SetParticipantConfig(bobAudioConfig);

    auto bobTranslator = ConversationTranslator::FromConfig(bobAudioConfig);
    auto bobEvents = ConversationTranslatorCallbacks::From(bobTranslator);
    bobTranslator->JoinConversationAsync(conversation->GetConversationId(), bobName, bobLang).get();


    // do audio playback
    hostTranslator->StartTranscribingAsync().get();
    bobTranslator->StartTranscribingAsync().get();

    hostEvents->WaitForAudioStreamCompletion(MAX_WAIT_FOR_AUDIO_TO_COMPLETE);
    bobEvents->WaitForAudioStreamCompletion(MAX_WAIT_FOR_AUDIO_TO_COMPLETE, WAIT_AFTER_AUDIO_COMPLETE);

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
    bobEvents->VerifyBasicEvents(true, bobName, false, bobId);
    hostEvents->VerifyBasicEvents(true, hostName, true, hostId);
    bobEvents->VerifyTranscriptions(bobId,
    {
        ExpectedTranscription(bobId, AudioUtterancesMap[SINGLE_UTTERANCE_CHINESE].Utterances["zh-CN"][0].Text, bobLang),
        ExpectedTranscription(hostId, AudioUtterancesMap[SINGLE_UTTERANCE_ENGLISH].Utterances["en-US"][0].Text, hostLang)
    });

    hostEvents->VerifyTranscriptions(hostId,
    {
        ExpectedTranscription(bobId, AudioUtterancesMap[SINGLE_UTTERANCE_CHINESE].Utterances["zh-CN"][0].Text, bobLang, { { "en-US", "Weather." }, { "de", "wetter." } }),
        ExpectedTranscription(hostId, AudioUtterancesMap[SINGLE_UTTERANCE_ENGLISH].Utterances["en-US"][0].Text, hostLang, { { "de", "Wie ist das Wetter?" } }),
    });
}

TEST_CASE("[CT] Host sends an instant message", "[!hide][api][cxx][conversation_translator][cxx_conversation_translator][im][host]")
{
    CT_INTEGRATION_TEST_INIT;
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

TEST_CASE("[CT] Host and participants send an instant messages", "[!hide][api][cxx][conversation_translator][cxx_conversation_translator][im][join]")
{
    CT_INTEGRATION_TEST_INIT;
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

    this_thread::sleep_for(3s);

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

TEST_CASE("[CT] Join locked room", "[api][cxx][conversation_translator][cxx_conversation_translator][join_locked]")
{
    CT_INTEGRATION_TEST_INIT;
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

TEST_CASE("[CT] ConversationTranslator Host disconnects room", "[api][cxx][conversation_translator][cxx_conversation_translator][host_disconnect]")
{
    CT_INTEGRATION_TEST_INIT;
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

TEST_CASE("[CT] Conversation Translator call methods when not joined", "[api][cxx][conversation_translator][cxx_conversation_translator][not_joined]")
{
    CT_INTEGRATION_TEST_INIT;

    SECTION("Host")
    {
        auto speechConfig = CreateConfig("de-DE", {});
        auto conversation = Conversation::CreateConversationAsync(speechConfig).get();
        conversation->StartConversationAsync().get();

        auto translator = ConversationTranslator::FromConfig();

        REQUIRE_THROWS_MATCHES(
            translator->StartTranscribingAsync().get(),
            std::runtime_error,
            Catch::HasHR(SPXERR_UNINITIALIZED));

        REQUIRE_THROWS_MATCHES(
            translator->StopTranscribingAsync().get(),
            std::runtime_error,
            Catch::HasHR(SPXERR_UNINITIALIZED));

        REQUIRE_THROWS_MATCHES(
            translator->SendTextMessageAsync("This is a test").get(),
            std::runtime_error,
            Catch::HasHR(SPXERR_UNINITIALIZED));

        REQUIRE_NOTHROW(translator->LeaveConversationAsync().get());
    }

    SECTION("Participant")
    {
        auto translator = ConversationTranslator::FromConfig();

        REQUIRE_THROWS_MATCHES(
            translator->StartTranscribingAsync().get(),
            std::runtime_error,
            Catch::HasHR(SPXERR_UNINITIALIZED));

        REQUIRE_THROWS_MATCHES(
            translator->StopTranscribingAsync().get(),
            std::runtime_error,
            Catch::HasHR(SPXERR_UNINITIALIZED));

        REQUIRE_THROWS_MATCHES(
            translator->SendTextMessageAsync("This is a test").get(),
            std::runtime_error,
            Catch::HasHR(SPXERR_UNINITIALIZED));

        REQUIRE_NOTHROW(translator->LeaveConversationAsync().get());
    }
}

TEST_CASE("[CT] Double join should fail", "[api][cxx][conversation_translator][cxx_conversation_translator][double_join]")
{
    CT_INTEGRATION_TEST_INIT;

    REQUIRE(exists(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH)));
    auto hostSpeechConfig = CreateConfig("en-US", {});
    auto hostAudioConfig = AudioConfig::FromWavFileInput(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH));

    REQUIRE(exists(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_CHINESE)));
    auto aliceAudioConfig = AudioConfig::FromWavFileInput(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_CHINESE));

    TestConversationParticipant host(hostSpeechConfig, "Host");
    host.Join(hostAudioConfig);

    // host tries to join again. should fail
    REQUIRE_THROWS_MATCHES(
        host.ConvTrans->JoinConversationAsync(host.Conv, host.Name).get(),
        std::runtime_error,
        Catch::HasHR(SPXERR_INVALID_STATE));

    TestConversationParticipant alice("Alice", "zh-CN", host);
    alice.Join(aliceAudioConfig);

    // Alice tries to join again, should fail
    REQUIRE_THROWS_MATCHES(
        alice.ConvTrans->JoinConversationAsync(host.ConversationId, alice.Name, alice.Lang).get(),
        std::runtime_error,
        Catch::HasHR(SPXERR_INVALID_STATE));

    alice.Leave();
    host.Leave();
}

TEST_CASE("[CT] Conversation Translator Connection Before Join", "[api][cxx][conversation_translator][cxx_conversation_translator][connection][before_join]")
{
    CT_INTEGRATION_TEST_INIT;

    REQUIRE(exists(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH)));
    auto audioConfig = AudioConfig::FromWavFileInput(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH));

    auto conversationTranslator = ConversationTranslator::FromConfig(audioConfig);

    auto connection = Connection::FromConversationTranslator(conversationTranslator);

    REQUIRE_THROWS_MATCHES(
        connection->Open(false),
        std::runtime_error,
        Catch::HasHR(SPXERR_INVALID_STATE)
    );

    REQUIRE_THROWS_MATCHES(
        connection->Open(true),
        std::runtime_error,
        Catch::HasHR(SPXERR_INVALID_STATE)
    );

    // Close should not throw exceptions
    REQUIRE_NOTHROW(connection->Close());
}

TEST_CASE("[CT] Conversation Translator Connection After Leave", "[api][cxx][conversation_translator][cxx_conversation_translator][connection][after_leave]")
{
    CT_INTEGRATION_TEST_INIT;

    auto speechConfig = CreateConfig("en-US", {});
    REQUIRE(exists(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH)));
    auto audioConfig = AudioConfig::FromWavFileInput(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH));

    TestConversationParticipant host(speechConfig, "Host");
    host.Join(audioConfig);

    std::this_thread::sleep_for(200ms);

    host.Leave();

    // connecting should fail now
    REQUIRE_THROWS_MATCHES(
        host.Conn->Open(false),
        std::runtime_error,
        Catch::HasHR(SPXERR_INVALID_STATE)
    );

    REQUIRE_THROWS_MATCHES(
        host.Conn->Open(true),
        std::runtime_error,
        Catch::HasHR(SPXERR_INVALID_STATE)
    );

    // Close should not throw exceptions
    REQUIRE_NOTHROW(host.Conn->Close());
}

TEST_CASE("[CT] Conversation Translator Connection Recognizer events/methods", "[api][cxx][conversation_translator][cxx_conversation_translator][connection][recognizer]")
{
    CT_INTEGRATION_TEST_INIT;

    auto speechConfig = CreateConfig("en-US", {});
    REQUIRE(exists(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH)));
    auto audioConfig = AudioConfig::FromWavFileInput(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH));

    TestConversationParticipant host(speechConfig, "Host");
    host.Join(audioConfig);

    std::vector<shared_ptr<ConnectionMessage>> evts;

    host.Conn->MessageReceived += [&evts](const ConnectionMessageEventArgs& evt)
    {
        evts.push_back(evt.GetMessage());
    };

    host.StartAudio();

    host.WaitForAudioToFinish();

    // send message
    host.Conn->SendMessageAsync("speech.context", "{\"translationcontext\":{\"to\":[\"en-US\"]}}").get();

    host.Leave();

    SPXTEST_REQUIRE(evts.size() > 0);
}

TEST_CASE("[CT] Conversation Translator Host Leave Rejoin", "[!hide][api][cxx][conversation_translator][cxx_conversation_translator][connection][host][rejoin]")
{
    CT_INTEGRATION_TEST_INIT;
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

    auto eventHandlers = ConversationTranslatorCallbacks::From(conversationTranslator);

    SPX_TRACE_INFO("Joining conversation");
    conversationTranslator->JoinConversationAsync(conversation, hostname).get();

    SPX_TRACE_INFO("Getting connection object");
    auto connection = Connection::FromConversationTranslator(conversationTranslator);
    eventHandlers->AddConnectionCallbacks(connection);

    std::this_thread::sleep_for(200ms);

    SPX_TRACE_INFO("Disconnecting conversation");
    connection->Close();

    std::this_thread::sleep_for(200ms);

    SPX_TRACE_INFO("Reconnecting conversation");
    connection->Open(false);

    SPX_TRACE_INFO("Start transcribing after reconnect");
    conversationTranslator->StartTranscribingAsync().get();

    SPX_TRACE_INFO("Send text message after reconnect");
    conversationTranslator->SendTextMessageAsync("This is a test").get();

    eventHandlers->WaitForAudioStreamCompletion(MAX_WAIT_FOR_AUDIO_TO_COMPLETE, WAIT_AFTER_AUDIO_COMPLETE);

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
    eventHandlers->VerifyBasicEvents(true, hostname, true, participantId);
    eventHandlers->VerifyConnectionEvents(1, 2);
    eventHandlers->VerifyTranscriptions(participantId,
        {
            ExpectedTranscription(participantId, AudioUtterancesMap[SINGLE_UTTERANCE_ENGLISH].Utterances["en-US"][0].Text, speechLang),
        });
    eventHandlers->VerifyIms(participantId,
        {
            ExpectedTranscription(participantId, "This is a test", speechLang, {{ "fr", "C'est un test" }, { "de", "Dies ist ein Test" }})
        });
}

TEST_CASE("[CT] Conversation Translator can't call methods after disconnect", "[api][cxx][conversation_translator][cxx_conversation_translator][connection][after_disconnect]")
{
    CT_INTEGRATION_TEST_INIT;

    REQUIRE(exists(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH)));
    auto speechConfig = CreateConfig("en-US", {});
    auto audioConfig = AudioConfig::FromWavFileInput(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH));

    TestConversationParticipant host(speechConfig, "Host");
    host.Join(audioConfig);

    std::this_thread::sleep_for(200ms);

    host.Conn->Close();

    std::this_thread::sleep_for(200ms);

    REQUIRE_THROWS_MATCHES(
        host.ConvTrans->StartTranscribingAsync().get(),
        std::runtime_error,
        Catch::HasHR(SPXERR_INVALID_STATE));

    REQUIRE_THROWS_MATCHES(
        host.ConvTrans->StopTranscribingAsync().get(),
        std::runtime_error,
        Catch::HasHR(SPXERR_INVALID_STATE));

    REQUIRE_THROWS_MATCHES(
        host.ConvTrans->SendTextMessageAsync("This is a short test").get(),
        std::runtime_error,
        Catch::HasHR(SPXERR_INVALID_STATE));

    host.Leave();
}

TEST_CASE("[CT] Conversation Translator Participant Rejoin", "[api][cxx][conversation_translator][cxx_conversation_translator][connection][participant][rejoin]")
{
    CT_INTEGRATION_TEST_INIT;

    REQUIRE(exists(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH)));
    auto hostSpeechConfig = CreateConfig("en-US", {});
    auto hostAudioConfig = AudioConfig::FromWavFileInput(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH));

    REQUIRE(exists(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_CHINESE)));
    auto aliceAudioConfig = AudioConfig::FromWavFileInput(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_CHINESE));

    TestConversationParticipant host(hostSpeechConfig, "Host");
    host.Join(hostAudioConfig);

    TestConversationParticipant alice("Alice", "zh-CN", host);
    alice.Join(aliceAudioConfig);

    std::this_thread::sleep_for(200ms);

    // Alice disconnects
    SPX_TRACE_INFO("Alice disconnecting");
    alice.Conn->Close();
    std::this_thread::sleep_for(400ms);

    // Alice reconnects
    SPX_TRACE_INFO("Alice reconnecting");
    alice.Conn->Open(false);

    // Do audio playback for both host and participant
    host.StartAudio();
    alice.StartAudio();

    host.WaitForAudioToFinish();
    alice.WaitForAudioToFinish();

    std::this_thread::sleep_for(2s);

    // Disconnect both
    alice.Leave();
    host.Leave();

    // validate events
    SPX_TRACE_INFO("Validating host basic events");
    host.VerifyBasicEvents(true);
    SPX_TRACE_INFO("Validating Alice basic events");
    alice.VerifyBasicEvents(true);
    SPXTEST_REQUIRE(alice.Events->Connected.size() == 2);
    SPXTEST_REQUIRE(alice.Events->Disconnected.size() == 2);

    auto expectedTranscriptions =
    {
        ExpectedTranscription(host.ParticipantId, AudioUtterancesMap[SINGLE_UTTERANCE_ENGLISH].Utterances["en-US"][0].Text, host.Lang),
        ExpectedTranscription(alice.ParticipantId, AudioUtterancesMap[SINGLE_UTTERANCE_CHINESE].Utterances["zh-CN"][0].Text, alice.Lang)
    };

    SPX_TRACE_INFO("Validating host transcriptions");
    host.VerifyTranscriptions(expectedTranscriptions);
    SPX_TRACE_INFO("Validating Alice transcriptions");
    alice.VerifyTranscriptions(expectedTranscriptions);
}

TEST_CASE("[CT] Conversation Translator Participant Rejoin After Delete", "[api][cxx][conversation_translator][cxx_conversation_translator][connection][participant][rejoin_after_delete]")
{
    CT_INTEGRATION_TEST_INIT;

    REQUIRE(exists(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH)));
    auto hostSpeechConfig = CreateConfig("en-US", {});
    auto hostAudioConfig = AudioConfig::FromWavFileInput(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH));

    REQUIRE(exists(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_CHINESE)));
    auto aliceAudioConfig = AudioConfig::FromWavFileInput(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_CHINESE));

    TestConversationParticipant host(hostSpeechConfig, "Host");
    host.Join(hostAudioConfig);

    TestConversationParticipant alice("Alice", "zh-CN", host);
    alice.Join(aliceAudioConfig);

    std::this_thread::sleep_for(200ms);

    // Alice disconnects. This prevents the conversation translator from detecting the conversation
    // has been deleted since we no longer have an active web socket connection
    SPX_TRACE_INFO("Alice disconnecting");
    alice.Conn->Close();
    std::this_thread::sleep_for(400ms);

    // Delete the room
    host.Leave();

    // Alice tries to reconnect
    SPX_TRACE_INFO("Alice reconnecting");
    REQUIRE_THROWS_MATCHES(
        alice.Conn->Open(false),
        std::runtime_error,
        Catch::MessageContains("BadRequest") && Catch::MessageContains("WebSocket Upgrade failed"));

    std::this_thread::sleep_for(200ms);

    // Make sure we got the correct cancelled event
    SPXTEST_REQUIRE(alice.Events->Canceled.size() > 0);
    const auto& canceled = alice.Events->Canceled[0];
    SPXTEST_REQUIRE(canceled.Reason == CancellationReason::Error);
    SPXTEST_REQUIRE(canceled.ErrorCode == CancellationErrorCode::BadRequest);

    // Make sure we can't call open again
    REQUIRE_THROWS_MATCHES(
        alice.Conn->Open(false),
        std::runtime_error,
        Catch::HasHR(SPXERR_INVALID_STATE)
    );

    REQUIRE_THROWS_MATCHES(
        alice.Conn->Open(true),
        std::runtime_error,
        Catch::HasHR(SPXERR_INVALID_STATE)
    );
}

TEST_CASE("[CT] Conversation Translator SpeechRecognizer Connection Still Works", "[api][cxx][conversation_translator][cxx_conversation_translator][connection][trans_recognizer]")
{
    auto utterance = AudioUtterancesMap[SINGLE_UTTERANCE_ENGLISH];

    REQUIRE(exists(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH)));
    auto audioConfig = AudioConfig::FromWavFileInput(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH));
    auto speechConfig = CreateConfig("en-US", { });

    auto recognizer = SpeechRecognizer::FromConfig(speechConfig, audioConfig);
    auto connection = Connection::FromRecognizer(recognizer);

    auto evts = ConversationTranslatorCallbacks::From(recognizer);
    evts->AddConnectionCallbacks(connection);

    recognizer->StartContinuousRecognitionAsync().get();
    evts->WaitForAudioStreamCompletion(MAX_WAIT_FOR_AUDIO_TO_COMPLETE, WAIT_AFTER_AUDIO_COMPLETE);
    recognizer->StopContinuousRecognitionAsync().get();

    connection->Close();

    std::this_thread::sleep_for(2s);

    evts->VerifySessionAndConnectEvents(true);
    SPXTEST_REQUIRE(evts->Transcribed.size() > 0);
    SPXTEST_REQUIRE_THAT(evts->Transcribed[0].Text, Catch::FuzzyMatch(utterance.Utterances["en-US"][0].Text));
}



TEST_CASE("[CT] Conversation Translator Sweden demo", "[!hide][cxx_conversation_translator][Sweden]")
{
    CT_INTEGRATION_TEST_INIT;

    auto subscriptionKey = SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Key;
    auto subscriptionRegion = SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Region;

    std::string speechLang("en-US");
    std::string endpointUrl;
    {
        std::string host("its-loadbalancer-devb9503283r42485a4.princetondev.customspeech.ai");
        std::ostringstream oss;

        oss << "wss://" << host << "/speech/recognition/dynamicaudio?&format=simple"
            << "&channelCount=1";;

        endpointUrl = oss.str();
    }

    auto speechConfig = SpeechConfig::FromEndpoint(endpointUrl, subscriptionKey);
    speechConfig->SetSpeechRecognitionLanguage(speechLang);
    speechConfig->SetProperty("ConversationTranslator_Region", subscriptionRegion);
    speechConfig->SetProperty("ConversationTranslator_MultiChannelAudio", "true");
    //speechConfig->SetProxy("127.0.0.1", 8888);

    shared_ptr<Conversation> conversation = Conversation::CreateConversationAsync(speechConfig).get();
    SPX_TRACE_INFO("Starting conversation");
    conversation->StartConversationAsync().get();

    shared_ptr<AudioConfig> audioConfig = AudioConfig::FromWavFileInput(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH));
    shared_ptr<ConversationTranslator> conversationTranslator = ConversationTranslator::FromConfig(audioConfig);

    auto eventHandlers = ConversationTranslatorCallbacks::From(conversationTranslator);

    SPX_TRACE_INFO("Joining conversation");
    conversationTranslator->JoinConversationAsync(conversation, "TheHost").get();

    SPX_TRACE_INFO("Start transcribing");
    conversationTranslator->StartTranscribingAsync().get();

    //eventHandlers->WaitForAudioStreamCompletion(MAX_WAIT_FOR_AUDIO_TO_COMPLETE, WAIT_AFTER_AUDIO_COMPLETE);
    std::this_thread::sleep_for(15s);

    SPX_TRACE_INFO("Stop Transcribing");
    conversationTranslator->StopTranscribingAsync().get();

    SPX_TRACE_INFO("Leave conversation");
    conversationTranslator->LeaveConversationAsync().get();

    SPX_TRACE_INFO("End conversation");
    conversation->EndConversationAsync().get();
    SPX_TRACE_INFO("Delete conversation");
    conversation->DeleteConversationAsync().get();
}

