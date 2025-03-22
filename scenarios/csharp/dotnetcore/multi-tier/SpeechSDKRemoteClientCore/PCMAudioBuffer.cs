//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using SpeechSDKSamples.Remote.Core;
using System;
using System.Collections.Generic;
using System.Linq;

namespace SpeechSDKSamples.Remote.Client.Core
{
    /// <summary>
    /// A buffer for constant bit rate PCM audio that maintains audio data for replay until it is acknowledged.
    /// </summary>
    /// <remarks>
    /// The buffer operates in DataChunk units. Each DataChunk represents a chunk of audio data and the time it was received.
    /// A chunk of audio may be shrunk by an arbitrary number of bytes, but it cannot be expanded.
    /// When chunks are returned, they are returned in the order they were added.
    /// </remarks>
    public sealed class PCMAudioBuffer : AudioBuffer
    {
        private static readonly int MillisecondsInSecond = 1000;
        private static readonly int TicksInMillisecond = 10000;

        private readonly AudioFormat _header;
        private readonly object _lock = new object();
        private readonly List<DataChunk> _audioBuffers = new List<DataChunk>();
        private readonly int _bytesPerSample;
        private readonly int _samplesPerSecond;

        private int _totalSizeInBytes;
        private int _currentChunk;
        private int _bufferStartOffsetInBytesAbsolute;

        /// <summary>
        /// Initializes a new instance of the PCMAudioBuffer class.
        /// </summary>
        /// <param name="header">Header describing the format of the PCM audio.</param>
        /// <exception cref="ArgumentNullException">Thrown when header is null.</exception>
        /// <exception cref="ArgumentException">Thrown when header contains invalid values.</exception>
        public PCMAudioBuffer(AudioFormat header)
        {
            _header = header;

            if (header.BitsPerSample % 8 != 0)
                throw new ArgumentException($"Bits per sample '{header.BitsPerSample}' is not supported. It must be divisible by 8.", nameof(header));

            if (header.BitsPerSample <= 0)
                throw new ArgumentException("Bits per sample must be greater than 0.", nameof(header));

            if (header.ChannelCount <= 0)
                throw new ArgumentException("Channel count must be greater than 0.", nameof(header));

            if (header.SamplesPerSecond <= 0)
                throw new ArgumentException("Samples per second must be greater than 0.", nameof(header));

            _bytesPerSample = header.BitsPerSample / 8;
            _samplesPerSecond = header.SamplesPerSecond;
        }

        /// <inheritdoc/>
        public override int BufferSize
        {
            get
            {
                lock (_lock)
                {
                    return _totalSizeInBytes;
                }
            }
        }

        /// <inheritdoc/>
        public override bool IsEmpty
        {
            get
            {
                lock (_lock)
                {
                    return _audioBuffers.Count == 0;
                }
            }
        }

        /// <inheritdoc/>
        public override TimeSpan BufferedDuration
        {
            get
            {
                lock (_lock)
                {
                    return BytesToDuration(_totalSizeInBytes);
                }
            }
        }

        /// <inheritdoc/>
        public override void Add(DataChunk audioChunk)
        {
            ThrowIfDisposed();

            if (audioChunk == null)
                throw new ArgumentNullException(nameof(audioChunk));

            lock (_lock)
            {
                _audioBuffers.Add(audioChunk);
                _totalSizeInBytes += audioChunk.Data.Length;
            }
        }

        /// <inheritdoc/>
        public override DataChunk GetNext()
        {
            ThrowIfDisposed();

            lock (_lock)
            {
                return GetNextUnlocked();
            }
        }

        /// <inheritdoc/>
        public override void NewTurn()
        {
            ThrowIfDisposed();

            lock (_lock)
            {
                _currentChunk = 0;
            }
        }

        /// <inheritdoc/>
        public override void DiscardBytes(int bytes)
        {
            ThrowIfDisposed();

            if (bytes < 0)
                throw new ArgumentException("Number of bytes to discard cannot be negative.", nameof(bytes));

            lock (_lock)
            {
                DiscardBytesUnlocked(bytes);
            }
        }

        /// <inheritdoc/>
        public override void DiscardTill(TimeSpan offset)
        {
            ThrowIfDisposed();

            if (offset < TimeSpan.Zero)
                throw new ArgumentException("Offset cannot be negative.", nameof(offset));

            lock (_lock)
            {
                DiscardTillUnlocked(offset);
            }
        }

