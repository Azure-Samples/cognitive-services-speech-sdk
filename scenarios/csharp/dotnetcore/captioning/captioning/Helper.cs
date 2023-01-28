//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using Microsoft.CognitiveServices.Speech;
using Microsoft.CognitiveServices.Speech.Audio;

namespace Captioning
{
    public static class LinqHelper
    {
        // Transform list {1,2,3,4} into {(1,2),(2,3),(3,4)}.
        public static IEnumerable<(T, T)> Pairwise<T>(this IEnumerable<T> xs)
        {
            return xs.Zip(xs.Skip(1), (a, b) => (a, b));
        }
    }
    
    // This code is adapted from:
    // https://github.com/Azure-Samples/cognitive-services-speech-sdk/blob/master/samples/csharp/sharedcontent/console/helper.cs
    public static class Helper
    {
        public static AudioConfig OpenWavFile(string filename, AudioProcessingOptions audioProcessingOptions)
        {
            BinaryReader reader = new BinaryReader(File.OpenRead(filename));
            return OpenWavFile(reader, audioProcessingOptions);
        }

        public static AudioConfig OpenWavFile(BinaryReader reader, AudioProcessingOptions audioProcessingOptions)
        {
            AudioStreamFormat format = readWaveHeader(reader);
            return (audioProcessingOptions == null)
                    ? AudioConfig.FromStreamInput(new BinaryAudioStreamReader(reader), format)
                    : AudioConfig.FromStreamInput(new BinaryAudioStreamReader(reader), format, audioProcessingOptions);
        }

        public static BinaryAudioStreamReader CreateWavReader(string filename)
        {
            BinaryReader reader = new BinaryReader(File.OpenRead(filename));
            // read the wave header so that it won't get into the in the following readings
            AudioStreamFormat format = readWaveHeader(reader);
            return new BinaryAudioStreamReader(reader);
        }

        public static BinaryAudioStreamReader CreateBinaryFileReader(string filename)
        {
            BinaryReader reader = new BinaryReader(File.OpenRead(filename));
            return new BinaryAudioStreamReader(reader);
        }

        public static AudioStreamFormat readWaveHeader(BinaryReader reader)
        {
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
            int formatSize = reader.ReadInt32();
            ushort formatTag = reader.ReadUInt16();
            ushort channels = reader.ReadUInt16();
            uint samplesPerSecond = reader.ReadUInt32();
            uint avgBytesPerSec = reader.ReadUInt32();
            ushort blockAlign = reader.ReadUInt16();
            ushort bitsPerSample = reader.ReadUInt16();

            // Until now we have read 16 bytes in format, the rest is cbSize and is ignored for now.
            if (formatSize > 16)
                reader.ReadBytes((int)(formatSize - 16));

            // Handle optional LIST chunk.
            // tag: "LIST"
            reader.Read(data, 0, 4);
            if (data[0] == 'L' && data[1] == 'I' && data[2] == 'S' && data[3] == 'T')
            {
                uint listChunkSize = reader.ReadUInt32();
                reader.ReadBytes((int)listChunkSize);
                reader.Read(data, 0, 4);
            }

            // Second Chunk, data
            // tag: "data"
            Trace.Assert((data[0] == 'd') && (data[1] == 'a') && (data[2] == 't') && (data[3] == 'a'), "Wrong data tag in wav");
            // data chunk size
            int dataSize = reader.ReadInt32();

            // now, we have the format in the format parameter and the
            // reader set to the start of the body, i.e., the raw sample data
            return AudioStreamFormat.GetWaveFormatPCM(samplesPerSecond, (byte)bitsPerSample, (byte)channels);
        }
    }

    /// <summary>
    /// Adapter class to the native stream api.
    /// </summary>
    public sealed class BinaryAudioStreamReader : PullAudioInputStreamCallback
    {
        private System.IO.BinaryReader _reader;

        /// <summary>
        /// Creates and initializes an instance of BinaryAudioStreamReader.
        /// </summary>
        /// <param name="reader">The underlying stream to read the audio data from. Note: The stream contains the bare sample data, not the container (like wave header data, etc).</param>
        public BinaryAudioStreamReader(System.IO.BinaryReader reader)
        {
            _reader = reader;
        }

        /// <summary>
        /// Creates and initializes an instance of BinaryAudioStreamReader.
        /// </summary>
        /// <param name="stream">The underlying stream to read the audio data from. Note: The stream contains the bare sample data, not the container (like wave header data, etc).</param>
        public BinaryAudioStreamReader(System.IO.Stream stream)
            : this(new System.IO.BinaryReader(stream))
        {
        }

        /// <summary>
        /// Reads binary data from the stream.
        /// </summary>
        /// <param name="dataBuffer">The buffer to fill</param>
        /// <param name="size">The size of data in the buffer.</param>
        /// <returns>The number of bytes filled, or 0 in case the stream hits its end and there is no more data available.
        /// If there is no data immediate available, Read() blocks until the next data becomes available.</returns>
        public override int Read(byte[] dataBuffer, uint size)
        {
            return _reader.Read(dataBuffer, 0, (int)size);
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
                _reader.Dispose();
            }

            disposed = true;
            base.Dispose(disposing);
        }

        private bool disposed = false;
    }
}