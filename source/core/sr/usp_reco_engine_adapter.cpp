//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// usp_reco_engine_adapter.cpp: Implementation definitions for CSpxUspRecoEngineAdapter C++ class
//

#include <vector>
#include <array>

#include "stdafx.h"
#include "usp_reco_engine_adapter.h"
#include "file_utils.h"
#include <inttypes.h>
#include <cstring>
#include <sstream>
#include <chrono>
#include <usp_text_message.h>
#include <usp_binary_message.h>
#include "service_helpers.h"
#include "create_object_helpers.h"
#include "exception.h"
#include "property_id_2_name_map.h"
#include "spx_build_information.h"
#include "platform.h"
#include "guid_utils.h"
#include "error_info.h"
#include "pronunciation_assessment_config.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

using namespace std;

using json = nlohmann::json;

CSpxUspRecoEngineAdapter::CSpxUspRecoEngineAdapter() :
    m_startingOffset(0),
    m_resetUspAfterAudioByteCount(0),
    m_uspAudioByteCount(0),
    m_audioState(AudioState::Idle),
    m_uspState(UspState::Idle)
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    SPX_DBG_TRACE_VERBOSE("%s: this=0x%8p", __FUNCTION__, (void*)this);
}

CSpxUspRecoEngineAdapter::~CSpxUspRecoEngineAdapter()
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    SPX_DBG_TRACE_VERBOSE("%s: this=0x%8p", __FUNCTION__, (void*)this);
    SPX_DBG_ASSERT(m_uspCallbacks == nullptr);
    SPX_DBG_ASSERT(m_uspConnection == nullptr);
}

void CSpxUspRecoEngineAdapter::Init()
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    SPX_DBG_TRACE_VERBOSE("%s: this=0x%8p", __FUNCTION__, (void*)this);

    SPX_IFTRUE_THROW_HR(GetSite() == nullptr, SPXERR_UNINITIALIZED);
    SPX_IFTRUE_THROW_HR(m_uspConnection != nullptr, SPXERR_ALREADY_INITIALIZED);
    SPX_IFTRUE_THROW_HR(m_uspCallbacks != nullptr, SPXERR_ALREADY_INITIALIZED);
    m_message_name_to_type_map =
    {
        { "speech.event", USP::MessageType::SpeechEvent },
        { "event", USP::MessageType::Event },
        { "speech.context", USP::MessageType::Context },
        { "speech.config", USP::MessageType::Config },
        { "speech.agent", USP::MessageType::Agent },
        { "speech.agentcontext", USP::MessageType::AgentContext },
        { "ssml", USP::MessageType::Ssml }
    };

    SPX_DBG_ASSERT(IsState(AudioState::Idle) && IsState(UspState::Idle));

    // Get a couple of control properties.
    auto properties = SpxQueryService<ISpxNamedProperties>(GetSite());
    SPX_IFTRUE_THROW_HR(properties == nullptr, SPXERR_UNEXPECTED_USP_SITE_FAILURE);

    m_resetUspAfterAudioSeconds = stoul(properties->GetStringValue("SPEECH-ForceUSPReconnectionByAudioSentTimeSeconds", "120"));
    m_allowUspResetAfterAudioByteCount = 0 != m_resetUspAfterAudioSeconds;

    m_resetUspAfterTimeSeconds = stoul(properties->GetStringValue("SPEECH-ForceUSPReconnectionByTimeConnectedSeconds", "240"));
    m_allowUspResetAfterTime = 0 != m_resetUspAfterAudioSeconds;
}

void CSpxUspRecoEngineAdapter::Term()
{
    SPX_DBG_TRACE_SCOPE("Terminating CSpxUspRecoEngineAdapter...", "Terminating CSpxUspRecoEngineAdapter... Done!");
    SPX_DBG_TRACE_VERBOSE("%s: this=0x%8p", __FUNCTION__, (void*)this);

    if (TryChangeState(UspState::Terminating))
    {
        SPX_DBG_TRACE_VERBOSE("%s: Terminating USP Connection (0x%8p)", __FUNCTION__, (void*)m_uspConnection.get());
#ifdef _DEBUG
        m_audioLogger.reset();
#endif

        UspTerminate();
        TryChangeState(UspState::Zombie);
    }
    else
    {
        SPX_TRACE_ERROR("%s: (0x%8p) UNEXPECTED USP State transition ... (audioState/uspState=%d/%d)", __FUNCTION__, (void*)this, m_audioState, m_uspState);
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
    const char* recoModePropertyName = GetPropertyName(PropertyId::SpeechServiceConnection_RecoMode);
    std::string currentRecoMode = properties->GetStringValue(recoModePropertyName);
    std::string recoModeToSet;

    // Get recognizer type
    uint16_t countSpeech, countIntent, countTranslation, countDialog, countTranscriber;
    GetSite()->GetScenarioCount(&countSpeech, &countIntent, &countTranslation, &countDialog, &countTranscriber);
    SPX_DBG_ASSERT(countSpeech + countIntent + countTranslation + countDialog + countTranscriber == 1); // currently only support one recognizer

    if (countIntent == 1)
    {
        // The connection to service for IntentRecognizer is depending on the Intent model being used,
        // so it is not possible to set up the connection now.
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
    else if (countDialog == 1)
    {
        recoModeToSet = g_recoModeInteractive;
    }

    // Set reco mode.
    if (currentRecoMode.empty())
    {
        properties->SetStringValue(recoModePropertyName, recoModeToSet.c_str());
    }
    else
    {
        // If the reco mode is set to dictation (which can only be set before starting any recognition), just use it.
        // But switching between interactive and conversation after connection setup is not allowed.
        SPX_IFTRUE_THROW_HR((currentRecoMode.compare(g_recoModeDictation) != 0 && currentRecoMode != recoModeToSet), SPXERR_SWITCH_MODE_NOT_ALLOWED);
    }

    // Establish the connection to service.
    EnsureUspInit();
}

void CSpxUspRecoEngineAdapter::CloseConnection()
{
    SPX_DBG_TRACE_VERBOSE("%s: Close connection.", __FUNCTION__);

    // Get recognizer type
    uint16_t countSpeech, countIntent, countTranslation, countDialog, countTranscriber;
    GetSite()->GetScenarioCount(&countSpeech, &countIntent, &countTranslation, &countDialog, &countTranscriber);
    SPX_DBG_ASSERT(countSpeech + countIntent + countTranslation + countDialog + countTranscriber == 1); // currently only support one recognizer

    if (countIntent == 1)
    {
        // The connection to service for IntentRecognizer is depending on the Intent model being used,
        // so it is not possible to set up the connection now.
        SPX_DBG_TRACE_INFO("%s: Skip setting up connection in advance for intent recognizer.", __FUNCTION__);
        SPX_THROW_HR(SPXERR_CHANGE_CONNECTION_STATUS_NOT_ALLOWED);
        return;
    }
    // Terminate the connection to service.
    UspTerminate();
}

void CSpxUspRecoEngineAdapter::SendSpeechEventMessage(std::string&& message)
{
    // Establish the connection to service.
    EnsureUspInit();
    UspSendMessage("speech.event", message, USP::MessageType::SpeechEvent);
}

USP::MessageType CSpxUspRecoEngineAdapter::GetMessageType(const std::string& path)
{
    return m_message_name_to_type_map.find(path) == m_message_name_to_type_map.end() ? USP::MessageType::Unknown : m_message_name_to_type_map[path];
}

void CSpxUspRecoEngineAdapter::SendNetworkMessage(std::string&& path, std::string&& payload, const std::shared_ptr<std::promise<bool>>& pr)
{
    // Establish the connection to service.
    EnsureUspInit();

    // for some reason, no connection is established
    if (m_uspConnection == nullptr || IsState(UspState::Error))
    {
        std::string error_message{ "Invalid state in USP adapter when sending " };
        error_message += path;
        return;
    }

    auto message = std::make_unique<USP::TextMessage>(move(payload), path, GetMessageType(path));
    message->SetMessageSentPromise(pr);
    return UspSendMessage(move(message));
}

void CSpxUspRecoEngineAdapter::SendNetworkMessage(std::string&& path, std::vector<uint8_t>&& payload, const std::shared_ptr<std::promise<bool>>& pr)
{
    // Establish the connection to service.
    EnsureUspInit();

    // for some reason, no connection is established
    if (m_uspConnection == nullptr || IsState(UspState::Error))
    {
        ThrowRuntimeError("No usp connection.");
    }
    auto message = std::make_unique<USP::BinaryMessage>(move(payload), path, GetMessageType(path));
    message->SetMessageSentPromise(pr);
    return UspSendMessage(move(message));
}

void CSpxUspRecoEngineAdapter::SetFormat(const SPXWAVEFORMATEX* pformat)
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    SPX_DBG_TRACE_VERBOSE("%s: this=0x%8p", __FUNCTION__, (void*)this);

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
        SPX_DBG_TRACE_VERBOSE("%s: (0x%8p) IGNORING... (audioState/uspState=%d/%d) USP-ZOMBIE", __FUNCTION__, (void*)this, m_audioState, m_uspState);
    }
    else if (IsBadState() && !IsState(UspState::Terminating))
    {
        // In case of error there still can be some calls to SetFormat in flight.
        SPX_DBG_TRACE_VERBOSE("%s: (0x%8p) IGNORING... (audioState/uspState=%d/%d)", __FUNCTION__, (void*)this, m_audioState, m_uspState);
        if (pformat == nullptr)
        {
            /* We still need to do this in order for the session to not go into a non recoverable state */
            InvokeOnSite([this](const SitePtr& p) { p->AdapterCompletedSetFormatStop(this); });
        }
    }
    else if (pformat != nullptr && IsState(UspState::Idle) && TryChangeState(AudioState::Idle, AudioState::Ready))
    {
        // we could call site when errors happen.
        // The m_audioState is Ready at this time. So, if we have two SetFormat calls in a row, the next one won't come in here
        // it goes to the else.
        //
        SPX_DBG_TRACE_VERBOSE("%s: (0x%8p)->PrepareFirstAudioReadyState()", __FUNCTION__, (void*)this);
        PrepareFirstAudioReadyState(pformat);
    }
    else if (pformat == nullptr && (TryChangeState(AudioState::Idle) || IsState(UspState::Terminating)))
    {
        SPX_DBG_TRACE_VERBOSE("%s: (0x%8p) site->AdapterCompletedSetFormatStop()", __FUNCTION__, (void*)this);
        InvokeOnSite([this](const SitePtr& p) { p->AdapterCompletedSetFormatStop(this); });

        m_format.reset();
    }
    else
    {
        SPX_TRACE_ERROR("%s: (0x%8p) UNEXPECTED USP State transition ... (audioState/uspState=%d/%d)", __FUNCTION__, (void*)this, m_audioState, m_uspState);
    }
}

void CSpxUspRecoEngineAdapter::ProcessAudio(const DataChunkPtr& audioChunk)
{
    auto size = audioChunk->size;
    if (IsState(UspState::Zombie) && size == 0)
    {
        SPX_DBG_TRACE_VERBOSE("%s: (0x%8p) IGNORING... size=0 ... (audioState/uspState=%d/%d) USP-ZOMBIE", __FUNCTION__, (void*)this, m_audioState, m_uspState);
    }
    else if (IsBadState())
    {
        // In case of error there still can be some calls to ProcessAudio in flight.
        SPX_DBG_TRACE_VERBOSE("%s: (0x%8p) IGNORING... (audioState/uspState=%d/%d)", __FUNCTION__, (void*)this, m_audioState, m_uspState);
    }
    else if (size > 0 && TryChangeState(AudioState::Ready, UspState::Idle, AudioState::Sending, UspState::WaitingForTurnStart))
    {
        SPX_DBG_TRACE_VERBOSE_IF(1, "%s: (0x%8p)->PrepareUspAudioStream() ... size=%d", __FUNCTION__, (void*)this, size);
        PrepareUspAudioStream();
        ProcessAudioChunk(audioChunk);

        SPX_DBG_TRACE_VERBOSE("%s: site->AdapterStartingTurn()", __FUNCTION__);
        InvokeOnSite([this](const SitePtr& p) { p->AdapterStartingTurn(this); });
    }
    else if (audioChunk->size > 0 && IsState(AudioState::Sending))
    {
        SPX_DBG_TRACE_VERBOSE_IF(1, "%s: (0x%8p) Sending Audio ... size=%d", __FUNCTION__, (void*)this, size);
        ProcessAudioChunk(audioChunk);
    }
    else if (size == 0 && IsState(AudioState::Sending))
    {
        SPX_DBG_TRACE_VERBOSE_IF(1, "%s: (0x%8p) Flushing Audio ... size=0 USP-FLUSH", __FUNCTION__, (void*)this);
        FlushAudio(true);
    }
    else if (!IsState(AudioState::Sending))
    {
        SPX_DBG_TRACE_VERBOSE_IF(1, "%s: (0x%8p) Ignoring audio size=%d ... (audioState/uspState=%d/%d)", __FUNCTION__, (void*)this, size, m_audioState, m_uspState);
    }
    else
    {
        SPX_TRACE_ERROR("%s: (0x%8p) UNEXPECTED USP State transition ... (audioState/uspState=%d/%d)", __FUNCTION__, (void*)this, m_audioState, m_uspState);
    }
}

