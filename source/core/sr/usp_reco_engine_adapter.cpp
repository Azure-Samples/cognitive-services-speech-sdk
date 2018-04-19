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
#include "service_helpers.h"
#include "named_properties_constants.h"
#include "exception.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


int CSpxUspRecoEngineAdapter::m_saveToWavCounter = 0;


CSpxUspRecoEngineAdapter::CSpxUspRecoEngineAdapter() :
    m_audioState(AudioState::Idle),
    m_uspState(UspState::Idle),
    m_servicePreferedBufferSize(0),
    m_bytesInBuffer(0),
    m_ptrIntoBuffer(nullptr),
    m_bytesLeftInBuffer(0)
{
}

CSpxUspRecoEngineAdapter::~CSpxUspRecoEngineAdapter()
{
    SPX_DBG_TRACE_FUNCTION();
}

void CSpxUspRecoEngineAdapter::Init()
{
    SPX_IFTRUE_THROW_HR(GetSite() == nullptr, SPXERR_UNINITIALIZED);
    SPX_IFTRUE_THROW_HR(m_handle != nullptr, SPXERR_ALREADY_INITIALIZED);

    WriteLock_Type writeLock(m_stateMutex);
    if (ChangeState(AudioState::Idle, UspState::Idle, AudioState::Idle, UspState::ReadyForFormat))
    {
        writeLock.unlock();

        UspInitialize();
        SaveToWavInit();
    }
    else
    {
        SPX_DBG_ASSERT(writeLock.owns_lock()); // need to keep the lock for trace warning
        SPX_TRACE_WARNING("%s: Unexpected USP State transition (audioState/uspState=%d/%d)", __FUNCTION__, m_audioState, m_uspState);
    }
}

void CSpxUspRecoEngineAdapter::Term()
{
    SPX_DBG_TRACE_SCOPE("Terminating CSpxUspRecoEngineAdapter...", "Terminating CSpxUspRecoEngineAdapter... Done!");

    WriteLock_Type writeLock(m_stateMutex);
    if (ChangeState(UspState::Terminating))
    {
        writeLock.unlock();

        SPX_DBG_TRACE_VERBOSE("%s(0x%x)", __FUNCTION__, m_handle.get());
        m_handle.reset();
        SaveToWavClose();

        writeLock.lock();
        ChangeState(UspState::Zombie);
    }
    else
    {
        SPX_DBG_ASSERT(writeLock.owns_lock()); // need to keep the lock for trace warning
        SPX_TRACE_WARNING("%s: Unexpected USP State transition (audioState/uspState=%d/%d)", __FUNCTION__, m_audioState, m_uspState);
    }
}

void CSpxUspRecoEngineAdapter::SetFormat(WAVEFORMATEX* pformat)
{
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
    SPX_IFTRUE_THROW_HR(m_handle == nullptr, SPXERR_UNINITIALIZED);

    WriteLock_Type writeLock(m_stateMutex);
    if (IsState(UspState::Error) || IsState(UspState::Terminating) || IsState(UspState::Zombie))
    {
        SPX_DBG_ASSERT(writeLock.owns_lock()); // need to keep the lock for trace warning
        SPX_DBG_TRACE_VERBOSE("%s: IGNORING (Err/Terminating/Zombie)... (audioState/uspState=%d/%d)", __FUNCTION__, m_audioState, m_uspState);
    }
    else if (pformat != nullptr && ChangeState(AudioState::Idle, UspState::ReadyForFormat, AudioState::ReadyToProcess, UspState::SendingAudioData))
    {
        writeLock.unlock();

        UspSendSpeechContext();
        UspWriteFormat(pformat);
        m_servicePreferedBufferSize = (size_t)pformat->nSamplesPerSec * pformat->nBlockAlign * m_servicePreferedMilliseconds / 1000;
    }
    else if (pformat == nullptr && IsState(AudioState::Idle) &&
             (IsState(UspState::SendingAudioData) ||
              IsStateBetween(UspState::SendingAudioData, UspState::TurnEnded)))
    {
        SPX_DBG_ASSERT(writeLock.owns_lock()); // need to keep the lock for trace warning
        SPX_DBG_TRACE_VERBOSE("%s: Already Idle; Waiting for uspStates to complete ... (audioState/uspState=%d/%d)", __FUNCTION__, m_audioState, m_uspState);
    }
    else if (pformat == nullptr &&
             (ChangeState(AudioState::ReadyToProcess, AudioState::WaitingForDone) ||
              ChangeState(AudioState::ProcessingAudio, AudioState::WaitingForDone) ||
              ChangeState(AudioState::Paused, AudioState::WaitingForDone)))
    {
        writeLock.unlock();

        UspWrite_Flush();
        m_servicePreferedBufferSize = 0;
    }
    else
    {
        SPX_DBG_ASSERT(writeLock.owns_lock()); // need to keep the lock for trace warning
        SPX_TRACE_WARNING("%s: Unexpected USP State transition (audioState/uspState=%d/%d)", __FUNCTION__, m_audioState, m_uspState);
    }
}

void CSpxUspRecoEngineAdapter::ProcessAudio(AudioData_Type data, uint32_t size)
{
    SPX_DBG_TRACE_VERBOSE_IF(1, "%s: size=%d", __FUNCTION__, size);

    WriteLock_Type writeLock(m_stateMutex);
    if (IsState(UspState::Error) || IsState(UspState::Terminating) || IsState(UspState::Zombie))
    {
        SPX_DBG_ASSERT(writeLock.owns_lock()); // need to keep the lock for trace warning
        SPX_DBG_TRACE_VERBOSE("%s: IGNORING (Err/Terminating/Zombie)... (audioState/uspState=%d/%d)", __FUNCTION__, m_audioState, m_uspState);
    }
    else if (size == 0 && ChangeState(AudioState::ReadyToProcess, UspState::SendingAudioData, AudioState::Idle, UspState::SendingAudioData))
    {
        SPX_DBG_ASSERT(writeLock.owns_lock()); // need to keep the lock for trace warning
        SPX_DBG_TRACE_VERBOSE("%s: Ignoring audio; size == 0 (audioState/uspState=%d/%d)", __FUNCTION__, m_audioState, m_uspState);
    }
    else if (IsState(AudioState::ProcessingAudio) || ChangeState(AudioState::ReadyToProcess, AudioState::ProcessingAudio))
    {
        writeLock.unlock();
        UspWrite(data.get(), size);
    }
    else if (IsState(AudioState::Paused))
    {
        SPX_DBG_ASSERT(writeLock.owns_lock()); // need to keep the lock for trace warning
        SPX_DBG_TRACE_VERBOSE("%s: Ignoring audio (audioState/uspState=%d/%d)", __FUNCTION__, m_audioState, m_uspState);
        // TODO: RobCh: Deal with paused correctly (store the data until we need it, and then when we transition to ProcessingAudio, send it)
    }
    else
    {
        SPX_DBG_ASSERT(writeLock.owns_lock()); // need to keep the lock for trace warning
        SPX_TRACE_WARNING("%s: Ignoring audio (audioState/uspState=%d/%d)", __FUNCTION__, m_audioState, m_uspState);
    }
}

