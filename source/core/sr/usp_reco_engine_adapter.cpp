//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// usp_reco_engine_adapter.cpp: Implementation definitions for CSpxUspRecoEngineAdapter C++ class
//

#include <vector>

#include "stdafx.h"
#include "usp_reco_engine_adapter.h"
#include "handle_table.h"
#include "file_utils.h"
#include <inttypes.h>
#include <cstring>
#include <chrono>
#include "service_helpers.h"
#include "create_object_helpers.h"
#include "exception.h"
#include "property_id_2_name_map.h"
#include "spx_build_information.h"
#include "json.h"
#include "platform.h"
#include "guid_utils.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

using json = nlohmann::json;

CSpxUspRecoEngineAdapter::CSpxUspRecoEngineAdapter() :
    m_resetUspAfterAudioByteCount(0),
    m_uspAudioByteCount(0),
    m_audioState(AudioState::Idle),
    m_uspState(UspState::Idle)
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    SPX_DBG_TRACE_VERBOSE("%s: this=0x%8x", __FUNCTION__, this);
}

CSpxUspRecoEngineAdapter::~CSpxUspRecoEngineAdapter()
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    SPX_DBG_TRACE_VERBOSE("%s: this=0x%8x", __FUNCTION__, this);
    SPX_DBG_ASSERT(m_uspCallbacks == nullptr);
    SPX_DBG_ASSERT(m_uspConnection == nullptr);
}

void CSpxUspRecoEngineAdapter::Init()
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    SPX_DBG_TRACE_VERBOSE("%s: this=0x%8x", __FUNCTION__, this);

    SPX_IFTRUE_THROW_HR(GetSite() == nullptr, SPXERR_UNINITIALIZED);
    SPX_IFTRUE_THROW_HR(m_uspConnection != nullptr, SPXERR_ALREADY_INITIALIZED);
    SPX_IFTRUE_THROW_HR(m_uspCallbacks != nullptr, SPXERR_ALREADY_INITIALIZED);
    SPX_DBG_ASSERT(IsState(AudioState::Idle) && IsState(UspState::Idle));
}

void CSpxUspRecoEngineAdapter::Term()
{
    SPX_DBG_TRACE_SCOPE("Terminating CSpxUspRecoEngineAdapter...", "Terminating CSpxUspRecoEngineAdapter... Done!");
    SPX_DBG_TRACE_VERBOSE("%s: this=0x%8x", __FUNCTION__, this);

    if (ChangeState(UspState::Terminating))
    {
        SPX_DBG_TRACE_VERBOSE("%s: Terminating USP Connection (0x%8x)", __FUNCTION__, m_uspConnection.get());
        UspTerminate();
        ChangeState(UspState::Zombie);
    }
    else
    {
        SPX_DBG_TRACE_WARNING("%s: (0x%8x) UNEXPECTED USP State transition ... (audioState/uspState=%d/%d)", __FUNCTION__, this, m_audioState, m_uspState);
    }
}

void CSpxUspRecoEngineAdapter::SetAdapterMode(bool singleShot)
{
    SPX_DBG_TRACE_VERBOSE("%s: singleShot=%d", __FUNCTION__, singleShot);
    m_singleShot = singleShot;
}

void CSpxUspRecoEngineAdapter::OpenConnection(bool singleShot)
{
    SPX_DBG_TRACE_VERBOSE("%s: Open connection.", __FUNCTION__);

    auto properties = SpxQueryService<ISpxNamedProperties>(GetSite());
    SPX_IFTRUE_THROW_HR(properties == nullptr, SPXERR_UNEXPECTED_USP_SITE_FAILURE);
    std::string currentRecoMode = properties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_RecoMode));
    std::string recoModeToSet;

    // Get recognizer type
    uint16_t countSpeech, countIntent, countTranslation, countBot;
    GetSite()->GetScenarioCount(&countSpeech, &countIntent, &countTranslation, &countBot);
    SPX_DBG_ASSERT(countSpeech + countIntent + countTranslation + countBot == 1); // currently only support one recognizer

    if (countIntent == 1)
    {
        // The connection to service for IntentRecognizer is depending on the Intent model being used,
        // so it is not possbile to set up the connection now.
        SPX_DBG_TRACE_INFO("%s: Skip setting up connection in advance for intent recognizer.", __FUNCTION__);
        SPX_THROW_HR(SPXERR_CHANGE_CONNECTION_STATUS_NOT_ALLOWED);
        return;
    }
    else if (countSpeech == 1)
    {
        recoModeToSet = singleShot ? g_recoModeInteractive : g_recoModeConversation;
    }
    else if (countTranslation == 1)
    {
        // Always use conversation mode for translation recognizer
        recoModeToSet = g_recoModeConversation;
    }
    else if (countBot == 1)
    {
        recoModeToSet = g_recoModeInteractive;
    }
    // Set reco mode.
    if (currentRecoMode.empty())
    {
        properties->SetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_RecoMode), recoModeToSet.c_str());
    }
    else
    {
        SPX_IFTRUE_THROW_HR(currentRecoMode != recoModeToSet, SPXERR_SWITCH_MODE_NOT_ALLOWED);
    }

    // Establish the connection to service.
    EnsureUspInit();
}

void CSpxUspRecoEngineAdapter::UspClearReconnectCache()
{
    m_reconnectWaitingTimeMs = c_initialReconnectWaitingTimeMs;
    if (m_endpointType == USP::EndpointType::Bot)
    {
        m_botConversationId.clear();
    }
}

void CSpxUspRecoEngineAdapter::CloseConnection()
{
    SPX_DBG_TRACE_VERBOSE("%s: Close connection.", __FUNCTION__);

    auto properties = SpxQueryService<ISpxNamedProperties>(GetSite());
    SPX_IFTRUE_THROW_HR(properties == nullptr, SPXERR_UNEXPECTED_USP_SITE_FAILURE);
    std::string currentRecoMode = properties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_RecoMode));
    std::string recoModeToSet;

    // Get recognizer type
    uint16_t countSpeech, countIntent, countTranslation, countBot;
    GetSite()->GetScenarioCount(&countSpeech, &countIntent, &countTranslation, &countBot);
    SPX_DBG_ASSERT(countSpeech + countIntent + countTranslation + countBot == 1); // currently only support one recognizer

    if (countIntent == 1)
    {
        // The connection to service for IntentRecognizer is depending on the Intent model being used,
        // so it is not possbile to set up the connection now.
        SPX_DBG_TRACE_INFO("%s: Skip setting up connection in advance for intent recognizer.", __FUNCTION__);
        SPX_THROW_HR(SPXERR_CHANGE_CONNECTION_STATUS_NOT_ALLOWED);
        return;
    }
    if (countBot == 1)
    {
        UspClearReconnectCache();
    }
    // Terminate the connection to service.
    UspTerminate();
}

void CSpxUspRecoEngineAdapter::SetFormat(const SPXWAVEFORMATEX* pformat)
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    SPX_DBG_TRACE_VERBOSE("%s: this=0x%8x", __FUNCTION__, this);

    SPX_DBG_TRACE_VERBOSE_IF(pformat == nullptr, "%s - pformat == nullptr", __FUNCTION__);
    SPX_DBG_TRACE_VERBOSE_IF(pformat != nullptr, "%s\n  wFormatTag:      %s\n  nChannels:       %d\n  nSamplesPerSec:  %d\n  nAvgBytesPerSec: %d\n  nBlockAlign:     %d\n  wBitsPerSample:  %d\n  cbSize:          %d",
        __FUNCTION__,
        pformat->wFormatTag == WAVE_FORMAT_PCM ? "PCM" : std::to_string(pformat->wFormatTag).c_str(),
        pformat->nChannels,
        pformat->nSamplesPerSec,
        pformat->nAvgBytesPerSec,
        pformat->nBlockAlign,
        pformat->wBitsPerSample,
        pformat->cbSize);

    if (IsState(UspState::Zombie))
    {
        SPX_DBG_TRACE_VERBOSE("%s: (0x%8x) IGNORING... (audioState/uspState=%d/%d) USP-ZOMBIE", __FUNCTION__, this, m_audioState, m_uspState);
    }
    else if (IsBadState() && !IsState(UspState::Terminating))
    {
        // In case of error there still can be some calls to SetFormat in flight.
        SPX_DBG_TRACE_VERBOSE("%s: (0x%8x) IGNORING... (audioState/uspState=%d/%d)", __FUNCTION__, this, m_audioState, m_uspState);
    }
    else if (pformat != nullptr && IsState(UspState::Idle) && ChangeState(AudioState::Idle, AudioState::Ready))
    {
        // we could call site when errors happen.
        // The m_audioState is Ready at this time. So, if we have two SetFormat calls in a row, the next one won't come in here
        // it goes to the else.
        //
        SPX_DBG_TRACE_VERBOSE("%s: (0x%8x)->PrepareFirstAudioReadyState()", __FUNCTION__, this);
        PrepareFirstAudioReadyState(pformat);
    }
    else if (pformat == nullptr && (ChangeState(AudioState::Idle) || IsState(UspState::Terminating)))
    {
        SPX_DBG_TRACE_VERBOSE("%s: (0x%8x) site->AdapterCompletedSetFormatStop()", __FUNCTION__, this);
        InvokeOnSite([this](const SitePtr& p) { p->AdapterCompletedSetFormatStop(this); });

        m_format.reset();
    }
    else
    {
        SPX_DBG_TRACE_WARNING("%s: (0x%8x) UNEXPECTED USP State transition ... (audioState/uspState=%d/%d)", __FUNCTION__, this, m_audioState, m_uspState);
    }
}