void CSpxUspRecoEngineAdapter::SendAgentMessage(const std::string& buffer)
{
    SPX_DBG_TRACE_VERBOSE("%s: this=0x%8p", __FUNCTION__, (void*)this);
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
    SPX_DBG_TRACE_VERBOSE("%s: this=0x%8p", __FUNCTION__, (void*)this);
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
    SetUserDefinedHttpHeaders(client);
    SetUspProxyInfo(properties, client);
    SetUspSingleTrustedCert(properties, client);

    // Construct config message payload
    SetSpeechConfigMessage(*properties);
    if (m_endpointType == USP::EndpointType::Dialog)
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
        auto error = ErrorInfo::FromExplicitError(CancellationErrorCode::ConnectionFailure, e.what());
        OnError(error);
    }
    catch (...)
    {
        SPX_TRACE_ERROR("Error: Unexpected exception in UspInitialize");
        auto error = ErrorInfo::FromExplicitError(
            CancellationErrorCode::ConnectionFailure,
            "Error: Unexpected exception in UspInitialize");
        OnError(error);
    }

    // if error occurs in the above client.Connect, return.
    if (uspConnection == nullptr)
    {
        return;
    }
    // Keep track of what time we initialized (so we can reset later)
    m_uspInitTime = std::chrono::system_clock::now();
    m_uspResetTime = m_uspInitTime + std::chrono::seconds(m_resetUspAfterTimeSeconds);

    // We're done!!
    m_uspCallbacks = uspCallbacks;
    m_uspConnection = std::move(uspConnection);

    if (m_uspConnection != nullptr)
    {
        properties->SetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_Url), m_uspConnection->GetConnectionUrl().c_str());

        UspSendSpeechConfig();
        /* This will noop for non dialog endpoints */
        UspSendAgentConfig();
    }
}

void CSpxUspRecoEngineAdapter::UspTerminate()
{
    // Inform upper layer about disconnection.
    if ((m_uspConnection != nullptr) && m_uspConnection->IsConnected())
    {
        auto disconnectionReason = ErrorInfo::FromWebSocket(WebSocketError::UNKNOWN, WebSocketDisconnectReason::Normal);
        OnDisconnected(disconnectionReason);
    }

    // Term the callbacks first and then reset/release the connection
    SpxTermAndClear(m_uspCallbacks); // After this ... we won't be called back on ISpxUspCallbacks ever again...

    // NOTE: Even if we are in a callback from the USP on another thread, we won't be destroyed until those functions release their shared ptrs
    m_uspConnection.reset();

    // Fix up some of our counters...
    m_uspAudioByteCount = 0;
}

USP::Client& CSpxUspRecoEngineAdapter::SetUspEndpoint(const std::shared_ptr<ISpxNamedProperties>& properties, USP::Client& client)
{
    SPX_DBG_ASSERT(GetSite() != nullptr);

    // How many recognizers of each type do we have?
    uint16_t countSpeech = 0, countIntent = 0, countTranslation = 0, countDialog = 0, countTranscriber = 0;
    GetSite()->GetScenarioCount(&countSpeech, &countIntent, &countTranslation, &countDialog, &countTranscriber);
    SPX_DBG_ASSERT(countSpeech + countIntent + countTranslation + countDialog + countTranscriber == 1); // currently only support one recognizer

    // set endpoint url if this is provided.
    auto endpoint = properties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_Endpoint));

    // See if we have a single use endpoint saved. This is used to redirect in response to a HTTP 307 and must be cleared if set after use.
    auto singleUseEndpoint = properties->GetStringValue("SPEECH-SingleUseEndpoint");
    if (!singleUseEndpoint.empty())
    {
        SPX_DBG_TRACE_VERBOSE("%s: Using single use redirect endpoint: %s", __FUNCTION__, endpoint.c_str());
        m_customEndpoint = true;
        client.SetEndpointUrl(singleUseEndpoint);

        // Set it to the default of empty.
        properties->SetStringValue("SPEECH-SingleUseEndpoint", "");
    }
    else if (!endpoint.empty())
    {
        SPX_DBG_TRACE_VERBOSE("%s: Using Custom endpoint: %s", __FUNCTION__, endpoint.c_str());
        m_customEndpoint = true;
        client.SetEndpointUrl(endpoint);
    }
    else
    {
        // Set host url if this is provided
        auto host = properties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_Host));
        if (!host.empty())
        {
            SPX_DBG_TRACE_VERBOSE("%s: Using custom host: %s", __FUNCTION__, host.c_str());
            m_customHost = true;
            client.SetHostUrl(host);
        }
    }

    // set user defined query parameters if provided.
    auto userDefinedQueryParameters = properties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_UserDefinedQueryParameters));
    if (!userDefinedQueryParameters.empty())
    {
        SPX_DBG_TRACE_VERBOSE("%s: Using user provided query parameters: %s", __FUNCTION__, userDefinedQueryParameters.c_str());
        client.SetUserDefinedQueryParameters(userDefinedQueryParameters);
    }

    // set endpoint type.
    if (countIntent == 1)
    {
        SetUspEndpointIntent(properties, client);
    }
    else if (countTranslation == 1)
    {
        SetUspEndpointTranslation(properties, client);
    }
    else if (countDialog == 1)
    {
        SetUspEndpointDialog(properties, client);
    }
     else if (countTranscriber == 1)
    {
        SetUspEndpointTranscriber(properties, client);
    }
    else
    {
        SPX_DBG_ASSERT(countSpeech == 1);
        SetUspEndpointDefaultSpeechService(properties, client);
    }

    SetUspRecoMode(properties, client);

    auto pollingInterval = static_cast<uint16_t>(stoi(properties->GetStringValue("SPEECH-USPPollingInterval", "10")));
    SPX_DBG_TRACE_VERBOSE("%s: Setting Websocket Polling interval to %d", __FUNCTION__, pollingInterval);

    client.SetPollingIntervalms(pollingInterval);

    return client;
}

USP::Client& CSpxUspRecoEngineAdapter::SetUspRecoMode(const std::shared_ptr<ISpxNamedProperties>& properties, USP::Client& client)
{
    // set reco mode based on recognizer type
    USP::RecognitionMode mode = USP::RecognitionMode::Interactive;
    SPXHR checkHr = SPX_NOERROR;
    // Reco mode should be already set in properties.
    checkHr = GetRecoModeFromProperties(properties, mode);
    SPX_THROW_HR_IF(checkHr, SPX_FAILED(checkHr));
    m_isInteractiveMode = mode == USP::RecognitionMode::Interactive;
    client.SetRecognitionMode(mode);
    SPX_DBG_TRACE_VERBOSE("%s: recoMode=%d", __FUNCTION__, mode);

    return client;
}

USP::Client& CSpxUspRecoEngineAdapter::SetUspEndpointTranscriber(const std::shared_ptr<ISpxNamedProperties>& properties, USP::Client& client)
{
    SPX_DBG_TRACE_VERBOSE("%s: Endpoint type: ConversationTranscriptionService", __FUNCTION__);
    m_endpointType = USP::EndpointType::ConversationTranscriptionService;
    client.SetEndpointType(m_endpointType);

    SetUspRegion(properties, client, /*isIntentRegion=*/ false);

    UpdateDefaultLanguage(properties);

    UpdateOutputFormatOption(properties);

    SetUspQueryParameters(USP::endpoint::conversationTranscriber::queryParameters, properties, client);

    return client;
}

USP::Client& CSpxUspRecoEngineAdapter::SetUspEndpointDialog(const std::shared_ptr<ISpxNamedProperties>& properties, USP::Client& client)
{
    SPX_DBG_TRACE_VERBOSE("%s: Endpoint type: Dialog.", __FUNCTION__);
    m_endpointType = USP::EndpointType::Dialog;
    client.SetEndpointType(m_endpointType);

    SetUspRegion(properties, client, /*isIntentRegion=*/ false);

    UpdateDefaultLanguage(properties);


    /* Set conversation id if present */
    m_dialogConversationId = properties->GetStringValue(GetPropertyName(PropertyId::Conversation_Conversation_Id));

    auto dialogType = properties->GetStringValue(GetPropertyName(PropertyId::Conversation_DialogType));
    USP::Client::DialogBackend dialogBackend{ USP::Client::DialogBackend::NotSet };
    if (dialogType == g_dialogType_BotFramework)
    {
        dialogBackend = USP::Client::DialogBackend::BotFramework;
        SetUspQueryParameters(USP::endpoint::dialog::botFramework::queryParameters, properties, client);
    }
    else if (dialogType == g_dialogType_CustomCommands)
    {
        dialogBackend = USP::Client::DialogBackend::CustomCommands;
        SetUspQueryParameters(USP::endpoint::dialog::customCommands::queryParameters, properties, client);
    }
    else
    {
        /* We shouldn't be here */
        SPX_THROW_HR(SPXERR_INVALID_ARG);
    }

    client.SetDialogBackend(dialogBackend);

    return client.SetAudioResponseFormat("raw-16khz-16bit-mono-pcm");
}

USP::Client& CSpxUspRecoEngineAdapter::SetUspEndpointIntent(const std::shared_ptr<ISpxNamedProperties>& properties, USP::Client& client)
{
    SPX_DBG_TRACE_VERBOSE("%s: Endpoint type: Intent.", __FUNCTION__);
    m_endpointType = USP::EndpointType::Intent;
    client.SetEndpointType(m_endpointType);

    SetUspRegion(properties, client, /*isIntentRegion=*/ true);

    UpdateDefaultLanguage(properties);

    // For intent, always use detailed output format.
    properties->SetStringValue(GetPropertyName(PropertyId::SpeechServiceResponse_OutputFormatOption), USP::endpoint::outputFormatDetailed);

    SetUspQueryParameters(USP::endpoint::luis::queryParameters, properties, client);
    return client;
}

USP::Client& CSpxUspRecoEngineAdapter::SetUspEndpointTranslation(const std::shared_ptr<ISpxNamedProperties>& properties, USP::Client& client)
{
    SPX_DBG_TRACE_VERBOSE("%s: Endpoint type: Translation.", __FUNCTION__);
    m_endpointType = USP::EndpointType::Translation;
    client.SetEndpointType(m_endpointType);

    SetUspRegion(properties, client, /*isIntentRegion=*/ false);

    UpdateOutputFormatOption(properties);

    SetUspQueryParameters(USP::endpoint::translation::queryParameters, properties, client);

    return client;
}

USP::Client& CSpxUspRecoEngineAdapter::SetUspEndpointDefaultSpeechService(const std::shared_ptr<ISpxNamedProperties>& properties, USP::Client& client)
{
    SPX_DBG_TRACE_VERBOSE("%s: Endpoint type: Speech.", __FUNCTION__);
    m_endpointType = USP::EndpointType::Speech;
    client.SetEndpointType(m_endpointType);

    SetUspRegion(properties, client, /*isIntentRegion=*/ false);

    UpdateDefaultLanguage(properties);

    UpdateOutputFormatOption(properties);

    SetUspQueryParameters(USP::endpoint::unifiedspeech::queryParameters, properties, client);

    return client;
}

