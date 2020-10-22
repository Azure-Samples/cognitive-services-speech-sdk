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
#include "try_catch_helpers.h"
#include "shared_ptr_helpers.h"
#include "error_info.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

using namespace std;
using namespace std::chrono;

void CSpxHttpAudioStreamSession::Init()
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    // The CSpxApiFactory is created at XX_from_config. and the factory is needed when call enrollment which is after creating voice profile client.
    m_keepFactoryAlive = GetSite();

    m_fromMicrophone = false;

    m_threadService = make_shared<CSpxThreadService>();
    m_threadService->Init();
}

void CSpxHttpAudioStreamSession::Term()
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    if (m_audioPump && m_audioPump->GetState() == ISpxAudioPump::State::Processing)
    {
        SPX_DBG_TRACE_VERBOSE("[%p]CSpxHttpAudioStreamSession::Term: StopPump[%p]", (void*)this, (void*)m_audioPump.get());

        m_audioPump->StopPump();

        auto audioIsDone = m_resultPromise;
        if (audioIsDone)
        {
            Error("Terminate the http session.");
        }
    }

    if (m_codecAdapter)
    {
        m_codecAdapter->Close();
    }

    m_threadService->Term();
    if (m_audioPump)
    {
        SpxTermAndClear(m_audioPump);
        m_audioPump = nullptr;
    }
    SpxTermAndClear(m_keepFactoryAlive);
    m_keepFactoryAlive = nullptr;
    SpxTermAndClear(m_codecAdapter);
    m_codecAdapter = nullptr;
    SpxTermAndClear(m_reco);
    m_reco = nullptr;
}

void CSpxHttpAudioStreamSession::InitFromFile(const wchar_t* pszFileName)
{
    auto keepAlive = SpxSharedPtrFromThis<ISpxAudioProcessor>(this);
    wstring filename = pszFileName;
    auto task = CreateTask([this, keepAlive, filename]() {
        SPX_THROW_HR_IF(SPXERR_ALREADY_INITIALIZED, m_audioPump.get() != nullptr);
        // Create the wav file pump
        auto audio_file_pump = SpxCreateObjectWithSite<ISpxAudioFile>("CSpxWavFilePump", this);
        m_audioPump = SpxQueryInterface<ISpxAudioPump>(audio_file_pump);

        // Open the WAV file
        audio_file_pump->Open(filename.c_str());
        SPX_DBG_TRACE_VERBOSE("[%p]InitFromFile Pump from file:[%p]", (void*)this, (void*)m_audioPump.get());
        });
    m_threadService->ExecuteAsync(move(task));
}

void CSpxHttpAudioStreamSession::InitFromMicrophone()
{
    auto keepAlive = SpxSharedPtrFromThis<ISpxAudioProcessor>(this);
    auto task = CreateTask([this, keepAlive]() {
        SPX_THROW_HR_IF(SPXERR_ALREADY_INITIALIZED, m_audioPump.get() != nullptr);
        // Create the microphone pump
        auto site = SpxSiteFromThis(this);
        m_audioPump = SpxCreateObjectWithSite<ISpxAudioPump>("CSpxInteractiveMicrophone", site);
        SPX_DBG_TRACE_VERBOSE("[%p]InitFromMicrophone: Pump from microphone:[%p]", (void*)this, (void*)m_audioPump.get());
        m_fromMicrophone = true;
        });
    m_threadService->ExecuteAsync(move(task));
}

void CSpxHttpAudioStreamSession::InitFromStream(shared_ptr<ISpxAudioStream> stream)
{
    auto keepAlive = SpxSharedPtrFromThis<ISpxAudioProcessor>(this);
    auto task = CreateTask([this, keepAlive, stream = stream]() {
        SPX_THROW_HR_IF(SPXERR_ALREADY_INITIALIZED, m_audioPump.get() != nullptr);

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
        });
    m_threadService->ExecuteAsync(move(task));
}

RecognitionResultPtr CSpxHttpAudioStreamSession::EnrollVoiceProfile(VoiceProfileType type, std::string&& profileId)
{
    vector<string> ids{ profileId };
    return StartStreamingAudioAndWaitForResult(true, type, move(ids));
}

RecognitionResultPtr CSpxHttpAudioStreamSession::RecognizeVoiceProfile(VoiceProfileType type, std::vector<std::string>&& profileIds)
{
    return StartStreamingAudioAndWaitForResult(false, type, move(profileIds));
}