        /// <inheritdoc/>
        public override TimeSpan ToAbsolute(TimeSpan offsetTurnRelative)
        {
            ThrowIfDisposed();

            if (offsetTurnRelative < TimeSpan.Zero)
                throw new ArgumentException("Offset cannot be negative.", nameof(offsetTurnRelative));

            var bytes = DurationToBytes(offsetTurnRelative);
            return BytesToDuration(_bufferStartOffsetInBytesAbsolute + bytes);
        }

        /// <inheritdoc/>
        public override void Drop()
        {
            ThrowIfDisposed();

            lock (_lock)
            {
                var unconfirmedBytes = NonAcknowledgedSizeInBytesUnlocked();
                DiscardBytesUnlocked(unconfirmedBytes);

                DataChunk chunk;
                while ((chunk = GetNextUnlocked()) != null)
                {
                    DiscardBytesUnlocked(chunk.Data.Length);
                }
            }
        }

        /// <inheritdoc/>
        protected override void Dispose(bool disposing)
        {
            if (disposing)
            {
                lock (_lock)
                {
                    _audioBuffers.Clear();
                    _totalSizeInBytes = 0;
                    _currentChunk = 0;
                }
            }

            base.Dispose(disposing);
        }

        private void DiscardBytesUnlocked(int bytes)
        {
            int chunkBytes;
            while (_audioBuffers.Count > 0 && bytes > 0 &&
                   (chunkBytes = _audioBuffers.First().Data.Length) <= bytes)
            {
                bytes -= chunkBytes;
                _audioBuffers.RemoveAt(0);
                _currentChunk = Math.Max(0, _currentChunk - 1);
                _totalSizeInBytes -= chunkBytes;
                _bufferStartOffsetInBytesAbsolute += chunkBytes;
            }

            if (_audioBuffers.Count == 0)
            {
                if (_totalSizeInBytes != 0)
                    throw new InvalidOperationException("Invalid state: buffer is empty but total size is not zero.");

                _currentChunk = 0;
            }
            else if (bytes > 0)
            {
                var chunk = _audioBuffers.First();
                var newChunk = new DataChunk(chunk.Data.Slice(bytes), chunk.ReceivedTime);
                _audioBuffers[0] = newChunk;
                _bufferStartOffsetInBytesAbsolute += bytes;
                _totalSizeInBytes -= bytes;
            }
        }

        private void DiscardTillUnlocked(TimeSpan offset)
        {
            var offsetInBytes = DurationToBytes(offset);

            if (offsetInBytes < _bufferStartOffsetInBytesAbsolute)
                return;

            DiscardBytesUnlocked(offsetInBytes - _bufferStartOffsetInBytesAbsolute);
        }

        private int DurationToBytes(TimeSpan duration)
        {
            if (_samplesPerSecond % MillisecondsInSecond == 0)
            {
                return Convert.ToInt32(_header.ChannelCount * _bytesPerSample *
                                     (_samplesPerSecond / MillisecondsInSecond) *
                                     ((double)duration.Ticks / TicksInMillisecond));
            }
            else
            {
                return Convert.ToInt32(Math.Ceiling((double)_samplesPerSecond / MillisecondsInSecond *
                                                  ((double)duration.Ticks / TicksInMillisecond)) *
                                     _header.ChannelCount * _bytesPerSample);
            }
        }

        private TimeSpan BytesToDuration(int bytes)
        {
            if (_samplesPerSecond % MillisecondsInSecond == 0)
            {
                return TimeSpan.FromTicks((bytes * TicksInMillisecond * MillisecondsInSecond) /
                                        (_header.ChannelCount * _bytesPerSample * _samplesPerSecond));
            }
            else
            {
                return TimeSpan.FromTicks((long)Math.Ceiling(bytes * TicksInMillisecond * MillisecondsInSecond / (double)_samplesPerSecond) /
                                        (_header.ChannelCount * _bytesPerSample));
            }
        }

        private DataChunk GetNextUnlocked()
        {
            if (_currentChunk >= _audioBuffers.Count)
                return null;

            var result = _audioBuffers[_currentChunk];
            _currentChunk++;
            return result;
        }

        private int StashedSizeInBytesUnlocked()
        {
            int size = 0;
            for (int i = _currentChunk; i < _audioBuffers.Count; i++)
            {
                size += _audioBuffers[i].Data.Length;
            }
            return size;
        }

        private int NonAcknowledgedSizeInBytesUnlocked()
        {
            int unconfirmedBytes = 0;
            for (int i = 0; i < _currentChunk && i < _audioBuffers.Count; i++)
            {
                unconfirmedBytes += _audioBuffers[i].Data.Length;
            }
            return unconfirmedBytes;
        }
    }
}
