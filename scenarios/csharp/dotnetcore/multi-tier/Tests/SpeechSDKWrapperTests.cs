//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using Azure.AI.Test.Common.Servers;
using Azure.AI.Test.Common.Servers.USP;
using Microsoft.CognitiveServices.Speech;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using SpeechSDKSamples.Remote.Client.App;
using SpeechSDKSamples.Remote.Core;
using SpeechSDKSamples.Remote.Service.Core;
using System;
using System.IO;
using System.Threading;
using System.Threading.Tasks;

namespace SpeechSDKSamples.Remote.Service.Tests
{
    internal class ResponseTransport : IResponseTransport
    {
        public Task AcknowledgeAudioAsync(TimeSpan offset)
        {
            AcknowledgeAudioCompleted?.Invoke(this, offset);
            return Task.CompletedTask;
        }

        public event EventHandler<TimeSpan> AcknowledgeAudioCompleted;


        public Task StopRecognitionAsync(RecognitionStoppedReason reason, int errorCode, string errorMessage)
        {
            StoppedInfo stoppedInfo = new StoppedInfo()
            {
                RecognitionStoppedReason = reason,
                ErrorCode = errorCode,
                ErrorMessage = errorMessage
            };

            StopRecognitionCompleted?.Invoke(this, stoppedInfo);

            return Task.CompletedTask;
        }

        public event EventHandler<StoppedInfo> StopRecognitionCompleted;
    }

    internal struct StoppedInfo
    {
        public RecognitionStoppedReason RecognitionStoppedReason;
        public int ErrorCode;
        public string ErrorMessage;
    }

    [TestClass]
    public class SpeechSDKWrapperTests : TimeOutTestBase
    {
        private MockSpeechServer _mockSpeechServer;
        private SpeechSDKWrapper _wrapper;
        private SpeechConfig _speechConfig;
        private AudioFormat _audioFormat = new AudioFormat()
        {
            BitsPerSample = 16,
            ChannelCount = 1,
            SamplesPerSecond = 160
        };

        private TimeSpan _timePerSample = TimeSpan.FromSeconds(0.1);
        private int _bytesPerSample = 32;

        private ResponseTransport _responseTransport = new();

        [TestInitialize]
        public void TestInitialize()
        {
            _mockSpeechServer = MockSpeechServer.Start();

            _speechConfig = SpeechConfig.FromEndpoint(_mockSpeechServer.EndpointUri);
            _timePerSample = TimeSpan.FromSeconds((double)_bytesPerSample / _audioFormat.AverageBytesPerSecond);
            _wrapper = new SpeechSDKWrapper(_speechConfig, _audioFormat, _responseTransport, null);
        }

        [ClassInitialize]
        public static void ClassInitialize(TestContext context)
        {
            Microsoft.CognitiveServices.Speech.Diagnostics.Logging.FileLogger.Start("log.log");
        }

        [ClassCleanup]
        public static void ClassCleanup()
        {
            Microsoft.CognitiveServices.Speech.Diagnostics.Logging.FileLogger.Stop();
        }

        [TestMethod]
        public async Task ConfigureAudio()
        {
            var messageTCS = new TaskCompletionSource<Message>(TaskCreationOptions.RunContinuationsAsynchronously);

            _mockSpeechServer.MessageReceived += ( message) =>
            {
                if (!message.IsBinary || messageTCS.Task.IsCompleted)
                {
                    return Task.CompletedTask;
                }

                messageTCS.SetResult(message);
                return Task.CompletedTask;
            };

            await _wrapper.StartRecognitionAsync().ConfigureAwait(false);

            // The SDK also needs an audio message to "start" fully.
            var audioFrame = new byte[32];
            _wrapper.OnAudioReceived(audioFrame);

            // First binary message sent should be the wav header.
            await TimeoutTestAction(messageTCS.Task, "Audio message was not received in the timeout period").ConfigureAwait(false);

            var headerMessage = await messageTCS.Task.ConfigureAwait(false) as BinaryMessage;

            GC.KeepAlive(_wrapper);

            Assert.IsNotNull(headerMessage);
            Assert.IsTrue(headerMessage.IsBinary);
            Assert.AreEqual(44, headerMessage.Data.Length);

            var dataReader = new BinaryReader(new MemoryStream(headerMessage.Data));

            var format = WavFileUtilities.ReadWaveHeader(dataReader);

            Assert.AreEqual(16, format.BitsPerSample);
            Assert.AreEqual(160, format.SamplesPerSecond);
            Assert.AreEqual(1, format.ChannelCount);
            Assert.AreEqual(320, format.AverageBytesPerSecond);
        }

