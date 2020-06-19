//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "common.h"
#include <create_object_helpers.h>
#include <http_utils.h>
#include <http_exception.h>
#include <string_utils.h>
#include <http_endpoint_info.h>
#include <log_helpers.h>
#include "conversation_impl.h"
#include "conversation_manager.h"
#include "conversation_utils.h"
#include "conversation_translator_logging.h"


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


    static void ConfigureEndpointFromProperties(
        HttpEndpointInfo& endpoint, shared_ptr<ISpxNamedProperties> properties, const char * urlPropertyName)
    {
        // Set the override URL if specified
        string url = properties->GetStringValue(urlPropertyName);
        if (false == url.empty())
        {
            endpoint.EndpointUrl(url);
        }

        // Get the proxy server info
        string proxyHost = NamedPropertiesHelper::GetString(properties, PropertyId::SpeechServiceConnection_ProxyHostName);
        const int32_t proxyPort = NamedPropertiesHelper::GetInt(properties, PropertyId::SpeechServiceConnection_ProxyPort, 0);
        string proxyUserName = NamedPropertiesHelper::GetString(properties, PropertyId::SpeechServiceConnection_ProxyUserName);
        string proxyPassword = NamedPropertiesHelper::GetString(properties, PropertyId::SpeechServiceConnection_ProxyPassword);

        if (false == proxyHost.empty())
        {
            endpoint.Proxy(proxyHost, proxyPort, proxyUserName, proxyPassword);
        }

#ifdef SPEECHSDK_USE_OPENSSL
        // Set the SSL certificate properties
        string singleTrustedCert = properties->GetStringValue("OPENSSL_SINGLE_TRUSTED_CERT");
        if (!singleTrustedCert.empty())
        {
            bool disable_crl_check = NamedPropertiesHelper::GetBool(properties, "OPENSSL_SINGLE_TRUSTED_CERT_CRL_CHECK");
            endpoint
                .DisableDefaultVerifyPaths(true)
                .SingleTrustedCertificate(singleTrustedCert, disable_crl_check);
        }
#endif
    }


    CSpxConversationImpl::CSpxConversationImpl()
        : ThreadingHelpers(ISpxThreadService::Affinity::User),
        m_conversationId(), m_args(), m_manager(), m_connection(), m_canRejoin(false), m_permanentRoom(false)
    {
        CT_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    }

    CSpxConversationImpl::~CSpxConversationImpl()
    {
        CT_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

        try
        {
            Term();

            m_connection.reset();
            m_manager.reset();
            m_args.reset();
        }
        catch (const ExceptionWithCallStack& ex)
        {
            CT_I_LOG_ERROR("Caught exception in CSpxConversationImpl disposer. Message: '%s'. Stack: %s", ex.what(), ex.GetCallStack());
        }
        catch (const std::exception& ex)
        {
            CT_I_LOG_ERROR("Caught exception in CSpxConversationImpl disposer. Message: '%s'", ex.what());
        }
        catch (...)
        {
            CT_I_LOG_ERROR("Caught a non-exception item thrown in CSpxConversationImpl disposer.");
        }
    }

    void CSpxConversationImpl::Init()
    {
        CT_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
        ThreadingHelpers::Init();
        
        std::shared_ptr<ISpxRecognizerSite> site = GetSite();

        auto properties = SpxQueryService<ISpxNamedProperties>(site);
        CT_I_THROW_HR_IF(properties == nullptr, SPXERR_UNEXPECTED_CONVERSATION_SITE_FAILURE);

        // Initialize logging
        SpxDiagLogSetProperties(properties);

        // Initialize websocket platform
        USP::PlatformInit(nullptr, 0, nullptr, nullptr);

        // Create the conversation manager object
        {
            HttpEndpointInfo restEndpoint;
            restEndpoint
                .Scheme(UriScheme::HTTPS)
                .Host(ConversationConstants::Host)
                .Path(ConversationConstants::RestPath);

            ConfigureEndpointFromProperties(restEndpoint, properties, ConversationKeys::Conversation_Management_Endpoint);

            m_manager = ConversationManager::Create(restEndpoint);
        }

        {
            // Create the conversation web socket connection client
            HttpEndpointInfo webSocketEndpoint;
            webSocketEndpoint
                .Scheme(UriScheme::WSS)
                .Host(ConversationConstants::Host)
                .Path(ConversationConstants::WebSocketPath);

            ConfigureEndpointFromProperties(webSocketEndpoint, properties, ConversationKeys::Conversation_Endpoint);

            // Create the client we can use to create the conversation web socket connections
            // NOTE: We are using the thread service's 'user' affinity here act as a dispatcher for our
            //       synchronous operations. To prevent deadlocks, we need to set the conversation web
            //       socket connection to use the 'background' affinity of the thread service to prevent
            //       deadlocks. If we were sharing the same thread, deadlocks could happen if we are
            //       processing an operation that needs to wait for the web socket to send/receive
            //       something, but waiting blocks progress on the web socket.
            m_connection = ConversationConnection::Create(
                webSocketEndpoint,
                m_threadService,
                ISpxThreadService::Affinity::Background,
                NamedPropertiesHelper::GetString(properties, PropertyId::Speech_SessionId));
        }
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
        RunSynchronously([this, cid = conversationId]() mutable
        {
            CT_I_THROW_HR_IF(m_args != nullptr, SPXERR_INVALID_STATE);
            m_conversationId = std::move(cid);
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

            CT_I_THROW_HR_IF(m_manager == nullptr, SPXERR_UNINITIALIZED);
            auto properties = SpxQueryService<ISpxNamedProperties>(GetSite());

            // first create the room
            CreateConversationArgs create;
            create.CognitiveSpeechRegion = NamedPropertiesHelper::GetString(properties,
            {
                ConversationKeys::Conversation_Region,
                GetPropertyName(PropertyId::SpeechServiceConnection_Region)
            });

            create.CognitiveSpeechAuthenticationToken = NamedPropertiesHelper::GetString(properties, PropertyId::SpeechServiceAuthorization_Token);
            if (create.CognitiveSpeechAuthenticationToken.empty())
            {
                create.CognitiveSpeechSubscriptionKey = NamedPropertiesHelper::GetString(properties,
                {
                    ConversationKeys::ConversationServiceConnection_Key,
                    GetPropertyName(PropertyId::SpeechServiceConnection_Key)
                });
            }
            
            create.CorrelationId = NamedPropertiesHelper::GetString(properties, PropertyId::Speech_SessionId);
            create.LanguageCode = NamedPropertiesHelper::GetString(properties, PropertyId::SpeechServiceConnection_RecoLanguage);
            create.Nickname = nickname;
            create.TranslateTo = StringUtils::Tokenize(
                NamedPropertiesHelper::GetString(properties, PropertyId::SpeechServiceConnection_TranslationToLanguages),
                ", ");
            create.TtsVoiceCode = NamedPropertiesHelper::GetString(properties, PropertyId::SpeechServiceConnection_SynthVoice);
            create.TtsFormat = TextToSpeechFormat::Wav; // TODO ralphe: parse the PropertyId::SpeechServiceConnection_SynthOutputFormat?
            create.ClientAppId = properties->GetStringValue(ConversationKeys::Conversation_ClientId, ConversationConstants::ClientAppId);

            create.HostCode = NamedPropertiesHelper::GetString(properties,
            {
                ConversationKeys::Conversation_HostCode,
                "ConversationTranslator_RoomPin" // for backwards compatibility with the previous value
            });
            create.ParticipantCode = properties->GetStringValue(ConversationKeys::Conversation_ParticipantCode);

            m_args = make_unique<ConversationArgs>(m_manager->CreateOrJoin(create, m_conversationId));

            m_canRejoin = true;

            // Update the properties based on the service response
            if (!m_args->CognitiveSpeechAuthToken.empty())
            {
                NamedPropertiesHelper::SetString(properties, PropertyId::SpeechServiceAuthorization_Token, m_args->CognitiveSpeechAuthToken);
            }

            if (!m_args->ParticipantId.empty())
            {
                NamedPropertiesHelper::SetString(properties, PropertyId::Conversation_ParticipantId, m_args->ParticipantId);
            }

            // set the region to match what the conversation translator service tells us to use when joining a room
            // except when the full endpoint URL is specified as that causes issues with the USP code
            if (NamedPropertiesHelper::GetString(properties, PropertyId::SpeechServiceConnection_Region) != m_args->CognitiveSpeechRegion
                && NamedPropertiesHelper::GetString(properties, PropertyId::SpeechServiceConnection_Endpoint).empty())
            {
                NamedPropertiesHelper::SetString(properties, PropertyId::SpeechServiceConnection_Region, m_args->CognitiveSpeechRegion.c_str());
            }
        });
    }

    void CSpxConversationImpl::StartConversation()
    {
        RunSynchronously([this]()
        {
            CT_I_THROW_HR_IF(
                m_args == nullptr || m_args->RoomCode.empty() || m_args->SessionToken.empty() || m_args->ParticipantId.empty(),
                SPXERR_INVALID_STATE);
            CT_I_THROW_HR_IF(m_connection == nullptr, SPXERR_UNINITIALIZED);

            m_connection->Connect(m_args->ParticipantId, m_args->SessionToken);

            CT_I_LOG_INFO("CSpxConversationImpl::StartConversationAsync has completed");
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
            CT_I_THROW_HR_IF(m_manager == nullptr, SPXERR_UNINITIALIZED);
            m_canRejoin = false;
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
            && m_connection != nullptr
            && m_connection->IsConnected();
    }

    bool CSpxConversationImpl::CanRejoin() const
    {
        return m_canRejoin;
    }

    std::shared_ptr<ConversationArgs> CSpxConversationImpl::GetConversationArgs() const
    {
        return m_args;
    }
    
    std::shared_ptr<ConversationManager> CSpxConversationImpl::GetConversationManager() const
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
            try
            {
                m_manager->Leave(m_args->SessionToken);
            }
            catch (HttpException& ex)
            {
                if (ex.statusCode() == 404)
                {
                    // This response usually means the service has already deleted the room on your behalf.
                    // This can happen for example if you are a participant and the host deletes the room.
                    CT_I_LOG_INFO("Got a HTTP 404 response when trying to delete the conversation. Ignoring");
                }
                else
                {
                    CT_I_LOG_ERROR("Failed to delete the conversation. '%s'", ex.what());
                    throw;
                }
            }

            ConversationDeleted.raise();

            m_args->SessionToken = "";
        }
    }

}

}}}} // Microsoft::CognitiveServices::Speech::Impl::Conversation