RecognitionResultPtr CSpxHttpAudioStreamSession::StartStreamingAudioAndWaitForResult(bool enroll, VoiceProfileType type, vector<string>&& profileIds)
{
    auto keepAlive = SpxSharedPtrFromThis<ISpxAudioProcessor>(this);
    RecognitionResultPtr result;
    auto task = CreateTask([&result, this, keepAlive, profileIds = move(profileIds), type = type, enroll = enroll]() mutable {
        SPX_DBG_TRACE_SCOPE("StartStreamingAudioAndWaitForResult starting","StartStreamingAudioAndWaitForResult ended");
        if (m_reco == nullptr)
        {
            auto site = SpxSiteFromThis(this);
            m_reco = SpxCreateObjectWithSite<ISpxHttpRecoEngineAdapter>("CSpxHttpRecoEngineAdapter", site);
        }
        auto audioPump = m_audioPump;
        if (audioPump == nullptr)
        {
            auto error = ErrorInfo::FromRuntimeMessage("Error accessing audio pump");
            result = CreateErrorResult(error);
            return;
        }

        auto cbFormat = audioPump->GetFormat(nullptr, 0);
        auto waveformat = SpxAllocWAVEFORMATEX(cbFormat);
        audioPump->GetFormat(waveformat.get(), cbFormat);
        m_reco->SetFormat(waveformat.get(), type, move(profileIds), enroll);

        m_microphoneTimeoutInMS = GetMicrophoneTimeout();
        m_totalAudioinMS = 0;
        auto ptr = (ISpxAudioProcessor*)this;
        auto pISpxAudioProcessor = ptr->shared_from_this();
        m_resultPromise = std::make_shared<std::promise<RecognitionResultPtr>>();
        auto future = (*m_resultPromise).get_future();
        audioPump->StartPump(pISpxAudioProcessor);

        // the max time we wait for audio streaming and result back from http post is 1 minutes.
        auto status = future.wait_for(m_microphoneTimeoutInMS + (milliseconds)1min);

        result = status == future_status::timeout
            ? CreateErrorResult(ErrorInfo::FromRuntimeMessage("Bailed out due to wait more than 1 minutes for the result of enrollment or speaker recognition."))
            : GetResult(move(future));

        // each enroll or verify/identify has its own audio config, so we have to destroy the all audio input and its related member variables here.
        auto finish = shared_ptr<void>(nullptr, [this](void*) {CleanupAfterEachAudioPumping(); });
    });

    m_threadService->ExecuteSync(std::move(task));

    return result;
}

RecognitionResultPtr CSpxHttpAudioStreamSession::GetResult(future<RecognitionResultPtr>&& future)
{
    RecognitionResultPtr result;

    try
    {
        result = future.get();
    }
    catch (const std::exception& e)
    {
        result = CreateErrorResult(ErrorInfo::FromRuntimeMessage(e.what()));
    }

    return result;
}

void CSpxHttpAudioStreamSession::CleanupAfterEachAudioPumping()
{
    StopPump();
    SpxTermAndClear(m_audioPump);
    // make sure the current http post thread is done. Occasionally, HTTP post takes long, AudioDone future times out in StartStreamingAudioAndWaitForResult
    // If we let the postAudioThread lingering around. The next session might creating a new promise at the moment postAudioThread set the result to the promise. The result from previous session could got post to the  new session.
    if (m_postAudioThread.joinable())
    {
        m_postAudioThread.join();
    }
    m_audioPump = nullptr;
    m_fromMicrophone = false;
    m_audioDataCollected = false;
    m_totalAudioinMS = 0;
}

milliseconds CSpxHttpAudioStreamSession::GetMicrophoneTimeout()
{
    auto valueFromUser = GetStringValue("SPEECH-MicrophoneTimeoutInSpeakerRecognitionInMilliseconds", "0");
    if (valueFromUser == "0")
    {
        return m_microphoneTimeoutInMS;
    }

    uint32_t valueInMS = 0;
    try
    {
        valueInMS = std::stoi(valueFromUser);
    }
    catch (const logic_error& e)
    {
        string msg{"error in parsing"};
        msg += valueFromUser;
        msg += e.what();
        ThrowLogicError(msg);
    }

    return (milliseconds)valueInMS;
}
//--- ISpxAudioPumpSite
void CSpxHttpAudioStreamSession::Error(const string& msg)
{
    const auto error = ErrorInfo::FromRuntimeMessage(msg);
    auto result = CreateErrorResult(error);
    auto audioIsDone = m_resultPromise;
    if (audioIsDone)
    {
        (*audioIsDone).set_value(result);
    }
}

