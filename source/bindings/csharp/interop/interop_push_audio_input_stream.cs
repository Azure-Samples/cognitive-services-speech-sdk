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
    using SPXAUDIOSTREAMHANDLE = System.IntPtr;

    internal class PushAudioInputStream : AudioInputStream
    {
        internal PushAudioInputStream(IntPtr streamPtr) : base(streamPtr)
        {
        }

        ~PushAudioInputStream()
        {
            Dispose(false);
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

        public static PushAudioInputStream Create()
        {
            return Create(null);
        }

        public static PushAudioInputStream Create(AudioStreamFormat format)
        {
            AudioStreamFormat streamFormat = UseDefaultFormatIfNull(format);
            SPXAUDIOSTREAMHANDLE audioStreamHandle = IntPtr.Zero;
            ThrowIfFail(audio_stream_create_push_audio_input_stream(out audioStreamHandle, streamFormat.FormatHandle));
            GC.KeepAlive(streamFormat);
            return new PushAudioInputStream(audioStreamHandle);
        }

        public void Write(byte[] dataBuffer, uint size)
        {
            ThrowIfFail(push_audio_input_stream_write(streamHandle, dataBuffer, size));
        }

        public void Close()
        {
            ThrowIfFail(push_audio_input_stream_close(streamHandle));
        }

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR audio_stream_create_push_audio_input_stream(out SPXAUDIOSTREAMHANDLE haudioStream, SPXAUDIOSTREAMFORMATHANDLE hformat);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR push_audio_input_stream_write(SPXAUDIOSTREAMHANDLE haudioStream, [MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 2)] byte[] buffer, UInt32 size);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR push_audio_input_stream_close(SPXAUDIOSTREAMHANDLE haudioStream);

    }
}