void CSpxUspRecoEngineAdapter::ProcessAudio(const DataChunkPtr& audioChunk)
{
    auto size = audioChunk->size;
    if (IsState(UspState::Zombie) && size == 0)
    {
        SPX_DBG_TRACE_VERBOSE("%s: (0x%8x) IGNORING... size=0 ... (audioState/uspState=%d/%d) USP-ZOMBIE", __FUNCTION__, this, m_audioState, m_uspState);
    }
    else if (IsBadState())
    {
        // In case of error there still can be some calls to ProcessAudio in flight.
        SPX_DBG_TRACE_VERBOSE("%s: (0x%8x) IGNORING... (audioState/uspState=%d/%d)", __FUNCTION__, this, m_audioState, m_uspState);
    }
    else if (size > 0 && ChangeState(AudioState::Ready, UspState::Idle, AudioState::Sending, UspState::WaitingForTurnStart))
    {
        SPX_DBG_TRACE_VERBOSE_IF(1, "%s: (0x%8x)->SendPreAudioMessages() ... size=%d", __FUNCTION__, this, size);
        SendPreAudioMessages();
        UspWrite(audioChunk);

        SPX_DBG_TRACE_VERBOSE("%s: site->AdapterStartingTurn()", __FUNCTION__);
        InvokeOnSite([this](const SitePtr& p) { p->AdapterStartingTurn(this); });
    }
    else if (audioChunk->size > 0 && IsState(AudioState::Sending))
    {
        SPX_DBG_TRACE_VERBOSE_IF(1, "%s: (0x%8x) Sending Audio ... size=%d", __FUNCTION__, this, size);
        UspWrite(audioChunk);
    }
    else if (size == 0 && IsState(AudioState::Sending))
    {
        SPX_DBG_TRACE_VERBOSE_IF(1, "%s: (0x%8x) Flushing Audio ... size=0 USP-FLUSH", __FUNCTION__, this);
        UspWriteFlush();
    }
    else if (!IsState(AudioState::Sending))
    {
        SPX_DBG_TRACE_VERBOSE_IF(1, "%s: (0x%8x) Ignoring audio size=%d ... (audioState/uspState=%d/%d)", __FUNCTION__, this, size, m_audioState, m_uspState);
    }
    else
    {
        SPX_DBG_TRACE_WARNING("%s: (0x%8x) UNEXPECTED USP State transition ... (audioState/uspState=%d/%d)", __FUNCTION__, this, m_audioState, m_uspState);
    }
}

void CSpxUspRecoEngineAdapter::SendAgentMessage(const std::string& buffer)
{
    SPX_DBG_TRACE_VERBOSE("%s: this=0x%8x", __FUNCTION__, this);
    EnsureUspInit();
    UspSendMessage("agent", buffer, USP::MessageType::Agent);
}

void CSpxUspRecoEngineAdapter::EnsureUspInit()
{
    if (m_uspConnection == nullptr)
    {
        UspInitialize();
    }
}

void CSpxUspRecoEngineAdapter::UspInitialize()
{
    SPX_DBG_TRACE_VERBOSE("%s: this=0x%8x", __FUNCTION__, this);
    SPX_IFTRUE_THROW_HR(m_uspConnection != nullptr, SPXERR_ALREADY_INITIALIZED);
    SPX_IFTRUE_THROW_HR(m_uspCallbacks != nullptr, SPXERR_ALREADY_INITIALIZED);

    // Get the named property service...
    auto properties = SpxQueryService<ISpxNamedProperties>(GetSite());
    SPX_IFTRUE_THROW_HR(properties == nullptr, SPXERR_UNEXPECTED_USP_SITE_FAILURE);

    auto threadService = SpxQueryService<ISpxThreadService>(GetSite());
    SPX_IFTRUE_THROW_HR(threadService == nullptr, SPXERR_UNEXPECTED_USP_SITE_FAILURE);

    // Create the usp client, which we'll configure and use to create the actual connection
    auto uspCallbacks = SpxCreateObjectWithSite<ISpxUspCallbacks>("CSpxUspCallbackWrapper", this);

    // Currently we use a session id as a connection id to correlate logs on the server side with a particular user
    // session, because currently there is a single connection for session at any point in time.
    auto sessionId = PAL::ToWString(properties->GetStringValue(GetPropertyName(PropertyId::Speech_SessionId)));
    USP::Client client(uspCallbacks, USP::EndpointType::Speech, sessionId, threadService);

    // Set up the connection properties, and create the connection
    SetUspEndpoint(properties, client);
    SetUspAuthentication(properties, client);
    SetUspProxyInfo(properties, client);
    SetUspSingleTrustedCert(properties, client);

    // Construct config message payload
    SetSpeechConfigMessage(*properties);
    if (m_endpointType == USP::EndpointType::Bot)
    {
        SetAgentConfigMessage(*properties);
    }

    USP::ConnectionPtr uspConnection;

    try
    {
        uspConnection = client.Connect();
    }
    catch (const std::exception& e)
    {
        SPX_TRACE_ERROR("Error: '%s'", e.what());
        OnError(true, USP::ErrorCode::ConnectionError, e.what());
    }
    catch (...)
    {
        SPX_TRACE_ERROR("Error: Unexpected exception in UspInitialize");
        OnError(true, USP::ErrorCode::ConnectionError, "Error: Unexpected exception in UspInitialize");
    }

    // Keep track of what time we initialized (so we can reset later)
    m_uspInitTime = std::chrono::system_clock::now();
    m_uspResetTime = m_uspInitTime + std::chrono::seconds(m_resetUspAfterTimeSeconds);

    // We're done!!
    m_uspCallbacks = uspCallbacks;
    m_uspConnection = std::move(uspConnection);

    if (m_uspConnection != nullptr)
    {
        UspSendSpeechConfig();
        /* This will noop for non bot endpoints */
        UspSendAgentConfig();
    }
}

void CSpxUspRecoEngineAdapter::UspTerminate()
{
    // Inform upper layer about disconnection.
    if ((m_uspConnection != nullptr) && m_uspConnection->IsConnected())
    {
        OnDisconnected();
    }

    // Term the callbacks first and then reset/release the connection
    SpxTermAndClear(m_uspCallbacks); // After this ... we won't be called back on ISpxUspCallbacks ever again...

    // NOTE: Even if we are in a callback from the USP on another thread, we won't be destoyed until those functions release their shared ptrs
    m_uspConnection.reset();

    // Fix up some of our counters...
    m_uspAudioByteCount = 0;
}

USP::Client& CSpxUspRecoEngineAdapter::SetUspEndpoint(std::shared_ptr<ISpxNamedProperties>& properties, USP::Client& client)
{
    SPX_DBG_ASSERT(GetSite() != nullptr);

    // How many recognizers of each type do we have?
    uint16_t countSpeech = 0, countIntent = 0, countTranslation = 0, countBot = 0;
    GetSite()->GetScenarioCount(&countSpeech, &countIntent, &countTranslation, &countBot);
    SPX_DBG_ASSERT(countSpeech + countIntent + countTranslation + countBot == 1); // currently only support one recognizer

    // set endpoint url if this is provided.
    auto endpoint = properties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_Endpoint));
    if (!endpoint.empty())
    {
        SPX_DBG_TRACE_VERBOSE("%s: Using Custom endpoint: %s", __FUNCTION__, endpoint.c_str());
        m_customEndpoint = true;
        client.SetEndpointUrl(endpoint);
    }

    // set user defined query parameters if provided.
    auto userDefinedQueryParameters = properties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_UserDefinedQueryParameters));
    if (!userDefinedQueryParameters.empty())
    {
        SPX_DBG_TRACE_VERBOSE("%s: Using user provided query parameters: %s", __FUNCTION__, userDefinedQueryParameters.c_str());
        client.SetUserDefinedQueryParameters(userDefinedQueryParameters);
    }

    // set endpoint type.
    if (!endpoint.empty() && 0 == PAL::stricmp(endpoint.c_str(), "CORTANA"))
    {
        SetUspEndpoint_Cortana(properties, client);
    }
    else if (countIntent == 1)
    {
        SetUspEndpoint_Intent(properties, client);
    }
    else if (countTranslation == 1)
    {
        SetUspEndpoint_Translation(properties, client);
    }
    else if (countBot == 1)
    {
        SetUspEndpoint_Bot(properties, client);
    }
    else
    {
        SPX_DBG_ASSERT(countSpeech == 1);
        SetUspEndpoint_DefaultSpeechService(properties, client);
    }

    // set reco mode based on recognizer type
    USP::RecognitionMode mode = USP::RecognitionMode::Interactive;
    SPXHR checkHr = SPX_NOERROR;
    // Reco mode should be already set in properties.
    checkHr = GetRecoModeFromProperties(properties, mode);
    SPX_THROW_HR_IF(checkHr, SPX_FAILED(checkHr));
    m_isInteractiveMode = mode == USP::RecognitionMode::Interactive;
    client.SetRecognitionMode(mode);
    SPX_DBG_TRACE_VERBOSE("%s: recoMode=%d", __FUNCTION__, mode);

    // Set output format.
    client.SetOutputFormat(GetOutputFormat(properties));
    auto pollingInterval = static_cast<uint16_t>(stoi(properties->GetStringValue("SPEECH-USPPollingInterval", "10")));
    SPX_DBG_TRACE_VERBOSE("%s: Setting Websocket Polling interval to %d", __FUNCTION__, pollingInterval);

    client.SetPollingIntervalms(pollingInterval);

    return client;
}

USP::Client& CSpxUspRecoEngineAdapter::SetUspEndpoint_Bot(std::shared_ptr<ISpxNamedProperties>& properties, USP::Client& client)
{
    UNUSED(properties);

    m_endpointType = USP::EndpointType::Bot;

    auto region = properties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_Region));
    SPX_IFTRUE_THROW_HR(region.empty(), SPXERR_INVALID_REGION);

    auto language = properties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_RecoLanguage));
    SPX_IFTRUE_THROW_HR(language.empty(), SPXERR_INVALID_LANGUAGE);
    client.SetLanguage(language);

    SPX_DBG_TRACE_VERBOSE("%s: Using Bot URL/endpoint...", __FUNCTION__);
    return client.SetEndpointType(m_endpointType)
        .SetRegion(region)
        .SetLanguage(language)
        .SetAudioResponseFormat("raw-16khz-16bit-mono-pcm");
}

USP::Client& CSpxUspRecoEngineAdapter::SetUspEndpoint_Cortana(std::shared_ptr<ISpxNamedProperties>& properties, USP::Client& client)
{
    UNUSED(properties);

    SPX_DBG_TRACE_VERBOSE("%s: Using Cortana endpoint...", __FUNCTION__);
    m_endpointType = USP::EndpointType::CDSDK;
    return client.SetEndpointType(m_endpointType);
}

USP::Client& CSpxUspRecoEngineAdapter::SetUspEndpoint_Intent(std::shared_ptr<ISpxNamedProperties>& properties, USP::Client& client)
{
    SPX_DBG_TRACE_VERBOSE("%s: Endpoint type: Intent", __FUNCTION__);
    m_endpointType = USP::EndpointType::Intent;
    client.SetEndpointType(m_endpointType);
    SetUspRegion(properties, client, /*isIntentRegion=*/ true);

    auto language = properties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_RecoLanguage));
    client.SetLanguage(language);

    return client;
}

USP::Client& CSpxUspRecoEngineAdapter::SetUspEndpoint_Translation(std::shared_ptr<ISpxNamedProperties>& properties, USP::Client& client)
{
    SPX_DBG_TRACE_VERBOSE("%s: Endpoint type: Translation.", __FUNCTION__);
    m_endpointType = USP::EndpointType::Translation;
    client.SetEndpointType(m_endpointType);
    SetUspRegion(properties, client, /*isIntentRegion=*/ false);

    auto fromLang = properties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_RecoLanguage));
    client.SetTranslationSourceLanguage(fromLang);
    auto toLangs = properties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_TranslationToLanguages));
    client.SetTranslationTargetLanguages(toLangs);
    auto customSpeechModelId = properties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_EndpointId));
    client.SetModelId(customSpeechModelId);
    auto voice = properties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_TranslationVoice));
    client.SetTranslationVoice(voice);

    return client;
}

