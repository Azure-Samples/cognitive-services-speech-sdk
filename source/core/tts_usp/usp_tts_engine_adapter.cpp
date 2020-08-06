//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// usp_tts_engine_adapter.cpp: Implementation definitions for CSpxUspTtsEngineAdapter C++ class
//

#include "stdafx.h"
#include <usp_text_message.h>
#include "synthesis_helper.h"
#include "usp_tts_engine_adapter.h"
#include "create_object_helpers.h"
#include "guid_utils.h"
#include "handle_table.h"
#include "service_helpers.h"
#include "property_bag_impl.h"
#include "property_id_2_name_map.h"
#include "json.h"
#include "spx_build_information.h"
#include "time_utils.h"
#include "thread_service.h"

#define SPX_DBG_TRACE_USP_TTS 1

#define MAX_RETRY 1

#define WAIT_FOR_FIRST_CHUNK_TIMEOUT 20000  // in milliseconds
#define RECEIVE_ALL_CHUNKS_TIMEOUT 300000   // in milliseconds
#ifdef _DEBUG
#define DEBUG_WAIT_INTERVAL 100         // in milliseconds
#endif

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


CSpxUspTtsEngineAdapter::CSpxUspTtsEngineAdapter()
{
    SPX_DBG_TRACE_VERBOSE_IF(SPX_DBG_TRACE_USP_TTS, __FUNCTION__);
}

CSpxUspTtsEngineAdapter::~CSpxUspTtsEngineAdapter()
{
    SPX_DBG_TRACE_VERBOSE_IF(SPX_DBG_TRACE_USP_TTS, __FUNCTION__);
    SPX_DBG_TRACE_VERBOSE("%s: this=0x%8p", __FUNCTION__, (void*)this);
    SPX_DBG_ASSERT(m_uspCallbacks == nullptr);
    SPX_DBG_ASSERT(m_uspConnection == nullptr);
}

void CSpxUspTtsEngineAdapter::Init()
{
    // Initialize thread service
    m_threadService = SpxCreateObjectWithSite<ISpxThreadService>("CSpxThreadService", SpxQueryInterface<ISpxGenericSite>(GetSite()));

    // Get proxy setting
    GetProxySetting();

    // Initialize websocket platform
    Microsoft::CognitiveServices::Speech::USP::PlatformInit(m_proxyHost.c_str(), m_proxyPort, m_proxyUsername.c_str(), m_proxyPassword.c_str());

}

void CSpxUspTtsEngineAdapter::Term()
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    UspTerminate();
    SpxTermAndClear(m_threadService);
}

void CSpxUspTtsEngineAdapter::SetOutput(std::shared_ptr<ISpxAudioOutput> output)
{
    SPX_DBG_TRACE_VERBOSE_IF(SPX_DBG_TRACE_USP_TTS, __FUNCTION__);
    m_audioOutput = output;
}

std::shared_ptr<ISpxSynthesisResult> CSpxUspTtsEngineAdapter::Speak(const std::string& text, bool isSsml, const std::wstring& requestId, bool retry)
{
    SPX_DBG_TRACE_VERBOSE_IF(SPX_DBG_TRACE_USP_TTS, __FUNCTION__);
    SPX_DBG_TRACE_VERBOSE("%d", int(m_uspState.load()));
    SPX_DBG_ASSERT(UspState::Idle == m_uspState || UspState::Error == m_uspState);

    m_shouldStop = false;

    std::shared_ptr<ISpxSynthesisResult> result;
    const auto maxRetry = std::stoi(ISpxPropertyBagImpl::GetStringValue("SpeechSynthesis_MaxRetryTimes", std::to_string(MAX_RETRY).c_str()));
    for (auto tryCount = 0; tryCount <= maxRetry * static_cast<int>(retry); ++tryCount)
    {
        SPX_DBG_TRACE_VERBOSE_IF(SPX_DBG_TRACE_USP_TTS,
            "%s: start to send synthesis request, request id : %s, try: %d", __FUNCTION__, PAL::ToString(requestId).c_str(), tryCount);
        result = SpeakInternal(text, isSsml, requestId);
        if (ResultReason::SynthesizingAudioCompleted == result->GetReason())
        {
            break;
        }
        else if (ResultReason::Canceled == result->GetReason() && !result->GetAudioData()->empty())
        {
            SPX_TRACE_ERROR("Synthesis cancelled with partial data received, cannot retry.");
            break;
        }
        else if (m_currentErrorCode == USP::ErrorCode::ClientClosingConnection)
        {
            SPX_TRACE_ERROR("Synthesis cancelled by user, won't retry.");
            break;
        }

        SPX_TRACE_ERROR("Synthesis cancelled without data received, retrying.");
    }

    return result;
}

