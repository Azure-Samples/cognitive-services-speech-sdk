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
#include "service_helpers.h"
#include "audio_sys.h"
#include <object_with_site_init_impl.h>

#ifdef WIN32
#include "windows\com_init_and_uninit.h"
#endif

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class CSpxMicrophonePumpBase :
    public ISpxAudioPump,
    public ISpxObjectWithSiteInitImpl<ISpxGenericSite>,
    public ISpxGenericSite,
    public ISpxServiceProvider
{
protected:
    using SinkType = std::shared_ptr<ISpxAudioProcessor>;

public:

    CSpxMicrophonePumpBase();
    virtual ~CSpxMicrophonePumpBase();

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioPump)
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectWithSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxServiceProvider)
        SPX_INTERFACE_MAP_ENTRY(ISpxGenericSite)
    SPX_INTERFACE_MAP_END()

    // ISpxAudioPump interface
    virtual uint16_t GetFormat(SPXWAVEFORMATEX* format, uint16_t size) const override;
    virtual void StartPump(SinkType processor) override;
    virtual void StopPump() override;
    virtual State GetState() const override;
    virtual void SetFormat(const SPXWAVEFORMATEX* format, uint16_t size) override
    {
        UNUSED(format);
        UNUSED(size);

        SPX_THROW_HR(SPXERR_NOT_IMPL);
    }
    virtual void PausePump() override
    {
        SPX_THROW_HR(SPXERR_NOT_IMPL);
    }

    virtual std::string GetPropertyValue(const std::string& key) const override;

    // --- ISpxObjectInit
    virtual void Init() override;
    virtual void Term() override;

    // --- IServiceProvider
    SPX_SERVICE_MAP_BEGIN()
        SPX_SERVICE_MAP_ENTRY_SITE(GetSite())
    SPX_SERVICE_MAP_END()

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
    SPXWAVEFORMATEX m_format;
    AUDIO_SYS_HANDLE m_audioHandle;
    SinkType m_sink;
    mutable std::mutex m_mutex;
    std::condition_variable m_cv;
    const int m_waitMsStartPumpRequestTimeout = 5000;
    const int m_waitMsStopPumpRequestTimeout = 5000;

    int Process(const uint8_t* pBuffer, uint32_t size);

    static void OnInputStateChange(void* pContext, AUDIO_STATE state)
    {
        static_cast<CSpxMicrophonePumpBase*>(pContext)->UpdateState(state);
    }

    static int OnInputWrite(void* pContext, uint8_t* pBuffer, uint32_t size)
    {
        return static_cast<CSpxMicrophonePumpBase*>(pContext)->Process(pBuffer, size);
    }

    uint16_t GetChannelsFromConfig();
    std::string GetDeviceNameFromConfig();
    bool UseEmbeddedSRFromConfig() const;

    // This is for querying any properties needed by audio_create, and validating the values.
    virtual AUDIO_SETTINGS_HANDLE SetOptionsBeforeCreateAudioHandle();

    // This is for querying any properties needed after m_audioHandle is created.
    virtual void SetOptionsAfterCreateAudioHandle();

private:

#ifdef WIN32
    ComInitAndUnInit   m_com;
#endif
};

} } } } // Microsoft::CognitiveServices::Speech::Impl