void CSpxUspRecoEngineAdapter::UspInitialize()
{
    SPX_DBG_TRACE_VERBOSE("%s(0x%x)", __FUNCTION__, m_handle.get());
    
    USP::Client clientConfig(*this, USP::EndpointType::BingSpeech);

    for (auto f : { 
            &CSpxUspRecoEngineAdapter::SetUspEndpoint,
            &CSpxUspRecoEngineAdapter::SetUspRecoMode,
            &CSpxUspRecoEngineAdapter::SetUspLanguage,
            &CSpxUspRecoEngineAdapter::SetUspModelId,
            &CSpxUspRecoEngineAdapter::SetUspAuthentication
        }) {
        (this->*f)(clientConfig);
    }

    // Finally ... Connect to the service
    m_handle = clientConfig.Connect();
}

USP::Client& CSpxUspRecoEngineAdapter::SetUspEndpoint(USP::Client& client)
{
    // Get the named property service...
    auto properties = SpxQueryService<ISpxNamedProperties>(GetSite());
    SPX_IFTRUE_THROW_HR(properties == nullptr, SPXERR_UNEXPECTED_USP_SITE_FAILURE);

    auto customSpeechModelId = properties->GetStringValue(g_SPEECH_ModelId);
    if (!customSpeechModelId.empty())                       // Use the Custom Recognition Intelligent Service
    {
        return client.SetEndpointType(USP::EndpointType::Cris);
    }

    auto endpoint = properties->GetStringValue(g_SPEECH_Endpoint);
    if (PAL::wcsicmp(endpoint.c_str(), L"CORTANA") == 0)    // Use the CORTANA SDK endpoint
    {
        return client.SetEndpointType(USP::EndpointType::CDSDK);
    }

    if (!endpoint.empty())                                  // Use the SPECIFIED endpoint
    {
        SPX_DBG_TRACE_VERBOSE("Using Custom URL: %ls", endpoint.c_str());
        return client.SetEndpointUrl(PAL::ToString(endpoint));
    }
    
     // Otherwise ... Use the default SPEECH endpoints
    return client.SetEndpointType(USP::EndpointType::BingSpeech);
}

USP::Client& CSpxUspRecoEngineAdapter::SetUspRecoMode(USP::Client& client)
{
    // Get the named properties service...
    auto properties = SpxQueryService<ISpxNamedProperties>(GetSite());
    SPX_IFTRUE_THROW_HR(properties == nullptr, SPXERR_UNEXPECTED_USP_SITE_FAILURE);

    // Get the property that indicates what reco mode to use...
    auto value = properties->GetStringValue(g_SPEECH_RecoMode);

    // Convert that value to the appropriate UspRecognitionMode...
    if (value.empty() || PAL::wcsicmp(value.c_str(), L"INTERACTIVE") == 0)
    {
        return client.SetRecognitionMode(USP::RecognitionMode::Interactive);
    }

    if (PAL::wcsicmp(value.c_str(), L"CONVERSATION") == 0)
    {
        return client.SetRecognitionMode(USP::RecognitionMode::Conversation);
    }

    if (PAL::wcsicmp(value.c_str(), L"DICTATION") == 0)
    {
        return client.SetRecognitionMode(USP::RecognitionMode::Dictation);
    }

    SPX_DBG_ASSERT_WITH_MESSAGE(false, "Did someone add a new value to the USP::RecognitionMode enumeration?");

    ThrowInvalidArgumentException("Unknown RecognitionMode value " + PAL::ToString(value));

    return client; // to make compiler happy.
}

USP::Client& CSpxUspRecoEngineAdapter::SetUspLanguage(USP::Client& client)
{
    // Get the named properties service...
    auto properties = SpxQueryService<ISpxNamedProperties>(GetSite());
    SPX_IFTRUE_THROW_HR(properties == nullptr, SPXERR_UNEXPECTED_USP_SITE_FAILURE);

    if (!properties->HasStringValue(g_SPEECH_RecoLanguage))
    {
        return client;
    }

    // Get the property that indicates what language to use...
    auto value = properties->GetStringValue(g_SPEECH_RecoLanguage);
    return client.SetLanguage(PAL::ToString(value));
}

USP::Client&  CSpxUspRecoEngineAdapter::SetUspModelId(USP::Client& client)
{
    // Get the named properties service...
    auto properties = SpxQueryService<ISpxNamedProperties>(GetSite());
    SPX_IFTRUE_THROW_HR(properties == nullptr, SPXERR_UNEXPECTED_USP_SITE_FAILURE);

    if (!properties->HasStringValue(g_SPEECH_ModelId))
    {
        return client;
    }

    // Get the property that indicates what model to use...
    auto value = properties->GetStringValue(g_SPEECH_ModelId);
    return client.SetModelId(PAL::ToString(value));
}

