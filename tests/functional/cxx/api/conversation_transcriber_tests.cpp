//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
#include "test_utils.h"
#include "file_utils.h"
#include "recognizer_utils.h"

#include "speechapi_cxx.h"
#include "wav_file_reader.h"
#include "guid_utils.h"

#include "speechapi_cxx_conversation.h"
#include "speechapi_cxx_conversation_transcriber.h"

using namespace std;
using namespace Microsoft::CognitiveServices::Speech::Impl; // for CSpxWavFileReader
using namespace Microsoft::CognitiveServices::Speech;
using namespace Microsoft::CognitiveServices::Speech::Audio;
using namespace Microsoft::CognitiveServices::Speech::Transcription;

#define SPX_CONFIG_TRACE_INTERFACE_MAP

std::shared_ptr<SpeechConfig> CreateSpeechConfigForCTSInRoom()
{
    auto audioEndpoint = Config::InRoomAudioEndpoint;
    audioEndpoint += "/multiaudio";
    auto config = SpeechConfig::FromEndpoint(audioEndpoint, Keys::ConversationTranscriber);
    // this is the clue for Carbon to tell go to CTSInRoom service by using CSpxParticipantMgrImpl.
    config->SetProperty("ConversationTranscriptionInRoomAndOnline", "true");
    return config;
}

TEST_CASE("conversation transcriber no join", "[api][cxx]")
{
    auto config = CreateSpeechConfigForCTSInRoom();

    katieSteve.UpdateFullFilename(Config::InputDir);
    auto audioInput = AudioConfig::FromWavFileInput(katieSteve.m_inputDataFilename);
    auto myId = PAL::CreateGuidWithDashesUTF8();
    INFO(myId);
    auto conversation = Conversation::CreateConversationAsync(config, myId).get();

    auto transcriber = ConversationTranscriber::FromConfig(audioInput);

    REQUIRE_THROWS(transcriber->StartTranscribingAsync().get());
}

TEST_CASE("conversation transcriber leave conversation", "[api][cxx]")
{
    auto config = CreateSpeechConfigForCTSInRoom();

    katieSteve.UpdateFullFilename(Config::InputDir);
    auto audioInput = AudioConfig::FromWavFileInput(katieSteve.m_inputDataFilename);
    auto myId = PAL::CreateGuidWithDashesUTF8();
    INFO(myId);
    auto conversation = Conversation::CreateConversationAsync(config, myId).get();

    auto transcriber = ConversationTranscriber::FromConfig(audioInput);

    REQUIRE_NOTHROW(transcriber->JoinConversationAsync(conversation).get());
    // leave it so that we won't receive any reco events. We should still receive sessionStarted, sessionStopped and Cancaled events.
    REQUIRE_NOTHROW(transcriber->LeaveConversationAsync().get());
    auto result = make_shared<RecoPhrases>();

    int recogEventCount = 0;
    std::promise<void> ready;
    std::atomic<bool> sessionStoppedReceived{ false };
    std::atomic<bool> sessionStartedReceived{ false };
    std::atomic<bool> canceledReceived{ false };

    transcriber->Transcribing.DisconnectAll();
    transcriber->Transcribing.Connect([&recogEventCount](const ConversationTranscriptionEventArgs& e)
    {
        if (e.Result->Reason == ResultReason::RecognizingSpeech)
        {
            ostringstream os;
            os << "Text= " << e.Result->Text
                << " Offset= " << e.Result->Offset()
                << " Duration= " << e.Result->Duration();
            INFO(os.str());
            recogEventCount++;
        }
    });
    transcriber->Transcribed.DisconnectAll();
    transcriber->Transcribed.Connect([&recogEventCount](const ConversationTranscriptionEventArgs& e)
    {
        if (e.Result->Reason == ResultReason::RecognizingSpeech)
        {
            ostringstream os;
            os << "Text= " << e.Result->Text
                << " Offset= " << e.Result->Offset()
                << " Duration= " << e.Result->Duration();
            INFO(os.str());
            recogEventCount++;
        }
    });
    transcriber->SessionStopped.DisconnectAll();
    transcriber->SessionStopped.Connect([&ready, &sessionStoppedReceived](const SessionEventArgs& e)
    {
        UNUSED(e);
        sessionStoppedReceived = true;
        ready.set_value();
    });

    transcriber->SessionStarted.DisconnectAll();
    transcriber->SessionStarted.Connect([&ready, &sessionStartedReceived](const SessionEventArgs& e)
    {
        UNUSED(e);
        sessionStartedReceived = true;
    });

    transcriber->Canceled.DisconnectAll();
    transcriber->Canceled.Connect([&ready, &canceledReceived](const ConversationTranscriptionCanceledEventArgs& e)
    {
        if (e.Reason == CancellationReason::Error)
        {
            canceledReceived = true;
            INFO("Canceled received due to error.");
            ready.set_value();
        }
    });
    transcriber->StartTranscribingAsync().get();
    WaitForResult(ready.get_future(), 5min);
    transcriber->StopTranscribingAsync().get();
    SPXTEST_REQUIRE(recogEventCount == 0);
    SPXTEST_REQUIRE(sessionStartedReceived);
    SPXTEST_REQUIRE(sessionStoppedReceived);
}

