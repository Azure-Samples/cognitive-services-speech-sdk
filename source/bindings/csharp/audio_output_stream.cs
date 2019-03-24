//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Collections.Generic;
using System.Globalization;
using System.Text;

namespace Microsoft.CognitiveServices.Speech.Audio
{
    /// <summary>
    /// Represents audio output stream used for custom audio output configurations.
    /// Added in version 1.4.0
    /// </summary>
    public class AudioOutputStream : IDisposable
    {
        /// <summary>
        /// Creates a memory backed PullAudioOutputStream using the default format (16Khz 16bit mono PCM).
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
        /// Creates a PushAudioOutputStream that delegates to the specified callback interface for write() and close() methods, using the default format (16Khz 16bit mono PCM).
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
                streamImpl.Dispose();
            }

            disposed = true;
        }

        private bool disposed = false;

        internal AudioOutputStream(Microsoft.CognitiveServices.Speech.Internal.AudioOutputStream stream)
        {
            streamImpl = stream;
        }

        internal Microsoft.CognitiveServices.Speech.Internal.AudioOutputStream streamImpl { get; }
    }

    /// <summary>
    /// Represents memory backed pull audio output stream used for custom audio output configurations.
    /// Added in version 1.4.0
    /// </summary>
    public sealed class PullAudioOutputStream : AudioOutputStream
    {
        /// <summary>
        /// Creates a memory backed PullAudioOutputStream using the default format (16Khz 16bit mono PCM).
        /// </summary>
        public PullAudioOutputStream() :
            this(Microsoft.CognitiveServices.Speech.Internal.PullAudioOutputStream.CreatePullStream())
        {
        }

        /// <summary>
        /// Creates a memory backed PullAudioOutputStream with the specified audio format.
        /// </summary>
        /// <param name="format">The audio data format in which audio will be read from the pull audio stream's read() method.</param>
        public PullAudioOutputStream(AudioStreamFormat format) :
            this(Microsoft.CognitiveServices.Speech.Internal.PullAudioOutputStream.CreatePullStream(format.formatImpl))
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
            return pullImpl.Read(buffer);
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
                pullImpl.Dispose();
            }

            disposed = true;
            base.Dispose(disposing);
        }

        private bool disposed = false;

        internal PullAudioOutputStream(Microsoft.CognitiveServices.Speech.Internal.PullAudioOutputStream stream) :
            base(stream)
        {
            pullImpl = stream;
        }

        internal Microsoft.CognitiveServices.Speech.Internal.PullAudioOutputStream pullImpl { get; }
    }

    /// <summary>
    /// Represents audio output stream used for custom audio output configurations.
    /// Added in version 1.4.0
    /// </summary>
    public sealed class PushAudioOutputStream : AudioOutputStream
    {
        /// <summary>
        /// Creates a PushAudioOutputStream that delegates to the specified callback interface for write() and close() methods using the default format (16Khz 16bit mono PCM).
        /// </summary>
        /// <param name="callback">The custom audio output object, derived from PushAudioOutputStreamCallback.</param>
        /// <returns>The push audio output stream being created.</returns>
        public PushAudioOutputStream(PushAudioOutputStreamCallback callback) :
            this(Microsoft.CognitiveServices.Speech.Internal.PushAudioOutputStream.CreatePushStream(callback.Adapter), callback)
        {
        }

        /// <summary>
        /// Creates a PushAudioOutputStream that delegates to the specified callback interface for write() and close() methods.
        /// </summary>
        /// <param name="callback">The custom audio output object, derived from PushAudioOutputStreamCallback.</param>
        /// <param name="format">The audio data format in which audio will be sent to the callback's write() method.</param>
        /// <returns>The push audio output stream being created.</returns>
        public PushAudioOutputStream(PushAudioOutputStreamCallback callback, AudioStreamFormat format) :
            this(Microsoft.CognitiveServices.Speech.Internal.PushAudioOutputStream.CreatePushStream(format.formatImpl, callback.Adapter), callback)
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

            if (disposing)
            {
                callbackKeepAlive = null;
            }

            disposed = true;
            base.Dispose(disposing);
        }

        private bool disposed = false;

        internal PushAudioOutputStream(Microsoft.CognitiveServices.Speech.Internal.PushAudioOutputStream stream, PushAudioOutputStreamCallback callback) :
            base(stream)
        {
            callbackKeepAlive = callback;
        }

        private PushAudioOutputStreamCallback callbackKeepAlive = null;
    }

    /// <summary>
    /// An abstract base class that defines callback methods (Write() and Close()) for custom audio output streams).
    /// Added in version 1.4.0
    /// </summary>
    public abstract class PushAudioOutputStreamCallback : IDisposable
    {
        /// <summary>
        /// The adapter to the internal
        /// </summary>
        internal PushAudioOutputStreamCallbackInternalAdapter Adapter { get; private set; }

        /// <summary>
        /// Creates a new push audio output stream callback.
        /// </summary>
        public PushAudioOutputStreamCallback()
        {
            Adapter = new PushAudioOutputStreamCallbackInternalAdapter(this);
        }

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
                this.Adapter.Dispose();
            }

            disposed = true;
        }

        private bool disposed = false;
    }

    /// <summary>
    /// Adapter class to the native audio stream interface.
    /// Added in version 1.4.0
    /// </summary>
    internal class PushAudioOutputStreamCallbackInternalAdapter : Internal.PushAudioOutputStreamCallback, IDisposable
    {
        private PushAudioOutputStreamCallback callback;

        /// <summary>
        /// Creates a new push audio output stream callback adapter.
        /// </summary>
        /// <param name="callback">PushAudioOutputStreamCallback instance.</param>
        public PushAudioOutputStreamCallbackInternalAdapter(PushAudioOutputStreamCallback callback)
        {
            this.callback = callback;
        }

        /// <summary>
        /// Writes binary data into the stream.
        /// </summary>
        /// <param name="dataBuffer">The buffer with the binary data.</param>
        /// <param name="size">The size of the buffer.</param>
        /// <returns>The number of bytes being written.</returns>
        public override uint Write(byte[] dataBuffer, uint size)
        {
            if (size != dataBuffer.Length)
            {
                throw new ArgumentException(nameof(size));
            }

            return callback.Write(dataBuffer);
        }

        /// <summary>
        /// Closes the stream.
        /// </summary>
        public override void Close()
        {
            callback.Close();
        }

        /// <summary>
        /// Dispose of associated resources.
        /// </summary>
        public override void Dispose()
        {
            Dispose(true);
            base.Dispose();
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
                callback = null;
            }

            disposed = true;
        }

        private bool disposed = false;
    };
}
