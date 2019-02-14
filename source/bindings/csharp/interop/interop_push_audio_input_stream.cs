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

    internal class PushAudioInputStream : AudioInputStream
    {
        internal PushAudioInputStream(IntPtr streamPtr) : base(streamPtr)
        {
        }

        protected override void Dispose(bool disposing)
        {
            if (disposed) return;

            if (disposing)
            {
                // Free any other managed objects here.
                if (streamHandle != null)
                {
                    if (audio_stream_is_handle_valid(streamHandle))
                    {
                        LogErrorIfFail(push_audio_input_stream_close(streamHandle));
                    }
                }
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
            ThrowIfNull(streamHandle, "Invalid stream handle.");
            ThrowIfFail(push_audio_input_stream_write(streamHandle, dataBuffer, size));
        }

        public void Close()
        {
            ThrowIfNull(streamHandle, "Invalid stream handle.");
            ThrowIfFail(push_audio_input_stream_close(streamHandle));
        }

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR audio_stream_create_push_audio_input_stream(out SPXAUDIOSTREAMHANDLE audioStream, InteropSafeHandle format);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR push_audio_input_stream_write(InteropSafeHandle audioStream, [MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 2)] byte[] buffer, UInt32 size);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR push_audio_input_stream_close(InteropSafeHandle audioStream);

    }
}