void CSpxHttpAudioStreamSession::SetFormat(const SPXWAVEFORMATEX* pformat)
{
    const char* msg = pformat ? "some format" : "null format";
    SPX_DBG_TRACE_INFO("Enter CSpxHttpAudioStreamSession::SetFormat with %s", msg);

    if (m_reco == nullptr)
    {
        SPX_TRACE_ERROR("http reco engine adapter is null.");
        SPX_THROW_HR(SPXERR_RUNTIME_ERROR);
    }

    if (pformat)
    {
        m_avgBytesPerSecond = pformat->nAvgBytesPerSec;
    }
    // all audio is done pumping.
    if (pformat == nullptr && !m_audioDataCollected)
    {
        m_audioDataCollected = true;
        OnDoneAudioPumping();
    }
    SPX_DBG_TRACE_INFO("Exit CSpxHttpAudioStreamSession::SetFormat with %s", msg);
}

// OnDoneAudioPumping is called at the AudioPump thread. HTTP Post does take time, so we don't want to hog the audio pump thread.
void CSpxHttpAudioStreamSession::OnDoneAudioPumping()
{
    if (m_postAudioThread.joinable())
    {
        m_postAudioThread.join();
    }
    auto keepAlive = SpxSharedPtrFromThis<ISpxSpeakerRecognition>(this);
    m_postAudioThread = thread([this, keepAlive]() {
        SPX_DBG_TRACE_VERBOSE("Starting to flush all audio data to the HTTP Adapter.");
        auto httpRecoAdapter = m_reco;
        if (!httpRecoAdapter)
        {
            if (m_resultPromise)
            {
                (*m_resultPromise).set_exception(std::make_exception_ptr(std::runtime_error("The http adapter is a nullptr.")));
            }
            return;
        }

        httpRecoAdapter->FlushAudio();
        auto result = httpRecoAdapter->GetResult();
        SPX_DBG_TRACE_INFO("Audio session received the result of flush audio.");
        if (m_resultPromise)
        {
            (*m_resultPromise).set_value(result);
        }
        SPX_DBG_TRACE_VERBOSE("Done sending result back to the caller.");
        });
}

uint32_t CSpxHttpAudioStreamSession::FromBytesToMilisecond(uint32_t bytes, uint32_t bytesPerSecond)
{
    return static_cast<uint32_t>(bytes * 1000) / bytesPerSecond;
}

void CSpxHttpAudioStreamSession::ProcessAudio(const DataChunkPtr& audioChunk)
{
    if (m_reco == nullptr)
    {
        SPX_TRACE_ERROR("http reco engine adapter is null.");
        SPX_THROW_HR(SPXERR_RUNTIME_ERROR);
    }
    m_reco->ProcessAudio(audioChunk);

    // need to stop pump after we have seen enough samples.
    if (m_fromMicrophone)
    {
        m_totalAudioinMS += FromBytesToMilisecond(audioChunk->size, m_avgBytesPerSecond);
        if (m_totalAudioinMS >= m_microphoneTimeoutInMS.count() && !m_audioDataCollected)
        {
            SPX_DBG_TRACE_INFO("Collected enough audio samples from microphone.");
            SetFormat(nullptr);
        }
    }
}

void CSpxHttpAudioStreamSession::StopPump()
{
    auto audioPump = m_audioPump;
    if (audioPump && audioPump->GetState() == ISpxAudioPump::State::Processing)
    {
        audioPump->StopPump();
    }
}

VoiceProfilePtr CSpxHttpAudioStreamSession::CreateVoiceProfile(VoiceProfileType type, string&& locale) const
{
    auto reco = SpxCreateObjectWithSite<ISpxHttpRecoEngineAdapter>("CSpxHttpRecoEngineAdapter", GetNonConstSite());
    return reco->CreateVoiceProfile(type, move(locale));
}

