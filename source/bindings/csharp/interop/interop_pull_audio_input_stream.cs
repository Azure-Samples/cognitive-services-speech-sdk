//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Runtime.InteropServices;
using System.Threading;
using static Microsoft.CognitiveServices.Speech.Internal.SpxExceptionThrower;

namespace Microsoft.CognitiveServices.Speech.Internal
{
    using SPXHR = System.IntPtr;
    using SPXAUDIOSTREAMFORMATHANDLE = System.IntPtr;
    using SPXAUDIOSTREAMHANDLE = System.IntPtr;

    internal class PullAudioInputStreamCallback : IDisposable
    {
        public virtual int Read(byte[] dataBuffer, uint size) { return 0;  }
        public virtual void Close() {}
        public virtual void Dispose() {}
    }

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate int PullAudioStreamReadDelegate(IntPtr context, IntPtr dataBuffer, uint size);
    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate void PullAudioStreamCloseDelegate(IntPtr context);
    internal class PullAudioInputStream : AudioInputStream
    {
        private PullAudioInputStreamCallback callback = null;
        private PullAudioStreamReadDelegate streamReadDelegate;
        private PullAudioStreamCloseDelegate streamCloseDelegate;
        internal PullAudioInputStream(IntPtr streamPtr, PullAudioInputStreamCallback cb) : base(streamPtr)
        {
            callback = cb;
            streamReadDelegate = StreamReadCallback;
            streamCloseDelegate = StreamCloseCallback;
            ThrowIfFail(pull_audio_input_stream_set_callbacks(streamHandle, IntPtr.Zero, streamReadDelegate, streamCloseDelegate));
        }

        protected override void Dispose(bool disposing)
        {
            if (disposed) return;

            if (disposing)
            {
                lock (thisLock)
                {
                    // dispose managed resources
                    if (callback != null)
                    {
                        callback = null;
                    }
                }
            }
            // dispose unmanaged resources

            base.Dispose(disposing);
            streamReadDelegate = null;
            streamCloseDelegate = null;
        }

        public static PullAudioInputStream Create(PullAudioInputStreamCallback callback)
        {
            return Create(null, callback);
        }

        public static PullAudioInputStream Create(AudioStreamFormat format, PullAudioInputStreamCallback callback)
        {
            AudioStreamFormat streamFormat = UseDefaultFormatIfNull(format);
            ThrowIfNull(streamFormat);
            SPXAUDIOSTREAMHANDLE audioStreamHandle = IntPtr.Zero;
            ThrowIfFail(audio_stream_create_pull_audio_input_stream(out audioStreamHandle, streamFormat.FormatHandle));
            GC.KeepAlive(streamFormat);
            return new PullAudioInputStream(audioStreamHandle, callback);
        }

        [MonoPInvokeCallback]
        private int StreamReadCallback(IntPtr context, IntPtr buffer, uint size)
        {
            int result = 0;
            try
            {
                PullAudioInputStreamCallback callback = null;
                if (isDisposing)
                {
                    return result;
                }
                lock (thisLock)
                {
                    callback = this.callback;
                }
                ThrowIfNull(callback);
                byte[] srcBuffer = new byte[size];
                result = callback.Read(srcBuffer, size);
                if (buffer != IntPtr.Zero)
                {
                    Marshal.Copy(srcBuffer, 0, buffer, result);
                }
            }
            catch (InvalidOperationException)
            {
                LogError(SpxError.InvalidHandle);
            }
            return result;
        }

        [MonoPInvokeCallback]
        private void StreamCloseCallback(IntPtr context)
        {
            try
            {
                PullAudioInputStreamCallback callback = null;
                if (isDisposing)
                {
                    return;
                }
                lock (thisLock)
                {
                    callback = this.callback;
                }
                ThrowIfNull(callback);
                callback.Close();
            }
            catch (InvalidOperationException)
            {
                LogError(SpxError.InvalidHandle);
            }
        }

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR audio_stream_create_pull_audio_input_stream(out SPXAUDIOSTREAMHANDLE audioStream, InteropSafeHandle format);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR pull_audio_input_stream_set_callbacks(InteropSafeHandle audioStream, IntPtr context, PullAudioStreamReadDelegate readCallback, PullAudioStreamCloseDelegate closeCallback);

    }
}