TEST_CASE("conversation transcriber reco", "[api][cxx]")
{
    auto config = CreateSpeechConfigForCTSInRoom();

    katieSteve.UpdateFullFilename(Config::InputDir);
    auto audioInput = AudioConfig::FromWavFileInput(katieSteve.m_inputDataFilename);
    auto myId = PAL::CreateGuidWithDashesUTF8();
    INFO(myId);
    auto conversation = Conversation::CreateConversationAsync(config, myId).get();

    auto transcriber = ConversationTranscriber::FromConfig(audioInput);

    REQUIRE_NOTHROW(transcriber->JoinConversationAsync(conversation).get());

    auto result = make_shared<RecoPhrases>();
    ConnectCallbacks(transcriber.get(), result);
    transcriber->StartTranscribingAsync().get();

    WaitForResult(result->ready.get_future(), 5min);
    transcriber->StopTranscribingAsync().get();

    INFO(GetText(result->phrases));
    SPXTEST_REQUIRE(!result->phrases.empty());
    SPXTEST_REQUIRE(FindTheRef(result->phrases, katieSteve.m_utterance));
}

//TEST_CASE("conversation add while pumping", "[api][cxx]")
//{
//    auto config = CreateSpeechConfigForCTSInRoom();
//    auto myId = PAL::CreateGuidWithDashesUTF8();
//    INFO(myId);
//    auto conversation = Conversation::CreateConversationAsync(config, myId).get();
//
//    std::shared_ptr<PullAudioInputStream> pullAudio;
//    auto audioInput = CreateAudioPullFromRecordedFile(pullAudio);
//    auto transcriber = ConversationTranscriber::FromConfig(audioInput);
//
//    // must join
//    REQUIRE_NOTHROW(transcriber->JoinConversationAsync(conversation).get());
//
//    auto result = make_shared<RecoPhrases>();
//    ConnectCallbacks(transcriber.get(), result);
//
//    transcriber->StartTranscribingAsync().get();
//    this_thread::sleep_for(100ms);
//    conversation->AddParticipantAsync("AddParticipantWhileAudioPumping").get();
//    // the recorded audio is really long, intentionally timeout in 5min.
//    WaitForResult(result->ready.get_future(), 15min);
//    auto text = GetText(result->phrases);
//    INFO(text);
//
//    // Changed verification string from 'ABC' to '123' per suggestion as work around from Sarah Lu
//    bool res = VerifyTextAndSpeaker(result->phrases, "123.", "Unidentified") || VerifyTextAndSpeaker(result->phrases, "123", "Unidentified");
//    SPXTEST_REQUIRE(res == true);
//}

TEST_CASE("conversation bad connection", "[api][cxx]")
{
    auto audioEndpoint = "wrong_endpoint";
    auto config = SpeechConfig::FromEndpoint(audioEndpoint, Keys::ConversationTranscriber);
    config->SetProperty("ConversationTranscriptionInRoomAndOnline", "true");

    std::shared_ptr<PullAudioInputStream> pullAudio;
    auto audioInput = CreateAudioPullUsingKatieSteveFile(pullAudio);
    auto myId = PAL::CreateGuidWithDashesUTF8();
    INFO(myId);
    auto conversation = Conversation::CreateConversationAsync(config, myId).get();
    auto transcriber = ConversationTranscriber::FromConfig(audioInput);
    REQUIRE_NOTHROW(transcriber->JoinConversationAsync(conversation).get());

    auto result = make_shared<RecoPhrases>();
    ConnectCallbacks(transcriber.get(), result);

    transcriber->StartTranscribingAsync().get();
    WaitForResult(result->ready.get_future(), 5min);
    transcriber->StopTranscribingAsync().get();

    SPXTEST_REQUIRE(result->phrases[0].Text.find("Runtime error: Failed to create transport request.") != string::npos);
}

