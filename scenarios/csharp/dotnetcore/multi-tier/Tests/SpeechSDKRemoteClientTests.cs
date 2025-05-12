//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using Microsoft.VisualStudio.TestTools.UnitTesting;
using Moq;
using SpeechSDKSamples.Remote.Client.Core;
using SpeechSDKSamples.Remote.Core;
using SpeechSDKSamples.Remote.WebSocket.Shared;
using SpeechSDKSamples.Remote.Service.Tests;
using System;
using System.Threading;
using System.Threading.Tasks;

namespace SpeechSDKSamples.Remote.Client.Tests
{
    [TestClass]
    public class SpeechSDKRemoteClientTests : TimeOutTestBase
    {
        [TestMethod]
        public void AudioFormatSetByTransportDelayed()
        {
            var mock = new Mock<IStreamTransport<object>>();
            var testClient = new SpeechSDKRemoteClient<object>(() => mock.Object);
            var format = CreateAudioStreamFormat(16, 16000, 1);

            testClient.SetAudioFormat(format);
            mock.Verify(m => m.SetAudioFormat(format), Times.Never);
        }

        [TestMethod]
        public async Task AudioFormatSetOnAudioSend()
        {
            var mock = new Mock<IStreamTransport<object>>();
            var testClient = new SpeechSDKRemoteClient<object>(() => mock.Object);
            var format = CreateAudioStreamFormat(16, 16000, 1);

            testClient.SetAudioFormat(format);
            mock.Verify(m => m.SetAudioFormat(format), Times.Never);

            await testClient.SendAudioAsync(new byte[] { 1, 2, 3, 4 });
            mock.Verify(m => m.SetAudioFormat(format), Times.Once);
        }

        [TestMethod]
        public async Task AudioDataSent()
        {
            var sendAudioCalledTcs = new TaskCompletionSource<bool>();
            var cancellationSource = new CancellationTokenSource();
            var mock = new Mock<IStreamTransport<object>>();

            mock.Setup(mock => mock.SendAudioAsync(It.IsAny<ReadOnlyMemory<byte>>(), It.IsAny<CancellationToken>()))
                .Callback<ReadOnlyMemory<byte>, CancellationToken>((data, token) =>
                {
                    sendAudioCalledTcs.TrySetResult(true);
                })
                .Returns(Task.CompletedTask);

            var testClient = new SpeechSDKRemoteClient<object>(() => mock.Object);

            var format = CreateAudioStreamFormat(16, 16000, 1);
            testClient.SetAudioFormat(format);

            var b1 = new byte[] { 1, 2, 3, 4 };
            var b2 = new byte[] { 5, 6, 7, 8 };
            var b3 = new byte[] { 9, 10, 11, 12 };
            var b4 = new byte[] { 13, 14, 15, 16 };

            await testClient.SendAudioAsync(new ReadOnlyMemory<byte>(b1));
            await sendAudioCalledTcs.Task;
            sendAudioCalledTcs = new TaskCompletionSource<bool>();
            mock.Verify(m => m.SendAudioAsync(new ReadOnlyMemory<byte>(b1), It.IsAny<CancellationToken>()), Times.Once);

            await testClient.SendAudioAsync(new ReadOnlyMemory<byte>(b2));
            await sendAudioCalledTcs.Task;
            sendAudioCalledTcs = new TaskCompletionSource<bool>();
            mock.Verify(m => m.SendAudioAsync(new ReadOnlyMemory<byte>(b1), It.IsAny<CancellationToken>()), Times.Once);
            mock.Verify(m => m.SendAudioAsync(new ReadOnlyMemory<byte>(b2), It.IsAny<CancellationToken>()), Times.Once);

            await testClient.SendAudioAsync(new ReadOnlyMemory<byte>(b3));
            await sendAudioCalledTcs.Task;
            sendAudioCalledTcs = new TaskCompletionSource<bool>();
            mock.Verify(m => m.SendAudioAsync(new ReadOnlyMemory<byte>(b1), It.IsAny<CancellationToken>()), Times.Once);
            mock.Verify(m => m.SendAudioAsync(new ReadOnlyMemory<byte>(b2), It.IsAny<CancellationToken>()), Times.Once);
            mock.Verify(m => m.SendAudioAsync(new ReadOnlyMemory<byte>(b3), It.IsAny<CancellationToken>()), Times.Once);

            await testClient.SendAudioAsync(new ReadOnlyMemory<byte>(b4));
            await sendAudioCalledTcs.Task;
            sendAudioCalledTcs = new TaskCompletionSource<bool>();
            mock.Verify(m => m.SendAudioAsync(new ReadOnlyMemory<byte>(b1), It.IsAny<CancellationToken>()), Times.Once);
            mock.Verify(m => m.SendAudioAsync(new ReadOnlyMemory<byte>(b2), It.IsAny<CancellationToken>()), Times.Once);
            mock.Verify(m => m.SendAudioAsync(new ReadOnlyMemory<byte>(b3), It.IsAny<CancellationToken>()), Times.Once);
            mock.Verify(m => m.SendAudioAsync(new ReadOnlyMemory<byte>(b4), It.IsAny<CancellationToken>()), Times.Once);
        }

