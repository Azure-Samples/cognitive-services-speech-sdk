//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Diagnostics.Contracts;
using System.Globalization;
using System.Runtime.InteropServices;
using System.Threading.Tasks;
using Microsoft.CognitiveServices.Speech.Internal;
using static Microsoft.CognitiveServices.Speech.Internal.SpxExceptionThrower;

namespace Microsoft.CognitiveServices.Speech
{
    /// <summary>
    /// Represents audio data stream used for operating audio data as a stream.
    /// Added in version 1.4.0
    /// </summary>
    public class AudioDataStream : IDisposable
    {
        /// <summary>
        /// Creates a memory backed AudioDataStream from given speech synthesis result.
        /// </summary>
        /// <param name="result">The speech synthesis result.</param>
        /// <returns>The audio data stream being created.</returns>
        public static AudioDataStream FromResult(SpeechSynthesisResult result)
        {
            IntPtr streamHandle = IntPtr.Zero;
            ThrowIfFail(Internal.AudioDataStream.audio_data_stream_create_from_result(out streamHandle, result.resultHandle));
            return new AudioDataStream(streamHandle);
        }

        internal AudioDataStream(IntPtr streamHandlePtr)
        {
            ThrowIfNull(streamHandlePtr);
            streamHandle = new InteropSafeHandle(streamHandlePtr, Internal.AudioDataStream.audio_data_stream_release);

            IntPtr propertyHandle = IntPtr.Zero;
            ThrowIfFail(Internal.AudioDataStream.audio_data_stream_get_property_bag(streamHandle, out propertyHandle));
            Properties = new PropertyCollection(propertyHandle);
        }

        /// <summary>
        /// Get current status of the audio data stream.
        /// </summary>
        /// <returns>Current status.</returns>
        public StreamStatus GetStatus()
        {
            ThrowIfNull(streamHandle, "Invalid stream handle.");

            StreamStatus status;
            ThrowIfFail(Internal.AudioDataStream.audio_data_stream_get_status(streamHandle, out status));
            return status;
        }

        /// <summary>
        /// Check whether the stream has enough data to be read.
        /// </summary>
        /// <param name="bytesRequested">The requested data size in bytes.</param>
        /// <returns>A bool indicating whether the stream has enough data to be read.</returns>
        public bool CanReadData(uint bytesRequested)
        {
            ThrowIfNull(streamHandle, "Invalid stream handle.");

            return Internal.AudioDataStream.audio_data_stream_can_read_data(streamHandle, bytesRequested);
        }

        /// <summary>
        /// Check whether the stream has enough data to be read, starting from the specified position.
        /// </summary>
        /// <param name="pos">The position counting from start of the stream.</param>
        /// <param name="bytesRequested">The requested data size in bytes.</param>
        /// <returns>A bool indicating whether the stream has enough data to be read.</returns>
        public bool CanReadData(uint pos, uint bytesRequested)
        {
            ThrowIfNull(streamHandle, "Invalid stream handle.");

            return Internal.AudioDataStream.audio_data_stream_can_read_data_from_position(streamHandle, bytesRequested, pos);
        }

        /// <summary>
        /// Reads the audio data from the audio data stream.
        /// The maximal number of bytes to be read is determined by the size of buffer.
        /// If there is no data immediately available, ReadData() blocks until the next data becomes available.
        /// </summary>
        /// <param name="buffer">The buffer to receive the audio data.</param>
        /// <returns>The number of bytes filled, or 0 in case the stream hits its end and there is no more data available.</returns>
        public uint ReadData(byte[] buffer)
        {
            ThrowIfNull(streamHandle, "Invalid stream handle.");

            uint filledSize;
            IntPtr nativeBuffer = Marshal.AllocHGlobal(buffer.Length);

            try
            {
                ThrowIfFail(Internal.AudioDataStream.audio_data_stream_read(streamHandle, nativeBuffer, (uint)(buffer.Length), out filledSize));
                filledSize = filledSize % int.MaxValue;
                Marshal.Copy(nativeBuffer, buffer, 0, (int)filledSize);
            }
            finally
            {
                Marshal.FreeHGlobal(nativeBuffer);
            }

            return filledSize;
        }

        /// <summary>
        /// Reads the audio data from the audio data stream, starting from the specified position.
        /// The maximal number of bytes to be read is determined by the size of buffer.
        /// If there is no data immediately available, ReadData() blocks until the next data becomes available.
        /// </summary>
        /// <param name="pos">The position counting from start of the stream.</param>
        /// <param name="buffer">The buffer to receive the audio data.</param>
        /// <returns>The number of bytes filled, or 0 in case the stream hits its end and there is no more data available.</returns>
        public uint ReadData(uint pos, byte[] buffer)
        {
            ThrowIfNull(streamHandle, "Invalid stream handle.");

            uint filledSize;
            IntPtr nativeBuffer = Marshal.AllocHGlobal(buffer.Length);

            try
            {
                ThrowIfFail(Internal.AudioDataStream.audio_data_stream_read_from_position(streamHandle, nativeBuffer, (uint)(buffer.Length), pos, out filledSize));
                filledSize = filledSize % int.MaxValue;
                Marshal.Copy(nativeBuffer, buffer, 0, (int)filledSize);
            }
            finally
            {
                Marshal.FreeHGlobal(nativeBuffer);
            }

            return filledSize;
        }

        /// <summary>
        /// Save the audio data to a file, asynchronously.
        /// </summary>
        /// <param name="fileName">Name of the file for saving.</param>
        /// <returns>An asynchronous operation representing the saving.</returns>
        public Task SaveToWaveFileAsync(string fileName)
        {
            ThrowIfNull(streamHandle, "Invalid stream handle.");

            return Task.Run(() => {
                ThrowIfFail(Internal.AudioDataStream.audio_data_stream_save_to_wave_file(streamHandle, fileName));
            });
        }

        /// <summary>
        /// Set current position of the audio data stream.
        /// </summary>
        /// <param name="pos">Position to be set.</param>
        public void SetPosition(uint pos)
        {
            ThrowIfNull(streamHandle, "Invalid stream handle.");

            ThrowIfFail(Internal.AudioDataStream.audio_data_stream_set_position(streamHandle, pos));
        }

        /// <summary>
        /// Get current position of the audio data stream.
        /// </summary>
        /// <returns>Current position.</returns>
        public uint GetPosition()
        {
            ThrowIfNull(streamHandle, "Invalid stream handle.");

            uint pos;
            ThrowIfFail(Internal.AudioDataStream.audio_data_stream_get_position(streamHandle, out pos));
            return pos;
        }

        /// <summary>
        /// Dispose of associated resources.
        /// </summary>
        public void Dispose()
        {
            Properties.Close();
            streamHandle.Dispose();
        }

        /// <summary>
        /// Contains properties of the audio data stream.
        /// </summary>
        public PropertyCollection Properties { get; private set; }

        internal InteropSafeHandle streamHandle;
    }
}
