//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Globalization;

namespace Microsoft.CognitiveServices.Speech.Audio
{
    /// <summary>
    /// Represents audio stream format used for custom audio input configurations.
    /// </summary>
    public sealed class AudioStreamFormat : IDisposable
    {
        /// <summary>
        /// Creates an audio stream format object representing the default microphone input format (16Khz 16bit mono PCM).
        /// </summary>
        /// <returns>The audio stream format being created.</returns>
        public static AudioStreamFormat GetDefaultInputFormat()
        {
            return new AudioStreamFormat(Microsoft.CognitiveServices.Speech.Internal.AudioStreamFormat.GetDefaultInputFormat());
        }

        /// <summary>
        /// Creates an audio stream format object with the specified pcm waveformat characteristics.
        /// </summary>
        /// <param name="samplesPerSecond">Sample rate, in samples per second (hertz).</param>
        /// <param name="bitsPerSample">Bits per sample, typically 16.</param>
        /// <param name="channels">Number of channels in the waveform-audio data. Monaural data uses one channel and stereo data uses two channels.</param>
        /// <returns>The audio stream format being created.</returns>
        public static AudioStreamFormat GetWaveFormatPCM(uint samplesPerSecond, byte bitsPerSample, byte channels)
        {
            return new AudioStreamFormat(Microsoft.CognitiveServices.Speech.Internal.AudioStreamFormat.GetWaveFormatPCM(samplesPerSecond, bitsPerSample, channels));
        }

        /// <summary>
        /// Dispose of associated resources.
        /// </summary>
        public void Dispose()
        {
            if (disposed)
            {
                return;
            }

            formatImpl.Dispose();
            disposed = true;
        }

        private bool disposed = false;

        internal AudioStreamFormat(Microsoft.CognitiveServices.Speech.Internal.AudioStreamFormat format)
        {
            formatImpl = format;
        }

        internal Microsoft.CognitiveServices.Speech.Internal.AudioStreamFormat formatImpl { get; }
    }

    /// <summary>
    /// Represents audio input stream used for custom audio input configurations.
    /// </summary>
    public class AudioInputStream : IDisposable
    {
        /// <summary>
        /// Creates a memory backed PushAudioInputStream using the default format (16Khz 16bit mono PCM).
        /// </summary>
        /// <returns>The push audio input stream being created.</returns>
        public static PushAudioInputStream CreatePushStream()
        {
            return new PushAudioInputStream();
        }

        /// <summary>
        /// Creates a memory backed PushAudioInputStream with the specified audio format.
        /// </summary>
        /// <param name="format">The audio data format in which audio will be written to the push audio stream's write() method (currently only support 16Khz 16bit mono PCM).</param>
        /// <returns>The push audio input stream being created.</returns>
        public static PushAudioInputStream CreatePushStream(AudioStreamFormat format)
        {
            return new PushAudioInputStream(format);
        }

        /// <summary>
        /// Creates a PullAudioInputStream that delegates to the specified callback interface for read() and close() methods, using the default format (16Khz 16bit mono PCM).
        /// </summary>
        /// <param name="callback">The custom audio input object, derived from PullAudioInputStreamCallback</param>
        /// <returns>The pull audio input stream being created.</returns>
        public static PullAudioInputStream CreatePullStream(PullAudioInputStreamCallback callback)
        {
            return new PullAudioInputStream(callback);
        }

        /// <summary>
        /// Creates a PullAudioInputStream that delegates to the specified callback interface for read() and close() methods.
        /// </summary>
        /// <param name="callback">The custom audio input object, derived from PullAudioInputStreamCallback.</param>
        /// <param name="format">The audio data format in which audio will be returned from the callback's read() method (currently only support 16Khz 16bit mono PCM).</param>
        /// <returns>The pull audio input stream being created.</returns>
        public static PullAudioInputStream CreatePullStream(PullAudioInputStreamCallback callback, AudioStreamFormat format)
        {
            return new PullAudioInputStream(callback, format);
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

        internal AudioInputStream(Microsoft.CognitiveServices.Speech.Internal.AudioInputStream stream)
        {
            streamImpl = stream;
        }

        internal Microsoft.CognitiveServices.Speech.Internal.AudioInputStream streamImpl { get; }
    }

    /// <summary>
    /// Represents audio input configuration used for specifying what type of input to use (microphone, file, stream).
    /// </summary>
    public sealed class AudioConfig : IDisposable
    {
        /// <summary>
        /// Creates an AudioConfig object representing the default microphone on the system.
        /// </summary>
        /// <returns>The audio input configuration being created.</returns>
        public static AudioConfig FromDefaultMicrophoneInput()
        {
            return new AudioConfig(Microsoft.CognitiveServices.Speech.Internal.AudioConfig.FromDefaultMicrophoneInput());
        }

