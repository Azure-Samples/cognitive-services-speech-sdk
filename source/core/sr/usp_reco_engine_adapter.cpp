//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// usp_reco_engine_adapter.cpp: Implementation definitions for CSpxUspRecoEngineAdapter C++ class
//

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


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


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

    WriteLock_Type writeLock(m_stateMutex);
    if (ChangeState(UspState::Terminating))
    {
        SPX_DBG_TRACE_VERBOSE("%s: Terminating USP Connection (0x%8x)", __FUNCTION__, m_uspConnection.get());
        writeLock.unlock();
        UspTerminate();

        writeLock.lock();
        ChangeState(UspState::Zombie);
    }
    else
    {
        SPX_DBG_ASSERT(writeLock.owns_lock()); // need to keep the lock for trace message
        SPX_DBG_TRACE_WARNING("%s: (0x%8x) UNEXPECTED USP State transition ... (audioState/uspState=%d/%d)", __FUNCTION__, this, m_audioState, m_uspState);
    }
}

void CSpxUspRecoEngineAdapter::SetAdapterMode(bool singleShot)
{
    SPX_DBG_TRACE_VERBOSE("%s: singleShot=%d", __FUNCTION__, singleShot);
    m_singleShot = singleShot;
}

void CSpxUspRecoEngineAdapter::SetFormat(SPXWAVEFORMATEX* pformat)
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

    WriteLock_Type writeLock(m_stateMutex);
    if (IsState(UspState::Zombie))
    {
        SPX_DBG_ASSERT(writeLock.owns_lock()); // need to keep the lock for trace message
        SPX_DBG_TRACE_VERBOSE("%s: (0x%8x) IGNORING... (audioState/uspState=%d/%d) USP-ZOMBIE", __FUNCTION__, this, m_audioState, m_uspState);
    }
    else if (IsBadState() && !IsState(UspState::Terminating))
    {
        SPX_DBG_ASSERT(writeLock.owns_lock()); // need to keep the lock for trace message
        SPX_DBG_TRACE_VERBOSE("%s: (0x%8x) IGNORING... (audioState/uspState=%d/%d) %s", __FUNCTION__, this, m_audioState, m_uspState, IsState(UspState::Terminating) ? "(USP-TERMINATING)" : "********** USP-UNEXPECTED !!!!!!");
    }
    else if (pformat != nullptr && IsState(UspState::Idle) && ChangeState(AudioState::Idle, AudioState::Ready))
    {
        SPX_DBG_ASSERT(writeLock.owns_lock()); // need to keep the lock for trace message
        SPX_DBG_TRACE_VERBOSE("%s: (0x%8x)->PrepareFirstAudioReadyState()", __FUNCTION__, this);
        PrepareFirstAudioReadyState(pformat);
    }
    else if (pformat == nullptr && (ChangeState(AudioState::Idle) || IsState(UspState::Terminating)))
    {
        writeLock.unlock(); // calls to site shouldn't hold locks

        SPX_DBG_TRACE_VERBOSE("%s: (0x%8x) site->AdapterCompletedSetFormatStop()", __FUNCTION__, this);
        InvokeOnSite([this](const SitePtr& p) { p->AdapterCompletedSetFormatStop(this); });

        m_format.reset();
    }
    else
    {
        SPX_DBG_ASSERT(writeLock.owns_lock()); // need to keep the lock for trace message
        SPX_DBG_TRACE_WARNING("%s: (0x%8x) UNEXPECTED USP State transition ... (audioState/uspState=%d/%d)", __FUNCTION__, this, m_audioState, m_uspState);
    }
}

