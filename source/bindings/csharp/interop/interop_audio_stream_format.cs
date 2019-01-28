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

    internal class AudioStreamFormat : SpxExceptionThrower, IDisposable
    {
        private SPXAUDIOSTREAMFORMATHANDLE formatHandle = IntPtr.Zero;
        private bool disposed = false;

        internal AudioStreamFormat(IntPtr formatPtr)
        {
            formatHandle = formatPtr;
        }

        ~AudioStreamFormat()
        {
            Dispose(false);
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        protected virtual void Dispose(bool disposing)
        {
            if (disposed) return;

            if (disposing)
            {
                // dispose managed resources
            }
            // dispose unmanaged resources
            if (formatHandle != IntPtr.Zero)
            {
                LogErrorIfFail(audio_stream_format_release(formatHandle));
                formatHandle = IntPtr.Zero;
            }
            disposed = true;
        }

        public static AudioStreamFormat DefaultInputFormat
        {
            get
            {
                SPXAUDIOSTREAMFORMATHANDLE streamFormatHandle = IntPtr.Zero;
                ThrowIfFail(audio_stream_format_create_from_default_input(out streamFormatHandle));
                return new AudioStreamFormat(streamFormatHandle);
            }
        }

        public static AudioStreamFormat GetWaveFormatPCM(uint samplesPerSecond, byte bitsPerSample, byte channels)
        {
            SPXAUDIOSTREAMFORMATHANDLE streamFormatHandle = IntPtr.Zero;
            ThrowIfFail(audio_stream_format_create_from_waveformat_pcm(out streamFormatHandle, samplesPerSecond, bitsPerSample, channels));
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

        public SPXAUDIOSTREAMFORMATHANDLE FormatHandle
        {
            get
            {
                return formatHandle;
            }
        }

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern bool audio_stream_format_is_handle_valid(SPXAUDIOSTREAMFORMATHANDLE hformat);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR audio_stream_format_create_from_default_input(out SPXAUDIOSTREAMFORMATHANDLE hformat);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR audio_stream_format_create_from_waveformat_pcm(out SPXAUDIOSTREAMFORMATHANDLE hformat, UInt32 samplesPerSecond, Byte bitsPerSample, Byte channels);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR audio_stream_format_release(SPXAUDIOSTREAMFORMATHANDLE hformat);

    }
}