USP::Client&  CSpxUspRecoEngineAdapter::SetUspAuthentication(USP::Client& client)
{
    // Get the named properties service...
    auto properties = SpxQueryService<ISpxNamedProperties>(GetSite());
    SPX_IFTRUE_THROW_HR(properties == nullptr, SPXERR_UNEXPECTED_USP_SITE_FAILURE);

    // Get the properties that indicates what endpoint to use...
    auto uspSubscriptionKey = properties->GetStringValue(g_SPEECH_SubscriptionKey);
    auto uspAuthToken = properties->GetStringValue(g_SPEECH_AuthToken);
    auto uspRpsToken = properties->GetStringValue(g_SPEECH_RpsToken);

    // Use those properties to determine which authentication type to use
    if (!uspSubscriptionKey.empty())
    {
        return client.SetAuthentication(USP::AuthenticationType::SubscriptionKey, PAL::ToString(uspSubscriptionKey));
    }
    if (!uspAuthToken.empty())
    {
        return client.SetAuthentication(USP::AuthenticationType::AuthorizationToken, PAL::ToString(uspAuthToken));
    }
    if (!uspRpsToken.empty())
    {
        return client.SetAuthentication(USP::AuthenticationType::SearchDelegationRPSToken, PAL::ToString(uspRpsToken));
    }

    ThrowInvalidArgumentException("No Authentication parameters were specified.");
    
    return client; // fixes "not all control paths return a value"
}

void CSpxUspRecoEngineAdapter::UspSendSpeechContext()
{
    // Get the Dgi json payload
    std::list<std::string> listenForList = GetListenForListFromSite();
    auto listenForJson = GetDgiJsonFromListenForList(listenForList);

    // Get the intent payload
    std::string provider, id, key;
    GetIntentInfoFromSite(provider, id, key);
    auto intentJson = GetIntentJsonFromIntentInfo(provider, id, key);

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
    m_handle->SendMessage(messagePath, buffer, size);
}

void CSpxUspRecoEngineAdapter::UspWriteFormat(WAVEFORMATEX* pformat)
{
    static const uint16_t cbTag = 4;
    static const uint16_t cbChunkType = 4;
    static const uint16_t cbChunkSize = 4;

    uint32_t cbFormatChunk = sizeof(WAVEFORMAT) + pformat->cbSize;
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

    // The 'fmt ' chunk (consists of 'fmt ' followed by the total size of the WAVEFORMAT(EX)(TENSIBLE), followed by the WAVEFORMAT(EX)(TENSIBLE)
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

    auto fn = !m_fUseBufferedImplementation || m_servicePreferedBufferSize == 0
        ? &CSpxUspRecoEngineAdapter::UspWrite_Actual
        : &CSpxUspRecoEngineAdapter::UspWrite_Buffered;

    (this->*fn)(buffer, byteToWrite);
}

void CSpxUspRecoEngineAdapter::UspWrite_Actual(const uint8_t* buffer, size_t byteToWrite)
{
    SPX_DBG_TRACE_VERBOSE("%s(..., %d)", __FUNCTION__, byteToWrite);
    m_handle->WriteAudio(buffer, byteToWrite);
    SaveToWavWrite(buffer, byteToWrite);
}

void CSpxUspRecoEngineAdapter::UspWrite_Buffered(const uint8_t* buffer, size_t bytesToWrite)
{
    bool flushBuffer = bytesToWrite == 0;

    if (m_buffer.get() == nullptr)
    {
        m_buffer = SpxAllocSharedUint8Buffer(m_servicePreferedBufferSize);
        m_bytesInBuffer = m_servicePreferedBufferSize;

        m_ptrIntoBuffer = m_buffer.get();
        m_bytesLeftInBuffer = m_bytesInBuffer;
    }

    for (;;)
    {
        if (flushBuffer || (m_bytesInBuffer > 0 && m_bytesLeftInBuffer == 0))
        {
            auto bytesToFlush = m_bytesInBuffer - m_bytesLeftInBuffer;
            UspWrite_Actual(m_buffer.get(), bytesToFlush);

            m_bytesLeftInBuffer = m_bytesInBuffer;
            m_ptrIntoBuffer = m_buffer.get();
        }

        if (flushBuffer)
        {
            m_buffer = nullptr;
            m_bytesInBuffer = 0;
            m_ptrIntoBuffer = nullptr;
            m_bytesLeftInBuffer = 0;
        }

        if (bytesToWrite == 0)
        {
            break;
        }

        size_t bytesThisLoop = std::min(bytesToWrite, m_bytesLeftInBuffer);
        std::memcpy(m_ptrIntoBuffer, buffer, bytesThisLoop);

        m_ptrIntoBuffer += bytesThisLoop;
        m_bytesLeftInBuffer -= bytesThisLoop;
        bytesToWrite -= bytesThisLoop;
    }
}

void CSpxUspRecoEngineAdapter::UspWrite_Flush()
{
    m_handle->FlushAudio();
}

void CSpxUspRecoEngineAdapter::OnSpeechStartDetected(const USP::SpeechStartDetectedMsg& message)
{
    SPX_DBG_TRACE_VERBOSE("Response: Speech.StartDetected message. Speech starts at offset %" PRIu64 " (100ns).\n", message.offset);

    WriteLock_Type writeLock(m_stateMutex);
    if (IsState(UspState::Error) || IsState(UspState::Terminating) || IsState(UspState::Zombie))
    {
        SPX_DBG_ASSERT(writeLock.owns_lock()); // need to keep the lock for trace warning
        SPX_DBG_TRACE_VERBOSE("%s: IGNORING (Err/Terminating/Zombie)... (audioState/uspState=%d/%d)", __FUNCTION__, m_audioState, m_uspState);
    }
    else if (ChangeState(AudioState::ProcessingAudio, UspState::TurnStarted, AudioState::ProcessingAudio, UspState::SpeechStarted) ||
             ChangeState(AudioState::WaitingForDone, UspState::TurnStarted, AudioState::WaitingForDone, UspState::SpeechStarted) ||
             ChangeState(AudioState::Idle, UspState::TurnStarted, AudioState::Idle, UspState::SpeechStarted))
    {
        writeLock.unlock();

        SPX_DBG_ASSERT(GetSite());
        GetSite()->SpeechStartDetected(this, message.offset);
    }
    else
    {
        SPX_DBG_ASSERT(writeLock.owns_lock()); // need to keep the lock for trace warning
        SPX_TRACE_WARNING("%s: Unexpected USP State transition (audioState/uspState=%d/%d)", __FUNCTION__, m_audioState, m_uspState);
    }
}