TEST_CASE("conversation_inroom_8_channel_file", "[api][cxx]")
{
    auto config = CreateSpeechConfigForCTSInRoom();
    katieSteve.UpdateFullFilename(Config::InputDir);
    auto audioInput = AudioConfig::FromWavFileInput(katieSteve.m_inputDataFilename);

    auto myId = PAL::CreateGuidWithDashesUTF8();
    INFO(myId);
    auto conversation = Conversation::CreateConversationAsync(config, myId).get();
    auto transcriber = ConversationTranscriber::FromConfig(audioInput);
    REQUIRE_NOTHROW(transcriber->JoinConversationAsync(conversation).get());

    auto result = make_shared<RecoPhrases>();
    ConnectCallbacks(transcriber.get(), result);
    auto p = Participant::From("conversation_inroom_8_channel_file", "en-us");
    REQUIRE_NOTHROW(conversation->AddParticipantAsync(p).get());
    auto text = PumpAudioAndWaitForResult(transcriber.get(), result);
    INFO(text);
    SPXTEST_REQUIRE(result->phrases.empty() == false);
    SPXTEST_REQUIRE(FindTheRef(result->phrases, katieSteve.m_utterance));
}

TEST_CASE("conversation_inroom_8_channel_audio_pull", "[api][cxx]")
{
    auto config = CreateSpeechConfigForCTSInRoom();

    std::shared_ptr<PullAudioInputStream> pullAudio;
    auto audioInput = CreateAudioPullUsingKatieSteveFile(pullAudio);

    string katieVoiceSignature, steveVoiceSignature;
    CreateVoiceSignatures(&katieVoiceSignature, &steveVoiceSignature);
    auto katie = Participant::From("katie@example.com", "en-us", katieVoiceSignature);

    auto myId = PAL::CreateGuidWithDashesUTF8();
    INFO(myId);
    auto conversation = Conversation::CreateConversationAsync(config, myId).get();
    auto transcriber = ConversationTranscriber::FromConfig(audioInput);
    REQUIRE_NOTHROW(transcriber->JoinConversationAsync(conversation).get());

    SPXTEST_SECTION("AddParticipantByParticipantObj")
    {
        auto result = make_shared<RecoPhrases>();
        ConnectCallbacks(transcriber.get(), result);

        REQUIRE_NOTHROW(conversation->AddParticipantAsync(katie).get());
        auto text = PumpAudioAndWaitForResult(transcriber.get(), result);

        // Note: INFO prints out the phrases before removing punctuations and lowercasing.
        INFO(text);
        SPXTEST_REQUIRE(result->phrases.empty() == false);
        SPXTEST_REQUIRE(FindTheRef(result->phrases, katieSteve.m_utterance));
    }

    SPXTEST_SECTION("AddParticipantByUserId")
    {
        auto result = make_shared<RecoPhrases>();
        ConnectCallbacks(transcriber.get(), result);

        //Verify maximum number of participants
        int max_expected = 10;
        conversation->Properties.SetProperty("Conversation-MaximumAllowedParticipants", std::to_string(max_expected));
        auto maxInAudioConfig = std::stoi(conversation->Properties.GetProperty("Conversation-MaximumAllowedParticipants"), nullptr, 10);
        REQUIRE(max_expected == maxInAudioConfig);

        // add by user id
        REQUIRE_NOTHROW(conversation->AddParticipantAsync("AddParticipantById_1").get());
        REQUIRE_NOTHROW(conversation->AddParticipantAsync("AddParticipantById_2").get());

        // verify in the debugger that no AddParticipantById_3 in the speech.event
        REQUIRE_NOTHROW(conversation->AddParticipantAsync("AddParticipantById_3").get());
        REQUIRE_NOTHROW(conversation->RemoveParticipantAsync("AddParticipantById_3").get());

        REQUIRE_THROWS(conversation->RemoveParticipantAsync("AddParticipantById_4").get());

        transcriber->StartTranscribingAsync().get();
        WaitForResult(result->ready.get_future(), 15min);
        transcriber->StopTranscribingAsync().get();

        INFO(GetText(result->phrases));
        SPXTEST_REQUIRE(!result->phrases.empty());
        SPXTEST_REQUIRE(FindTheRef(result->phrases, katieSteve.m_utterance));
    }

    SPXTEST_SECTION("AddParticipantByUserObject")
    {
        auto result = make_shared<RecoPhrases>();
        ConnectCallbacks(transcriber.get(), result);

        // create a user object
        auto user = User::FromUserId("CreateUserfromUserId");
        REQUIRE_NOTHROW(conversation->AddParticipantAsync(user).get());

        transcriber->StartTranscribingAsync().get();
        WaitForResult(result->ready.get_future(), 15min);
        transcriber->StopTranscribingAsync().get();

        INFO(GetText(result->phrases));
        SPXTEST_REQUIRE(!result->phrases.empty());
        SPXTEST_REQUIRE(FindTheRef(result->phrases, katieSteve.m_utterance));
    }
    SPXTEST_SECTION("AddParticipantSetters")
    {
        auto result = make_shared<RecoPhrases>();
        ConnectCallbacks(transcriber.get(), result);

        auto p1 = Participant::From("id1");

        REQUIRE_NOTHROW(p1->SetVoiceSignature({ katieVoiceSignature }));
        REQUIRE_THROWS(p1->SetVoiceSignature(""));

        REQUIRE_NOTHROW(p1->SetPreferredLanguage("en-us"));
        REQUIRE_THROWS(p1->SetPreferredLanguage(""));

        conversation->AddParticipantAsync(p1);

        transcriber->StartTranscribingAsync().get();
        WaitForResult(result->ready.get_future(), 15min);
        transcriber->StopTranscribingAsync().get();

        INFO(GetText(result->phrases));
        SPXTEST_REQUIRE(!result->phrases.empty());
        SPXTEST_REQUIRE(FindTheRef(result->phrases, katieSteve.m_utterance));
    }
    SPXTEST_SECTION("AddParticipants")
    {
        auto result = make_shared<RecoPhrases>();
        ConnectCallbacks(transcriber.get(), result);

        auto p1 = Participant::From("katie@example.com", "en-us", katieVoiceSignature);
        conversation->AddParticipantAsync(p1).get();
        auto p2 = Participant::From("steve@example.com", "en-us", steveVoiceSignature);
        conversation->AddParticipantAsync(p2).get();

        transcriber->StartTranscribingAsync().get();
        WaitForResult(result->ready.get_future(), 15min);
        transcriber->StopTranscribingAsync().get();

        INFO(GetText(result->phrases));
        SPXTEST_REQUIRE(!result->phrases.empty());
        SPXTEST_REQUIRE(FindTheRef(result->phrases, katieSteve.m_utterance));
    }
    SPXTEST_SECTION("AddRemoveParticipants")
    {
        auto result = make_shared<RecoPhrases>();
        ConnectCallbacks(transcriber.get(), result);

        auto p1 = Participant::From("id1", "en-us", katieVoiceSignature);
        conversation->AddParticipantAsync(p1).get();
        auto p2 = Participant::From("id2", "en-us", steveVoiceSignature);
        conversation->AddParticipantAsync(p2).get();

        REQUIRE_NOTHROW(conversation->RemoveParticipantAsync(p1).get());
        REQUIRE_NOTHROW(conversation->AddParticipantAsync(p2).get());
        REQUIRE_NOTHROW(conversation->AddParticipantAsync(p1).get());
        REQUIRE_NOTHROW(conversation->RemoveParticipantAsync(p1).get());

        transcriber->StartTranscribingAsync().get();
        WaitForResult(result->ready.get_future(), 15min);
        transcriber->StopTranscribingAsync().get();

        INFO(GetText(result->phrases));
        SPXTEST_REQUIRE(!result->phrases.empty());
        SPXTEST_REQUIRE(FindTheRef(result->phrases, katieSteve.m_utterance));
    }
    // a meeting without participant should still be fine!
    SPXTEST_SECTION("conversation_with_no_participant")
    {
        auto result = make_shared<RecoPhrases>();
        ConnectCallbacks(transcriber.get(), result);

        transcriber->StartTranscribingAsync();
        WaitForResult(result->ready.get_future(), 15min);
        transcriber->StopTranscribingAsync().get();

        INFO(GetText(result->phrases));
        SPXTEST_REQUIRE(!result->phrases.empty());
        SPXTEST_REQUIRE(FindTheRef(result->phrases, katieSteve.m_utterance));
    }
}