void CSpxUspRecoEngineAdapter::SetUspQueryParametersInternal(const char *queryParamName, const ISpxNamedProperties *properties, USP::Client& client) const
{
    enum class PropertyValueType {
        StringProperty,
        IntProperty,
        BoolProperty
    };

    using TupleType = std::tuple<const char *, const char *, PropertyValueType>;
    constexpr std::array<TupleType, 17> QueryParamInfo{{
        TupleType{ USP::endpoint::langQueryParam, GetPropertyName(PropertyId::SpeechServiceConnection_RecoLanguage), PropertyValueType::StringProperty },
        TupleType{ USP::endpoint::deploymentIdQueryParam, GetPropertyName(PropertyId::SpeechServiceConnection_EndpointId), PropertyValueType::StringProperty },
        TupleType{ USP::endpoint::initialSilenceTimeoutQueryParam, GetPropertyName(PropertyId::SpeechServiceConnection_InitialSilenceTimeoutMs), PropertyValueType::IntProperty },
        TupleType{ USP::endpoint::endSilenceTimeoutQueryParam, GetPropertyName(PropertyId::SpeechServiceConnection_EndSilenceTimeoutMs), PropertyValueType::IntProperty },
        TupleType{ USP::endpoint::storeAudioQueryParam, GetPropertyName(PropertyId::SpeechServiceConnection_EnableAudioLogging), PropertyValueType::BoolProperty },
        TupleType{ USP::endpoint::outputFormatQueryParam, GetPropertyName(PropertyId::SpeechServiceResponse_OutputFormatOption), PropertyValueType::StringProperty },
        TupleType{ USP::endpoint::wordLevelTimestampsQueryParam, GetPropertyName(PropertyId::SpeechServiceResponse_RequestWordLevelTimestamps), PropertyValueType::BoolProperty },
        TupleType{ USP::endpoint::profanityQueryParam, GetPropertyName(PropertyId::SpeechServiceResponse_ProfanityOption), PropertyValueType::StringProperty },
        TupleType{ USP::endpoint::stableIntermediateThresholdQueryParam, GetPropertyName(PropertyId::SpeechServiceResponse_StablePartialResultThreshold), PropertyValueType::IntProperty },
        TupleType{ USP::endpoint::unifiedspeech::postprocessingQueryParam, GetPropertyName(PropertyId::SpeechServiceResponse_PostProcessingOption), PropertyValueType::StringProperty },
        TupleType{ USP::endpoint::unifiedspeech::lidEnabledQueryParam, GetPropertyName(PropertyId::SpeechServiceConnection_AutoDetectSourceLanguages), PropertyValueType::StringProperty },
        TupleType{ USP::endpoint::translation::fromQueryParam, GetPropertyName(PropertyId::SpeechServiceConnection_RecoLanguage), PropertyValueType::StringProperty },
        TupleType{ USP::endpoint::translation::toQueryParam, GetPropertyName(PropertyId::SpeechServiceConnection_TranslationToLanguages), PropertyValueType::StringProperty },
        TupleType{ USP::endpoint::translation::voiceQueryParam, GetPropertyName(PropertyId::SpeechServiceConnection_TranslationVoice), PropertyValueType::StringProperty },
        TupleType{ USP::endpoint::translation::stableTranslationQueryParam, GetPropertyName(PropertyId::SpeechServiceResponse_TranslationRequestStablePartialResult), PropertyValueType::BoolProperty },
        TupleType{ USP::endpoint::dialog::customVoiceDeploymentIdsQueryParam, GetPropertyName(PropertyId::Conversation_Custom_Voice_Deployment_Ids), PropertyValueType::StringProperty },
        TupleType{ USP::endpoint::dialog::botIdQueryParam, GetPropertyName(PropertyId::Conversation_ApplicationId), PropertyValueType::StringProperty }
    }};

    auto QueryParameterToPropertyId = [&](const char *queryName) -> const TupleType&
    {
        for (auto& info : QueryParamInfo)
        {
            const auto& name = std::get<0>(info);
            if (strcmp(name, queryName) == 0)
            {
                return info;
            }
        }
        ThrowLogicError(std::string{ "parameter name cannot be mapped to property name: " } + queryName);
        return QueryParamInfo[0];
    };
    bool setValue = false;
    auto entry =  QueryParameterToPropertyId(queryParamName);

    const auto& propertyName = std::get<1>(entry);
    auto found = properties->HasStringValue(propertyName);
    if (!found)
    {
        return;
    }

    auto propertyValueStr = properties->GetStringValue(propertyName);
    const auto& propertyType = std::get<2>(entry);
    switch (propertyType)
    {
    case PropertyValueType::BoolProperty:
        if (propertyValueStr != TrueString && propertyValueStr != FalseString)
        {
            ThrowInvalidArgumentException("The boolean property " + string(propertyName) + " has an invalid value: " + propertyValueStr);
        }
        else
        {
            setValue = true;
        }
        break;
    case PropertyValueType::IntProperty:
        if (stoi(propertyValueStr) >= 0)
        {
            setValue = true;
        }
        else
        {
            ThrowInvalidArgumentException(std::string{ "The property " } + propertyName + " has an invalid value: " + propertyValueStr);
        }
        break;
    case PropertyValueType::StringProperty:
        if (!propertyValueStr.empty())
        {
            setValue = true;
        }
        else
        {
            SPX_TRACE_ERROR("The property %s has an empty value, ignored.", propertyName);
        }
        break;
    default:
        ThrowLogicError("Unexpected PropertyValueType.");
        break;
    }

    if (setValue)
    {
        if(strcmp(queryParamName, USP::endpoint::unifiedspeech::lidEnabledQueryParam) == 0)
        {
            client.SetQueryParameter(queryParamName, TrueString);
        }
        else
        {
            client.SetQueryParameter(queryParamName, propertyValueStr);
        }
    }
}

USP::Client& CSpxUspRecoEngineAdapter::SetUspRegion(const std::shared_ptr<ISpxNamedProperties>& properties, USP::Client& client, bool isIntentRegion)
{
    auto region = isIntentRegion ? properties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_IntentRegion))
        : properties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_Region));
    if (!m_customEndpoint && !m_customHost)
    {
        SPX_IFTRUE_THROW_HR(region.empty(), SPXERR_INVALID_REGION);
        isIntentRegion ? client.SetIntentRegion(region) : client.SetRegion(region);
    }
    else
    {
        if (!region.empty())
        {
            SPX_TRACE_ERROR("when using custom endpoint, region should not be specified separately.");
            SPX_THROW_HR(SPXERR_INVALID_ARG);
        }
    }
    return client;
}

USP::Client& CSpxUspRecoEngineAdapter::SetUspAuthentication(const std::shared_ptr<ISpxNamedProperties>& properties, USP::Client& client)
{
    // Get the properties that indicates what endpoint to use...
    auto uspSubscriptionKey = properties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_Key));
    auto uspAuthToken = properties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceAuthorization_Token));
    auto uspRpsToken = properties->GetStringValue("SPEECH-RpsToken");
    auto uspDialogApplicationId = properties->GetStringValue(GetPropertyName(PropertyId::Conversation_ApplicationId));
    auto uspConversationToken = properties->GetStringValue("ConversationTranslator_Token");

    std::array<std::string, static_cast<size_t>(USP::AuthenticationType::SIZE_AUTHENTICATION_TYPE)> authData;


    authData[static_cast<size_t>(USP::AuthenticationType::SubscriptionKey)] = std::move(uspSubscriptionKey);
    authData[static_cast<size_t>(USP::AuthenticationType::AuthorizationToken)] = std::move(uspAuthToken);
    authData[static_cast<size_t>(USP::AuthenticationType::SearchDelegationRPSToken)] = std::move(uspRpsToken);
    authData[static_cast<size_t>(USP::AuthenticationType::DialogApplicationId)] = std::move(uspDialogApplicationId);
    authData[static_cast<size_t>(USP::AuthenticationType::ConversationToken)] = std::move(uspConversationToken);

    return client.SetAuthentication(authData);
}

USP::Client& CSpxUspRecoEngineAdapter::SetUserDefinedHttpHeaders(USP::Client& client)
{
    auto headers = FindPrefix("HttpHeader");
    return client.SetUserDefinedHttpHeaders(headers);
}

void CSpxUspRecoEngineAdapter::UpdateDefaultLanguage(const std::shared_ptr<ISpxNamedProperties>& properties)
{
    constexpr auto languageParameterName = GetPropertyName(PropertyId::SpeechServiceConnection_RecoLanguage);
    // if the language parameter has been set, no needs to set the default language.
    if (!properties->GetStringValue(languageParameterName).empty())
    {
        return;
    }
    // If no custom model is used, set the default language.
    if (properties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_EndpointId)).empty() &&
        properties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_Endpoint)).find(USP::endpoint::deploymentIdQueryParam) == string::npos)
    {
        properties->SetStringValue(languageParameterName, s_defaultRecognitionLanguage);
    }
}

void CSpxUspRecoEngineAdapter::UpdateOutputFormatOption(const std::shared_ptr<ISpxNamedProperties>& properties)
{
    auto outputFormatOptionName = GetPropertyName(PropertyId::SpeechServiceResponse_OutputFormatOption);
    if (!properties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceResponse_RequestWordLevelTimestamps)).empty())
    {
        // Word level timestamp always use detailed format.
        properties->SetStringValue(outputFormatOptionName, USP::endpoint::outputFormatDetailed);
        return;
    }

    if (!properties->GetStringValue(outputFormatOptionName).empty())
    {
        // do not overwrite the option value that has been set.
        return;
    }

    auto value = properties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceResponse_RequestDetailedResultTrueFalse));
    if (!value.empty())
    {
        if (value == FalseString)
        {
            properties->SetStringValue(outputFormatOptionName, USP::endpoint::outputFormatSimple);
        }
        else if (value == TrueString)
        {
            properties->SetStringValue(outputFormatOptionName, USP::endpoint::outputFormatDetailed);
        }
        else
        {
            SPX_TRACE_ERROR("Unknown output format value %s", value.c_str());
            SPX_THROW_HR(SPXERR_INVALID_ARG);
        }
    }
    return;
}

USP::Client& CSpxUspRecoEngineAdapter::SetUspSingleTrustedCert(const std::shared_ptr<ISpxNamedProperties>& properties, USP::Client& client)
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

USP::Client& CSpxUspRecoEngineAdapter::SetUspProxyInfo(const std::shared_ptr<ISpxNamedProperties>& properties, USP::Client& client)
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
        SPX_TRACE_ERROR("Unknown RecognitionMode value %s", value.c_str());
        hr = SPXERR_INVALID_ARG;
    }

    return hr;
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

    auto keywords = properties.GetStringValue(KEYWORDS_PROPERTY_NAME);
    if (keywords != "")
    {
        json keywordsJSON;

        auto keywordsVector = PAL::split(keywords, ';');

        auto keywordsTimeout = stoi(properties.GetStringValue("SPEECH-KeywordSearchTimeout", "5000"));

        for (auto oneKeyword : keywordsVector)
        {
            json oneKeywordJSON;
            oneKeywordJSON = { { "text", oneKeyword }, { "triggerThreshold", "Medium" }, { "timeout", keywordsTimeout } };

            keywordsJSON.push_back(oneKeywordJSON);
        }

        json keywordRegistry;
        keywordRegistry = keywordRegistry.array({ json({{"type", "startTrigger" },
                                                {"keywords", keywordsJSON } }) });

        speechConfig["context"]["keywordRegistry"] = keywordRegistry;

    }

    auto userDefinedParams = GetParametersFromUser("speech.config");
    for (const auto& item : userDefinedParams)
    {
        const auto& name = item.first;
        const auto& value = item.second;

        if (value.length()<1)
        {
            ThrowInvalidArgumentException("User must provide non-empty value for a speech.config field.");
        }
        speechConfig["context"][name] = json::parse(value);
        SPX_DBG_TRACE_VERBOSE("Set '%s' as '%s' in speech.config.", name.c_str(), value.c_str());
    }
    m_speechConfig = speechConfig.dump();
}

void CSpxUspRecoEngineAdapter::SetAgentConfigMessage(const ISpxNamedProperties& properties)
{
    constexpr auto dialogCommunicationType = "Conversation_Communication_Type";
    constexpr auto dialogConnectionId = GetPropertyName(PropertyId::Conversation_ApplicationId);
    constexpr auto dialogFromId = GetPropertyName(PropertyId::Conversation_From_Id);
    constexpr auto dialogTTSAudioFormat = GetPropertyName(PropertyId::SpeechServiceConnection_SynthOutputFormat);
    constexpr auto dialogBackendType = GetPropertyName(PropertyId::Conversation_DialogType);
    constexpr auto recoLanguage = GetPropertyName(PropertyId::SpeechServiceConnection_RecoLanguage);

    json agentConfigJson;
    agentConfigJson["version"] = 0.2;

    auto connectionId = properties.GetStringValue(dialogConnectionId);
    auto dialogType = properties.GetStringValue(dialogBackendType);

    if (connectionId.empty() && (dialogType == g_dialogType_CustomCommands))
    {
        SPX_THROW_HR(SPXERR_INVALID_ARG);
    }
    agentConfigJson["botInfo"]["connectionId"] = connectionId;

    auto communicationType = properties.GetStringValue(dialogCommunicationType);
    if (communicationType.empty())
    {
        communicationType = "Default";
    }

    agentConfigJson["botInfo"]["commType"] = communicationType;

    auto ttsAudioFormat = properties.GetStringValue(dialogTTSAudioFormat);
    if (!ttsAudioFormat.empty())
    {
        agentConfigJson["ttsAudioFormat"] = ttsAudioFormat;
    }

    if (!m_dialogConversationId.empty())
    {
        agentConfigJson["botInfo"]["conversationId"] = m_dialogConversationId;
    }

    if (dialogType == g_dialogType_BotFramework)
    {
        auto fromId = properties.GetStringValue(dialogFromId);
        if (!fromId.empty())
        {
            agentConfigJson["botInfo"]["fromId"] = fromId;
        }
    }
    else if (dialogType == g_dialogType_CustomCommands)
    {
        auto language = properties.GetStringValue(recoLanguage);
        if (!language.empty())
        {
            agentConfigJson["botInfo"]["commandsCulture"] = language;
        }
    }

    m_agentConfig = agentConfigJson.dump();
}

void CSpxUspRecoEngineAdapter::UspSendSpeechConfig()
{
    constexpr auto messagePath = "speech.config";
    SPX_DBG_TRACE_VERBOSE("%s %s", messagePath, m_speechConfig.c_str());
    UspSendMessage(
        std::make_unique<USP::TextMessage>(
            m_speechConfig, messagePath, "application/json", USP::MessageType::Config));
}

void CSpxUspRecoEngineAdapter::UspSendAgentConfig()
{
    constexpr auto messagePath = "agent.config";
    /* Only send it for dialog endpoint */
    if (m_endpointType == USP::EndpointType::Dialog)
    {
        SPX_DBG_TRACE_VERBOSE("%s %s", messagePath, m_agentConfig.c_str());
        UspSendMessage(messagePath, m_agentConfig, USP::MessageType::Config);
    }
}

void CSpxUspRecoEngineAdapter::UspSendSpeechAgentContext()
{
    // The Dialog Service Connector is responsible for generating an interaction ID here, so we send it as a speech.agent.context message
    if (m_endpointType == USP::EndpointType::Dialog)
    {
        auto site = GetSite();
        auto provider = SpxQueryInterface<ISpxInteractionIdProvider>(site);
        auto interactionId = provider->GetInteractionId(InteractionIdPurpose::Speech);

        auto properties = SpxQueryService<ISpxNamedProperties>(GetSite());
        SPX_IFTRUE_THROW_HR(properties == nullptr, SPXERR_UNEXPECTED_USP_SITE_FAILURE);

        auto activityTemplate = properties->GetStringValue(GetPropertyName(PropertyId::Conversation_Speech_Activity_Template));

        json contextJson = {
            {"version", 0.5},
            {"context", {
                {"interactionId", interactionId}
            }},
            {"channelData", ""}
        };

        if (activityTemplate.size() > 0)
        {
            contextJson["messagePayload"] = activityTemplate;
        }

        UspSendMessage("speech.agent.context", contextJson.dump(), USP::MessageType::AgentContext);
    }
}

