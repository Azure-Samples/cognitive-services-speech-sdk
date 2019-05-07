//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Globalization;
using System.Runtime.InteropServices;
using Microsoft.CognitiveServices.Speech.Internal;
using static Microsoft.CognitiveServices.Speech.Internal.SpxExceptionThrower;

namespace Microsoft.CognitiveServices.Speech.Audio
{
    /// <summary>
    /// Represents audio input stream used for custom audio input configurations.
    /// </summary>
    public class AudioInputStream : IDisposable
    {
        /// <summary>
        /// Creates a memory backed PushAudioInputStream using the default format (16 kHz, 16 bit, mono PCM).
        /// </summary>
        /// <returns>The push audio input stream being created.</returns>
        public static PushAudioInputStream CreatePushStream()
        {
            return new PushAudioInputStream();
        }

        /// <summary>
        /// Creates a memory backed PushAudioInputStream with the specified audio format.
        /// Currently, only WAV / PCM with 16-bit samples, 16 kHz sample rate, and a single channel (Mono) is supported. When used with Conversation Transcription, eight channels are supported.
        /// </summary>
        /// <param name="format">The audio data format in which audio will be written to the push audio stream's write() method.</param>
        /// <returns>The push audio input stream being created.</returns>
        public static PushAudioInputStream CreatePushStream(AudioStreamFormat format)
        {
            return new PushAudioInputStream(format);
        }

        /// <summary>
        /// Creates a PullAudioInputStream that delegates to the specified callback interface for read() and close() methods, using the default format (16 kHz, 16 bit, mono PCM).
        /// </summary>
        /// <param name="callback">The custom audio input object, derived from PullAudioInputStreamCallback</param>
        /// <returns>The pull audio input stream being created.</returns>
        public static PullAudioInputStream CreatePullStream(PullAudioInputStreamCallback callback)
        {
            return new PullAudioInputStream(callback);
        }

        /// <summary>
        /// Creates a PullAudioInputStream that delegates to the specified callback interface for read() and close() methods.
        /// Currently, only WAV / PCM with 16-bit samples, 16 kHz sample rate, and a single channel (Mono) is supported. When used with Conversation Transcription, eight channels are supported.
        /// </summary>
        /// <param name="callback">The custom audio input object, derived from PullAudioInputStreamCallback.</param>
        /// <param name="format">The audio data format in which audio will be returned from the callback's read() method.</param>
        /// <returns>The pull audio input stream being created.</returns>
        public static PullAudioInputStream CreatePullStream(PullAudioInputStreamCallback callback, AudioStreamFormat format)
        {
            return new PullAudioInputStream(callback, format);
        }

        internal static AudioStreamFormat UseDefaultFormatIfNull(AudioStreamFormat format)
        {
            return format != null ? format : AudioStreamFormat.GetDefaultInputFormat();
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
                StreamHandle.Dispose();
            }

            disposed = true;
        }

        /// <summary>
        /// isDisposing is a flag used to indicate if object is being disposed.
        /// </summary>
        protected bool isDisposing = false;
        private bool disposed = false;
        internal InteropSafeHandle StreamHandle { get; }

