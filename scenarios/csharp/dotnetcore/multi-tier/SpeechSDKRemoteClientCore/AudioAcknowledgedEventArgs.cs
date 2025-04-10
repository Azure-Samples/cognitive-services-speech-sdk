//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;

namespace SpeechSDKSamples.Remote.Client.Core
{
    /// <summary>
    /// Event arguments raised when the service has acknowledged processing audio up to a specific offset.
    /// This acknowledgment allows the client to manage its audio buffer by discarding processed data.
    /// </summary>
    public sealed class AudioAcknowledgedEventArgs : EventArgs
    {
        /// <summary>
        /// Gets the time offset up to which the audio has been processed by the service.
        /// This offset is relative to the start of the audio stream.
        /// </summary>
        /// <remarks>
        /// Audio data before this offset can be safely discarded as it has been fully processed
        /// by the service and will not need to be replayed in case of connection issues.
        /// </remarks>
        public TimeSpan AcknowledgedAudio { get; }

        /// <summary>
        /// Initializes a new instance of the AudioAcknowledgedEventArgs class.
        /// </summary>
        /// <param name="offset">The time offset up to which audio has been processed.
        /// This should be a non-negative TimeSpan value.</param>
        /// <exception cref="ArgumentException">Thrown when offset is negative.</exception>
        public AudioAcknowledgedEventArgs(TimeSpan offset)
        {
            if (offset < TimeSpan.Zero)
                throw new ArgumentException("Audio acknowledgment offset cannot be negative.", nameof(offset));

            AcknowledgedAudio = offset;
        }
    }
}