void CSpxUspRecoEngineAdapter::UspSendSpeechEvent()
{
    auto site = GetSite();
    auto provider = SpxQueryInterface<ISpxSpeechEventPayloadProvider>(site);
    SPX_IFTRUE_THROW_HR(provider == nullptr, SPXERR_UNEXPECTED_USP_SITE_FAILURE);

    // true for starting streaming audio.
    auto payload = provider->GetSpeechEventPayload(true);

    if (!payload.empty())
    {
        UspSendMessage("speech.event", payload, USP::MessageType::SpeechEvent);
    }
}

void CSpxUspRecoEngineAdapter::UspSendSpeechContext()
{
    // create speech context json
    auto speechContext = GetSpeechContextJson();

    if (!speechContext.empty())
    {
        std::string messagePath = "speech.context";
        UspSendMessage(messagePath, speechContext.dump(), USP::MessageType::Context);
    }
 }

void CSpxUspRecoEngineAdapter::UspSendMessage(const std::string& messagePath, const std::string &buffer, USP::MessageType messageType)
{
    SPX_DBG_TRACE_VERBOSE("%s='%s'", messagePath.c_str(), buffer.c_str());
    return UspSendMessage(std::make_unique<USP::TextMessage>(buffer, messagePath, messageType));
}

void CSpxUspRecoEngineAdapter::UspSendMessage(std::unique_ptr<USP::Message> message)
{
    SPX_DBG_ASSERT(m_uspConnection != nullptr || IsState(UspState::Terminating) || IsState(UspState::Zombie));
    if (IsBadState() || m_uspConnection == nullptr)
    {
        /* Notify user there was an error */
        InvokeOnSite([this](const SitePtr& p)
        {
            auto error = ErrorInfo::FromExplicitError(CancellationErrorCode::ConnectionFailure, "Connection is in a bad state.");
            p->Error(this, error);
        });
        SPX_TRACE_ERROR("no connection established or in bad USP state. m_uspConnection %s nullptr, m_uspState:%d.", m_uspConnection == nullptr ? "is" : "is not", m_uspState);

        return;
    }
    return m_uspConnection->SendMessage(move(message));
}

//
// This is called for writing the audio file format
//
void CSpxUspRecoEngineAdapter::ProcessAudioFormat(SPXWAVEFORMATEX* pformat)
{
    bool useAudioCompression = m_compressionCodec != nullptr;

    if (!useAudioCompression
#ifdef _DEBUG
        || m_audioLogger
#endif
    )
    {
        auto wavHeaderDataPtr = MakeDataChunkForAudioFormat(pformat);

        m_uspAudioByteCount += wavHeaderDataPtr->size;
        if (!useAudioCompression)
        {
            UspWriteActual(wavHeaderDataPtr);
        }

#ifdef _DEBUG
        if (m_audioLogger)
        {
            m_audioLogger->WriteAudio(wavHeaderDataPtr->data.get(), wavHeaderDataPtr->size);
        }
#endif
    }
}

void CSpxUspRecoEngineAdapter::ProcessAudioChunk(const DataChunkPtr& audioChunk)
{
    m_uspAudioByteCount += audioChunk->size;

    if (m_compressionCodec != nullptr)
    {
        m_compressionCodec->Encode(audioChunk->data.get(), audioChunk->size);
    }
    else
    {
        UspWriteActual(audioChunk);
    }

#ifdef _DEBUG
    if (m_audioLogger)
    {
        m_audioLogger->WriteAudio(audioChunk->data.get(), audioChunk->size);
    }
#endif
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
        SPX_TRACE_ERROR("%s: unexpected USP connection state:%d. Not sending audio chunk (size=%d).", __FUNCTION__, m_uspState, audioChunk->size);
    }
}

void CSpxUspRecoEngineAdapter::FlushAudio(bool flushCodec /*= false*/)
{
    // We should only ever be asked to Flush when we're in a valid state ...
    SPX_DBG_ASSERT(m_uspConnection != nullptr || IsState(UspState::Terminating) || IsState(UspState::Zombie));
    if (!IsState(UspState::Terminating) && !IsState(UspState::Zombie) && m_uspConnection != nullptr)
    {
        // Get the remains of compressed audio out of the codec
        if (m_compressionCodec != nullptr && flushCodec && !m_audioFlushed)
        {
            m_compressionCodec->Flush();
            m_audioFlushed = true;
        }

        m_uspConnection->FlushAudio();
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

void CSpxUspRecoEngineAdapter::OnMessageReceived(const USP::RawMsg& m)
{
    // Check the message for offset or tokens.

    InvokeOnSite([&](const SitePtr& p) { p->FireConnectionMessageReceived(m.headers, m.path, m.buffer, m.bufferSize, m.isBufferBinary); });
}

void CSpxUspRecoEngineAdapter::OnSpeechStartDetected(const USP::SpeechStartDetectedMsg& message)
{
    // The USP message for SpeechStartDetected isn't what it might sound like in all "reco modes" ...
    // * In INTERACTIVE mode, it works as it sounds. It indicates the beginning of speech for the "phrase" message that will arrive later
    // * In CONTINUOUS modes, however, it corresponds to the time of the beginning of speech for the FIRST "phrase" message of many inside one turn

    SPX_DBG_TRACE_VERBOSE("Response: Speech.StartDetected message. Speech starts at offset %" PRIu64 " (100ns).\n", message.offset);

    if (IsBadState())
    {
        SPX_DBG_TRACE_VERBOSE("%s: (0x%8p) IGNORING... (audioState/uspState=%d/%d) %s", __FUNCTION__, (void*)this, m_audioState, m_uspState, IsState(UspState::Terminating) ? "(USP-TERMINATING)" : "********** USP-UNEXPECTED !!!!!!");
    }
    else if (IsState(UspState::WaitingForPhrase))
    {
        SPX_DBG_TRACE_VERBOSE("%s: (0x%8p) site->AdapterDetectedSpeechStart()", __FUNCTION__, (void*)this);
        InvokeOnSite([this, &message](const SitePtr& p) { p->AdapterDetectedSpeechStart(this, message.offset + m_startingOffset); });
    }
    else
    {
        SPX_TRACE_ERROR("%s: (0x%8p) UNEXPECTED USP State transition ... (audioState/uspState=%d/%d)", __FUNCTION__, (void*)this, m_audioState, m_uspState);
    }
}

void CSpxUspRecoEngineAdapter::OnSpeechEndDetected(const USP::SpeechEndDetectedMsg& message)
{
    SPX_DBG_TRACE_VERBOSE("Response: Speech.EndDetected message. Speech ends at offset %" PRIu64 " (100ns)\n", message.offset + m_startingOffset);

    auto requestMute = TryChangeState(AudioState::Sending, AudioState::Mute);

    if (IsBadState())
    {
        SPX_DBG_TRACE_VERBOSE("%s: (0x%8p) IGNORING... (audioState/uspState=%d/%d) %s", __FUNCTION__, (void*)this, m_audioState, m_uspState, IsState(UspState::Terminating) ? "(USP-TERMINATING)" : "********** USP-UNEXPECTED !!!!!!");
    }
    else if (IsStateBetweenIncluding(UspState::WaitingForPhrase, UspState::WaitingForTurnEnd) &&
             (IsState(AudioState::Idle) ||
              IsState(AudioState::Mute)))
    {
        SPX_DBG_TRACE_VERBOSE("%s: (0x%8p) site->AdapterDetectedSpeechEnd()", __FUNCTION__, (void*)this);
        InvokeOnSite([this, &message](const SitePtr& p) { p->AdapterDetectedSpeechEnd(this, message.offset + m_startingOffset); });
    }
    else
    {
        SPX_TRACE_ERROR("%s: (0x%8p) UNEXPECTED USP State transition ... (audioState/uspState=%d/%d)", __FUNCTION__, (void*)this, m_audioState, m_uspState);
        return;
    }

    SPX_DBG_TRACE_VERBOSE("%s: Flush ... (audioState/uspState=%d/%d)  USP-FLUSH", __FUNCTION__, m_audioState, m_uspState);

    FlushAudio();
    if (requestMute && !IsBadState())
    {
        SPX_DBG_TRACE_VERBOSE("%s: site->AdapterRequestingAudioMute(true) ... (audioState/uspState=%d/%d)", __FUNCTION__, m_audioState, m_uspState);
        InvokeOnSite([this](const SitePtr& p) { p->AdapterRequestingAudioMute(this, true); });
    }
}

void CSpxUspRecoEngineAdapter::OnSpeechHypothesis(const USP::SpeechHypothesisMsg& message)
{
    SPX_DBG_TRACE_VERBOSE("Response: Speech.Hypothesis message. Starts at offset %" PRIu64 ", with duration %" PRIu64 " (100ns). Text: %ls\n", message.offset + m_startingOffset, message.duration, message.text.c_str());

    if (IsBadState())
    {
        SPX_DBG_TRACE_VERBOSE("%s: (0x%8p) IGNORING... (audioState/uspState=%d/%d) %s", __FUNCTION__, (void*)this, m_audioState, m_uspState, IsState(UspState::Terminating) ? "(USP-TERMINATING)" : "********** USP-UNEXPECTED !!!!!!");
    }
    else if (IsState(UspState::WaitingForPhrase))
    {
        SPX_DBG_TRACE_VERBOSE("%s: site->FireAdapterResult_Intermediate()", __FUNCTION__);

        InvokeOnSite([&](const SitePtr& site)
        {
            auto factory = SpxQueryService<ISpxRecoResultFactory>(site);
            auto result = factory->CreateIntermediateResult(message.text.c_str(), message.offset + m_startingOffset, message.duration);
            auto namedProperties = SpxQueryInterface<ISpxNamedProperties>(result);
            namedProperties->SetStringValue(GetPropertyName(PropertyId::SpeechServiceResponse_JsonResult), PAL::ToString(message.json).c_str());
            if (!message.speaker.empty())
            {
                CreateConversationResult(result, message.speaker, message.id);
            }

            if (!message.language.empty())
            {
                namedProperties->SetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_AutoDetectSourceLanguageResult), message.language.c_str());
            }

            UpdateAdapterResult_JsonResult(result);
            site->FireAdapterResult_Intermediate(this, message.offset + m_startingOffset, result);
        });
    }
    else
    {
        SPX_TRACE_ERROR("%s: (0x%8p) UNEXPECTED USP State transition ... (audioState/uspState=%d/%d)", __FUNCTION__, (void*)this, m_audioState, m_uspState);
    }
}

#define JSON_KEY_NBEST  "NBest"
#define JSON_KEY_WORDS  "Words"
#define JSON_KEY_OFFSET  "Offset"
#define JSON_KEY_ITN     "ITN"
#define JSON_KEY_LEXICAL "Lexical"
#define JSON_KEY_PRONUNCIATION_ASSESSMENT "PronunciationAssessment"

void CSpxUspRecoEngineAdapter::UpdateAdapterResult_JsonResult(shared_ptr<ISpxRecognitionResult> result)
{
    auto namedProperties = SpxQueryInterface<ISpxNamedProperties>(result);
    auto jsonResult = namedProperties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceResponse_JsonResult));
    if (jsonResult.empty())
    {
        return;
    }

    SPX_DBG_TRACE_VERBOSE("%s: before update: json='%s'", __FUNCTION__, jsonResult.c_str());
    auto root = json::parse(jsonResult);
    bool valueChanged = false;
    uint64_t offset = 0;
    auto iteratorOffset = root.find(JSON_KEY_OFFSET);
    if (iteratorOffset != root.end())
    {
        uint64_t oldOffset = iteratorOffset->get<uint64_t>();
        uint64_t newOffset = oldOffset + m_startingOffset;
        if (oldOffset != newOffset)
        {
            root[JSON_KEY_OFFSET] = newOffset;
            valueChanged = true;
        }
    }
    auto iteratorNBest = root.find(JSON_KEY_NBEST);
    if (iteratorNBest != root.end() && iteratorNBest->is_array())
    {
        size_t nBestSize = iteratorNBest->size();
        for (size_t index = 0; index < nBestSize; index++)
        {
            auto item = iteratorNBest->at(index);
            if (index == 0)
            {
                auto iteratorItn = item.find(JSON_KEY_ITN);
                if (iteratorItn != item.end())
                {
                    namedProperties->SetStringValue("ITN", iteratorItn->get<string>().c_str());
                }
                auto iteratorLexical = item.find(JSON_KEY_LEXICAL);
                if ( iteratorLexical != item.end())
                {
                    namedProperties->SetStringValue("Lexical", iteratorLexical->get<string>().c_str());
                }

                auto iteratorPron = item.find(JSON_KEY_PRONUNCIATION_ASSESSMENT);
                if (iteratorPron != item.end())
                {
                    for (auto& it : iteratorPron.value().items())
                    {
                        if (it.value().is_number())
                        {
                            namedProperties->SetStringValue(it.key().c_str(), it.value().dump().c_str());
                        }
                    }

                }
            }
            auto iteratorWords = item.find(JSON_KEY_WORDS);
            if (iteratorWords != item.end() && iteratorWords->is_array())
            {
                valueChanged = true;
                size_t wordListSize = iteratorWords->size();
                for (size_t wordIndex = 0; wordIndex < wordListSize; wordIndex++)
                {
                    auto word = iteratorWords->at(wordIndex);
                    auto iteratorWordOffset = word.find(JSON_KEY_OFFSET);
                    if (iteratorWordOffset != word.end())
                    {
                        offset = iteratorWordOffset->get<uint64_t>();
                        root[JSON_KEY_NBEST][index][JSON_KEY_WORDS][wordIndex][JSON_KEY_OFFSET] = offset + m_startingOffset;
                    }
                }
            }
        }
    }

    if (!namedProperties->HasStringValue("Lexical"))
    {
        auto iteratorLexical = root.find(JSON_KEY_LEXICAL);
        if (iteratorLexical != root.end())
        {
            namedProperties->SetStringValue("Lexical", iteratorLexical->get<string>().c_str());
        }
    }

    if (!namedProperties->HasStringValue("ITN"))
    {
        auto iteratorItn = root.find(JSON_KEY_ITN);
        if (iteratorItn != root.end())
        {
            namedProperties->SetStringValue("ITN", iteratorItn->get<string>().c_str());
        }
    }

    if (valueChanged)
    {
        string updatedJsonStr = root.dump();
        SPX_DBG_TRACE_VERBOSE("%s: after update: json='%s'", __FUNCTION__, updatedJsonStr.c_str());
        namedProperties->SetStringValue(GetPropertyName(PropertyId::SpeechServiceResponse_JsonResult), updatedJsonStr.c_str());
    }
}

