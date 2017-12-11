#include "stdafx.h"
#include <windows.h>
#include <mfapi.h>
#include <mfidl.h>
#include "sample_handler.h"

#undef WAVE_FORMAT_PCM // already defined in mmeapi.h
#include "ispxinterfaces.h"

namespace CARBON_IMPL_NAMESPACE() {

using namespace std;

SampleHandler::SampleHandler(GUID major, GUID subType)
    : m_majorType(major), m_subType(subType), m_streamIndex(0)
{
}

//
//  OnReadSample()
//
//  Implementation of SampleHandler::OnReadSample().  When a sample is ready, add it to the sample queue.
//
// This is a callback, executed on the thread owned/controlled by the MFF.
HRESULT SampleHandler::OnReadSample(
    _In_ HRESULT /*hrStatus*/, 
    _In_ DWORD /*dwStreamIndex*/, 
    _In_ DWORD /*dwStreamFlags*/, 
    _In_ LONGLONG /*llTimestamp*/, 
    _In_opt_ IMFSample *pSample)
{
    Process(pSample);
    QueueRead();
    SPX_RETURN_HR(SPX_NOERROR);
}

SPXHR SampleHandler::Start(const Sink_Type& sink)
{
    Sink_Type prevSink(sink);
    {
        unique_lock<mutex> lock(m_mutex);
        // Somebody already called start. Second time around we throw.
        SPX_IFTRUE_THROW_HR((m_sink != nullptr), SPXERR_AUDIO_IS_PUMPING);
        // Sink is nullptr, wait until the currently scheduled read operation completes, 
        // before scheduling a new one.
        m_cv.wait(lock, [&] { return !m_readInProgress; });
        
        SPX_ASSERT_WITH_MESSAGE(!m_readInProgress, "@ %s", __FUNCTION__);
        m_sink = sink;
        m_readInProgress = true;
    }

    const auto& format = GetFormat();
    sink->SetFormat(const_cast<WAVEFORMATEX*>(&format));

    // we're transitioning from non-running to running, 
    // kick off the first ReadSample.
    return QueueRead();
}

void SampleHandler::Stop() {
    Sink_Type prevSink;
    {
        unique_lock<mutex> lock(m_mutex);
        m_sink.swap(prevSink);
        if (prevSink == nullptr) {
            // already stopped.
            SPX_ASSERT_WITH_MESSAGE(!m_readInProgress, "@ %s", __FUNCTION__);
            return;
        }
    }

    // Let the sink know we're done for now... 
    // TODO: this should be a dedicated method.
    prevSink->SetFormat(nullptr);
}

SPXHR SampleHandler::QueueRead()
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, m_reader == nullptr);
    SPX_INIT_HR(hr);
    Sink_Type sink;
    {
        unique_lock<mutex> lock(m_mutex);
        SPX_ASSERT_WITH_MESSAGE(m_readInProgress, "@ %s", __FUNCTION__);
        if (!m_sink) 
        {
            // somebody called stop.
            m_readInProgress = false;
            m_cv.notify_all();
            return SPXERR_ABORT;
        }
    }

    hr = m_reader->ReadSample(m_streamIndex, 0, NULL, NULL, NULL, NULL);

    if (!SUCCEEDED(hr))
    {
        SPX_TRACE_VERBOSE("%s: ReadSample failed", __FUNCTION__);
        unique_lock<mutex> lock(m_mutex);
        // In case, read failed, unset the sink, so that the next call to Start 
        // won't fail without an explicit call to Stop.
        m_sink.reset();
        m_readInProgress = false;
        m_cv.notify_all();
    }

    return hr;
}

