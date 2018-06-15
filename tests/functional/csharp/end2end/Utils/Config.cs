//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Text.RegularExpressions;

namespace Microsoft.CognitiveServices.Speech.Tests.EndToEnd
{
    static class Config
    {
        public static T GetSettingByKey<T>(TestContext context, string key)
        {
            var value = context.Properties[key];
            if (string.IsNullOrWhiteSpace(value.ToString()) || value.Equals(string.Empty))
            {
                throw new System.Exception("Field " + key + " is missing in the runsettings");
            }
            return (T)(object)(value);
        }

        public static List<byte[]> GetByteArraysForFilesWithSamePrefix(string dir, string prefix)
        {
            List<byte[]> byteArrList = new List<byte[]>();
            var fileInfos = new DirectoryInfo(dir).GetFiles(string.Concat(prefix,"*.*"));
            for (int i = 1; i <= fileInfos.Length; i++)
            {
                byteArrList.Add(File.ReadAllBytes(Path.Combine(dir, string.Concat(prefix, i, ".wav"))));
            }
            return byteArrList;
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

        public static bool AreResultsMatching(string actualText, string expectedText)
        {
            string plainActualText = Normalize(actualText);
            string plainExpectedText = Normalize(expectedText);

            if (plainActualText.Length <= plainExpectedText.Length)
            {
                return plainExpectedText.Contains(plainActualText);
            }
            else
            {
                return plainActualText.Contains(plainExpectedText);
            }
        }

        private static string Normalize(string str)
        {
            return Regex.Replace(str, "[^a-zA-Z0-9' ]+", "", RegexOptions.Compiled).ToLower();
        }
    }
}