#undef JSON_KEY_NBEST
#undef JSON_KEY_WORDS
#undef JSON_KEY_OFFSET
#undef JSON_KEY_ITN
#undef JSON_KEY_LEXICAL

void CSpxUspRecoEngineAdapter::OnSpeechKeywordDetected(const USP::SpeechKeywordDetectedMsg& message)
{
    SPX_DBG_TRACE_VERBOSE("Response: Speech.Keyword message. Status: %d, Text: %ls, starts at %" PRIu64 ", with duration %" PRIu64 " (100ns).\n", message.status, message.text.c_str(), message.offset + m_startingOffset, message.duration);

    if (IsBadState())
    {
        SPX_DBG_TRACE_VERBOSE("%s: (0x%8p) IGNORING... (audioState/uspState=%d/%d) %s", __FUNCTION__, (void*)this, m_audioState, m_uspState, IsState(UspState::Terminating) ? "(USP-TERMINATING)" : "********** USP-UNEXPECTED !!!!!!");
    }
    else if (message.status == USP::KeywordVerificationStatus::Accepted && IsState(UspState::WaitingForPhrase))
    {
        SPX_DBG_TRACE_VERBOSE("%s: site->FireAdapterResult_Intermediate()", __FUNCTION__);

        InvokeOnSite([&](const SitePtr& site)
        {
            auto factory = SpxQueryService<ISpxRecoResultFactory>(site);
            auto result = factory->CreateKeywordResult(1.0, message.offset, message.duration, message.text.c_str(), ResultReason::RecognizedKeyword, nullptr);
            result->SetOffset(message.offset + m_startingOffset);
            auto namedProperties = SpxQueryInterface<ISpxNamedProperties>(result);
            namedProperties->SetStringValue(GetPropertyName(PropertyId::SpeechServiceResponse_JsonResult), PAL::ToString(message.json).c_str());
            UpdateAdapterResult_JsonResult(result);
            site->FireAdapterResult_KeywordResult(this, message.offset + m_startingOffset, result, true);
        });
    }
    else if (message.status == USP::KeywordVerificationStatus::Rejected && !m_continueOnKeywordReject && TryChangeState(UspState::WaitingForPhrase, UspState::WaitingForTurnEnd))
    {
        SPX_DBG_TRACE_VERBOSE("%s: site->FireAdapterResult_Final()", __FUNCTION__);

        InvokeOnSite([&](const SitePtr& site)
        {
            auto factory = SpxQueryService<ISpxRecoResultFactory>(site);
            auto result = factory->CreateKeywordResult(1.0, message.offset, message.duration, message.text.c_str(), ResultReason::NoMatch, nullptr);
            result->SetOffset(message.offset + m_startingOffset);
            auto namedProperties = SpxQueryInterface<ISpxNamedProperties>(result);
            namedProperties->SetStringValue(GetPropertyName(PropertyId::SpeechServiceResponse_JsonResult), PAL::ToString(message.json).c_str());
            UpdateAdapterResult_JsonResult(result);
            site->FireAdapterResult_KeywordResult(this, message.offset + m_startingOffset, result, false);
        });

    }
    else
    {
        SPX_TRACE_ERROR("%s: (0x%8p) UNEXPECTED USP State transition ... (audioState/uspState=%d/%d)", __FUNCTION__, (void*)this, m_audioState, m_uspState);
    }
}

void CSpxUspRecoEngineAdapter::OnSpeechFragment(const USP::SpeechFragmentMsg& message)
{
    SPX_DBG_TRACE_VERBOSE("Response: Speech.Fragment message. Starts at offset %" PRIu64 ", with duration %" PRIu64 " (100ns). Text: %ls\n", message.offset + m_startingOffset, message.duration, message.text.c_str());

    bool sendIntermediate = false;

    if (IsBadState())
    {
        SPX_DBG_TRACE_VERBOSE("%s: (0x%8p) IGNORING... (audioState/uspState=%d/%d) %s", __FUNCTION__, (void*)this, m_audioState, m_uspState, IsState(UspState::Terminating) ? "(USP-TERMINATING)" : "********** USP-UNEXPECTED !!!!!!");
    }
    else if (TryChangeState(UspState::WaitingForIntent, UspState::WaitingForIntent2))
    {
        SPX_DBG_TRACE_VERBOSE("%s: Intent never came from service!!", __FUNCTION__);
        sendIntermediate = true;
        FireFinalResultLater_WaitingForIntentComplete();
        TryChangeState(UspState::WaitingForIntent2, UspState::WaitingForPhrase);
    }
    else if (IsState(UspState::WaitingForPhrase))
    {
        sendIntermediate = true;
    }
    else
    {
        SPX_TRACE_ERROR("%s: (0x%8p) UNEXPECTED USP State transition ... (audioState/uspState=%d/%d)", __FUNCTION__, (void*)this, m_audioState, m_uspState);
    }

    if (sendIntermediate)
    {
        SPX_DBG_TRACE_VERBOSE("%s: site->FireAdapterResult_Intermediate()", __FUNCTION__);

        InvokeOnSite([&](const SitePtr& site)
        {
            auto factory = SpxQueryService<ISpxRecoResultFactory>(site);
            auto result = factory->CreateIntermediateResult(message.text.c_str(), message.offset, message.duration);
            result->SetOffset(message.offset + m_startingOffset);

            auto namedProperties = SpxQueryInterface<ISpxNamedProperties>(result);
            namedProperties->SetStringValue(GetPropertyName(PropertyId::SpeechServiceResponse_JsonResult), PAL::ToString(message.json).c_str());
            if (!message.speaker.empty())
            {
                CreateConversationResult(result, message.speaker, message.id);
            }
            if (!message.language.empty())
            {
                namedProperties->SetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_AutoDetectSourceLanguageResult), message.language.c_str());
            }
            UpdateAdapterResult_JsonResult(result);
            site->FireAdapterResult_Intermediate(this, message.offset, result);
        });
    }
}

void CSpxUspRecoEngineAdapter::OnSpeechPhrase(const USP::SpeechPhraseMsg& message)
{
    SPX_DBG_TRACE_VERBOSE("Response: Speech.Phrase message. Status: %d, Text: %ls, starts at %" PRIu64 ", with duration %" PRIu64 " (100ns).\n", message.recognitionStatus, message.displayText.c_str(), message.offset + m_startingOffset, message.duration);
    SPX_DBG_TRACE_VERBOSE("%s: this=0x%8p", __FUNCTION__, (void*)this);

    if (IsBadState())
    {
        SPX_DBG_TRACE_VERBOSE("%s: (0x%8p) IGNORING... (audioState/uspState=%d/%d) %s", __FUNCTION__, (void*)this, m_audioState, m_uspState, IsState(UspState::Terminating) ? "(USP-TERMINATING)" : "********** USP-UNEXPECTED !!!!!!");
    }
    else if (m_expectIntentResponse &&
             message.recognitionStatus == RecognitionStatus::Success &&
             TryChangeState(UspState::WaitingForPhrase, UspState::WaitingForIntent))
    {
        if (message.recognitionStatus == RecognitionStatus::EndOfDictation)
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
    else if ((m_isInteractiveMode && TryChangeState(UspState::WaitingForPhrase, UspState::WaitingForTurnEnd)) ||
        (!m_isInteractiveMode && TryChangeState(UspState::WaitingForPhrase, UspState::WaitingForPhrase)))
    {
        if (message.recognitionStatus == RecognitionStatus::EndOfDictation)
        {
            InvokeOnSite([&](const SitePtr& site)
            {
                site->AdapterEndOfDictation(this, message.offset + m_startingOffset, message.duration);
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
        SPX_TRACE_ERROR("%s: (0x%8p) UNEXPECTED USP State transition ... (audioState/uspState=%d/%d)", __FUNCTION__, (void*)this, m_audioState, m_uspState);
    }
}

void CSpxUspRecoEngineAdapter::CreateConversationResult(std::shared_ptr<ISpxRecognitionResult>& result, const std::wstring& userId, const std::wstring& utteranceId)
{
    auto initConversationResult = SpxQueryInterface<ISpxConversationTranscriptionResultInit>(result);
    if (initConversationResult == nullptr)
    {
        ThrowInvalidArgumentException("Can't get conversation result");
    }
    initConversationResult->InitConversationResult(userId.c_str(), utteranceId.c_str());

}
static TranslationStatusCode GetTranslationStatus(USP::TranslationStatus uspStatus)
{
    TranslationStatusCode status = TranslationStatusCode::Error;
    switch (uspStatus)
    {
    case USP::TranslationStatus::Success:
        status = TranslationStatusCode::Success;
        break;
    case USP::TranslationStatus::Error:
        break;
    case USP::TranslationStatus::InvalidMessage:
        // The failureReason contains additional error messages.
        // Todo: have better error handling for different statuses.
        break;
    default:
        SPX_THROW_HR(SPXERR_RUNTIME_ERROR);
        break;
    }
    return status;
}

#ifdef _DEBUG
void CSpxUspRecoEngineAdapter::SetupAudioDumpFile()
{
    m_audioLogger.reset();

    auto properties = SpxQueryService<ISpxNamedProperties>(GetSite());
    auto audioDumpDir = properties->GetStringValue("CARBON-INTERNAL-DumpAudioToDir");
    SPX_DBG_TRACE_VERBOSE("CARBON-INTERNAL-DumpAudioToDir : %s", audioDumpDir.c_str());
    m_audioLogger = CSpxAudioFileLogger::Setup(audioDumpDir);
}
#endif

DataChunkPtr CSpxUspRecoEngineAdapter::MakeDataChunkForAudioFormat(SPXWAVEFORMATEX* pformat)
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
    auto wavHeaderDataPtr = std::make_shared<DataChunk>(buffer, cbHeader);
    wavHeaderDataPtr->isWavHeader = true;

    return wavHeaderDataPtr;
}


void CSpxUspRecoEngineAdapter::OnTranslationHypothesis(const USP::TranslationHypothesisMsg& message)
{
    SPX_DBG_TRACE_VERBOSE("Response: Translation.Hypothesis message. RecoText: %ls, TranslationStatus: %d, starts at %" PRIu64 ", with duration %" PRIu64 " (100ns).\n",
        message.text.c_str(), message.translation.translationStatus, message.offset + m_startingOffset, message.duration);
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
                auto result = factory->CreateIntermediateResult(message.text.c_str(), message.offset, message.duration);
                result->SetOffset(message.offset + m_startingOffset);

                auto namedProperties = SpxQueryInterface<ISpxNamedProperties>(result);
                namedProperties->SetStringValue(GetPropertyName(PropertyId::SpeechServiceResponse_JsonResult), PAL::ToString(message.json).c_str());
                if (!message.language.empty())
                {
                    namedProperties->SetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_AutoDetectSourceLanguageResult), message.language.c_str());
                }

                // Update our result to be a "TranslationText" result.
                auto initTranslationResult = SpxQueryInterface<ISpxTranslationRecognitionResultInit>(result);

                auto status = GetTranslationStatus(message.translation.translationStatus);
                initTranslationResult->InitTranslationRecognitionResult(status, message.translation.translations, message.translation.failureReason);

                // Fire the result
                UpdateAdapterResult_JsonResult(result);
                site->FireAdapterResult_Intermediate(this, message.offset + m_startingOffset, result);
            });
        }
    }
    else
    {
        SPX_TRACE_ERROR("%s: Unexpected USP State transition (audioState/uspState=%d/%d)", __FUNCTION__, m_audioState, m_uspState);
    }
}

