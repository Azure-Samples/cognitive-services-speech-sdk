//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// http_audio_stream_session.cpp: Private implementation declarations for CSpxHttpAudioStreamSession
//
#include "stdafx.h"
#include "create_object_helpers.h"
#include "http_audio_stream_session.h"
#include "site_helpers.h"
#include "thread_service.h"

using namespace std;

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

void CSpxHttpAudioStreamSession::Init()
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    // The CSpxApiFactory is created at XX_from_config. and the factory is needed when call enrollment which is after creating voice profile client.
    m_keepFactoryAlive = GetSite();

    m_enroll = false;
}

void CSpxHttpAudioStreamSession::Term()
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    SpxTermAndClear(m_audioPump);
    SpxTermAndClear(m_keepFactoryAlive);
    SpxTermAndClear(m_codecAdapter);
    m_reco = nullptr;
}

void CSpxHttpAudioStreamSession::InitFromFile(const wchar_t* pszFileName)
{
    // Create the wav file pump
    auto audio_file_pump = SpxCreateObjectWithSite<ISpxAudioFile>("CSpxWavFilePump", this);
    m_audioPump = SpxQueryInterface<ISpxAudioPump>(audio_file_pump);

    // Open the WAV file
    audio_file_pump->Open(pszFileName);
    SPX_DBG_TRACE_VERBOSE("[%p]CSpxHttpAudioStreamSession::InitFromFile Pump from file:[%p]", (void*)this, (void*)m_audioPump.get());
}

void CSpxHttpAudioStreamSession::InitFromMicrophone()
{
    // Create the microphone pump
    auto site = SpxSiteFromThis(this);
    m_audioPump = SpxCreateObjectWithSite<ISpxAudioPump>("CSpxInteractiveMicrophone", site);
    SPX_DBG_TRACE_VERBOSE("[%p]CSpxHttpAudioStreamSession::InitFromMicrophone: Pump from microphone:[%p]", (void*)this, (void*)m_audioPump.get());
}

void CSpxHttpAudioStreamSession::InitFromStream(shared_ptr<ISpxAudioStream> stream)
{
    shared_ptr<ISpxAudioPumpInit> audioPump;
    shared_ptr<ISpxAudioStreamReader> reader;

    // Create the stream pump
    auto cbFormat = stream->GetFormat(nullptr, 0);
    auto waveformat = SpxAllocWAVEFORMATEX(cbFormat);
    stream->GetFormat(waveformat.get(), cbFormat);

    if (waveformat->wFormatTag != WAVE_FORMAT_PCM)
    {
        m_codecAdapter = SpxCreateObjectWithSite<ISpxAudioStreamReader>("CSpxCodecAdapter", GetSite());
        SPX_IFTRUE_THROW_HR(m_codecAdapter == nullptr, SPXERR_GSTREAMER_NOT_FOUND_ERROR);

        reader = SpxQueryInterface<ISpxAudioStreamReader>(stream);

        auto initCallbacks = SpxQueryInterface<ISpxAudioStreamReaderInitCallbacks>(m_codecAdapter);
        initCallbacks->SetCallbacks(
            [=](uint8_t* buffer, uint32_t size) { return reader->Read(buffer, size); },
            [=]() { { reader->Close(); } });

        initCallbacks->SetPropertyCallback2(
            [=](PropertyId propertyId) {
                return reader->GetProperty(propertyId);
            });

        auto adapterAsSetFormat = SpxQueryInterface<ISpxAudioStreamInitFormat>(m_codecAdapter);

        auto numChannelsString = GetStringValue("OutputPCMChannelCount", "1");
        auto numBitsPerSampleString = GetStringValue("OutputPCMNumBitsPerSample", "16");
        auto sampleRateString = GetStringValue("OutputPCMSamplerate", "16000");

        try
        {
            waveformat->nChannels = (uint16_t)stoi(numChannelsString);
            waveformat->wBitsPerSample = (uint16_t)stoi(numBitsPerSampleString);
            waveformat->nSamplesPerSec = (uint32_t)stoi(sampleRateString);
        }
        catch (const exception& e)
        {
            SPX_DBG_TRACE_VERBOSE("Error Parsing %s", e.what());
            SPX_DBG_TRACE_VERBOSE("Setting default output format samplerate = 16000, bitspersample = 16 and numchannels = 1");
            waveformat->nChannels = 1;
            waveformat->wBitsPerSample = 16;
            waveformat->nSamplesPerSec = 16000;
        }

        adapterAsSetFormat->SetFormat(waveformat.get());
    }

    audioPump = SpxCreateObjectWithSite<ISpxAudioPumpInit>("CSpxAudioPump", this);

    m_audioPump = SpxQueryInterface<ISpxAudioPump>(audioPump);

    // Attach the stream to the pump
    if (m_codecAdapter == nullptr)
    {
        reader = SpxQueryInterface<ISpxAudioStreamReader>(stream);
    }
    else
    {
        reader = SpxQueryInterface<ISpxAudioStreamReader>(m_codecAdapter);
    }

    audioPump->SetReader(reader);
}

