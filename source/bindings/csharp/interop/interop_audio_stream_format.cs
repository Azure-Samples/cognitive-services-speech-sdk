//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Text;
using System.Runtime.InteropServices;

namespace Microsoft.CognitiveServices.Speech.Internal
{
    using SPXHR = System.IntPtr;
    using SPXAUDIOSTREAMFORMATHANDLE = System.IntPtr;

    internal class AudioStreamFormat
    {
        internal AudioStreamFormat(IntPtr formatPtr)
        {
            FormatHandle = new InteropSafeHandle(formatPtr, audio_stream_format_release);
        }

        public static AudioStreamFormat DefaultInputFormat
        {
            get
            {
                SPXAUDIOSTREAMFORMATHANDLE streamFormatHandle = IntPtr.Zero;
                SpxExceptionThrower.ThrowIfFail(audio_stream_format_create_from_default_input(out streamFormatHandle));
                return new AudioStreamFormat(streamFormatHandle);
            }
        }

        public static AudioStreamFormat DefaultOutputFormat
        {
            get
            {
                SPXAUDIOSTREAMFORMATHANDLE streamFormatHandle = IntPtr.Zero;
                SpxExceptionThrower.ThrowIfFail(audio_stream_format_create_from_default_output(out streamFormatHandle));
                return new AudioStreamFormat(streamFormatHandle);
            }
        }

        public static AudioStreamFormat GetWaveFormatPCM(uint samplesPerSecond, byte bitsPerSample, byte channels)
        {
            SPXAUDIOSTREAMFORMATHANDLE streamFormatHandle = IntPtr.Zero;
            SpxExceptionThrower.ThrowIfFail(audio_stream_format_create_from_waveformat_pcm(out streamFormatHandle, samplesPerSecond, bitsPerSample, channels));
            return new AudioStreamFormat(streamFormatHandle);
        }

        public static AudioStreamFormat GetWaveFormatPCM(uint samplesPerSecond, byte bitsPerSample)
        {
            return GetWaveFormatPCM(samplesPerSecond, bitsPerSample, 1);
        }

        public static AudioStreamFormat GetWaveFormatPCM(uint samplesPerSecond)
        {
            return GetWaveFormatPCM(samplesPerSecond, 16, 1);
        }

        public InteropSafeHandle FormatHandle { get; }

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern bool audio_stream_format_is_handle_valid(SPXAUDIOSTREAMFORMATHANDLE format);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR audio_stream_format_create_from_default_input(out SPXAUDIOSTREAMFORMATHANDLE format);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR audio_stream_format_create_from_default_output(out SPXAUDIOSTREAMFORMATHANDLE format);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR audio_stream_format_create_from_waveformat_pcm(out SPXAUDIOSTREAMFORMATHANDLE format, UInt32 samplesPerSecond, Byte bitsPerSample, Byte channels);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR audio_stream_format_release(SPXAUDIOSTREAMFORMATHANDLE format);

    }
}
