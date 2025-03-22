//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;

namespace SpeechSDKSamples.Remote.Client.Core
{
    /// <summary>
    /// Represents an immutable chunk of audio data along with its reception timestamp.
    /// </summary>
    public sealed class DataChunk
    {
        /// <summary>
        /// Initializes a new instance of the DataChunk class.
        /// </summary>
        /// <param name="data">The audio data to store in this chunk.</param>
        /// <param name="receivedTime">The time when this audio chunk was received.</param>
        /// <exception cref="ArgumentException">Thrown when data is empty.</exception>
        public DataChunk(ReadOnlyMemory<byte> data, DateTime receivedTime)
        {
            Data = data;
            ReceivedTime = receivedTime;
        }

        /// <summary>
        /// Gets the audio data contained in this chunk.
        /// </summary>
        public ReadOnlyMemory<byte> Data { get; }

        /// <summary>
        /// Gets the time when this audio chunk was received by the client.
        /// </summary>
        public DateTime ReceivedTime { get; }

        /// <summary>
        /// Gets the size of the audio data in bytes.
        /// </summary>
        public int Size => Data.Length;

        /// <summary>
        /// Creates an empty data chunk with the current timestamp.
        /// </summary>
        /// <returns>A new DataChunk instance with empty data.</returns>
        public static DataChunk CreateEmpty()
        {
            return new DataChunk(ReadOnlyMemory<byte>.Empty, DateTime.UtcNow);
        }

        /// <summary>
        /// Creates a data chunk with the specified data and the current timestamp.
        /// </summary>
        /// <param name="data">The audio data to store in the chunk.</param>
        /// <returns>A new DataChunk instance.</returns>
        /// <exception cref="ArgumentException">Thrown when data is empty.</exception>
        public static DataChunk Create(ReadOnlyMemory<byte> data)
        {
            return new DataChunk(data, DateTime.UtcNow);
        }

        /// <summary>
        /// Returns a string that represents the current object.
        /// </summary>
        /// <returns>A string containing the size of the chunk and when it was received.</returns>
        public override string ToString()
        {
            return $"DataChunk[Size={Size} bytes, Received={ReceivedTime:yyyy-MM-dd HH:mm:ss.fff}]";
        }
    }
}
