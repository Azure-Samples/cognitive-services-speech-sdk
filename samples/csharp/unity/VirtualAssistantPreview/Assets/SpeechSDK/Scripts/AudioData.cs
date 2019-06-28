//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.IO;
using System.Text;
using UnityEngine;

internal class WaveAudioData
{
    private const UInt32 fmtId = 0x20746d66;
    private const UInt32 dataId = 0x61746164;

    /// <summary>
    /// Creates WaveAudioData class
    /// </summary>
    /// <param name="data">the audio data</param>
    /// <remarks>This is a helper class to parse the audio data with wave header</remarks>
    public WaveAudioData(byte[] data)
    {
        Debug.Log($"Creating wav for playback");
        var nextChunkStart = 12;

        while (nextChunkStart < data.Length)
        {
            var chunkId = BitConverter.ToUInt32(data, nextChunkStart);
            var chunkSize = BitConverter.ToUInt32(data, nextChunkStart + 4);
            var chunkDataOffset = nextChunkStart + 8;
            switch (chunkId)
            {
                case fmtId:
                    break;
                case dataId:
                    AudioSamples = Math.Min(data.Length - chunkDataOffset, Convert.ToInt32(chunkSize)) / 2;
                    AudioData = new float[AudioSamples];
                    for (var i = 0; i < AudioSamples; i++)
                    {
                        var sample = BitConverter.ToInt16(data, chunkDataOffset + 2 * i);
                        AudioData[i] = sample / 32768.0f;
                    }
                    Debug.Log($"Data chunk size: {chunkSize}, Actual data size {AudioSamples * 2}");
                    break;
                default:
                    break;
            }

            nextChunkStart = chunkDataOffset + Convert.ToInt32(chunkSize);
        }
    }

    // Helper method for crafting the WAV header
    // totalSampleCount needs to be the combined count of samples of all channels. So if the left and right channels contain 1000 samples each, then totalSampleCount should be 2000.
    // isFloatingPoint should only be true if the audio data is in 32-bit floating-point format.
    public static void WriteWavHeader(MemoryStream stream, bool isFloatingPoint, ushort channelCount, ushort bitDepth, int sampleRate, int totalSampleCount)
    {
        stream.Position = 0;

        // RIFF header.
        // Chunk ID.
        stream.Write(Encoding.ASCII.GetBytes("RIFF"), 0, 4);

        // Chunk size.
        stream.Write(BitConverter.GetBytes(((bitDepth / 8) * totalSampleCount) + 36), 0, 4);

        // Format.
        stream.Write(Encoding.ASCII.GetBytes("WAVE"), 0, 4);

        // Sub-chunk 1.
        // Sub-chunk 1 ID.
        stream.Write(Encoding.ASCII.GetBytes("fmt "), 0, 4);

        // Sub-chunk 1 size.
        stream.Write(BitConverter.GetBytes(16), 0, 4);

        // Audio format (floating point (3) or PCM (1)). Any other format indicates compression.
        stream.Write(BitConverter.GetBytes((ushort)(isFloatingPoint ? 3 : 1)), 0, 2);

        // Channels.
        stream.Write(BitConverter.GetBytes(channelCount), 0, 2);

        // Sample rate.
        stream.Write(BitConverter.GetBytes(sampleRate), 0, 4);

        // Bytes rate.
        stream.Write(BitConverter.GetBytes(sampleRate * channelCount * (bitDepth / 8)), 0, 4);

        // Block align.
        stream.Write(BitConverter.GetBytes((ushort)channelCount * (bitDepth / 8)), 0, 2);

        // Bits per sample.
        stream.Write(BitConverter.GetBytes(bitDepth), 0, 2);

        // Sub-chunk 2.
        // Sub-chunk 2 ID.
        stream.Write(Encoding.ASCII.GetBytes("data"), 0, 4);

        // Sub-chunk 2 size.
        stream.Write(BitConverter.GetBytes((bitDepth / 8) * totalSampleCount), 0, 4);
    }

    public int AudioSamples { get; private set; }
    public float[] AudioData { get; private set; }
}