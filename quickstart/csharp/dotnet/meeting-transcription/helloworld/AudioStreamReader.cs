//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Microsoft.CognitiveServices.Speech;
using Microsoft.CognitiveServices.Speech.Audio;

namespace helloworld
{
    class AudioStreamReader : PullAudioInputStreamCallback
    {
        private System.IO.BinaryReader binaryReader;
        private bool disposed = false;
        
        /// <summary>
        /// Creates and initializes an instance of BinaryAudioStreamReader.
        /// </summary>
        /// <param name="reader">The underlying stream to read the audio data from. Note: The stream contains the bare sample data, not the container (like wave header data, etc).</param>
        public AudioStreamReader(System.IO.BinaryReader reader)
        {
            binaryReader = reader;
        }

        /// <summary>
        /// Creates and initializes an instance of BinaryAudioStreamReader.
        /// </summary>
        /// <param name="stream">The underlying stream to read the audio data from. Note: The stream contains the bare sample data, not the container (like wave header data, etc).</param>
        public AudioStreamReader(System.IO.Stream stream)
            : this(new System.IO.BinaryReader(stream))
        {
        }

        /// <summary>
        /// Reads binary data from the stream.
        /// </summary>
        /// <param name="dataBuffer">The buffer to fill</param>
        /// <param name="size">The size of the buffer.</param>
        /// <returns>The number of bytes filled, or 0 in case the stream hits its end and there is no more data available.
        /// If there is no data immediate available, Read() blocks until the next data becomes available.</returns>
        public override int Read(byte[] dataBuffer, uint size)
        {
            return binaryReader.Read(dataBuffer, 0, (int)size);
        }

        /// <summary>
        /// Get property associated to data buffer, such as a timestamp or userId. if the property is not available, an empty string must be returned. 
        /// </summary>
        /// <param name="id">A property id.</param>
        /// <returns>The value of the property </returns>
        public override string GetProperty(PropertyId id)
        {
            if (PropertyId.ConversationTranscribingService_DataBufferUserId == id)
            {
                return "";
            }
            else if (PropertyId.ConversationTranscribingService_DataBufferTimeStamp == id)
            {
                return "";
            }
            return "";
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
                binaryReader.Dispose();
            }

            disposed = true;
            base.Dispose(disposing);
        }

        /// <summary>
        /// This method opens wavefile.
        /// </summary>
        /// <param name="filename">The wavfile to read the audio data from.</param>
        public static AudioConfig OpenWavFile(string filename)
        {
            AudioStreamFormat format = null;
            var callback = OpenWavFileStream(filename, out format);
            return AudioConfig.FromStreamInput(callback, format);
        }

        private static PullAudioInputStreamCallback OpenWavFileStream(string filename, out AudioStreamFormat format)
        {
            BinaryReader reader = new BinaryReader(File.OpenRead(filename));

            // Tag "RIFF"
            char[] data = new char[4];
            reader.Read(data, 0, 4);
            Trace.Assert((data[0] == 'R') && (data[1] == 'I') && (data[2] == 'F') && (data[3] == 'F'), "Wrong wav header");

            // Chunk size
            long fileSize = reader.ReadInt32();

            // Subchunk, Wave Header
            // Subchunk, Format
            // Tag: "WAVE"
            reader.Read(data, 0, 4);
            Trace.Assert((data[0] == 'W') && (data[1] == 'A') && (data[2] == 'V') && (data[3] == 'E'), "Wrong wav tag in wav header");

            // Tag: "fmt"
            reader.Read(data, 0, 4);
            Trace.Assert((data[0] == 'f') && (data[1] == 'm') && (data[2] == 't') && (data[3] == ' '), "Wrong format tag in wav header");

            // chunk format size
            var formatSize = reader.ReadInt32();
            var unusedFormatTag = reader.ReadUInt16();
            var channels = reader.ReadUInt16();
            var samplesPerSecond = reader.ReadUInt32();
            var unusedAvgBytesPerSec = reader.ReadUInt32();
            var unusedBlockAlign = reader.ReadUInt16();
            var bitsPerSample = reader.ReadUInt16();

            // Until now we have read 16 bytes into format, the rest is cbSize and is ignored for now.
            if (formatSize > 16)
                reader.ReadBytes((int)(formatSize - 16));

            bool foundDataChunk = false;
            while (!foundDataChunk)
            {
                reader.Read(data, 0, 4);
                var chunkSize = reader.ReadInt32();
                if ((data[0] == 'd') && (data[1] == 'a') && (data[2] == 't') && (data[3] == 'a'))
                {
                    foundDataChunk = true;
                    break;
                }
                reader.ReadBytes(chunkSize);
            }
            if (!foundDataChunk)
            {
                throw new System.ApplicationException("${filename} does not contain a data chunk!");
            }

            // Now, we have the format in the format parameter and the
            // reader set to the start of the body, i.e., the raw sample data
            format = AudioStreamFormat.GetWaveFormatPCM(samplesPerSecond, (byte)bitsPerSample, (byte)channels);
            return new AudioStreamReader(reader);
        }
    }
}
