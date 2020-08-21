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
#include <speechapi_cxx_connection_eventargs.h>
#include <speechapi_cxx_speech_recognition_eventargs.h>
#include <speechapi_cxx_speech_recognizer.h>
#include <string_utils.h>
#include <http_request.h>
#include <http_response.h>
#include "test_PAL.h"
#include "conversation_utils.h"
#include "translator_languages.h"
#include "authentication_token_provider.h"

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


    constexpr std::chrono::milliseconds MAX_WAIT_FOR_AUDIO_TO_COMPLETE(20s);
    constexpr std::chrono::milliseconds WAIT_AFTER_AUDIO_COMPLETE(2s);


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

    static std::string GetSubscriptionKey()
    {
        return FirstOrDefault(
        {
            SubscriptionsRegionsMap[CONVERSATION_TRANSLATOR_SUBSCRIPTION].Key,
            SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Key
        });
    }

    static std::string GetRegion()
    {
        return FirstOrDefault(
        {
            SubscriptionsRegionsMap[CONVERSATION_TRANSLATOR_SUBSCRIPTION].Region,
            SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Region
        });
    }

    template<typename I>
    static void SetCommonConfig(std::shared_ptr<I> config)
    {
        if (false == DefaultSettingsMap[CONVERSATION_TRANSLATOR_HOST].empty())
        {
            config->SetProperty(ConversationKeys::Conversation_Management_Endpoint, (string("https://") + DefaultSettingsMap[CONVERSATION_TRANSLATOR_HOST] + "/capito/room").c_str());
            config->SetProperty(ConversationKeys::Conversation_Endpoint, (string("wss://") + DefaultSettingsMap[CONVERSATION_TRANSLATOR_HOST] + "/capito/translate").c_str());
        }

        config->SetProperty(PropertyId::Speech_SessionId, string("IntegrationTest:") + to_string(chrono::high_resolution_clock::now().time_since_epoch().count()));

        // set conversation specific overrides
        if (!SubscriptionsRegionsMap[CONVERSATION_TRANSLATOR_SUBSCRIPTION].Key.empty())
        {
            config->SetProperty(ConversationKeys::ConversationServiceConnection_Key, SubscriptionsRegionsMap[CONVERSATION_TRANSLATOR_SUBSCRIPTION].Key.c_str());
        }

        if (!SubscriptionsRegionsMap[CONVERSATION_TRANSLATOR_SUBSCRIPTION].Region.empty())
        {
            config->SetProperty(ConversationKeys::Conversation_Region, SubscriptionsRegionsMap[CONVERSATION_TRANSLATOR_SUBSCRIPTION].Region.c_str());
        }

        if (!DefaultSettingsMap[CONVERSATION_TRANSLATOR_CLIENTID].empty())
        {
            config->SetProperty(ConversationKeys::Conversation_ClientId, DefaultSettingsMap[CONVERSATION_TRANSLATOR_CLIENTID].c_str());
        }

        // set proxy for easier debugging if e.g. using fiddler
        TEST_PAL_Proxy_Info proxy;
        if (TEST_PAL_Get_Default_Proxy(proxy))
        {
            config->SetProperty(PropertyId::SpeechServiceConnection_ProxyHostName, proxy.host);
            config->SetProperty(PropertyId::SpeechServiceConnection_ProxyPort, to_string(proxy.port));
        }
    }

    static std::shared_ptr<SpeechTranslationConfig> CreateConfig(const string& lang, const vector<string> to, chrono::seconds validity, const char* file, int line)
    {
        // FIXME ralphe: Merge with Rob's traffic type changes
        (void)file;
        (void)line;
        shared_ptr<SpeechTranslationConfig> config;

        string subscriptionKey = GetSubscriptionKey();
        string region = GetRegion();
        string endpoint = DefaultSettingsMap[CONVERSATION_TRANSLATOR_SPEECH_ENDPOINT];

        string authToken;
        if (validity == 0s)
        {
            authToken = AuthenticationTokenProvider::GetOrCreateToken(subscriptionKey, region);
        }
        else
        {
            authToken = AuthenticationTokenProvider::CreateToken(subscriptionKey, region, validity);
        }

        if (endpoint.empty())
        {
            config = SpeechTranslationConfig::FromAuthorizationToken(authToken, region);
        }
        else
        {
            config = SpeechTranslationConfig::FromEndpoint(endpoint);
            config->SetAuthorizationToken(authToken);
        }

        config->SetSpeechRecognitionLanguage(lang);
        for (auto t : to)
        {
            config->AddTargetLanguage(t);
        }
        
        config->SetServiceProperty("TrafficType", SpxGetTestTrafficType(file, line), ServicePropertyChannel::UriQueryParameter);

        SetCommonConfig(config);
        return config;
    }

    static std::shared_ptr<SpeechTranslationConfig> CreateConfig(const string& lang, const vector<string> to, const char* file, int line)
    {
        return CreateConfig(lang, to, 0s, file, line);
    }

