//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Globalization;

namespace Microsoft.CognitiveServices.Speech.Audio
{
    /// <summary>
    /// Represents audio stream format used for custom audio input configurations.
    /// </summary>
    public sealed partial class AudioStreamFormat : IDisposable
    {
        /// <summary>
        /// Creates an audio stream format object representing the default microphone input format (16Khz 16bit mono PCM).
        /// </summary>
        /// <returns>The audio stream format being created.</returns>
        public static AudioStreamFormat GetDefaultInputFormat()
        {
            return new AudioStreamFormat(Microsoft.CognitiveServices.Speech.Internal.AudioStreamFormat.DefaultInputFormat);
        }

        /// <summary>
        /// Creates an audio stream format object representing the default speaker output format (16Khz 16bit mono PCM).
        /// Added in version 1.4.0
        /// </summary>
        /// <returns>The audio stream format being created.</returns>
        public static AudioStreamFormat GetDefaultOutputFormat()
        {
            return new AudioStreamFormat(Microsoft.CognitiveServices.Speech.Internal.AudioStreamFormat.DefaultOutputFormat);
        }

        /// <summary>
        /// Creates an audio stream format object with the specified pcm waveformat characteristics.
        /// </summary>
        /// <param name="samplesPerSecond">Sample rate, in samples per second (hertz).</param>
        /// <param name="bitsPerSample">Bits per sample, typically 16.</param>
        /// <param name="channels">Number of channels in the waveform-audio data. Monaural data uses one channel and stereo data uses two channels.</param>
        /// <returns>The audio stream format being created.</returns>
        public static AudioStreamFormat GetWaveFormatPCM(uint samplesPerSecond, byte bitsPerSample, byte channels)
        {
            return new AudioStreamFormat(Microsoft.CognitiveServices.Speech.Internal.AudioStreamFormat.GetWaveFormatPCM(samplesPerSecond, bitsPerSample, channels));
        }

        /// <summary>
        /// Dispose of associated resources.
        /// </summary>
        public void Dispose()
        {
            if (disposed)
            {
                return;
            }

            disposed = true;
        }

        private bool disposed = false;

        internal AudioStreamFormat(Microsoft.CognitiveServices.Speech.Internal.AudioStreamFormat format)
        {
            formatImpl = format;
        }

        internal Microsoft.CognitiveServices.Speech.Internal.AudioStreamFormat formatImpl { get; }
    }
}