USP::Client& CSpxUspRecoEngineAdapter::SetUspEndpoint_DefaultSpeechService(std::shared_ptr<ISpxNamedProperties>& properties, USP::Client& client)
{
    SPX_DBG_TRACE_VERBOSE("%s: Endpoint type: Speech.", __FUNCTION__);
    m_endpointType = USP::EndpointType::Speech;
    client.SetEndpointType(m_endpointType);
    SetUspRegion(properties, client, /*isIntentRegion=*/ false);

    auto lang = properties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_RecoLanguage));
    client.SetLanguage(lang);
    auto customSpeechModelId = properties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_EndpointId));
    client.SetModelId(customSpeechModelId);

    return client;
}

USP::Client& CSpxUspRecoEngineAdapter::SetUspRegion(std::shared_ptr<ISpxNamedProperties>& properties, USP::Client& client, bool isIntentRegion)
{
    auto region = isIntentRegion ? properties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_IntentRegion))
        : properties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_Region));
    if (!m_customEndpoint)
    {
        SPX_IFTRUE_THROW_HR(region.empty(), SPXERR_INVALID_REGION);
        isIntentRegion ? client.SetIntentRegion(region) : client.SetRegion(region);
    }
    else
    {
        if (!region.empty())
        {
            SPX_DBG_TRACE_ERROR("%s: when using custom endpoint, region should not be specified separately.");
            SPX_THROW_HR(SPXERR_INVALID_ARG);
        }
    }
    return client;
}

USP::Client& CSpxUspRecoEngineAdapter::SetUspAuthentication(std::shared_ptr<ISpxNamedProperties>& properties, USP::Client& client)
{
    // Get the properties that indicates what endpoint to use...
    auto uspSubscriptionKey = properties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_Key));
    auto uspAuthToken = properties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceAuthorization_Token));
    auto uspRpsToken = properties->GetStringValue("SPEECH-RpsToken");
    std::vector<std::string> authData((size_t)USP::AuthenticationType::SIZE_AUTHENTICATION_TYPE);

    authData[(size_t)USP::AuthenticationType::SubscriptionKey] = uspSubscriptionKey;
    authData[(size_t)USP::AuthenticationType::AuthorizationToken] = uspAuthToken;
    authData[(size_t)USP::AuthenticationType::SearchDelegationRPSToken] = uspRpsToken;

    return client.SetAuthentication(authData);
}

USP::Client& CSpxUspRecoEngineAdapter::SetUspSingleTrustedCert(std::shared_ptr<ISpxNamedProperties>& properties, USP::Client& client)
{
#if SPEECHSDK_USE_OPENSSL
    // N.B. the names of the options below have been shared with a customer. Do
    // not change them without consulting with them.
    auto singleTrustedCert = properties->GetStringValue("OPENSSL_SINGLE_TRUSTED_CERT");
    if (!singleTrustedCert.empty())
    {
        bool disable_crl_check = properties->GetStringValue("OPENSSL_SINGLE_TRUSTED_CERT_CRL_CHECK") == "false";
        return client.SetSingleTrustedCert(singleTrustedCert, disable_crl_check);
    }
#else
    UNUSED(properties);
#endif

    return client;
}

USP::Client& CSpxUspRecoEngineAdapter::SetUspProxyInfo(std::shared_ptr<ISpxNamedProperties>& properties, USP::Client& client)
{
    // Get proxy related properties.
    if (!properties->HasStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_ProxyHostName)))
    {
        return client;
    }

    auto proxyHostName = properties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_ProxyHostName));
    if (proxyHostName.empty())
    {
        ThrowInvalidArgumentException("Proxy hostname is empty.");
    }

    if (!properties->HasStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_ProxyPort)))
    {
        ThrowInvalidArgumentException("No proxy port is specified.");
    }
    auto proxyPort = std::stoi(properties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_ProxyPort)));
    if (proxyPort <= 0)
    {
        ThrowInvalidArgumentException("Invalid proxy port: %d", proxyPort);
    }

    bool hasUserName = properties->HasStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_ProxyUserName));
    bool hasPassword = properties->HasStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_ProxyPassword));
    std::string userName;
    std::string password;

    if (hasUserName)
    {
        userName = properties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_ProxyUserName));
    }
    if (hasPassword)
    {
        password = properties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_ProxyPassword));
    }
    if (hasUserName != hasPassword)
    {
        ThrowInvalidArgumentException("Either proxy user name or password is empty.");
    }
    return client.SetProxyServerInfo(proxyHostName.c_str(), proxyPort, hasUserName ? userName.c_str() : nullptr, hasPassword ? password.c_str() : nullptr);
}

SPXHR CSpxUspRecoEngineAdapter::GetRecoModeFromProperties(const std::shared_ptr<ISpxNamedProperties>& properties, USP::RecognitionMode& recoMode) const
{
     SPXHR hr = SPX_NOERROR;

    // Get the property that indicates what reco mode to use...
    auto value = properties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_RecoMode));

    if (value.empty())
    {
        hr = SPXERR_NOT_FOUND;
    }
    else if (PAL::stricmp(value.c_str(), g_recoModeInteractive) == 0)
    {
        recoMode = USP::RecognitionMode::Interactive;
    }
    else if (PAL::stricmp(value.c_str(), g_recoModeConversation) == 0)
    {
        recoMode = USP::RecognitionMode::Conversation;
    }
    else if (PAL::stricmp(value.c_str(), g_recoModeDictation) == 0)
    {
        recoMode = USP::RecognitionMode::Dictation;
    }
    else
    {
        SPX_DBG_ASSERT_WITH_MESSAGE(false, "Unknown RecognitionMode in USP::RecognitionMode.");
        LogError("Unknown RecognitionMode value %s", value.c_str());
        hr = SPXERR_INVALID_ARG;
    }

    return hr;
}

USP::OutputFormat CSpxUspRecoEngineAdapter::GetOutputFormat(const std::shared_ptr<ISpxNamedProperties>& properties) const
{
    if (!properties->HasStringValue(GetPropertyName(PropertyId::SpeechServiceResponse_RequestDetailedResultTrueFalse)))
        return USP::OutputFormat::Simple;

    auto value = properties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceResponse_RequestDetailedResultTrueFalse));
    if (value.empty() || PAL::stricmp(value.c_str(), PAL::BoolToString(false).c_str()) == 0)
    {
        return USP::OutputFormat::Simple;
    }
    else if (PAL::stricmp(value.c_str(), PAL::BoolToString(true).c_str()) == 0)
    {
        return USP::OutputFormat::Detailed;
    }

    LogError("Unknown output format value %s", value.c_str());
    SPX_THROW_HR(SPXERR_INVALID_ARG);
    return USP::OutputFormat::Simple; // Make compiler happy.
}

void CSpxUspRecoEngineAdapter::SetSpeechConfigMessage(const ISpxNamedProperties& properties)
{
    constexpr auto systemLanguagePropertyName = "SPEECHSDK-SPEECH-CONFIG-SYSTEM-LANGUAGE";
    constexpr auto systemName = "SpeechSDK";
    constexpr auto osPropertyName = "SPEECHSDK-SPEECH-CONFIG-OS";

    auto osPropertyValue = properties.GetStringValue(osPropertyName);

    json osJson;

    json speechConfig;
    speechConfig["context"]["system"]["version"] = BuildInformation::g_fullVersion;
    speechConfig["context"]["system"]["name"] = systemName;
    speechConfig["context"]["system"]["build"] = BuildInformation::g_buildPlatform;
    auto languagePropertyValue = properties.GetStringValue(systemLanguagePropertyName);
    if (languagePropertyValue != "")
    {
        speechConfig["context"]["system"]["lang"] = languagePropertyValue;
    }
    auto osInfo = PAL::getOperatingSystem();
    speechConfig["context"]["os"]["platform"] = osInfo.platform;
    speechConfig["context"]["os"]["name"] = osInfo.name;
    speechConfig["context"]["os"]["version"] = osInfo.version;

    // Set the audio configuration data.
    // Todo: Fill audio configuration data with the value via property bags.
    speechConfig["context"]["audio"]["source"]["type"] = properties.GetStringValue(GetPropertyName(PropertyId::AudioConfig_AudioSource));
    speechConfig["context"]["audio"]["source"]["model"] = properties.GetStringValue("SPEECH-MicrophoneNiceName");
    speechConfig["context"]["audio"]["source"]["samplerate"] = properties.GetStringValue(GetPropertyName(PropertyId::AudioConfig_SampleRateForCapture));
    speechConfig["context"]["audio"]["source"]["bitspersample"] = properties.GetStringValue(GetPropertyName(PropertyId::AudioConfig_BitsPerSampleForCapture));
    speechConfig["context"]["audio"]["source"]["channelcount"] = properties.GetStringValue(GetPropertyName(PropertyId::AudioConfig_NumberOfChannelsForCapture));

    m_speechConfig = speechConfig.dump();
}

void CSpxUspRecoEngineAdapter::SetAgentConfigMessage(const ISpxNamedProperties& properties)
{
    constexpr auto botCommunicationType = "Conversation_Communication_Type";
    constexpr auto botSecretKey = "BOT-SecretKey";
    constexpr auto botFromId = "BOT-FromId";
    constexpr auto botTTSAudioFormat = "SPEECH-SynthOutputFormat";
    constexpr auto botAuthorizationToken = "BOT-BotAuthorizationToken";

    json agentConfigJson;
    agentConfigJson["version"] = 0.2;

    auto communicationType = properties.GetStringValue(botCommunicationType);
    if (communicationType.empty())
    {
        communicationType = "Default";
    }
    agentConfigJson["botInfo"]["commType"] = communicationType;

    auto connectionId = properties.GetStringValue(botSecretKey);
    if (connectionId.empty())
    {
        SPX_THROW_HR(SPXERR_INVALID_ARG);
    }
    agentConfigJson["botInfo"]["connectionId"] = connectionId;

    auto fromId = properties.GetStringValue(botFromId);
    if (!fromId.empty())
    {
        agentConfigJson["botInfo"]["fromId"] = fromId;
    }

    auto ttsAudioFormat = properties.GetStringValue(botTTSAudioFormat);
    if (!ttsAudioFormat.empty())
    {
        agentConfigJson["ttsAudioFormat"] = ttsAudioFormat;
    }

    auto authToken = properties.GetStringValue(botAuthorizationToken);
    if (!authToken.empty())
    {
        agentConfigJson["botInfo"]["authorization"] = authToken;
    }

    if (!m_botConversationId.empty())
    {
        agentConfigJson["botInfo"]["conversationId"] = m_botConversationId;
    }

    m_agentConfig = agentConfigJson.dump();
}