std::shared_ptr<ISpxSynthesisResult> CSpxUspTtsEngineAdapter::SpeakInternal(const std::string& text, bool isSsml, const std::wstring& requestId)
{
    auto ssml = text;
    if (!isSsml)
    {
        const auto properties = SpxQueryService<ISpxNamedProperties>(GetSite());
        ssml = CSpxSynthesisHelper::BuildSsml(text, properties);
    }

    SPX_DBG_TRACE_VERBOSE("SSML sent to TTS cognitive service: %s", ssml.c_str());

    if (!m_shouldStop)
    {
        EnsureUspConnection();
    }

    if (UspState::Error != m_uspState)
    {
        // Set initial values for current utterance
        m_currentRequestId = requestId;
        m_currentText = PAL::ToWString(text);
        m_currentTextIsSsml = isSsml;
        m_currentTextOffset = 0;
        m_currentErrorCode = static_cast<USP::ErrorCode>(0);
        m_currentErrorMessage = std::string();

        // Send request
        m_uspState = UspState::Sending;
        m_currentReceivedData.clear();
        UspSendSynthesisContext(PAL::ToString(requestId));
        UspSendSsml(ssml, PAL::ToString(requestId));
    }

    std::unique_lock<std::mutex> lock(m_mutex);

    // wait to get the first audio chunk
    const auto firstChunkTimeout = std::stoi(ISpxPropertyBagImpl::GetStringValue("SpeechSynthesis_FirstChunkTimeoutMs", std::to_string(WAIT_FOR_FIRST_CHUNK_TIMEOUT).c_str()));
#ifdef _DEBUG
    auto remainingTime = firstChunkTimeout;
    while (!m_cv.wait_for(lock, std::chrono::milliseconds(DEBUG_WAIT_INTERVAL), [&] { return m_uspState == UspState::ReceivingData || m_uspState == UspState::Idle || m_uspState == UspState::Error; }) && remainingTime > 0)
    {
        SPX_DBG_TRACE_VERBOSE("%s: waiting for USP to get first audio chunk ...", __FUNCTION__);
        remainingTime -= DEBUG_WAIT_INTERVAL;
    }
#else
    m_cv.wait_for(lock, std::chrono::milliseconds(firstChunkTimeout), [&] { return m_uspState == UspState::ReceivingData || m_uspState == UspState::Idle || m_uspState == UspState::Error; });
#endif

    if (m_uspState == UspState::Sending || m_uspState == UspState::TurnStarted)
    {
        SPX_TRACE_ERROR("USP error, timeout to get the first audio chunk.");
        m_currentErrorMessage = "USP error, timeout to get the first audio chunk.";
        m_uspState = UspState::Error;
        UspTerminate();
    }

    // wait to receive all audio data
    const auto allChunkTimeout = std::stoi(ISpxPropertyBagImpl::GetStringValue("SpeechSynthesis_AllChunkTimeoutMs", std::to_string(RECEIVE_ALL_CHUNKS_TIMEOUT).c_str()));
#ifdef _DEBUG
    remainingTime = allChunkTimeout;
    while (!m_cv.wait_for(lock, std::chrono::milliseconds(DEBUG_WAIT_INTERVAL), [&] { return m_uspState == UspState::Idle || m_uspState == UspState::Error; }) && remainingTime > 0)
    {
        SPX_DBG_TRACE_VERBOSE("%s: waiting for USP to finish receiving data ...", __FUNCTION__);
        remainingTime -= DEBUG_WAIT_INTERVAL;
    }
#else
    m_cv.wait_for(lock, std::chrono::milliseconds(allChunkTimeout), [&] { return m_uspState == UspState::Idle || m_uspState == UspState::Error; });
#endif

    if (m_uspState == UspState::ReceivingData)
    {
        SPX_TRACE_ERROR("USP error, timeout to get all audio data.");
        m_currentErrorMessage = "USP error, timeout to get all audio data.";
        m_currentErrorMessage += " Received audio size: " + std::to_string(m_currentReceivedData.size()) + "bytes.";
        m_uspState = UspState::Error;
        UspTerminate();
    }

    bool hasHeader = false;
    auto outputFormat = GetOutputFormat(m_audioOutput, &hasHeader);

    auto result = GetSite()->CreateEmptySynthesisResult();
    auto resultInit = SpxQueryInterface<ISpxSynthesisResultInit>(result);
    if (m_uspState == UspState::Error)
    {
        resultInit->InitSynthesisResult(requestId, ResultReason::Canceled,
            m_currentErrorCode == USP::ErrorCode::ClientClosingConnection ? CancellationReason::CancelledByUser : CancellationReason::Error,
            UspErrorCodeToCancellationErrorCode(m_currentErrorCode),
            m_currentReceivedData.data(), m_currentReceivedData.size(), outputFormat.get(), hasHeader);
        SpxQueryInterface<ISpxNamedProperties>(resultInit)->SetStringValue(GetPropertyName(PropertyId::CancellationDetails_ReasonDetailedText), m_currentErrorMessage.data());
        m_shouldStop = false;
    }
    else
    {
        resultInit->InitSynthesisResult(requestId, ResultReason::SynthesizingAudioCompleted,
            REASON_CANCELED_NONE, CancellationErrorCode::NoError, m_currentReceivedData.data(), m_currentReceivedData.size(), outputFormat.get(), hasHeader);
    }

    return result;
}

