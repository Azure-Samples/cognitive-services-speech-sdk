//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Runtime.InteropServices;
using Microsoft.CognitiveServices.Speech.Internal;
using static Microsoft.CognitiveServices.Speech.Internal.SpxExceptionThrower;

namespace Microsoft.CognitiveServices.Speech.Audio
{
    /// <summary>
    /// Represents audio output stream used for custom audio output configurations.
    /// Added in version 1.4.0
    /// </summary>
    public class AudioOutputStream : IDisposable
    {
        /// <summary>
        /// Creates a memory backed PullAudioOutputStream using the default format (16 kHz, 16 bit, mono PCM).
        /// </summary>
        /// <returns>The pull audio output stream being created.</returns>
        public static PullAudioOutputStream CreatePullStream()
        {
            return new PullAudioOutputStream();
        }

        /// <summary>
        /// Creates a memory backed PullAudioOutputStream with the specified audio format.
        /// </summary>
        /// <param name="format">The audio data format in which audio will be read from the pull audio stream's read() method.</param>
        /// <returns>The pull audio output stream being created.</returns>
        public static PullAudioOutputStream CreatePullStream(AudioStreamFormat format)
        {
            return new PullAudioOutputStream(format);
        }

        /// <summary>
        /// Creates a PushAudioOutputStream that delegates to the specified callback interface for write() and close() methods, using the default format (16 kHz, 16 bit, mono PCM).
        /// </summary>
        /// <param name="callback">The custom audio output object, derived from PushAudioOutputStreamCallback</param>
        /// <returns>The push audio output stream being created.</returns>
        public static PushAudioOutputStream CreatePushStream(PushAudioOutputStreamCallback callback)
        {
            return new PushAudioOutputStream(callback);
        }

        /// <summary>
        /// Creates a PushAudioOutputStream that delegates to the specified callback interface for write() and close() methods.
        /// </summary>
        /// <param name="callback">The custom audio output object, derived from PushAudioOutputStreamCallback.</param>
        /// <param name="format">The audio data format in which audio will be sent to the callback's write() method.</param>
        /// <returns>The push audio output stream being created.</returns>
        public static PushAudioOutputStream CreatePushStream(PushAudioOutputStreamCallback callback, AudioStreamFormat format)
        {
            return new PushAudioOutputStream(callback, format);
        }

        /// <summary>
        /// Dispose of associated resources.
        /// </summary>
        public void Dispose()
        {
            isDisposing = true;
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        /// <summary>
        /// This method performs cleanup of resources.
        /// The Boolean parameter <paramref name="disposing"/> indicates whether the method is called from <see cref="IDisposable.Dispose"/> (if <paramref name="disposing"/> is true) or from the finalizer (if <paramref name="disposing"/> is false).
        /// Derived classes should override this method to dispose resource if needed.
        /// </summary>
        /// <param name="disposing">Flag to request disposal.</param>
        protected virtual void Dispose(bool disposing)
        {
            if (disposed)
            {
                return;
            }

            if (disposing)
            {
                streamHandle.Dispose();
            }

            disposed = true;
        }

        /// <summary>
        /// isDisposing is a flag used to indicate if object is being disposed.
        /// </summary>
        protected bool isDisposing = false;
        private bool disposed = false;
        internal readonly InteropSafeHandle streamHandle;

        internal AudioOutputStream(IntPtr streamPtr)
        {
            ThrowIfNull(streamPtr);
            streamHandle = new InteropSafeHandle(streamPtr, Internal.AudioOutputStream.audio_stream_release);
        }
    }

    /// <summary>
    /// Represents memory backed pull audio output stream used for custom audio output configurations.
    /// Added in version 1.4.0
    /// </summary>
    public sealed class PullAudioOutputStream : AudioOutputStream
    {