void CSpxUspRecoEngineAdapter::UspSendSpeechConfig()
{
    constexpr auto messagePath = "speech.config";
    SPX_DBG_TRACE_VERBOSE("%s %s", messagePath, m_speechConfig.c_str());
    UspSendMessage(messagePath, m_speechConfig, USP::MessageType::Config);
}

void CSpxUspRecoEngineAdapter::UspSendAgentConfig()
{
    constexpr auto messagePath = "agent.config";
    /* Only send it for bot endpoint */
    if (m_endpointType == USP::EndpointType::Bot)
    {
        SPX_DBG_TRACE_VERBOSE("%s %s", messagePath, m_agentConfig.c_str());
        UspSendMessage(messagePath, m_agentConfig, USP::MessageType::Config);
    }
}

void CSpxUspRecoEngineAdapter::UspSendSpeechContext()
{
    if (m_endpointType == USP::EndpointType::Bot)
    {
        json contextJson = {
            {"version", 0.5},
            {"context", {
                {"interactionId", PAL::ToString(PAL::CreateGuidWithoutDashes())}
            }},
            {"channelData", ""}
        };
        UspSendMessage("speech.agent.context", contextJson.dump(), USP::MessageType::Context);
    }
    else
    {
        // Get the Dgi json payload
        std::list<std::string> listenForList = GetListenForListFromSite();
        auto listenForJson = GetDgiJsonFromListenForList(listenForList);

        // Get the intent payload
        std::string provider, id, key, region;
        GetIntentInfoFromSite(provider, id, key, region);
        auto intentJson = GetLanguageUnderstandingJsonFromIntentInfo(provider, id, key, region);

        // Do we expect to receive an intent payload from the service?
        m_expectIntentResponse = !intentJson.empty();

        // Take the json payload and the intent payload, and create the speech context json
        auto speechContext = GetSpeechContextJson(listenForJson, intentJson);
        if (!speechContext.empty())
        {
            // Since it's not empty, we'll send it (empty means we don't have either dgi or intent payload)
            std::string messagePath = "speech.context";
            UspSendMessage(messagePath, speechContext, USP::MessageType::Context);
        }
    }
}

void CSpxUspRecoEngineAdapter::UspSendMessage(const std::string& messagePath, const std::string &buffer, USP::MessageType messageType)
{
    SPX_DBG_TRACE_VERBOSE("%s='%s'", messagePath.c_str(), buffer.c_str());
    UspSendMessage(messagePath, (const uint8_t*)buffer.c_str(), buffer.length(), messageType);
}

void CSpxUspRecoEngineAdapter::UspSendMessage(const std::string& messagePath, const uint8_t* buffer, size_t size, USP::MessageType messageType)
{
    SPX_DBG_ASSERT(m_uspConnection != nullptr || IsState(UspState::Terminating) || IsState(UspState::Zombie));
    if (!IsState(UspState::Terminating) && !IsState(UspState::Zombie) && m_uspConnection != nullptr)
    {
        m_uspConnection->SendMessage(messagePath, buffer, size, messageType);
    }
}

void CSpxUspRecoEngineAdapter::UspWriteFormat(SPXWAVEFORMATEX* pformat)
{
    static const uint16_t cbTag = 4;
    static const uint16_t cbChunkType = 4;
    static const uint16_t cbChunkSize = 4;

    uint32_t cbFormatChunk = sizeof(SPXWAVEFORMAT) + pformat->cbSize;
    uint32_t cbRiffChunk = 0;       // NOTE: This isn't technically accurate for a RIFF/WAV file, but it's fine for Truman/Newman/Skyman
    uint32_t cbDataChunk = 0;       // NOTE: Similarly, this isn't technically correct for the 'data' chunk, but it's fine for Truman/Newman/Skyman

    uint32_t cbHeader =
        cbTag + cbChunkSize +       // 'RIFF' #size_of_RIFF#
        cbChunkType +               // 'WAVE'
        cbChunkType + cbChunkSize + // 'fmt ' #size_fmt#
        cbFormatChunk +             // actual format
        cbChunkType + cbChunkSize;  // 'data' #size_of_data#

    // Allocate the buffer, and create a ptr we'll use to advance thru the buffer as we're writing stuff into it
    auto buffer = SpxAllocSharedAudioBuffer(cbHeader);
    auto ptr = buffer.get();

    // The 'RIFF' header (consists of 'RIFF' followed by size of payload that follows)
    ptr = FormatBufferWriteChars(ptr, "RIFF", cbTag);
    ptr = FormatBufferWriteNumber(ptr, cbRiffChunk);

    // The 'WAVE' chunk header
    ptr = FormatBufferWriteChars(ptr, "WAVE", cbChunkType);

    // The 'fmt ' chunk (consists of 'fmt ' followed by the total size of the SPXWAVEFORMAT(EX)(TENSIBLE), followed by the SPXWAVEFORMAT(EX)(TENSIBLE)
    ptr = FormatBufferWriteChars(ptr, "fmt ", cbChunkType);
    ptr = FormatBufferWriteNumber(ptr, cbFormatChunk);
    ptr = FormatBufferWriteBytes(ptr, (uint8_t*)pformat, cbFormatChunk);

    // The 'data' chunk is next
    ptr = FormatBufferWriteChars(ptr, "data", cbChunkType);
    ptr = FormatBufferWriteNumber(ptr, cbDataChunk);

    // Now that we've prepared the header/buffer, send it along to Truman/Newman/Skyman via UspWrite
    SPX_DBG_ASSERT(cbHeader == uint32_t(ptr - buffer.get()));
    UspWrite(std::make_shared<DataChunk>(buffer, cbHeader));
}

void CSpxUspRecoEngineAdapter::UspWrite(const DataChunkPtr& audioChunk)
{
    m_uspAudioByteCount += audioChunk->size;
    UspWriteActual(audioChunk);
}

void CSpxUspRecoEngineAdapter::UspWriteActual(const DataChunkPtr& audioChunk)
{
    SPX_DBG_TRACE_VERBOSE("%s(..., %d)", __FUNCTION__, audioChunk->size);
    SPX_DBG_ASSERT(m_uspConnection != nullptr || IsState(UspState::Terminating) || IsState(UspState::Zombie));
    if (!IsState(UspState::Terminating) && !IsState(UspState::Zombie) && m_uspConnection != nullptr)
    {
        m_uspConnection->WriteAudio(audioChunk);
    }
    else
    {
        SPX_DBG_TRACE_ERROR("%s: unexpected USP connection state:%d. Not sending audio chunk (size=%d).", __FUNCTION__, m_uspState, audioChunk->size);
    }
}

void CSpxUspRecoEngineAdapter::UspWriteFlush()
{
    // We should only ever be asked to Flush when we're in a valid state ...
    SPX_DBG_ASSERT(m_uspConnection != nullptr || IsState(UspState::Terminating) || IsState(UspState::Zombie));
    if (!IsState(UspState::Terminating) && !IsState(UspState::Zombie) && m_uspConnection != nullptr)
    {
        m_uspConnection->FlushAudio();
    }
}

void CSpxUspRecoEngineAdapter::UspResetConnection()
{
    SPX_DBG_TRACE_VERBOSE("%s: Reconnecting....", __FUNCTION__);
    std::weak_ptr<ISpxRecoEngineAdapter> wk{ ISpxInterfaceBaseFor<ISpxRecoEngineAdapter>::shared_from_this() };
    std::this_thread::sleep_for(std::chrono::milliseconds((m_reconnectWaitingTimeMs)));

    if (auto keep_alive = wk.lock())
    {
        UspTerminate();
        EnsureUspInit();
    }
}

void CSpxUspRecoEngineAdapter::WriteTelemetryLatency(uint64_t latencyInTicks, bool isPhraseLatency)
{
    SPX_DBG_ASSERT(m_uspConnection != nullptr);
    if (m_uspConnection == nullptr)
    {
        SPX_TRACE_ERROR("%s: m_uspConnection is null.", __FUNCTION__);
    }
    else
    {
        m_uspConnection->WriteTelemetryLatency(latencyInTicks, isPhraseLatency);
    }
}

void CSpxUspRecoEngineAdapter::OnSpeechStartDetected(const USP::SpeechStartDetectedMsg& message)
{
    // The USP message for SpeechStartDetected isn't what it might sound like in all "reco modes" ...
    // * In INTERACTIVE mode, it works as it sounds. It indicates the beginning of speech for the "phrase" message that will arrive later
    // * In CONTINUOUS modes, however, it corresponds to the time of the beginning of speech for the FIRST "phrase" message of many inside one turn

    SPX_DBG_TRACE_VERBOSE("Response: Speech.StartDetected message. Speech starts at offset %" PRIu64 " (100ns).\n", message.offset);

    if (IsBadState())
    {
        SPX_DBG_TRACE_VERBOSE("%s: (0x%8x) IGNORING... (audioState/uspState=%d/%d) %s", __FUNCTION__, this, m_audioState, m_uspState, IsState(UspState::Terminating) ? "(USP-TERMINATING)" : "********** USP-UNEXPECTED !!!!!!");
    }
    else if (IsState(UspState::WaitingForPhrase))
    {
        SPX_DBG_TRACE_VERBOSE("%s: (0x%8x) site->AdapterDetectedSpeechStart()", __FUNCTION__, this);
        InvokeOnSite([this, &message](const SitePtr& p) { p->AdapterDetectedSpeechStart(this, message.offset); });
    }
    else
    {
        SPX_DBG_TRACE_WARNING("%s: (0x%8x) UNEXPECTED USP State transition ... (audioState/uspState=%d/%d)", __FUNCTION__, this, m_audioState, m_uspState);
    }
}