RecognitionResultPtr CSpxHttpAudioStreamSession::StartStreamingAudioAndWaitForResult(bool enroll, VoiceProfileType type, vector<string>&& profileIds)
{
    std::unique_lock<std::mutex> lock(m_mutex);

    if (m_reco == nullptr)
    {
        auto site = SpxSiteFromThis(this);
        m_reco = SpxCreateObjectWithSite<ISpxHttpRecoEngineAdapter>("CSpxHttpRecoEngineAdapter", site);
    }
    auto cbFormat = m_audioPump->GetFormat(nullptr, 0);
    auto waveformat = SpxAllocWAVEFORMATEX(cbFormat);
    m_audioPump->GetFormat(waveformat.get(), cbFormat);

    m_reco->SetFormat(waveformat.get(), type, move(profileIds), enroll);

    auto ptr = (ISpxAudioProcessor*)this;
    auto pISpxAudioProcessor = ptr->shared_from_this();

    m_audioIsDone = std::make_shared<std::promise<RecognitionResultPtr>>();
    m_enroll = enroll;
    m_future = (*m_audioIsDone).get_future();

    m_audioPump->StartPump(pISpxAudioProcessor);

    auto result = m_future.get();

    return result;
}

//--- ISpxAudioPumpSite
void CSpxHttpAudioStreamSession::Error(const string& msg)
{
    wstring error = PAL::ToWString(msg);
    auto result = CreateFinalResult(nullptr, ResultReason::Canceled, NO_MATCH_REASON_NONE, CancellationReason::Error, CancellationErrorCode::RuntimeError, error.c_str(), 0, 0);
    (*m_audioIsDone).set_value(result);
}

void CSpxHttpAudioStreamSession::SetFormat(const SPXWAVEFORMATEX* pformat)
{
    if (m_reco == nullptr)
    {
        SPX_TRACE_ERROR("http reco engine adapter is null.");
        SPX_THROW_HR(SPXERR_RUNTIME_ERROR);
    }

    // all audio is done pumping.
    if (pformat == nullptr)
    {
        m_reco->FlushAudio();
        auto result = m_reco->GetResult();
        (*m_audioIsDone).set_value(result);
        m_audioIsDone = nullptr;
    }
}

void CSpxHttpAudioStreamSession::ProcessAudio(const DataChunkPtr& audioChunk)
{
    if (m_reco == nullptr)
    {
        SPX_TRACE_ERROR("http reco engine adapter is null.");
        SPX_THROW_HR(SPXERR_RUNTIME_ERROR);
    }
    m_reco->ProcessAudio(audioChunk);
}

string CSpxHttpAudioStreamSession::CreateVoiceProfile(VoiceProfileType type, string&& locale)
{
    auto site = SpxSiteFromThis(this);
    auto reco = SpxCreateObjectWithSite<ISpxHttpRecoEngineAdapter>("CSpxHttpRecoEngineAdapter", site);
    return reco->CreateVoiceProfile(type, move(locale));
}

RecognitionResultPtr CSpxHttpAudioStreamSession::ModifyVoiceProfile(bool reset, VoiceProfileType type, std::string&& id)
{
    std::unique_lock<std::mutex> lock(m_mutex);

    auto site = SpxSiteFromThis(this);
    auto reco = SpxCreateObjectWithSite<ISpxHttpRecoEngineAdapter>("CSpxHttpRecoEngineAdapter", site);
    return reco->ModifyVoiceProfile(reset, type, move(id));
}

shared_ptr<ISpxNamedProperties> CSpxHttpAudioStreamSession::GetParentProperties() const
{
    return SpxQueryInterface<ISpxNamedProperties>(GetSite());
}

shared_ptr<ISpxRecognitionResult> CSpxHttpAudioStreamSession::CreateIntermediateResult(const wchar_t* resultId, const wchar_t* text, uint64_t offset, uint64_t duration)
{
    UNUSED(resultId);
    UNUSED(text);
    UNUSED(offset);
    UNUSED(duration);

    SPX_DBG_ASSERT(false);
    return nullptr;
}

std::shared_ptr<ISpxRecognitionResult> CSpxHttpAudioStreamSession::CreateKeywordResult(const double confidence, const uint64_t offset, const uint64_t duration, const wchar_t* keyword, ResultReason reason, std::shared_ptr<ISpxAudioDataStream> stream)
{
    UNUSED(confidence);
    UNUSED(offset);
    UNUSED(duration);
    UNUSED(keyword);
    UNUSED(reason);
    UNUSED(stream);

    SPX_DBG_ASSERT(false);
    return nullptr;
}

shared_ptr<ISpxRecognitionResult> CSpxHttpAudioStreamSession::CreateFinalResult(const wchar_t* resultId, ResultReason reason, NoMatchReason noMatchReason, CancellationReason cancellation, CancellationErrorCode errorCode, const wchar_t* text, uint64_t offset, uint64_t duration, const wchar_t* userId)
{
    UNUSED(userId);
    auto result = SpxCreateObjectWithSite<ISpxRecognitionResult>("CSpxRecognitionResult", this);

    auto initResult = SpxQueryInterface<ISpxRecognitionResultInit>(result);
    initResult->InitFinalResult(resultId, reason, noMatchReason, cancellation, errorCode, text, offset, duration);

    return result;
}

}}}}
