//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using SpeechSDKSamples.Remote.Core;
using System.Diagnostics;
using System.Runtime.CompilerServices;

[assembly: InternalsVisibleTo("Tests")]

namespace SpeechSDKSamples.Remote.Client.App
{
    /// <summary>
    /// Utility to help reading and parsing wav file headers.
    /// </summary>
    /// <exception cref="InvalidOperationException">Thrown when the file is not a valid WAV file.</exception>
    internal static class WavFileUtilities
    {
        private const string RiffTag = "RIFF";
        private const string WaveTag = "WAVE";
        private const string FmtTag = "fmt ";
        private const string ListTag = "LIST";
        private const string DataTag = "data";
        private const int MinFormatChunkSize = 16;
        private const int StandardWavHeaderSize = 44;

        /// <summary>
        /// Gets the audio format of a WAV file.
        /// </summary>
        /// <param name="filePath">Path to the WAV file.</param>
        /// <returns>An <see cref="AudioFormat"/> object with information about the audio format in the file.</returns>
        /// <exception cref="ArgumentNullException">Thrown when filePath is null.</exception>
        /// <exception cref="FileNotFoundException">Thrown when the file does not exist.</exception>
        /// <exception cref="InvalidOperationException">Thrown when the file is not a valid WAV file or contains invalid format data.</exception>
        public static AudioFormat ReadWaveHeader(string filePath)
        {
            using (var reader = new BinaryReader(File.OpenRead(filePath)))
            {
                return ReadWaveHeader(reader);
            }
        }

        /// <summary>
        /// Reads and validates the WAV file header from a binary reader.
        /// </summary>
        /// <param name="reader">The binary reader positioned at the start of a WAV file.</param>
        /// <returns>An <see cref="AudioFormat"/> object containing the audio format information.</returns>
        /// <exception cref="ArgumentNullException">Thrown when reader is null.</exception>
        /// <exception cref="InvalidOperationException">Thrown when the WAV header is invalid or contains unsupported format data.</exception>
        /// <remarks>
        /// This method reads the WAV file header according to the standard WAV format specification.
        /// The reader must be positioned at the beginning of the WAV file.
        /// Based on the implementation from the Azure Cognitive Services Speech SDK samples.
        /// </remarks>
        public static AudioFormat ReadWaveHeader(BinaryReader reader)
        {
            if (reader == null)
                throw new ArgumentNullException(nameof(reader));

            try
            {
                // Read and validate "RIFF" tag
                var tag = new string(reader.ReadChars(4));
                if (tag != RiffTag)
                    throw new InvalidOperationException($"Invalid WAV header. Expected '{RiffTag}' tag but found '{tag}'");

                // Read file size (not used but must be read to advance position)
                _ = reader.ReadInt32();

                // Read and validate "WAVE" tag
                tag = new string(reader.ReadChars(4));
                if (tag != WaveTag)
                    throw new InvalidOperationException($"Invalid WAV header. Expected '{WaveTag}' tag but found '{tag}'");

                // Read and validate "fmt " tag
                tag = new string(reader.ReadChars(4));
                if (tag != FmtTag)
                    throw new InvalidOperationException($"Invalid WAV header. Expected '{FmtTag}' tag but found '{tag}'");

                // Read format chunk
                var formatSize = reader.ReadInt32();
                if (formatSize < MinFormatChunkSize)
                    throw new InvalidOperationException($"Invalid format chunk size: {formatSize}");

                var formatTag = reader.ReadUInt16();
                var channels = reader.ReadUInt16();
                var samplesPerSecond = reader.ReadInt32();
                var avgBytesPerSec = reader.ReadUInt32();
                var blockAlign = reader.ReadUInt16();
                var bitsPerSample = reader.ReadUInt16();

                ValidateAudioFormat(channels, samplesPerSecond, avgBytesPerSec, bitsPerSample, blockAlign);

                // Skip any extra format bytes
                if (formatSize > MinFormatChunkSize)
                    reader.ReadBytes(formatSize - MinFormatChunkSize);

                // Handle optional LIST chunk
                tag = new string(reader.ReadChars(4));
                if (tag == ListTag)
                {
                    var listChunkSize = reader.ReadUInt32();
                    reader.ReadBytes((int)listChunkSize);
                    tag = new string(reader.ReadChars(4));
                }

                // Validate "data" tag
                if (tag != DataTag)
                    throw new InvalidOperationException($"Invalid WAV header. Expected '{DataTag}' tag but found '{tag}'");

                // Read data chunk size (not used but must be read to advance position)
                _ = reader.ReadInt32();

                return new AudioFormat
                {
                    BitsPerSample = bitsPerSample,
                    SamplesPerSecond = samplesPerSecond,
                    ChannelCount = channels
                };
            }
            catch (EndOfStreamException)
            {
                throw new InvalidOperationException("Unexpected end of file while reading WAV header");
            }
        }

        /// <summary>
        /// Validates the audio format parameters for consistency and supported values.
        /// </summary>
        /// <exception cref="InvalidOperationException">Thrown when any parameter is invalid or unsupported.</exception>
        private static void ValidateAudioFormat(ushort channels, int samplesPerSecond, uint avgBytesPerSec, ushort bitsPerSample, ushort blockAlign)
        {
            if (channels == 0 || channels > 8)
                throw new InvalidOperationException($"Unsupported number of channels: {channels}");

            if (samplesPerSecond <= 0)
                throw new InvalidOperationException($"Invalid samples per second: {samplesPerSecond}");

            if (bitsPerSample != 8 && bitsPerSample != 16 && bitsPerSample != 24 && bitsPerSample != 32)
                throw new InvalidOperationException($"Unsupported bits per sample: {bitsPerSample}");

            var expectedBlockAlign = channels * (bitsPerSample / 8);
            if (blockAlign != expectedBlockAlign)
                throw new InvalidOperationException($"Invalid block align. Expected {expectedBlockAlign} but got {blockAlign}");

            var expectedAvgBytesPerSec = samplesPerSecond * blockAlign;
            if (avgBytesPerSec != expectedAvgBytesPerSec)
                throw new InvalidOperationException($"Invalid average bytes per second. Expected {expectedAvgBytesPerSec} but got {avgBytesPerSec}");
        }
    }
}