void CSpxUspRecoEngineAdapter::OnSpeechEndDetected(const USP::SpeechEndDetectedMsg& message)
{
    SPX_DBG_TRACE_VERBOSE("Response: Speech.EndDetected message. Speech ends at offset %" PRIu64 " (100ns)\n", message.offset);

    auto requestMute = ChangeState(AudioState::Sending, AudioState::Mute);

    if (IsBadState())
    {
        SPX_DBG_TRACE_VERBOSE("%s: (0x%8x) IGNORING... (audioState/uspState=%d/%d) %s", __FUNCTION__, this, m_audioState, m_uspState, IsState(UspState::Terminating) ? "(USP-TERMINATING)" : "********** USP-UNEXPECTED !!!!!!");
    }
    else if (IsStateBetweenIncluding(UspState::WaitingForPhrase, UspState::WaitingForTurnEnd) &&
             (IsState(AudioState::Idle) ||
              IsState(AudioState::Mute)))
    {
        SPX_DBG_TRACE_VERBOSE("%s: (0x%8x) site->AdapterDetectedSpeechEnd()", __FUNCTION__, this);
        InvokeOnSite([this, &message](const SitePtr& p) { p->AdapterDetectedSpeechEnd(this, message.offset); });
    }
    else
    {
        SPX_DBG_TRACE_WARNING("%s: (0x%8x) UNEXPECTED USP State transition ... (audioState/uspState=%d/%d)", __FUNCTION__, this, m_audioState, m_uspState);
        return;
    }

    SPX_DBG_TRACE_VERBOSE("%s: Flush ... (audioState/uspState=%d/%d)  USP-FLUSH", __FUNCTION__, m_audioState, m_uspState);

    UspWriteFlush();
    if (requestMute && !IsBadState())
    {
        SPX_DBG_TRACE_VERBOSE("%s: site->AdapterRequestingAudioMute(true) ... (audioState/uspState=%d/%d)", __FUNCTION__, m_audioState, m_uspState);
        InvokeOnSite([this](const SitePtr& p) { p->AdapterRequestingAudioMute(this, true); });
    }
}

void CSpxUspRecoEngineAdapter::OnSpeechHypothesis(const USP::SpeechHypothesisMsg& message)
{
    SPX_DBG_TRACE_VERBOSE("Response: Speech.Hypothesis message. Starts at offset %" PRIu64 ", with duration %" PRIu64 " (100ns). Text: %ls\n", message.offset, message.duration, message.text.c_str());

    if (IsBadState())
    {
        SPX_DBG_TRACE_VERBOSE("%s: (0x%8x) IGNORING... (audioState/uspState=%d/%d) %s", __FUNCTION__, this, m_audioState, m_uspState, IsState(UspState::Terminating) ? "(USP-TERMINATING)" : "********** USP-UNEXPECTED !!!!!!");
    }
    else if (IsState(UspState::WaitingForPhrase))
    {
        SPX_DBG_TRACE_VERBOSE("%s: site->FireAdapterResult_Intermediate()", __FUNCTION__);

        InvokeOnSite([&](const SitePtr& site)
        {
            auto factory = SpxQueryService<ISpxRecoResultFactory>(site);
            auto result = factory->CreateIntermediateResult(nullptr, message.text.c_str(), message.offset, message.duration);
            auto namedProperties = SpxQueryInterface<ISpxNamedProperties>(result);
            namedProperties->SetStringValue(GetPropertyName(PropertyId::SpeechServiceResponse_JsonResult), PAL::ToString(message.json).c_str());
            site->FireAdapterResult_Intermediate(this, message.offset, result);
        });
    }
    else
    {
        SPX_DBG_TRACE_WARNING("%s: (0x%8x) UNEXPECTED USP State transition ... (audioState/uspState=%d/%d)", __FUNCTION__, this, m_audioState, m_uspState);
    }
}

void CSpxUspRecoEngineAdapter::OnSpeechFragment(const USP::SpeechFragmentMsg& message)
{
    SPX_DBG_TRACE_VERBOSE("Response: Speech.Fragment message. Starts at offset %" PRIu64 ", with duration %" PRIu64 " (100ns). Text: %ls\n", message.offset, message.duration, message.text.c_str());
    SPX_DBG_ASSERT(!m_isInteractiveMode);

    bool sendIntermediate = false;

    if (IsBadState())
    {
        SPX_DBG_TRACE_VERBOSE("%s: (0x%8x) IGNORING... (audioState/uspState=%d/%d) %s", __FUNCTION__, this, m_audioState, m_uspState, IsState(UspState::Terminating) ? "(USP-TERMINATING)" : "********** USP-UNEXPECTED !!!!!!");
    }
    else if (ChangeState(UspState::WaitingForIntent, UspState::WaitingForIntent2))
    {
        SPX_DBG_TRACE_VERBOSE("%s: Intent never came from service!!", __FUNCTION__);
        sendIntermediate = true;
        FireFinalResultLater_WaitingForIntentComplete();
        ChangeState(UspState::WaitingForIntent2, UspState::WaitingForPhrase);
    }
    else if (IsState(UspState::WaitingForPhrase))
    {
        sendIntermediate = true;
    }
    else
    {
        SPX_DBG_TRACE_WARNING("%s: (0x%8x) UNEXPECTED USP State transition ... (audioState/uspState=%d/%d)", __FUNCTION__, this, m_audioState, m_uspState);
    }

    if (sendIntermediate)
    {
        SPX_DBG_TRACE_VERBOSE("%s: site->FireAdapterResult_Intermediate()", __FUNCTION__);

        InvokeOnSite([&](const SitePtr& site)
        {
            auto factory = SpxQueryService<ISpxRecoResultFactory>(site);
            auto result = factory->CreateIntermediateResult(nullptr, message.text.c_str(), message.offset, message.duration);
            auto namedProperties = SpxQueryInterface<ISpxNamedProperties>(result);
            namedProperties->SetStringValue(GetPropertyName(PropertyId::SpeechServiceResponse_JsonResult), PAL::ToString(message.json).c_str());
            site->FireAdapterResult_Intermediate(this, message.offset, result);
        });
    }
}

void CSpxUspRecoEngineAdapter::OnSpeechPhrase(const USP::SpeechPhraseMsg& message)
{
    SPX_DBG_TRACE_VERBOSE("Response: Speech.Phrase message. Status: %d, Text: %ls, starts at %" PRIu64 ", with duration %" PRIu64 " (100ns).\n", message.recognitionStatus, message.displayText.c_str(), message.offset, message.duration);
    SPX_DBG_TRACE_VERBOSE("%s: this=0x%8x", __FUNCTION__, this);

    if (IsBadState())
    {
        SPX_DBG_TRACE_VERBOSE("%s: (0x%8x) IGNORING... (audioState/uspState=%d/%d) %s", __FUNCTION__, this, m_audioState, m_uspState, IsState(UspState::Terminating) ? "(USP-TERMINATING)" : "********** USP-UNEXPECTED !!!!!!");
    }
    else if (m_expectIntentResponse &&
             message.recognitionStatus == USP::RecognitionStatus::Success &&
             ChangeState(UspState::WaitingForPhrase, UspState::WaitingForIntent))
    {
        if (message.recognitionStatus == USP::RecognitionStatus::EndOfDictation)
        {
            // Intent recognition is using interactive mode, so it should not receive any EndOfDictation message.
            SPX_DBG_TRACE_VERBOSE("EndOfDictation message is not expected for intent recognizer.");
            SPX_THROW_HR(SPXERR_RUNTIME_ERROR);
        }
        else
        {
            SPX_DBG_TRACE_VERBOSE("%s: FireFinalResultLater()", __FUNCTION__);
            FireFinalResultLater(message);
        }
    }
    else if ((m_isInteractiveMode && ChangeState(UspState::WaitingForPhrase, UspState::WaitingForTurnEnd)) ||
        (!m_isInteractiveMode && ChangeState(UspState::WaitingForPhrase, UspState::WaitingForPhrase)))
    {
        if (message.recognitionStatus == USP::RecognitionStatus::EndOfDictation)
        {
            InvokeOnSite([&](const SitePtr& site)
            {
                site->AdapterEndOfDictation(this, message.offset, message.duration);
            });
        }
        else
        {
            SPX_DBG_TRACE_VERBOSE("%s: FireFinalResultNow()", __FUNCTION__);
            FireFinalResultNow(message);
        }
    }
    else
    {
        SPX_DBG_TRACE_WARNING("%s: (0x%8x) UNEXPECTED USP State transition ... (audioState/uspState=%d/%d)", __FUNCTION__, this, m_audioState, m_uspState);
    }
}

static TranslationStatusCode GetTranslationStatus(::USP::TranslationStatus uspStatus)
{
    TranslationStatusCode status = TranslationStatusCode::Error;
    switch (uspStatus)
    {
    case ::USP::TranslationStatus::Success:
        status = TranslationStatusCode::Success;
        break;
    case ::USP::TranslationStatus::Error:
        break;
    case ::USP::TranslationStatus::InvalidMessage:
        // The failureReason contains additional error messages.
        // Todo: have better error handling for different statuses.
        break;
    default:
        SPX_THROW_HR(SPXERR_RUNTIME_ERROR);
        break;
    }
    return status;
}

void CSpxUspRecoEngineAdapter::OnTranslationHypothesis(const USP::TranslationHypothesisMsg& message)
{
    SPX_DBG_TRACE_VERBOSE("Response: Translation.Hypothesis message. RecoText: %ls, TranslationStatus: %d, starts at %" PRIu64 ", with duration %" PRIu64 " (100ns).\n",
        message.text.c_str(), message.translation.translationStatus, message.offset, message.duration);
    auto resultMap = message.translation.translations;
#ifdef _DEBUG
    for (const auto& it : resultMap)
    {
        SPX_DBG_TRACE_VERBOSE("          Translation in %ls: %ls,\n", it.first.c_str(), it.second.c_str());
    }
#endif

    if (IsBadState())
    {
        SPX_DBG_TRACE_VERBOSE("%s: IGNORING (Err/Terminating/Zombie)... (audioState/uspState=%d/%d)", __FUNCTION__, m_audioState, m_uspState);
    }
    else if (IsState(UspState::WaitingForPhrase))
    {
        {
            SPX_DBG_TRACE_SCOPE("Fire intermediate translation result: Creating Result", "FireIntermeidateResult: GetSite()->FireAdapterResult_Intermediate()  complete!");
            InvokeOnSite([&](const SitePtr& site)
            {
                // Create the result
                auto factory = SpxQueryService<ISpxRecoResultFactory>(site);
                auto result = factory->CreateIntermediateResult(nullptr, message.text.c_str(), message.offset, message.duration);

                auto namedProperties = SpxQueryInterface<ISpxNamedProperties>(result);
                namedProperties->SetStringValue(GetPropertyName(PropertyId::SpeechServiceResponse_JsonResult), PAL::ToString(message.json).c_str());

                // Update our result to be a "TranslationText" result.
                auto initTranslationResult = SpxQueryInterface<ISpxTranslationRecognitionResultInit>(result);

                auto status = GetTranslationStatus(message.translation.translationStatus);
                initTranslationResult->InitTranslationRecognitionResult(status, message.translation.translations, message.translation.failureReason);

                // Fire the result
                site->FireAdapterResult_Intermediate(this, message.offset, result);
            });
        }
    }
    else
    {
        SPX_DBG_TRACE_WARNING("%s: Unexpected USP State transition (audioState/uspState=%d/%d)", __FUNCTION__, m_audioState, m_uspState);
    }
}

