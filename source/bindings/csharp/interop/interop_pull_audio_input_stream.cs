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
        private GCHandle gch;
        private PullAudioStreamReadDelegate streamReadDelegate;
        private PullAudioStreamCloseDelegate streamCloseDelegate;
        internal PullAudioInputStream(IntPtr streamPtr, PullAudioInputStreamCallback cb) : base(streamPtr)
        {
            callback = cb;
            gch = GCHandle.Alloc(this);
            streamReadDelegate = StreamReadCallback;
            streamCloseDelegate = StreamCloseCallback;
            ThrowIfFail(pull_audio_input_stream_set_callbacks(streamPtr, GCHandle.ToIntPtr(gch), streamReadDelegate, streamCloseDelegate));
        }

        ~PullAudioInputStream()
        {
            Dispose(false);
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
            if (gch.IsAllocated)
            {
                gch.Free();
            }
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
        private static int StreamReadCallback(IntPtr context, IntPtr buffer, uint size)
        {
            int result = 0;
            try
            {
                PullAudioInputStreamCallback callback = null;
                PullAudioInputStream stream = (PullAudioInputStream)GCHandle.FromIntPtr(context).Target;
                lock (stream.thisLock)
                {
                    if (stream.isDisposing) return result;
                    callback = stream.callback;
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
        private static void StreamCloseCallback(IntPtr context)
        {
            try
            {
                PullAudioInputStreamCallback callback = null;
                PullAudioInputStream stream = (PullAudioInputStream)GCHandle.FromIntPtr(context).Target;
                lock (stream.thisLock)
                {
                    if (stream.isDisposing) return;
                    callback = stream.callback;
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
        public static extern SPXHR audio_stream_create_pull_audio_input_stream(out SPXAUDIOSTREAMHANDLE haudioStream, SPXAUDIOSTREAMFORMATHANDLE hformat);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR pull_audio_input_stream_set_callbacks(SPXAUDIOSTREAMHANDLE haudioStream, IntPtr pvContext, PullAudioStreamReadDelegate readCallback, PullAudioStreamCloseDelegate closeCallback);

    }
}