void CSpxUspRecoEngineAdapter::OnSpeechEndDetected(const USP::SpeechEndDetectedMsg& message)
{
    SPX_DBG_TRACE_VERBOSE("Response: Speech.EndDetected message. Speech ends at offset %" PRIu64 " (100ns)\n", message.offset);

    WriteLock_Type writeLock(m_stateMutex);
    if (IsState(UspState::Error) || IsState(UspState::Terminating) || IsState(UspState::Zombie))
    {
        SPX_DBG_ASSERT(writeLock.owns_lock()); // need to keep the lock for trace warning
        SPX_DBG_TRACE_VERBOSE("%s: IGNORING (Err/Terminating/Zombie)... (audioState/uspState=%d/%d)", __FUNCTION__, m_audioState, m_uspState);
    }
    else if (ChangeState(UspState::SpeechEnded))
    {
        writeLock.unlock();

        SPX_DBG_ASSERT(GetSite());
        GetSite()->SpeechEndDetected(this, message.offset);
    }
    else
    {
        SPX_DBG_ASSERT(writeLock.owns_lock()); // need to keep the lock for trace warning
        SPX_TRACE_WARNING("%s: Unexpected USP State transition (audioState/uspState=%d/%d)", __FUNCTION__, m_audioState, m_uspState);
    }
}

void CSpxUspRecoEngineAdapter::OnSpeechHypothesis(const USP::SpeechHypothesisMsg& message)
{
    SPX_DBG_TRACE_VERBOSE("Response: Speech.Hypothesis message. Starts at offset %" PRIu64 ", with duration %" PRIu64 " (100ns). Text: %ls\n", message.offset, message.duration, message.text.c_str());

    WriteLock_Type writeLock(m_stateMutex);
    if (IsState(UspState::Error) || IsState(UspState::Terminating) || IsState(UspState::Zombie))
    {
        SPX_DBG_ASSERT(writeLock.owns_lock()); // need to keep the lock for trace warning
        SPX_DBG_TRACE_VERBOSE("%s: IGNORING (Err/Terminating/Zombie)... (audioState/uspState=%d/%d)", __FUNCTION__, m_audioState, m_uspState);
    }
    else if (IsState(UspState::SpeechHypothesis) ||
             ChangeState(AudioState::ProcessingAudio, UspState::SpeechStarted, AudioState::ProcessingAudio, UspState::SpeechHypothesis) ||
             ChangeState(AudioState::WaitingForDone, UspState::SpeechStarted, AudioState::WaitingForDone, UspState::SpeechHypothesis) ||
             ChangeState(AudioState::Idle, UspState::SpeechStarted, AudioState::Idle, UspState::SpeechHypothesis))
    {
        writeLock.unlock();

        SPX_DBG_ASSERT(GetSite());
        auto factory = SpxQueryService<ISpxRecoResultFactory>(GetSite());

        auto result = factory->CreateIntermediateResult(nullptr, message.text.c_str(), ResultType::Speech);
        GetSite()->IntermediateRecoResult(this, message.offset, result);
    }
    else
    {
        SPX_DBG_ASSERT(writeLock.owns_lock()); // need to keep the lock for trace warning
        SPX_TRACE_WARNING("%s: Unexpected USP State transition (audioState/uspState=%d/%d)", __FUNCTION__, m_audioState, m_uspState);
    }
}

void CSpxUspRecoEngineAdapter::OnSpeechFragment(const USP::SpeechFragmentMsg& message)
{
    SPX_DBG_TRACE_VERBOSE("Response: Speech.Fragment message. Starts at offset %" PRIu64 ", with duration %" PRIu64 " (100ns). Text: %ls\n", message.offset, message.duration, message.text.c_str());

    WriteLock_Type writeLock(m_stateMutex);
    if (IsState(UspState::Error) || IsState(UspState::Terminating) || IsState(UspState::Zombie))
    {
        SPX_DBG_ASSERT(writeLock.owns_lock()); // need to keep the lock for trace warning
        SPX_DBG_TRACE_VERBOSE("%s: IGNORING (Err/Terminating/Zombie)... (audioState/uspState=%d/%d)", __FUNCTION__, m_audioState, m_uspState);
    }
    else if (IsState(UspState::SpeechHypothesis) ||
             ChangeState(AudioState::ProcessingAudio, UspState::SpeechStarted, AudioState::ProcessingAudio, UspState::SpeechHypothesis) ||
             ChangeState(AudioState::WaitingForDone, UspState::SpeechStarted, AudioState::WaitingForDone, UspState::SpeechHypothesis) ||
             ChangeState(AudioState::Idle, UspState::SpeechStarted, AudioState::Idle, UspState::SpeechHypothesis))
    {
        writeLock.unlock();

        SPX_DBG_ASSERT(GetSite());
        auto factory = SpxQueryService<ISpxRecoResultFactory>(GetSite());

        auto result = factory->CreateIntermediateResult(nullptr, message.text.c_str(), ResultType::Speech);
        GetSite()->IntermediateRecoResult(this, message.offset, result);
    }
    else
    {
        SPX_DBG_ASSERT(writeLock.owns_lock()); // need to keep the lock for trace warning
        SPX_TRACE_WARNING("%s: Unexpected USP State transition (audioState/uspState=%d/%d)", __FUNCTION__, m_audioState, m_uspState);
    }
}

