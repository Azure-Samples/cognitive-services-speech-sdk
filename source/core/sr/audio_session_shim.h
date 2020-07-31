//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// audio_session_shim.h: Definitions for the shim to interface an AudioSource with the AudioStreamSession
//

#pragma once

#include <memory>
#include <list>
#include <deque>
#include "spxcore_common.h"
#include <ispxinterfaces.h>
#include <mutex>

#include <audio_source_init_delegate_impl.h>
#include <interface_helpers.h>

#include "session_audio_source_helper.h"
#include "audio_source_notify_me_track_delegate_impl.h"
#include <object_with_site_init_impl.h>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class CSpxAudioSessionShim:
    public ISpxGenericSite,
    /* Interface that defines what a shim is */
    public ISpxAudioSessionShim,
    /* Deals with object initialization and termination of objects while providing helpers for sites */
    public ISpxObjectWithSiteInitImpl<ISpxGenericSite>,
    /* Enables creation of audio sources from Mic, File and Stream */
    public ISpxAudioSourceInitDelegateImpl<>,
    /* Enables the AudioSourceNotifyMe interface and provide utilites from tracking it */
    public ISpxAudioSourceNotifyMeTrackDelegateImpl<>,
    public ISpxServiceProvider,
    /* Provides utilities for Initialization and Termination of AudioSources */
    public CSpxSessionAudioSourceHelper<CSpxAudioSessionShim>
{
public:
    CSpxAudioSessionShim() = default;
    virtual ~CSpxAudioSessionShim() = default;

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxGenericSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectWithSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioSessionShim)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioSourceInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioSourceNotifyMe)
        SPX_INTERFACE_MAP_ENTRY(ISpxServiceProvider)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioSourceNotifyMe)
    SPX_INTERFACE_MAP_END()

    SPX_SERVICE_MAP_BEGIN()
    SPX_SERVICE_MAP_ENTRY_SITE(GetSite())
    SPX_SERVICE_MAP_END()

    // ISpxObjectInit
    void Init() final;
    void Term() final;

    // ISpxAudioSessionShim
    void StartAudio() final;
    void StopAudio() final;

    inline SpxWaveFormatEx GetFormat() final
    {
        auto src = EnsureInitAudioSource();
        return src->GetFormat();
    }


protected:
    inline void AudioSourceStarted() final
    {
        auto site = GetSite();
        auto ptr = site->QueryInterface<ISpxAudioProcessor>();
        if (ptr != nullptr)
        {
            auto format = GetFormat();
            ptr->SetFormat(format.get());
        }
    }

    inline void AudioSourceStopped() final
    {
        auto site = GetSite();
        auto ptr = site->QueryInterface<ISpxAudioProcessor>();
        if (ptr != nullptr)
        {
            ptr->SetFormat(nullptr);
        }
    }

    void AudioSourceDataAvailable(bool /* first */) final;

    inline void AudioSourceEndOfStreamDetected() final
    {


    }

protected:
    void InitDelegatePtr(std::shared_ptr<ISpxAudioSourceInit>& ptr) final;
    void InitDelegatePtr(std::shared_ptr<ISpxAudioSourceNotifyMe>& ptr) final;

private:
    /* Site stuff */
    std::shared_ptr<ISpxGenericSite> m_siteKeepAlive;

    void InitSiteKeepAlive();
    void TermSiteKeepAlive();

    /* Audio source stuff */
    using AudioSourceInitDelegate = ISpxAudioSourceInitDelegateImpl<>;


    void TermAudioSource();
};

} } } }
