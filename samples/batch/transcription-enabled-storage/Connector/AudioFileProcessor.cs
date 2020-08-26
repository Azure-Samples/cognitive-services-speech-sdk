// <copyright file="AudioFileProcessor.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector
{
    using System;
    using System.IO;
    using NAudio.Wave;

    public static class AudioFileProcessor
    {
        // prices in euro
        private const double STTCostPerHour = 0.844d;

        private const double STTCustomModelCostPerHour = 1.181d;

        private const double TextAnalyticsCostPerHour = 0.72d;

        public static TimeSpan GetDuration(byte[] fileBytes, string fileNameExtension)
        {
            if (fileNameExtension == null)
            {
                throw new ArgumentNullException(nameof(fileNameExtension));
            }

            using (var reader = GetWaveStream(fileBytes, fileNameExtension))
            {
                return reader.TotalTime;
            }
        }

        public static int GetNumberOfChannels(byte[] fileBytes, string fileNameExtension)
        {
            if (fileNameExtension == null)
            {
                throw new ArgumentNullException(nameof(fileNameExtension));
            }

            using (var reader = GetWaveStream(fileBytes, fileNameExtension))
            {
                return reader.WaveFormat.Channels;
            }
        }

        public static double GetCostEstimation(
            TimeSpan timeSpan,
            int numberOfChannels,
            bool isCustomModel,
            bool sentimentAnalysisAdded,
            bool entityRedactionAdded)
        {
            double costPerHour = isCustomModel ? STTCustomModelCostPerHour : STTCostPerHour;
            var price = timeSpan.TotalHours * costPerHour;

            if (sentimentAnalysisAdded)
            {
                price += timeSpan.TotalHours * TextAnalyticsCostPerHour;
            }

            if (entityRedactionAdded)
            {
                price += timeSpan.TotalHours * TextAnalyticsCostPerHour;
            }

            price *= numberOfChannels;
            return price;
        }

        public static byte[] ConvertToWaveBytes(byte[] fileBytes, string fileNameExtension)
        {
            if (fileNameExtension == null)
            {
                throw new ArgumentNullException(nameof(fileNameExtension));
            }

            using var wavStream = GetWaveStream(fileBytes, fileNameExtension);
            wavStream.Position = wavStream.Length / 2;

            using var memStream = new MemoryStream();

            using (var pcmStream = WaveFormatConversionStream.CreatePcmStream(wavStream))
            {
                WaveFileWriter.WriteWavFileToStream(memStream, pcmStream);
            }

            return memStream.ToArray();
        }

        private static WaveStream GetWaveStream(byte[] fileBytes, string fileNameExtension)
        {
            if (fileNameExtension.Equals(".mp3", StringComparison.OrdinalIgnoreCase))
            {
                return new Mp3FileReader(new MemoryStream(fileBytes));
            }

            if (fileNameExtension.Equals(".aiff", StringComparison.OrdinalIgnoreCase))
            {
                return new AiffFileReader(new MemoryStream(fileBytes));
            }

            return new WaveFileReader(new MemoryStream(fileBytes));
        }
    }
}