void CSpxUspTtsEngineAdapter::StopSpeaking()
{
    SPX_DBG_TRACE_VERBOSE_IF(SPX_DBG_TRACE_USP_TTS, __FUNCTION__);
    m_shouldStop = true;
    OnError(true, USP::ErrorCode::ClientClosingConnection, "Synthesis request cancelled by user.");
}

std::shared_ptr<ISpxNamedProperties> CSpxUspTtsEngineAdapter::GetParentProperties() const
{
    return SpxQueryService<ISpxNamedProperties>(GetSite());
}

void CSpxUspTtsEngineAdapter::GetProxySetting()
{
    m_proxyHost = ISpxPropertyBagImpl::GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_ProxyHostName), "");
    m_proxyPort = std::stoi(ISpxPropertyBagImpl::GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_ProxyPort), "0"));
    if (m_proxyPort < 0)
    {
        ThrowInvalidArgumentException("Invalid proxy port: %d", m_proxyPort);
    }

    m_proxyUsername = ISpxPropertyBagImpl::GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_ProxyUserName), "");
    m_proxyPassword = ISpxPropertyBagImpl::GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_ProxyPassword), "");
}

void CSpxUspTtsEngineAdapter::SetSpeechConfigMessage()
{
    constexpr auto systemName = "SpeechSDK";

    nlohmann::json speechConfig;

    // Set system configuration data.
    speechConfig["context"]["system"]["version"] = BuildInformation::g_fullVersion;
    speechConfig["context"]["system"]["name"] = systemName;
    speechConfig["context"]["system"]["build"] = BuildInformation::g_buildPlatform;

    // Set OS configuration data.
    auto osInfo = PAL::getOperatingSystem();
    speechConfig["context"]["os"]["platform"] = osInfo.platform;
    speechConfig["context"]["os"]["name"] = osInfo.name;
    speechConfig["context"]["os"]["version"] = osInfo.version;

    m_speechConfig = speechConfig.dump();
}

void CSpxUspTtsEngineAdapter::UspSendSpeechConfig()
{
    constexpr auto messagePath = "speech.config";
    SPX_DBG_TRACE_VERBOSE("%s %s", messagePath, m_speechConfig.c_str());

    UspSendMessage(std::make_unique<USP::TextMessage>(m_speechConfig, messagePath, USP::MessageType::Config));
}