void CSpxUspRecoEngineAdapter::OnTranslationPhrase(const USP::TranslationPhraseMsg& message)
{
    auto resultMap = message.translation.translations;

    SPX_DBG_TRACE_VERBOSE("Response: Translation.Phrase message. RecoStatus: %d, TranslationStatus: %d, RecoText: %ls, starts at %" PRIu64 ", with duration %" PRIu64 " (100ns).\n",
        message.recognitionStatus, message.translation.translationStatus,
        message.text.c_str(), message.offset + m_startingOffset, message.duration);
#ifdef _DEBUG
    if (message.translation.translationStatus != USP::TranslationStatus::Success)
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
    else if ((m_isInteractiveMode && TryChangeState(UspState::WaitingForPhrase, UspState::WaitingForTurnEnd)) ||
             (!m_isInteractiveMode && TryChangeState(UspState::WaitingForPhrase, UspState::WaitingForPhrase)))
    {
        SPX_DBG_TRACE_SCOPE("Fire final translation result: Creating Result", "FireFinalResul: GetSite()->FireAdapterResult_FinalResult()  complete!");
        if (message.recognitionStatus == RecognitionStatus::EndOfDictation)
        {
            InvokeOnSite([&](const SitePtr& site)
            {
                site->AdapterEndOfDictation(this, message.offset + m_startingOffset, message.duration);
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
                auto result = factory->CreateFinalResult(ToReason(message.recognitionStatus), ToNoMatchReason(message.recognitionStatus), message.text.c_str(), message.offset, message.duration);
                result->SetOffset(message.offset + m_startingOffset);

                auto namedProperties = SpxQueryInterface<ISpxNamedProperties>(result);
                namedProperties->SetStringValue(GetPropertyName(PropertyId::SpeechServiceResponse_JsonResult), PAL::ToString(message.json).c_str());
                if (!message.language.empty())
                {
                    namedProperties->SetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_AutoDetectSourceLanguageResult), message.language.c_str());
                }

                // Update our result to be an "TranslationText" result.
                auto initTranslationResult = SpxQueryInterface<ISpxTranslationRecognitionResultInit>(result);

                auto status = GetTranslationStatus(message.translation.translationStatus);
                initTranslationResult->InitTranslationRecognitionResult(status, message.translation.translations, message.translation.failureReason);

                UpdateAdapterResult_JsonResult(result);

                // Fire the result
                site->FireAdapterResult_FinalResult(this, message.offset + m_startingOffset, result);
            });
        }
    }
    else
    {
        SPX_TRACE_ERROR("%s: Unexpected USP State transition (audioState/uspState=%d/%d)", __FUNCTION__, m_audioState, m_uspState);
    }
}

void CSpxUspRecoEngineAdapter::OnAudioOutputChunk(const USP::AudioOutputChunkMsg& message)
{
    SPX_DBG_TRACE_VERBOSE("Response: Audio output chunk message. Audio data size: %zu\n", message.audioLength);

    if (m_endpointType == USP::EndpointType::Dialog)
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
        auto result = factory->CreateFinalResult(ResultReason::SynthesizingAudio, NO_MATCH_REASON_NONE, L"", 0, 0);

        // Update our result to be an "TranslationSynthesis" result.
        auto initTranslationResult = SpxQueryInterface<ISpxTranslationSynthesisResultInit>(result);
        initTranslationResult->InitTranslationSynthesisResult(message.audioBuffer, message.audioLength, message.requestId);

        site->FireAdapterResult_TranslationSynthesis(this, result);
    });
}

void CSpxUspRecoEngineAdapter::OnTurnStart(const USP::TurnStartMsg& message)
{
    SPX_DBG_TRACE_VERBOSE("Response: Turn.Start message. Context.ServiceTag: %s\n", message.contextServiceTag.c_str());
    SPX_DBG_TRACE_VERBOSE("%s: this=0x%8p", __FUNCTION__, (void*)this);

    if (IsBadState())
    {
        SPX_DBG_TRACE_VERBOSE("%s: (0x%8p) IGNORING... (audioState/uspState=%d/%d) %s", __FUNCTION__, (void*)this, m_audioState, m_uspState, IsState(UspState::Terminating) ? "(USP-TERMINATING)" : "********** USP-UNEXPECTED !!!!!!");
    }
    else if (TryChangeState(UspState::WaitingForTurnStart, UspState::WaitingForPhrase))
    {
        SPX_DBG_TRACE_VERBOSE("%s: site->AdapterStartedTurn()", __FUNCTION__);
        SPX_DBG_ASSERT(GetSite());

        auto properties = SpxQueryService<ISpxNamedProperties>(GetSite());
        SPX_IFTRUE_THROW_HR(properties == nullptr, SPXERR_UNEXPECTED_USP_SITE_FAILURE);
        if (!message.serviceManagesOffset && properties->HasStringValue("SPEECH-UspContinuationOffset"))
        {
            m_startingOffset = stoull(properties->GetStringValue("SPEECH-UspContinuationOffset"));
        }

        InvokeOnSite([this, &message](const SitePtr& p) { p->AdapterStartedTurn(this, message.contextServiceTag); });

       properties->SetStringValue("SPEECH-UspContinuationServiceTag", message.contextServiceTag.c_str());
    }
    else
    {
        SPX_TRACE_ERROR("%s: (0x%8p) UNEXPECTED USP State transition ... (audioState/uspState=%d/%d)", __FUNCTION__, (void*)this, m_audioState, m_uspState);
    }

#ifdef _DEBUG
    if (m_audioLogger)
        m_audioLogger->SetDumpInstanceId(message.contextServiceTag);
#endif
}

void CSpxUspRecoEngineAdapter::OnTurnEnd(const USP::TurnEndMsg& message)
{
    SPX_DBG_TRACE_SCOPE("CSpxUspRecoEngineAdapter::OnTurnEnd ... started... ", "CSpxUspRecoEngineAdapter::OnTurnEnd ... DONE!");
    SPX_DBG_TRACE_VERBOSE("Response: Turn.End message.\n");
    UNUSED(message);

    auto adapterTurnStopped = false;

    auto prepareReady =  !m_singleShot &&
        (TryChangeState(AudioState::Sending, AudioState::Ready) ||
         TryChangeState(AudioState::Mute, AudioState::Ready));

    auto requestMute = m_singleShot && TryChangeState(AudioState::Sending, AudioState::Mute);

    if (IsBadState())
    {
        SPX_DBG_TRACE_VERBOSE("%s: (0x%8p) IGNORING... (audioState/uspState=%d/%d) %s", __FUNCTION__, (void*)this, m_audioState, m_uspState, IsState(UspState::Terminating) ? "(USP-TERMINATING)" : "********** USP-UNEXPECTED !!!!!!");
    }
    else if (( m_isInteractiveMode && TryChangeState(UspState::WaitingForTurnEnd, UspState::Idle)) ||
             (!m_isInteractiveMode && TryChangeState(UspState::WaitingForPhrase, UspState::Idle)))
    {
        adapterTurnStopped = true;
    }
    else if (TryChangeState(UspState::WaitingForIntent, UspState::WaitingForIntent2))
    {
        SPX_DBG_TRACE_VERBOSE("%s: Intent never came from service!!", __FUNCTION__);
        adapterTurnStopped = true;
        FireFinalResultLater_WaitingForIntentComplete();
        TryChangeState(UspState::WaitingForIntent2, UspState::Idle);
    }
    else
    {
        SPX_TRACE_ERROR("%s: (0x%8p) UNEXPECTED USP State transition ... (audioState/uspState=%d/%d)", __FUNCTION__, (void*)this, m_audioState, m_uspState);
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
        SPX_DBG_TRACE_VERBOSE("%s: FlushAudio()  USP-FLUSH", __FUNCTION__);
        FlushAudio();

        SPX_DBG_TRACE_VERBOSE("%s: site->AdapterRequestingAudioMute(true) ... (audioState/uspState=%d/%d)", __FUNCTION__, m_audioState, m_uspState);
        site->AdapterRequestingAudioMute(this, true);
    }

    // Disposing of the codec.
    m_compressionCodec.reset();

#ifdef _DEBUG
    m_audioLogger.reset();
#endif
}


void CSpxUspRecoEngineAdapter::OnMessageStart(const USP::TurnStartMsg& message)
{
    /* For now only handle service generated messages for dialog endpoints */
    if (m_endpointType == USP::EndpointType::Dialog)
    {
        std::weak_ptr<CSpxUspRecoEngineAdapter> wk{ std::dynamic_pointer_cast<CSpxUspRecoEngineAdapter>(ISpxInterfaceBaseFor<ISpxRecoEngineAdapter>::shared_from_this()) };
        // start the initial state as SessionStart
        m_request_session_map.emplace(message.requestId, std::make_unique<CSpxActivitySession>(wk));
    }
}