        /// <summary>
        /// Creates a memory backed PullAudioOutputStream using the default format (16 kHz, 16 bit, mono PCM).
        /// </summary>
        public PullAudioOutputStream() :
            this(CreateStreamHandle(AudioInputStream.UseDefaultFormatIfNull(null)))
        {
        }

        /// <summary>
        /// Creates a memory backed PullAudioOutputStream with the specified audio format.
        /// </summary>
        /// <param name="format">The audio data format in which audio will be read from the pull audio stream's read() method.</param>
        public PullAudioOutputStream(AudioStreamFormat format) :
            this(CreateStreamHandle(AudioInputStream.UseDefaultFormatIfNull(format)))
        {
            GC.KeepAlive(format);
        }

        /// <summary>
        /// Read audio from the stream.
        /// The maximal number of bytes to be read is determined by the size of dataBuffer.
        /// If there is no data immediately available, read() blocks until the next data becomes available.
        /// </summary>
        /// <param name="buffer">The buffer to receive the audio data</param>
        /// <returns>The number of bytes filled, or 0 in case the stream hits its end and there is no more data available.</returns>
        public uint Read(byte[] buffer)
        {
            ThrowIfNull(streamHandle, "Invalid stream handle.");

            IntPtr nativeBuffer = Marshal.AllocHGlobal(buffer.Length);
            try
            {
                uint filledSize;
                ThrowIfFail(Internal.PullAudioOutputStream.pull_audio_output_stream_read(streamHandle, nativeBuffer, (uint)(buffer.Length), out filledSize));
                Marshal.Copy(nativeBuffer, buffer, 0, (int)filledSize);
                return filledSize;
            }
            finally
            {
                Marshal.FreeHGlobal(nativeBuffer);
            }
        }

        /// <summary>
        /// This method performs cleanup of resources.
        /// The Boolean parameter <paramref name="disposing"/> indicates whether the method is called from <see cref="IDisposable.Dispose"/> (if <paramref name="disposing"/> is true) or from the finalizer (if <paramref name="disposing"/> is false).
        /// Derived classes should override this method to dispose resource if needed.
        /// </summary>
        /// <param name="disposing">Flag to request disposal.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposed)
            {
                return;
            }

            if (disposing)
            {

            }

            disposed = true;
            base.Dispose(disposing);
        }

        private bool disposed = false;

        private static IntPtr CreateStreamHandle(AudioStreamFormat streamFormat)
        {
            ThrowIfNull(streamFormat);
            IntPtr audioStreamHandle = IntPtr.Zero;
            ThrowIfFail(Internal.PullAudioOutputStream.audio_stream_create_pull_audio_output_stream(out audioStreamHandle, streamFormat.FormatHandle));
            GC.KeepAlive(streamFormat);
            return audioStreamHandle;
        }

        internal PullAudioOutputStream(IntPtr streamPtr) :
            base(streamPtr)
        {
        }

    }

    /// <summary>
    /// Represents audio output stream used for custom audio output configurations.
    /// Added in version 1.4.0
    /// </summary>
    public sealed class PushAudioOutputStream : AudioOutputStream
    {
        /// <summary>
        /// Creates a PushAudioOutputStream that delegates to the specified callback interface for write() and close() methods using the default format (16 kHz, 16 bit, mono PCM).
        /// </summary>
        /// <param name="callback">The custom audio output object, derived from PushAudioOutputStreamCallback.</param>
        /// <returns>The push audio output stream being created.</returns>
        public PushAudioOutputStream(PushAudioOutputStreamCallback callback) :
            this(Create(AudioInputStream.UseDefaultFormatIfNull(null)), callback)
        {
        }

        /// <summary>
        /// Creates a PushAudioOutputStream that delegates to the specified callback interface for write() and close() methods.
        /// </summary>
        /// <param name="callback">The custom audio output object, derived from PushAudioOutputStreamCallback.</param>
        /// <param name="format">The audio data format in which audio will be sent to the callback's write() method.</param>
        /// <returns>The push audio output stream being created.</returns>
        public PushAudioOutputStream(PushAudioOutputStreamCallback callback, AudioStreamFormat format) :
            this(Create(AudioInputStream.UseDefaultFormatIfNull(format)), callback)
        {
            GC.KeepAlive(format);
        }