        /// <summary>
        /// Creates an AudioConfig object representing the specified file.
        /// </summary>
        /// <param name="fileName">Specifies the audio input file. Currently, only WAV / PCM with 16-bit samples, 16 kHz sample rate, and a single channel (Mono) is supported.</param>
        /// <returns>The audio input configuration being created.</returns>
        public static AudioConfig FromWavFileInput(string fileName)
        {
            return new AudioConfig(Microsoft.CognitiveServices.Speech.Internal.AudioConfig.FromWavFileInput(fileName));
        }

        /// <summary>
        /// Creates an AudioConfig object representing the specified stream.
        /// </summary>
        /// <param name="audioStream">Specifies the custom audio input stream. Currently, only WAV / PCM with 16-bit samples, 16 kHz sample rate, and a single channel (Mono) is supported.</param>
        /// <returns>The audio input configuration being created.</returns>
        public static AudioConfig FromStreamInput(AudioInputStream audioStream)
        {
            return new AudioConfig(Microsoft.CognitiveServices.Speech.Internal.AudioConfig.FromStreamInput(audioStream.streamImpl), audioStream);
        }

        /// <summary>
        /// Creates an AudioConfig object representing the specified stream.
        /// </summary>
        /// <param name="callback">Specifies the pull audio input stream callback. Currently, only WAV / PCM with 16-bit samples, 16 kHz sample rate, and a single channel (Mono) is supported.</param>
        /// <returns>The audio input configuration being created.</returns>
        public static AudioConfig FromStreamInput(PullAudioInputStreamCallback callback)
        {
            PullAudioInputStream pullStream = new PullAudioInputStream(callback);
            return new AudioConfig(Microsoft.CognitiveServices.Speech.Internal.AudioConfig.FromStreamInput(pullStream.streamImpl), pullStream, true);
        }

        /// <summary>
        /// Creates an AudioConfig object representing the specified stream.
        /// </summary>
        /// <param name="callback">Specifies the pull audio input stream callback. Currently, only WAV / PCM with 16-bit samples, 16 kHz sample rate, and a single channel (Mono) is supported.</param>
        /// <param name="format">The audio data format in which audio will be written to the push audio stream's write() method (currently only support 16Khz 16bit mono PCM).</param>
        /// <returns>The audio input configuration being created.</returns>
        public static AudioConfig FromStreamInput(PullAudioInputStreamCallback callback, AudioStreamFormat format)
        {
            PullAudioInputStream pullStream = new PullAudioInputStream(callback, format);
            return new AudioConfig(Microsoft.CognitiveServices.Speech.Internal.AudioConfig.FromStreamInput(pullStream.streamImpl), pullStream, true);
        }

        /// <summary>
        /// Dispose of associated resources.
        /// </summary>
        public void Dispose()
        {
            if (disposed)
            {
                return;
            }

            configImpl.Dispose();
            disposed = true;

            if (disposeStream)
            {
                streamKeepAlive.Dispose();
            }
            else
            {
                streamKeepAlive = null;
            }
        }

        private bool disposed = false;
        private AudioInputStream streamKeepAlive = null;
        private bool disposeStream = false;

        internal AudioConfig(Microsoft.CognitiveServices.Speech.Internal.AudioConfig config, AudioInputStream audioStream = null, bool ownStream = false)
        {
            configImpl = config;
            streamKeepAlive = audioStream;
            disposeStream = ownStream;
        }

        internal Microsoft.CognitiveServices.Speech.Internal.AudioConfig configImpl { get; }
    }

    /// <summary>
    /// Represents memory backed push audio input stream used for custom audio input configurations.
    /// </summary>
    public sealed class PushAudioInputStream : AudioInputStream
    {
        /// <summary>
        /// Creates a memory backed PushAudioInputStream using the default format (16Khz 16bit mono PCM).
        /// </summary>
        public PushAudioInputStream() :
            this(Microsoft.CognitiveServices.Speech.Internal.PushAudioInputStream.CreatePushStream())
        {
        }

        /// <summary>
        /// Creates a memory backed PushAudioInputStream with the specified audio format.
        /// </summary>
        /// <param name="format">The audio data format in which audio will be written to the push audio stream's write() method (currently only support 16Khz 16bit mono PCM).</param>
        public PushAudioInputStream(AudioStreamFormat format) :
            this(Microsoft.CognitiveServices.Speech.Internal.PushAudioInputStream.CreatePushStream(format.formatImpl))
        {
        }

        /// <summary>
        /// Writes the audio data specified by making an internal copy of the data.
        /// </summary>
        /// <param name="dataBuffer">The audio buffer of which this function will make a copy.</param>
        public void Write(byte[] dataBuffer)
        {
            pushImpl.Write(dataBuffer, (uint)dataBuffer.Length);
        }

