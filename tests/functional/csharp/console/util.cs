//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using Microsoft.CognitiveServices.Speech;
using Microsoft.CognitiveServices.Speech.Audio;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Net.Http;
using System.Threading.Tasks;

namespace MicrosoftSpeechSDKSamples
{
    public class Util
    {
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
            /// <param name="size">The size of the buffer.</param>
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

        internal sealed class RepeatedAudioInputStream : PullAudioInputStreamCallback
        {
            private readonly List<PullAudioInputStreamCallback> callbacks;

            public RepeatedAudioInputStream(List<PullAudioInputStreamCallback> callbacks)
            {
                if (callbacks == null || callbacks.Count < 1)
                {
                    throw new ArgumentException("Please provide valid streams");
                }

                this.callbacks = callbacks;
            }

            public override int Read(byte[] dataBuffer, uint size)
            {
                while (this.callbacks.Count > 0)
                {
                    var result = this.callbacks.First().Read(dataBuffer, size);
                    if (result != 0)
                    {
                        return result;
                    }
                    this.callbacks.First().Close();
                    this.callbacks.RemoveAt(0);
                }
                return 0;
            }

            public override void Close()
            {
                while (this.callbacks.Count > 0)
                {
                    this.callbacks.First().Close();
                    this.callbacks.RemoveAt(0);
                }
            }
        }

        public static AudioConfig OpenWavFile(string filename, int times)
        {
            AudioStreamFormat format = null;
            var callbacks = new List<PullAudioInputStreamCallback>();
            for (int i = 0; i < times; ++i)
            {
                callbacks.Add(OpenWavFile(filename, out format));
            }

             return AudioConfig.FromStreamInput(new RepeatedAudioInputStream(callbacks), format);
        }

        public static AudioConfig OpenWavFile(string filename)
        {
            AudioStreamFormat format = null;
            var callback = OpenWavFile(filename, out format);
            return AudioConfig.FromStreamInput(callback, format);
        }

        private static PullAudioInputStreamCallback OpenWavFile(string filename, out AudioStreamFormat format)
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

            // The following code is wrong. As the cbSize is not specified
            // at this position, but should be derived from formatSize.
            // skip over reamining header bytes, if any
            //int cbSize = reader.ReadUInt16();
            //if (cbSize > 0) reader.ReadBytes(cbSize);
            // Until now we have read 16 bytes in format, the rest is cbSize and is ignored for now.
            if (formatSize > 16)
                reader.ReadBytes((int)(formatSize - 16));

            // Second Chunk, data
            // tag: data.
            reader.Read(data, 0, 4);
            Trace.Assert((data[0] == 'd') && (data[1] == 'a') && (data[2] == 't') && (data[3] == 'a'), "Wrong data tag in wav");
            // data chunk size
            int dataSize = reader.ReadInt32();

            // now, we have the format in the format parameter and the
            // reader set to the start of the body, i.e., the raw sample data
            format = AudioStreamFormat.GetWaveFormatPCM(samplesPerSecond, (byte)bitsPerSample, (byte)channels);
            return new BinaryAudioStreamReader(reader);
        }

        public static async Task<string> GetToken(string key, string region)
        {
            string fetchTokenUri = $"https://{region}.api.cognitive.microsoft.com/sts/v1.0";
            using (var client = new HttpClient())
            {
                client.DefaultRequestHeaders.Add("Ocp-Apim-Subscription-Key", key);
                UriBuilder uriBuilder = new UriBuilder(fetchTokenUri);
                uriBuilder.Path += "/issueToken";

                using (var result = await client.PostAsync(uriBuilder.Uri.AbsoluteUri, null))
                {
                    Console.WriteLine("Token Uri: {0}", uriBuilder.Uri.AbsoluteUri);
                    if (result.IsSuccessStatusCode)
                    {
                        return await result.Content.ReadAsStringAsync();
                    }
                    else
                    {
                        throw new HttpRequestException($"Cannot get token from {fetchTokenUri}. Error: {result.StatusCode}");
                    }
                }
            }
        }
    }
}