void CSpxUspTtsEngineAdapter::UspSendSynthesisContext(const std::string& requestId)
{
    constexpr auto messagePath = "synthesis.context";

    const auto properties = SpxQueryService<ISpxNamedProperties>(GetSite());

    // Set synthesis context data.
    nlohmann::json synthesisContext;
    synthesisContext["synthesis"]["audio"]["outputFormat"] = GetOutputFormatString(m_audioOutput);
    synthesisContext["synthesis"]["audio"]["metadataOptions"]["wordBoundaryEnabled"] = PAL::BoolToString(WordBoundaryEnabled());
    synthesisContext["synthesis"]["audio"]["metadataOptions"]["sentenceBoundaryEnabled"] = properties->GetStringValue("SpeechServiceResponse_Synthesis_SentenceBoundaryEnabled", "false");

    synthesisContext["synthesis"]["language"]["autoDetection"] = CSpxSynthesisHelper::LanguageAutoDetectionEnabled(properties);

    UspSendMessage(
        std::make_unique<USP::TextMessage>(
            synthesisContext.dump(), messagePath, USP::MessageType::Context, requestId));
}

void CSpxUspTtsEngineAdapter::UspSendSsml(const std::string& ssml, const std::string& requestId)
{
    constexpr auto messagePath = "ssml";
    constexpr auto contentType = "application/ssml+xml";

    SPX_DBG_TRACE_VERBOSE("%s %s", messagePath, ssml.c_str());

    UspSendMessage(
        std::make_unique<USP::TextMessage>(
            ssml, messagePath, contentType, USP::MessageType::Ssml, requestId));
}

void CSpxUspTtsEngineAdapter::UspSendMessage(std::unique_ptr<USP::TextMessage> message)
{
    if (message == nullptr)
    {
        SPX_TRACE_WARNING("Received a null message to send. Ignoring");
        return;
    }

    SPX_DBG_TRACE_VERBOSE("%s='%s'", message->Path().c_str(), message->Data().c_str());
    std::weak_ptr<USP::Connection> connection(m_uspConnection);

    // NOTE:
    // -----
    // There is a bug in the Microsoft C++ compiler that prevents from creating a packaged_task
    // from a move only lambda: https://github.com/microsoft/STL/issues/321
    // To work around this, we'll need to pass a raw pointer (icky I know)

#if _MSC_VER <= 1926
    USP::TextMessage* ptr = nullptr;
    try
    {
        ptr = message.release();

        // NOTE: If the following code doesn't compile complaining about a deleted method call, increment the #IF version check value in line 292
        
        std::packaged_task<void()> task([connection, ptr]()
        {
            DoSendMessageWork(connection, std::unique_ptr<USP::TextMessage>(ptr));
        });
        m_threadService->ExecuteAsync(move(task));

        // successfully queued so no need to free it
        // WARNING: There is still the potential of a memory leak here if the thread_service gets terminated
        //          before it is able to execute the queued lambda but I can't see an easy way to prevent that
        ptr = nullptr;
    }
    catch (...)
    {
        // Free to prevent memory leaks. This works since message uses the default deleter
        delete ptr;

        throw;
    }

    // Free to prevent memory leaks. This works since message uses the default deleter
    delete ptr;
#else
    std::packaged_task<void()> task([connection, message = std::move(message)]() mutable
    {
        DoSendMessageWork(connection, std::move(message));
    });
    m_threadService->ExecuteAsync(move(task));
#endif
}

void CSpxUspTtsEngineAdapter::DoSendMessageWork(std::weak_ptr<USP::Connection> connectionPtr, std::unique_ptr<USP::TextMessage> message)
{
    auto connection = connectionPtr.lock();
    if (connection != nullptr)
    {
        connection->SendMessage(move(message));
    }
    else
    {
        SPX_TRACE_ERROR("usp connection lost when trying to send message.");
    }
}