TEST_CASE("conversation_inroom_8_channel_audio_push", "[api][cxx]")
{
    auto config = CreateSpeechConfigForCTSInRoom();

    std::shared_ptr<PushAudioInputStream> pushAudio;
    auto audioInput = CreateAudioPushUsingKatieSteveFile(pushAudio);

    string katieVoiceSignature, steveVoiceSignature;
    CreateVoiceSignatures(&katieVoiceSignature, &steveVoiceSignature);
    auto katie = Participant::From("katie@example.com", "en-us", katieVoiceSignature);

    auto myId = PAL::CreateGuidWithDashesUTF8();
    INFO(myId);
    auto conversation = Conversation::CreateConversationAsync(config, myId).get();
    auto transcriber = ConversationTranscriber::FromConfig(audioInput);
    REQUIRE_NOTHROW(transcriber->JoinConversationAsync(conversation).get());

    auto result = make_shared<RecoPhrases>();
    ConnectCallbacks(transcriber.get(), result);

    // add the speaker1 to usp
    conversation->AddParticipantAsync(katie).get();

    transcriber->StartTranscribingAsync().get();

    auto reader = std::make_shared<CSpxWavFileReader>();

    reader->Open(PAL::ToWString(katieSteve.m_inputDataFilename).c_str());
    SPXWAVEFORMATEX format;
    reader->GetFormat(&format, sizeof(SPXWAVEFORMATEX));
    auto bytesPerSample = format.wBitsPerSample / 8;
    auto samplesPerSec = format.nSamplesPerSec;
    auto framesPerSec = 10;
    auto channels = format.nChannels;
    SPX_DBG_ASSERT(channels >= 1);

    auto bytesPerFrame = samplesPerSec / framesPerSec * bytesPerSample * channels;
    auto data = SpxAllocSharedAudioBuffer(bytesPerFrame);

    auto  sleepBetween = 5ms;
    auto cbAudio = 0;
    do
    {
        // SetProperty before calling Write
        pushAudio->SetProperty(PropertyId::DataBuffer_UserId, "SpeakerA");

        cbAudio = reader->Read(data.get(), bytesPerFrame);
        pushAudio->Write(data.get(), cbAudio);
        std::this_thread::sleep_for(sleepBetween);
    } while (cbAudio != 0);

    pushAudio->Close();

    WaitForResult(result->ready.get_future(), 15min);
    transcriber->StopTranscribingAsync().get();

    INFO(GetText(result->phrases));
    SPXTEST_REQUIRE(!result->phrases.empty());
    SPXTEST_REQUIRE(FindTheRef(result->phrases, katieSteve.m_utterance));
}