void CSpxUspRecoEngineAdapter::OnSpeechPhrase(const USP::SpeechPhraseMsg& message)
{
    SPX_DBG_TRACE_VERBOSE("Response: Speech.Phrase message. Status: %d, Text: %ls, starts at %" PRIu64 ", with duration %" PRIu64 " (100ns).\n", message.recognitionStatus, message.displayText.c_str(), message.offset, message.duration);

    WriteLock_Type writeLock(m_stateMutex);
    if (IsState(UspState::Error) || IsState(UspState::Terminating) || IsState(UspState::Zombie))
    {
        SPX_DBG_ASSERT(writeLock.owns_lock()); // need to keep the lock for trace warning
        SPX_DBG_TRACE_VERBOSE("%s: IGNORING (Err/Terminating/Zombie)... (audioState/uspState=%d/%d)", __FUNCTION__, m_audioState, m_uspState);
    }
    else if (ChangeState(AudioState::ProcessingAudio, UspState::SpeechEnded, AudioState::ProcessingAudio, UspState::ReceivedSpeechResult) ||
             ChangeState(AudioState::WaitingForDone, UspState::SpeechEnded, AudioState::WaitingForDone, UspState::ReceivedSpeechResult) ||
             ChangeState(AudioState::Idle, UspState::SpeechEnded, AudioState::Idle, UspState::ReceivedSpeechResult))
    {
        SPX_DBG_ASSERT(writeLock.owns_lock()); // need to keep the lock for trace statement
        SPX_DBG_TRACE_VERBOSE_IF(IsState(AudioState::Idle), "%s: Already Idle; Waiting for uspStates to complete ... (audioState/uspState=%d/%d)", __FUNCTION__, m_audioState, m_uspState);

        writeLock.unlock();

        // If we expect an intent response and we've received a successful speech recognition message...
        if (m_expectIntentResponse && message.recognitionStatus == USP::RecognitionStatus::Success)
        {
            // Let's wait for the intent response (we'll fire the final result once we receive it)
            FireFinalResultLater(message);
        }
        else
        {
            // Otherwise, we'll fire it right now
            FireFinalResultNow(message);
        }
    }
    else
    {
        SPX_DBG_ASSERT(writeLock.owns_lock()); // need to keep the lock for warning trace
        SPX_TRACE_WARNING("%s: Unexpected USP State transition (audioState/uspState=%d/%d)", __FUNCTION__, m_audioState, m_uspState);
    }
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
    if (IsState(UspState::Error) || IsState(UspState::Terminating) || IsState(UspState::Zombie))
    {
        SPX_DBG_ASSERT(writeLock.owns_lock()); // need to keep the lock for trace warning
        SPX_DBG_TRACE_VERBOSE("%s: IGNORING (Err/Terminating/Zombie)... (audioState/uspState=%d/%d)", __FUNCTION__, m_audioState, m_uspState);
    }
    else if (IsState(UspState::SpeechHypothesis) ||
        ChangeState(AudioState::ProcessingAudio, UspState::SpeechStarted, AudioState::ProcessingAudio, UspState::SpeechHypothesis) ||
        ChangeState(AudioState::WaitingForDone, UspState::SpeechStarted, AudioState::WaitingForDone, UspState::SpeechHypothesis) ||
        ChangeState(AudioState::Idle, UspState::SpeechStarted, AudioState::Idle, UspState::SpeechHypothesis))
    {
        SPX_DBG_ASSERT(writeLock.owns_lock()); // need to keep the lock for trace statement
        {
            writeLock.unlock();
            SPX_DBG_TRACE_SCOPE("Fire final translation result: Creating Result", "FireFinalResul: GetSite()->FinalRecoResult()  complete!");

            // Create the result
            auto factory = SpxQueryService<ISpxRecoResultFactory>(GetSite());
            auto result = factory->CreateIntermediateResult(nullptr, message.text.c_str(), ResultType::TranslationText);

            // Update our result to be an "TranslationText" result.
            auto initTranslationResult = SpxQueryInterface<ISpxTranslationTextResultInit>(result);
            // Todo: better convert translation status
            ISpxTranslationStatus status;
            switch (message.translation.translationStatus)
            {
            case ::USP::TranslationStatus::Success:
                status = ISpxTranslationStatus::Success;
                break;
            default:
                status = ISpxTranslationStatus::Error;
                break;
            }

            initTranslationResult->InitTranslationTextResult(status, message.translation.translations);

            // Fire the result
            SPX_ASSERT(GetSite() != nullptr);
            GetSite()->IntermediateRecoResult(this, message.offset, result);
        }
    }
    else
    {
        SPX_DBG_ASSERT(writeLock.owns_lock()); // need to keep the lock for warning trace
        SPX_TRACE_WARNING("%s: Unexpected USP State transition (audioState/uspState=%d/%d)", __FUNCTION__, m_audioState, m_uspState);
    }

}