SPXHR SampleHandler::Process(IMFSample* sample)
{
    // the first time after Start() and the last time after Stop() 
    // this callback is invoked with a nullptr. Safe to ignore.
    SPX_RETURN_HR_IF(SPX_NOERROR, sample == nullptr);

    Sink_Type sink;
    {
        unique_lock<mutex> lock(m_mutex);
        sink = m_sink;
        if (!sink) {
            return SPXERR_ABORT;
        }
        SPX_ASSERT_WITH_MESSAGE(m_readInProgress, "@ %s", __FUNCTION__);
    }

    SPX_INIT_HR(hr);
    CComPtr<IMFMediaBuffer> mediaBuffer;
    BYTE* audioData = nullptr;
    DWORD numBytes = 0;

    SPX_EXITFN_ON_FAIL(hr = sample->ConvertToContiguousBuffer(&mediaBuffer));

    // Lock the sample
    SPX_EXITFN_ON_FAIL(hr = mediaBuffer->Lock(&audioData, NULL, &numBytes));

    if (audioData != nullptr) {
        // Is this really a good idea to create a copy of the audio data
        // here? It seems, the sink should be capable of creating a copy if it 
        // needs to.
        // TODO: maybe change the ISpxAudioProcessor::ProcessAudio signature to
        // take a const ptr?
        ISpxAudioProcessor::AudioData_Type copy(new uint8_t[numBytes]);
        memcpy(copy.get(), audioData, numBytes);
        sink->ProcessAudio(copy, numBytes);
    }

SPX_EXITFN_CLEANUP:
    // Unlock the buffer
    SPXHR unlock_hr = mediaBuffer->Unlock();
    hr = (SPX_SUCCEEDED(hr)) ? unlock_hr : hr;
    audioData = nullptr;
    return hr;
}

//
//  Create an audio type.
//
SPXHR SampleHandler::CreateMediaType(_Inout_ CComPtr<IMFMediaType>& mediaType)
{
    CComPtr<IMFMediaType> partialMT;

    // Create a partial media type (specifies uncompressed PCM audio).
    SPX_IFFAILED_RETURN_HR(MFCreateMediaType(&partialMT));
    SPX_IFFAILED_RETURN_HR(partialMT->SetGUID(MF_MT_MAJOR_TYPE, m_majorType));
    SPX_IFFAILED_RETURN_HR(partialMT->SetGUID(MF_MT_SUBTYPE, m_subType));

    const auto& format = GetFormat();

    SPX_IFFAILED_RETURN_HR(partialMT->SetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, format.wBitsPerSample));
    SPX_IFFAILED_RETURN_HR(partialMT->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, format.nSamplesPerSec));
    SPX_IFFAILED_RETURN_HR(partialMT->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, format.nChannels));
    SPX_IFFAILED_RETURN_HR(partialMT->SetUINT32(MF_MT_AUDIO_BLOCK_ALIGNMENT, format.nBlockAlign));

    mediaType = partialMT;

    SPX_RETURN_HR(SPX_NOERROR);
}

//
//  Create an audio type.
//
SPXHR SampleHandler::ConfigureStream(_In_ IMFSourceReader* reader, DWORD streamIndex)
{
    m_audioType.Detach();

    CComPtr<IMFMediaType> desireMT;
    SPX_IFFAILED_RETURN_HR(CreateMediaType(desireMT));
    // Set type on source reader, so the necessary converters / decoders will be added.
    SPX_IFFAILED_RETURN_HR(reader->SetCurrentMediaType(streamIndex, nullptr, desireMT));

    CComPtr<IMFMediaType> currentMT;
    // Get the complete uncompressed format.
    SPX_IFFAILED_RETURN_HR(reader->GetCurrentMediaType(streamIndex, &currentMT));

    SPX_IFFAILED_RETURN_HR(reader->SetStreamSelection(streamIndex, true));

    m_streamIndex = streamIndex;
    m_audioType = currentMT;

    SPX_RETURN_HR(SPX_NOERROR);
}


SPXHR SampleHandler::SetStreamReader(_In_ IMFSourceReader* reader, DWORD streamIndex)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, reader == nullptr);

    SPX_IFFAILED_RETURN_HR(ConfigureStream(reader, streamIndex));

    reader->AddRef();
    m_reader.Attach(reader);

    SPX_RETURN_HR(SPX_NOERROR);
}

HRESULT SampleHandler::OnFlush(
    _In_  DWORD /* dwStreamIndex */)
{
    return E_NOTIMPL;
}

HRESULT SampleHandler::OnEvent(
    _In_  DWORD          /* dwStreamIndex */,
    _In_  IMFMediaEvent* /*pEvent */)
{
    return E_NOTIMPL;
}

}; // CARBON_IMPL_NAMESPACE()