        /// <summary>
        /// This method performs cleanup of resources.
        /// The Boolean parameter <paramref name="disposing"/> indicates whether the method is called from <see cref="IDisposable.Dispose"/> (if <paramref name="disposing"/> is true) or from the finalizer (if <paramref name="disposing"/> is false).
        /// Derived classes should override this method to dispose resource if needed.
        /// </summary>
        /// <param name="disposing">Flag to request disposal.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposed)
            {
                return;
            }

            callback = null;
            streamWriteDelegate = null;
            streamCloseDelegate = null;
            if (gch.IsAllocated)
            {
                gch.Free();
            }

            disposed = true;
            base.Dispose(disposing);
        }

        private bool disposed = false;

        private static IntPtr Create(AudioStreamFormat streamFormat)
        {
            ThrowIfNull(streamFormat);
            IntPtr audioStreamHandle = IntPtr.Zero;
            ThrowIfFail(Internal.PushAudioOutputStream.audio_stream_create_push_audio_output_stream(out audioStreamHandle, streamFormat.FormatHandle));
            GC.KeepAlive(streamFormat);
            return audioStreamHandle;
        }

        internal PushAudioOutputStream(IntPtr streamPtr, PushAudioOutputStreamCallback cb) :
            base(streamPtr)
        {
            callback = cb;
            gch = GCHandle.Alloc(this, GCHandleType.Weak);
            streamWriteDelegate = StreamWriteCallback;
            streamCloseDelegate = StreamCloseCallback;
            ThrowIfFail(Internal.PushAudioOutputStream.push_audio_output_stream_set_callbacks(streamHandle, GCHandle.ToIntPtr(gch), streamWriteDelegate, streamCloseDelegate));
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
                uint dstBufferSize = size % int.MaxValue;
                byte[] dstBuffer = new byte[dstBufferSize];
                if (buffer != IntPtr.Zero)
                {
                    Marshal.Copy(buffer, dstBuffer, 0, (int)(dstBufferSize));
                }

                result = callback.Write(dstBuffer);
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

        private PushAudioOutputStreamCallback callback = null;
        private GCHandle gch;
        private PushAudioStreamWriteDelegate streamWriteDelegate;
        private PushAudioStreamCloseDelegate streamCloseDelegate;
    }

    /// <summary>
    /// An abstract base class that defines callback methods (Write() and Close()) for custom audio output streams).
    /// Added in version 1.4.0
    /// </summary>
    public abstract class PushAudioOutputStreamCallback : IDisposable
    {

        /// <summary>
        /// Writes binary data to the stream.
        /// </summary>
        /// <param name="dataBuffer">The buffer containing the data to be writen</param>
        /// <returns>The number of bytes writen.</returns>
        abstract public uint Write(byte[] dataBuffer);

        /// <summary>
        /// Closes the audio output stream.
        /// </summary>
        virtual public void Close()
        {
            Dispose();
        }

        /// <summary>
        /// Dispose of associated resources.
        /// </summary>
        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        /// <summary>
        /// This method performs cleanup of resources.
        /// The Boolean parameter <paramref name="disposing"/> indicates whether the method is called from <see cref="IDisposable.Dispose"/> (if <paramref name="disposing"/> is true) or from the finalizer (if <paramref name="disposing"/> is false).
        /// Derived classes should override this method to dispose resource if needed.
        /// </summary>
        /// <param name="disposing">Flag to request disposal.</param>
        protected virtual void Dispose(bool disposing)
        {
            if (disposed)
            {
                return;
            }

            if (disposing)
            {
            }

            disposed = true;
        }

        private bool disposed = false;
    }

}
