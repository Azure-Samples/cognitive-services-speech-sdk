//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "stdafx.h"
#include <create_object_helpers.h>
#include <http_utils.h>
#include <string_utils.h>
#include <thread_service.h>
#include "conversation_impl.h"
#include "conversation_manager.h"
#include "conversation_utils.h"


using namespace std;

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace USP {

    extern void PlatformInit(const char* proxyHost, int proxyPort, const char* proxyUsername, const char* proxyPassword);

}}}} // Microsoft::CognitiveServices::Speech::USP

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {
namespace ConversationTranslation {

    using StringUtils = PAL::StringUtils;

    static constexpr auto UNSET_DEFAULT_VALUE = "!!<<NOT_SET>>!!";

    static inline const string __GetString(shared_ptr<ISpxNamedProperties> properties, PropertyId id)
    {
        const char * propertyName = GetPropertyName(id);
        if (propertyName == nullptr)
        {
            SPX_THROW_HR(SPXERR_RUNTIME_ERROR);
        }
        
        return properties->GetStringValue(propertyName, UNSET_DEFAULT_VALUE);
    }

    static inline const string GetString(shared_ptr<ISpxNamedProperties> properties, PropertyId id, const char * defaultValue = nullptr)
    {
        string value = __GetString(properties, id);
        return value == UNSET_DEFAULT_VALUE
            ? string(defaultValue == nullptr ? "" : defaultValue)
            : value;
    }

    static inline const string GetString(shared_ptr<ISpxNamedProperties> properties, initializer_list<std::string> ids, const char * defaultValue = nullptr)
    {
        for (const std::string& id : ids)
        {
            string value = properties->GetStringValue(id.c_str(), UNSET_DEFAULT_VALUE);
            if (value != UNSET_DEFAULT_VALUE)
            {
                return value;
            }
        }

        return defaultValue == nullptr ? "" : defaultValue;
    }

    static inline int32_t GetInt(shared_ptr<ISpxNamedProperties> properties, PropertyId id, int32_t defaultValue = -1)
    {
        string value = __GetString(properties, id);
        if (value == UNSET_DEFAULT_VALUE || value.empty())
        {
            return defaultValue;
        }

        return static_cast<int32_t>(std::stol(value));
    }

    CSpxConversationImpl::CSpxConversationImpl()
        : m_conversationId(), m_args(), m_manager(), m_client(), m_connection(), m_permanentRoom(false)
    {
        SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    }

    CSpxConversationImpl::~CSpxConversationImpl()
    {
        SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

        try
        {
            Term();

            m_connection.reset();
            m_client.reset();
            m_manager.reset();
            m_args.reset();
        }
        catch (const ExceptionWithCallStack& ex)
        {
            SPX_TRACE_ERROR("Caught exception in CSpxConversationImpl disposer. Message: '%s'. Stack: %s", ex.what(), ex.GetCallStack());
        }
        catch (const std::exception& ex)
        {
            SPX_TRACE_ERROR("Caught exception in CSpxConversationImpl disposer. Message: '%s'", ex.what());
        }
        catch (...)
        {
            SPX_TRACE_ERROR("Caught a non-exception item thrown in CSpxConversationImpl disposer.");
        }
    }

    void CSpxConversationImpl::Init()
    {
        SPX_DBG_TRACE_FUNCTION();
        ThreadingHelpers::Init();
        
        std::shared_ptr<ISpxRecognizerSite> site = GetSite();

        auto properties = SpxQueryService<ISpxNamedProperties>(site);
        SPX_IFTRUE_THROW_HR(properties == nullptr, SPXERR_UNEXPECTED_CONVERSATION_SITE_FAILURE);

        // proxy server info
        string proxyHost = GetString(properties, PropertyId::SpeechServiceConnection_ProxyHostName);
        const int32_t proxyPort = GetInt(properties, PropertyId::SpeechServiceConnection_ProxyPort, 0);
        string proxyUserName = GetString(properties, PropertyId::SpeechServiceConnection_ProxyUserName);
        string proxyPassword = GetString(properties, PropertyId::SpeechServiceConnection_ProxyPassword);

        // Initialize websocket platform
        USP::PlatformInit(nullptr, 0, nullptr, nullptr);

        // Create the conversation manager object
        string restEndpoint = properties->GetStringValue(ConversationKeys::Conversation_Management_Endpoint);
        if (false == restEndpoint.empty())
        {
            // parse out the URL parts
            Url restEndpointParts = HttpUtils::ParseUrl(restEndpoint);

            m_manager = make_shared<CSpxConversationManager>(restEndpointParts.host);
            m_manager->SetPath(restEndpointParts.path);
            m_manager->SetPort(restEndpointParts.port);
            m_manager->SetSecure(restEndpointParts.secure);
        }
        else
        {
            m_manager = make_shared<CSpxConversationManager>(ConversationConstants::Host);
        }

        if (false == proxyHost.empty())
        {
            m_manager->SetProxy(proxyHost.c_str(), proxyPort, proxyUserName.c_str(), proxyPassword.c_str());
        }

        m_manager->Init();

        // Create the client we can use to create conversation connections
        // NOTE: We need to use a separate thread service for this CSpxConversationImpl class and the 
        //       ConvesationConnection (which is the web socket connection). The reason for this is
        //       that the web socket will use the passed in thread service to send/receive on a
        //       separate thread. We are also using this same thread service here to do our asynchronous
        //       operations. Since the thread service works as a dispatcher and we are using the same
        //       background queue, processing the asynchronous operations here will block progress from
        //       happening on the web socket. This becomes problematic if we want to wait for the web
        //       socket to connect but doing so, blocks it from making the progress needed to connect
        m_client = make_unique<ConversationClient>(
            nullptr,
            properties->GetStringValue(GetPropertyName(PropertyId::Speech_SessionId)),
            FactoryUtils::CreateInstance<ISpxThreadService, CSpxThreadService>());

        string webSocketEndpoint = properties->GetStringValue(ConversationKeys::Conversation_Endpoint);
        if (false == webSocketEndpoint.empty())
        {
            m_client->SetEndpointUrl(webSocketEndpoint);
        }

        if (false == proxyHost.empty())
        {
            m_client->SetProxyServerInfo(proxyHost.c_str(), proxyPort, proxyUserName.c_str(), proxyPassword.c_str());
        }

        // TODO ralphe: how to set Single Cert, SSL properties for Linux?
    }

    void CSpxConversationImpl::Term()
    {
        // disconnect if we are still connected
        EndConversationInternal();

        // If we have a permanent room, we should not delete it because the conversation object
        // has gone out of scope
        if (false == m_permanentRoom)
        {
            DeleteConversationInternal();
        }
    }

    const std::string CSpxConversationImpl::GetConversationId() const
    {
        return RunSynchronously<std::string>([this]()
        {
            std::string conversationId;
            if (m_args != nullptr)
            {
                conversationId = m_args->RoomCode;
            }

            return conversationId.empty() ? m_conversationId : conversationId;
        });
    }

    void CSpxConversationImpl::SetConversationId(const std::string & conversationId)
    {
        RunSynchronously([this, conversationId]
        {
            SPX_IFTRUE_THROW_HR(m_args != nullptr, SPXERR_INVALID_STATE);
            m_conversationId = std::move(conversationId);
        });
    }

    void CSpxConversationImpl::CreateConversation(const std::string& nickname)
    {
        RunSynchronously([this, nickname]()
        {
            if (m_args != nullptr && !m_args->RoomCode.empty())
            {
                // it looks like we're already created the room
                return;
            }

            SPX_IFTRUE_THROW_HR(m_manager == nullptr, SPXERR_UNINITIALIZED);
            auto properties = SpxQueryService<ISpxNamedProperties>(GetSite());

            // first create the room
            CreateConversationArgs create;
            create.CognitiveSpeechAuthenticationToken = GetString(properties, PropertyId::SpeechServiceAuthorization_Token);
            create.CognitiveSpeechRegion = GetString(properties,
            {
                ConversationKeys::Conversation_Region,
                GetPropertyName(PropertyId::SpeechServiceConnection_Region)
            });
            create.CognitiveSpeechSubscriptionKey = GetString(properties,
            {
                ConversationKeys::ConversationServiceConnection_Key,
                GetPropertyName(PropertyId::SpeechServiceConnection_Key)
            });
            create.CorrelationId = GetString(properties, PropertyId::Speech_SessionId);
            create.LanguageCode = GetString(properties, PropertyId::SpeechServiceConnection_RecoLanguage);
            create.Nickname = nickname;
            create.TranslateTo = StringUtils::Tokenize(
                    GetString(properties, PropertyId::SpeechServiceConnection_TranslationToLanguages),
                    ", ");
            create.TtsVoiceCode = GetString(properties, PropertyId::SpeechServiceConnection_SynthVoice);
            create.TtsFormat = TextToSpeechFormat::Wav; // TODO ralphe: parse the PropertyId::SpeechServiceConnection_SynthOutputFormat?
            create.ClientAppId = properties->GetStringValue(ConversationKeys::Conversation_ClientId, ConversationConstants::ClientAppId);

            std::string roomPin = properties->GetStringValue(ConversationKeys::Conversation_RoomPin);

            m_args = make_unique<ConversationArgs>(m_manager->CreateOrJoin(create, m_conversationId, roomPin));
        });
    }

    void CSpxConversationImpl::StartConversation()
    {
        RunSynchronously([this]()
        {
            SPX_IFTRUE_THROW_HR(
                m_args == nullptr || m_args->RoomCode.empty() || m_args->SessionToken.empty() || m_args->ParticipantId.empty(),
                SPXERR_INVALID_STATE);
            SPX_IFTRUE_THROW_HR(m_client == nullptr, SPXERR_UNINITIALIZED);

            m_client->SetAuthenticationToken(m_args->SessionToken);
            m_client->SetParticipantId(m_args->ParticipantId);

            m_connection = m_client->Connect();

            SPX_TRACE_INFO("CSpxConversationImpl::StartConversationAsync has completed");
        });
    }

    void CSpxConversationImpl::EndConversation()
    {
        RunSynchronously([this]() { this->EndConversationInternal(); });
    }

    void CSpxConversationImpl::DeleteConversation()
    {
        RunSynchronously([this]()
        {
            SPX_IFTRUE_THROW_HR(m_manager == nullptr, SPXERR_UNINITIALIZED);
            this->DeleteConversationInternal();
        });
    }

    void CSpxConversationImpl::SetLockConversation(bool lock)
    {
        SendSynchronously(&ConversationConnection::SetRoomLocked, lock == true);
    }

    void CSpxConversationImpl::SetMuteAllParticipants(bool mute)
    {
        SendSynchronously(&ConversationConnection::SetMuteEveryoneElse, mute == true);
    }

    void CSpxConversationImpl::SetMuteParticipant(bool mute, const std::string & participantId)
    {
        SendSynchronously(&ConversationConnection::SetMuteParticipant, participantId, mute == true);
    }

    void CSpxConversationImpl::UpdateParticipant(bool add, const std::string & userId)
    {
        if (add)
        {
            Impl::ThrowWithCallstack(SPXERR_UNSUPPORTED_API_ERROR);
        }

        SendSynchronously(&ConversationConnection::EjectParticipant, userId);
    }

    void CSpxConversationImpl::UpdateParticipant(bool add, const std::string& userId, std::shared_ptr<ISpxParticipant>)
    {
        if (add)
        {
            Impl::ThrowWithCallstack(SPXERR_UNSUPPORTED_API_ERROR);
        }

        SendSynchronously(&ConversationConnection::EjectParticipant, userId);
    }

    bool CSpxConversationImpl::IsConnected() const
    {
        return m_args != nullptr
            && m_manager != nullptr
            && m_client != nullptr
            && m_connection != nullptr
            && m_connection->IsConnected();
    }

    std::shared_ptr<ConversationArgs> CSpxConversationImpl::GetConversationArgs() const
    {
        return m_args;
    }
    
    std::shared_ptr<CSpxConversationManager> CSpxConversationImpl::GetConversationManager() const
    {
        return m_manager;
    }

    std::shared_ptr<ConversationConnection> CSpxConversationImpl::GetConversationConnection() const
    {
        return m_connection;
    }

    void CSpxConversationImpl::EndConversationInternal()
    {
        if (m_connection != nullptr)
        {
            m_connection->Disconnect();
        }
    }

    void CSpxConversationImpl::DeleteConversationInternal()
    {
        if (m_manager != nullptr && m_args != nullptr && !m_args->SessionToken.empty())
        {
            m_manager->Leave(m_args->SessionToken);
            m_args->SessionToken = "";
        }
    }

}

}}}} // Microsoft::CognitiveServices::Speech::Impl::Conversation