void CSpxUspRecoEngineAdapter::OnMessageEnd(const USP::TurnEndMsg& message)
{
    /* For now only handle service generated messages for dialog endpoints */
    if (m_endpointType == USP::EndpointType::Dialog)
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

void CSpxUspRecoEngineAdapter::OnError(const std::shared_ptr<ISpxErrorInformation>& error)
{
    SPX_TRACE_ERROR("Response: On Error: Code:%d, Message: %s.\n", (int)error->GetCancellationCode(), error->GetDetails().c_str());

    /* If we receive an error, clear all ongoing activity sessions. */
    if (m_endpointType == USP::EndpointType::Dialog)
    {
        m_request_session_map.clear();
    }

    if (IsBadState())
    {
        SPX_TRACE_ERROR("%s: (0x%8p) IGNORING... (audioState/uspState=%d/%d) %s", __FUNCTION__, (void*)this, m_audioState, m_uspState, IsState(UspState::Terminating) ? "(USP-TERMINATING)" : "********** USP-UNEXPECTED !!!!!!");
    }
    else if (TryChangeState(UspState::Error))
    {
        SPX_TRACE_ERROR("%s: site->Error() ... error='%s'", __FUNCTION__, error->GetDetails().c_str());
        InvokeOnSite([this, error](const SitePtr& p) { p->Error(this, error); });
    }
    else
    {
       SPX_TRACE_ERROR("%s: (0x%8p) UNEXPECTED USP State transition ... (audioState/uspState=%d/%d)", __FUNCTION__, (void*)this, m_audioState, m_uspState);
    }
}

void CSpxUspRecoEngineAdapter::OnUserMessage(const USP::UserMsg& msg)
{
    SPX_DBG_TRACE_VERBOSE("Response: Usp User Message: %s, content-type=%s", msg.path.c_str(), msg.contentType.c_str());

    if (msg.path == "response")
    {
        if (TryChangeState(UspState::WaitingForIntent, UspState::WaitingForIntent2))
        {
            std::string luisJson((const char*)msg.buffer, msg.size);
            SPX_DBG_TRACE_VERBOSE("USP User Message: response; luisJson='%s'", luisJson.c_str());
            FireFinalResultLater_WaitingForIntentComplete(luisJson);
            TryChangeState(UspState::WaitingForIntent2, m_isInteractiveMode ? UspState::WaitingForTurnEnd : UspState::WaitingForPhrase);
        }
        else if (m_endpointType == USP::EndpointType::Dialog)
        {
            std::string message{ reinterpret_cast<const char*>(msg.buffer), msg.size };
            SPX_DBG_TRACE_VERBOSE("USP User Message: response; message='%s'", message.c_str());
            auto responseMessage = json::parse(message);
            if (!responseMessage["conversationId"].is_null())
            {
                /* Update conversation id */
                m_dialogConversationId = responseMessage["conversationId"].get<std::string>();
                InvokeOnServiceIfAvailable<ISpxNamedProperties>(GetSite(), [&](ISpxNamedProperties& properties)
                {
                    properties.SetStringValue(GetPropertyName(PropertyId::Conversation_Conversation_Id), m_dialogConversationId.c_str());
                });
            }
            auto it = m_request_session_map.find(msg.requestId);
            if (it != m_request_session_map.end())
            {
                auto& machine = it->second;
                machine->Switch(CSpxActivitySession::State::ActivityReceived, &message, nullptr);
                return;
            }
            SPX_TRACE_ERROR("Unexpected message; request_id='%s'", msg.requestId.c_str());
        }
        else
        {
            SPX_TRACE_ERROR("%s: (0x%8p) UNEXPECTED USP State transition ... (audioState/uspState=%d/%d)", __FUNCTION__, (void*)this, m_audioState, m_uspState);
        }
    }
}

void CSpxUspRecoEngineAdapter::OnConnected()
{
    InvokeOnSite([](const SitePtr& p) { p->AdapterConnected(); });
}

void CSpxUspRecoEngineAdapter::OnDisconnected(const std::shared_ptr<ISpxErrorInformation>& payload)
{
    InvokeOnSite([&payload](const SitePtr& p) { p->AdapterDisconnected(payload); });
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

json CSpxUspRecoEngineAdapter::GetDgiJsonFromListenForList(std::list<std::string>& listenForList)
{
    SPX_DBG_ASSERT(GetSite() != nullptr);
    auto properties = SpxQueryService<ISpxNamedProperties>(GetSite());
    SPX_IFTRUE_THROW_HR(properties == nullptr, SPXERR_UNEXPECTED_USP_SITE_FAILURE);
    auto noDGI = PAL::ToBool(properties->GetStringValue("CARBON-INTERNAL-USP-NoDGI"));
    json dgiJson;

    if (noDGI) {
        return dgiJson;
    }


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

    if (genericItems.size() > 0)
    {
        json group;
        group["Type"] = "Generic";
        auto grammerArray = json::array();
        for (auto item : genericItems)
        {
            grammerArray.push_back({ {"Text", item} });
        }
        group["Items"] = grammerArray;
        auto groupsArray = json::array();
        groupsArray.push_back(std::move(group));
        dgiJson["Groups"] = groupsArray;

        // Check and see if a word level factor has been set.
        if (properties->HasStringValue("SPEECH-WordLevelRecognitionFactor"))
        {
            auto recognitionFactor = properties->GetStringValue("SPEECH-WordLevelRecognitionFactor");
            auto val = std::stod(recognitionFactor);
            SPX_IFTRUE_THROW_HR(0 > val, SPXERR_INVALID_ARG);

            dgiJson["bias"] = recognitionFactor;
        }
    }

    if (grammars.size() > 0)
    {
        dgiJson["ReferenceGrammars"] = json(grammars);
    }

    return dgiJson;
}

void CSpxUspRecoEngineAdapter::GetIntentInfoFromSite(std::string& provider, std::string& id, std::string& key, std::string& region)
{
    SPX_DBG_ASSERT(GetSite() != nullptr);
    GetSite()->GetIntentInfo(provider, id, key, region);
}

json CSpxUspRecoEngineAdapter::GetLanguageUnderstandingJsonFromIntentInfo(const std::string& provider, const std::string& id, const std::string& key, const std::string& region)
{
    SPX_DBG_ASSERT(GetSite() != nullptr);
    auto properties = SpxQueryService<ISpxNamedProperties>(GetSite());
    SPX_IFTRUE_THROW_HR(properties == nullptr, SPXERR_UNEXPECTED_USP_SITE_FAILURE);
    auto noIntentJson = PAL::ToBool(properties->GetStringValue("CARBON-INTERNAL-USP-NoIntentJson"));

    UNUSED(region);

    json intentJson;
    if (!noIntentJson && !provider.empty() && !id.empty())
    {
        intentJson["provider"] = provider;
        intentJson["id"] = id;
        intentJson["key"] = key;
    }

    return intentJson;
}

// Construct and return the detected keywords in json format based on USP.
// If no keyword id detected or keyword verification is turned off, return an empty string.
json CSpxUspRecoEngineAdapter::GetKeywordDetectionJson()
{
    auto site = GetSite();
    auto properties = SpxQueryService<ISpxNamedProperties>(site);
    SPX_IFTRUE_THROW_HR(properties == nullptr, SPXERR_UNEXPECTED_USP_SITE_FAILURE);
    auto keywordVerificationEnabled = PAL::ToBool(properties->GetStringValue(KeywordConfig_EnableKeywordVerification, PAL::BoolToString(false)));
    json keywordDetectionJson;
    if (keywordVerificationEnabled)
    {
        auto spottedKeywordResult = site->GetSpottedKeywordResult();
        if (spottedKeywordResult != nullptr) {
            keywordDetectionJson = { {
                {"type", "startTrigger"},
                {"clientDetectedKeywords", {{
                    {"text", PAL::ToString(spottedKeywordResult->GetText())},
                    {"confidence", SpxQueryInterface<ISpxKeywordRecognitionResult>(spottedKeywordResult)->GetConfidence()},
                    {"startOffset", spottedKeywordResult->GetOffset()},
                    {"duration", spottedKeywordResult->GetDuration()}
                }}},
                {"onReject", {
                    {"action", "EndOfTurn"}
                }}
            } };
        }
    }
    else if (properties->HasStringValue(KEYWORDS_PROPERTY_NAME))
    {
        auto keywords = properties->GetStringValue(KEYWORDS_PROPERTY_NAME);
        if (keywords != "")
        {
            json keywordsJSON;

            auto keywordsVector = PAL::split(keywords, ';');

            for(auto oneKeyword: keywordsVector)
            {
                json oneKeywordJSON;
                oneKeywordJSON = { { "text", oneKeyword } };

                keywordsJSON.push_back(oneKeywordJSON);
            }

            auto cancelOnMissing = PAL::ToBool(properties->GetStringValue("SPEECH-CancelOnKeywordMissing", "true"));
            auto rejectAction = cancelOnMissing ? "EndOfTurn" : "Continue";
            m_continueOnKeywordReject = !cancelOnMissing;

            keywordDetectionJson = { {
                {"type", "startTrigger"},
                {"clientDetectedKeywords",
                        keywordsJSON
                },
                {"onReject", {
                    {"action", rejectAction}
                }}
            } };
        }
    }

    return keywordDetectionJson;
}

json CSpxUspRecoEngineAdapter::GetSpeechContextJson()
{
    json contextJson;

    // Get the Dgi json payload
    list<string> listenForList = GetListenForListFromSite();
    auto dgiJson = GetDgiJsonFromListenForList(listenForList);
    if (!dgiJson.empty())
    {
        contextJson["dgi"] = dgiJson;
    }

    // Get the intent payload
    string provider, id, key, region;
    GetIntentInfoFromSite(provider, id, key, region);
    auto intentJson = GetLanguageUnderstandingJsonFromIntentInfo(provider, id, key, region);
    m_expectIntentResponse = !intentJson.empty();
    if (m_expectIntentResponse)
    {
        contextJson["intent"] = intentJson;
    }

    // Get keyword detection json payload
    auto keywordDetectionJson = GetKeywordDetectionJson();
    if (!keywordDetectionJson.empty())
    {
        contextJson["invocationSource"] = "VoiceActivationWithKeyword";
        contextJson["keywordDetection"] = keywordDetectionJson;

        auto site = GetSite();
        auto properties = SpxQueryService<ISpxNamedProperties>(site);
        SPX_IFTRUE_THROW_HR(properties == nullptr, SPXERR_UNEXPECTED_USP_SITE_FAILURE);
        auto removeKeyword = PAL::ToBool(properties->GetStringValue("SPEECH-RemoveKeyword", PAL::BoolToString(false)));
        if (removeKeyword)
        {
            contextJson["phraseEnrichment"]["stripStartTriggerKeyword"] = true;
        }

    }

    auto leftAndRight = GetLeftRightContext();
    auto insertionPointLeft = leftAndRight.first;
    auto insertionPointRight = leftAndRight.second;

    if (!insertionPointLeft.empty())
    {
        contextJson["dictation"]["insertionPoint"]["left"] = json(insertionPointLeft);
    }

    if (!insertionPointRight.empty())
    {
        contextJson["dictation"]["insertionPoint"]["right"] = json(insertionPointRight);
    }

    auto properties = SpxQueryService<ISpxNamedProperties>(GetSite());
    SPX_IFTRUE_THROW_HR(properties == nullptr, SPXERR_UNEXPECTED_USP_SITE_FAILURE);
    auto toLanguages = GetLanguages(PropertyId::SpeechServiceConnection_TranslationToLanguages);
    if (toLanguages.size() > 0 )
    {
        contextJson["translationcontext"]["to"] = json(toLanguages);
    }

    // Language id feature need below speech context
    // Currently only 1P new translation endpoint and 3P speech endpoint support it
    // 1P users use this through FromEndpointAPI by hitting a specific endpoint
    // 3P users use this from SpeechRecognizer FromConfig API
    bool isLanguageIdSupported = m_endpointType == USP::EndpointType::Translation || m_endpointType == USP::EndpointType::Speech;
    if (isLanguageIdSupported
        && properties->HasStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_AutoDetectSourceLanguages)))
    {
        auto sourceLanguages = GetLanguages(PropertyId::SpeechServiceConnection_AutoDetectSourceLanguages);
        bool languageDetectionOnly = properties->GetStringValue("Auto-Detect-Source-Language-Only") == TrueString;
        std::unordered_map<string, string> languageToEndpointIdMap;
        if (sourceLanguages.size() > 0)
        {
            languageToEndpointIdMap = GetPerLanguageSetting(sourceLanguages, PropertyId::SpeechServiceConnection_EndpointId);
            auto onSuccessAction = languageDetectionOnly ? "None" : "Recognize";
            contextJson["languageId"] = GetLanguageIdJson(move(sourceLanguages), onSuccessAction);
        }
        if (!languageDetectionOnly) {
            contextJson["phraseDetection"] = GetPhraseDetectionJson(m_endpointType == USP::EndpointType::Translation, move(languageToEndpointIdMap));
            contextJson["phraseOutput"] = GetPhraseOutputJson(m_endpointType == USP::EndpointType::Speech);
            if (m_endpointType == USP::EndpointType::Translation)
            {
                auto voiceNameMap = GetPerLanguageSetting(toLanguages, PropertyId::SpeechServiceConnection_TranslationVoice);
                bool doSynthesis = !voiceNameMap.empty();
                contextJson["translation"] = GetTranslationJson(move(toLanguages), doSynthesis);
                if (doSynthesis)
                {
                    contextJson["synthesis"] = GetSynthesisJson(move(voiceNameMap));
                }
            }
        }

        SPX_DBG_TRACE_VERBOSE("Speech context with LID scenario %s.", contextJson.dump().c_str());
    }

    // Setup the audio stream and continuation token.
    contextJson["audio"]["streams"]["1"] = {};

    if (properties->HasStringValue("SPEECH-UspContinuationOffset"))
    {
        contextJson["continuation"]["audio"]["streams"]["1"]["offset"] = properties->GetStringValue("SPEECH-UspContinuationOffset");
    }

    if (properties->HasStringValue("SPEECH-UspContinuationToken"))
    {
        contextJson["continuation"]["token"] = properties->GetStringValue("SPEECH-UspContinuationToken");
    }

    if (properties->HasStringValue("SPEECH-UspContinuationServiceTag"))
    {
        contextJson["continuation"]["previousServiceTag"] = properties->GetStringValue("SPEECH-UspContinuationServiceTag");
    }

    auto userParams = GetParametersFromUser("speech.context");
    for (const auto& it : userParams)
    {
        const auto& name = it.first;
        const auto& value = it.second;

        if (value.size() < 1 || name.empty())
        {
            ThrowInvalidArgumentException("User provided an empty name or value in a speech.context field");
        }
        contextJson[name] = json::parse(value);
        SPX_DBG_TRACE_VERBOSE("Set '%s' as '%s' in speech.context", name.c_str(), value.c_str());
    }

    auto pronunciationAssessmentParams = properties->GetStringValue(GetPropertyName(PropertyId::PronunciationAssessment_Params));
    if (!pronunciationAssessmentParams.empty() && m_endpointType == USP::EndpointType::Speech)
    {
        // pronunciation assessment requires detailed format and word timings.
        // TODO: for now, format and word timings are needed to set in speech.context, which is different from current SDK
        // implementation (in connection url). Need to update the logic after service update.
        contextJson["phraseDetection"]["enrichment"]["pronunciationAssessment"] = json::parse(pronunciationAssessmentParams);
        // set detailed format
        contextJson["phraseOutput"]["format"] = "Detailed";
        // enable word timings
        if (!properties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceResponse_RequestWordLevelTimestamps)).empty() ||
            properties->GetStringValue(GetPropertyName(PropertyId::PronunciationAssessment_Granularity)) !=
            PronunciationAssessment::pronunciationAssessmentGranularityToString.at(PronunciationAssessmentGranularity::FullText))
        {
            auto options = contextJson["phraseOutput"]["detailed"]["options"];
            if (std::find(options.begin(), options.end(), "WordTimings") == options.end())
            {
                contextJson["phraseOutput"]["detailed"]["options"].push_back("WordTimings");
            }
        }

        contextJson["phraseOutput"]["detailed"]["options"].push_back("PronunciationAssessment");
    }

    return contextJson;
}

ResultReason CSpxUspRecoEngineAdapter::ToReason(RecognitionStatus uspRecognitionStatus)
{
    switch (uspRecognitionStatus)
    {
    case RecognitionStatus::Success:
        return ResultReason::RecognizedSpeech;

    case RecognitionStatus::NoMatch:
    case RecognitionStatus::InitialSilenceTimeout:
    case RecognitionStatus::InitialBabbleTimeout:
        return ResultReason::NoMatch;

    case RecognitionStatus::Error:
    case RecognitionStatus::TooManyRequests:
    case RecognitionStatus::BadRequest:
    case RecognitionStatus::Forbidden:
    case RecognitionStatus::ServiceUnavailable:
    case RecognitionStatus::InvalidMessage:
        return ResultReason::Canceled;

    default:
        SPX_TRACE_ERROR("Unexpected recognition status %d when converting to ResultReason.", uspRecognitionStatus);
        SPX_THROW_HR(SPXERR_RUNTIME_ERROR);
        return ResultReason::Canceled;
    }
}

CancellationReason CSpxUspRecoEngineAdapter::ToCancellationReason(RecognitionStatus uspRecognitionStatus)
{
    switch (uspRecognitionStatus)
    {
    case RecognitionStatus::Success:
    case RecognitionStatus::NoMatch:
    case RecognitionStatus::InitialSilenceTimeout:
    case RecognitionStatus::InitialBabbleTimeout:
        return REASON_CANCELED_NONE;

    case RecognitionStatus::Error:
    case RecognitionStatus::TooManyRequests:
    case RecognitionStatus::BadRequest:
    case RecognitionStatus::Forbidden:
    case RecognitionStatus::ServiceUnavailable:
    case RecognitionStatus::InvalidMessage:
        return CancellationReason::Error;

    default:
        SPX_TRACE_ERROR("Unexpected recognition status %d when converting to CancellationReason.", uspRecognitionStatus);
        SPX_THROW_HR(SPXERR_RUNTIME_ERROR);
        return CancellationReason::Error;
    }
}