void CSpxUspRecoEngineAdapter::ProcessAudio(AudioData_Type data, uint32_t size)
{
    WriteLock_Type writeLock(m_stateMutex);
    if (IsState(UspState::Zombie) && size == 0)
    {
        SPX_DBG_ASSERT(writeLock.owns_lock()); // need to keep the lock for trace message
        SPX_DBG_TRACE_VERBOSE("%s: (0x%8x) IGNORING... size=0 ... (audioState/uspState=%d/%d) USP-ZOMBIE", __FUNCTION__, this, m_audioState, m_uspState);
    }
    else if (IsBadState())
    {
        SPX_DBG_ASSERT(writeLock.owns_lock()); // need to keep the lock for trace message
        SPX_DBG_TRACE_VERBOSE("%s: (0x%8x) IGNORING... (audioState/uspState=%d/%d) %s", __FUNCTION__, this, m_audioState, m_uspState, IsState(UspState::Terminating) ? "(USP-TERMINATING)" : "********** USP-UNEXPECTED !!!!!!");
    }
    else if (size > 0 && ChangeState(AudioState::Ready, UspState::Idle, AudioState::Sending, UspState::WaitingForTurnStart))
    {
        writeLock.unlock(); // SendPreAudioMessages() will use USP to send speech config/context ... don't hold the lock

        SPX_DBG_TRACE_VERBOSE_IF(1, "%s: (0x%8x)->SendPreAudioMessages() ... size=%d", __FUNCTION__, this, size);
        SendPreAudioMessages();
        UspWrite(data.get(), size);

        SPX_DBG_TRACE_VERBOSE("%s: site->AdapterStartingTurn()", __FUNCTION__);
        InvokeOnSite([this](const SitePtr& p) { p->AdapterStartingTurn(this); });
    }
    else if (size > 0 && IsState(AudioState::Sending))
    {
        writeLock.unlock(); // UspWrite() will use USP to send data... don't hold the lock

        SPX_DBG_TRACE_VERBOSE_IF(1, "%s: (0x%8x) Sending Audio ... size=%d", __FUNCTION__, this, size);
        UspWrite(data.get(), size);
    }
    else if (size == 0 && IsState(AudioState::Sending))
    {
        writeLock.unlock(); // UspWriteFlush() will use USP to send data... don't hold the lock

        SPX_DBG_TRACE_VERBOSE_IF(1, "%s: (0x%8x) Flushing Audio ... size=0 USP-FLUSH", __FUNCTION__, this);
        UspWriteFlush();
    }
    else if (!IsState(AudioState::Sending))
    {
        SPX_DBG_ASSERT(writeLock.owns_lock()); // need to keep the lock for trace message
        SPX_DBG_TRACE_VERBOSE_IF(1, "%s: (0x%8x) Ignoring audio size=%d ... (audioState/uspState=%d/%d)", __FUNCTION__, this, size, m_audioState, m_uspState);
    }
    else
    {
        SPX_DBG_ASSERT(writeLock.owns_lock()); // need to keep the lock for trace message
        SPX_DBG_TRACE_WARNING("%s: (0x%8x) UNEXPECTED USP State transition ... (audioState/uspState=%d/%d)", __FUNCTION__, this, m_audioState, m_uspState);
    }
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

    // Create the usp client, which we'll configure and use to create the actual connection
    auto uspCallbacks = SpxCreateObjectWithSite<ISpxUspCallbacks>("CSpxUspCallbackWrapper", this);

    // Currently we use a session id as a connection id to correlate logs on the server side with a particular user
    // session, because currently there is a single connection for session at any point in time.
    auto sessionId = PAL::ToWString(properties->GetStringValue(GetPropertyName(PropertyId::Speech_SessionId)));
    USP::Client client(uspCallbacks, USP::EndpointType::Speech, sessionId);

    // Set up the connection properties, and create the connection
    SetUspEndpoint(properties, client);
    SetUspRecoMode(properties, client);
    SetUspAuthentication(properties, client);
    client.SetOutputFormat(GetOutputFormat(*properties));

    SPX_DBG_TRACE_VERBOSE("%s: recoMode=%d", __FUNCTION__, m_recoMode);
    auto uspConnection = client.Connect();

    // Keep track of what time we initialized (so we can reset later)
    m_uspInitTime = std::chrono::system_clock::now();
    m_uspResetTime = m_uspInitTime + std::chrono::seconds(m_resetUspAfterTimeSeconds);

    // We're done!!
    m_uspCallbacks = uspCallbacks;
    m_uspConnection = std::move(uspConnection);
}

void CSpxUspRecoEngineAdapter::UspTerminate()
{
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
    uint16_t countSpeech, countIntent, countTranslation;
    GetSite()->GetScenarioCount(&countSpeech, &countIntent, &countTranslation);
    SPX_DBG_ASSERT(countSpeech + countIntent + countTranslation == 1); // currently only support one recognizer

    // now based on the endpoint, which types of recognizers, and/or the custom model id ... set up the endpoint
    auto endpoint = properties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_Endpoint));
    if (0 == PAL::stricmp(endpoint.c_str(), "CORTANA"))
    {
        return SetUspEndpoint_Cortana(properties, client);
    }
    else if (!endpoint.empty())
    {
        return SetUspEndpoint_Custom(properties, client);
    }
    else if (countIntent == 1)
    {
        return SetUspEndpoint_Intent(properties, client);
    }
    else if (countTranslation == 1)
    {
        return SetUspEndpoint_Translation(properties, client);
    }

    return SetUspEndpoint_DefaultSpeechService(properties, client);
}

USP::Client& CSpxUspRecoEngineAdapter::SetUspEndpoint_Cortana(std::shared_ptr<ISpxNamedProperties>& properties, USP::Client& client)
{
    UNUSED(properties);

    SPX_DBG_TRACE_VERBOSE("%s: Using Cortana endpoint...", __FUNCTION__);
    m_customEndpoint = true;

    return client.SetEndpointType(USP::EndpointType::CDSDK);
}

USP::Client& CSpxUspRecoEngineAdapter::SetUspEndpoint_Custom(std::shared_ptr<ISpxNamedProperties>& properties, USP::Client& client)
{
    UNUSED(properties);

    auto endpoint = properties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_Endpoint));
    m_customEndpoint = true;

    SPX_DBG_TRACE_VERBOSE("%s: Using Custom URL/endpoint: %s", __FUNCTION__, endpoint.c_str());
    return client.SetEndpointUrl(endpoint);
}

USP::Client& CSpxUspRecoEngineAdapter::SetUspEndpoint_Intent(std::shared_ptr<ISpxNamedProperties>& properties, USP::Client& client)
{
    SPX_DBG_TRACE_VERBOSE("%s: Using Intent URL/endpoint...", __FUNCTION__);
    m_customEndpoint = false;

    auto intentRegion = properties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_IntentRegion));
    SPX_IFTRUE_THROW_HR(intentRegion.empty(), SPXERR_INVALID_REGION);

    client.SetEndpointType(USP::EndpointType::Intent)
          .SetIntentRegion(intentRegion);

    auto language = properties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_RecoLanguage));
    if (!language.empty())
    {
        client.SetLanguage(language);
    }
    return client;
}