void CSpxUspRecoEngineAdapter::OnTranslationPhrase(const USP::TranslationPhraseMsg& message)
{
    auto resultMap = message.translation.translations;

    SPX_DBG_TRACE_VERBOSE("Response: Translation.Phrase message. RecoStatus: %d, TranslationStatus: %d, RecoText: %ls, starts at %" PRIu64 ", with duration %" PRIu64 " (100ns).\n",
        message.recognitionStatus, message.translation.translationStatus,
        message.text.c_str(), message.offset, message.duration);
#ifdef _DEBUG
    if (message.translation.translationStatus != ::USP::TranslationStatus::Success)
    {
        SPX_DBG_TRACE_VERBOSE(" FailureReason: %ls.", message.translation.failureReason.c_str());
    }
    for (const auto& it : resultMap)
    {
        SPX_DBG_TRACE_VERBOSE("          , translated to %ls: %ls,\n", it.first.c_str(), it.second.c_str());
    }
#endif

    if (IsBadState())
    {
        SPX_DBG_TRACE_VERBOSE("%s: IGNORING (Err/Terminating/Zombie)... (audioState/uspState=%d/%d)", __FUNCTION__, m_audioState, m_uspState);
    }
    else if ((m_isInteractiveMode && ChangeState(UspState::WaitingForPhrase, UspState::WaitingForTurnEnd)) ||
             (!m_isInteractiveMode && ChangeState(UspState::WaitingForPhrase, UspState::WaitingForPhrase)))
    {
        SPX_DBG_TRACE_SCOPE("Fire final translation result: Creating Result", "FireFinalResul: GetSite()->FireAdapterResult_FinalResult()  complete!");
        if (message.recognitionStatus == USP::RecognitionStatus::EndOfDictation)
        {
            InvokeOnSite([&](const SitePtr& site)
            {
                site->AdapterEndOfDictation(this, message.offset, message.duration);
            });
        }
        else
        {
            auto cancellationReason = ToCancellationReason(message.recognitionStatus);
            if (cancellationReason != REASON_CANCELED_NONE)
            {
                // The status above should have been treated as error result, so this should not happen here.
                SPX_TRACE_ERROR("Unexpected recognition status %d.", message.recognitionStatus);
                SPX_THROW_HR(SPXERR_RUNTIME_ERROR);
            }

            InvokeOnSite([&](const SitePtr& site)
            {
                // Create the result
                auto factory = SpxQueryService<ISpxRecoResultFactory>(site);
                auto result = factory->CreateFinalResult(nullptr, ToReason(message.recognitionStatus), ToNoMatchReason(message.recognitionStatus), cancellationReason, CancellationErrorCode::NoError, message.text.c_str(), message.offset, message.duration);

                auto namedProperties = SpxQueryInterface<ISpxNamedProperties>(result);
                namedProperties->SetStringValue(GetPropertyName(PropertyId::SpeechServiceResponse_JsonResult), PAL::ToString(message.json).c_str());

                // Update our result to be an "TranslationText" result.
                auto initTranslationResult = SpxQueryInterface<ISpxTranslationRecognitionResultInit>(result);

                auto status = GetTranslationStatus(message.translation.translationStatus);
                initTranslationResult->InitTranslationRecognitionResult(status, message.translation.translations, message.translation.failureReason);

                // Fire the result
                site->FireAdapterResult_FinalResult(this, message.offset, result);
            });
        }
    }
    else
    {
        SPX_DBG_TRACE_WARNING("%s: Unexpected USP State transition (audioState/uspState=%d/%d)", __FUNCTION__, m_audioState, m_uspState);
    }
}

void CSpxUspRecoEngineAdapter::OnAudioOutputChunk(const USP::AudioOutputChunkMsg& message)
{
    SPX_DBG_TRACE_VERBOSE("Response: Audio output chunk message. Audio data size: %d\n", message.audioLength);

    if (m_endpointType == USP::EndpointType::Bot)
    {
        auto it = m_request_session_map.find(message.requestId);
        if (it != m_request_session_map.end())
        {
            auto& machine = it->second;
            machine->Switch(CSpxActivitySession::State::AudioReceived, nullptr, &message);
        }
        return;
    }

    InvokeOnSite([this, &message](const SitePtr &site)
    {
        auto factory = SpxQueryService<ISpxRecoResultFactory>(site);
        auto result = factory->CreateFinalResult(nullptr, ResultReason::SynthesizingAudio, NO_MATCH_REASON_NONE, REASON_CANCELED_NONE, CancellationErrorCode::NoError, L"", 0, 0);

        // Update our result to be an "TranslationSynthesis" result.
        auto initTranslationResult = SpxQueryInterface<ISpxTranslationSynthesisResultInit>(result);
        initTranslationResult->InitTranslationSynthesisResult(message.audioBuffer, message.audioLength, message.requestId);

        site->FireAdapterResult_TranslationSynthesis(this, result);
    });
}

void CSpxUspRecoEngineAdapter::OnTurnStart(const USP::TurnStartMsg& message)
{
    SPX_DBG_TRACE_VERBOSE("Response: Turn.Start message. Context.ServiceTag: %s\n", message.contextServiceTag.c_str());
    SPX_DBG_TRACE_VERBOSE("%s: this=0x%8x", __FUNCTION__, this);

    if (IsBadState())
    {
        SPX_DBG_TRACE_VERBOSE("%s: (0x%8x) IGNORING... (audioState/uspState=%d/%d) %s", __FUNCTION__, this, m_audioState, m_uspState, IsState(UspState::Terminating) ? "(USP-TERMINATING)" : "********** USP-UNEXPECTED !!!!!!");
    }
    else if (ChangeState(UspState::WaitingForTurnStart, UspState::WaitingForPhrase))
    {
        SPX_DBG_TRACE_VERBOSE("%s: site->AdapterStartedTurn()", __FUNCTION__);
        SPX_DBG_ASSERT(GetSite());
        InvokeOnSite([this, &message](const SitePtr& p) { p->AdapterStartedTurn(this, message.contextServiceTag); });
    }
    else
    {
        SPX_DBG_TRACE_WARNING("%s: (0x%8x) UNEXPECTED USP State transition ... (audioState/uspState=%d/%d)", __FUNCTION__, this, m_audioState, m_uspState);
    }
}

void CSpxUspRecoEngineAdapter::OnTurnEnd(const USP::TurnEndMsg& message)
{
    SPX_DBG_TRACE_SCOPE("CSpxUspRecoEngineAdapter::OnTurnEnd ... started... ", "CSpxUspRecoEngineAdapter::OnTurnEnd ... DONE!");
    SPX_DBG_TRACE_VERBOSE("Response: Turn.End message.\n");
    UNUSED(message);

    auto adapterTurnStopped = false;

    auto prepareReady =  !m_singleShot &&
        (ChangeState(AudioState::Sending, AudioState::Ready) ||
         ChangeState(AudioState::Mute, AudioState::Ready));

    auto requestMute = m_singleShot && ChangeState(AudioState::Sending, AudioState::Mute);

    if (IsBadState())
    {
        SPX_DBG_TRACE_VERBOSE("%s: (0x%8x) IGNORING... (audioState/uspState=%d/%d) %s", __FUNCTION__, this, m_audioState, m_uspState, IsState(UspState::Terminating) ? "(USP-TERMINATING)" : "********** USP-UNEXPECTED !!!!!!");
    }
    else if (( m_isInteractiveMode && ChangeState(UspState::WaitingForTurnEnd, UspState::Idle)) ||
             (!m_isInteractiveMode && ChangeState(UspState::WaitingForPhrase, UspState::Idle)))
    {
        adapterTurnStopped = true;
    }
    else if (ChangeState(UspState::WaitingForIntent, UspState::WaitingForIntent2))
    {
        SPX_DBG_TRACE_VERBOSE("%s: Intent never came from service!!", __FUNCTION__);
        adapterTurnStopped = true;
        FireFinalResultLater_WaitingForIntentComplete();
        ChangeState(UspState::WaitingForIntent2, UspState::Idle);
    }
    else
    {
        SPX_DBG_TRACE_WARNING("%s: (0x%8x) UNEXPECTED USP State transition ... (audioState/uspState=%d/%d)", __FUNCTION__, this, m_audioState, m_uspState);
    }

    if (prepareReady && !IsBadState())
    {
        SPX_DBG_TRACE_VERBOSE("%s: PrepareAudioReadyState()", __FUNCTION__);
        PrepareAudioReadyState();

        SPX_DBG_TRACE_VERBOSE("%s: site->AdapterRequestingAudioMute(false) ... (audioState/uspState=%d/%d)", __FUNCTION__, m_audioState, m_uspState);
        InvokeOnSite([this](const SitePtr& p) { p->AdapterRequestingAudioMute(this, false); });
    }

    if (adapterTurnStopped && ShouldResetAfterTurnStopped())
    {
        ResetAfterTurnStopped();
    }

    auto site = GetSite();
    if (!site)
        return;

    if (adapterTurnStopped)
    {
        SPX_DBG_TRACE_VERBOSE("%s: site->AdapterStoppedTurn()", __FUNCTION__);
        site->AdapterStoppedTurn(this);
    }

    if (requestMute)
    {
        SPX_DBG_TRACE_VERBOSE("%s: UspWriteFlush()  USP-FLUSH", __FUNCTION__);
        UspWriteFlush();

        SPX_DBG_TRACE_VERBOSE("%s: site->AdapterRequestingAudioMute(true) ... (audioState/uspState=%d/%d)", __FUNCTION__, m_audioState, m_uspState);
        site->AdapterRequestingAudioMute(this, true);
    }
}


void CSpxUspRecoEngineAdapter::OnMessageStart(const USP::TurnStartMsg& message)
{
    /* For now only handle service generated messages for bot endpoints */
    if (m_endpointType == USP::EndpointType::Bot)
    {
        std::weak_ptr<CSpxUspRecoEngineAdapter> wk{ std::dynamic_pointer_cast<CSpxUspRecoEngineAdapter>(ISpxInterfaceBaseFor<ISpxRecoEngineAdapter>::shared_from_this()) };
        // start the initial state as SessionStart
        m_request_session_map.emplace(message.requestId, std::make_unique<CSpxActivitySession>(wk));
    }
}

void CSpxUspRecoEngineAdapter::OnMessageEnd(const USP::TurnEndMsg& message)
{
    /* For now only handle service generated messages for bot endpoints */
    if (m_endpointType == USP::EndpointType::Bot)
    {
        auto it = m_request_session_map.find(message.requestId);
        if (it != m_request_session_map.end())
        {
            auto& machine = it->second;
            machine->Switch(CSpxActivitySession::State::End, nullptr, nullptr);
            m_request_session_map.erase(it);
        }
    }
}

