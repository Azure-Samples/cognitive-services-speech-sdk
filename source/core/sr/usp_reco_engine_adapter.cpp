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


namespace CARBON_IMPL_NAMESPACE() {


int CSpxUspRecoEngineAdapter::m_instanceCounter = 0;


CSpxUspRecoEngineAdapter::CSpxUspRecoEngineAdapter() :
    m_servicePreferedBufferSize(0),
    m_bytesInBuffer(0),
    m_ptrIntoBuffer(nullptr),
    m_bytesLeftInBuffer(0)
{
    m_instanceCounter++;
}

void CSpxUspRecoEngineAdapter::Init()
{
    SPX_IFTRUE_THROW_HR(GetSite() == nullptr, SPXERR_UNINITIALIZED);
    SPX_IFTRUE_THROW_HR(m_handle != nullptr, SPXERR_ALREADY_INITIALIZED);

    UspInitialize();
    DumpFileInit();
}

void CSpxUspRecoEngineAdapter::Term()
{
    SPX_DBG_TRACE_SCOPE("Terminating CSpxUspRecoEngineAdapter...", "Terminating CSpxUspRecoEngineAdapter... Done!");
    
    SPX_DBG_TRACE_VERBOSE("%s(0x%x)", __FUNCTION__, m_handle.get());
    m_handle.reset();

    DumpFileClose();
}

void CSpxUspRecoEngineAdapter::SetFormat(WAVEFORMATEX* pformat)
{
    SPX_IFTRUE_THROW_HR(m_handle == nullptr, SPXERR_UNINITIALIZED);

    if (pformat != nullptr)
    {
        UspWriteFormat(pformat);
        m_servicePreferedBufferSize = (size_t)pformat->nSamplesPerSec * pformat->nBlockAlign * m_servicePreferedMilliseconds / 1000;
    }
    else
    {
        UspWrite_Flush();
    }
}

void CSpxUspRecoEngineAdapter::ProcessAudio(AudioData_Type data, uint32_t size)
{
    UspWrite(data.get(), size);
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

    throw std::runtime_error("Unknown RecognitionMode value " + PAL::ToString(value));
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

    throw std::runtime_error("No Authentication parameters were specified.");
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
    DumpFileWrite(buffer, byteToWrite);
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
    SPX_DBG_ASSERT(GetSite());
    GetSite()->SpeechStartDetected(this, message.offset);
}

void CSpxUspRecoEngineAdapter::OnSpeechEndDetected(const USP::SpeechEndDetectedMsg& message)
{
    SPX_DBG_TRACE_VERBOSE("Response: Speech.EndDetected message. Speech ends at offset %" PRIu64 " (100ns)\n", message.offset);
    SPX_DBG_ASSERT(GetSite());
    GetSite()->SpeechEndDetected(this, message.offset);
}

void CSpxUspRecoEngineAdapter::OnSpeechHypothesis(const USP::SpeechHypothesisMsg& message)
{
    SPX_DBG_TRACE_VERBOSE("Response: Speech.Hypothesis message. Starts at offset %" PRIu64 ", with duration %" PRIu64 " (100ns). Text: %ls\n", message.offset, message.duration, message.text.c_str());
    SPX_DBG_ASSERT(GetSite());

    // TODO: RobCh: Do something with the other fields in UspMsgSpeechHypothesis
    auto factory = SpxQueryService<ISpxRecoResultFactory>(GetSite());
    auto result = factory->CreateIntermediateResult(nullptr, message.text.c_str());

    GetSite()->IntermediateRecoResult(this, message.offset, result);
}

void CSpxUspRecoEngineAdapter::OnSpeechFragment(const USP::SpeechFragmentMsg& message)
{
    SPX_DBG_TRACE_VERBOSE("Response: Speech.Fragment message. Starts at offset %" PRIu64 ", with duration %" PRIu64 " (100ns). Text: %ls\n", message.offset, message.duration, message.text.c_str());
    SPX_DBG_ASSERT(GetSite());

    // TODO: RobCh: Do something with the other fields in UspMsgSpeechHypothesis
    // TODO: Rob: do we want to treate speech.fragment message different than speech.hypothesis message at this level?

    auto factory = SpxQueryService<ISpxRecoResultFactory>(GetSite());
    auto result = factory->CreateIntermediateResult(nullptr, message.text.c_str());
    GetSite()->IntermediateRecoResult(this, message.offset, result);
}

void CSpxUspRecoEngineAdapter::OnSpeechPhrase(const USP::SpeechPhraseMsg& message)
{
    SPX_DBG_TRACE_VERBOSE("Response: Speech.Phrase message. Status: %d, Text: %ls, starts at %" PRIu64 ", with duration %" PRIu64 " (100ns).\n", message.recognitionStatus, message.displayText.c_str(), message.offset, message.duration);
    SPX_DBG_ASSERT(GetSite());

    // TODO: RobCh: Do something with the other fields in UspMsgSpeechPhrase
    auto factory = SpxQueryService<ISpxRecoResultFactory>(GetSite());
    auto result = factory->CreateFinalResult(nullptr, message.displayText.c_str());

    GetSite()->FinalRecoResult(this, message.offset, result);
}

void CSpxUspRecoEngineAdapter::OnTurnStart(const USP::TurnStartMsg& message)
{
    SPX_DBG_TRACE_VERBOSE("Response: Turn.Start message. Context.ServiceTag: %s\n", message.contextServiceTag.c_str());
    SPX_DBG_ASSERT(GetSite());
    GetSite()->AdditionalMessage(this, 0, AdditionalMessagePayloadFrom(message));
}

void CSpxUspRecoEngineAdapter::OnTurnEnd(const USP::TurnEndMsg& message)
{
    SPX_DBG_TRACE_VERBOSE("Response: Turn.End message.\n");
    UNUSED(message);
    SPX_DBG_ASSERT(GetSite());
    GetSite()->DoneProcessingAudio(this);
}

void CSpxUspRecoEngineAdapter::OnError(const std::string& error)
{
    SPX_DBG_TRACE_VERBOSE("Response: On Error: %s.\n", error.c_str());
    SPX_DBG_ASSERT(GetSite());
    auto site = GetSite();
    site->Error(this, error);
    site->DoneProcessingAudio(this);
}

void CSpxUspRecoEngineAdapter::DumpFileInit()
{
    // TODO: is this audio dumping some debugging cruft, is it still needed?
    // on Windows, this fopen returns null if the same file was already opened,
    // as a result writing segfaults. Adding a counter the filename, so that
    // each recognizer will get its own dumpfile.
    // On Android, this fails even because the default location might not be writeable
    // for the app package.
    std::string filename = "uspaudiodump_" + std::to_string(m_instanceCounter) + ".wav";
    PAL::fopen_s(&m_hfile, filename.c_str(), "wb");
}

void CSpxUspRecoEngineAdapter::DumpFileWrite(const uint8_t* buffer, size_t bytesToWrite)
{
    if (m_hfile != nullptr)
    {
        fwrite(buffer, 1, bytesToWrite, m_hfile);
    }
}

void CSpxUspRecoEngineAdapter::DumpFileClose()
{
    if (m_hfile != nullptr)
        fclose(m_hfile);
    m_hfile = nullptr;
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


} // CARBON_IMPL_NAMESPACE