        [TestMethod]
        public async Task AudioDataResentOnError()
        {
            var sendAudioCalledTcs = new TaskCompletionSource<bool>(TaskCreationOptions.RunContinuationsAsynchronously);
            var mock = new Mock<IStreamTransport<object>>();

            int callbackCount = 0;
            mock.Setup(mock => mock.SendAudioAsync(It.IsAny<ReadOnlyMemory<byte>>(), It.IsAny<CancellationToken>()))
                .Callback<ReadOnlyMemory<byte>, CancellationToken>((data, token) =>
                {
                    if (++callbackCount == 4)
                    {
                        sendAudioCalledTcs.TrySetResult(true);
                    }
                })
                .Returns(Task.CompletedTask);

            var testClient = new SpeechSDKRemoteClient<object>(() => mock.Object);

            var format = CreateAudioStreamFormat(16, 16000, 1);
            testClient.SetAudioFormat(format);
            var b1 = new byte[] { 1, 2, 3, 4 };
            var b2 = new byte[] { 5, 6, 7, 8 };
            var b3 = new byte[] { 9, 10, 11, 12 };
            var b4 = new byte[] { 13, 14, 15, 16 };

            await testClient.SendAudioAsync(b1);
            await testClient.SendAudioAsync(b2);
            await testClient.SendAudioAsync(b3);
            await testClient.SendAudioAsync(b4);

            await TimeoutTestAction(sendAudioCalledTcs).ConfigureAwait(false);

            mock.Verify(m => m.SendAudioAsync(new ReadOnlyMemory<byte>(b1), It.IsAny<CancellationToken>()), Times.Once);
            mock.Verify(m => m.SendAudioAsync(new ReadOnlyMemory<byte>(b2), It.IsAny<CancellationToken>()), Times.Once);
            mock.Verify(m => m.SendAudioAsync(new ReadOnlyMemory<byte>(b3), It.IsAny<CancellationToken>()), Times.Once);
            mock.Verify(m => m.SendAudioAsync(new ReadOnlyMemory<byte>(b4), It.IsAny<CancellationToken>()), Times.Once);

            sendAudioCalledTcs = new TaskCompletionSource<bool>();
            callbackCount = 0;

            mock.Raise(m => m.Stopped += null, new StoppedEventArgs(RecognitionStoppedReason.Error, new Exception()));

            await TimeoutTestAction(sendAudioCalledTcs).ConfigureAwait(false);

            mock.Verify(m => m.SendAudioAsync(new ReadOnlyMemory<byte>(b1), It.IsAny<CancellationToken>()), Times.Exactly(2));
            mock.Verify(m => m.SendAudioAsync(new ReadOnlyMemory<byte>(b2), It.IsAny<CancellationToken>()), Times.Exactly(2));
            mock.Verify(m => m.SendAudioAsync(new ReadOnlyMemory<byte>(b3), It.IsAny<CancellationToken>()), Times.Exactly(2));
            mock.Verify(m => m.SendAudioAsync(new ReadOnlyMemory<byte>(b4), It.IsAny<CancellationToken>()), Times.Exactly(2));
        }