void CSpxUspRecoEngineAdapter::OnError(bool isTransport, USP::ErrorCode errorCode, const std::string& errorMessage)
{
    SPX_DBG_TRACE_VERBOSE("Response: On Error: Code:%d, Message: %s.\n", errorCode, errorMessage.c_str());

    /* Only try reconnecting for bot endpoints. */
    if (m_endpointType == USP::EndpointType::Bot)
    {
        if (errorCode == USP::ErrorCode::ConnectionError && m_audioState != AudioState::Sending && errorMessage.find("Internal error:") != std::string::npos)
        {
            /* Increment first so  we sleep at most ~c_reconnectWaitingTimeThreasholdMs */
            m_reconnectWaitingTimeMs = m_reconnectWaitingTimeMs * 2;
            if (m_reconnectWaitingTimeMs < c_reconnectWaitingTimeThreasholdMs)
            {
                UspResetConnection();
                return;
            }
            else
            {
                UspClearReconnectCache();
            }
        }
    }

    if (IsBadState())
    {
        SPX_DBG_TRACE_VERBOSE("%s: (0x%8x) IGNORING... (audioState/uspState=%d/%d) %s", __FUNCTION__, this, m_audioState, m_uspState, IsState(UspState::Terminating) ? "(USP-TERMINATING)" : "********** USP-UNEXPECTED !!!!!!");
    }
    else if (ChangeState(UspState::Error))
    {
        SPX_DBG_TRACE_VERBOSE("%s: site->Error() ... error='%s'", __FUNCTION__, errorMessage.c_str());
        CancellationErrorCode cancellationErrorCode;
        std::string errorInfo = errorMessage;
        switch (errorCode)
        {
        case USP::ErrorCode::AuthenticationError:
            cancellationErrorCode = CancellationErrorCode::AuthenticationFailure;
            break;
        case USP::ErrorCode::ConnectionError:
            cancellationErrorCode = CancellationErrorCode::ConnectionFailure;
            break;
        case USP::ErrorCode::RuntimeError:
            cancellationErrorCode = CancellationErrorCode::RuntimeError;
            break;
        case USP::ErrorCode::BadRequest:
            cancellationErrorCode = CancellationErrorCode::BadRequest;
            break;
        case USP::ErrorCode::TooManyRequests:
            cancellationErrorCode = CancellationErrorCode::TooManyRequests;
            break;
        case USP::ErrorCode::Forbidden:
            cancellationErrorCode = CancellationErrorCode::Forbidden;
            break;
        case USP::ErrorCode::ServiceError:
            cancellationErrorCode = CancellationErrorCode::ServiceError;
            break;
        case USP::ErrorCode::ServiceUnavailable:
            cancellationErrorCode = CancellationErrorCode::ServiceUnavailable;
            break;
        default:
            cancellationErrorCode = CancellationErrorCode::RuntimeError;
            errorInfo = "Unknown error code:" + std::to_string((int)errorCode) + ". Error message:" + errorMessage;
            break;
        };
        InvokeOnSite([this, errorInfo, isTransport, cancellationErrorCode](const SitePtr& p) { p->Error(this,  std::make_shared<SpxRecoEngineAdapterError>(isTransport, CancellationReason::Error, cancellationErrorCode, errorInfo)); });
    }
    else
    {
       SPX_DBG_TRACE_WARNING("%s: (0x%8x) UNEXPECTED USP State transition ... (audioState/uspState=%d/%d)", __FUNCTION__, this, m_audioState, m_uspState);
    }
}

void CSpxUspRecoEngineAdapter::OnUserMessage(const USP::UserMsg& msg)
{
    SPX_DBG_TRACE_VERBOSE("Response: Usp User Message: %s, content-type=%s", msg.path.c_str(), msg.contentType.c_str());

    if (msg.path == "response")
    {
        if (ChangeState(UspState::WaitingForIntent, UspState::WaitingForIntent2))
        {
            std::string luisJson((const char*)msg.buffer, msg.size);
            SPX_DBG_TRACE_VERBOSE("USP User Message: response; luisJson='%s'", luisJson.c_str());
            FireFinalResultLater_WaitingForIntentComplete(luisJson);
            ChangeState(UspState::WaitingForIntent2, m_isInteractiveMode ? UspState::WaitingForTurnEnd : UspState::WaitingForPhrase);
        }
        else if (m_endpointType == USP::EndpointType::Bot)
        {
            std::string message{ reinterpret_cast<const char*>(msg.buffer), msg.size };
            SPX_DBG_TRACE_VERBOSE("USP User Message: response; message='%s'", message.c_str());
            auto responseMessage = json::parse(message);
            if (!responseMessage["botConversationId"].is_null())
            {
                m_botConversationId = responseMessage["botConversationId"].get<std::string>();
            }
            auto it = m_request_session_map.find(msg.requestId);
            if (it != m_request_session_map.end())
            {
                auto& machine = it->second;
                machine->Switch(CSpxActivitySession::State::ActivityReceived, &message, nullptr);
                return;
            }
            SPX_DBG_TRACE_WARNING("Unexpected message; request_id='%s'", msg.requestId.c_str());
        }
        else
        {
            SPX_DBG_TRACE_WARNING("%s: (0x%8x) UNEXPECTED USP State transition ... (audioState/uspState=%d/%d)", __FUNCTION__, this, m_audioState, m_uspState);
        }
    }
}

void CSpxUspRecoEngineAdapter::OnConnected()
{
    /* In case of a successful connection, we reset the exponential back off */
    m_reconnectWaitingTimeMs = c_initialReconnectWaitingTimeMs;
    InvokeOnSite([](const SitePtr& p) { p->FireConnectedEvent(); });
}


void CSpxUspRecoEngineAdapter::OnDisconnected()
{
    InvokeOnSite([](const SitePtr& p) { p->FireDisconnectedEvent(); });
}

uint8_t* CSpxUspRecoEngineAdapter::FormatBufferWriteBytes(uint8_t* buffer, const uint8_t* source, size_t bytes)
{
    std::memcpy(buffer, source, bytes);
    return buffer + bytes;
}

uint8_t* CSpxUspRecoEngineAdapter::FormatBufferWriteNumber(uint8_t* buffer, uint32_t number)
{
    std::memcpy(buffer, &number, sizeof(number));
    return buffer + sizeof(number);
}

uint8_t* CSpxUspRecoEngineAdapter::FormatBufferWriteChars(uint8_t* buffer, const char* psz, size_t cch)
{
    std::memcpy(buffer, psz, cch);
    return buffer + cch;
}

std::list<std::string> CSpxUspRecoEngineAdapter::GetListenForListFromSite()
{
    SPX_DBG_ASSERT(GetSite() != nullptr);
    return GetSite()->GetListenForList();
}

std::string CSpxUspRecoEngineAdapter::GetDgiJsonFromListenForList(std::list<std::string>& listenForList)
{
    SPX_DBG_ASSERT(GetSite() != nullptr);
    auto properties = SpxQueryService<ISpxNamedProperties>(GetSite());
    auto noDGI = PAL::ToBool(properties->GetStringValue("CARBON-INTERNAL-USP-NoDGI"));

    std::string dgiJson;

    std::list<std::string> grammars;
    std::list<std::string> genericItems;

    for (auto listenFor : listenForList)
    {
        if (listenFor.length() > 3 &&
            listenFor[0] == '{' && listenFor[listenFor.length() - 1] == '}' &&
            listenFor.find(':') != std::string::npos)
        {
            std::string ref = listenFor.substr(1, listenFor.length() - 2);
            ref.replace(ref.find(':'), 1, "/");
            grammars.push_back(std::move(ref));
        }
        else
        {
            genericItems.push_back(std::move(listenFor));
        }
    }

    if (grammars.size() > 0 || genericItems.size() > 0)
    {
        bool appendComma = false;

        dgiJson = "{";  // start object

        if (genericItems.size() > 0)
        {
            dgiJson += R"("Groups": [)";  // start "Group" array
            dgiJson += R"({"Type":"Generic","Items":[)"; // start Generic Items array

            appendComma = false;
            for (auto item : genericItems)
            {
                dgiJson += appendComma ? "," : "";
                dgiJson += R"({"Text":")";
                dgiJson += item;
                dgiJson += R"("})";
                appendComma = true;
            }

            dgiJson += "]}"; // close Generic Items array
            dgiJson += "]";  // close "Group" array

            appendComma = true;
        }

        if (grammars.size() > 0)
        {
            dgiJson += appendComma ? "," : "";
            dgiJson += R"("ReferenceGrammars": [)";

            appendComma = false;
            for (auto grammar : grammars)
            {
                // deal with commas
                dgiJson += appendComma ? "," : "";
                dgiJson += "\"";
                dgiJson += grammar;
                dgiJson += "\"";
                appendComma = true;
            }

            dgiJson += "]";
        }

        dgiJson += "}";  // close object
    }

    return noDGI ? "" : dgiJson;
}

void CSpxUspRecoEngineAdapter::GetIntentInfoFromSite(std::string& provider, std::string& id, std::string& key, std::string& region)
{
    SPX_DBG_ASSERT(GetSite() != nullptr);
    GetSite()->GetIntentInfo(provider, id, key, region);
}

std::string CSpxUspRecoEngineAdapter::GetLanguageUnderstandingJsonFromIntentInfo(const std::string& provider, const std::string& id, const std::string& key, const std::string& region)
{
    SPX_DBG_ASSERT(GetSite() != nullptr);
    auto properties = SpxQueryService<ISpxNamedProperties>(GetSite());
    auto noIntentJson = PAL::ToBool(properties->GetStringValue("CARBON-INTERNAL-USP-NoIntentJson"));

    UNUSED(region);
    UNUSED(key);

    std::string intentJson;
    if (!provider.empty() && !id.empty())
    {
        intentJson = "{"; // start object

        intentJson += R"("provider":")";
        intentJson += provider + R"(",)";

        intentJson += R"("id":")";
        intentJson += id + R"(",)";

        intentJson += R"("key":")";
        intentJson += key + R"(")";

        intentJson += "}"; // end object
    }

    return noIntentJson ? "" : intentJson;
}

std::string CSpxUspRecoEngineAdapter::GetSpeechContextJson(const std::string& dgiJson, const std::string& intentJson)
{
    std::string contextJson;

    if (!dgiJson.empty() || !intentJson.empty())
    {
        bool appendComma = false;
        contextJson += "{";

        if (!dgiJson.empty())
        {
            contextJson += appendComma ? "," : "";
            contextJson += R"("dgi":)";
            contextJson += dgiJson;
            appendComma = true;
            appendComma = true;
        }

        if (!intentJson.empty())
        {
            contextJson += appendComma ? "," : "";
            contextJson += R"("intent":)";
            contextJson += intentJson;
        }

        contextJson += "}";
    }

    return contextJson;
}