void CSpxUspRecoEngineAdapter::OnTranslationPhrase(const USP::TranslationPhraseMsg& message)
{
    SPX_DBG_TRACE_VERBOSE("Response: Translation.Phrase message. RecoStatus: %d, TranslationStatus: %d, RecoText: %ls, starts at %" PRIu64 ", with duration %" PRIu64 " (100ns).\n",
        message.recognitionStatus, message.translation.translationStatus,
        message.text.c_str(), message.offset, message.duration);
    auto resultMap = message.translation.translations;
#ifdef _DEBUG
    for (const auto& it : resultMap)
    {
        SPX_DBG_TRACE_VERBOSE("          , tranlated to %ls: %ls,\n", it.first.c_str(), it.second.c_str());
    }
#endif

    WriteLock_Type writeLock(m_stateMutex);
    if (IsState(UspState::Error) || IsState(UspState::Terminating) || IsState(UspState::Zombie))
    {
        SPX_DBG_ASSERT(writeLock.owns_lock()); // need to keep the lock for trace warning
        SPX_DBG_TRACE_VERBOSE("%s: IGNORING (Err/Terminating/Zombie)... (audioState/uspState=%d/%d)", __FUNCTION__, m_audioState, m_uspState);
    }
    else if (
        ChangeState(AudioState::ProcessingAudio, UspState::SpeechHypothesis, AudioState::ProcessingAudio, UspState::ReceivedSpeechResult) ||
        ChangeState(AudioState::WaitingForDone, UspState::SpeechHypothesis, AudioState::WaitingForDone, UspState::ReceivedSpeechResult) ||
        ChangeState(AudioState::Idle, UspState::SpeechHypothesis, AudioState::Idle, UspState::ReceivedSpeechResult) ||
        ChangeState(AudioState::ProcessingAudio, UspState::SpeechEnded, AudioState::ProcessingAudio, UspState::ReceivedSpeechResult) ||
        ChangeState(AudioState::WaitingForDone, UspState::SpeechEnded, AudioState::WaitingForDone, UspState::ReceivedSpeechResult) ||
        ChangeState(AudioState::Idle, UspState::SpeechEnded, AudioState::Idle, UspState::ReceivedSpeechResult))
    {
        SPX_DBG_ASSERT(writeLock.owns_lock()); // need to keep the lock for trace statement
        SPX_DBG_TRACE_VERBOSE_IF(IsState(AudioState::Idle), "%s: Already Idle; Waiting for uspStates to complete ... (audioState/uspState=%d/%d)", __FUNCTION__, m_audioState, m_uspState);

        // Todo: deal with nomatch event? failed events?
        if (IsState(AudioState::Idle) && ChangeState(UspState::FiredFinalResult))
        {
            SPX_DBG_ASSERT(writeLock.owns_lock()); // need to keep the lock for trace warning
            SPX_DBG_TRACE_VERBOSE("%s: Already Idle; Waiting for uspStates to complete ... (audioState/uspState=%d/%d)", __FUNCTION__, m_audioState, m_uspState);
        }
        else if (ChangeState(UspState::FiredFinalResult))
        {
            writeLock.unlock();

            SPX_DBG_TRACE_SCOPE("Fire final translation result: Creating Result", "FireFinalResul: GetSite()->FinalRecoResult()  complete!");

            // Create the result
            auto factory = SpxQueryService<ISpxRecoResultFactory>(GetSite());
            auto result = factory->CreateFinalResult(nullptr, message.text.c_str(), ResultType::TranslationText);

            // Update our result to be an "TranslationText" result.
            auto initTranslationResult = SpxQueryInterface<ISpxTranslationTextResultInit>(result);
            // Todo: better convert translation status
            ISpxTranslationStatus status;
            switch (message.translation.translationStatus)
            {
            case ::USP::TranslationStatus::Success:
                status = ISpxTranslationStatus::Success;
                break;
            default:
                status = ISpxTranslationStatus::Error;
                break;
            }

            initTranslationResult->InitTranslationTextResult(status, message.translation.translations);

            // Fire the result
            SPX_ASSERT(GetSite() != nullptr);
            GetSite()->FinalRecoResult(this, message.offset, result);
        }
        else
        {
            SPX_DBG_ASSERT(writeLock.owns_lock()); // need to keep the lock for trace warning
            SPX_TRACE_WARNING("%s: Unexpected USP State transition (audioState/uspState=%d/%d)", __FUNCTION__, m_audioState, m_uspState);
        }
    }
    else
    {
        SPX_DBG_ASSERT(writeLock.owns_lock()); // need to keep the lock for warning trace
        SPX_TRACE_WARNING("%s: Unexpected USP State transition (audioState/uspState=%d/%d)", __FUNCTION__, m_audioState, m_uspState);
    }
}

void CSpxUspRecoEngineAdapter::OnTranslationSynthesis(const USP::TranslationSynthesisMsg& message)
{
    SPX_DBG_TRACE_VERBOSE("Response: Translation.Synthesis message. Audio data size: \n", message.audioLength);
    SPX_DBG_ASSERT(GetSite());

    // TODO: RobCh: Do something with the other fields in UspMsgSpeechPhrase
    auto factory = SpxQueryService<ISpxRecoResultFactory>(GetSite());
    auto result = factory->CreateFinalResult(nullptr, L"", ResultType::TranslationSynthesis);

    // Update our result to be an "TranslationSynthesis" result.
    // auto initTranslationResult = SpxQueryInterface<ISpxTranslationSynthesisResultInit>(result);
    // initTranslationResult->InitTranslationSynthesisResult(message.audioBuffer, message.audioLength, message.text);

    (void)message;
    // Todo: offset (and duration) should be part of result. Now, offset is autaully ignored by the following function.
    // Todo: need to differentiate whether this is the last audio or not, in order to trigger FinalRecoResult if needed.
    // Waiting for Rob's change for direct LUIS integration, which introduces a state machine in usp_reco_engine.
    GetSite()->TranslationSynthesisResult(this, result);
}

void CSpxUspRecoEngineAdapter::OnTurnStart(const USP::TurnStartMsg& message)
{
    SPX_DBG_TRACE_VERBOSE("Response: Turn.Start message. Context.ServiceTag: %s\n", message.contextServiceTag.c_str());

    WriteLock_Type writeLock(m_stateMutex);
    if (IsState(UspState::Error) || IsState(UspState::Terminating) || IsState(UspState::Zombie))
    {
        SPX_DBG_ASSERT(writeLock.owns_lock()); // need to keep the lock for trace warning
        SPX_DBG_TRACE_VERBOSE("%s: IGNORING (Err/Terminating/Zombie)... (audioState/uspState=%d/%d)", __FUNCTION__, m_audioState, m_uspState);
    }
    else if (ChangeState(AudioState::ProcessingAudio, UspState::SendingAudioData, AudioState::ProcessingAudio, UspState::TurnStarted) ||
             ChangeState(AudioState::WaitingForDone, UspState::SendingAudioData, AudioState::WaitingForDone, UspState::TurnStarted) ||
             ChangeState(AudioState::Idle, UspState::SendingAudioData, AudioState::Idle, UspState::TurnStarted))
    {
        writeLock.unlock();

        SPX_DBG_ASSERT(GetSite());
        GetSite()->AdditionalMessage(this, 0, AdditionalMessagePayloadFrom(message));
    }
    else
    {
        SPX_DBG_ASSERT(writeLock.owns_lock()); // need to keep the lock for trace warning
        SPX_TRACE_WARNING("%s: Unexpected USP State transition (audioState/uspState=%d/%d)", __FUNCTION__, m_audioState, m_uspState);
    }
}