        [TestMethod]
        public async Task AudioDataResentOnSendException()
        {
            var sendAudioCalledTcs = new TaskCompletionSource<bool>(TaskCreationOptions.RunContinuationsAsynchronously);
            var mock = new Mock<IStreamTransport<object>>();

            int callbackCount = 0;
            bool exceptionThrown = false;
            mock.Setup(mock => mock.SendAudioAsync(It.IsAny<ReadOnlyMemory<byte>>(), It.IsAny<CancellationToken>()))
                .Callback<ReadOnlyMemory<byte>, CancellationToken>((data, token) =>
                {
                    callbackCount++;
                    if (!exceptionThrown && callbackCount == 3)
                    {
                        callbackCount = 0;
                        exceptionThrown = true;
                        throw new Exception("Send failed");
                    }
                    else if (exceptionThrown && callbackCount == 4)
                    {
                        sendAudioCalledTcs.TrySetResult(true);
                    }
                })
                .Returns(Task.CompletedTask);

            var testClient = new SpeechSDKRemoteClient<object>(() => mock.Object);

            var format = CreateAudioStreamFormat(16, 16000, 1);
            testClient.SetAudioFormat(format);

            var b1 = new byte[] { 1, 2, 3, 4 };
            var b2 = new byte[] { 5, 6, 7, 8 };
            var b3 = new byte[] { 9, 10, 11, 12 };
            var b4 = new byte[] { 13, 14, 15, 16 };

            await testClient.SendAudioAsync(b1);
            await testClient.SendAudioAsync(b2);
            await testClient.SendAudioAsync(b3);
            await testClient.SendAudioAsync(b4);

            await TimeoutTestAction(sendAudioCalledTcs).ConfigureAwait(false);

            mock.Verify(m => m.SendAudioAsync(new ReadOnlyMemory<byte>(b1), It.IsAny<CancellationToken>()), Times.Exactly(2));
            mock.Verify(m => m.SendAudioAsync(new ReadOnlyMemory<byte>(b2), It.IsAny<CancellationToken>()), Times.Exactly(2));
            mock.Verify(m => m.SendAudioAsync(new ReadOnlyMemory<byte>(b3), It.IsAny<CancellationToken>()), Times.Exactly(2));
            mock.Verify(m => m.SendAudioAsync(new ReadOnlyMemory<byte>(b4), It.IsAny<CancellationToken>()), Times.Exactly(1));
        }

        [TestMethod]
        public async Task AcknowledgedAudioNotResent()
        {
            var sendAudioCalledTcs = new TaskCompletionSource<bool>(TaskCreationOptions.RunContinuationsAsynchronously);
            var mock = new Mock<IStreamTransport<object>>();

            int callbackCount = 0;
            bool exceptionThrown = false;
            mock.Setup(mock => mock.SendAudioAsync(It.IsAny<ReadOnlyMemory<byte>>(), It.IsAny<CancellationToken>()))
                .Callback<ReadOnlyMemory<byte>, CancellationToken>((data, token) =>
                {
                    callbackCount++;
                    if (!exceptionThrown && callbackCount == 2)
                    {
                        var bytesPerSecond = 2 * 16000;
                        var timePerSample = TimeSpan.FromSeconds((double)1 / bytesPerSecond);

                        var eventArgs = new AudioAcknowledgedEventArgs(timePerSample * 4);
                        mock.Raise(m => m.AudioAcknowledged += null, eventArgs);
                    }
                    if (!exceptionThrown && callbackCount == 3)
                    {
                        callbackCount = 0;
                        exceptionThrown = true;
                        throw new Exception("Send failed");
                    }
                    else if (exceptionThrown && callbackCount == 3)
                    {
                        sendAudioCalledTcs.TrySetResult(true);
                    }
                })
                .Returns(Task.CompletedTask);

            var testClient = new SpeechSDKRemoteClient<object>(() => mock.Object);

            var format = CreateAudioStreamFormat(16, 16000, 1);
            testClient.SetAudioFormat(format);

            var b1 = new byte[] { 1, 2, 3, 4 };
            var b2 = new byte[] { 5, 6, 7, 8 };
            var b3 = new byte[] { 9, 10, 11, 12 };
            var b4 = new byte[] { 13, 14, 15, 16 };

            await testClient.SendAudioAsync(b1);
            await testClient.SendAudioAsync(b2);
            await testClient.SendAudioAsync(b3);
            await testClient.SendAudioAsync(b4);

            await TimeoutTestAction(sendAudioCalledTcs).ConfigureAwait(false);

            mock.Verify(m => m.SendAudioAsync(new ReadOnlyMemory<byte>(b1), It.IsAny<CancellationToken>()), Times.Exactly(1));
            mock.Verify(m => m.SendAudioAsync(new ReadOnlyMemory<byte>(b2), It.IsAny<CancellationToken>()), Times.Exactly(2));
            mock.Verify(m => m.SendAudioAsync(new ReadOnlyMemory<byte>(b3), It.IsAny<CancellationToken>()), Times.Exactly(2));
            mock.Verify(m => m.SendAudioAsync(new ReadOnlyMemory<byte>(b4), It.IsAny<CancellationToken>()), Times.Exactly(1));
        }

