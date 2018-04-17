//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "stdafx.h"
#include <mfidl.h>
#include <mfapi.h>

#undef WAVE_FORMAT_PCM // already defined in mmeapi.h

#include "audio_sample_handler.h"
#include "microphone.h"
#include "ispxinterfaces.h"
#include "interface_helpers.h"

#undef min

// from mmdeviceapi.h - eCommunications
#define MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_ROLE_ECommunications  2

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

using namespace std;

class WMFInitializer {
public:
    WMFInitializer() {
        unique_lock<mutex> lock(s_mutex);
        if (s_count++ == 0) {
            SPX_THROW_ON_FAIL_IF_NOT(CoInitializeEx(0, COINIT_APARTMENTTHREADED), RPC_E_CHANGED_MODE);
            SPX_THROW_ON_FAIL(MFStartup(MF_VERSION, MFSTARTUP_FULL));
        }
    }

    ~WMFInitializer() {
        unique_lock<mutex> lock(s_mutex);
        if (--s_count == 0) {
            MFShutdown();
            CoUninitialize();
        }
    }

private:
    static long s_count;
    static mutex s_mutex;
};

long WMFInitializer::s_count { 0 };
mutex WMFInitializer::s_mutex;

// TODO (alrezni): factor out a shared base with Linux implementation. 
// Replace Win-specific bits with audio_sys from azure-c-shared.
class MicrophonePump : public ISpxAudioPump
{
public:

    MicrophonePump();
    ~MicrophonePump();

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioPump)
    SPX_INTERFACE_MAP_END()

    virtual uint16_t GetFormat(WAVEFORMATEX* format, uint16_t size) override;

    virtual void SetFormat(const WAVEFORMATEX* format, uint16_t size) override {
        UNUSED(format);
        UNUSED(size);
        // TODO: create a new sample handler with the given format.
        SPX_THROW_HR(SPXERR_NOT_IMPL);
    }

    virtual void StartPump(shared_ptr<ISpxAudioProcessor> processor) override;

    virtual void StopPump() override;

    virtual State GetState() { return m_state.load(); }

    virtual void PausePump() override {
        // TODO: remove from the inteface. Not needed.
        SPX_THROW_HR(SPXERR_NOT_IMPL);
    }
private:

    void SelectSource();

    atomic<State> m_state;
    CComPtr<IMFMediaSource> m_source;
    CComPtr<AudioSampleHandler> m_sampleHandler;

    WMFInitializer m_wmfInit;
};

shared_ptr<ISpxAudioPump> Microphone::Create() 
{
    return make_shared<MicrophonePump>();
}

MicrophonePump::MicrophonePump(): 
    m_sampleHandler(new AudioSampleHandler()), 
    m_state(State::NoInput)
{
    SelectSource();
}

MicrophonePump::~MicrophonePump()
{
    StopPump();

    if (m_source != nullptr)
    {
        m_source->Stop();
        m_source->Shutdown();
    }
}

uint16_t MicrophonePump::GetFormat(WAVEFORMATEX* format, uint16_t size) 
{
    const auto& micFormat = m_sampleHandler->GetFormat();
    uint16_t totalSize = sizeof(WAVEFORMATEX) + micFormat.cbSize;
    if (format != nullptr) {
        memcpy(format, &micFormat, min(totalSize, size));
    }
    return totalSize;
}

void MicrophonePump::SelectSource()
{
    CComPtr<IMFAttributes> attributes;
    CComPtr<IMFSourceReader> reader;
    UINT desiredRoles[] =
    {
        MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_ROLE_ECommunications,
    };

    SPX_THROW_ON_FAIL(MFCreateAttributes(&attributes, 1));
    for (int role = 0; !m_source && role <= ARRAYSIZE(desiredRoles); ++role)
    {
        ComPtrList<IMFActivate, UINT32> list;

        SPX_THROW_ON_FAIL(attributes->DeleteAllItems());

        SPX_THROW_ON_FAIL(attributes->SetGUID(
            MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE,
            MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_GUID
        ));

        if (role < ARRAYSIZE(desiredRoles))
        {
            SPX_THROW_ON_FAIL(attributes->SetUINT32(
                MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_ROLE,
                desiredRoles[role]
            ));
        }

        SPX_THROW_ON_FAIL(MFEnumDeviceSources(attributes, &list, list.size_ptr()));

        for (int i = 0; i < (int)list.size(); ++i)
        {
            if (SUCCEEDED(list[i]->ActivateObject(IID_PPV_ARGS(&m_source))))
            {
                break;
            }
        }
    }

    SPX_THROW_HR_IF(SPXERR_MIC_NOT_AVAILABLE, !m_source);
    SPX_THROW_ON_FAIL(attributes->DeleteAllItems());
    SPX_THROW_ON_FAIL(attributes->SetUnknown(MF_SOURCE_READER_ASYNC_CALLBACK, static_cast<IMFSourceReaderCallback*>(m_sampleHandler.p)));

    SPX_THROW_ON_FAIL(MFCreateSourceReaderFromMediaSource(m_source, attributes, &reader));

    // disable all streams by default
    SPX_THROW_ON_FAIL(reader->SetStreamSelection((DWORD)MF_SOURCE_READER_ALL_STREAMS, FALSE));

    // select the first audio stream
    SPX_THROW_ON_FAIL(m_sampleHandler->SetStreamReader(reader, (DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM));
}

void MicrophonePump::StartPump(shared_ptr<ISpxAudioProcessor> processor)
{
    SPX_DBG_TRACE_FUNCTION();
    SPX_IFTRUE_THROW_HR(processor == nullptr, SPXERR_INVALID_ARG);
    SPX_INIT_HR(hr);
    State initialState{ State::NoInput };
    if (!m_state.compare_exchange_strong(initialState, State::Idle)) 
    {
        SPX_DBG_TRACE_VERBOSE("%s when we're already in %s state",
            __FUNCTION__, (initialState == State::Idle ? "State::Idle" : "State::Processing"));
        return; // already started.
    }

    SPX_EXITFN_ON_FAIL(hr = m_sampleHandler->Start(processor));

    // TODO: clarify the semantics of the StartPump method. 
    // If it needs to be blocking, pass a callback to m_sampleHandler
    // and change the state to Processing the first time around ProcessAudio is invoked.
    // Wait here on a CV until the callback's been invoked.
    m_state.store(State::Processing);

SPX_EXITFN_CLEANUP:
    if (SPX_FAILED(hr))
    {
        m_state.store(initialState);
    }
    SPX_THROW_ON_FAIL(hr);
}

void MicrophonePump::StopPump()
{
    SPX_DBG_TRACE_FUNCTION();
    State initialState { State::Processing };
    if (!m_state.compare_exchange_strong(initialState, State::NoInput))
    {
        SPX_DBG_TRACE_VERBOSE("%s when we're already in %s state", 
            __FUNCTION__, (initialState == State::NoInput ? "State::NoInput" : "State::Idle"));
        return; // not running (could be in the no-input initialization phase).
    }

    m_sampleHandler->Stop();
}

} } } } // Microsoft::CognitiveServices::Speech::Impl