        internal AudioInputStream(IntPtr streamPtr)
        {
            ThrowIfNull(streamPtr);
            StreamHandle = new InteropSafeHandle(streamPtr, Internal.AudioInputStream.audio_stream_release);
        }

    }

    /// <summary>
    /// Represents memory backed push audio input stream used for custom audio input configurations.
    /// </summary>
    public sealed class PushAudioInputStream : AudioInputStream
    {
        /// <summary>
        /// Creates a memory backed PushAudioInputStream using the default format (16 kHz, 16 bit, mono PCM).
        /// </summary>
        public PushAudioInputStream() :
            this(CreateStreamHandle(UseDefaultFormatIfNull(null)))
        {
        }

        /// <summary>
        /// Creates a memory backed PushAudioInputStream with the specified audio format.
        /// Currently, only WAV / PCM with 16-bit samples, 16 kHz sample rate, and a single channel (Mono) is supported. When used with Conversation Transcription, eight channels are supported.
        /// </summary>
        /// <param name="format">The audio data format in which audio will be written to the push audio stream's write() method.</param>
        public PushAudioInputStream(AudioStreamFormat format) :
            this(CreateStreamHandle(UseDefaultFormatIfNull(format)))
        {
            GC.KeepAlive(format);
        }

        /// <summary>
        /// Writes the audio data specified by making an internal copy of the data.
        /// Note: The dataBuffer should not contain any audio header.
        /// </summary>
        /// <param name="dataBuffer">The audio buffer of which this function will make a copy.</param>
        public void Write(byte[] dataBuffer)
        {
            ThrowIfNull(StreamHandle, "Invalid stream handle.");
            ThrowIfFail(Internal.PushAudioInputStream.push_audio_input_stream_write(StreamHandle, dataBuffer, (uint)dataBuffer.Length));
        }

        /// <summary>
        /// Writes the audio data specified by making an internal copy of the data.
        /// </summary>
        /// <param name="dataBuffer">The audio buffer of which this function will make a copy.</param>
        /// <param name="size">The size of the data in the audio buffer. Note the size could be smaller than dataBuffer.Length</param>
        public void Write(byte[] dataBuffer, int size)
        {
            ThrowIfNull(StreamHandle, "Invalid stream handle.");
            ThrowIfFail(Internal.PushAudioInputStream.push_audio_input_stream_write(StreamHandle, dataBuffer, (uint)size));
        }

        /// <summary>
        /// Set value of a property associated to data buffer. The properties of the audio data should be set before writing the audio data.
        /// Added in version 1.5.0
        /// </summary>
        /// <param name="id">A property Id.</param>
        /// <param name="value">The value of the property.</param>
        public void SetProperty(PropertyId id, string value)
        {
            ThrowIfNull(StreamHandle);
            ThrowIfFail(Internal.PushAudioInputStream.push_audio_input_stream_set_property_by_id(StreamHandle, (int)id, value));
        }

        /// <summary>
        /// Set value of a property associated to data buffer. The properties of the audio data should be set before writing the audio data.
        /// Added in version 1.5.0
        /// </summary>
        /// <param name="name">The name of the property.</param>
        /// <param name="value">The value of the property.</param>
        public void SetProperty(string name, string value)
        {
            ThrowIfNull(StreamHandle);
            ThrowIfFail(Internal.PushAudioInputStream.push_audio_input_stream_set_property_by_name(StreamHandle, name, value));
        }

        /// <summary>
        /// Closes the stream.
        /// </summary>
        public void Close()
        {
            ThrowIfNull(StreamHandle, "Invalid stream handle.");
            ThrowIfFail(Internal.PushAudioInputStream.push_audio_input_stream_close(StreamHandle));
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
                if (StreamHandle != null)
                {
                    if (Internal.AudioInputStream.audio_stream_is_handle_valid(StreamHandle))
                    {
                        LogErrorIfFail(Internal.PushAudioInputStream.push_audio_input_stream_close(StreamHandle));
                    }
                }
            }

            disposed = true;
            base.Dispose(disposing);
        }

        private bool disposed = false;

        private static IntPtr CreateStreamHandle(AudioStreamFormat streamFormat)
        {
            ThrowIfNull(streamFormat);
            IntPtr audioStreamHandle = IntPtr.Zero;
            ThrowIfFail(Internal.PushAudioInputStream.audio_stream_create_push_audio_input_stream(out audioStreamHandle, streamFormat.FormatHandle));
            GC.KeepAlive(streamFormat);
            return audioStreamHandle;
        }

        internal PushAudioInputStream(IntPtr streamPtr) :
            base(streamPtr)
        {
        }

    }

    /// <summary>
    /// Represents audio input stream used for custom audio input configurations.
    /// </summary>
    public sealed class PullAudioInputStream : AudioInputStream
    {
        /// <summary>
        /// Creates a PullAudioInputStream that delegates to the specified callback interface for read() and close() methods using the default format (16 kHz, 16 bit, mono PCM).
        /// </summary>
        /// <param name="callback">The custom audio input object, derived from PullAudioInputStreamCallback.</param>
        /// <returns>The pull audio input stream being created.</returns>
        public PullAudioInputStream(PullAudioInputStreamCallback callback) :
            this(Create(UseDefaultFormatIfNull(null)), callback)
        {
        }

        /// <summary>
        /// Creates a PullAudioInputStream that delegates to the specified callback interface for read() and close() methods.
        /// Currently, only WAV / PCM with 16-bit samples, 16 kHz sample rate, and a single channel (Mono) is supported. When used with Conversation Transcription, eight channels are supported.
        /// </summary>
        /// <param name="callback">The custom audio input object, derived from PullAudioInputStreamCallback.</param>
        /// <param name="format">The audio data format in which audio will be returned from the callback's read() method.</param>
        /// <returns>The pull audio input stream being created.</returns>
        public PullAudioInputStream(PullAudioInputStreamCallback callback, AudioStreamFormat format) :
            this(Create(UseDefaultFormatIfNull(format)), callback)
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
            streamReadDelegate = null;
            streamCloseDelegate = null;
            streamGetPropertyDelegate = null;
            if (gch.IsAllocated)
            {
                gch.Free();
            }

            disposed = true;
            base.Dispose(disposing);
        }

        private bool disposed = false;
        private PullAudioInputStreamCallback callback = null;
        private PullAudioStreamReadDelegate streamReadDelegate;
        private PullAudioStreamCloseDelegate streamCloseDelegate;
        private PullAudioStreamGetPropertyDelegate streamGetPropertyDelegate;
        private GCHandle gch;

        private static IntPtr Create(AudioStreamFormat streamFormat)
        {
            ThrowIfNull(streamFormat);
            IntPtr audioStreamHandle = IntPtr.Zero;
            ThrowIfFail(Internal.PullAudioInputStream.audio_stream_create_pull_audio_input_stream(out audioStreamHandle, streamFormat.FormatHandle));
            GC.KeepAlive(streamFormat);
            return audioStreamHandle;
        }

        internal PullAudioInputStream(IntPtr streamPtr, PullAudioInputStreamCallback cb) :
            base(streamPtr)
        {
            callback = cb;
            streamReadDelegate = StreamReadCallback;
            streamCloseDelegate = StreamCloseCallback;
            streamGetPropertyDelegate = StreamGetPropertyCallback;
            gch = GCHandle.Alloc(this, GCHandleType.Weak);
            ThrowIfFail(Internal.PullAudioInputStream.pull_audio_input_stream_set_callbacks(StreamHandle, GCHandle.ToIntPtr(gch), streamReadDelegate, streamCloseDelegate));
            ThrowIfFail(Internal.PullAudioInputStream.pull_audio_input_stream_set_getproperty_callback(StreamHandle, GCHandle.ToIntPtr(gch), streamGetPropertyDelegate));
        }

        [MonoPInvokeCallback]
        private static int StreamReadCallback(IntPtr context, IntPtr buffer, uint size)
        {
            int result = 0;
            try
            {
                PullAudioInputStreamCallback callback = null;
                var stream = InteropSafeHandle.GetObjectFromWeakHandle<PullAudioInputStream>(context);
                ThrowIfNull(stream);
                if (stream.isDisposing)
                {
                    return result;
                }
                callback = stream.callback;
                ThrowIfNull(callback);
                byte[] srcBuffer = new byte[size];
                result = callback.Read(srcBuffer, size);
                if (result < 0)
                {
                    throw new InvalidOperationException(string.Format(CultureInfo.InvariantCulture, "Error: the stream Read callback returns negative value. It should return the number of bytes have been read."));
                }
                if (buffer != IntPtr.Zero)
                {
                    Marshal.Copy(srcBuffer, 0, buffer, result);
                }
            }
            catch (InvalidOperationException)
            {
                LogError(SpxError.InvalidHandle);
            }
            catch (ApplicationException ex)
            {
                LogError(nameof(ApplicationException) + ": " + ex.Message);
            }

            return result;
        }

        [MonoPInvokeCallback]
        private static void StreamCloseCallback(IntPtr context)
        {
            try
            {
                PullAudioInputStreamCallback callback = null;
                var stream = InteropSafeHandle.GetObjectFromWeakHandle<PullAudioInputStream>(context);
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
            catch (ApplicationException ex)
            {
                LogError(nameof(ApplicationException) + ": " + ex.Message);
            }
        }

        [MonoPInvokeCallback]
        private static void StreamGetPropertyCallback(IntPtr context, Internal.PropertyId id, IntPtr buffer, uint size)
        {
            IntPtr rePtr = IntPtr.Zero;
            try
            {
                PullAudioInputStreamCallback callback = null;
                var stream = InteropSafeHandle.GetObjectFromWeakHandle<PullAudioInputStream>(context);
                ThrowIfNull(stream);
                if (stream.isDisposing)
                {
                    return;
                }
                callback = stream.callback;
                ThrowIfNull(callback);

                string value = callback.GetProperty((PropertyId)id);
                ThrowIfNull(value);

                byte[] srcBuffer = System.Text.Encoding.UTF8.GetBytes(value);
                ThrowIfFail(srcBuffer.Length <= size);
                if (buffer != IntPtr.Zero)
                {
                    Marshal.Copy(srcBuffer, 0, buffer, (int)srcBuffer.Length);
                }
            }
            catch (InvalidOperationException)
            {
                LogError(SpxError.InvalidHandle);
            }
            catch (ApplicationException e)
            {
                LogError(e.Message);
            }
        }
    }

    /// <summary>
    /// An abstract base class that defines callback methods (Read() and Close()) for custom audio input streams).
    /// </summary>
    public abstract class PullAudioInputStreamCallback : IDisposable
    {
        /// <summary>
        /// Reads binary data from the stream.
        /// Note: The dataBuffer returned by Read() should not contain any audio header.
        /// </summary>
        /// <param name="dataBuffer">The buffer to fill</param>
        /// <param name="size">The size of the buffer.</param>
        /// <returns>The number of bytes filled, or 0 in case the stream hits its end and there is no more data available.
        /// If there is no data immediately available, Read() blocks until the next data becomes available.</returns>
        abstract public int Read(byte[] dataBuffer, uint size);

        /// <summary>
        /// Get property associated to data buffer, such as a timestamp or userId. if the property is not available, an empty string must be returned.
        /// Added in version 1.5.0
        /// </summary>
        /// <param name="id">A property id.</param>
        /// <returns>The value of the property </returns>
        virtual public string GetProperty(PropertyId id) { return ""; }

        /// <summary>
        /// Closes the audio input stream.
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
