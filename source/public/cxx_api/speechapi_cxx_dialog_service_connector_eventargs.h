//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#pragma once

#include <memory>
#include <vector>

#include <speechapi_cxx_audio_stream.h>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Dialog {

/// <summary>
/// Class for activity received event arguments.
/// </summary>
class ActivityReceivedEventArgs: public std::enable_shared_from_this<ActivityReceivedEventArgs>
{
public:
    friend DialogServiceConnector;
    /// <summary>
    /// Releases the event.
    /// </summary>
    inline ~ActivityReceivedEventArgs()
    {
        SPX_THROW_ON_FAIL(::dialog_service_connector_activity_received_event_release(m_handle));
    }

    /// <summary>
    /// Gets the activity associated with the event.
    /// </summary>
    /// <returns>The serialized activity activity.</returns>
    inline std::string GetActivity() const
    {
        size_t size;
        SPX_THROW_ON_FAIL(::dialog_service_connector_activity_received_event_get_activity_size(m_handle, &size));
        auto ptr = std::make_unique<char[]>(size + 1);
        SPX_THROW_ON_FAIL(::dialog_service_connector_activity_received_event_get_activity(m_handle, ptr.get(), size + 1));
        return std::string{ ptr.get() };
    }

    /// <summary>
    /// Gets the audio associated with the event.
    /// </summary>
    /// <returns>The audio.</returns>
    inline std::shared_ptr<Audio::PullAudioOutputStream> GetAudio() const
    {
        SPXAUDIOSTREAMHANDLE h_audio{ SPXHANDLE_INVALID };
        SPX_THROW_ON_FAIL(::dialog_service_connector_activity_received_event_get_audio(m_handle, &h_audio));
        if (h_audio == SPXHANDLE_INVALID)
        {
            return nullptr;
        }
        return std::shared_ptr<Audio::PullAudioOutputStream>(new Audio::PullAudioOutputStream(h_audio) );
    }

    /// <summary>
    /// Checks if the event contains audio.
    /// </summary>
    /// <returns>True if the event contains audio, false otherwise.</returns>
    inline bool HasAudio() const
    {
        return ::dialog_service_connector_activity_received_event_has_audio(m_handle);
    }
private:
    /*! \cond PROTECTED */
    inline ActivityReceivedEventArgs(SPXEVENTHANDLE h_event) : m_handle{ h_event }
    {
    }

    SPXEVENTHANDLE m_handle;
    /*! \endcond */
};

} } } }