        /// <summary>
        /// Writes the audio data specified by making an internal copy of the data.
        /// </summary>
        /// <param name="dataBuffer">The audio buffer of which this function will make a copy.</param>
        /// <param name="size">The size of the data in the audio buffer. Note the size could be smaller than dataBuffer.Length</param>
        public void Write(byte[] dataBuffer, int size)
        {
            pushImpl.Write(dataBuffer, (uint)size);
        }

        /// <summary>
        /// Closes the stream.
        /// </summary>
        public void Close()
        {
            pushImpl.Close();
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
                pushImpl.Dispose();
            }

            disposed = true;
            base.Dispose(disposing);
        }

        private bool disposed = false;

        internal PushAudioInputStream(Microsoft.CognitiveServices.Speech.Internal.PushAudioInputStream stream) :
            base(stream)
        {
            pushImpl = stream;
        }

        internal Microsoft.CognitiveServices.Speech.Internal.PushAudioInputStream pushImpl { get; }
    }

    /// <summary>
    /// Represents audio input stream used for custom audio input configurations.
    /// </summary>
    public sealed class PullAudioInputStream : AudioInputStream
    {
        /// <summary>
        /// Creates a PullAudioInputStream that delegates to the specified callback interface for read() and close() methods using the default format (16Khz 16bit mono PCM).
        /// </summary>
        /// <param name="callback">The custom audio input object, derived from PullAudioInputStreamCallback.</param>
        /// <returns>The pull audio input stream being created.</returns>
        public PullAudioInputStream(PullAudioInputStreamCallback callback) :
            this(Microsoft.CognitiveServices.Speech.Internal.PullAudioInputStream.CreatePullStream(callback.Adapter), callback)
        {
        }

        /// <summary>
        /// Creates a PullAudioInputStream that delegates to the specified callback interface for read() and close() methods.
        /// </summary>
        /// <param name="callback">The custom audio input object, derived from PullAudioInputStreamCallback.</param>
        /// <param name="format">The audio data format in which audio will be returned from the callback's read() method (currently only support 16Khz 16bit mono PCM).</param>
        /// <returns>The pull audio input stream being created.</returns>
        public PullAudioInputStream(PullAudioInputStreamCallback callback, AudioStreamFormat format) :
            this(Microsoft.CognitiveServices.Speech.Internal.PullAudioInputStream.CreatePullStream(format.formatImpl, callback.Adapter), callback)
        {
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

        internal PullAudioInputStream(Microsoft.CognitiveServices.Speech.Internal.PullAudioInputStream stream, PullAudioInputStreamCallback callback) :
            base(stream)
        {
            callbackKeepAlive = callback;
        }

        private PullAudioInputStreamCallback callbackKeepAlive = null;
    }

    /// <summary>
    /// An abstract base class that defines callback methods (Read() and Close()) for custom audio input streams).
    /// </summary>
    public abstract class PullAudioInputStreamCallback : IDisposable
    {
        /// <summary>
        /// The adapter to the internal
        /// </summary>
        internal PullAudioInputStreamCallbackInternalAdapter Adapter { get; private set; }

        /// <summary>
        /// Creates a new push audio input stream callback.
        /// </summary>
        public PullAudioInputStreamCallback()
        {
            Adapter = new PullAudioInputStreamCallbackInternalAdapter(this);
        }

        /// <summary>
        /// Reads binary data from the stream.
        /// </summary>
        /// <param name="dataBuffer">The buffer to fill</param>
        /// <param name="size">The size of the buffer.</param>
        /// <returns>The number of bytes filled, or 0 in case the stream hits its end and there is no more data available.
        /// If there is no data immediate available, Read() blocks until the next data becomes available.</returns>
        abstract public int Read(byte[] dataBuffer, uint size);

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
                this.Adapter.Dispose();
            }

            disposed = true;
        }

        private bool disposed = false;
    }

    /// <summary>
    /// Adapter class to the native audio stream interface.
    /// </summary>
    internal class PullAudioInputStreamCallbackInternalAdapter : Internal.PullAudioInputStreamCallback, IDisposable
    {
        private PullAudioInputStreamCallback callback;

        public PullAudioInputStreamCallbackInternalAdapter(PullAudioInputStreamCallback callback)
        {
            this.callback = callback;
        }

        override public int Read(byte[] dataBuffer, uint size)
        {
            if (size != dataBuffer.Length)
            {
                throw new ArgumentException(nameof(size));
            }

            int count = callback.Read(dataBuffer, size);
            if (count < 0)
            {
                throw new InvalidOperationException(string.Format(CultureInfo.InvariantCulture, "Invalid count: '{0}'. A negative value is not allowed.", count));
            }

            return count;
        }

        override public void Close()
        {
            callback.Close();
        }

        /// <summary>
        /// Dispose of associated resources.
        /// </summary>
        override public void Dispose()
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