NoMatchReason CSpxUspRecoEngineAdapter::ToNoMatchReason(RecognitionStatus uspRecognitionStatus)
{
    switch (uspRecognitionStatus)
    {
    case RecognitionStatus::Success:
    case RecognitionStatus::Error:
    case RecognitionStatus::TooManyRequests:
    case RecognitionStatus::BadRequest:
    case RecognitionStatus::Forbidden:
    case RecognitionStatus::ServiceUnavailable:
    case RecognitionStatus::InvalidMessage:
        return NO_MATCH_REASON_NONE;

    case RecognitionStatus::NoMatch:
        return NoMatchReason::NotRecognized;

    case RecognitionStatus::InitialSilenceTimeout:
        return NoMatchReason::InitialSilenceTimeout;

    case RecognitionStatus::InitialBabbleTimeout:
        return NoMatchReason::InitialBabbleTimeout;

    default:
        SPX_TRACE_ERROR("Unexpected recognition status %d when converting to NoMatchReason.", uspRecognitionStatus);
        SPX_THROW_HR(SPXERR_RUNTIME_ERROR);
        return NO_MATCH_REASON_NONE;
    }
}

void CSpxUspRecoEngineAdapter::FireActivityResult(std::string activity, std::shared_ptr<ISpxAudioOutput> audio)
{
    SPX_DBG_TRACE_SCOPE("FireActivityAndAudioResult: Creating Result", "FireActivityAndAudioResult: GetSite()->FireAdapterResult_ActivityAudioReceived()  complete!");

    InvokeOnSite([this, activity{std::move(activity)}, audio](const SitePtr& site)
    {
        site->FireAdapterResult_ActivityReceived(this, std::move(activity), audio);
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

        auto result = factory->CreateFinalResult(ToReason(message.recognitionStatus), ToNoMatchReason(message.recognitionStatus), message.displayText.c_str(), message.offset, message.duration);
        result->SetOffset(message.offset + m_startingOffset);

        auto namedProperties = SpxQueryInterface<ISpxNamedProperties>(result);
        namedProperties->SetStringValue(GetPropertyName(PropertyId::SpeechServiceResponse_JsonResult), PAL::ToString(message.json).c_str());

        // Do we already have the LUIS json payload from the service (1-hop)
        if (!luisJson.empty())
        {
            namedProperties->SetStringValue(GetPropertyName(PropertyId::LanguageUnderstandingServiceResponse_JsonResult), luisJson.c_str());
        }

        if (!message.speaker.empty())
        {
            CreateConversationResult(result, message.speaker, message.id);
        }

        if (!message.language.empty())
        {
            namedProperties->SetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_AutoDetectSourceLanguageResult), message.language.c_str());
        }

        UpdateAdapterResult_JsonResult(result);
        site->FireAdapterResult_FinalResult(this, message.offset + m_startingOffset, result);
    });
}

void CSpxUspRecoEngineAdapter::FireFinalResultLater_WaitingForIntentComplete(const std::string& luisJson)
{
    SPX_DBG_ASSERT(m_expectIntentResponse);
    FireFinalResultNow(m_finalResultMessageToFireLater, luisJson);
    m_finalResultMessageToFireLater = USP::SpeechPhraseMsg();
}

bool CSpxUspRecoEngineAdapter::TryChangeState(AudioState fromAudioState, UspState fromUspState, AudioState toAudioState, UspState toUspState)
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

SPXHR CSpxUspRecoEngineAdapter::PrepareCompressionCodec(
                                    const SPXWAVEFORMATEX* format,
                                    ISpxInternalAudioCodecAdapter::SPXCompressedDataCallback dataCallback)
{
    m_compressionCodec.reset();

    SPX_DBG_TRACE_VERBOSE("%s: Prepare compression codec.", __FUNCTION__);

    auto properties = SpxQueryService<ISpxNamedProperties>(GetSite());
    SPX_IFTRUE_THROW_HR(properties == nullptr, SPXERR_UNEXPECTED_USP_SITE_FAILURE);

    std::string codecModule = properties->GetStringValue("SPEECH-Compression-Codec-Module");
    if (codecModule.length() == 0) return SPXERR_NOT_FOUND; // don't spew into log... not fatal...

    // it should come as empty or audio/silk for now
    std::string encodingFormat = properties->GetStringValue("SPEECH-Compression-EncodingFormat");

    auto codecAdapter = SpxCreateObjectWithSite<ISpxInternalAudioCodecAdapter>("CSpxInternalAudioCodecAdapter", this);
    SPX_RETURN_ON_FAIL(codecAdapter->Load(codecModule, encodingFormat, dataCallback));

    codecAdapter->InitCodec(format);
    m_compressionCodec = codecAdapter;

    return SPX_NOERROR;
}

void CSpxUspRecoEngineAdapter::HandleCompressedAudioData(const uint8_t* outData, size_t nBytesOut)
{
    // Handle null / 0 bytes of data on ending the stream
    if (outData == nullptr || nBytesOut == 0)
    {
        // NOOP
        return;
    }

    // Gets the compressed audio data
    auto data = SpxAllocSharedAudioBuffer(nBytesOut);
    memcpy(data.get(), outData, nBytesOut);

    // BUGBUG: Track audio timestamps as well
    auto dataChunkPtr = std::make_shared<DataChunk>(data, (uint32_t)nBytesOut);

    if (!m_audioFormatSent)
    {
        dataChunkPtr->contentType = m_compressionCodec->GetContentType();
        m_audioFormatSent = true;
    }

    UspWriteActual(dataChunkPtr);
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

    auto codecInitResult = PrepareCompressionCodec(format,
        [this](const uint8_t* outData, size_t nBytesOut)
        {
            HandleCompressedAudioData(outData, nBytesOut);
        });

    SPX_DBG_TRACE_VERBOSE_IF(SPX_FAILED(codecInitResult), "%s: (0x%8p)->PrepareCompressionCodec() result: %8lx. Sending the audio uncompressed", __FUNCTION__, (void*)this, (unsigned long)codecInitResult);

    PrepareAudioReadyState();
}

void CSpxUspRecoEngineAdapter::PrepareAudioReadyState()
{
    if (!IsState(AudioState::Ready, UspState::Idle))
    {
        SPX_TRACE_ERROR("wrong state in PrepareAudioReadyState current audio state %d, usp state %d", m_audioState, m_uspState);
    }

    EnsureUspInit();
}

void CSpxUspRecoEngineAdapter::SendPreAudioMessages()
{
    SPX_DBG_ASSERT(IsState(AudioState::Sending));

    UspSendSpeechContext();
    UspSendSpeechAgentContext();
    UspSendSpeechEvent();
}

void CSpxUspRecoEngineAdapter::PrepareUspAudioStream()
{
    SendPreAudioMessages();
    m_audioFormatSent = false;

#ifdef _DEBUG
    SetupAudioDumpFile();
#endif

    ProcessAudioFormat(m_format.get());
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
    SPX_DBG_TRACE_VERBOSE("%s: this=0x%8p ... USP-RESET", __FUNCTION__, (void*)this);

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
    SPX_DBG_TRACE_VERBOSE("%s: this=0x%8p ... USP-RESET", __FUNCTION__, (void*)this);

    // Let's terminate our current usp connection and sever our callbacks
    UspTerminate();
}

std::pair<std::string, std::string> CSpxUspRecoEngineAdapter::GetLeftRightContext()
{
    // Get dictation left and right context of the insertion point.
    auto properties = SpxQueryService<ISpxNamedProperties>(GetSite());
    SPX_IFTRUE_THROW_HR(properties == nullptr, SPXERR_UNEXPECTED_USP_SITE_FAILURE);

    auto leftContext = properties->GetStringValue("DictationInsertionPointLeft");
    auto rightContext = properties->GetStringValue("DictationInsertionPointRight");

    return {leftContext, rightContext};
}

json CSpxUspRecoEngineAdapter::GetLanguageIdJson(std::vector<std::string> sourceLanguages, string onSuccessAction)
{
    json langDetectionJson;
    langDetectionJson["languages"] = json(move(sourceLanguages));
    langDetectionJson["onSuccess"]["action"] = onSuccessAction.c_str();
    langDetectionJson["onUnknown"]["action"] = "None";
    return langDetectionJson;
}

json CSpxUspRecoEngineAdapter::GetPhraseDetectionJson(bool doTranslation, unordered_map<string, string> languageToEndpointIdMap)
{
    json phraseDetectionJson;
    auto action = doTranslation ? "Translate" : "None";
    phraseDetectionJson["onSuccess"]["action"] = action;
    phraseDetectionJson["onInterim"]["action"] = action;
    if (!languageToEndpointIdMap.empty())
    {
        json customModels = json::array();
        for (const auto& pair: languageToEndpointIdMap)
        {
            json perLanguageEndpointId;
            perLanguageEndpointId["language"] = pair.first;
            perLanguageEndpointId["endpoint"] = pair.second;
            customModels.push_back(move(perLanguageEndpointId));
        }
        phraseDetectionJson["customModels"] = std::move(customModels);
    }

    return phraseDetectionJson;
}

json CSpxUspRecoEngineAdapter::GetPhraseOutputJson(bool needSpeechMessages)
{
    json phraseOutputJson;
    phraseOutputJson["interimResults"]["resultType"] = needSpeechMessages ? "Auto" : "None";
    phraseOutputJson["phraseResults"]["resultType"] = needSpeechMessages ? "Always" : "None";
    return phraseOutputJson;
}

json CSpxUspRecoEngineAdapter::GetTranslationJson(vector<string>&& targetLangs, bool doSynthesis)
{
    json translationJson;
    translationJson["targetLanguages"] = json(move(targetLangs));
    translationJson["output"]["interimResults"]["mode"] = "Always";
    translationJson["onSuccess"]["action"] = doSynthesis ? "Synthesize" : "None";
    return translationJson;
}

json CSpxUspRecoEngineAdapter::GetSynthesisJson(unordered_map<string, string>&& voiceNameMap)
{
    json synthesisJson;
    synthesisJson["defaultVoices"] = json(move(voiceNameMap));
    return synthesisJson;
}

vector<string> CSpxUspRecoEngineAdapter::GetLanguages(PropertyId propertyId)
{
    auto properties = SpxQueryService<ISpxNamedProperties>(GetSite());
    SPX_IFTRUE_THROW_HR(properties == nullptr, SPXERR_UNEXPECTED_USP_SITE_FAILURE);
    string languages = properties->GetStringValue(GetPropertyName(propertyId));
    if (!languages.empty())
    {
        return PAL::split(languages, CommaDelim);
    }
    return {};
}

unordered_map<string, string> CSpxUspRecoEngineAdapter::GetPerLanguageSetting(const vector<string>& languages, PropertyId propertyId)
{
    unordered_map<string, string> languageSettingMap;
    auto properties = SpxQueryService<ISpxNamedProperties>(GetSite());
    SPX_IFTRUE_THROW_HR(properties == nullptr, SPXERR_UNEXPECTED_USP_SITE_FAILURE);
    for (const string& language : languages)
    {
        string perLanguageSettingProperty = language + GetPropertyName(propertyId);
        auto perLanguageSettingPropertyValue = properties->GetStringValue(perLanguageSettingProperty.c_str());
        if (!perLanguageSettingPropertyValue.empty())
        {
            languageSettingMap[language] = perLanguageSettingPropertyValue;
        }
    }
    return languageSettingMap;
}

CSpxStringMap CSpxUspRecoEngineAdapter::GetParametersFromUser(std::string&& path)
{
    auto getter = SpxQueryInterface<ISpxGetUspMessageParamsFromUser>(GetSite());
    SPX_IFTRUE_THROW_HR(getter == nullptr, SPXERR_UNEXPECTED_USP_SITE_FAILURE);

    return getter->GetParametersFromUser(move(path));
}

void CSpxUspRecoEngineAdapter::OnToken(const std::string token)
{
    auto properties = SpxQueryService<ISpxNamedProperties>(GetSite());
    SPX_IFTRUE_THROW_HR(properties == nullptr, SPXERR_UNEXPECTED_USP_SITE_FAILURE);
    properties->SetStringValue("SPEECH-UspContinuationToken", token.c_str());
}

void CSpxUspRecoEngineAdapter::OnAcknowledgedAudio(uint64_t offset)
{
    offset = offset + m_startingOffset;

    SPX_DBG_TRACE_VERBOSE("%s: this=0x%8p Service acknowledging to offset %" PRIu64 " (100ns).", __FUNCTION__, (void*)this, offset);
    auto site = GetSite();
    auto properties = SpxQueryService<ISpxNamedProperties>(site);
    SPX_IFTRUE_THROW_HR(properties == nullptr, SPXERR_UNEXPECTED_USP_SITE_FAILURE);
    properties->SetStringValue("SPEECH-UspContinuationOffset", std::to_string(offset).c_str());

    auto replayer = SpxQueryInterface<ISpxAudioReplayer>(site);
    if (nullptr != replayer)
    {
        replayer->ShrinkReplayBuffer(offset);
    }
}

std::shared_ptr<ISpxNamedProperties> CSpxUspRecoEngineAdapter::GetParentProperties() const
{
    return SpxQueryService<ISpxNamedProperties>(GetSite());
}

} } } } // Microsoft::CognitiveServices::Speech::Impl
