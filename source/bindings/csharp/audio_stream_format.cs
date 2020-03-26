//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using Microsoft.CognitiveServices.Speech.Internal;
using static Microsoft.CognitiveServices.Speech.Internal.SpxExceptionThrower;

namespace Microsoft.CognitiveServices.Speech.Audio
{
    /// <summary>
    /// Represents audio stream format used for custom audio input configurations.
    /// Updated in version 1.5.0.
    /// </summary>
    public sealed partial class AudioStreamFormat : IDisposable
    {
        /// <summary>
        /// Creates an audio stream format object representing the default microphone input format (16 kHz, 16 bit, mono PCM).
        /// </summary>
        /// <returns>The audio stream format being created.</returns>
        public static AudioStreamFormat GetDefaultInputFormat()
        {
            IntPtr streamFormatHandle = IntPtr.Zero;
            ThrowIfFail(Internal.AudioStreamFormat.audio_stream_format_create_from_default_input(out streamFormatHandle));
            return new AudioStreamFormat(streamFormatHandle);
        }

        /// <summary>
        /// Creates an audio stream format object representing the default speaker output format (16 kHz, 16 bit, mono PCM).
        /// Added in version 1.4.0
        /// </summary>
        /// <returns>The audio stream format being created.</returns>
        public static AudioStreamFormat GetDefaultOutputFormat()
        {
            IntPtr streamFormatHandle = IntPtr.Zero;
            ThrowIfFail(Internal.AudioStreamFormat.audio_stream_format_create_from_default_output(out streamFormatHandle));
            return new AudioStreamFormat(streamFormatHandle);
        }

        /// <summary>
        /// Creates an audio stream format object with the specified PCM waveformat characteristics.
        /// </summary>
        /// <param name="samplesPerSecond">Sample rate, in samples per second (Hertz).</param>
        /// <param name="bitsPerSample">Bits per sample.</param>
        /// <param name="channels">Number of channels in the waveform-audio data.</param>
        /// <returns>The audio stream format being created.</returns>
        public static AudioStreamFormat GetWaveFormatPCM(uint samplesPerSecond, byte bitsPerSample, byte channels)
        {
            IntPtr streamFormatHandle = IntPtr.Zero;
            ThrowIfFail(Internal.AudioStreamFormat.audio_stream_format_create_from_waveformat_pcm(out streamFormatHandle, samplesPerSecond, bitsPerSample, channels));
            return new AudioStreamFormat(streamFormatHandle);
        }

        /// <summary>
        /// Creates an audio stream format object with the specified compressed audio container format, to be used as input format.
        /// Support added in 1.4.0.
        /// </summary>
        /// <param name="compressedFormat">Formats are defined in AudioStreamContainerFormat enum</param>
        /// <returns>The audio stream format being created.</returns>
        public static AudioStreamFormat GetCompressedFormat(Microsoft.CognitiveServices.Speech.Audio.AudioStreamContainerFormat compressedFormat)
        {
            IntPtr streamFormatHandle = IntPtr.Zero;
            ThrowIfFail(Internal.AudioStreamFormat.audio_stream_format_create_from_compressed_format(out streamFormatHandle, compressedFormat));
            return new AudioStreamFormat(streamFormatHandle);
        }

        /// <summary>
        /// Dispose of associated resources.
        /// </summary>
        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        private void Dispose(bool disposing)
        {
            if (disposed)
            {
                return;
            }

            if (disposing)
            {
                FormatHandle.Dispose();
            }

            disposed = true;
        }

        private bool disposed = false;

        internal AudioStreamFormat(IntPtr formatPtr)
        {
            ThrowIfNull(formatPtr);
            FormatHandle = new InteropSafeHandle(formatPtr, Internal.AudioStreamFormat.audio_stream_format_release);
        }

        internal InteropSafeHandle FormatHandle { get; }
    }
}
