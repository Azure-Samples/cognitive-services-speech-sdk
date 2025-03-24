//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;

namespace SpeechSDKSamples.Remote.Client.Core
{
    /// <summary>
    /// Abstract base class for audio buffer implementations that store and manage streaming audio data.
    /// </summary>
    /// <remarks>
    /// The buffer stores audio data received from an audio source and manages its transmission and acknowledgment.
    /// It supports rewinding to replay unacknowledged audio data in case of connection issues or other failures.
    /// When NewTurn is called, the buffer rewinds to its beginning, making the oldest chunk the next to send.
    /// The buffer tracks acknowledged audio data and automatically discards it to prevent memory growth.
    /// </remarks>
    public abstract class AudioBuffer : IDisposable
    {
        private bool _disposed;

        /// <summary>
        /// Gets the current size of the buffer in bytes.
        /// </summary>
        public abstract int BufferSize { get; }

        /// <summary>
        /// Gets whether the buffer is empty.
        /// </summary>
        public abstract bool IsEmpty { get; }

        /// <summary>
        /// Gets the duration of audio data currently in the buffer.
        /// </summary>
        public abstract TimeSpan BufferedDuration { get; }

        /// <summary>
        /// Adds an audio chunk to the buffer.
        /// </summary>
        /// <param name="audioChunk">The audio chunk to add.</param>
        /// <exception cref="ArgumentNullException">Thrown when audioChunk is null.</exception>
        /// <exception cref="ObjectDisposedException">Thrown when the buffer has been disposed.</exception>
        /// <exception cref="InvalidOperationException">Thrown when the buffer is full or in an invalid state.</exception>
        public abstract void Add(DataChunk audioChunk);

        /// <summary>
        /// Gets the next chunk of audio from the buffer.
        /// </summary>
        /// <returns>The next chunk, or null if no more chunks are available.</returns>
        /// <exception cref="ObjectDisposedException">Thrown when the buffer has been disposed.</exception>
        /// <exception cref="InvalidOperationException">Thrown when the buffer is in an invalid state.</exception>
        public abstract DataChunk GetNext();

        /// <summary>
        /// Indicates that a new speech recognition turn has started.
        /// </summary>
        /// <remarks>
        /// Resets the buffer's internal read position to the oldest chunk in the buffer.
        /// The next call to GetNext() will return the oldest chunk in the buffer.
        /// </remarks>
        /// <exception cref="ObjectDisposedException">Thrown when the buffer has been disposed.</exception>
        public abstract void NewTurn();

        /// <summary>
        /// Discards all audio data up to the specified offset.
        /// </summary>
        /// <param name="offset">A TimeSpan representing the audio duration to discard.</param>
        /// <exception cref="ArgumentException">Thrown when offset is negative.</exception>
        /// <exception cref="ObjectDisposedException">Thrown when the buffer has been disposed.</exception>
        public abstract void DiscardTill(TimeSpan offset);

        /// <summary>
        /// Discards the specified number of bytes from the buffer.
        /// </summary>
        /// <param name="size">The number of bytes to discard.</param>
        /// <exception cref="ArgumentException">Thrown when size is negative or greater than the buffer size.</exception>
        /// <exception cref="ObjectDisposedException">Thrown when the buffer has been disposed.</exception>
        public abstract void DiscardBytes(int size);

        /// <summary>
        /// Converts a turn-relative offset to an absolute offset from buffer creation.
        /// </summary>
        /// <remarks>
        /// Converts an offset relative to the last NewTurn() call to an offset relative to buffer creation.
        /// This is useful for maintaining consistent timing across multiple recognition turns.
        /// </remarks>
        /// <param name="offsetTurnRelative">The offset relative to the start of the last turn.</param>
        /// <returns>An offset with buffer creation as the reference point (0).</returns>
        /// <exception cref="ArgumentException">Thrown when offsetTurnRelative is negative.</exception>
        /// <exception cref="ObjectDisposedException">Thrown when the buffer has been disposed.</exception>
        public abstract TimeSpan ToAbsolute(TimeSpan offsetTurnRelative);

        /// <summary>
        /// Drops all data in the buffer.
        /// </summary>
        /// <exception cref="ObjectDisposedException">Thrown when the buffer has been disposed.</exception>
        public abstract void Drop();

        /// <summary>
        /// Releases all resources used by the AudioBuffer.
        /// </summary>
        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        /// <summary>
        /// Releases the unmanaged resources used by the AudioBuffer and optionally releases the managed resources.
        /// </summary>
        /// <param name="disposing">true to release both managed and unmanaged resources; false to release only unmanaged resources.</param>
        protected virtual void Dispose(bool disposing)
        {
            if (_disposed)
                return;

            if (disposing)
            {
                Drop();
            }

            _disposed = true;
        }

        /// <summary>
        /// Throws an ObjectDisposedException if the buffer has been disposed.
        /// </summary>
        /// <exception cref="ObjectDisposedException">Thrown when the buffer has been disposed.</exception>
        protected void ThrowIfDisposed()
        {
            if (_disposed)
                throw new ObjectDisposedException(GetType().Name);
        }
    }
}
