//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Collections.Generic;
using System.Globalization;

namespace Microsoft.CognitiveServices.Speech
{
    /// <summary>
    /// The Audio stream format.
    /// </summary>
    public struct AudioInputStreamFormat
    {
        /// <summary>
        /// The format of the audio, valid values: 1 (PCM)
        /// </summary>
        public int FormatTag;

        /// <summary>
        /// The number of channels, valid values: 1 (Mono).
        /// </summary>
        public int Channels;

        /// <summary>
        /// The sample rate, valid values: 16000.
        /// </summary>
        public int SamplesPerSec;

        /// <summary>
        /// Average bytes per second, usually calculated as nSamplesPerSec * nChannels * ceil(wBitsPerSample, 8).
        /// </summary>
        public int AvgBytesPerSec;

        /// <summary>
        /// The size of a single frame, valid values: nChannels * ceil(wBitsPerSample, 8).
        /// </summary>
        public int BlockAlign;

        /// <summary>
        /// The bits per sample, valid values: 16
        /// </summary>
        public int BitsPerSample;
    };

    /// <summary>
    /// Defines audio input stream.
    /// </summary>
    public abstract class AudioInputStream : IDisposable
    {
        /// <summary>
        /// The adapter to the internal 
        /// </summary>
        internal AudioInputStreamForwarder Forwarder { get; private set; }

        /// <summary>
        /// Creates a new audio input stream.
        /// </summary>
        public AudioInputStream()
        {
            Forwarder = new AudioInputStreamForwarder(this);
        }

        /// <summary>
        /// The format of this audio stream.
        /// </summary>
        /// <returns>Returns the format of this audio stream.</returns>
        abstract public AudioInputStreamFormat GetFormat();

        /// <summary>
        /// Provides the binary data of the stream.
        /// </summary>
        /// <param name="dataBuffer">The buffer to fill</param>
        /// <param name="size">The number of bytes to fill, always starting at offset 0.</param>
        /// <returns>The number of bytes filled, or 0 in case the stream hits its end and there is no more data available.
        /// Please note that it is expected that the call to Read() blocks until data is available in case where there is
        /// no immediate data available at the time of the call.</returns>
        abstract public int Read(byte[] dataBuffer, int size);

        /// <summary>
        /// Closes the audio input stream.
        /// </summary>
        virtual public void Close()
        {
            Dispose();
        }

        /// <summary>
        /// Disposes the resources held by this instance.
        /// </summary>
        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        /// <summary>
        /// Disposes the resources held by this instance.
        /// </summary>
        /// <param name="disposing">True if called by Dispose().</param>
        protected virtual void Dispose(bool disposing)
        {
            if (disposing)
            {
                this.Forwarder.Dispose();
            }
        }
    }

    /// <summary>
    /// Adapter class to the native stream api.
    /// </summary>
    public sealed class BinaryAudioStreamReader : AudioInputStream, IDisposable
    {
        private System.IO.BinaryReader _reader;
        private AudioInputStreamFormat _format;

        /// <summary>
        /// Creates and initializes an instance of the AudioStreamReader.
        /// </summary>
        /// <param name="format">The format of the underlying stream</param>
        /// <param name="reader">The underlying stream to read the audio data from. Note: The stream contains the bare sample data, not the container (like wave header data, etc).</param>
        public BinaryAudioStreamReader(AudioInputStreamFormat format , System.IO.BinaryReader reader)
        {
            _format = format;
            _reader = reader;
        }

        /// <summary>
        /// Creates and initializes an instance of the AudioStreamReader.
        /// </summary>
        /// <param name="format">The format of the underlying stream</param>
        /// <param name="stream">The underlying stream to read the audio data from. Note: The stream contains the bare sample data, not the container (like wave header data, etc).</param>
        public BinaryAudioStreamReader(AudioInputStreamFormat format, System.IO.Stream stream) 
            : this(format, new System.IO.BinaryReader(stream))
        {
        }

        /// <summary>
        /// Returns the format of the stream.
        /// </summary>
        /// <returns>Returns the format of the stream</returns>
        public override AudioInputStreamFormat GetFormat()
        {
            return _format;
        }

        /// <summary>
        /// Provides the binary data of the stream.
        /// </summary>
        /// <param name="dataBuffer">The buffer to fill</param>
        /// <param name="size">The number of bytes to fill, always starting at offset 0.</param>
        /// <returns>The number of bytes filled, or 0 in case the stream hits its end and there is no more data available.
        /// Please note that it is expected that the call to Read() blocks until data is available in case where there is
        /// no immediate data available at the time of the call.</returns>
        public override int Read(byte[] dataBuffer, int size)
        {
            return _reader.Read(dataBuffer, 0, size);
        }

        /// <summary>
        /// Disposes the resorces held by this instance.
        /// </summary>
        /// <param name="disposing">If true, called through Dispose().</param>
        override protected void Dispose(bool disposing)
        {
            if(disposing)
            {
                _reader.Dispose();
            }
            base.Dispose(disposing);
        }
    }

    /// <summary>
    /// Adapter class to the native audio stream interface.
    /// </summary>
    internal class AudioInputStreamForwarder : Internal.AudioInputStream, IDisposable
    {
        private AudioInputStream _target;

        public AudioInputStreamForwarder(AudioInputStream target)
        {
            _target = target;
        }

        override public int Read(byte[] dataBuffer, int size)
        {
            if (size < 0)
            {
                throw new ArgumentOutOfRangeException(string.Format(CultureInfo.InvariantCulture, "Invalid size: {0}. A negative value is not allowed.", size));
            }

            int count = _target.Read(dataBuffer, size);
            return count;
        }

        override public void Close()
        {
            _target.Close();
        }

        override public int GetFormat(Internal.AudioInputStreamFormat pformat, int cbFormat)
        {
            if (pformat == null || cbFormat < 44)
                return 44;

            var format = _target.GetFormat();

            // cbSize is the block size after the field cbSize, so it should be 0.
            pformat.cbSize = 0;
            pformat.nAvgBytesPerSec = format.AvgBytesPerSec;
            pformat.nBlockAlign = format.BlockAlign;
            pformat.nChannels = format.Channels;
            pformat.nSamplesPerSec = format.SamplesPerSec;
            pformat.wBitsPerSample = format.BitsPerSample;
            pformat.wFormatTag = format.FormatTag;

            return 44;
        }
    };
}
