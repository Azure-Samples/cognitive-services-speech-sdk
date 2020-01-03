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

std::shared_ptr<SpeechConfig> CreateCTSInroomServiceSpeechConfig()
{
    auto audioEndpoint = DefaultSettingsMap[INROOM_AUDIO_ENDPOINT];
    audioEndpoint += "/multiaudio";
    auto config = SpeechConfig::FromEndpoint(audioEndpoint, SubscriptionsRegionsMap[CONVERSATION_TRANSCRIPTION_PPE_SUBSCRIPTION].Key);
    config->SetProperty("ConversationTranscriptionInRoomAndOnline", "true");
    return config;
}

TEST_CASE("conversation create without id", "[api][cxx]")
{
    auto config = CreateCTSInroomServiceSpeechConfig();
    auto conversation = Conversation::CreateConversationAsync(config).get();

    auto got_id = conversation->GetConversationId();
    SPXTEST_REQUIRE(got_id.empty());
}

TEST_CASE("conversation create with id", "[api][cxx]")
{
    auto config = CreateCTSInroomServiceSpeechConfig();
    auto id = PAL::CreateGuidWithDashesUTF8();
    auto conversation = Conversation::CreateConversationAsync(config, id).get();

    auto got_id = conversation->GetConversationId();
    SPXTEST_REQUIRE(got_id == id);
}

TEST_CASE("conversation id in Chinese", "[api][cxx]")
{
    auto config = CreateCTSInroomServiceSpeechConfig();

    // chinese meaning "de" in "wo1 de".
    unsigned char s[] = { 0xe7, 0x9a, 0x84, 0 };
    auto conversation = Conversation::CreateConversationAsync(config, reinterpret_cast<char*>(s)).get();
    auto gotId = conversation->GetConversationId();
    SPXTEST_REQUIRE((PAL::stricmp(reinterpret_cast<char*>(s), gotId.c_str())) == 0);
}

TEST_CASE("conversation create transcriber no get", "[api][cxx]")
{
    auto config = CreateCTSInroomServiceSpeechConfig();

    std::string id = "asdf";
    auto conversation = Conversation::CreateConversationAsync(config, id).get();

    auto audioInput = AudioConfig::FromWavFileInput(ROOT_RELATIVE_PATH(CONVERSATION_BETWEEN_TWO_PERSONS_ENGLISH));
    auto transcriber = ConversationTranscriber::FromConfig(audioInput);

    REQUIRE_NOTHROW(transcriber->JoinConversationAsync(conversation));  // did not have .get() by intention. Carbon should not crash when customers forget .get()
}

TEST_CASE("conversation create transcriber", "[api][cxx]")
{
    auto config = CreateCTSInroomServiceSpeechConfig();

    std::string id = "asdf";
    auto conversation = Conversation::CreateConversationAsync(config, id).get();

    auto audioInput = AudioConfig::FromWavFileInput(ROOT_RELATIVE_PATH(CONVERSATION_BETWEEN_TWO_PERSONS_ENGLISH));
    auto transcriber = ConversationTranscriber::FromConfig(audioInput);

    REQUIRE_NOTHROW(transcriber->JoinConversationAsync(conversation).get());
}

TEST_CASE("conversation create users", "[api][cxx]")
{
    auto myId = "emailOfSomeUser";
    auto user = User::FromUserId(myId);
    SPXTEST_REQUIRE(user->GetId() == myId);
}

TEST_CASE("conversation create participant", "[api][cxx]")
{
    REQUIRE_THROWS(Participant::From(""));
    REQUIRE_NOTHROW(Participant::From("secondparticipant"));
    REQUIRE_NOTHROW(Participant::From("secondparticipant", "en-us"));
    REQUIRE_THROWS(Participant::From("secondparticipant", "en-us", "voiceSignature"));
}