void CSpxUspRecoEngineAdapter::OnTurnEnd(const USP::TurnEndMsg& message)
{
    UNUSED(message);
    SPX_DBG_TRACE_VERBOSE("Response: Turn.End message.\n");

    WriteLock_Type writeLock(m_stateMutex);
    if (IsState(UspState::Error) || IsState(UspState::Terminating) || IsState(UspState::Zombie))
    {
        SPX_DBG_ASSERT(writeLock.owns_lock()); // need to keep the lock for trace warning
        SPX_DBG_TRACE_VERBOSE("%s: IGNORING (Err/Terminating/Zombie)... (audioState/uspState=%d/%d)", __FUNCTION__, m_audioState, m_uspState);
    }
   else if (ChangeState(AudioState::ProcessingAudio, UspState::FiredFinalResult, AudioState::ReadyToProcess, UspState::SendingAudioData))
    {
        SPX_DBG_ASSERT(writeLock.owns_lock()); // need to keep the lock for the trace statement
        SPX_DBG_TRACE_VERBOSE("%s: Resuming ProcessingAudio; audioState/uspState=%d/%d", __FUNCTION__, m_audioState, m_uspState);
    }
    else if (IsState(UspState::WaitingForIntent))
    {
        SPX_DBG_ASSERT(writeLock.owns_lock()); // need to keep the lock for trace warning
        SPX_DBG_TRACE_VERBOSE("%s: Intent never came from service!! ... complete the delayed fire final result", __FUNCTION__);

        writeLock.unlock();
        FireFinalResultLater_WaitingForIntentComplete();
    }
    else if (ChangeState(AudioState::WaitingForDone, UspState::FiredFinalResult, AudioState::Idle, UspState::ReadyForFormat))
    {
        writeLock.unlock();
        SPX_DBG_ASSERT(GetSite());
        GetSite()->DoneProcessingAudio(this);
    }
    else if (ChangeState(AudioState::WaitingForDone, UspState::WaitingForIntent, AudioState::ProcessingAudio, UspState::SendingAudioData))
    {
        writeLock.unlock();
        FireFinalResultLater_WaitingForIntentComplete();

        SPX_DBG_ASSERT(GetSite());
        GetSite()->DoneProcessingAudio(this);
    }
    else if (ChangeState(AudioState::Idle, UspState::FiredFinalResult, AudioState::Idle, UspState::ReadyForFormat))
    {
        SPX_DBG_ASSERT(writeLock.owns_lock()); // need to keep the lock for the trace statement
        SPX_DBG_TRACE_VERBOSE("%s: Resuming ProcessingAudio; audioState/uspState=%d/%d", __FUNCTION__, m_audioState, m_uspState);
    }
    else if (ChangeState(AudioState::Paused, UspState::FiredFinalResult, AudioState::ProcessingAudio, UspState::SendingAudioData))
    {
        SPX_DBG_ASSERT(writeLock.owns_lock()); // need to keep the lock for the trace statement
        SPX_DBG_TRACE_VERBOSE("%s: Resuming ProcessingAudio; audioState/uspState=%d/%d", __FUNCTION__, m_audioState, m_uspState);
    }
    else
    {
        SPX_DBG_ASSERT(writeLock.owns_lock()); // need to keep the lock for trace warning
        SPX_TRACE_WARNING("%s: Unexpected USP State transition (audioState/uspState=%d/%d)", __FUNCTION__, m_audioState, m_uspState);
    }
}

void CSpxUspRecoEngineAdapter::OnError(const std::string& error)
{
    SPX_DBG_TRACE_VERBOSE("Response: On Error: %s.\n", error.c_str());

    WriteLock_Type writeLock(m_stateMutex);
    if (IsState(UspState::Error) || IsState(UspState::Terminating) || IsState(UspState::Zombie))
    {
        SPX_DBG_ASSERT(writeLock.owns_lock()); // need to keep the lock for trace warning
        SPX_DBG_TRACE_VERBOSE("%s: IGNORING (Err/Terminating/Zombie)... (audioState/uspState=%d/%d)", __FUNCTION__, m_audioState, m_uspState);
    }
    else if (ChangeState(UspState::Error))
    {
        auto site = GetSite();

        writeLock.unlock();
        site->Error(this, error);
        writeLock.lock();

        if (ChangeState(AudioState::ReadyToProcess, AudioState::Idle) ||
            ChangeState(AudioState::ProcessingAudio, AudioState::Idle))
        {
            writeLock.unlock();
            site->DoneProcessingAudio(this);
        }
    }
    else
    {
        SPX_DBG_ASSERT(writeLock.owns_lock()); // need to keep the lock for trace warning
        SPX_TRACE_WARNING("%s: Unexpected USP State transition (audioState/uspState=%d/%d)", __FUNCTION__, m_audioState, m_uspState);
    }
}

void CSpxUspRecoEngineAdapter::OnUserMessage(const std::string& path, const std::string& contentType, const uint8_t* buffer, size_t size)
{
    UNUSED(contentType);
    UNUSED(buffer);
    UNUSED(size);
    SPX_DBG_TRACE_VERBOSE("Response: Usp User Message: %s, content-type=%s", path.c_str(), contentType.c_str());

    if (path == "response")
    {
        WriteLock_Type writeLock(m_stateMutex);
        if (IsState(UspState::WaitingForIntent))
        {
            std::string luisJson((const char*)buffer, size);
            SPX_DBG_TRACE_VERBOSE("USP User Message: response; luisJson='%s'", luisJson.c_str());

            writeLock.unlock();
            FireFinalResultLater_WaitingForIntentComplete(luisJson);
        }
        else
        {
            SPX_DBG_ASSERT(writeLock.owns_lock()); // need to keep the lock for trace warning
            SPX_TRACE_WARNING("%s: Unexpected USP State transition (audioState/uspState=%d/%d)", __FUNCTION__, m_audioState, m_uspState);
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
    auto properties = SpxQueryService<ISpxNamedProperties>(GetSite());
    auto noDGI = properties->GetBooleanValue(L"CARBON-INTERNAL-USP-NoDGI", false);

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
            appendComma = true;
        }

        dgiJson += "}";  // close object
    }

    return noDGI ? "" : dgiJson;
}

void CSpxUspRecoEngineAdapter::GetIntentInfoFromSite(std::string& provider, std::string& id, std::string& key)
{
    SPX_DBG_ASSERT(GetSite() != nullptr);
    GetSite()->GetIntentInfo(provider, id, key);
}