        [TestMethod]
        public async Task InitialConnectionErrorViaSend()
        {
            var sendAudioCalledTcs = new TaskCompletionSource(TaskCreationOptions.RunContinuationsAsynchronously);
            var mock = new Mock<IStreamTransport<object>>();

            mock.Setup(mock => mock.ConnectAsync(It.IsAny<CancellationToken>()))
                .Callback<CancellationToken>((token) =>
                {
                    throw new Exception("Connect failed");
                })
                .Returns(Task.CompletedTask);

            var testClient = new SpeechSDKRemoteClient<object>(() => mock.Object);

            var format = CreateAudioStreamFormat(16, 16000, 1);
            testClient.SetAudioFormat(format);

            var b1 = new byte[] { 1, 2, 3, 4 };

            try
            {
                var sendT = testClient.SendAudioAsync(b1);
                await TimeoutTestAction(sendT, "Audio Send did not complete.").ConfigureAwait(false);
                await sendT;
                Assert.Fail("Should have thrown");
            }
            catch (ConnectionFailedException)
            { }

            mock.Verify(m => m.ConnectAsync(It.IsAny<CancellationToken>()), Times.Exactly(4));

            // Try again....
            try
            {
                var sendT = testClient.SendAudioAsync(b1);
                await TimeoutTestAction(sendT, "Audio Send did not complete.").ConfigureAwait(false);
                await sendT;
                Assert.Fail("Should have thrown");
            }
            catch (ConnectionFailedException)
            { }

            mock.Verify(m => m.ConnectAsync(It.IsAny<CancellationToken>()), Times.Exactly(4));
        }

        [TestMethod]
        public async Task InitialConnectionErrorViaConnect()
        {
            var sendAudioCalledTcs = new TaskCompletionSource(TaskCreationOptions.RunContinuationsAsynchronously);
            var mock = new Mock<IStreamTransport<object>>();

            mock.Setup(mock => mock.ConnectAsync(It.IsAny<CancellationToken>()))
                .Callback<CancellationToken>((token) =>
                {
                    throw new Exception("Connect failed");
                })
                .Returns(Task.CompletedTask);

            var testClient = new SpeechSDKRemoteClient<object>(() => mock.Object);

            var format = CreateAudioStreamFormat(16, 16000, 1);
            testClient.SetAudioFormat(format);

            try
            {
                var conectT = testClient.ConnectAsync();
                await TimeoutTestAction(conectT, "connect did not complete.").ConfigureAwait(false);
                await conectT;
                Assert.Fail("Should have thrown");
            }
            catch (ConnectionFailedException)
            { }

            mock.Verify(m => m.ConnectAsync(It.IsAny<CancellationToken>()), Times.Exactly(4));

            // Try again....

            try
            {
                var conectT = testClient.ConnectAsync();
                await TimeoutTestAction(conectT, "connect did not complete.").ConfigureAwait(false);
                await conectT;
                Assert.Fail("Should have thrown");
            }
            catch (ConnectionFailedException)
            { }

            mock.Verify(m => m.ConnectAsync(It.IsAny<CancellationToken>()), Times.Exactly(8));
        }

