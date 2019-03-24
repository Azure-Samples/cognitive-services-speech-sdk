//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Runtime.InteropServices;
using static Microsoft.CognitiveServices.Speech.Internal.SpxExceptionThrower;

namespace Microsoft.CognitiveServices.Speech.Internal
{
    using SPXHR = System.IntPtr;
    using SPXAUDIOSTREAMHANDLE = System.IntPtr;

    internal class PullAudioOutputStream : AudioOutputStream
    {
        internal PullAudioOutputStream(IntPtr streamPtr) : base(streamPtr)
        {
        }

        protected override void Dispose(bool disposing)
        {
            if (disposed) return;

            if (disposing)
            {
                // Free any other managed objects here.
            }

            // Free any unmanaged objects here.
            disposed = true;
            base.Dispose(disposing);
        }

        public static PullAudioOutputStream Create()
        {
            return Create(null);
        }

        public static PullAudioOutputStream Create(AudioStreamFormat format)
        {
            AudioStreamFormat streamFormat = UseDefaultFormatIfNull(format);
            SPXAUDIOSTREAMHANDLE audioStreamHandle = IntPtr.Zero;
            ThrowIfFail(audio_stream_create_pull_audio_output_stream(out audioStreamHandle, streamFormat.FormatHandle));
            GC.KeepAlive(streamFormat);
            return new PullAudioOutputStream(audioStreamHandle);
        }

        public uint Read(byte[] buffer)
        {
            ThrowIfNull(streamHandle, "Invalid stream handle.");

            IntPtr nativeBuffer = Marshal.AllocHGlobal(buffer.Length);

            try
            {
                uint filledSize;
                ThrowIfFail(pull_audio_output_stream_read(streamHandle, nativeBuffer, (uint)(buffer.Length), out filledSize));
                Marshal.Copy(nativeBuffer, buffer, 0, (int)filledSize);
                return filledSize;
            }
            finally
            {
                Marshal.FreeHGlobal(nativeBuffer);
            }
        }

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR audio_stream_create_pull_audio_output_stream(out SPXAUDIOSTREAMHANDLE audioStream, InteropSafeHandle format);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR pull_audio_output_stream_read(InteropSafeHandle audioStream, IntPtr buffer, UInt32 bufferSize, out UInt32 filledSize);
    }
}
