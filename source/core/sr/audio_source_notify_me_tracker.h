//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// audio_source_notify_me_tracker.h: Implementation declarations for CSpxAudioSourceNotifyMeTracker C++ class
//

// ROBCH: Introduced in AUDIO.V3

#pragma once
#include "spxcore_common.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class CSpxAudioSourceNotifyMeTracker
{
public:

    using State = ISpxAudioSource::State;

    void Reset()
    {
        *this = CSpxAudioSourceNotifyMeTracker();
    }

    void TrackNotifyMe(const std::shared_ptr<ISpxAudioSource>& source, const std::shared_ptr<ISpxBufferData>& data)
    {
        m_source = source;
        m_data = data;

        auto newState = source->GetState();
        auto oldState = m_state;
        m_state = newState;


        auto newBytesReady = data != nullptr ? data->GetBytesReady() : 0;
        auto oldBytesReady = m_bytesReady;
        m_bytesReady = newBytesReady;

        if (oldState != newState || oldBytesReady != newBytesReady)
        {
            OnStateChange(oldState, newState, newBytesReady);
        }
    }

protected:

    virtual void AudioSourceStarted() { }
    virtual void AudioSourceStopped() { }
    virtual void AudioSourceDataAvailable(bool first) { UNUSED(first);  }
    virtual void AudioSourceEndOfStreamDetected() { }

private:

    void OnStateChange(State oldState, State newState, uint64_t bytesAvailable)
    {
        //SPX_DBG_ASSERT((oldState != newState) || (bytesAvailable > 0));
        switch (newState)
        {
            case State::Idle:
                OnAudioSourceIdle(oldState, newState);
                break;

            case State::Started:
                OnAudioSourceStarted(oldState, newState);
                break;

            case State::DataAvailable:
                OnAudioSourceDataAvailable(oldState, newState, bytesAvailable);
                break;

            case State::EndOfStream:
                OnAudioSourceEndOfStream(oldState, newState);
                break;
        }
    }

    void OnAudioSourceIdle(State oldState, State newState)
    {
        (void)oldState;
        (void)newState;
        SPX_DBG_ASSERT(newState == State::Idle);
        SPX_DBG_ASSERT(oldState != State::Idle);
        AudioSourceStopped();
    }

    void OnAudioSourceStarted(State oldState, State newState)
    {
        (void)oldState;
        (void)newState;
        SPX_DBG_ASSERT(newState == State::Started);
        SPX_DBG_ASSERT(oldState == State::Idle);
        AudioSourceStarted();
    }

    void OnAudioSourceDataAvailable(State oldState, State newState, uint64_t bytesAvailable)
    {
        UNUSED(bytesAvailable);
        UNUSED(newState);
        UNUSED(oldState);
        /*SPX_DBG_ASSERT(newState == State::DataAvailable);
        SPX_DBG_ASSERT(oldState != State::Idle);
        SPX_DBG_ASSERT(oldState != State::EndOfStream);*/
        AudioSourceDataAvailable(oldState == State::Started);
    }

    void OnAudioSourceEndOfStream(State oldState, State newState)
    {
        SPX_DBG_ASSERT(newState == State::EndOfStream);
        SPX_DBG_ASSERT(oldState != State::EndOfStream);
        UNUSED(newState);
        UNUSED(oldState);
        AudioSourceEndOfStreamDetected();
    }

protected:

    State m_state { State::Idle };
    uint64_t m_bytesReady { 0 };

    std::shared_ptr<ISpxAudioSource> m_source;
    std::shared_ptr<ISpxBufferData> m_data;
};

} } } } // Microsoft::CognitiveServices::Speech::Impl
