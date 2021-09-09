// <copyright file="AudioUtilities.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace FetchTranscription
{
    using System;
    using System.Collections.Generic;
    using System.IO;
    using Connector.Extensions;
    using NAudio.Wave;
    using Newtonsoft.Json.Linq;

    public static class AudioUtilities
    {
        public static byte[] RedactAudio(byte[] bytes, JObject redactionResponse, bool isDiarizationResult)
        {
            redactionResponse = redactionResponse ?? throw new ArgumentNullException(nameof(redactionResponse));
            bytes = bytes ?? throw new ArgumentNullException(nameof(bytes));

            if (NeedsConversion(bytes))
            {
                bytes = ConvertFormat(bytes);
            }

            var memoryStream = new MemoryStream(bytes);
            using var waveFileReader = new WaveFileReader(memoryStream);

            var sampleRate = waveFileReader.WaveFormat.SampleRate;
            var channelsCount = waveFileReader.WaveFormat.Channels;
            var millisecondsPerFrame = 1000 / (float)sampleRate;
            var framesPerMs = (float)sampleRate / 1000;

            var frames = new List<float>();

            float[] buffer;
            while ((buffer = waveFileReader.ReadNextSampleFrame())?.Length > 0)
            {
                frames.AddRange(buffer);
            }

            var framesArray = frames.ToArray();

            foreach (var redactedTranscripts in redactionResponse["redactedTranscript"])
            {
                var channel = isDiarizationResult ? 0 : redactedTranscripts["channel"].ToObject<int>();
                foreach (var redactedAudioSpans in redactedTranscripts["redactedAudioSpans"])
                {
                    var startOffset = redactedAudioSpans["startOffset"].ToObject<long>();
                    var duration = redactedAudioSpans["duration"].ToObject<long>();

                    var offsetInMs = startOffset / 10000;
                    var durationInMs = duration / 10000;

                    var offsetInFrames = (long)Math.Floor(offsetInMs * framesPerMs);
                    var durationInFrames = (int)Math.Ceiling(durationInMs * framesPerMs);

                    framesArray.RedactFrames(offsetInFrames, durationInFrames, channel, channelsCount);
                }
            }

            var outStream = new MemoryStream();
            using var writer = new WaveFileWriter(outStream, new WaveFormat(sampleRate, channelsCount));

            for (var i = 0;  i < framesArray.Length; i++)
            {
                writer.WriteSample(framesArray[i]);
            }

            writer.Flush();
            var outBytes = outStream.ToArray();

            return outBytes;
        }

        private static byte[] ConvertFormat(byte[] input)
        {
            using var memstream = new MemoryStream();

            using var reader = new WaveFileReader(new MemoryStream(input));
            var newFormat = new WaveFormat(reader.WaveFormat.SampleRate, 16, reader.WaveFormat.Channels);

            using var conversionStream = new WaveFormatConversionStream(newFormat, reader);
            WaveFileWriter.WriteWavFileToStream(memstream, conversionStream);

            memstream.Flush();

            return memstream.ToArray();
        }

        private static bool NeedsConversion(byte[] bytes)
        {
            var memoryStream = new MemoryStream(bytes);
            using var waveFileReader = new WaveFileReader(memoryStream);

            return waveFileReader.WaveFormat.Encoding == WaveFormatEncoding.MuLaw ||
                (waveFileReader.WaveFormat.BitsPerSample != 16 &&
                waveFileReader.WaveFormat.BitsPerSample != 24 &&
                waveFileReader.WaveFormat.BitsPerSample != 32);
        }
    }
}
