//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using Microsoft.CognitiveServices.Speech;
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
        internal sealed class RepeatedAudioInputStream : AudioInputStream
        {
            private readonly List<AudioInputStream> streamsWithEqualFormat;
            private readonly AudioInputStreamFormat format;

            /// <summary>
            /// Creates an object.
            /// </summary>
            /// <param name="streamsWithEqualFormat">Streams with equal format</param>
            public RepeatedAudioInputStream(List<AudioInputStream> streamsWithEqualFormat)
            {
                if (streamsWithEqualFormat == null || streamsWithEqualFormat.Count < 1)
                {
                    throw new ArgumentException("Please provide valid streams");
                }

                this.streamsWithEqualFormat = streamsWithEqualFormat;
                this.format = streamsWithEqualFormat.First().GetFormat();
            }

            public override AudioInputStreamFormat GetFormat()
            {
                return this.format;
            }

            public override int Read(byte[] dataBuffer)
            {
                while (this.streamsWithEqualFormat.Count > 0)
                {
                    var result = this.streamsWithEqualFormat.First().Read(dataBuffer);
                    if (result != 0)
                    {
                        return result;
                    }
                    this.streamsWithEqualFormat.First().Close();
                    this.streamsWithEqualFormat.RemoveAt(0);
                }
                return 0;
            }
        }

        public static AudioInputStream OpenWaveFile(string filename, int times)
        {
            var streams = new List<AudioInputStream>();
            for (int i = 0; i < times; ++i)
            {
                streams.Add(OpenWaveFile(filename));
            }

            return new RepeatedAudioInputStream(streams);
        }

        public static AudioInputStream OpenWaveFile(string filename)
        {
            BinaryReader reader = new BinaryReader(File.OpenRead(filename));
            AudioInputStreamFormat format = new AudioInputStreamFormat();

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
            long formatSize = reader.ReadInt32();
            format.FormatTag = reader.ReadUInt16();
            format.Channels = reader.ReadUInt16();
            format.SamplesPerSec = (int)reader.ReadUInt32();
            format.AvgBytesPerSec = (int)reader.ReadUInt32();
            format.BlockAlign = reader.ReadUInt16();
            format.BitsPerSample = reader.ReadUInt16();

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
            return new BinaryAudioStreamReader(format, reader);
        }

        public static async Task<string> GetToken(string key)
        {
            string fetchTokenUri = "https://westus.api.cognitive.microsoft.com/sts/v1.0";
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
