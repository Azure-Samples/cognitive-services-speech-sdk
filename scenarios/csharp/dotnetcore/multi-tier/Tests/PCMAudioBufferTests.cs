//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using Microsoft.VisualStudio.TestTools.UnitTesting;
using SpeechSDKSamples.Remote.Client.Core;
using SpeechSDKSamples.Remote.Core;
using System;

namespace SpeechSDKSamples.RemoteClient.Core.Tests
{
    [TestClass]
    public class PcmAudioBufferTests
    {
        private AudioFormat CreateAudioStreamFormat(int bitsPerSample, int samplesPerSecond, int channelCount)
        {
            return new AudioFormat
                            {
                BitsPerSample = bitsPerSample,
                SamplesPerSecond = samplesPerSecond,
                ChannelCount = channelCount
            };
        }

        private DataChunk CreateDataChunk(byte[] data, DateTime receivedTime)
        {
            return new DataChunk(data, receivedTime);
        }

        [TestMethod]
        [ExpectedException(typeof(ArgumentException))]
        public void Constructor_InvalidBitsPerSample_ThrowsException()
        {
            var format = CreateAudioStreamFormat(7, 16000, 2);
            var buffer = new PCMAudioBuffer(format);
        }

        [TestMethod]
        public void Add_AddsDataChunkToBuffer()
        {
            var format = CreateAudioStreamFormat(16, 16000, 2);
            var buffer = new PCMAudioBuffer(format);
            var chunk = CreateDataChunk(new byte[] { 1, 2, 3, 4 }, DateTime.Now);

            buffer.Add(chunk);

            var addedChunk = buffer.GetNext();
            Assert.AreEqual(chunk, addedChunk);
        }

        [TestMethod]
        public void GetNext_ReturnsNextChunkInOrder()
        {
            var format = CreateAudioStreamFormat(16, 16000, 2);
            var buffer = new PCMAudioBuffer(format);
            var chunk1 = CreateDataChunk(new byte[] { 1, 2, 3, 4 }, DateTime.Now);
            var chunk2 = CreateDataChunk(new byte[] { 5, 6, 7, 8 }, DateTime.Now);

            buffer.Add(chunk1);
            buffer.Add(chunk2);

            var nextChunk1 = buffer.GetNext();
            var nextChunk2 = buffer.GetNext();

            Assert.AreEqual(chunk1, nextChunk1);
            Assert.AreEqual(chunk2, nextChunk2);
        }

        [TestMethod]
        public void NewTurn_ResetsCurrentChunkPointer()
        {
            var format = CreateAudioStreamFormat(16, 16000, 2);
            var buffer = new PCMAudioBuffer(format);
            var chunk1 = CreateDataChunk(new byte[] { 1, 2, 3, 4 }, DateTime.Now);
            var chunk2 = CreateDataChunk(new byte[] { 5, 6, 7, 8 }, DateTime.Now);

            buffer.Add(chunk1);
            buffer.Add(chunk2);

            buffer.GetNext();
            buffer.NewTurn();

            var nextChunk = buffer.GetNext();
            Assert.AreEqual(chunk1, nextChunk);
        }

        [TestMethod]
        public void DiscardBytes_RemovesOldestBytes()
        {
            var format = CreateAudioStreamFormat(16, 16000, 2);
            var buffer = new PCMAudioBuffer(format);
            var chunk1 = CreateDataChunk(new byte[] { 1, 2, 3, 4 }, DateTime.Now);
            var chunk2 = CreateDataChunk(new byte[] { 5, 6, 7, 8 }, DateTime.Now);

            buffer.Add(chunk1);
            buffer.Add(chunk2);

            buffer.DiscardBytes(4);

            var nextChunk = buffer.GetNext();
            CollectionAssert.AreEqual(new byte[] { 5, 6, 7, 8 }, nextChunk.Data.ToArray());
        }

        [TestMethod]
        public void DiscardTill_RemovesBytesTillOffset()
        {
            var format = CreateAudioStreamFormat(16, 16000, 2);
            var buffer = new PCMAudioBuffer(format);
            var chunk1 = CreateDataChunk(new byte[] { 1, 2, 3, 4 }, DateTime.Now);
            var chunk2 = CreateDataChunk(new byte[] { 5, 6, 7, 8 }, DateTime.Now);

            buffer.Add(chunk1);
            buffer.Add(chunk2);

            var offsetInTicks = buffer.ToAbsolute(TimeSpan.FromTicks(156)); // 1 byte in ticks  
            buffer.DiscardTill(offsetInTicks);

            var nextChunk = buffer.GetNext();
            CollectionAssert.AreEqual(new byte[] { 2, 3, 4 }, nextChunk.Data.ToArray());
            nextChunk = buffer.GetNext();
            CollectionAssert.AreEqual(new byte[] { 5, 6, 7, 8 }, nextChunk.Data.ToArray());

        }

        [TestMethod]
        public void ToAbsolute_ConvertsToAbsoluteTicks()
        {
            var format = CreateAudioStreamFormat(16, 16000, 2);
            var buffer = new PCMAudioBuffer(format);
            var chunk = CreateDataChunk(new byte[] { 1, 2, 3, 4 }, DateTime.Now);

            buffer.Add(chunk);
            buffer.NewTurn();

            var absoluteOffset = buffer.ToAbsolute(TimeSpan.FromTicks(10000)); // 1 millisecond in ticks  
            Assert.AreEqual(TimeSpan.FromTicks(10000), absoluteOffset);
        }
    }
}