#define CreateConfig(lang, ...) CreateConfig(lang, __VA_ARGS__, __FILE__, __LINE__)

    static void SetParticipantConfig(std::shared_ptr<AudioConfig> config, const char * file, int line)
    {
        if (!DefaultSettingsMap[CONVERSATION_TRANSLATOR_SPEECH_ENDPOINT].empty())
        {
            config->SetProperty(PropertyId::SpeechServiceConnection_Endpoint, DefaultSettingsMap[CONVERSATION_TRANSLATOR_SPEECH_ENDPOINT]);
        }

        // manually set the user defined query URL
        const std::string propertyName = GetPropertyName(PropertyId::SpeechServiceConnection_UserDefinedQueryParameters);
        std::string queryParam = config->GetProperty(propertyName);
        if (!queryParam.empty())
        {
            queryParam += "&";
        }

        queryParam += "TrafficType=" + HttpUtils::UrlEscape(SpxGetTestTrafficType(file, line));
        config->SetProperty(propertyName, queryParam);

        SetCommonConfig(config);
    }

#define SetParticipantConfig(...) SetParticipantConfig(__VA_ARGS__, __FILE__, __LINE__)


    struct ExpectedTranscription
    {
        ExpectedTranscription(const string& id, const string& text, const string& lang, std::initializer_list<std::pair<string, string>> tran)
            : ExpectedTranscription(id, text, lang, 0, tran)
        {
        }
        ExpectedTranscription(const string& id, const string& text, const string& lang, int min = 0, std::initializer_list<std::pair<string, string>> tran = {})
            : participantId(id), text(text), lang(TranslatorTextLanguage::Parse(lang)), minPartials(min), translations()
        {
            for (const auto& pair : tran)
            {
                translations[pair.first] = pair.second;
            }
        }

        string participantId;
        string text;
        TranslatorTextLanguage lang;
        int minPartials;
        unordered_map<string, string> translations;
    };


    class ConversationTranslatorCallbacks : public std::enable_shared_from_this<ConversationTranslatorCallbacks>
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
        struct _Connection
        {
            _Connection() = default;
            _Connection(const ConnectionEventArgs& args)
                : SessionId(args.SessionId)
            {
            }

            std::string SessionId;
        };
        struct _Canceled : public _Session
        {
            _Canceled() = default;
            _Canceled(const SpeechRecognitionCanceledEventArgs& args) :
                _Session(args),
                Reason(args.Reason),
                ErrorCode(args.ErrorCode)
            {
            }
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
            _Reco(const SpeechRecognitionEventArgs& args) :
                _Session(args),
                Offset(args.Offset),
                Translations(),
                ResultId(args.Result->ResultId),
                Text(args.Result->Text),
                Reason(args.Result->Reason)
            {
            }
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
                    auto textLang = TranslatorTextLanguage::Parse(kvp.first);
                    Translations[textLang.Code()] = kvp.second;
                    Translations[textLang.Lang()] = kvp.second;

                    auto speechLang = TranslatorSpeechLanguage::Parse(kvp.first);
                    Translations[speechLang.Code()] = kvp.second;
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

        static std::shared_ptr<ConversationTranslatorCallbacks> From(std::shared_ptr<ConversationTranslator> convTrans);
        static std::shared_ptr<ConversationTranslatorCallbacks> From(std::shared_ptr<SpeechRecognizer> reco);

        std::vector<_Session> SessionStarted;
        std::vector<_Session> SessionStopped;
        std::vector<_Connection> Connected;
        std::vector<_Connection> Disconnected;
        std::vector<_Canceled> Canceled;
        std::vector<_ParticipantChanged> ParticipantsChanged;
        std::vector<_Expiration> ConversationExpiration;
        std::vector<_Reco> Transcribing;
        std::vector<_Reco> Transcribed;
        std::vector<_Reco> TextMessageReceived;

        void AddConnectionCallbacks(std::shared_ptr<Connection> connection)
        {
            Bind(Connected, connection->Connected, "Connected");
            Bind(Disconnected, connection->Disconnected, "Disconnected");

            m_hasConnection = true;
        }

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

        void VerifySessionAndConnectEvents(bool expectEndOfStream)
        {
            SPXTEST_REQUIRE(SessionStarted.size() > 0);
            SPXTEST_REQUIRE(SessionStopped.size() > 0);

            if (m_hasConnection)
            {
                SPXTEST_REQUIRE(Connected.size() > 0);
                SPXTEST_REQUIRE(Disconnected.size() > 0);
            }

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
        }

        void VerifyBasicEvents(bool expectEndOfStream, const std::string name, bool isHost, std::string& participantId)
        {
            VerifySessionAndConnectEvents(expectEndOfStream);

            SPXTEST_REQUIRE(ParticipantsChanged.size() >= 2);
            SPXTEST_REQUIRE(ParticipantsChanged[0].Reason == Transcription::ParticipantChangedReason::JoinedConversation);
            SPXTEST_REQUIRE(ParticipantsChanged[1].Reason == Transcription::ParticipantChangedReason::Updated);

            // get the participant ID from the name changed event
            participantId =
                First(
                    ParticipantsChanged[1].Participants,
                    [&name](auto ptr) { return ptr->DisplayName == name; })
                ->Id;

            if (isHost)
            {
                SPXTEST_REQUIRE(ParticipantsChanged[0].Participants.size() == 1);
                SPXTEST_REQUIRE(ParticipantsChanged[0].Participants.front()->IsHost);
            }
            else
            {
                SPXTEST_REQUIRE(ParticipantsChanged[0].Participants.size() >= 2);
                SPXTEST_REQUIRE_NOTHROW(First(ParticipantsChanged[0].Participants, [participantId](auto ptr) { return ptr->Id == participantId; }));
            }

            SPXTEST_REQUIRE(ParticipantsChanged[1].Participants.size() == 1);
            SPXTEST_REQUIRE(ParticipantsChanged[1].Participants.front()->DisplayName == name);
        }

        void VerifyConnectionEvents(size_t expectedConnections, size_t expectedDisconnections)
        {
            SPXTEST_REQUIRE(Connected.size() == expectedConnections);
            SPXTEST_REQUIRE(Disconnected.size() == expectedDisconnections);
        }

        void VerifyTranscriptions(const std::string participantId, std::initializer_list<ExpectedTranscription> expectedTranscriptions)
        {
            std::unordered_map<std::string, std::vector<ConversationTranslatorCallbacks::_Reco>> partials;
            std::unordered_map<std::string, std::vector<ConversationTranslatorCallbacks::_Reco>> finals;

            for (const auto& evt : Transcribing)
            {
                partials[evt.ParticipantId].push_back(evt);
            }

            for (const auto& evt : Transcribed)
            {
                finals[evt.ParticipantId].push_back(evt);
            }

            for (const auto& e : expectedTranscriptions)
            {
                auto expectedParticipantId = e.participantId;
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
                try
                {
                    match = First(f, [y = e.text](auto x)
                    {
                        return x.Text == y
                            || PAL::stricmp(x.Text.c_str(), y.c_str()) == 0
                            || StringComparisions::AssertFuzzyMatch(x.Text, y);
                    });
                }
                catch (const std::exception&)
                {
                    std::ostringstream oss;
                    oss << "Could not find final that matches '" << e.text << "'" << std::endl;
                    oss << "Finals found:" << std::endl;
                    for (const auto& t : f)
                    {
                        oss << "  [" << t.ParticipantId << "] '" << t.Text << "'" << std::endl;
                    }

                    FAIL(oss.str().c_str());
                }

                SPXTEST_REQUIRE(TranslatorSpeechLanguage::Parse(match.OriginalLang).Equals(e.lang));
                SPXTEST_REQUIRE_THAT(match.ParticipantId, Catch::Equals(e.participantId, Catch::CaseSensitive::No));
                SPXTEST_REQUIRE(match.Reason == expectedFinalReason);
                SPXTEST_REQUIRE(match.Translations.size() >= e.translations.size());


                for (const auto& expectedEntry : e.translations)
                {
                    auto textLang = TranslatorSpeechLanguage::Parse(expectedEntry.first).TextLanguage();

                    SPXTEST_REQUIRE_THAT(match.Translations[textLang.Code()], Catch::FuzzyMatch(expectedEntry.second));
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
                ims[evt.ParticipantId].push_back(evt);
            }

            for (const auto& expected : expectedIms)
            {
                auto expectedParticipantId = expected.participantId;
                auto receivedFromUser = ims[expectedParticipantId];

                auto expectedReason = expectedParticipantId == participantId
                    ? ResultReason::TranslatedInstantMessage
                    : ResultReason::TranslatedParticipantInstantMessage;

                // find an IM that matches the expected string
                ConversationTranslatorCallbacks::_Reco match;
                try
                {
                    match = First(receivedFromUser, [y = expected.text](auto x)
                    {
                        return x.Text == y
                            || PAL::stricmp(x.Text.c_str(), y.c_str()) == 0
                            || StringComparisions::AssertFuzzyMatch(x.Text, y);
                    });
                }
                catch (const std::exception&)
                {
                    std::ostringstream oss;
                    oss << "Could not find instant message that matches '" << expected.text << "'" << std::endl;
                    oss << "Finals found:" << std::endl;
                    for (const auto& im : receivedFromUser)
                    {
                        oss << "  [" << im.ParticipantId << "] '" << im.Text << "'" << std::endl;
                    }

                    FAIL(oss.str().c_str());
                }

                SPXTEST_REQUIRE(TranslatorSpeechLanguage::Parse(match.OriginalLang).Equals(expected.lang));
                SPXTEST_REQUIRE_THAT(match.ParticipantId, Catch::Equals(expected.participantId, Catch::CaseSensitive::No));
                SPXTEST_REQUIRE(match.Reason == expectedReason);
                SPXTEST_REQUIRE(match.Translations.size() >= expected.translations.size());
                for (const auto& expectedEntry : expected.translations)
                {
                    auto textLang = TranslatorSpeechLanguage::Parse(expectedEntry.first).TextLanguage();

                    SPXTEST_REQUIRE_THAT(match.Translations[textLang.Code()], Catch::FuzzyMatch(expectedEntry.second));
                }
            }
        }

    private:
        std::promise<void> m_audioStreamFinished;
        bool m_hasConnection;

    private:
        ConversationTranslatorCallbacks()
            : m_audioStreamFinished(), m_hasConnection(false)
        {
        }

        template<typename TArgs, typename TStore>
        void Bind(std::vector<TStore>& list, EventSignal<TArgs>& evt, const std::string& name)
        {
            auto keepalive = shared_from_this();
            evt.Connect([&list, name, keepalive](TArgs arg)
            {
                SPX_TRACE_INFO("%s: Received event", name.c_str());
                list.push_back(arg);
            });
        }
    };

    template<>
    void ConversationTranslatorCallbacks::Bind(std::vector<_Canceled>& list, EventSignal<const ConversationTranslationCanceledEventArgs&>& evt, const std::string& name)
    {
        auto keepalive = shared_from_this();
        evt.Connect([&list, name, keepalive](const ConversationTranslationCanceledEventArgs& arg)
        {
            SPX_TRACE_INFO(">>%s: Received event. Reason: %d, Error: %d, Details: %s", name.c_str(), arg.Reason, arg.ErrorCode, arg.ErrorDetails.c_str());

            _Canceled parsed(arg);

            list.push_back(parsed);

            try
            {
                if (parsed.Reason == CancellationReason::EndOfStream)
                {
                    keepalive->m_audioStreamFinished.set_value();
                }
                else
                {
                    std::string error = std::string("cancel message indicating error ") + std::to_string((int)parsed.ErrorCode);
                    throw std::runtime_error(error.c_str());
                }
            }
            catch (std::exception&)
            {
                keepalive->m_audioStreamFinished.set_exception(std::current_exception());
            }
        });
    }

    template<>
    void ConversationTranslatorCallbacks::Bind(std::vector<_Canceled>& list, EventSignal<const SpeechRecognitionCanceledEventArgs&>& evt, const std::string& name)
    {
        auto keepalive = shared_from_this();
        evt.Connect([&list, name, keepalive](const SpeechRecognitionCanceledEventArgs& arg)
        {
            SPX_TRACE_INFO(">>%s: Received event. Reason: %d, Error: %d, Details: %s", name.c_str(), arg.Reason, arg.ErrorCode, arg.ErrorDetails.c_str());

            _Canceled parsed(arg);

            list.push_back(parsed);

            try
            {
                if (parsed.Reason == CancellationReason::EndOfStream)
                {
                    keepalive->m_audioStreamFinished.set_value();
                }
                else
                {
                    std::string error = std::string("cancel message indicating error ") + std::to_string((int)parsed.ErrorCode);
                    throw std::runtime_error(error.c_str());
                }
            }
            catch (std::exception&)
            {
                keepalive->m_audioStreamFinished.set_exception(std::current_exception());
            }
        });
    }

    std::shared_ptr<ConversationTranslatorCallbacks> ConversationTranslatorCallbacks::From(std::shared_ptr<ConversationTranslator> convTrans)
    {
        auto evts = std::shared_ptr<ConversationTranslatorCallbacks>(new ConversationTranslatorCallbacks());

        evts->Bind(evts->SessionStarted, convTrans->SessionStarted, "SessionStarted");
        evts->Bind(evts->SessionStopped, convTrans->SessionStopped, "SessionStopped");
        evts->Bind(evts->Canceled, convTrans->Canceled, "Canceled");
        evts->Bind(evts->ParticipantsChanged, convTrans->ParticipantsChanged, "ParticipantsChanged");
        evts->Bind(evts->ConversationExpiration, convTrans->ConversationExpiration, "ConversationExpiration");
        evts->Bind(evts->Transcribing, convTrans->Transcribing, "Transcribing");
        evts->Bind(evts->Transcribed, convTrans->Transcribed, "Transcribed");
        evts->Bind(evts->TextMessageReceived, convTrans->TextMessageReceived, "TextMessageReceived");

        return evts;
    }

    std::shared_ptr<ConversationTranslatorCallbacks> ConversationTranslatorCallbacks::From(std::shared_ptr<SpeechRecognizer> reco)
    {
        auto evts = std::shared_ptr<ConversationTranslatorCallbacks>(new ConversationTranslatorCallbacks());

        evts->Bind(evts->SessionStarted, reco->SessionStarted, "SessionStarted");
        evts->Bind(evts->SessionStopped, reco->SessionStopped, "SessionStopped");
        evts->Bind(evts->Canceled, reco->Canceled, "Canceled");
        evts->Bind(evts->Transcribing, reco->Recognizing, "Recognizing");
        evts->Bind(evts->Transcribed, reco->Recognized, "Recognized");

        return evts;
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
            if (audioConfig == nullptr)
            {
                auto utterance = AudioUtterancesMap[SINGLE_UTTERANCE_ENGLISH];
                REQUIRE(exists(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH)));
                audioConfig = AudioConfig::FromWavFileInput(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH));
            }

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
                Events = ConversationTranslatorCallbacks::From(ConvTrans);

                SPX_TRACE_INFO(">> [%s] Creating connection", Name.c_str());
                Conn = Connection::FromConversationTranslator(ConvTrans);
                Events->AddConnectionCallbacks(Conn);

                SPX_TRACE_INFO(">> [%s] Joining conversation", Name.c_str());
                ConvTrans->JoinConversationAsync(Conv, Name).get();
            }
            else
            {
                SPX_TRACE_INFO(">> [%s] Creating conversation translator", Name.c_str());
                ConvTrans = ConversationTranslator::FromConfig(audioConfig);
                Events = ConversationTranslatorCallbacks::From(ConvTrans);

                SPX_TRACE_INFO(">> [%s] Creating connection", Name.c_str());
                Conn = Connection::FromConversationTranslator(ConvTrans);
                Events->AddConnectionCallbacks(Conn);

                SPX_TRACE_INFO(">> [%s] Joining conversation %s", Name.c_str(), m_convId.c_str());
                ConvTrans->JoinConversationAsync(m_convId, Name, Lang).get();
            }
        }

        void StartAudio()
        {
            SPX_TRACE_INFO(">> [%s] Starting audio", Name.c_str());
            ConvTrans->StartTranscribingAsync().get();
        }

        void WaitForAudioToFinish(std::chrono::milliseconds maxWait = MAX_WAIT_FOR_AUDIO_TO_COMPLETE)
        {
            SPX_TRACE_INFO(">> [%s] Waiting for audio to complete", Name.c_str());
            Events->WaitForAudioStreamCompletion(maxWait, WAIT_AFTER_AUDIO_COMPLETE);
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
        shared_ptr<Connection> Conn;
        shared_ptr<ConversationTranslatorCallbacks> Events;
    };

#ifdef _MSC_VER
#pragma warning(pop)
#endif



}}}}

