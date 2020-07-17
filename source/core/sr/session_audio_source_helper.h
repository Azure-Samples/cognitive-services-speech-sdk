//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// session_audio_source_helper.h: Implementation declarations for CSpxSessionAudioSourceHelper C++ class
//

// ROBCH: Introduced in AUDIO.V3

#pragma once
#include "spxcore_common.h"
#include "create_object_helpers.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

template <typename T>
class CSpxSessionAudioSourceHelper
{
protected:

    using State = ISpxAudioSource::State;

    CSpxSessionAudioSourceHelper() = default;

    ~CSpxSessionAudioSourceHelper()
    {
        SPX_DBG_ASSERT(m_audioSource == nullptr);
    }

    std::shared_ptr<ISpxAudioSource> EnsureInitAudioSource()
    {
        return m_audioSource != nullptr ? m_audioSource : InitAudioSourceInternal();
    }

    bool IsClear()
    {
        return m_audioSource == nullptr;
    }

    void TermAudioSource()
    {
        SpxTermAndClear(m_audioSource);
        SPX_DBG_ASSERT(m_audioSource == nullptr);
    }

    void EnsureStartAudioSource()
    {
        auto source = EnsureInitAudioSource();
        auto state = source->GetState();
        SPX_IFTRUE(state == State::Idle, StartAudioSource(source));
    }

    void EnsureStopAudioSource()
    {
        auto source = EnsureInitAudioSource();
        if (source != nullptr && (source->GetState() != State::Idle))
        {
            StopAudioSource(source);
        }
    }

private:
    std::shared_ptr<ISpxAudioSource> InitAudioSourceInternal()
    {
        SPX_DBG_ASSERT(m_audioSource == nullptr);
        auto site = static_cast<T*>(this);
        auto source = SpxCreateObjectWithSite<ISpxAudioSourceInit>("CSpxAudioSourceWrapper", site);
        return m_audioSource = SpxQueryInterface<ISpxAudioSource>(source);
    }

    void StartAudioSource(const std::shared_ptr<ISpxAudioSource>& source)
    {
        ISpxInterfaceBase* ptr = static_cast<T*>(this);
        auto notifyMe = ptr->QueryInterface<ISpxAudioSourceNotifyMe>();
        auto control = SpxQueryInterface<ISpxAudioSourceControl>(source);
        control->StartAudio(notifyMe);
    }

    void StopAudioSource(const std::shared_ptr<ISpxAudioSource>& source)
    {
        auto control = SpxQueryInterface<ISpxAudioSourceControl>(source);
        control->StopAudio();
    }

private:
    std::shared_ptr<ISpxAudioSource> m_audioSource;
};

} } } } // Microsoft::CognitiveServices::Speech::Impl