RecognitionResultPtr CSpxHttpAudioStreamSession::ModifyVoiceProfile(ModifyOperation operation, VoiceProfileType type, std::string&& id)
{
    auto keepAlive = SpxSharedPtrFromThis<ISpxAudioProcessor>(this);
    RecognitionResultPtr result;
    auto task = CreateTask([this, keepAlive, &result, operation = operation, type = type, id = move(id)]() mutable {
        auto site = SpxSiteFromThis(this);
        auto reco = SpxCreateObjectWithSite<ISpxHttpRecoEngineAdapter>("CSpxHttpRecoEngineAdapter", site);
        result = reco->ModifyVoiceProfile(operation, type, move(id));
        SpxTermAndClear(reco);
        });
    m_threadService->ExecuteSync(move(task));

    return result;
}

VoiceProfilePtr CSpxHttpAudioStreamSession::GetVoiceProfileStatus(VoiceProfileType type, string&& voiceProfileId) const
{
    auto reco = SpxCreateObjectWithSite<ISpxHttpRecoEngineAdapter>("CSpxHttpRecoEngineAdapter", GetNonConstSite());
    return reco->GetVoiceProfileStatus(type, move(voiceProfileId));
}

vector<shared_ptr<ISpxVoiceProfile>> CSpxHttpAudioStreamSession::GetVoiceProfiles(VoiceProfileType type) const
{
    auto reco = SpxCreateObjectWithSite<ISpxHttpRecoEngineAdapter>("CSpxHttpRecoEngineAdapter", GetNonConstSite());
    return reco->GetVoiceProfiles(type);
}

std::shared_ptr<ISpxGenericSite> CSpxHttpAudioStreamSession::GetNonConstSite() const
{
    // cast away the const-ness of the object. This is still a const pointer.
    CSpxHttpAudioStreamSession * const localThis = const_cast<CSpxHttpAudioStreamSession* const>(this);
    return SpxSiteFromThis(localThis);
}

packaged_task<void()> CSpxHttpAudioStreamSession::CreateTask(function<void()> func)
{
    // Creates a packaged task that propagates all exceptions
    // to the user thread and then user callback.
    auto keepAlive = SpxSharedPtrFromThis<ISpxSpeakerRecognition>(this);

    // Catches all exceptions and sends them to the user thread.
    return packaged_task<void()>([this, keepAlive, func]() {
        string error;
        SPXAPI_TRY()
        {
            func();
            return;
        }
        SPXAPI_CATCH_ONLY()
            Error(error);
        });
}


shared_ptr<ISpxNamedProperties> CSpxHttpAudioStreamSession::GetParentProperties() const
{
    return SpxQueryInterface<ISpxNamedProperties>(GetSite());
}

shared_ptr<ISpxRecognitionResult> CSpxHttpAudioStreamSession::CreateIntermediateResult(const wchar_t* text, uint64_t offset, uint64_t duration)
{
    UNUSED(text);
    UNUSED(offset);
    UNUSED(duration);

    SPX_DBG_ASSERT(false);
    return nullptr;
}

std::shared_ptr<ISpxRecognitionResult> CSpxHttpAudioStreamSession::CreateFinalResult(ResultReason reason, NoMatchReason noMatchReason, const wchar_t* text, uint64_t offset, uint64_t duration, const wchar_t*)
{
    auto result = SpxCreateObjectWithSite<ISpxRecognitionResult>("CSpxRecognitionResult", this);
    auto initResult = SpxQueryInterface<ISpxRecognitionResultInit>(result);
    initResult->InitFinalResult(reason, noMatchReason, text, offset, duration);

    return result;
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

std::shared_ptr<ISpxRecognitionResult> CSpxHttpAudioStreamSession::CreateErrorResult(const std::shared_ptr<ISpxErrorInformation>& error)
{
    auto result = SpxCreateObjectWithSite<ISpxRecognitionResult>("CSpxRecognitionResult", this);
    auto initResult = SpxQueryInterface<ISpxRecognitionResultInit>(result);
    initResult->InitErrorResult(error);

    return result;
}

std::shared_ptr<ISpxRecognitionResult> CSpxHttpAudioStreamSession::CreateEndOfStreamResult()
{
    auto result = SpxCreateObjectWithSite<ISpxRecognitionResult>("CSpxRecognitionResult", this);
    auto initResult = SpxQueryInterface<ISpxRecognitionResultInit>(result);
    initResult->InitEndOfStreamResult();

    return result;
}

}}}}