TEST_CASE("conversation_online_pull_stream", "[api][cxx][transcriber]")
{
    REQUIRE(!Config::OnlineAudioEndpoint.empty());
    REQUIRE(!Keys::ConversationTranscriber.empty());
    auto config = SpeechConfig::FromEndpoint(Config::OnlineAudioEndpoint, Keys::ConversationTranscriber);
    config->SetProperty("ConversationTranscriptionInRoomAndOnline", "true");
    weather.UpdateFullFilename(Config::InputDir);
    auto fs = OpenWaveFile(weather.m_inputDataFilename);

    // Create the "pull stream" object with C++ lambda callbacks
    auto pullStream = AudioInputStream::CreatePullStream(
        AudioStreamFormat::GetWaveFormatPCM(16000, 16, 1),
        [&fs](uint8_t* buffer, uint32_t size) -> int { return (int)ReadBuffer(fs, buffer, size); },
        [=]() {},
        [=](PropertyId propertyId) -> SPXSTRING {
        if (propertyId == PropertyId::DataBuffer_TimeStamp)
        {
            return CreateTimestamp();
        }
        else if (propertyId == PropertyId::DataBuffer_UserId)
        {
            return "SP042@speechdemo.onmicrosoft.com";
        }
        else
        {
            return "";
        }
    }
    );

    auto audioInput = AudioConfig::FromStreamInput(pullStream);
    auto myId = PAL::CreateGuidWithDashesUTF8();
    INFO(myId);
    auto conversation = Conversation::CreateConversationAsync(config, myId).get();
    auto transcriber = ConversationTranscriber::FromConfig(audioInput);
    REQUIRE_NOTHROW(transcriber->JoinConversationAsync(conversation).get());

    auto result = make_shared<RecoPhrases>();
    ConnectCallbacks(transcriber.get(), result);

    SPXTEST_SECTION("conversation_id")
    {
        auto got_id = conversation->GetConversationId();
        SPXTEST_REQUIRE(myId == got_id);
    }

    SPXTEST_SECTION("all_features")
    {
        auto participant = Participant::From("SP042@speechdemo.onmicrosoft.com", "en-us");
        conversation->AddParticipantAsync(participant).get();

        // set iCalUid
        conversation->Properties.SetProperty("iCalUid", "asdf");

        // set callId
        conversation->Properties.SetProperty("callId", "asdf");

        // set organizer
        conversation->Properties.SetProperty("organizer", "SP049@somedomain.com");

        // turn on audio recording
        conversation->Properties.SetProperty("audiorecording", "on");

        transcriber->StartTranscribingAsync().get();
        WaitForResult(result->ready.get_future(), 15min);
        transcriber->StopTranscribingAsync().get();
        SPXTEST_REQUIRE(result->phrases.size() >= 1);
        SPXTEST_REQUIRE(result->phrases[0].Text == weather.m_utterance);
    }
}

