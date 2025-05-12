//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using Azure.AI.Test.Common.Servers;
using Azure.AI.Test.Common.Servers.USP;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using SpeechSDKSamples.Remote.Client.Core;
using SpeechSDKSamples.Remote.Core;
using SpeechSDKSamples.Remote.Service;
using SpeechSDKSamples.Remote.Service.Tests;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.WebSockets;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace SpeechSDKSamples.RemoteClient.Tests
{
    [TestClass]
    public abstract class EndToEndBase<MessageType> : TimeOutTestBase
    {
        protected MockSpeechServer _mockSpeechServer;

        [TestInitialize]
        public virtual async Task TestInitialize()
        {
            _mockSpeechServer = MockSpeechServer.Start();

            await ServiceInitialize();
        }

        [TestCleanup]
        public virtual void TestCleanup()
        {
            _mockSpeechServer?.Dispose();
        }

        protected abstract Task ServiceInitialize();

        protected abstract IStreamTransport<MessageType> CreateClientTransport();

        private AudioFormat _audioFormat = new AudioFormat()
        {
            BitsPerSample = 16,
            ChannelCount = 1,
            SamplesPerSecond = 160
        };

        private TimeSpan _timePerFrame = TimeSpan.FromSeconds(0.1);
        private int _bytesPerFrame = 32;

        [TestMethod]
        public async Task InferencesDeliveredToClient()
        {
            var recognitionStoppedTCS = new TaskCompletionSource(TaskCreationOptions.RunContinuationsAsynchronously);

            using SpeechSDKRemoteClient<MessageType> client = new SpeechSDKRemoteClient<MessageType>(CreateClientTransport);
            int i = 0;

            _mockSpeechServer.MessageReceived += async (message) =>
            {
                if (!message.IsBinary)
                {
                    return;
                }

                switch (i)
                {
                    case 0: // Ingore the WAV header
                    case 1: // Skip the first frame to "look" a frame behind and introduce a (minor) delay.
                        i++;
                        break;
                    case 2:
                        i++;
                        await _mockSpeechServer.SendTurnStartMessageAsync(TimeoutToken).ConfigureAwait(false);
                        await _mockSpeechServer.SendSpeechStartMessageAsync(TimeSpan.Zero, TimeoutToken).ConfigureAwait(false);
                        break;
                    case 3:
                        i++;
                        await _mockSpeechServer.SendRecognizingMessageAsync("recognizing", TimeSpan.Zero, _timePerFrame, TimeoutToken).ConfigureAwait(false);
                        break;
                    case 4:
                        i++;
                        await _mockSpeechServer.SendRecognizedMessageAsync("recognized", TimeSpan.Zero, _timePerFrame * 2, TimeoutToken).ConfigureAwait(false);
                        break;
                    case 5:
                        i++;

                        await _mockSpeechServer.SendDictationEndMessageAsync(_timePerFrame * 5, TimeoutToken).ConfigureAwait(false);
                        await _mockSpeechServer.SendSpeechEndMessageAsync(_timePerFrame * 5, TimeoutToken).ConfigureAwait(false);
                        await _mockSpeechServer.SendTurnEndMessageAsync(TimeoutToken).ConfigureAwait(false);
                        break;
                }

                return;
            };

            bool gotRecognizing = false;
            bool gotRecognized = false;

            client.SetAudioFormat(_audioFormat);
            client.MessageReceived += (s, e) =>
            {
                var displayType = GetDisplayTypeForMessage(e.Message);

                if (displayType == DisplayType.Intermedaite)
                {
                    gotRecognizing = true;
                }
                else if (displayType == DisplayType.Final)
                {
                    gotRecognized = true;
                }

            };

            client.Stopped += (s, e) =>
            {
                recognitionStoppedTCS.SetResult();
            };

            for (int j = 0; j < 5; j++)
            {
                await client.SendAudioAsync(new byte[_bytesPerFrame], TimeoutToken).ConfigureAwait(false);
            }

            await client.SendAudioAsync(new byte[0]).ConfigureAwait(false);

            await TimeoutTestAction(recognitionStoppedTCS).ConfigureAwait(false);

            Assert.IsTrue(gotRecognizing);
            Assert.IsTrue(gotRecognized);
        }

        protected abstract DisplayType GetDisplayTypeForMessage(MessageType message);

        [TestMethod]
        public async Task ErrorDeliveredToClient()
        {
            var recognitionStoppedTCS = new TaskCompletionSource<RecognitionStoppedReason>(TaskCreationOptions.RunContinuationsAsynchronously);

            using SpeechSDKRemoteClient<MessageType> client = new SpeechSDKRemoteClient<MessageType>(CreateClientTransport);
            _mockSpeechServer.MessageReceived += async (message) =>
            {
                if (!message.IsBinary)
                {
                    return;
                }

                await _mockSpeechServer.CloseWebSocketAsync(System.Net.WebSockets.WebSocketCloseStatus.ProtocolError).ConfigureAwait(false);
            };

            client.Stopped += (s, e) =>
            {
                recognitionStoppedTCS.SetResult(e.Reason);
            };

            client.SetAudioFormat(_audioFormat);

            for (int j = 0; j < 5; j++)
            {
                await client.SendAudioAsync(new byte[_bytesPerFrame], TimeoutToken).ConfigureAwait(false);
            }

            await client.SendAudioAsync(new byte[0]).ConfigureAwait(false);

            await TimeoutTestAction(recognitionStoppedTCS).ConfigureAwait(false);
            Assert.AreEqual(RecognitionStoppedReason.Error, await recognitionStoppedTCS.Task);
        }

        [TestMethod]
        public async Task AudioReplayedOnError()
        {
            var recognitionStoppedTCS = new TaskCompletionSource<RecognitionStoppedReason>(TaskCreationOptions.RunContinuationsAsynchronously);

            SpeechSDKRemoteClient<MessageType> client = new SpeechSDKRemoteClient<MessageType>(CreateClientTransport);
            var receivedBytes = new List<byte[]>();

            int i = 0;
            const int iterations = 3; // arbitrary number of iterations for this test
            const int connectionAttemptCount = 4; // default number of connection attempts.
            const int expectedFrames = iterations * connectionAttemptCount;
            StringBuilder sb = new StringBuilder();

            _mockSpeechServer.MessageReceived += async (message) =>
            {
                if (!message.IsBinary)
                {
                    return;
                }

                var binaryMessage = (BinaryMessage)message;
                i++;

                if (i == 1 && binaryMessage.Data.Length == 44) // Wav header.
                {
                    return;
                }

                receivedBytes.Add(binaryMessage.Data);

                if (i == iterations + 1)
                {
                    i = 0;
                    await _mockSpeechServer.CloseWebSocketAsync(System.Net.WebSockets.WebSocketCloseStatus.ProtocolError).ConfigureAwait(false);
                }
            };

            client.SetAudioFormat(_audioFormat);

            client.Stopped += (s, e) =>
                  {
                      recognitionStoppedTCS.SetResult(e.Reason);
                  };

            for (int j = 0; j < iterations; j++)
            {
                var data = Enumerable.Repeat((byte)j, _bytesPerFrame).ToArray();
                await client.SendAudioAsync(data, TimeoutToken).ConfigureAwait(false);
            }

            await TimeoutTestAction(recognitionStoppedTCS).ConfigureAwait(false);
            Assert.AreEqual(RecognitionStoppedReason.Error, await recognitionStoppedTCS.Task);
            Assert.AreEqual(expectedFrames, receivedBytes.Count);

            for (int j = 0; j < expectedFrames; j++)
            {
                var expected = (byte)(j % iterations);
                var actual = receivedBytes[j];

                Assert.AreEqual(_bytesPerFrame, actual.Length);
                Assert.IsTrue(actual.All(b => b == expected));
            }
        }
    }
}
