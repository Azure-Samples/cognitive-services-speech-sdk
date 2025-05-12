//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace SpeechSDKSamples.Remote.Core
{
    /// <summary>
    /// A struct describint a constant bit rate audio format.
    /// </summary>
    public struct AudioFormat
    {
        /// <summary>
        /// Bits per sample.
        /// </summary>
        public int BitsPerSample { get; set; }

        /// <summary>
        /// Samples per second
        /// </summary>
        public int SamplesPerSecond { get; set; }

        /// <summary>
        ///  Number of channels.
        /// </summary>
        public int ChannelCount { get; set; }

        /// <summary>
        /// Total bit rate.
        /// </summary>
        public int AverageBytesPerSecond { get => BitsPerSample / 8 * SamplesPerSecond * ChannelCount; }
    }
}