        [TestMethod]
        public async Task WrapperEventsFire()
        {
            var messageTCS = new TaskCompletionSource<Message>(TaskCreationOptions.RunContinuationsAsynchronously);

            int i = 0;
            var timespanPerMessage = TimeSpan.FromSeconds(0.1);

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
                        await _mockSpeechServer.SendRecognizingMessageAsync("recognizing", TimeSpan.Zero, timespanPerMessage, TimeoutToken).ConfigureAwait(false);
                        break;
                    case 4:
                        i++;
                        await _mockSpeechServer.SendRecognizedMessageAsync("recognized", TimeSpan.Zero, timespanPerMessage * 2, TimeoutToken).ConfigureAwait(false);
                        break;
                    case 5:
                        i++;

                        await _mockSpeechServer.SendDictationEndMessageAsync(timespanPerMessage * 5, TimeoutToken).ConfigureAwait(false);
                        await _mockSpeechServer.SendSpeechEndMessageAsync(timespanPerMessage * 5, TimeoutToken).ConfigureAwait(false);
                        await _mockSpeechServer.SendTurnEndMessageAsync(TimeoutToken).ConfigureAwait(false);
                        break;
                }
            };

            var sessionStartedTCS = new TaskCompletionSource<bool>(TaskCreationOptions.RunContinuationsAsynchronously);
            var recognizingTCS = new TaskCompletionSource<bool>(TaskCreationOptions.RunContinuationsAsynchronously);
            var recognizedTCS = new TaskCompletionSource<bool>(TaskCreationOptions.RunContinuationsAsynchronously);
            var sessionStoppedTCS = new TaskCompletionSource<bool>(TaskCreationOptions.RunContinuationsAsynchronously);
            var sessioncanceledTCS = new TaskCompletionSource<bool>(TaskCreationOptions.RunContinuationsAsynchronously);
            var speechStartDetectedTCS = new TaskCompletionSource<bool>(TaskCreationOptions.RunContinuationsAsynchronously);
            var speechEndDetectedTCS = new TaskCompletionSource<bool>(TaskCreationOptions.RunContinuationsAsynchronously);

            int sessionStartedCount = 0;
            int recognizingCount = 0;
            int recognizedCount = 0;
            int sessionStopped = 0;

            _wrapper.SDKSessionStarted += (sender, e) =>
            {
                sessionStartedCount++;
                sessionStartedTCS.SetResult(true);
            };

            _wrapper.SDKRecognizing += (sender, e) =>
            {
                try
                {
                    Assert.AreEqual("recognizing", e.Result.Text);
                    Assert.AreEqual(TimeSpan.Zero, TimeSpan.FromTicks(e.Result.OffsetInTicks));
                    Assert.AreEqual(TimeSpan.FromSeconds(0.1), e.Result.Duration);
                    recognizingCount++;
                    recognizingTCS.SetResult(true);
                }
                catch (Exception ex)
                {
                    recognizingTCS.SetException(ex);
                }
            };

            _wrapper.SDKRecognized += (sender, e) =>
            {
                try
                {
                    Assert.AreEqual("recognized", e.Result.Text);
                    Assert.AreEqual(TimeSpan.Zero, TimeSpan.FromTicks(e.Result.OffsetInTicks));
                    Assert.AreEqual(TimeSpan.FromSeconds(0.2), e.Result.Duration);
                    recognizedCount++;
                    recognizedTCS.SetResult(true);
                }
                catch (Exception ex)
                {
                    recognizedTCS.SetException(ex);
                }
            };

            _wrapper.SDKSessionStopped += (sender, e) =>
            {
                sessionStopped++;
                sessionStoppedTCS.SetResult(true);
            };

            _wrapper.SDKCanceled += (sender, e) =>
            {
                sessioncanceledTCS.SetResult(true);
            };

            _wrapper.SDKSpeechStartDetected += (sender, e) =>
            {
                speechStartDetectedTCS.SetResult(true);
            };

            _wrapper.SDKSpeechEndDetected += (sender, e) =>
            {
                speechEndDetectedTCS.SetResult(true);
            };

            // The SDK also needs an audio message to "start" fully.
            var audioFrame = new byte[32];
            _wrapper.OnAudioReceived(audioFrame);

            await _wrapper.StartRecognitionAsync();

            await TimeoutTestAction(sessionStartedTCS).ConfigureAwait(false);

            _wrapper.OnAudioReceived(audioFrame);

            await TimeoutTestAction(speechStartDetectedTCS).ConfigureAwait(false);

            _wrapper.OnAudioReceived(audioFrame);

            await TimeoutTestAction(recognizingTCS).ConfigureAwait(false);

            await recognizingTCS.Task;

            _wrapper.OnAudioReceived(audioFrame);

