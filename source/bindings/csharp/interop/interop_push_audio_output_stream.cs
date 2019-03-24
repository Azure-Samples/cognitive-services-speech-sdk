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

    internal class PushAudioOutputStreamCallback : IDisposable
    {
        public virtual uint Write(byte[] dataBuffer, uint size) { return 0; }

        public virtual void Close() { }

        public virtual void Dispose() { }
    }

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate uint PushAudioStreamWriteDelegate(IntPtr context, IntPtr dataBuffer, uint size);

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate void PushAudioStreamCloseDelegate(IntPtr context);

    internal class PushAudioOutputStream : AudioOutputStream
    {
        private PushAudioOutputStreamCallback callback = null;
        private GCHandle gch;
        private PushAudioStreamWriteDelegate streamWriteDelegate;
        private PushAudioStreamCloseDelegate streamCloseDelegate;
        internal PushAudioOutputStream(IntPtr streamPtr, PushAudioOutputStreamCallback cb) : base(streamPtr)
        {
            callback = cb;
            gch = GCHandle.Alloc(this, GCHandleType.Weak);
            streamWriteDelegate = StreamWriteCallback;
            streamCloseDelegate = StreamCloseCallback;
            ThrowIfFail(push_audio_output_stream_set_callbacks(streamHandle, GCHandle.ToIntPtr(gch), StreamWriteCallback, streamCloseDelegate));
        }

        protected override void Dispose(bool disposing)
        {
            if (disposed) return;

            if (disposing)
            {
                // dispose managed resources
                callback = null;
            }

            // dispose unmanaged resources
            streamWriteDelegate = null;
            streamCloseDelegate = null;
            if (gch.IsAllocated)
            {
                gch.Free();
            }

            base.Dispose(disposing);
        }

        public static PushAudioOutputStream Create(PushAudioOutputStreamCallback callback)
        {
            return Create(null, callback);
        }

        public static PushAudioOutputStream Create(AudioStreamFormat format, PushAudioOutputStreamCallback callback)
        {
            AudioStreamFormat streamFormat = UseDefaultFormatIfNull(format);
            ThrowIfNull(streamFormat);
            SPXAUDIOSTREAMHANDLE audioStreamHandle = IntPtr.Zero;
            ThrowIfFail(audio_stream_create_push_audio_output_stream(out audioStreamHandle, streamFormat.FormatHandle));
            GC.KeepAlive(streamFormat);
            return new PushAudioOutputStream(audioStreamHandle, callback);
        }

        [MonoPInvokeCallback]
        private static uint StreamWriteCallback(IntPtr context, IntPtr buffer, uint size)
        {
            uint result = 0;
            try
            {
                PushAudioOutputStreamCallback callback = null;
                var stream = InteropSafeHandle.GetObjectFromWeakHandle<PushAudioOutputStream>(context);
                ThrowIfNull(stream);
                if (stream.isDisposing)
                {
                    return result;
                }

                callback = stream.callback;

                ThrowIfNull(callback);
                byte[] dstBuffer = new byte[size % int.MaxValue];
                if (buffer != IntPtr.Zero)
                {
                    Marshal.Copy(buffer, dstBuffer, 0, (int)(size % int.MaxValue));
                }

                result = callback.Write(dstBuffer, size % int.MaxValue);
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
                PushAudioOutputStreamCallback callback = null;
                var stream = InteropSafeHandle.GetObjectFromWeakHandle<PushAudioOutputStream>(context);
                ThrowIfNull(stream);
                if (stream.isDisposing)
                {
                    return;
                }

                callback = stream.callback;
                ThrowIfNull(callback);
                callback.Close();
            }
            catch (InvalidOperationException)
            {
                LogError(SpxError.InvalidHandle);
            }
        }

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR audio_stream_create_push_audio_output_stream(out SPXAUDIOSTREAMHANDLE audioStream, InteropSafeHandle format);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR push_audio_output_stream_set_callbacks(InteropSafeHandle audioStream, IntPtr context, PushAudioStreamWriteDelegate writeCallback, PushAudioStreamCloseDelegate closeCallback);

    }
}