TEST_CASE("conversation add_remove participant by user id", "[api][cxx]")
{
    auto config = CreateCTSInroomServiceSpeechConfig();
    auto conv = Conversation::CreateConversationAsync(config).get();

    auto user_id = "afdsafd";
    auto participant = conv->AddParticipantAsync(user_id).get();

    // verify that the handle is ok.
    SPXTEST_REQUIRE((bool)participant == true);

    auto value = "a_value_I_want_to_set";
    REQUIRE_NOTHROW(participant->Properties.SetProperty("AnyKey", value));

    auto got_value = participant->Properties.GetProperty("AnyKey", "DefaltValue");
    SPXTEST_REQUIRE(got_value == value);

    REQUIRE_NOTHROW(conv->RemoveParticipantAsync(user_id).get());
}

TEST_CASE("conversation add_remove participant by user object", "[api][cxx]")
{
    auto config = CreateCTSInroomServiceSpeechConfig();
    auto conv = Conversation::CreateConversationAsync(config).get();

    auto id = "afdsafd";
    auto user_object = User::FromUserId(id);
    auto user_from_add = conv->AddParticipantAsync(user_object).get();

    SPXTEST_REQUIRE((bool)user_object == true);
    SPXTEST_REQUIRE(user_from_add == user_object);
    SPXTEST_REQUIRE(user_object->GetId() == id);

    REQUIRE_NOTHROW(conv->RemoveParticipantAsync(user_object).get());
}

TEST_CASE("conversation add_remove participant by participant object", "[api][cxx]")
{
    auto config = CreateCTSInroomServiceSpeechConfig();
    auto conv = Conversation::CreateConversationAsync(config).get();

    auto id = "afdsafd";
    auto participant = Participant::From(id);
    auto participant_from_add = conv->AddParticipantAsync(participant).get();

    SPXTEST_REQUIRE((bool)participant == true);
    SPXTEST_REQUIRE(participant_from_add == participant);

    REQUIRE_NOTHROW(conv->RemoveParticipantAsync(participant).get());
}

TEST_CASE("conversation online end meeting destroy resources", "[api][cxx]")
{
    auto config = SpeechConfig::FromEndpoint(DefaultSettingsMap[ONLINE_AUDIO_ENDPOINT], SubscriptionsRegionsMap[CONVERSATION_TRANSCRIPTION_PPE_SUBSCRIPTION].Key);
    config->SetProperty("ConversationTranscriptionInRoomAndOnline", "true");

    auto audioInput = AudioConfig::FromWavFileInput(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH));
    auto myId = PAL::CreateGuidWithDashesUTF8();
    INFO(myId);
    auto conversation = Conversation::CreateConversationAsync(config, myId).get();
    auto transcriber = ConversationTranscriber::FromConfig(audioInput);
    REQUIRE_NOTHROW(transcriber->JoinConversationAsync(conversation).get());

    // set iCalUid
    conversation->Properties.SetProperty("iCalUid", "040000008200E00074C5B7101A82E008000000001003D722197CD4010000000000000000100000009E970FDF583F9D4FB999E607891A2F66");

    // set callId
    conversation->Properties.SetProperty("callId", PAL::CreateGuidWithDashesUTF8());

    // set organizer
    conversation->Properties.SetProperty("organizer", "SP049@somedomain.com");

    auto result = make_shared<RecoPhrases>();
    ConnectCallbacks(transcriber.get(), result);
    auto p = Participant::From("one@example.com", "en-us");

    // add the speaker1 to usp
    conversation->AddParticipantAsync(p).get();

    transcriber->StartTranscribingAsync().get();

    REQUIRE_NOTHROW(conversation->EndConversationAsync().get());
    WaitForResult(result->ready.get_future(), 10min);

    SPXTEST_REQUIRE(!result->phrases.empty());
    auto text = GetText(result->phrases);
    INFO(text);
    SPXTEST_REQUIRE(FindTheRef(result->phrases, AudioUtterancesMap[SINGLE_UTTERANCE_ENGLISH].Utterances["en-US"][0].Text));

    // allow EndConversationAsync at any time.
    REQUIRE_NOTHROW(conversation->EndConversationAsync().get());
}