        [TestMethod]
        public async Task ErrorOnReconnect()
        {
            var errorTCS = new TaskCompletionSource(TaskCreationOptions.RunContinuationsAsynchronously);
            var mock = new Mock<IStreamTransport<object>>();

            int conectionCount = 0;

            mock.Setup(mock => mock.ConnectAsync(It.IsAny<CancellationToken>()))
                .Callback<CancellationToken>((token) =>
                {
                    if (conectionCount++ >= 1)
                    {
                        throw new Exception("Connect failed");
                    }
                })
                .Returns(Task.CompletedTask);

            int callbackCount = 0;
            bool exceptionThrown = false;
            mock.Setup(mock => mock.SendAudioAsync(It.IsAny<ReadOnlyMemory<byte>>(), It.IsAny<CancellationToken>()))
                .Callback<ReadOnlyMemory<byte>, CancellationToken>((data, token) =>
                {
                    callbackCount++;
                    if (!exceptionThrown && callbackCount == 3)
                    {
                        callbackCount = 0;
                        exceptionThrown = true;
                        throw new Exception("Send failed");
                    }
                })
                .Returns(Task.CompletedTask);

            var testClient = new SpeechSDKRemoteClient<object>(() => mock.Object);

            testClient.Stopped += (s, e) =>
            {
                errorTCS.SetResult();
            };

            var format = CreateAudioStreamFormat(16, 16000, 1);
            testClient.SetAudioFormat(format);

            var b1 = new byte[] { 1, 2, 3, 4 };
            var b2 = new byte[] { 5, 6, 7, 8 };
            var b3 = new byte[] { 9, 10, 11, 12 };
            var b4 = new byte[] { 13, 14, 15, 16 };

            await testClient.SendAudioAsync(b1);
            await testClient.SendAudioAsync(b2);
            await testClient.SendAudioAsync(b3);

            await TimeoutTestAction(errorTCS);

            try
            {
                await testClient.SendAudioAsync(b4);
                Assert.Fail("Should have thrown");
            }
            catch (ConnectionFailedException)
            {

            }
            catch (Exception e)
            {
                Assert.Fail($"Got wrong exception type. {Environment.NewLine} {e}");
            }

            mock.Verify(m => m.ConnectAsync(It.IsAny<CancellationToken>()), Times.Exactly(4));
        }

