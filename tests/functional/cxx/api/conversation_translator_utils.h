//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#pragma once

#include "test_utils.h"
#include <vector>
#include <memory>
#include <unordered_map>
#include <json.h>
#include <speechapi_cxx_common.h>
#include <speechapi_cxx_speech_translation_config.h>
#include <speechapi_cxx_conversation.h>
#include <speechapi_cxx_conversation_translator.h>
#include <speechapi_cxx_participant.h>
#include <string_utils.h>
#include "test_PAL.h"
#include "conversation_utils.h"

#define SPX_CONFIG_TRACE_INTERFACE_MAP
using json = nlohmann::json;

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace IntegrationTests {

    using StringUtils = PAL::StringUtils;
    using namespace std;
    using namespace Microsoft::CognitiveServices::Speech::Impl;
    using namespace Microsoft::CognitiveServices::Speech::Audio;
    using namespace Microsoft::CognitiveServices::Speech::Translation;
    using namespace Microsoft::CognitiveServices::Speech::Transcription;
    using namespace Microsoft::CognitiveServices::Speech::Impl::ConversationTranslation;


#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 6237)
        // Disable: (<zero> && <expression>) is always zero.  <expression> is never evaluated and might have side effects.
#endif

    template<typename T, typename P>
    static T First(std::vector<T>& v, P predicate)
    {
        auto f = find_if(v.begin(), v.end(), predicate);
        if (f != v.end())
        {
            return *f;
        }

        throw std::invalid_argument("Not found");
    }

    static string FirstOrDefault(initializer_list<string> values)
    {
        for (const auto& v : values)
        {
            if (false == v.empty())
            {
                return v;
            }
        }

        return string();
    }

    template<typename I>
    static void SetCommonConfig(std::shared_ptr<I> config)
    {
        if (false == Config::ConversationTranslatorHost.empty())
        {
            config->SetProperty(ConversationKeys::Conversation_Management_Endpoint, (string("https://") + Config::ConversationTranslatorHost + "/capito/room").c_str());
            config->SetProperty(ConversationKeys::Conversation_Endpoint, (string("wss://") + Config::ConversationTranslatorHost + "/capito/translate").c_str());
        }

        config->SetProperty(PropertyId::Speech_SessionId, string("IntegrationTest:") + to_string(chrono::high_resolution_clock::now().time_since_epoch().count()));

        // set conversation specific overrides
        if (!Keys::ConversationTranslator.empty())
        {
            config->SetProperty(ConversationKeys::ConversationServiceConnection_Key, Keys::ConversationTranslator.c_str());
        }

        if (!Config::ConversationTranslatorRegion.empty())
        {
            config->SetProperty(ConversationKeys::Conversation_Region, Config::ConversationTranslatorRegion.c_str());
        }

        if (!Config::ConversationTranslatorClientId.empty())
        {
            config->SetProperty(ConversationKeys::Conversation_ClientId, Config::ConversationTranslatorClientId.c_str());
        }

        // set proxy for easier debugging if e.g. using fiddler
        TEST_PAL_Proxy_Info proxy;
        if (TEST_PAL_Get_Default_Proxy(proxy))
        {
            config->SetProperty(PropertyId::SpeechServiceConnection_ProxyHostName, proxy.host);
            config->SetProperty(PropertyId::SpeechServiceConnection_ProxyPort, to_string(proxy.port));
        }
    }

    static std::shared_ptr<SpeechTranslationConfig> CreateConfig(const string& lang, const vector<string> to)
    {
        shared_ptr<SpeechTranslationConfig> config;

        string subscriptionKey = FirstOrDefault({ Keys::ConversationTranslator, Keys::Speech });
        string region = FirstOrDefault({ Config::ConversationTranslatorRegion, Config::Region });

        if (Config::ConversationTranslatorSpeechEndpoint.empty())
        {
            config = SpeechTranslationConfig::FromSubscription(subscriptionKey, region);
        }
        else
        {
            config = SpeechTranslationConfig::FromEndpoint(Config::ConversationTranslatorSpeechEndpoint, subscriptionKey);
        }

        config->SetSpeechRecognitionLanguage(lang);
        for (auto t : to)
        {
            config->AddTargetLanguage(t);
        }

        SetCommonConfig(config);
        return config;
    }

    static void SetParticipantConfig(std::shared_ptr<AudioConfig> config)
    {
        if (!Config::ConversationTranslatorSpeechEndpoint.empty())
        {
            config->SetProperty(PropertyId::SpeechServiceConnection_Endpoint, Config::ConversationTranslatorSpeechEndpoint);
        }

        SetCommonConfig(config);
    }


    struct ExpectedTranscription
    {
        ExpectedTranscription() = default;
        ExpectedTranscription(string id, string text, string lang, std::initializer_list<std::pair<string, string>> tran)
            : ExpectedTranscription(id, text, lang, 1, tran)
        {
        }
        ExpectedTranscription(string id, string text, string lang, int min = 0, std::initializer_list<std::pair<string, string>> tran = {})
            : participantId(id), text(text), lang(lang), minPartials(min), translations()
        {
            for (const auto& pair : tran)
            {
                translations[pair.first] = pair.second;
            }
        }

        string participantId;
        string text;
        string lang;
        int minPartials;
        unordered_map<string, string> translations;
    };


    class ConversationTranslatorCallbacks
    {
    public:
        struct _Session
        {
            _Session() = default;
            _Session(const SessionEventArgs& args)
                : SessionId(args.SessionId)
            {
            }

            std::string SessionId;
        };
        struct _Canceled : public _Session
        {
            _Canceled() = default;
            _Canceled(const ConversationTranslationCanceledEventArgs& args) :
                _Session(args),
                Reason(args.Reason),
                ErrorCode(args.ErrorCode)
            {
            }

            CancellationReason Reason;
            CancellationErrorCode ErrorCode;
        };
        struct _Expiration : public _Session
        {
            _Expiration() = default;
            _Expiration(const ConversationExpirationEventArgs& args) :
                _Session(args),
                TimeLeft(args.ExpirationTime)
            { }

            std::chrono::minutes TimeLeft;
        };
        struct _ParticipantChanged : public _Session
        {
            _ParticipantChanged() = default;
            _ParticipantChanged(const ConversationParticipantsChangedEventArgs& args) :
                _Session(args),
                Reason(args.Reason),
                Participants()
            {
                for (auto& p : args.Participants)
                {
                    Participants.push_back(p);
                }
            }

            ParticipantChangedReason Reason;
            std::vector<std::shared_ptr<Participant>> Participants;
        };
        struct _Reco : public _Session
        {
            _Reco() = default;
            _Reco(const ConversationTranslationEventArgs& args) :
                _Session(args),
                Offset(args.Offset),
                ParticipantId(args.Result->ParticipantId),
                OriginalLang(args.Result->OriginalLanguage),
                Translations(),
                ResultId(args.Result->ResultId),
                Text(args.Result->Text),
                Reason(args.Result->Reason)
            {
                for (auto&kvp : args.Result->Translations)
                {
                    Translations[kvp.first] = kvp.second;
                }
            }

            uint64_t Offset;
            std::string ParticipantId;
            std::string OriginalLang;
            std::unordered_map<std::string, std::string> Translations;
            std::string ResultId;
            std::string Text;
            ResultReason Reason;
        };

        ConversationTranslatorCallbacks(std::shared_ptr<ConversationTranslator> convTrans);

        std::vector<_Session> SessionStarted;
        std::vector<_Session> SessionStopped;
        std::vector<_Canceled> Canceled;
        std::vector<_ParticipantChanged> ParticipantsChanged;
        std::vector<_Expiration> ConversationExpiration;
        std::vector<_Reco> Transcribing;
        std::vector<_Reco> Transcribed;
        std::vector<_Reco> TextMessageReceived;

        void WaitForAudioStreamCompletion(std::chrono::milliseconds maxWaitTime, std::chrono::milliseconds additionalWaitTime = 0ms)
        {
            SPX_TRACE_INFO("Waiting for audio stream to complete (%lldms)...", maxWaitTime.count());
            auto future = m_audioStreamFinished.get_future();
            auto status = future.wait_for(maxWaitTime);
            switch (status)
            {
                case std::future_status::ready:
                    SPX_TRACE_INFO("Audio stream completed, sleeping for an additional (%lldms)...", additionalWaitTime.count());
                    std::this_thread::sleep_for(additionalWaitTime);
                    return;

                case std::future_status::timeout:
                    SPX_TRACE_INFO("Timed out waiting for audio stream to complete");
                    throw std::runtime_error("Timed out waiting for audio to stop streaming");

                case std::future_status::deferred:
                    throw std::runtime_error("Deferred status not supported");
            }
        }

        void VerifyBasicEvents(bool expectEndOfStream, const std::string name, bool isHost, std::string& participantId)
        {
            SPXTEST_REQUIRE(SessionStarted.size() > 0);
            SPXTEST_REQUIRE(SessionStopped.size() > 0);

            if (expectEndOfStream)
            {
                SPXTEST_REQUIRE(Canceled.size() == 1);
                SPXTEST_REQUIRE(Canceled[0].Reason == CancellationReason::EndOfStream);
                SPXTEST_REQUIRE(Canceled[0].ErrorCode == CancellationErrorCode::NoError);
            }
            else
            {
                SPXTEST_REQUIRE(Canceled.size() == 0);
            }

            SPXTEST_REQUIRE(ParticipantsChanged.size() >= 2);
            SPXTEST_REQUIRE(ParticipantsChanged[0].Reason == Transcription::ParticipantChangedReason::JoinedConversation);
            SPXTEST_REQUIRE(ParticipantsChanged[1].Reason == Transcription::ParticipantChangedReason::Updated);

            // get the participant ID from the name changed event
            participantId = StringUtils::ToUpper(
                First(
                    ParticipantsChanged[1].Participants,
                    [&name](auto ptr) { return ptr->DisplayName == name; })
                ->Id);

            if (isHost)
            {
                SPXTEST_REQUIRE(ParticipantsChanged[0].Participants.size() == 1);
                SPXTEST_REQUIRE(ParticipantsChanged[0].Participants.front()->IsHost);
            }
            else
            {
                SPXTEST_REQUIRE(ParticipantsChanged[0].Participants.size() >= 2);
                SPXTEST_REQUIRE_NOTHROW(First(ParticipantsChanged[0].Participants, [participantId](auto ptr) { return StringUtils::ToUpper(ptr->Id) == participantId; }));
            }

            SPXTEST_REQUIRE(ParticipantsChanged[1].Participants.size() == 1);
            SPXTEST_REQUIRE(ParticipantsChanged[1].Participants.front()->DisplayName == name);
        }

        void VerifyTranscriptions(const std::string participantId, std::initializer_list<ExpectedTranscription> expectedTranscriptions)
        {
            std::unordered_map<std::string, std::vector<ConversationTranslatorCallbacks::_Reco>> partials;
            std::unordered_map<std::string, std::vector<ConversationTranslatorCallbacks::_Reco>> finals;

            for (const auto& evt : Transcribing)
            {
                partials[StringUtils::ToUpper(evt.ParticipantId)].push_back(evt);
            }

            for (const auto& evt : Transcribed)
            {
                finals[StringUtils::ToUpper(evt.ParticipantId)].push_back(evt);
            }

            for (const auto& e : expectedTranscriptions)
            {
                auto expectedParticipantId = StringUtils::ToUpper(e.participantId);
                auto p = partials[expectedParticipantId];
                auto f = finals[expectedParticipantId];

                auto expectedPartialReason = expectedParticipantId == participantId
                    ? ResultReason::TranslatingSpeech
                    : ResultReason::TranslatingParticipantSpeech;
                auto expectedFinalReason = expectedParticipantId == participantId
                    ? ResultReason::TranslatedSpeech
                    : ResultReason::TranslatedParticipantSpeech;

                // find an utterance that matches the expected string
                ConversationTranslatorCallbacks::_Reco match;
                SPXTEST_REQUIRE_NOTHROW((match = First(f, [y = e.text](auto x) { return x.Text == y; })));

                SPXTEST_REQUIRE_THAT(match.Text, Catch::Equals(e.text, Catch::CaseSensitive::No));
                SPXTEST_REQUIRE(match.OriginalLang == e.lang);
                SPXTEST_REQUIRE_THAT(match.ParticipantId, Catch::Equals(e.participantId, Catch::CaseSensitive::No));
                SPXTEST_REQUIRE(match.Reason == expectedFinalReason);
                SPXTEST_REQUIRE(match.Translations.size() >= e.translations.size());
                for (const auto& expectedEntry : e.translations)
                {
                    SPXTEST_REQUIRE_THAT(match.Translations[expectedEntry.first], Catch::Equals(expectedEntry.second, Catch::CaseSensitive::No));
                }

                int numPartials = 0;
                for (size_t i = 0; i < p.size(); i++)
                {
                    if (p[i].ResultId == match.ResultId)
                    {
                        numPartials++;
                        SPXTEST_REQUIRE(p[i].Reason == expectedPartialReason);
                    }
                }

                SPXTEST_REQUIRE(numPartials >= e.minPartials);
            }
        }

        void VerifyIms(const std::string participantId, std::initializer_list<ExpectedTranscription> expectedIms)
        {
            std::unordered_map<std::string, std::vector<ConversationTranslatorCallbacks::_Reco>> ims;
            for (const auto& evt : TextMessageReceived)
            {
                ims[StringUtils::ToUpper(evt.ParticipantId)].push_back(evt);
            }

            for (const auto& expected : expectedIms)
            {
                auto expectedParticipantId = StringUtils::ToUpper(expected.participantId);
                auto receivedFromUser = ims[expectedParticipantId];

                auto expectedReason = expectedParticipantId == participantId
                    ? ResultReason::TranslatedInstantMessage
                    : ResultReason::TranslatedParticipantInstantMessage;

                // find an IM that matches the expected string
                ConversationTranslatorCallbacks::_Reco match;
                SPXTEST_REQUIRE_NOTHROW((match = First(receivedFromUser, [y = expected.text](auto x) { return x.Text == y; })));

                SPXTEST_REQUIRE_THAT(match.Text, Catch::Equals(expected.text, Catch::CaseSensitive::No));
                SPXTEST_REQUIRE(match.OriginalLang == expected.lang);
                SPXTEST_REQUIRE_THAT(match.ParticipantId, Catch::Equals(expected.participantId, Catch::CaseSensitive::No));
                SPXTEST_REQUIRE(match.Reason == expectedReason);
                SPXTEST_REQUIRE(match.Translations.size() >= expected.translations.size());
                for (const auto& expectedEntry : expected.translations)
                {
                    SPXTEST_REQUIRE_THAT(match.Translations[expectedEntry.first], Catch::Equals(expectedEntry.second, Catch::CaseSensitive::No));
                }
            }
        }

    private:
        std::promise<void> m_audioStreamFinished;

    private:
        template<typename TArgs, typename TStore>
        void Bind(std::vector<TStore>& list, EventSignal<TArgs>& evt, const std::string& name)
        {
            evt.Connect([&list, name](TArgs arg)
            {
                SPX_TRACE_INFO("%s: Received event", name.c_str());
                list.push_back(arg);
            });
        }
    };

    template<>
    void ConversationTranslatorCallbacks::Bind(std::vector<_Canceled>& list, EventSignal<const ConversationTranslationCanceledEventArgs&>& evt, const std::string& name)
    {
        evt.Connect([&list, name, this](const ConversationTranslationCanceledEventArgs& arg)
        {
            SPX_TRACE_INFO(">>%s: Received event. Reason: %d, Error: %d, Details: %s", name.c_str(), arg.Reason, arg.ErrorCode, arg.ErrorDetails.c_str());

            _Canceled parsed(arg);
            try
            {
                if (parsed.Reason == CancellationReason::EndOfStream)
                {
                    m_audioStreamFinished.set_value();
                }
                else
                {
                    std::string error = std::string("cancel message indicating error ") + std::to_string((int)parsed.ErrorCode);
                    throw std::runtime_error(error.c_str());
                }
            }
            catch (std::exception&)
            {
                m_audioStreamFinished.set_exception(std::current_exception());
            }

            list.push_back(parsed);
        });
    }

    ConversationTranslatorCallbacks::ConversationTranslatorCallbacks(std::shared_ptr<ConversationTranslator> convTrans)
    {
        Bind(SessionStarted, convTrans->SessionStarted, "SessionStarted");
        Bind(SessionStopped, convTrans->SessionStopped, "SessionStopped");
        Bind(Canceled, convTrans->Canceled, "Canceled");
        Bind(ParticipantsChanged, convTrans->ParticipantsChanged, "ParticipantsChanged");
        Bind(ConversationExpiration, convTrans->ConversationExpiration, "ConversationExpiration");
        Bind(Transcribing, convTrans->Transcribing, "Transcribing");
        Bind(Transcribed, convTrans->Transcribed, "Transcribed");
        Bind(TextMessageReceived, convTrans->TextMessageReceived, "TextMessageReceived");
    }

    struct TestConversationParticipant
    {
    private:
        shared_ptr<SpeechConfig> m_config;
        string m_convId;
        string m_partId;

    public:
        TestConversationParticipant(shared_ptr<SpeechConfig> config, const string& nickname) :
            m_config(config),
            m_convId(),
            m_partId(),
            IsHost(true),
            Name(nickname),
            Lang(config->GetSpeechRecognitionLanguage()),
            ConversationId(m_convId),
            ParticipantId(m_partId),
            Conv(),
            ConvTrans(),
            Events()
        {
        }

        TestConversationParticipant(const string& nickname, const string& lang, const TestConversationParticipant& other) :
            m_config(nullptr),
            m_convId(other.Conv->GetConversationId()),
            m_partId(),
            IsHost(false),
            Name(nickname),
            Lang(lang),
            ConversationId(m_convId),
            ParticipantId(m_partId),
            Conv(),
            ConvTrans(),
            Events()
        {
        }

        ~TestConversationParticipant() = default;

        void Join(std::shared_ptr<AudioConfig> audioConfig)
        {
            if (IsHost)
            {
                SPX_TRACE_INFO(">> [%s] Creating conversation", Name.c_str());
                Conv = Conversation::CreateConversationAsync(m_config).get();
                m_convId = Conv->GetConversationId();
                SPX_TRACE_INFO(">> [%s] Conversation Id: '%s'", Name.c_str(), m_convId.c_str());
                SPXTEST_REQUIRE(false == m_convId.empty());

                SPX_TRACE_INFO(">> [%s] Starting conversation", Name.c_str());
                Conv->StartConversationAsync().get();

                SPX_TRACE_INFO(">> [%s] Creating conversation translator", Name.c_str());
                ConvTrans = ConversationTranslator::FromConfig(audioConfig);
                Events = make_unique<ConversationTranslatorCallbacks>(ConvTrans);

                SPX_TRACE_INFO(">> [%s] Joining conversation", Name.c_str());
                ConvTrans->JoinConversationAsync(Conv, Name).get();
            }
            else
            {
                SPX_TRACE_INFO(">> [%s] Creating conversation translator", Name.c_str());
                ConvTrans = ConversationTranslator::FromConfig(audioConfig);
                Events = make_unique<ConversationTranslatorCallbacks>(ConvTrans);

                SPX_TRACE_INFO(">> [%s] Joining conversation %s", Name.c_str(), m_convId.c_str());
                ConvTrans->JoinConversationAsync(m_convId, Name, Lang).get();
            }
        }

        void StartAudio()
        {
            SPX_TRACE_INFO(">> [%s] Starting audio", Name.c_str());
            ConvTrans->StartTranscribingAsync().get();
        }

        void WaitForAudioToFinish(std::chrono::milliseconds maxWait = 15000ms)
        {
            SPX_TRACE_INFO(">> [%s] Waiting for audio to complete", Name.c_str());
            Events->WaitForAudioStreamCompletion(maxWait, 2000ms);
            SPX_TRACE_INFO(">> [%s] Audio has completed", Name.c_str());
        }

        void StopAudio()
        {
            SPX_TRACE_INFO(">> [%s] Stopping audio", Name.c_str());
            ConvTrans->StopTranscribingAsync().get();
        }

        void Leave()
        {
            if (ConvTrans)
            {
                SPX_TRACE_INFO(">> [%s] Leaving conversation", Name.c_str());
                ConvTrans->LeaveConversationAsync().get();
            }

            if (Conv)
            {
                SPX_TRACE_INFO(">> [%s] Ending conversation", Name.c_str());
                Conv->EndConversationAsync().get();
                SPX_TRACE_INFO(">> [%s] Deleting conversation", Name.c_str());
                Conv->DeleteConversationAsync().get();

                SPX_TRACE_INFO(">> [%s] DONE!", Name.c_str());
            }
        }

        void VerifyBasicEvents(bool expectEndOfStream)
        {
            Events->VerifyBasicEvents(expectEndOfStream, Name, IsHost, m_partId);
        }

        void VerifyTranscriptions(std::initializer_list<ExpectedTranscription> expectedTranscriptions)
        {
            Events->VerifyTranscriptions(ParticipantId, std::move(expectedTranscriptions));
        }

        void VerifyIms(std::initializer_list<ExpectedTranscription> expectedIms)
        {
            Events->VerifyIms(ParticipantId, std::move(expectedIms));
        }

        const bool IsHost;
        const std::string Name;
        const std::string Lang;
        const std::string& ConversationId;
        const std::string& ParticipantId;
        shared_ptr<Conversation> Conv;
        shared_ptr<ConversationTranslator> ConvTrans;
        unique_ptr<ConversationTranslatorCallbacks> Events;
    };

#ifdef _MSC_VER
#pragma warning(pop)
#endif



}}}}

