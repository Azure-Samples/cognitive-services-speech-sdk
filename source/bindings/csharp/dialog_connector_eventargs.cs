//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
using System;
using System.Text;
using Microsoft.CognitiveServices.Speech.Audio;
using static Microsoft.CognitiveServices.Speech.Internal.SpxExceptionThrower;
using I = Microsoft.CognitiveServices.Speech.Internal;

namespace Microsoft.CognitiveServices.Speech.Dialog
{
    /// <summary>
    /// Class for activity received event arguments.
    /// Added in version 1.5.0
    /// </summary>
    public sealed class ActivityReceivedEventArgs
    {
        internal I.InteropSafeHandle eventHandle;

        internal ActivityReceivedEventArgs(IntPtr eventHandlePtr)
        {
            ThrowIfNull(eventHandlePtr);
            eventHandle = new I.InteropSafeHandle(eventHandlePtr, Internal.ActivityReceivedEventArgs.dialog_connector_activity_received_event_release);

            IntPtr activityPtr = IntPtr.Zero;
            ThrowIfFail(Internal.ActivityReceivedEventArgs.dialog_connector_activity_received_event_get_activity(eventHandle, out activityPtr));
            uint size = 0;
            ThrowIfFail(Internal.Activity.activity_serialized_size(activityPtr, out size));
            var buffer = new StringBuilder((int)(size + 1));
            ThrowIfFail(Internal.Activity.activity_serialize(activityPtr, buffer, size + 1));
            Activity = buffer.ToString();
            ThrowIfFail(Internal.Activity.activity_handle_release(activityPtr));
            HasAudio = Internal.ActivityReceivedEventArgs.dialog_connector_activity_received_event_has_audio(eventHandle);
            if (HasAudio)
            {
                IntPtr audioPtr = IntPtr.Zero;
                ThrowIfFail(Internal.ActivityReceivedEventArgs.dialog_connector_activity_received_event_get_audio(eventHandle, out audioPtr));
                Audio = new PullAudioOutputStream(audioPtr);
            }
        }

        /// <summary>
        /// Gets the activity associated with the event as a serialized json.
        /// </summary>
        /// <returns>The activity.</returns>
        public string Activity { get; }

        /// <summary>
        /// Checks if the event contains audio.
        /// </summary>
        public bool HasAudio { get; }

        /// <summary>
        /// Gets the audio associated with the event.
        /// </summary>
        public PullAudioOutputStream Audio { get; }
    }
}