USP::Client& CSpxUspRecoEngineAdapter::SetUspEndpoint_Translation(std::shared_ptr<ISpxNamedProperties>& properties, USP::Client& client)
{
    SPX_DBG_TRACE_VERBOSE("%s: Using Translation URL/endpoint...", __FUNCTION__);
    m_customEndpoint = false;

    auto region = properties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_Region));
    SPX_IFTRUE_THROW_HR(region.empty(), SPXERR_INVALID_REGION);

    auto fromLang = properties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_RecoLanguage));
    auto toLangs = properties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_TranslationToLanguages));
    SPX_IFTRUE_THROW_HR(fromLang.empty(), SPXERR_INVALID_ARG);
    SPX_IFTRUE_THROW_HR(toLangs.empty(), SPXERR_INVALID_ARG);

    auto voice = properties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_TranslationVoice));

    return client.SetEndpointType(USP::EndpointType::Translation)
        .SetRegion(region)
        .SetTranslationSourceLanguage(fromLang)
        .SetTranslationTargetLanguages(toLangs)
        .SetTranslationVoice(voice);

}

USP::Client& CSpxUspRecoEngineAdapter::SetUspEndpoint_DefaultSpeechService(std::shared_ptr<ISpxNamedProperties>& properties, USP::Client& client)
{
    SPX_DBG_TRACE_VERBOSE("%s: Using Default Speech URL/endpoint...", __FUNCTION__);
    m_customEndpoint = false;

    auto region = properties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_Region));
    SPX_IFTRUE_THROW_HR(region.empty(), SPXERR_INVALID_REGION);

    client.SetEndpointType(USP::EndpointType::Speech)
          .SetRegion(region);

    auto customSpeechModelId = properties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_EndpointId));
    if (!customSpeechModelId.empty())
    {
        return client.SetModelId(customSpeechModelId);
    }

    auto lang = properties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_RecoLanguage));
    if (!lang.empty())
    {
        return client.SetLanguage(lang);
    }

    return client;
}

USP::Client& CSpxUspRecoEngineAdapter::SetUspRecoMode(std::shared_ptr<ISpxNamedProperties>& properties, USP::Client& client)
{
    USP::RecognitionMode mode = USP::RecognitionMode::Interactive;

    // Check mode in the property collection first.
    auto checkHr = GetRecoModeFromProperties(properties, mode);
    SPX_THROW_HR_IF(checkHr, SPX_FAILED(checkHr) && (checkHr != SPXERR_NOT_FOUND));

    // Check mode string in the custom URL if needed.
    if ((checkHr == SPXERR_NOT_FOUND) && m_customEndpoint)
    {
        SPX_DBG_TRACE_VERBOSE("%s: Check mode string in the Custom URL.", __FUNCTION__);

        auto endpoint = properties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_Endpoint));
        SPX_THROW_HR_IF(SPXERR_RUNTIME_ERROR, endpoint.empty());

        checkHr = GetRecoModeFromEndpoint(PAL::ToWString(endpoint), mode);
        SPX_THROW_HR_IF(checkHr, SPX_FAILED(checkHr) && (checkHr != SPXERR_NOT_FOUND));
    }

    m_recoMode = mode;
    return client.SetRecognitionMode(m_recoMode);
}

USP::Client& CSpxUspRecoEngineAdapter::SetUspAuthentication(std::shared_ptr<ISpxNamedProperties>& properties, USP::Client& client)
{
    // Get the properties that indicates what endpoint to use...
    auto uspSubscriptionKey = properties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_Key));
    auto uspAuthToken = properties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceAuthorization_Token));
    auto uspRpsToken = properties->GetStringValue("SPEECH-RpsToken");

    // Use those properties to determine which authentication type to use
    if (!uspSubscriptionKey.empty())
    {
        return client.SetAuthentication(USP::AuthenticationType::SubscriptionKey, uspSubscriptionKey);
    }
    if (!uspAuthToken.empty())
    {
        return client.SetAuthentication(USP::AuthenticationType::AuthorizationToken, uspAuthToken);
    }
    if (!uspRpsToken.empty())
    {
        return client.SetAuthentication(USP::AuthenticationType::SearchDelegationRPSToken, uspRpsToken);
    }

    ThrowInvalidArgumentException("No Authentication parameters were specified.");

    return client; // fixes "not all control paths return a value"
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

USP::OutputFormat CSpxUspRecoEngineAdapter::GetOutputFormat(const ISpxNamedProperties& properties) const
{
    if (!properties.HasStringValue(GetPropertyName(PropertyId::SpeechServiceResponse_RequestDetailedResultTrueFalse)))
        return USP::OutputFormat::Simple;

    auto value = properties.GetStringValue(GetPropertyName(PropertyId::SpeechServiceResponse_RequestDetailedResultTrueFalse));
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

SPXHR CSpxUspRecoEngineAdapter::GetRecoModeFromEndpoint(const std::wstring& endpoint, USP::RecognitionMode& recoMode)
{
    SPXHR hr = SPX_NOERROR;

    if (endpoint.find(L"/interactive/") != std::string::npos)
    {
        recoMode = USP::RecognitionMode::Interactive;
    }
    else if (endpoint.find(L"/conversation/") != std::string::npos)
    {
        recoMode = USP::RecognitionMode::Conversation;
    }
    else if (endpoint.find(L"/dictation/") != std::string::npos)
    {
        recoMode = USP::RecognitionMode::Dictation;
    }
    else
    {
        hr = SPXERR_NOT_FOUND;
    }

    return hr;
}

void CSpxUspRecoEngineAdapter::UspSendSpeechContext()
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
        UspSendMessage(messagePath, speechContext);
    }
}