void CSpxUspTtsEngineAdapter::EnsureUspConnection()
{
    m_uspState = UspState::Connecting;
    if (m_uspConnection == nullptr)
    {
        UspInitialize();
    }
    else if (!m_uspConnection->IsConnected())
    {
        // If the connection was closed due to any reason, we re-connect
        UspTerminate();
        UspInitialize();
    }
    else if (PAL::GetTicks(std::chrono::system_clock::now() - m_lastConnectTime) > static_cast<uint64_t>(9) * 60 * 1000 * 10000)
    {
        // Per https://speechwiki.azurewebsites.net/partners/protocol-websockets-tts.html#connection-duration-limitations
        // The service closes the active connect after 10 mins.
        // We re-connect it after 9 mins in case it breaks an on-going speak.
        UspTerminate();
        UspInitialize();
    }
}

void CSpxUspTtsEngineAdapter::UspInitialize()
{
    SPX_DBG_TRACE_VERBOSE("%s: this=0x%8p", __FUNCTION__, (void*)this);
    SPX_IFTRUE_THROW_HR(m_uspConnection != nullptr, SPXERR_ALREADY_INITIALIZED);

    // Fill authorization token
    std::array<std::string, static_cast<size_t>(USP::AuthenticationType::SIZE_AUTHENTICATION_TYPE)> authData;

    // Get the named property service...
    auto properties = SpxQueryService<ISpxNamedProperties>(GetSite());
    SPX_IFTRUE_THROW_HR(properties == nullptr, SPXERR_UNEXPECTED_USP_SITE_FAILURE);

    // Get the properties that indicates what endpoint to use...
    auto uspSubscriptionKey = properties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_Key));
    auto token = properties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceAuthorization_Token));

    authData[static_cast<size_t>(USP::AuthenticationType::SubscriptionKey)] = std::move(uspSubscriptionKey);
    authData[static_cast<size_t>(USP::AuthenticationType::AuthorizationToken)] = std::move(token);

    // Create the usp client, which we'll configure and use to create the actual connection
    auto uspCallbacks = SpxCreateObjectWithSite<ISpxUspCallbacks>("CSpxUspCallbackWrapper", this);
    auto client = USP::Client(uspCallbacks, USP::EndpointType::SpeechSynthesis, PAL::CreateGuidWithoutDashes(), m_threadService)
        .SetAuthentication(authData);

    SetUspEndpoint(properties, client);

    // Set proxy
    if (!m_proxyHost.empty() && m_proxyPort > 0)
    {
        client.SetProxyServerInfo(m_proxyHost.data(), m_proxyPort, m_proxyUsername.data(), m_proxyPassword.data());
    }

    SetUspSingleTrustedCert(properties, client);

    // Try to connect
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

    // if error occurs in the above client.Connect, set state to error and return.
    if (uspConnection == nullptr)
    {
        m_uspState = UspState::Error;
        m_currentErrorMessage = "USP connection establishment failed.";
        return;
    }

    // We're done!!
    m_uspCallbacks = uspCallbacks;
    m_uspConnection = std::move(uspConnection);
    if (m_uspState == UspState::Connecting)
    {
        m_uspState = UspState::Idle;

        m_lastConnectTime = std::chrono::system_clock::now();

        // Send speech config message
        if (m_uspConnection != nullptr)
        {
            properties->SetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_Url), m_uspConnection->GetConnectionUrl().c_str());

            // Construct config message payload
            SetSpeechConfigMessage();

            // Send speech config
            UspSendSpeechConfig();
        }
    }
}

void CSpxUspTtsEngineAdapter::UspTerminate()
{
    // Term the callbacks first and then reset/release the connection
    SpxTermAndClear(m_uspCallbacks);

    m_uspConnection.reset();

}

USP::Client& CSpxUspTtsEngineAdapter::SetUspEndpoint(const std::shared_ptr<ISpxNamedProperties>& properties,
    USP::Client& client) const
{
    SPX_DBG_ASSERT(GetSite() != nullptr);

    const auto endpoint = properties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_Endpoint));
    const auto host = properties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_Host));
    const auto region = properties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_Region));

    SPX_IFFALSE_THROW_HR(
        static_cast<int>(endpoint.empty()) + static_cast<int>(host.empty()) + static_cast<int>(region.empty()) == 2,
        SPXERR_INVALID_ARG);

    if (!endpoint.empty())
    {
        // set endpoint url if this is provided
        SPX_DBG_TRACE_VERBOSE("%s: Using custom endpoint: %s", __FUNCTION__, endpoint.c_str());
        client.SetEndpointUrl(endpoint);
    }
    else if (!host.empty())
    {
        // set host url if this is provided
        SPX_DBG_TRACE_VERBOSE("%s: Using custom host: %s", __FUNCTION__, host.c_str());
        client.SetHostUrl(host);
    }
    else
    {
        client.SetRegion(region);
    }

    return client;
}