TEST_CASE("conversation_online_1_channel_file", "[api][cxx]")
{
    auto config = SpeechConfig::FromEndpoint(Config::OnlineAudioEndpoint, Keys::ConversationTranscriber);
    config->SetProperty("ConversationTranscriptionInRoomAndOnline", "true");

    weather.UpdateFullFilename(Config::InputDir);
    auto audioInput = AudioConfig::FromWavFileInput(weather.m_inputDataFilename);

    auto myId = PAL::CreateGuidWithDashesUTF8();
    INFO(myId);
    auto conversation = Conversation::CreateConversationAsync(config, myId).get();
    auto transcriber = ConversationTranscriber::FromConfig(audioInput);
    REQUIRE_NOTHROW(transcriber->JoinConversationAsync(conversation).get());

    auto result = make_shared<RecoPhrases>();
    ConnectCallbacks(transcriber.get(), result);
    auto p = Participant::From("conversation_online_1_channel_file", "en-us");
    conversation->AddParticipantAsync(p).get();

    transcriber->StartTranscribingAsync().get();
    WaitForResult(result->ready.get_future(), 15min);
    transcriber->StopTranscribingAsync().get();
    SPXTEST_REQUIRE(result->phrases.size() >= 1);
    SPXTEST_REQUIRE(result->phrases[0].Text == weather.m_utterance);
}

#if 0
TEST_CASE("conversation_online_microphone", "[api][cxx]")
{
    auto config = SpeechConfig::FromEndpoint(Config::OnlineAudioEndpoint, Keys::Speech);
    config->SetProperty("ConversationTranscriptionInRoomAndOnline", "true");

    auto transcriber = ConversationTranscriber::FromConfig(config);

    auto result = make_shared<RecoPhrases>();
    ConnectCallbacks(transcriber.get(), result);

    transcriber->SetConversationId("ConversationWithNoparticipants");
    auto  participant = transcriber->AddParticipant("john@microsoft.com");

    transcriber->StartTranscribingAsync().get();

    WaitForResult(result->ready.get_future(), 30s);
    transcriber->StopTranscribingAsync().get();
    SPXTEST_REQUIRE(result->phrases.size() >= 1);
    SPXTEST_REQUIRE(result->phrases[0].Text == weather.m_utterance);
}
#endif