void CSpxUspRecoEngineAdapter::UspSendMessage(const std::string& messagePath, const std::string &buffer)
{
    SPX_DBG_TRACE_VERBOSE("%s='%s'", messagePath.c_str(), buffer.c_str());
    UspSendMessage(messagePath, (const uint8_t*)buffer.c_str(), buffer.length());
}

void CSpxUspRecoEngineAdapter::UspSendMessage(const std::string& messagePath, const uint8_t* buffer, size_t size)
{
    SPX_DBG_ASSERT(m_uspConnection != nullptr || IsState(UspState::Terminating) || IsState(UspState::Zombie));
    if (!IsState(UspState::Terminating) && !IsState(UspState::Zombie) && m_uspConnection != nullptr)
    {
        m_uspConnection->SendMessage(messagePath, buffer, size);
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

    size_t cbHeader =
        cbTag + cbChunkSize +       // 'RIFF' #size_of_RIFF#
        cbChunkType +               // 'WAVE'
        cbChunkType + cbChunkSize + // 'fmt ' #size_fmt#
        cbFormatChunk +             // actual format
        cbChunkType + cbChunkSize;  // 'data' #size_of_data#

    // Allocate the buffer, and create a ptr we'll use to advance thru the buffer as we're writing stuff into it
    std::unique_ptr<uint8_t[]> buffer(new uint8_t[cbHeader]);
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
    SPX_DBG_ASSERT(cbHeader == size_t(ptr - buffer.get()));
    UspWrite(buffer.get(), cbHeader);
}

void CSpxUspRecoEngineAdapter::UspWrite(const uint8_t* buffer, size_t byteToWrite)
{
    SPX_DBG_TRACE_VERBOSE_IF(byteToWrite == 0, "%s(..., %d)", __FUNCTION__, byteToWrite);

    m_uspAudioByteCount += byteToWrite;
    UspWriteActual(buffer, byteToWrite);
}

void CSpxUspRecoEngineAdapter::UspWriteActual(const uint8_t* buffer, size_t byteToWrite)
{
    SPX_DBG_ASSERT(m_uspConnection != nullptr || IsState(UspState::Terminating) || IsState(UspState::Zombie));
    if (!IsState(UspState::Terminating) && !IsState(UspState::Zombie) && m_uspConnection != nullptr)
    {
        SPX_DBG_TRACE_VERBOSE("%s(..., %d)", __FUNCTION__, byteToWrite);
        m_uspConnection->WriteAudio(buffer, byteToWrite);
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

void CSpxUspRecoEngineAdapter::OnSpeechStartDetected(const USP::SpeechStartDetectedMsg& message)
{
    // The USP message for SpeechStartDetected isn't what it might sound like in all "reco modes" ...
    // * In INTERACTIVE mode, it works as it sounds. It indicates the begging of speech for the "phrase" message that will arrive later
    // * In CONTINUOUS modes, however, it corresponds to the time of the beginning of speech for the FIRST "phrase" message of many inside one turn

    SPX_DBG_TRACE_VERBOSE("Response: Speech.StartDetected message. Speech starts at offset %" PRIu64 " (100ns).\n", message.offset);

    WriteLock_Type writeLock(m_stateMutex);
    if (IsBadState())
    {
        SPX_DBG_ASSERT(writeLock.owns_lock()); // need to keep the lock for trace message
        SPX_DBG_TRACE_VERBOSE("%s: (0x%8x) IGNORING... (audioState/uspState=%d/%d) %s", __FUNCTION__, this, m_audioState, m_uspState, IsState(UspState::Terminating) ? "(USP-TERMINATING)" : "********** USP-UNEXPECTED !!!!!!");
    }
    else if (IsState(UspState::WaitingForPhrase))
    {
        writeLock.unlock(); // calls to site shouldn't hold locks

        SPX_DBG_TRACE_VERBOSE("%s: (0x%8x) site->AdapterDetectedSpeechStart()", __FUNCTION__, this);
        InvokeOnSite([this, &message](const SitePtr& p) { p->AdapterDetectedSpeechStart(this, message.offset); });
    }
    else
    {
        SPX_DBG_ASSERT(writeLock.owns_lock()); // need to keep the lock for trace message
        SPX_DBG_TRACE_WARNING("%s: (0x%8x) UNEXPECTED USP State transition ... (audioState/uspState=%d/%d)", __FUNCTION__, this, m_audioState, m_uspState);
    }
}

void CSpxUspRecoEngineAdapter::OnSpeechEndDetected(const USP::SpeechEndDetectedMsg& message)
{
    SPX_DBG_TRACE_VERBOSE("Response: Speech.EndDetected message. Speech ends at offset %" PRIu64 " (100ns)\n", message.offset);

    WriteLock_Type writeLock(m_stateMutex);
    auto requestMute = ChangeState(AudioState::Sending, AudioState::Mute);

    if (IsBadState())
    {
        SPX_DBG_ASSERT(writeLock.owns_lock()); // need to keep the lock for trace message
        SPX_DBG_TRACE_VERBOSE("%s: (0x%8x) IGNORING... (audioState/uspState=%d/%d) %s", __FUNCTION__, this, m_audioState, m_uspState, IsState(UspState::Terminating) ? "(USP-TERMINATING)" : "********** USP-UNEXPECTED !!!!!!");
        writeLock.unlock(); // calls to site shouldn't hold locks
    }
    else if (IsStateBetweenIncluding(UspState::WaitingForPhrase, UspState::WaitingForTurnEnd) &&
             (IsState(AudioState::Idle) ||
              IsState(AudioState::Mute)))
    {
        writeLock.unlock(); // calls to site shouldn't hold locks

        SPX_DBG_TRACE_VERBOSE("%s: (0x%8x) site->AdapterDetectedSpeechEnd()", __FUNCTION__, this);
        InvokeOnSite([this, &message](const SitePtr& p) { p->AdapterDetectedSpeechEnd(this, message.offset); });
    }
    else
    {
        SPX_DBG_ASSERT(writeLock.owns_lock()); // need to keep the lock for trace message
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

    ReadLock_Type readLock(m_stateMutex);
    if (IsBadState())
    {
        SPX_DBG_ASSERT(readLock.owns_lock()); // need to keep the lock for trace message
        SPX_DBG_TRACE_VERBOSE("%s: (0x%8x) IGNORING... (audioState/uspState=%d/%d) %s", __FUNCTION__, this, m_audioState, m_uspState, IsState(UspState::Terminating) ? "(USP-TERMINATING)" : "********** USP-UNEXPECTED !!!!!!");
    }
    else if (IsState(UspState::WaitingForPhrase))
    {
        readLock.unlock(); // calls to site shouldn't hold locks

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
        SPX_DBG_ASSERT(readLock.owns_lock()); // need to keep the lock for trace message
        SPX_DBG_TRACE_WARNING("%s: (0x%8x) UNEXPECTED USP State transition ... (audioState/uspState=%d/%d)", __FUNCTION__, this, m_audioState, m_uspState);
    }
}

void CSpxUspRecoEngineAdapter::OnSpeechFragment(const USP::SpeechFragmentMsg& message)
{
    SPX_DBG_TRACE_VERBOSE("Response: Speech.Fragment message. Starts at offset %" PRIu64 ", with duration %" PRIu64 " (100ns). Text: %ls\n", message.offset, message.duration, message.text.c_str());
    SPX_DBG_ASSERT(!IsInteractiveMode());

    bool sendIntermediate = false;

    WriteLock_Type writeLock(m_stateMutex);
    if (IsBadState())
    {
        SPX_DBG_ASSERT(writeLock.owns_lock()); // need to keep the lock for trace message
        SPX_DBG_TRACE_VERBOSE("%s: (0x%8x) IGNORING... (audioState/uspState=%d/%d) %s", __FUNCTION__, this, m_audioState, m_uspState, IsState(UspState::Terminating) ? "(USP-TERMINATING)" : "********** USP-UNEXPECTED !!!!!!");
    }
    else if (ChangeState(UspState::WaitingForIntent, UspState::WaitingForIntent2))
    {
        SPX_DBG_TRACE_VERBOSE("%s: Intent never came from service!!", __FUNCTION__);
        sendIntermediate = true;

        writeLock.unlock();
        FireFinalResultLater_WaitingForIntentComplete();

        writeLock.lock();
        ChangeState(UspState::WaitingForIntent2, UspState::WaitingForPhrase);
    }
    else if (IsState(UspState::WaitingForPhrase))
    {
        sendIntermediate = true;
    }
    else
    {
        SPX_DBG_ASSERT(writeLock.owns_lock()); // need to keep the lock for trace message
        SPX_DBG_TRACE_WARNING("%s: (0x%8x) UNEXPECTED USP State transition ... (audioState/uspState=%d/%d)", __FUNCTION__, this, m_audioState, m_uspState);
    }

    if (sendIntermediate)
    {
        writeLock.unlock(); // calls to site shouldn't hold locks
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

    WriteLock_Type writeLock(m_stateMutex);
    if (IsBadState())
    {
        SPX_DBG_ASSERT(writeLock.owns_lock()); // need to keep the lock for trace message
        SPX_DBG_TRACE_VERBOSE("%s: (0x%8x) IGNORING... (audioState/uspState=%d/%d) %s", __FUNCTION__, this, m_audioState, m_uspState, IsState(UspState::Terminating) ? "(USP-TERMINATING)" : "********** USP-UNEXPECTED !!!!!!");
    }
    else if (m_expectIntentResponse &&
             message.recognitionStatus == USP::RecognitionStatus::Success &&
             ChangeState(UspState::WaitingForPhrase, UspState::WaitingForIntent))
    {
        writeLock.unlock(); // calls to site shouldn't hold locks

        SPX_DBG_TRACE_VERBOSE("%s: FireFinalResultLater()", __FUNCTION__);
        FireFinalResultLater(message);
    }
    else if (( IsInteractiveMode() && ChangeState(UspState::WaitingForPhrase, UspState::WaitingForTurnEnd)) ||
             (!IsInteractiveMode() && ChangeState(UspState::WaitingForPhrase, UspState::WaitingForPhrase)))
    {
        writeLock.unlock(); // calls to site shouldn't hold locks

        SPX_DBG_TRACE_VERBOSE("%s: FireFinalResultNow()", __FUNCTION__);
        FireFinalResultNow(message);
    }
    else
    {
        SPX_DBG_ASSERT(writeLock.owns_lock()); // need to keep the lock for warning trace
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

    WriteLock_Type writeLock(m_stateMutex);
    if (IsBadState())
    {
        SPX_DBG_ASSERT(writeLock.owns_lock()); // need to keep the lock for trace warning
        SPX_DBG_TRACE_VERBOSE("%s: IGNORING (Err/Terminating/Zombie)... (audioState/uspState=%d/%d)", __FUNCTION__, m_audioState, m_uspState);
    }
    else if (IsState(UspState::WaitingForPhrase))
    {
        SPX_DBG_ASSERT(writeLock.owns_lock()); // need to keep the lock for trace statement
        {
            writeLock.unlock();
            SPX_DBG_TRACE_SCOPE("Fire final translation result: Creating Result", "FireFinalResul: GetSite()->FireAdapterResult_FinalResult()  complete!");

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
        SPX_DBG_ASSERT(writeLock.owns_lock()); // need to keep the lock for warning trace
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

    WriteLock_Type writeLock(m_stateMutex);
    if (IsBadState())
    {
        SPX_DBG_ASSERT(writeLock.owns_lock()); // need to keep the lock for trace warning
        SPX_DBG_TRACE_VERBOSE("%s: IGNORING (Err/Terminating/Zombie)... (audioState/uspState=%d/%d)", __FUNCTION__, m_audioState, m_uspState);
    }
    else if ((IsInteractiveMode() && ChangeState(UspState::WaitingForPhrase, UspState::WaitingForTurnEnd)) ||
             (!IsInteractiveMode() && ChangeState(UspState::WaitingForPhrase, UspState::WaitingForPhrase)))
    {
        SPX_DBG_TRACE_SCOPE("Fire final translation result: Creating Result", "FireFinalResul: GetSite()->FireAdapterResult_FinalResult()  complete!");

        writeLock.unlock(); // calls to site shouldn't hold locks

        InvokeOnSite([&](const SitePtr& site)
        {
            // Create the result
            auto factory = SpxQueryService<ISpxRecoResultFactory>(site);
            auto result = factory->CreateFinalResult(nullptr, ToReason(message.recognitionStatus), ToNoMatchReason(message.recognitionStatus), ToCancellationReason(message.recognitionStatus), message.text.c_str(), message.offset, message.duration);

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
    else
    {
        SPX_DBG_ASSERT(writeLock.owns_lock()); // need to keep the lock for warning trace
        SPX_DBG_TRACE_WARNING("%s: Unexpected USP State transition (audioState/uspState=%d/%d)", __FUNCTION__, m_audioState, m_uspState);
    }
}

void CSpxUspRecoEngineAdapter::OnTranslationSynthesis(const USP::TranslationSynthesisMsg& message)
{
    SPX_DBG_TRACE_VERBOSE("Response: Translation.Synthesis message. Audio data size: %d\n", message.audioLength);

    SPX_DBG_ASSERT(message.audioLength > 0);
    if (message.audioLength > 0)
    {
        InvokeOnSite([this, &message](const SitePtr& site)
        {
            auto factory = SpxQueryService<ISpxRecoResultFactory>(site);
            auto result = factory->CreateFinalResult(nullptr, ResultReason::SynthesizingAudio, NO_MATCH_REASON_NONE, REASON_CANCELED_NONE, L"", 0, 0);

            // Update our result to be an "TranslationSynthesis" result.
            auto initTranslationResult = SpxQueryInterface<ISpxTranslationSynthesisResultInit>(result);
            initTranslationResult->InitTranslationSynthesisResult(SynthesisStatusCode::Success, message.audioBuffer, message.audioLength, L"");

            site->FireAdapterResult_TranslationSynthesis(this, result);
        });
    }
}

void CSpxUspRecoEngineAdapter::OnTranslationSynthesisEnd(const USP::TranslationSynthesisEndMsg& message)
{
    SPX_DBG_TRACE_VERBOSE("Response: Translation.Synthesis.End message. Status: %d, Reason: %ls\n", (int)message.synthesisStatus, message.failureReason.c_str());

    auto site = GetSite();
    if (!site)
        return;

    auto factory = SpxQueryService<ISpxRecoResultFactory>(site);
    auto result = factory->CreateFinalResult(nullptr, ResultReason::RecognizedSpeech, NO_MATCH_REASON_NONE, REASON_CANCELED_NONE, L"", 0, 0);

    // Update our result to be an "TranslationSynthesis" result.
    auto initTranslationResult = SpxQueryInterface<ISpxTranslationSynthesisResultInit>(result);
    SynthesisStatusCode status;
    switch (message.synthesisStatus)
    {
    case ::USP::SynthesisStatus::Success: // Indicates the end of synthesis.
        status = SynthesisStatusCode::SynthesisEnd;
        break;
    case ::USP::SynthesisStatus::Error:
        status = SynthesisStatusCode::Error;
        break;
    case ::USP::SynthesisStatus::InvalidMessage:
        // The failureReason contains additional error messages.
        // Todo: have better error handling for different statuses.
        status = SynthesisStatusCode::Error;
        break;
    default:
        status = SynthesisStatusCode::Error;
        SPX_THROW_HR(SPXERR_RUNTIME_ERROR);
        break;
    }
    initTranslationResult->InitTranslationSynthesisResult(status, nullptr, 0, message.failureReason);

    // Fire the result
    site->FireAdapterResult_TranslationSynthesis(this, result);
}

void CSpxUspRecoEngineAdapter::OnTurnStart(const USP::TurnStartMsg& message)
{
    SPX_DBG_TRACE_VERBOSE("Response: Turn.Start message. Context.ServiceTag: %s\n", message.contextServiceTag.c_str());
    SPX_DBG_TRACE_VERBOSE("%s: this=0x%8x", __FUNCTION__, this);

    WriteLock_Type writeLock(m_stateMutex);
    if (IsBadState())
    {
        SPX_DBG_ASSERT(writeLock.owns_lock()); // need to keep the lock for trace message
        SPX_DBG_TRACE_VERBOSE("%s: (0x%8x) IGNORING... (audioState/uspState=%d/%d) %s", __FUNCTION__, this, m_audioState, m_uspState, IsState(UspState::Terminating) ? "(USP-TERMINATING)" : "********** USP-UNEXPECTED !!!!!!");
    }
    else if (ChangeState(UspState::WaitingForTurnStart, UspState::WaitingForPhrase))
    {
        writeLock.unlock(); // calls to site shouldn't hold locks

        SPX_DBG_TRACE_VERBOSE("%s: site->AdapterStartedTurn()", __FUNCTION__);
        SPX_DBG_ASSERT(GetSite());
        InvokeOnSite([this, &message](const SitePtr& p) { p->AdapterStartedTurn(this, message.contextServiceTag); });
    }
    else
    {
        SPX_DBG_ASSERT(writeLock.owns_lock()); // need to keep the lock for trace message
        SPX_DBG_TRACE_WARNING("%s: (0x%8x) UNEXPECTED USP State transition ... (audioState/uspState=%d/%d)", __FUNCTION__, this, m_audioState, m_uspState);
    }
}

void CSpxUspRecoEngineAdapter::OnTurnEnd(const USP::TurnEndMsg& message)
{
    SPX_DBG_TRACE_SCOPE("CSpxUspRecoEngineAdapter::OnTurnEnd ... started... ", "CSpxUspRecoEngineAdapter::OnTurnEnd ... DONE!");
    SPX_DBG_TRACE_VERBOSE("Response: Turn.End message.\n");
    UNUSED(message);

    WriteLock_Type writeLock(m_stateMutex);
    auto adapterTurnStopped = false;

    auto prepareReady = !m_singleShot &&
        (ChangeState(AudioState::Sending, AudioState::Ready) ||
         ChangeState(AudioState::Mute, AudioState::Ready));

    auto requestMute = m_singleShot && ChangeState(AudioState::Sending, AudioState::Mute);

    if (IsBadState())
    {
        SPX_DBG_ASSERT(writeLock.owns_lock()); // need to keep the lock for trace message
        SPX_DBG_TRACE_VERBOSE("%s: (0x%8x) IGNORING... (audioState/uspState=%d/%d) %s", __FUNCTION__, this, m_audioState, m_uspState, IsState(UspState::Terminating) ? "(USP-TERMINATING)" : "********** USP-UNEXPECTED !!!!!!");
    }
    else if (( IsInteractiveMode() && ChangeState(UspState::WaitingForTurnEnd, UspState::Idle)) ||
             (!IsInteractiveMode() && ChangeState(UspState::WaitingForPhrase, UspState::Idle)))
    {
        adapterTurnStopped = true;
    }
    else if (ChangeState(UspState::WaitingForIntent, UspState::WaitingForIntent2))
    {
        SPX_DBG_TRACE_VERBOSE("%s: Intent never came from service!!", __FUNCTION__);
        adapterTurnStopped = true;

        writeLock.unlock();
        FireFinalResultLater_WaitingForIntentComplete();

        writeLock.lock();
        ChangeState(UspState::WaitingForIntent2, UspState::Idle);
    }
    else
    {
        SPX_DBG_ASSERT(writeLock.owns_lock()); // need to keep the lock for trace message
        SPX_DBG_TRACE_WARNING("%s: (0x%8x) UNEXPECTED USP State transition ... (audioState/uspState=%d/%d)", __FUNCTION__, this, m_audioState, m_uspState);
    }

    if (prepareReady && !IsBadState())
    {
        SPX_DBG_TRACE_VERBOSE("%s: PrepareAudioReadyState()", __FUNCTION__);
        PrepareAudioReadyState();

        SPX_DBG_TRACE_VERBOSE("%s: site->AdapterRequestingAudioMute(false) ... (audioState/uspState=%d/%d)", __FUNCTION__, m_audioState, m_uspState);

        // It's safe to call AdapterRequestingAudioMute() while locked... it will not call us back...
        InvokeOnSite([this](const SitePtr& p) { p->AdapterRequestingAudioMute(this, false); });
    }

    if (adapterTurnStopped && ShouldResetAfterTurnStopped())
    {
        ResetAfterTurnStopped();
    }
    writeLock.unlock();

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

void CSpxUspRecoEngineAdapter::OnError(bool isTransport, const std::string& error)
{
    SPX_DBG_TRACE_VERBOSE("Response: On Error: %s.\n", error.c_str());

    WriteLock_Type writeLock(m_stateMutex);
    if (IsBadState())
    {
        SPX_DBG_ASSERT(writeLock.owns_lock()); // need to keep the lock for trace message
        SPX_DBG_TRACE_VERBOSE("%s: (0x%8x) IGNORING... (audioState/uspState=%d/%d) %s", __FUNCTION__, this, m_audioState, m_uspState, IsState(UspState::Terminating) ? "(USP-TERMINATING)" : "********** USP-UNEXPECTED !!!!!!");
    }
    else if (ChangeState(UspState::Error))
    {
        writeLock.unlock(); // calls to site shouldn't hold locks

        SPX_DBG_TRACE_VERBOSE("%s: site->Error() ... error='%s'", __FUNCTION__, error.c_str());
        InvokeOnSite([this, error, isTransport](const SitePtr& p) { p->Error(this,  std::make_shared<SpxRecoEngineAdapterError>(isTransport, error)); });
    }
    else
    {
       SPX_DBG_ASSERT(writeLock.owns_lock()); // need to keep the lock for trace message
       SPX_DBG_TRACE_WARNING("%s: (0x%8x) UNEXPECTED USP State transition ... (audioState/uspState=%d/%d)", __FUNCTION__, this, m_audioState, m_uspState);
    }
}

void CSpxUspRecoEngineAdapter::OnUserMessage(const USP::UserMsg& msg)
{
    SPX_DBG_TRACE_VERBOSE("Response: Usp User Message: %s, content-type=%s", msg.path.c_str(), msg.contentType.c_str());

    if (msg.path == "response")
    {
        WriteLock_Type writeLock(m_stateMutex);
        if (ChangeState(UspState::WaitingForIntent, UspState::WaitingForIntent2))
        {
            writeLock.unlock(); // calls to site shouldn't hold locks

            std::string luisJson((const char*)msg.buffer, msg.size);
            SPX_DBG_TRACE_VERBOSE("USP User Message: response; luisJson='%s'", luisJson.c_str());
            FireFinalResultLater_WaitingForIntentComplete(luisJson);

            writeLock.lock();
            ChangeState(UspState::WaitingForIntent2, IsInteractiveMode() ? UspState::WaitingForTurnEnd : UspState::WaitingForPhrase);
        }
        else
        {
            SPX_DBG_ASSERT(writeLock.owns_lock()); // need to keep the lock for trace message
            SPX_DBG_TRACE_WARNING("%s: (0x%8x) UNEXPECTED USP State transition ... (audioState/uspState=%d/%d)", __FUNCTION__, this, m_audioState, m_uspState);
        }
    }
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
    const static std::map<USP::RecognitionStatus, ResultReason> reasonMap = {
        { USP::RecognitionStatus::Success, ResultReason::RecognizedSpeech },
        { USP::RecognitionStatus::NoMatch, ResultReason::NoMatch },
        { USP::RecognitionStatus::Error, ResultReason::Canceled },
        { USP::RecognitionStatus::InitialSilenceTimeout, ResultReason::NoMatch },
        { USP::RecognitionStatus::InitialBabbleTimeout, ResultReason::NoMatch },
    };

    auto item = reasonMap.find(uspRecognitionStatus);
    if (item == reasonMap.end())
    {
        SPX_TRACE_ERROR("Unexpected recognition status %d when converting to ResultReason.", uspRecognitionStatus);
        SPX_THROW_HR(SPXERR_RUNTIME_ERROR);
    }
    return item->second;
}

CancellationReason CSpxUspRecoEngineAdapter::ToCancellationReason(USP::RecognitionStatus uspRecognitionStatus)
{
    const static std::map<USP::RecognitionStatus, CancellationReason> reasonMap = {
        { USP::RecognitionStatus::Success, REASON_CANCELED_NONE },
        { USP::RecognitionStatus::NoMatch, REASON_CANCELED_NONE },
        { USP::RecognitionStatus::Error, CancellationReason::Error },
        { USP::RecognitionStatus::InitialSilenceTimeout, REASON_CANCELED_NONE },
        { USP::RecognitionStatus::InitialBabbleTimeout, REASON_CANCELED_NONE },
    };

    auto item = reasonMap.find(uspRecognitionStatus);
    if (item == reasonMap.end())
    {
        SPX_TRACE_ERROR("Unexpected recognition status %d when converting to CancellationReason.", uspRecognitionStatus);
        SPX_THROW_HR(SPXERR_RUNTIME_ERROR);
    }
    return item->second;
}

NoMatchReason CSpxUspRecoEngineAdapter::ToNoMatchReason(USP::RecognitionStatus uspRecognitionStatus)
{
    const static std::map<USP::RecognitionStatus, NoMatchReason> reasonMap = {
        { USP::RecognitionStatus::Success, NO_MATCH_REASON_NONE },
        { USP::RecognitionStatus::NoMatch, NoMatchReason::NotRecognized },
        { USP::RecognitionStatus::Error, NO_MATCH_REASON_NONE },
        { USP::RecognitionStatus::InitialSilenceTimeout, NoMatchReason::InitialSilenceTimeout },
        { USP::RecognitionStatus::InitialBabbleTimeout, NoMatchReason::InitialBabbleTimeout },
    };

    auto item = reasonMap.find(uspRecognitionStatus);
    if (item == reasonMap.end())
    {
        SPX_TRACE_ERROR("Unexpected recognition status %d when converting to CancellationReason.", uspRecognitionStatus);
        SPX_THROW_HR(SPXERR_RUNTIME_ERROR);
    }
    return item->second;
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
        auto result = factory->CreateFinalResult(nullptr, ToReason(message.recognitionStatus), ToNoMatchReason(message.recognitionStatus), ToCancellationReason(message.recognitionStatus), message.displayText.c_str(), message.offset, message.duration);

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

void CSpxUspRecoEngineAdapter::PrepareFirstAudioReadyState(SPXWAVEFORMATEX* format)
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