USP::Client& CSpxUspTtsEngineAdapter::SetUspSingleTrustedCert(const std::shared_ptr<ISpxNamedProperties>& properties, USP::Client& client) const
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

void CSpxUspTtsEngineAdapter::OnTurnStart(const USP::TurnStartMsg& message)
{
    UNUSED(message);
    std::unique_lock<std::mutex> lock(m_mutex);

    if (m_uspState == UspState::Sending)
    {
        m_uspState = UspState::TurnStarted;
    }
    else if (m_uspState != UspState::Error)
    {
        SPX_TRACE_ERROR("turn.start received in invalid state, current state is: %d", UspState(m_uspState));
        SPX_THROW_HR(SPXERR_INVALID_STATE);
    }

    m_currentReceivedData.clear();
    m_cv.notify_all();
}

void CSpxUspTtsEngineAdapter::OnAudioOutputChunk(const USP::AudioOutputChunkMsg& message)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    if (m_uspState == UspState::TurnStarted)
    {
        m_uspState = UspState::ReceivingData;
    }
    else if (m_uspState != UspState::ReceivingData)
    {
        SPX_TRACE_ERROR("Received chunk data in unexpected state, ingore. Current state: %d", UspState(m_uspState));
        return;
    }

    InvokeOnSite([this, message](const SitePtr& p) {
        if (message.audioLength > 0)
        {
            p->Write(this, m_currentRequestId, const_cast<uint8_t *>(message.audioBuffer), static_cast<uint32_t>(message.audioLength), nullptr);
        }
    });

    const auto originalSize = m_currentReceivedData.size();
    m_currentReceivedData.resize(originalSize + message.audioLength);
    memcpy(m_currentReceivedData.data() + originalSize, message.audioBuffer, message.audioLength);
    m_cv.notify_all();
}

void CSpxUspTtsEngineAdapter::OnAudioOutputMetadata(const USP::AudioOutputMetadataMsg& message)
{
    auto synthesizerEvents = SpxQueryInterface<ISpxSynthesizerEvents>(GetSite());

    for (auto iterator = message.metadatas.begin(); iterator != message.metadatas.end(); ++iterator)
    {
        if (PAL::stricmp(iterator->type.data(), METADATA_TYPE_WORD_BOUNDARY) == 0)
        {
            auto wordBoundary = iterator->textBoundary;
            auto textOffset = m_currentText.find(wordBoundary.text, m_currentTextOffset);

            if (m_currentTextIsSsml)
            {
                while (textOffset != std::string::npos && InSsmlTag(textOffset, m_currentText, m_currentTextOffset))
                {
                    textOffset = m_currentText.find(wordBoundary.text, textOffset + wordBoundary.text.length());
                }
            }

            if (textOffset != std::string::npos)
            {
                m_currentTextOffset = static_cast<uint32_t>(textOffset + wordBoundary.text.length());
                synthesizerEvents->FireWordBoundary(wordBoundary.audioOffset, static_cast<uint32_t>(textOffset), static_cast<uint32_t>(wordBoundary.text.length()));
            }
        }
    }
}

void CSpxUspTtsEngineAdapter::OnTurnEnd(const USP::TurnEndMsg& message)
{
    UNUSED(message);
    std::unique_lock<std::mutex> lock(m_mutex);
    m_uspState = UspState::Idle;
    m_cv.notify_all();
}

