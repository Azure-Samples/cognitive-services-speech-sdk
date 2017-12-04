//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "stdafx.h"
#include <mfidl.h>
#include <mfapi.h>

#undef WAVE_FORMAT_PCM // already defined in mmeapi.h
#include "audio_sample_handler.h"
#include "../microphone.h"

#undef min

// from mmdeviceapi.h - eCommunications
#define MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_ROLE_ECommunications  2

namespace CARBON_IMPL_NAMESPACE() {

class WMFInitializer {
public:
    WMFInitializer() {
        std::unique_lock<std::mutex> lock(s_mutex);
        if (s_count++ == 0) {
            s_hr = MFStartup(MF_VERSION, MFSTARTUP_FULL);
        }
    }

    ~WMFInitializer() {
        std::unique_lock<std::mutex> lock(s_mutex);
        if (--s_count == 0 && SUCCEEDED(s_hr)) {
            MFShutdown();
        }
    }

private:
    static HRESULT s_hr;
    static long s_count;
    static std::mutex s_mutex;
};

HRESULT WMFInitializer::s_hr;
long WMFInitializer::s_count;
std::mutex WMFInitializer::s_mutex;

class MicrophonePump : public ISpxAudioPump
{
public:

    MicrophonePump();
    ~MicrophonePump();

    virtual uint32_t GetFormat(WAVEFORMATEX* format, uint32_t size) override;

    virtual void SetFormat(const WAVEFORMATEX* format, uint32_t size) override {
        // TODO: create a new sample handler with the given format.
        SPX_THROW_HR(SPXERR_NOT_IMPL);
    }

    virtual void StartPump(std::shared_ptr<ISpxAudioProcessor> processor) override;

    virtual void StopPump() override;

    virtual State GetState() { return m_state.load(); }

    virtual void PausePump() override {
        // TODO: remove from the inteface. Not needed.
        SPX_THROW_HR(SPXERR_NOT_IMPL);
    }
private:

    SPXHR SelectSource();

    std::atomic<State> m_state;
    CComPtr<IMFMediaSource> m_source;
    CComPtr<AudioSampleHandler> m_sampleHandler;

    WMFInitializer m_wmfInit;
};


std::shared_ptr<ISpxAudioPump> Microphone::Create() {
    return std::make_shared<MicrophonePump>();
}

MicrophonePump::MicrophonePump()
    : m_sampleHandler(new AudioSampleHandler()), 
    m_state(State::NoInput)
{
}

MicrophonePump::~MicrophonePump()
{
    if (m_sampleHandler != nullptr)
    {
        m_sampleHandler->Stop();
        m_sampleHandler.Release();
    }

    if (m_source != nullptr)
    {
        m_source->Stop();
        m_source->Shutdown();
    }
}

uint32_t MicrophonePump::GetFormat(WAVEFORMATEX* format, uint32_t size) {
    const auto& micFormat = m_sampleHandler->GetFormat();
    uint32_t totalSize = sizeof(WAVEFORMATEX) + micFormat.cbSize;
    if (format != nullptr) {
        memcpy(format, &micFormat, std::min(totalSize, size));
    }
    return totalSize;
}

void MicrophonePump::StartPump(std::shared_ptr<ISpxAudioProcessor> processor)
{
    SPX_DBG_TRACE_FUNCTION();
    if (processor == nullptr) {
        SPX_THROW_ON_FAIL(SPXERR_INVALID_ARG);
    }
    SPX_INIT_HR(hr);
    State initialState { State::NoInput };
    if (!m_state.compare_exchange_strong(initialState, State::Idle)) {
        SPX_DBG_TRACE_VERBOSE("%s when we're already in %s state",
            __FUNCTION__, (initialState == State::Idle ? "State::Idle" : "State::Processing"));
        return; // already started.
    }

    if (m_source)
    {
        m_source.Detach();
    }

    // Need to reselect source on every audio start to support
    // multiple audio start and stop on the same conversation.
    // TODO: this is what CSP used to do, can we reuse the same source 
    // and just schedule a new read here?
    SPX_EXITFN_ON_FAIL(hr = SelectSource());
    SPX_EXITFN_ON_FAIL(hr = m_sampleHandler->Start(processor));

    // TODO: clarify the semantics of the StartPump method. 
    // If it needs to be blocking, pass a callback to m_sampleHandler
    // and change the state to Processing the first time around ProcessAudio is invoked.
    // Wait here on a CV until the callback's been invoked.
    // TODO: should this method return some result value (success/failure)?
    m_state.store(State::Processing);

SPX_EXITFN_CLEANUP:
    if (SPX_FAILED(hr))
    {
        m_state.store(initialState);
    }
    SPX_THROW_ON_FAIL(hr);
}


SPXHR MicrophonePump::SelectSource()
{
    SPX_INIT_HR(hr);
    CComPtr<IMFAttributes> attributes;
    CComPtr<IMFSourceReader> reader;
    UINT desiredRoles[] =
    {
        MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_ROLE_ECommunications,
    };

    SPX_EXITFN_ON_FAIL(hr = MFCreateAttributes(&attributes, 1));
    for (int role = 0; !m_source && role <= ARRAYSIZE(desiredRoles); ++role)
    {
        ComPtrList<IMFActivate, UINT32> list;

        SPX_EXITFN_ON_FAIL(hr = attributes->DeleteAllItems());

        SPX_EXITFN_ON_FAIL(hr = attributes->SetGUID(
            MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE,
            MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_GUID
        ));

        if (role < ARRAYSIZE(desiredRoles))
        {
            SPX_EXITFN_ON_FAIL(hr = attributes->SetUINT32(
                MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_ROLE,
                desiredRoles[role]
            ));
        }

        SPX_EXITFN_ON_FAIL(hr = MFEnumDeviceSources(attributes, &list, list.size_ptr()));

        for (int i = 0; i < (int)list.size(); ++i)
        {
            if (SUCCEEDED(list[i]->ActivateObject(IID_PPV_ARGS(&m_source))))
            {
                break;
            }
        }
    }

    SPX_IFTRUE_EXITFN_WHR(!m_source, hr = SPXERR_MIC_NOT_AVAILABLE);
    SPX_EXITFN_ON_FAIL(hr = attributes->DeleteAllItems());
    SPX_EXITFN_ON_FAIL(hr = attributes->SetUnknown(MF_SOURCE_READER_ASYNC_CALLBACK, static_cast<IMFSourceReaderCallback*>(m_sampleHandler.p)));

    SPX_EXITFN_ON_FAIL(hr = MFCreateSourceReaderFromMediaSource(m_source, attributes, &reader));

    // disable all streams by default
    SPX_EXITFN_ON_FAIL(hr = reader->SetStreamSelection((DWORD)MF_SOURCE_READER_ALL_STREAMS, FALSE));

    // select the first audio stream
    SPX_EXITFN_ON_FAIL(hr = m_sampleHandler->SetStreamReader(reader, (DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM));

SPX_EXITFN_CLEANUP:
    return hr;
}


void MicrophonePump::StopPump()
{
    SPX_DBG_TRACE_FUNCTION();
    State initialState { State::Processing };
    if (!m_state.compare_exchange_strong(initialState, State::NoInput)) {
        SPX_DBG_TRACE_VERBOSE("%s when we're already in %s state", 
            __FUNCTION__, (initialState == State::NoInput ? "State::NoInput" : "State::Idle"));
        return; // not running (could be in the no-input initialization phase).
    }

    m_sampleHandler->Stop();
    m_source->Stop();
}

}; // CARBON_IMPL_NAMESPACE()