        [TestMethod]
        public async Task ProgressResetsConnectionCounter()
        {
            var errorTCS = new TaskCompletionSource(TaskCreationOptions.RunContinuationsAsynchronously);
            var mock = new Mock<IStreamTransport<object>>();

            int conectionCount = 0;

            mock.Setup(mock => mock.ConnectAsync(It.IsAny<CancellationToken>()))
                .Callback<CancellationToken>((token) =>
                {
                    if (conectionCount++ % 2 == 0)
                    {
                        throw new Exception("Connect failed");
                    }
                })
                .Returns(Task.CompletedTask);

            bool sendThrow = false;
            int callbackCount = 0;
            int callbackTarget = 0;
            SemaphoreSlim callbackDone = new SemaphoreSlim(0);

            mock.Setup(mock => mock.SendAudioAsync(It.IsAny<ReadOnlyMemory<byte>>(), It.IsAny<CancellationToken>()))
                .Callback<ReadOnlyMemory<byte>, CancellationToken>((data, token) =>
                {
                    if (sendThrow)
                    {
                        sendThrow = false;
                        throw new Exception("Send failed");
                    }

                    if (++callbackCount == callbackTarget)
                    {
                        callbackDone.Release();
                    }
                })
                .Returns(Task.CompletedTask);

            var testClient = new SpeechSDKRemoteClient<object>(() => mock.Object);

            var format = CreateAudioStreamFormat(16, 16000, 1);
            testClient.SetAudioFormat(format);

            bool errorRaised = false;
            testClient.Stopped += (s, e) =>
            {
                errorRaised = true;
            };

            var b1 = new byte[] { 1, 2, 3, 4 };

            // Causes 3 errors, 2 connection, 1 sending.
            sendThrow = true;
            callbackCount = 0;
            callbackTarget = 1;
            await testClient.SendAudioAsync(b1);
            await TimeoutTestAction(callbackDone.WaitAsync());

            callbackTarget = 2;
            await testClient.SendAudioAsync(b1);
            await TimeoutTestAction(callbackDone.WaitAsync());

            mock.Verify(m => m.ConnectAsync(It.IsAny<CancellationToken>()), Times.Exactly(4));

            mock.Raise(m => m.AudioAcknowledged += null, new AudioAcknowledgedEventArgs(TimeSpan.Zero));

            // Causes 3 errors, 2 connection, 1 sending.
            sendThrow = true;
            callbackCount = 0;
            callbackTarget = 3;
            await testClient.SendAudioAsync(b1);
            await TimeoutTestAction(callbackDone.WaitAsync());

            callbackTarget = 4;
            await testClient.SendAudioAsync(b1);
            await TimeoutTestAction(callbackDone.WaitAsync());

            mock.Verify(m => m.ConnectAsync(It.IsAny<CancellationToken>()), Times.Exactly(6));

            mock.Raise(m => m.AudioAcknowledged += null, new AudioAcknowledgedEventArgs(TimeSpan.Zero));

            // Causes 3 errors, 2 connection, 1 sending.
            sendThrow = true;
            callbackCount = 0;
            callbackTarget = 5;
            await testClient.SendAudioAsync(b1);
            await TimeoutTestAction(callbackDone.WaitAsync());

            callbackTarget = 6;
            await testClient.SendAudioAsync(b1);
            await TimeoutTestAction(callbackDone.WaitAsync());

            mock.Verify(m => m.ConnectAsync(It.IsAny<CancellationToken>()), Times.Exactly(8));

            mock.Raise(m => m.AudioAcknowledged += null, new AudioAcknowledgedEventArgs(TimeSpan.Zero));

            // Causes 3 errors, 2 connection, 1 sending.
            sendThrow = true;
            callbackCount = 0;
            callbackTarget = 7;
            await testClient.SendAudioAsync(b1);
            await TimeoutTestAction(callbackDone.WaitAsync());

            callbackTarget = 8;
            await testClient.SendAudioAsync(b1);
            await TimeoutTestAction(callbackDone.WaitAsync());

            mock.Verify(m => m.ConnectAsync(It.IsAny<CancellationToken>()), Times.Exactly(10));

            Assert.IsFalse(errorRaised, "No user reportable errors should have happened.");
        }

        [TestMethod]
        public async Task StopPropogatesReasons()
        {
            var stoppedTCS = new TaskCompletionSource<RecognitionStoppedReason>(TaskCreationOptions.RunContinuationsAsynchronously);
            var mock = new Mock<IStreamTransport<object>>();

            mock.Setup(mock => mock.SendAudioAsync(It.IsAny<ReadOnlyMemory<byte>>(), It.IsAny<CancellationToken>()))
                    .Callback<ReadOnlyMemory<byte>, CancellationToken>((data, token) =>
                    {
                        mock.Raise(m => m.Stopped += null, new StoppedEventArgs(RecognitionStoppedReason.EndOfStream));
                    })
                    .Returns(Task.CompletedTask);

            var testClient = new SpeechSDKRemoteClient<object>(() => mock.Object);

            var format = CreateAudioStreamFormat(16, 16000, 1);
            testClient.SetAudioFormat(format);

            testClient.Stopped += (s, e) =>
            {
                stoppedTCS.SetResult(e.Reason);
            };

            var b1 = new byte[] { 1, 2, 3, 4 };

            await testClient.SendAudioAsync(b1);

            await TimeoutTestAction(stoppedTCS).ConfigureAwait(false);
            Assert.AreEqual(RecognitionStoppedReason.EndOfStream, await stoppedTCS.Task.ConfigureAwait(false));

        }
        private AudioFormat CreateAudioStreamFormat(int bitsPerSample, int samplesPerSecond, int channelCount)
        {
            return new AudioFormat
            {
                BitsPerSample = bitsPerSample,
                SamplesPerSecond = samplesPerSecond,
                ChannelCount = channelCount
            }; ;
        }
    }
}
