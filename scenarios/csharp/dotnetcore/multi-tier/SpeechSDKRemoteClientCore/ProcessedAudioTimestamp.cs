//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;

namespace SpeechSDKSamples.Remote.Client.Core
{
    /// <summary>
    /// Represents a timestamp for a processed audio chunk, including when it was received
    /// and how much audio data remains to be processed.
    /// </summary>
    public sealed class ProcessedAudioTimestamp
    {
        /// <summary>
        /// Initializes a new instance of the ProcessedAudioTimestamp class.
        /// </summary>
        /// <param name="chunkTimeStamp">The time when the audio chunk was received.</param>
        /// <param name="remaining">The duration of remaining audio data to be processed.</param>
        /// <exception cref="ArgumentException">Thrown when remaining is negative.</exception>
        public ProcessedAudioTimestamp(DateTime chunkTimeStamp, TimeSpan remaining)
        {
            if (remaining < TimeSpan.Zero)
                throw new ArgumentException("Remaining audio duration cannot be negative", nameof(remaining));

            ChunkReceivedTime = chunkTimeStamp;
            RemainingAudioInTicks = remaining;
        }

        /// <summary>
        /// Gets the time when the audio chunk was received.
        /// </summary>
        public DateTime ChunkReceivedTime { get; }

        /// <summary>
        /// Gets the duration of remaining audio data to be processed.
        /// </summary>
        public TimeSpan RemainingAudioInTicks { get; }
    }
}