std::string CSpxUspRecoEngineAdapter::GetIntentJsonFromIntentInfo(const std::string& provider, const std::string& id, const std::string& key)
{
    auto properties = SpxQueryService<ISpxNamedProperties>(GetSite());
    auto noIntentJson = properties->GetBooleanValue(L"CARBON-INTERNAL-USP-NoIntentJson", false);

    std::string intentJson;
    if (!provider.empty() && !id.empty() && !key.empty())
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
            appendComma = true;
        }

        contextJson += "}";
    }

    return contextJson;
}

void CSpxUspRecoEngineAdapter::FireFinalResultNow(const USP::SpeechPhraseMsg& message, const std::string& luisJson)
{
    WriteLock_Type writeLock(m_stateMutex);
    if (IsState(UspState::Error) || IsState(UspState::Terminating) || IsState(UspState::Zombie))
    {
        SPX_DBG_ASSERT(writeLock.owns_lock()); // need to keep the lock for trace warning
        SPX_DBG_TRACE_VERBOSE("%s: IGNORING (Err/Terminating/Zombie)... (audioState/uspState=%d/%d)", __FUNCTION__, m_audioState, m_uspState);
    }
    else if (IsState(AudioState::Idle) && ChangeState(UspState::FiredFinalResult))
    {
        SPX_DBG_ASSERT(writeLock.owns_lock()); // need to keep the lock for trace warning
        SPX_DBG_TRACE_VERBOSE("%s: Already Idle; Waiting for uspStates to complete ... (audioState/uspState=%d/%d)", __FUNCTION__, m_audioState, m_uspState);
    }
    else if (ChangeState(UspState::FiredFinalResult))
    {
        SPX_DBG_TRACE_SCOPE("FireFinalResultNow: Creating Result", "FireFinalResultNow: GetSite()->FinalRecoResult()  complete!");

        // Create the result
        auto factory = SpxQueryService<ISpxRecoResultFactory>(GetSite());
        auto result = factory->CreateFinalResult(nullptr, message.displayText.c_str(), ResultType::Speech);

        // Do we already have the LUIS json payload from the service (1-hop)
        if (!luisJson.empty())
        {
            auto namedProperties = SpxQueryInterface<ISpxNamedProperties>(result);
            namedProperties->SetStringValue(g_RESULT_LuisJson, PAL::ToWString(luisJson).c_str());
        }

        // Fire the result
        writeLock.unlock();
        SPX_ASSERT(GetSite() != nullptr);
        GetSite()->FinalRecoResult(this, message.offset, result);
    }
    else
    {
        SPX_DBG_ASSERT(writeLock.owns_lock()); // need to keep the lock for trace warning
        SPX_TRACE_WARNING("%s: Unexpected USP State transition (audioState/uspState=%d/%d)", __FUNCTION__, m_audioState, m_uspState);
    }
}

void CSpxUspRecoEngineAdapter::FireFinalResultLater(const USP::SpeechPhraseMsg& message)
{
    WriteLock_Type writeLock(m_stateMutex);
    if (IsState(UspState::Error) || IsState(UspState::Terminating) || IsState(UspState::Zombie))
    {
        SPX_DBG_ASSERT(writeLock.owns_lock()); // need to keep the lock for trace warning
        SPX_DBG_TRACE_VERBOSE("%s: IGNORING (Err/Terminating/Zombie)... (audioState/uspState=%d/%d)", __FUNCTION__, m_audioState, m_uspState);
    }
    else if (ChangeState(AudioState::Idle, UspState::ReceivedSpeechResult, AudioState::Idle, UspState::WaitingForIntent))
    {
        SPX_DBG_ASSERT(writeLock.owns_lock()); // need to keep the lock for trace warning
        SPX_DBG_TRACE_VERBOSE("%s: Already Idle; Waiting for uspStates to complete ... (audioState/uspState=%d/%d)", __FUNCTION__, m_audioState, m_uspState);
    }
    else if (ChangeState(AudioState::ProcessingAudio, UspState::ReceivedSpeechResult, AudioState::ProcessingAudio, UspState::WaitingForIntent) ||
             ChangeState(AudioState::Paused, UspState::ReceivedSpeechResult, AudioState::Paused, UspState::WaitingForIntent) ||
             ChangeState(AudioState::WaitingForDone, UspState::ReceivedSpeechResult, AudioState::WaitingForDone, UspState::WaitingForIntent))
    {
        m_finalResultMessageToFireLater = message;
    }
    else
    {
        SPX_DBG_ASSERT(writeLock.owns_lock()); // need to keep the lock for trace warning
        SPX_TRACE_WARNING("%s: Unexpected USP State transition (audioState/uspState=%d/%d)", __FUNCTION__, m_audioState, m_uspState);
    }
}

void CSpxUspRecoEngineAdapter::FireFinalResultLater_WaitingForIntentComplete(const std::string& luisJson)
{
    SPX_DBG_ASSERT(m_expectIntentResponse);
    FireFinalResultNow(m_finalResultMessageToFireLater, luisJson);
}

bool CSpxUspRecoEngineAdapter::ChangeState(UspState newState)
{
    return ChangeState(m_audioState, m_uspState, m_audioState, newState);
}

bool CSpxUspRecoEngineAdapter::ChangeState(UspState fromState, UspState toState)
{
    return ChangeState(m_audioState, fromState, m_audioState, toState);
}

bool CSpxUspRecoEngineAdapter::ChangeState(AudioState newState)
{
    return ChangeState(m_audioState, m_uspState, newState, m_uspState);
}

bool CSpxUspRecoEngineAdapter::ChangeState(AudioState fromState, AudioState toState)
{
    return ChangeState(fromState, m_uspState, toState, m_uspState);
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
        SPX_DBG_TRACE_VERBOSE("%s; audioState/uspState: %d/%d => %d/%d", __FUNCTION__, fromAudioState, fromUspState, toAudioState, toUspState);
        m_audioState = toAudioState;
        m_uspState = toUspState;
        return true;
    }

    return false;
}


} } } } // Microsoft::CognitiveServices::Speech::Impl