ResultReason CSpxUspRecoEngineAdapter::ToReason(USP::RecognitionStatus uspRecognitionStatus)
{
    switch (uspRecognitionStatus)
    {
    case USP::RecognitionStatus::Success:
        return ResultReason::RecognizedSpeech;

    case USP::RecognitionStatus::NoMatch:
    case USP::RecognitionStatus::InitialSilenceTimeout:
    case USP::RecognitionStatus::InitialBabbleTimeout:
        return ResultReason::NoMatch;

    case USP::RecognitionStatus::Error:
    case USP::RecognitionStatus::TooManyRequests:
    case USP::RecognitionStatus::BadRequest:
    case USP::RecognitionStatus::Forbidden:
    case USP::RecognitionStatus::ServiceUnavailable:
    case USP::RecognitionStatus::InvalidMessage:
        return ResultReason::Canceled;

    default:
        SPX_TRACE_ERROR("Unexpected recognition status %d when converting to ResultReason.", uspRecognitionStatus);
        SPX_THROW_HR(SPXERR_RUNTIME_ERROR);
        return ResultReason::Canceled;
    }
}

CancellationReason CSpxUspRecoEngineAdapter::ToCancellationReason(USP::RecognitionStatus uspRecognitionStatus)
{
    switch (uspRecognitionStatus)
    {
    case USP::RecognitionStatus::Success:
    case USP::RecognitionStatus::NoMatch:
    case USP::RecognitionStatus::InitialSilenceTimeout:
    case USP::RecognitionStatus::InitialBabbleTimeout:
        return REASON_CANCELED_NONE;

    case USP::RecognitionStatus::Error:
    case USP::RecognitionStatus::TooManyRequests:
    case USP::RecognitionStatus::BadRequest:
    case USP::RecognitionStatus::Forbidden:
    case USP::RecognitionStatus::ServiceUnavailable:
    case USP::RecognitionStatus::InvalidMessage:
        return CancellationReason::Error;

    default:
        SPX_TRACE_ERROR("Unexpected recognition status %d when converting to CancellationReason.", uspRecognitionStatus);
        SPX_THROW_HR(SPXERR_RUNTIME_ERROR);
        return CancellationReason::Error;
    }
}

NoMatchReason CSpxUspRecoEngineAdapter::ToNoMatchReason(USP::RecognitionStatus uspRecognitionStatus)
{
    switch (uspRecognitionStatus)
    {
    case USP::RecognitionStatus::Success:
    case USP::RecognitionStatus::Error:
    case USP::RecognitionStatus::TooManyRequests:
    case USP::RecognitionStatus::BadRequest:
    case USP::RecognitionStatus::Forbidden:
    case USP::RecognitionStatus::ServiceUnavailable:
    case USP::RecognitionStatus::InvalidMessage:
        return NO_MATCH_REASON_NONE;

    case USP::RecognitionStatus::NoMatch:
        return NoMatchReason::NotRecognized;

    case USP::RecognitionStatus::InitialSilenceTimeout:
        return NoMatchReason::InitialSilenceTimeout;

    case USP::RecognitionStatus::InitialBabbleTimeout:
        return NoMatchReason::InitialBabbleTimeout;

    default:
        SPX_TRACE_ERROR("Unexpected recognition status %d when converting to NoMatchReason.", uspRecognitionStatus);
        SPX_THROW_HR(SPXERR_RUNTIME_ERROR);
        return NO_MATCH_REASON_NONE;
    }
}

void CSpxUspRecoEngineAdapter::FireActivityResult(std::shared_ptr<ISpxActivity> activity, std::shared_ptr<ISpxAudioOutput> audio)
{
    SPX_DBG_TRACE_SCOPE("FireActivityAndAudioResult: Creating Result", "FireActivityAndAudioResult: GetSite()->FireAdapterResult_ActivityAudioReceived()  complete!");

    InvokeOnSite([&](const SitePtr& site)
    {
        site->FireAdapterResult_ActivityReceived(this, activity, audio);
    });
}

void CSpxUspRecoEngineAdapter::FireFinalResultLater(const USP::SpeechPhraseMsg& message)
{
    m_finalResultMessageToFireLater = message;
}

void CSpxUspRecoEngineAdapter::FireFinalResultNow(const USP::SpeechPhraseMsg& message, const std::string& luisJson)
{
    SPX_DBG_TRACE_SCOPE("FireFinalResultNow: Creating Result", "FireFinalResultNow: GetSite()->FireAdapterResult_FinalResult()  complete!");

    InvokeOnSite([&](const SitePtr& site)
    {
        // Create the result
        auto factory = SpxQueryService<ISpxRecoResultFactory>(site);

        auto cancellationReason = ToCancellationReason(message.recognitionStatus);
        if (cancellationReason != REASON_CANCELED_NONE)
        {
            // The status above should have been treated as error event, so this should not happen here.
            SPX_TRACE_ERROR("Unexpected recognition status %d.", message.recognitionStatus);
            SPX_THROW_HR(SPXERR_RUNTIME_ERROR);
        }
        auto result = factory->CreateFinalResult(nullptr, ToReason(message.recognitionStatus), ToNoMatchReason(message.recognitionStatus), cancellationReason, CancellationErrorCode::NoError, message.displayText.c_str(), message.offset, message.duration);

        auto namedProperties = SpxQueryInterface<ISpxNamedProperties>(result);
        namedProperties->SetStringValue(GetPropertyName(PropertyId::SpeechServiceResponse_JsonResult), PAL::ToString(message.json).c_str());

        // Do we already have the LUIS json payload from the service (1-hop)
        if (!luisJson.empty())
        {
            namedProperties->SetStringValue(GetPropertyName(PropertyId::LanguageUnderstandingServiceResponse_JsonResult), luisJson.c_str());
        }

        site->FireAdapterResult_FinalResult(this, message.offset, result);
    });
}

void CSpxUspRecoEngineAdapter::FireFinalResultLater_WaitingForIntentComplete(const std::string& luisJson)
{
    SPX_DBG_ASSERT(m_expectIntentResponse);
    FireFinalResultNow(m_finalResultMessageToFireLater, luisJson);
    m_finalResultMessageToFireLater = USP::SpeechPhraseMsg();
}

bool CSpxUspRecoEngineAdapter::ChangeState(AudioState fromAudioState, UspState fromUspState, AudioState toAudioState, UspState toUspState)
{
    if (fromAudioState == m_audioState &&       // are we in correct audio state, and ...
        fromUspState == m_uspState &&           // are we in correct usp state? ... if so great! but ...
        ((fromUspState != UspState::Error &&        // don't allow transit from Error
          fromUspState != UspState::Zombie &&       // don't allow transit from Zombie
          fromUspState != UspState::Terminating) || // don't allow transit from Terminating ...
         ((fromUspState == toUspState) ||           // unless we're staying in that same usp state
          (fromUspState == UspState::Error &&           // or we're going from Error to Terminating
           toUspState == UspState::Terminating) ||
          (fromUspState == UspState::Terminating &&     // or we're going from Terminating to Zombie
           toUspState == UspState::Zombie))))
    {
        SPX_DBG_TRACE_VERBOSE("%s; audioState/uspState: %d/%d => %d/%d %s%s%s%s%s", __FUNCTION__,
            fromAudioState, fromUspState,
            toAudioState, toUspState,
            toUspState == UspState::Error ? "USP-ERRORERROR" : "",
            (fromAudioState == AudioState::Idle && fromUspState == UspState::Idle &&
             toAudioState == AudioState::Ready && toUspState == UspState::Idle) ? "USP-START" : "",
            (toAudioState == AudioState::Idle && toUspState == UspState::Idle) ? "USP-DONE" : "",
            toUspState == UspState::Terminating ? "USP-TERMINATING" : "",
            toUspState == UspState::Zombie ? "USP-ZOMBIE" : ""
            );

        m_audioState = toAudioState;
        m_uspState = toUspState;
        return true;
    }

    return false;
}

void CSpxUspRecoEngineAdapter::PrepareFirstAudioReadyState(const SPXWAVEFORMATEX* format)
{
    SPX_DBG_ASSERT(IsState(AudioState::Ready, UspState::Idle));

    auto sizeOfFormat = sizeof(SPXWAVEFORMATEX) + format->cbSize;
    m_format = SpxAllocWAVEFORMATEX(sizeOfFormat);
    memcpy(m_format.get(), format, sizeOfFormat);

    m_resetUspAfterAudioByteCount = m_format->nAvgBytesPerSec * m_resetUspAfterAudioSeconds;
    if (ShouldResetBeforeFirstAudio())
    {
        ResetBeforeFirstAudio();
    }

    PrepareAudioReadyState();
}

void CSpxUspRecoEngineAdapter::PrepareAudioReadyState()
{
    SPX_DBG_ASSERT(IsState(AudioState::Ready, UspState::Idle));

    EnsureUspInit();
}

void CSpxUspRecoEngineAdapter::SendPreAudioMessages()
{
    SPX_DBG_ASSERT(IsState(AudioState::Sending));

    UspSendSpeechContext();
    UspWriteFormat(m_format.get());
}

bool CSpxUspRecoEngineAdapter::ShouldResetAfterError()
{
    return m_allowUspResetAfterError && IsState(UspState::Idle);
}

void CSpxUspRecoEngineAdapter::ResetAfterError()
{
    SPX_DBG_ASSERT(ShouldResetAfterError());

    // Let's terminate our current usp connection and sever our callbacks
    UspTerminate();

    // Let's get ready for more audio!!!
    PrepareAudioReadyState();
}

bool CSpxUspRecoEngineAdapter::ShouldResetAfterTurnStopped()
{
    return m_allowUspResetAfterAudioByteCount && m_uspAudioByteCount > m_resetUspAfterAudioByteCount;
}

void CSpxUspRecoEngineAdapter::ResetAfterTurnStopped()
{
    SPX_DBG_ASSERT(ShouldResetAfterTurnStopped());
    SPX_DBG_TRACE_VERBOSE("%s: this=0x%8x ... USP-RESET", __FUNCTION__, this);

    // Let's terminate our current usp connection and sever our callbacks
    UspTerminate();

    // If we're in the ready/idle state, be sure and re-initialize the usp
    if (IsState(AudioState::Ready, UspState::Idle))
    {
        UspInitialize();
    }
}

bool CSpxUspRecoEngineAdapter::ShouldResetBeforeFirstAudio()
{
    return m_allowUspResetAfterTime && std::chrono::system_clock::now() > m_uspResetTime;
}

void CSpxUspRecoEngineAdapter::ResetBeforeFirstAudio()
{
    SPX_DBG_ASSERT(ShouldResetBeforeFirstAudio() && IsState(AudioState::Ready, UspState::Idle));
    SPX_DBG_TRACE_VERBOSE("%s: this=0x%8x ... USP-RESET", __FUNCTION__, this);

    // Let's terminate our current usp connection and sever our callbacks
    UspTerminate();
}


} } } } // Microsoft::CognitiveServices::Speech::Impl
