//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
#pragma once
#include "stdafx.h"

#include <memory>
#include "spx_namespace.h"
#include "ispxinterfaces.h"
#include "interface_helpers.h"
#include "audio_sys.h"
#ifdef WIN32
#include "windows\com_init_and_uninit.h"
#endif

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class MicrophonePumpBase : public ISpxAudioPump
{
protected:
    using SinkType = std::shared_ptr<ISpxAudioProcessor>;

public:

    MicrophonePumpBase();
    ~MicrophonePumpBase();

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioPump)
    SPX_INTERFACE_MAP_END()

    // ISpxAudioPump interface
    virtual uint16_t GetFormat(WAVEFORMATEX* format, uint16_t size) override;
    virtual void StartPump(SinkType processor) override;
    virtual void StopPump() override;
    virtual State GetState() override;
    virtual void SetFormat(const WAVEFORMATEX* format, uint16_t size) override
    {
        UNUSED(format);
        UNUSED(size);

        SPX_THROW_HR(SPXERR_NOT_IMPL);
    }
    virtual void PausePump() override
    {
        SPX_THROW_HR(SPXERR_NOT_IMPL);
    }

public:
    //------------------------------------
    // helpers
    // UpdateStates needs to be public so that the c callback can access it.
    void UpdateState(AUDIO_STATE state);

private:
    class ReleaseSink
    {
    public:
        ReleaseSink(SinkType& sink)
            :m_sink(sink)
        {}

        ~ReleaseSink()
        {
            m_sink.reset();
        }
        SinkType& m_sink;
    };

protected:
    
    State m_state;
    const WAVEFORMATEX m_format;
    AUDIO_SYS_HANDLE m_audioHandle;
    SinkType m_sink;
    std::mutex m_mutex;
    std::condition_variable m_cv;
    const int m_waitMsStartPumpRequestTimeout = 5000;
    const int m_waitMsStopPumpRequestTimeout = 5000;

private:

#ifdef WIN32
    ComInitAndUnInit   m_com;
#endif

};
     
} } } } // Microsoft::CognitiveServices::Speech::Impl