            await TimeoutTestAction(recognizedTCS).ConfigureAwait(false);

            var endOfaudioFrame = new byte[0];
            _wrapper.OnAudioReceived(endOfaudioFrame);

            var endTasks = Task.WhenAll(sessionStoppedTCS.Task, sessioncanceledTCS.Task, speechEndDetectedTCS.Task);
            await TimeoutTestAction(endTasks).ConfigureAwait(false);
        }

        [TestMethod]
        public async Task AckaudioFrameIsSent()
        {
            var messageTCS = new TaskCompletionSource<TimeSpan>(TaskCreationOptions.RunContinuationsAsynchronously);

            int i = 0;

            _mockSpeechServer.MessageReceived += async (message) =>
            {
                if (!message.IsBinary)
                {
                    return;
                }

                switch (i)
                {
                    case 0:
                        i++;
                        break;
                    case 1:
                        await _mockSpeechServer.SendTurnStartMessageAsync(TimeoutToken).ConfigureAwait(false);
                        await _mockSpeechServer.SendSpeechStartMessageAsync(TimeSpan.Zero, TimeoutToken).ConfigureAwait(false);
                        await _mockSpeechServer.SendRecognizedMessageAsync("recognized", TimeSpan.Zero, _timePerSample, TimeoutToken).ConfigureAwait(false);
                        break;
                }
            };

            _responseTransport.AcknowledgeAudioCompleted += (s, e) =>
            {
                messageTCS.SetResult(e);
            };

            // The SDK also needs an audio message to "start" fully.
            var audioFrame = new byte[_bytesPerSample];
            _wrapper.OnAudioReceived(audioFrame);

            await _wrapper.StartRecognitionAsync();

            // First binary message sent should be the wav header.
            await TimeoutTestAction(messageTCS, "Audio message was not received in the timeout period").ConfigureAwait(false);

            var audioFrameAck = await messageTCS.Task.ConfigureAwait(false);
            Assert.IsNotNull(audioFrameAck);
            Assert.AreEqual(_timePerSample, audioFrameAck);
        }

        [TestMethod]
        public async Task SDKErrorPropogates()
        {
            var messageTCS = new TaskCompletionSource<StoppedInfo>(TaskCreationOptions.RunContinuationsAsynchronously);

            int i = 0;

            _mockSpeechServer.MessageReceived += async (message) =>
            {
                if (!message.IsBinary)
                {
                    return;
                }

                switch (i)
                {
                    case 0:
                        i++;
                        break;
                    case 1:
                        await _mockSpeechServer.CloseWebSocketAsync(System.Net.WebSockets.WebSocketCloseStatus.ProtocolError); // Fatal error.
                        break;
                }
            };

            _responseTransport.StopRecognitionCompleted += (sender, stoppedInfo) =>
            {
                messageTCS.SetResult(stoppedInfo);
            };

            // The SDK also needs an audio message to "start" fully.
            var audioFrame = new byte[_bytesPerSample];
            _wrapper.OnAudioReceived(audioFrame);

            await _wrapper.StartRecognitionAsync();

            // First binary message sent should be the wav header.
            await TimeoutTestAction(messageTCS, "Audio message was not received in the timeout period").ConfigureAwait(false);

            var stoppedMessage = await messageTCS.Task.ConfigureAwait(false);
            Assert.IsNotNull(stoppedMessage);
            Assert.AreEqual(stoppedMessage.RecognitionStoppedReason, RecognitionStoppedReason.Error);
        }

        [TestMethod]
        public async Task SDKEOSPropogates()
        {
            var messageTCS = new TaskCompletionSource<StoppedInfo>(TaskCreationOptions.RunContinuationsAsynchronously);

            int i = 0;
            var timespanPerMessage = TimeSpan.FromSeconds(0.1);

            _mockSpeechServer.MessageReceived += async (message) =>
            {
                if (!message.IsBinary)
                {
                    return;
                }

                switch (i)
                {
                    case 0:
                    case 1:
                        i++;
                        break;
                    case 2:
                        i++;
                        await _mockSpeechServer.SendTurnStartMessageAsync(TimeoutToken).ConfigureAwait(false);
                        await _mockSpeechServer.SendSpeechStartMessageAsync(TimeSpan.Zero, TimeoutToken).ConfigureAwait(false);
                        break;
                    case 3:
                        i++;
                        await _mockSpeechServer.SendRecognizingMessageAsync("recognizing", TimeSpan.Zero, timespanPerMessage * 2, TimeoutToken).ConfigureAwait(false);
                        break;
                    case 4:
                        i++;
                        await _mockSpeechServer.SendRecognizedMessageAsync("recognized", TimeSpan.Zero, timespanPerMessage * 3, TimeoutToken).ConfigureAwait(false);
                        break;
                    case 5:
                        i++;

                        await _mockSpeechServer.SendDictationEndMessageAsync(timespanPerMessage * 4, TimeoutToken).ConfigureAwait(false);
                        await _mockSpeechServer.SendSpeechEndMessageAsync(timespanPerMessage * 4, TimeoutToken).ConfigureAwait(false);
                        await _mockSpeechServer.SendTurnEndMessageAsync(TimeoutToken).ConfigureAwait(false);
                        break;
                }
            };

            _responseTransport.StopRecognitionCompleted += (sender, e) =>
                {
                    messageTCS.SetResult(e);
                };

            await _wrapper.StartRecognitionAsync();

            var audioFrame = new byte[32];

            _wrapper.OnAudioReceived(audioFrame);
            _wrapper.OnAudioReceived(audioFrame);
            _wrapper.OnAudioReceived(audioFrame);
            _wrapper.OnAudioReceived(audioFrame);

            var endOfaudioFrame = new byte[0];
            _wrapper.OnAudioReceived(endOfaudioFrame);

            // First binary message sent should be the wav header.
            await TimeoutTestAction(messageTCS.Task, "EOS message was not received in the timeout period").ConfigureAwait(false);

            var stoppedMessage = await messageTCS.Task.ConfigureAwait(false);
            Assert.IsNotNull(stoppedMessage);
            Assert.AreEqual(stoppedMessage.RecognitionStoppedReason, RecognitionStoppedReason.EndOfStream);
        }

        [TestMethod]
        public async Task SDKStopPropogates()
        {
            var messageTCS = new TaskCompletionSource<StoppedInfo>(TaskCreationOptions.RunContinuationsAsynchronously);
            var speechStartedTCS = new TaskCompletionSource(TaskCreationOptions.RunContinuationsAsynchronously);

            int i = 0;
            bool stop = false;

            var timespanPerMessage = TimeSpan.FromSeconds(0.1);

            _mockSpeechServer.MessageReceived += async (message) =>
            {
                if (!message.IsBinary)
                {
                    return;
                }

                if (i++ == 1)
                {
                    await _mockSpeechServer.SendTurnStartMessageAsync(TimeoutToken).ConfigureAwait(false);
                    await _mockSpeechServer.SendSpeechStartMessageAsync(TimeSpan.Zero, TimeoutToken).ConfigureAwait(false);
                }

                if (true == stop)
                {
                    await _mockSpeechServer.SendRecognizedMessageAsync("recognized", TimeSpan.Zero, timespanPerMessage * (i - 2), TimeoutToken).ConfigureAwait(false);
                    await _mockSpeechServer.SendDictationEndMessageAsync(timespanPerMessage * (i - 1), TimeoutToken).ConfigureAwait(false);
                    await _mockSpeechServer.SendSpeechEndMessageAsync(timespanPerMessage * (i - 1), TimeoutToken).ConfigureAwait(false);
                    await _mockSpeechServer.SendTurnEndMessageAsync(TimeoutToken).ConfigureAwait(false);
                }
            };

            _responseTransport.StopRecognitionCompleted += (sender, e) =>
            {
                messageTCS.SetResult(e);
            };

            // The SDK also needs an audio message to "start" fully.
            var audioFrame = new byte[32];
            _wrapper.OnAudioReceived(audioFrame);

            _wrapper.SDKSpeechStartDetected += (s, e) =>
            {
                speechStartedTCS.SetResult();
            };

            await _wrapper.StartRecognitionAsync();

            var messageCancellation = new CancellationTokenSource();

            var messageTask = Task.Run(() =>
            {
                while (!messageCancellation.IsCancellationRequested)
                {
                    _wrapper.OnAudioReceived(audioFrame);
                    Thread.CurrentThread.Join(timespanPerMessage);
                }
            });

            await TimeoutTestAction(speechStartedTCS, "Speech Started event was not received in the timeout period").ConfigureAwait(false); ;

            var stopTask = _wrapper.StopRecognition();
            stop = true;

            await TimeoutTestAction(messageTCS, "No stop message").ConfigureAwait(false);

            var stoppedMessage = await messageTCS.Task.ConfigureAwait(false);
            Assert.IsNotNull(stoppedMessage);
            Assert.AreEqual(RecognitionStoppedReason.StopRequested, stoppedMessage.RecognitionStoppedReason);

            await TimeoutTestAction(stopTask, "Wrapper did not stop").ConfigureAwait(false);

            messageCancellation.Cancel();

            await TimeoutTestAction(messageTask, "audioSend did not stop").ConfigureAwait(false);

        }

    }
}
