//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using Windows.Storage.Streams;
using Windows.Storage;
using Microsoft.CognitiveServices.Speech;
using Microsoft.CognitiveServices.Speech.Audio;
using System.Diagnostics;
using System.IO;
using System;
namespace MicrosoftSpeechSDKSamples
{
    public class Helper
    {
        public static AudioConfig OpenWaveFile(BinaryReader reader)
        {
            // Tag "RIFF"
            char[] data = new char[4];
            reader.Read(data, 0, 4);
            if (data[0] != 'R' || data[1] != 'I' || data[2] != 'F' || data[3] != 'F')
            {
                throw new global::System.FormatException("Wrong wav header");
            }
            
            // Chunk size
            long fileSize = reader.ReadInt32();

            // Subchunk, Wave Header
            // Subchunk, Format
            // Tag: "WAVE"
            reader.Read(data, 0, 4);
            if ((data[0] != 'W') || (data[1] != 'A') || (data[2] != 'V') || (data[3] != 'E'))
            {
                throw new global::System.FormatException("Wrong wav tag in wav header");
            }

            // Tag: "fmt"
            reader.Read(data, 0, 4);
            if ((data[0] != 'f') || (data[1] != 'm') || (data[2] != 't') && (data[3] != ' '))
            {
                throw new global::System.FormatException("Wrong format tag in wav header");
            }

            // chunk format size
            var formatSize = reader.ReadInt32();
            var formatTag = reader.ReadUInt16();
            var channels = reader.ReadUInt16();
            var samplesPerSec = reader.ReadUInt32();
            var avgBytesPerSec = (int)reader.ReadUInt32();
            var blockAlign = reader.ReadUInt16();
            var bitsPerSample = reader.ReadUInt16();

            // Until now we have read 16 bytes in format, the rest is cbSize and is ignored for now.
            if (formatSize > 16)
                reader.ReadBytes((int)(formatSize - 16));

            // Second Chunk, data
            // tag: data.
            reader.Read(data, 0, 4);

            if ((data[0] != 'd') || (data[1] != 'a') || (data[2] != 't') || (data[3] != 'a'))
            {
                throw new global::System.FormatException("Wrong data tag in wav");
            }
            // data chunk size
            int dataSize = reader.ReadInt32();

            // now, we have the format in the format parameter and the
            // reader set to the start of the body, i.e., the raw sample data
            AudioStreamFormat format = AudioStreamFormat.GetWaveFormatPCM(samplesPerSec, (byte)bitsPerSample, (byte)channels);
            return AudioConfig.FromStreamInput(new BinaryAudioStreamReader(reader), format);
        }
    }
}