void CSpxUspTtsEngineAdapter::OnError(bool transport, USP::ErrorCode errorCode, const std::string& errorMessage)
{
    UNUSED(transport);
    SPX_DBG_TRACE_VERBOSE("Response: On Error: Code:%d, Message: %s.\n", errorCode, errorMessage.c_str());
    std::unique_lock<std::mutex> lock(m_mutex);
    if (m_uspState != UspState::Idle || m_shouldStop) // no need to raise an error if synthesis is already done.
    {
        m_currentErrorCode = errorCode;
        m_currentErrorMessage = errorMessage;
        m_currentErrorMessage += " USP state: " + std::to_string((int)(UspState)m_uspState) + ".";
        m_currentErrorMessage += " Received audio size: " + std::to_string(m_currentReceivedData.size()) + "bytes.";
        m_uspState = UspState::Error;
        m_cv.notify_all();

        if (m_uspState != UspState::Idle)
        {
            // terminate usp connection on error
            UspTerminate();
        }
    }
}

SpxWAVEFORMATEX_Type CSpxUspTtsEngineAdapter::GetOutputFormat(std::shared_ptr<ISpxAudioOutput> output, bool* hasHeader)
{
    auto audioStream = SpxQueryInterface<ISpxAudioStream>(output);
    auto requiredFormatSize = audioStream->GetFormat(nullptr, 0);
    auto format = SpxAllocWAVEFORMATEX(requiredFormatSize);
    audioStream->GetFormat(format.get(), requiredFormatSize);

    if (hasHeader != nullptr)
    {
        *hasHeader = SpxQueryInterface<ISpxAudioOutputFormat>(output)->HasHeader();
    }

    return format;
}

std::string CSpxUspTtsEngineAdapter::GetOutputFormatString(std::shared_ptr<ISpxAudioOutput> output)
{
    auto outputFormat = SpxQueryInterface<ISpxAudioOutputFormat>(output);
    auto formatString = outputFormat->GetFormatString();
    if (outputFormat->HasHeader())
    {
        formatString = outputFormat->GetRawFormatString();
    }

    return formatString;
}

bool CSpxUspTtsEngineAdapter::WordBoundaryEnabled() const
{
    auto wordBoundaryEnabled = PAL::ToBool(ISpxPropertyBagImpl::GetStringValue("SpeechServiceResponse_Synthesis_WordBoundaryEnabled", "true"));

    auto synthesizerEvents = GetSite()->GetEventsSite();
    auto wordBoundaryConnected = synthesizerEvents->WordBoundary.IsConnected();

    return wordBoundaryEnabled && wordBoundaryConnected;
}

bool CSpxUspTtsEngineAdapter::InSsmlTag(size_t currentPos, const std::wstring& ssml, size_t beginningPos)
{
    if (currentPos < beginningPos || currentPos >= ssml.length() || beginningPos >= ssml.length())
    {
        return false;
    }

    auto pos = currentPos;
    while (pos >= beginningPos)
    {
        if (*(ssml.data() + pos) == '>')
        {
            return false;
        }

        if (*(ssml.data() + pos) == '<')
        {
            return true;
        }

        pos--;
    }

    return false;
}

CancellationErrorCode CSpxUspTtsEngineAdapter::UspErrorCodeToCancellationErrorCode(USP::ErrorCode uspErrorCode)
{
    std::map<USP::ErrorCode, CancellationErrorCode> uspErrorCodeToCancellationErrorCodeMapping = {
        { USP::ErrorCode::AuthenticationError, CancellationErrorCode::AuthenticationFailure },
        { USP::ErrorCode::BadRequest, CancellationErrorCode::BadRequest },
        { USP::ErrorCode::ConnectionError, CancellationErrorCode::ConnectionFailure },
        { USP::ErrorCode::Forbidden, CancellationErrorCode::Forbidden },
        { USP::ErrorCode::RuntimeError, CancellationErrorCode::RuntimeError },
        { USP::ErrorCode::ServiceError, CancellationErrorCode::ServiceError },
        { USP::ErrorCode::ServiceUnavailable, CancellationErrorCode::ServiceUnavailable },
        { USP::ErrorCode::TooManyRequests, CancellationErrorCode::TooManyRequests }
    };

    auto cancellationErrorCode = CancellationErrorCode::NoError;

    auto iter = uspErrorCodeToCancellationErrorCodeMapping.find(uspErrorCode);
    if (iter != uspErrorCodeToCancellationErrorCodeMapping.end())
    {
        cancellationErrorCode = iter->second;
    }

    return cancellationErrorCode;
}


} } } } // Microsoft::CognitiveServices::Speech::Impl
