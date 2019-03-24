//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.CognitiveServices.Speech.Audio;

using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace Microsoft.CognitiveServices.Speech.Tests.EndToEnd
{
    using static AssertHelpers;
    using static SpeechRecognitionTestsHelper;

    [TestClass]
    public class SpeechSynthesisTests : SynthesisTestBase
    {
        [ClassInitialize]
        public static void TestClassinitialize(TestContext context)
        {
            BaseClassInit(context);
        }

        [TestInitialize]
        public void Initialize()
        {
        }

        [TestMethod]
        public async Task Defaults()
        {
            using (var synthesizer = new SpeechSynthesizer(config))
            {
                using (var result1 = await synthesizer.SpeakTextAsync("{{{text1}}}")) /* "{{{text1}}}" has now completed rendering to default speakers */
                using (var result2 = await synthesizer.SpeakTextAsync("{{{text2}}}")) /* "{{{text2}}}" has now completed rendering to default speakers */
                {
                }
            }
        }

        [TestMethod]
        public async Task ExplicitlyUseDefaultSpeakers()
        {
            using (var deviceConfig = AudioConfig.FromDefaultSpeakerOutput())
            {
                using (var synthesizer = new SpeechSynthesizer(config, deviceConfig))
                {
                    using (var result1 = await synthesizer.SpeakTextAsync("{{{text1}}}")) /* "{{{text1}}}" has now completed rendering to default speakers */
                    using (var result2 = await synthesizer.SpeakTextAsync("{{{text2}}}")) /* "{{{text2}}}" has now completed rendering to default speakers */
                    {
                    }
                }
            }
        }

        [TestMethod]
        public async Task PickLanguage()
        {
            config.SpeechSynthesisLanguage = "en-GB";
            using (var synthesizer = new SpeechSynthesizer(config))
            {
                using (var result1 = await synthesizer.SpeakTextAsync("{{{text1}}}")) /* "{{{text1}}}" has now completed rendering to default speakers */
                using (var result2 = await synthesizer.SpeakTextAsync("{{{text2}}}")) /* "{{{text2}}}" has now completed rendering to default speakers */
                {
                }
            }
        }

        [TestMethod]
        public async Task PickVoice()
        {
            config.SpeechSynthesisVoiceName = "Microsoft Server Speech Text to Speech Voice (en-GB, HazelRUS)";
            using (var synthesizer = new SpeechSynthesizer(config))
            {
                using (var result1 = await synthesizer.SpeakTextAsync("{{{text1}}}")) /* "{{{text1}}}" has now completed rendering to default speakers */
                using (var result2 = await synthesizer.SpeakTextAsync("{{{text2}}}")) /* "{{{text2}}}" has now completed rendering to default speakers */
                {
                }
            }
        }

        [TestMethod]
        public async Task SynthesizerOutputToFile()
        {
            using (var fileConfig = AudioConfig.FromWavFileOutput("wavefile.wav"))
            {
                using (var synthesizer = new SpeechSynthesizer(config, fileConfig))
                {
                    using (var result1 = await synthesizer.SpeakTextAsync("{{{text1}}}")) /* "{{{wavefile.wav}}}" now contains synthesized audio for "{{{text1}}}"" */
                    {
                    }
                } /* "{{{wavefile.wav}}}" is now closed */

                var waveSize1 = new FileInfo("wavefile.wav").Length;
                Assert.IsTrue(waveSize1 > EmptyWaveFileSize);

                using (var synthesizer = new SpeechSynthesizer(config, fileConfig))
                {
                    using (var result1 = await synthesizer.SpeakTextAsync("{{{text1}}}")) /* "{{{wavefile.wav}}}" now contains synthesized audio for "{{{text1}}}"" */
                    using (var result2 = await synthesizer.SpeakTextAsync("{{{text2}}}")) /* "{{{wavefile.wav}}}" now contains synthesized audio for both "{{{text1}}}"" and "{{{text2}}}" */
                    {
                    }
                } /* "{{{wavefile.wav}}}" is now closed */

                var waveSize2 = new FileInfo("wavefile.wav").Length;
                Assert.IsTrue(waveSize2 > waveSize1);
            }
        }

        [TestMethod]
        public async Task SynthesizerOutputToPushStream()
        {
            using (var callback = new PushAudioOutputStreamTestCallback())
            using (var stream = AudioOutputStream.CreatePushStream(callback))
            using (var streamConfig = AudioConfig.FromStreamOutput(stream))
            {
                using (var synthesizer = new SpeechSynthesizer(config, streamConfig))
                {
                    using (var result1 = await synthesizer.SpeakTextAsync("{{{text1}}}")) /* "{{{text1}}}" has completed rendering to pushstream */
                    using (var result2 = await synthesizer.SpeakTextAsync("{{{text2}}}")) /* "{{{text2}}}" has completed rendering to pushstream */
                    {
                    }

                    Assert.IsFalse(callback.IsClosed());
                }

                Assert.IsTrue(callback.GetAudioLength() > 0);
                Assert.IsTrue(callback.IsClosed());
            }
        }

        [TestMethod]
        public async Task SynthesizerOutputToPullStreamUseAfterSynthesisCompleted()
        {
            using (var stream = AudioOutputStream.CreatePullStream())
            using (var streamConfig = AudioConfig.FromStreamOutput(stream))
            {
                using (var synthesizer = new SpeechSynthesizer(config, streamConfig))
                {
                    using (var result1 = await synthesizer.SpeakTextAsync("{{{text1}}}")) /* "{{{text1}}}" has completed rendering to pullstream */
                    using (var result2 = await synthesizer.SpeakTextAsync("{{{text2}}}")) /* "{{{text2}}}" has completed rendering to pullstream */
                    {
                    }
                }

                DoSomethingWithAudioInPullStream(stream);
            }
        }

        [TestMethod]
        public async Task SynthesizerOutputToPullStreamStartUsingBeforeDoneSynthesizing()
        {
            using (var stream = AudioOutputStream.CreatePullStream())
            {
                var future = DoSomethingWithAudioInPullStreamInBackground(stream);

                using (var streamConfig = AudioConfig.FromStreamOutput(stream))
                using (var synthesizer = new SpeechSynthesizer(config, streamConfig))
                {
                    using (var result1 = await synthesizer.SpeakTextAsync("{{{text1}}}")) /* "{{{text1}}}" has completed rendering to pullstream */
                    using (var result2 = await synthesizer.SpeakTextAsync("{{{text2}}}")) /* "{{{text2}}}" has completed rendering to pullstream */
                    {
                    }
                }

                await future;
            }
        }

        [TestMethod]
        public async Task SpeakOutInResults()
        {
            using (var synthesizer = new SpeechSynthesizer(config, null)) /* null indicates to do nothing with synthesizer audio by default */
            {
                using (var result1 = await synthesizer.SpeakTextAsync("{{{text1}}}")) /* "{{{text1}}}" has completed rendering, and available in result1 */
                {
                    Assert.IsTrue(result1.ResultId.Length == GuidLength);
                    Assert.AreEqual(ResultReason.SynthesizingAudioCompleted, result1.Reason);

                    DoSomethingWithAudioInVector(result1.AudioData);
                }

                using (var result2 = await synthesizer.SpeakTextAsync("{{{text2}}}")) /* "{{{text2}}}" has completed rendering, and available in result2 */
                {
                    Assert.IsTrue(result2.ResultId.Length == GuidLength);
                    Assert.AreEqual(ResultReason.SynthesizingAudioCompleted, result2.Reason);

                    DoSomethingWithAudioInVector(result2.AudioData);
                }
            }
        }

        [TestMethod]
        public async Task SpeakOutputInChunksInEventSynthesizing()
        {
            using (var synthesizer = new SpeechSynthesizer(config, null)) /* null indicates to do nothing with synthesizer audio by default */
            {
                synthesizer.Synthesizing += (s, e) =>
                {
                    var resultReason = e.Result.Reason;
                    Assert.AreEqual(ResultReason.SynthesizingAudio, resultReason);

                    var audioLength = e.Result.AudioData.Length;
                    Assert.IsTrue(audioLength > 0);

                    DoSomethingWithAudioInVector(e.Result.AudioData);
                };

                using (var result1 = await synthesizer.SpeakTextAsync("{{{text1}}}")) /* "{{{text1}}}" has completed rendering */
                using (var result2 = await synthesizer.SpeakTextAsync("{{{text2}}}")) /* "{{{text2}}}" has completed rendering */
                {
                }
            }
        }

        [TestMethod]
        public async Task SpeakOutputInStreams()
        {
            using (var synthesizer = new SpeechSynthesizer(config, null)) /* null indicates to do nothing with synthesizer audio by default */
            {
                using (var result1 = await synthesizer.SpeakTextAsync("{{{text1}}}")) /* "{{{text1}}}" has completed rendering, and available in result1 */
                {
                    Assert.AreEqual(ResultReason.SynthesizingAudioCompleted, result1.Reason);
                    Assert.IsTrue(result1.AudioData.Length > 0);
                    using (var stream1 = AudioDataStream.FromResult(result1)) /* of type AudioDataStream, does not block */
                    {
                        DoSomethingWithAudioInDataStream(stream1, true);
                    }
                }

                using (var result2 = await synthesizer.SpeakTextAsync("{{{text2}}}")) /* "{{{text2}}}" has completed rendering, and available in result2 */
                {
                    Assert.AreEqual(ResultReason.SynthesizingAudioCompleted, result2.Reason);
                    Assert.IsTrue(result2.AudioData.Length > 0);

                    using (var stream2 = AudioDataStream.FromResult(result2)) /* of type AudioDataStream, does not block */
                    {
                        DoSomethingWithAudioInDataStream(stream2, true);
                    }
                }
            }
        }

        [TestMethod]
        public async Task SpeakOutputInStreamsBeforeDoneFromEventSynthesisStarted()
        {
            using (var synthesizer = new SpeechSynthesizer(config, null)) /* null indicates to do nothing with synthesizer audio by default */
            {
                var futureList = new List<Task>();
                var streamList = new List<AudioDataStream>();
                synthesizer.SynthesisStarted += (s, e) => {
                    var resultReason = e.Result.Reason;
                    Assert.AreEqual(ResultReason.SynthesizingAudioStarted, resultReason);

                    var audioLength = e.Result.AudioData.Length;
                    Assert.AreEqual(0, audioLength);

                    var stream = AudioDataStream.FromResult(e.Result); /* of type AudioDataStream, does not block */
                    futureList.Add(DoSomethingWithAudioInDataStreamInBackground(stream, false));
                    streamList.Add(stream);
                };

                using (var result1 = await synthesizer.SpeakTextAsync("{{{text1}}}")) /* "{{{text1}}}" has completed rendering */
                using (var result2 = await synthesizer.SpeakTextAsync("{{{text2}}}")) /* "{{{text2}}}" has completed rendering */
                {
                }

                var future3 = synthesizer.SpeakTextAsync("{{{text3}}}"); /* "{{{text3}}}" synthesis might have started */
                using (var result3 = await future3) /* "{{{text3}}}" synthesis has completed */
                {
                }

                foreach (var future in futureList)
                {
                    await future;
                }

                foreach (var stream in streamList)
                {
                    stream.Dispose();
                }
            }
        }

        [TestMethod]
        public async Task SpeakOutputInStreamsBeforeDoneFromMethodStartSpeakingTextAsync()
        {
            using (var synthesizer = new SpeechSynthesizer(config, null)) /* null indicates to do nothing with synthesizer audio by default */
            {
                using (var result1 = await synthesizer.StartSpeakingTextAsync("{{{text1}}}")) /* "{{{text1}}}" synthesis has started, likely not finished */
                {
                    Assert.AreEqual(ResultReason.SynthesizingAudioStarted, result1.Reason);
                    Assert.AreEqual(0, result1.AudioData.Length);

                    using (var stream1 = AudioDataStream.FromResult(result1)) /* of type AudioDataStream, does not block */
                    {
                        var future1 = DoSomethingWithAudioInDataStreamInBackground(stream1, false); /* does not block, just spins a thread up */

                        using (var result2 = await synthesizer.StartSpeakingTextAsync("{{{text2}}}")) /* "{{{text2}}}" synthesis has started, likely not finished */
                        {
                            Assert.AreEqual(ResultReason.SynthesizingAudioStarted, result2.Reason);
                            Assert.AreEqual(0, result2.AudioData.Length);

                            using (var stream2 = AudioDataStream.FromResult(result2)) /* of type AudioDataStream, does not block */
                            {
                                var future2 = DoSomethingWithAudioInDataStreamInBackground(stream2, false); /* does not block, just spins a thread up */

                                await future1;
                                await future2;
                            }
                        }
                    }
                }
            }
        }

        [TestMethod]
        public async Task SpeakOutputInStreamsBeforeDoneQueued()
        {
            using (var synthesizer = new SpeechSynthesizer(config, null)) /* null indicates to do nothing with synthesizer audio by default */
            {
                int startedRequests = 0;
                synthesizer.SynthesisStarted += (s, e) =>
                {
                    startedRequests++;
                };

                int completedRequests = 0;
                synthesizer.SynthesisCompleted += (s, e) =>
                {
                    completedRequests++;
                    if (completedRequests == 1)
                    {
                        // This is to check the requests is queued
                        // When one request is already completed, the other one is still not started
                        Assert.AreEqual(1, startedRequests);
                    }
                };

                var futureResult1 = synthesizer.StartSpeakingTextAsync("{{{text1}}}"); /* "{{{text1}}}" synthesis might have started, likely not finished */
                var futureResult2 = synthesizer.StartSpeakingTextAsync("{{{text2}}}"); /* "{{{text2}}}" synthesis might have started (very unlikely) */

                var future1 = DoSomethingWithAudioInResultInBackground(futureResult1, false); /* does not block, just spins a thread up */
                var future2 = DoSomethingWithAudioInResultInBackground(futureResult2, false); /* does not block, just spins a thread up */

                await future1;
                await future2;
            }
        }

        [TestMethod]
        public async Task SpeakOutputInStreamsWithAllDataGetOnSynthesisStartedResult()
        {
            using (var synthesizer = new SpeechSynthesizer(config, null)) /* null indicates to do nothing with synthesizer audio by default */
            {
                using (var result = await synthesizer.StartSpeakingTextAsync("{{{text1}}}"))
                {
                    Assert.IsTrue(result.Reason == ResultReason.SynthesizingAudioStarted);
                    Assert.IsTrue(result.AudioData.Length == 0);

                    bool synthesisDone = false;
                    synthesizer.SynthesisCompleted += (s, e) =>
                    {
                        synthesisDone = true;
                    };

                    synthesizer.SynthesisCanceled += (s, e) =>
                    {
                        synthesisDone = true;
                    };

                    while(!synthesisDone)
                    {
                        Thread.Sleep(100); /* wait for the synthesis to be done */
                    }

                    using (var stream = AudioDataStream.FromResult(result))
                    {
                        DoSomethingWithAudioInDataStream(stream, true);
                    }
                }
            }
        }


        [TestMethod]
        [TestCategory("SpeechSynthesisMockTest")]
        public async Task DefaultsMock()
        {
            using (var synthesizer = new SpeechSynthesizer(mockConfig))
            {
                using (var result1 = await synthesizer.SpeakTextAsync("{{{text1}}}")) /* "{{{text1}}}" has now completed rendering to default speakers */
                using (var result2 = await synthesizer.SpeakTextAsync("{{{text2}}}")) /* "{{{text2}}}" has now completed rendering to default speakers */
                {
                    var expectedAudioData1 = BuildMockSynthesizedAudio("{{{text1}}}", DefaultLanguage, DefaultVoice);
                    var expectedAudioData2 = BuildMockSynthesizedAudio("{{{text2}}}", DefaultLanguage, DefaultVoice);

                    Assert.IsTrue(AreBinaryEqual(expectedAudioData1, result1.AudioData));
                    Assert.IsTrue(AreBinaryEqual(expectedAudioData2, result2.AudioData));
                }
            }
        }

        [TestMethod]
        [TestCategory("SpeechSynthesisMockTest")]
        public async Task ExplicitlyUseDefaultSpeakersMock()
        {
            using (var deviceConfig = AudioConfig.FromDefaultSpeakerOutput())
            {
                using (var synthesizer = new SpeechSynthesizer(mockConfig, deviceConfig))
                {
                    using (var result1 = await synthesizer.SpeakTextAsync("{{{text1}}}")) /* "{{{text1}}}" has now completed rendering to default speakers */
                    using (var result2 = await synthesizer.SpeakTextAsync("{{{text2}}}")) /* "{{{text2}}}" has now completed rendering to default speakers */
                    {
                        var expectedAudioData1 = BuildMockSynthesizedAudio("{{{text1}}}", DefaultLanguage, DefaultVoice);
                        var expectedAudioData2 = BuildMockSynthesizedAudio("{{{text2}}}", DefaultLanguage, DefaultVoice);

                        Assert.IsTrue(AreBinaryEqual(expectedAudioData1, result1.AudioData));
                        Assert.IsTrue(AreBinaryEqual(expectedAudioData2, result2.AudioData));
                    }
                }
            }
        }

        [TestMethod]
        [TestCategory("SpeechSynthesisMockTest")]
        public async Task PickLanguageMock()
        {
            mockConfig.SpeechSynthesisLanguage = "en-GB";
            using (var synthesizer = new SpeechSynthesizer(mockConfig))
            {
                using (var result1 = await synthesizer.SpeakTextAsync("{{{text1}}}")) /* "{{{text1}}}" has now completed rendering to default speakers */
                using (var result2 = await synthesizer.SpeakTextAsync("{{{text2}}}")) /* "{{{text2}}}" has now completed rendering to default speakers */
                {
                    var expectedAudioData1 = BuildMockSynthesizedAudio("{{{text1}}}", "en-GB", "Microsoft Server Speech Text to Speech Voice (en-GB, HazelRUS)");
                    var expectedAudioData2 = BuildMockSynthesizedAudio("{{{text2}}}", "en-GB", "Microsoft Server Speech Text to Speech Voice (en-GB, HazelRUS)");

                    Assert.IsTrue(AreBinaryEqual(expectedAudioData1, result1.AudioData));
                    Assert.IsTrue(AreBinaryEqual(expectedAudioData2, result2.AudioData));
                }
            }
        }

        [TestMethod]
        [TestCategory("SpeechSynthesisMockTest")]
        public async Task PickVoiceMock()
        {
            mockConfig.SpeechSynthesisVoiceName = "Microsoft Server Speech Text to Speech Voice (en-GB, HazelRUS)";
            using (var synthesizer = new SpeechSynthesizer(mockConfig))
            {
                using (var result1 = await synthesizer.SpeakTextAsync("{{{text1}}}")) /* "{{{text1}}}" has now completed rendering to default speakers */
                using (var result2 = await synthesizer.SpeakTextAsync("{{{text2}}}")) /* "{{{text2}}}" has now completed rendering to default speakers */
                {
                    var expectedAudioData1 = BuildMockSynthesizedAudio("{{{text1}}}", DefaultLanguage, "Microsoft Server Speech Text to Speech Voice (en-GB, HazelRUS)");
                    var expectedAudioData2 = BuildMockSynthesizedAudio("{{{text2}}}", DefaultLanguage, "Microsoft Server Speech Text to Speech Voice (en-GB, HazelRUS)");

                    Assert.IsTrue(AreBinaryEqual(expectedAudioData1, result1.AudioData));
                    Assert.IsTrue(AreBinaryEqual(expectedAudioData2, result2.AudioData));
                }
            }
        }

        [TestMethod]
        [TestCategory("SpeechSynthesisMockTest")]
        public async Task SynthesizerOutputToFileMock()
        {
            using (var fileConfig = AudioConfig.FromWavFileOutput("wavefile.wav"))
            {
                using (var synthesizer = new SpeechSynthesizer(mockConfig, fileConfig))
                {
                    using (var result1 = await synthesizer.SpeakTextAsync("{{{text1}}}")) /* "{{{wavefile.wav}}}" now contains synthesized audio for "{{{text1}}}"" */
                    {
                    }
                } /* "{{{wavefile.wav}}}" is now closed */

                var waveData1 = LoadWaveFileData("wavefile.wav");
                var expectedAudioData1 = BuildMockSynthesizedAudio("{{{text1}}}", DefaultLanguage, DefaultVoice);
                Assert.IsTrue(AreBinaryEqual(expectedAudioData1, waveData1));

                using (var synthesizer = new SpeechSynthesizer(mockConfig, fileConfig))
                {
                    using (var result1 = await synthesizer.SpeakTextAsync("{{{text1}}}")) /* "{{{wavefile.wav}}}" now contains synthesized audio for "{{{text1}}}"" */
                    using (var result2 = await synthesizer.SpeakTextAsync("{{{text2}}}")) /* "{{{wavefile.wav}}}" now contains synthesized audio for both "{{{text1}}}"" and "{{{text2}}}" */
                    {
                    }
                } /* "{{{wavefile.wav}}}" is now closed */

                var waveData12 = LoadWaveFileData("wavefile.wav");
                var expectedAudioData2 = BuildMockSynthesizedAudio("{{{text2}}}", DefaultLanguage, DefaultVoice);
                var expectedAudioData12 = MergeBinary(expectedAudioData1, expectedAudioData2);
                Assert.IsTrue(AreBinaryEqual(expectedAudioData12, waveData12));
            }
        }

        [Ignore]
        [TestMethod]
        [TestCategory("SpeechSynthesisMockTest")]
        public async Task SynthesizerOutputToPushStreamMock()
        {
            using (var callback = new PushAudioOutputStreamTestCallback())
            using (var stream = AudioOutputStream.CreatePushStream(callback))
            using (var streamConfig = AudioConfig.FromStreamOutput(stream))
            {
                using (var synthesizer = new SpeechSynthesizer(mockConfig, streamConfig))
                {
                    var expectedAudioData1 = BuildMockSynthesizedAudio("{{{text1}}}", DefaultLanguage, DefaultVoice);
                    var expectedAudioData2 = BuildMockSynthesizedAudio("{{{text2}}}", DefaultLanguage, DefaultVoice);
                    var expectedAudioData12 = MergeBinary(expectedAudioData1, expectedAudioData2);

                    using (var result1 = await synthesizer.SpeakTextAsync("{{{text1}}}")) /* "{{{text1}}}" has completed rendering to pushstream */
                    {
                        Assert.IsTrue(AreBinaryEqual(expectedAudioData1, callback.GetAudioData()));
                        Assert.IsFalse(callback.IsClosed());
                    }

                    using (var result2 = await synthesizer.SpeakTextAsync("{{{text2}}}")) /* "{{{text2}}}" has completed rendering to pushstream */
                    {
                        Assert.IsTrue(AreBinaryEqual(expectedAudioData12, callback.GetAudioData()));
                        Assert.IsFalse(callback.IsClosed());
                    }

                    Assert.IsFalse(callback.IsClosed());
                }

                Assert.IsTrue(callback.IsClosed());
            }
        }

        [TestMethod]
        [TestCategory("SpeechSynthesisMockTest")]
        public async Task SynthesizerOutputToPullStreamUseAfterSynthesisCompletedMock()
        {
            using (var stream = AudioOutputStream.CreatePullStream())
            using (var streamConfig = AudioConfig.FromStreamOutput(stream))
            {
                using (var synthesizer = new SpeechSynthesizer(mockConfig, streamConfig))
                {
                    using (var result1 = await synthesizer.SpeakTextAsync("{{{text1}}}")) /* "{{{text1}}}" has completed rendering to pullstream */
                    using (var result2 = await synthesizer.SpeakTextAsync("{{{text2}}}")) /* "{{{text2}}}" has completed rendering to pullstream */
                    {
                    }
                }

                var expectedAudioData1 = BuildMockSynthesizedAudio("{{{text1}}}", DefaultLanguage, DefaultVoice);
                var expectedAudioData2 = BuildMockSynthesizedAudio("{{{text2}}}", DefaultLanguage, DefaultVoice);
                var expectedAudioData12 = MergeBinary(expectedAudioData1, expectedAudioData2);

                DoSomethingWithAudioInPullStream(stream, expectedAudioData12);
            }
        }

        [TestMethod]
        [TestCategory("SpeechSynthesisMockTest")]
        public async Task SynthesizerOutputToPullStreamStartUsingBeforeDoneSynthesizingMock()
        {
            using (var stream = AudioOutputStream.CreatePullStream())
            {
                var expectedAudioData1 = BuildMockSynthesizedAudio("{{{text1}}}", DefaultLanguage, DefaultVoice);
                var expectedAudioData2 = BuildMockSynthesizedAudio("{{{text2}}}", DefaultLanguage, DefaultVoice);
                var expectedAudioData12 = MergeBinary(expectedAudioData1, expectedAudioData2);

                var future = DoSomethingWithAudioInPullStreamInBackground(stream, expectedAudioData12);

                using (var streamConfig = AudioConfig.FromStreamOutput(stream))
                using (var synthesizer = new SpeechSynthesizer(mockConfig, streamConfig))
                {
                    using (var result1 = await synthesizer.SpeakTextAsync("{{{text1}}}")) /* "{{{text1}}}" has completed rendering to pullstream */
                    using (var result2 = await synthesizer.SpeakTextAsync("{{{text2}}}")) /* "{{{text2}}}" has completed rendering to pullstream */
                    {
                    }
                }

                await future;
            }
        }

        [TestMethod]
        [TestCategory("SpeechSynthesisMockTest")]
        public async Task SpeakOutInResultsMock()
        {
            using (var synthesizer = new SpeechSynthesizer(mockConfig, null)) /* null indicates to do nothing with synthesizer audio by default */
            {
                using (var result1 = await synthesizer.SpeakTextAsync("{{{text1}}}")) /* "{{{text1}}}" has completed rendering, and available in result1 */
                {
                    Assert.IsTrue(result1.ResultId.Length == GuidLength);
                    Assert.AreEqual(ResultReason.SynthesizingAudioCompleted, result1.Reason);

                    var expectedAudioData1 = BuildMockSynthesizedAudio("{{{text1}}}", DefaultLanguage, DefaultVoice);
                    Assert.IsTrue(AreBinaryEqual(expectedAudioData1, result1.AudioData));
                }

                using (var result2 = await synthesizer.SpeakTextAsync("{{{text2}}}")) /* "{{{text2}}}" has completed rendering, and available in result2 */
                {
                    Assert.IsTrue(result2.ResultId.Length == GuidLength);
                    Assert.AreEqual(ResultReason.SynthesizingAudioCompleted, result2.Reason);

                    var expectedAudioData2 = BuildMockSynthesizedAudio("{{{text2}}}", DefaultLanguage, DefaultVoice);
                    Assert.IsTrue(AreBinaryEqual(expectedAudioData2, result2.AudioData));
                }
            }
        }

        [TestMethod]
        [TestCategory("SpeechSynthesisMockTest")]
        public async Task SpeakOutputInChunksInEventSynthesizingMock()
        {
            using (var synthesizer = new SpeechSynthesizer(mockConfig, null)) /* null indicates to do nothing with synthesizer audio by default */
            {
                var expectedAudioData1 = BuildMockSynthesizedAudio("{{{text1}}}", DefaultLanguage, DefaultVoice);
                var expectedAudioData2 = BuildMockSynthesizedAudio("{{{text2}}}", DefaultLanguage, DefaultVoice);
                var expectedAudioData12 = MergeBinary(expectedAudioData1, expectedAudioData2);

                int offset = 0;
                synthesizer.Synthesizing += (s, e) =>
                {
                    var resultReason = e.Result.Reason;
                    Assert.AreEqual(ResultReason.SynthesizingAudio, resultReason);

                    var audioLength = e.Result.AudioData.Length;
                    Assert.IsTrue(audioLength > 0);

                    byte[] expectedAudioChunk = new byte[audioLength];
                    Array.Copy(expectedAudioData12, offset, expectedAudioChunk, 0, audioLength);
                    Assert.IsTrue(AreBinaryEqual(expectedAudioChunk, e.Result.AudioData));

                    offset += audioLength;
                };

                using (var result1 = await synthesizer.SpeakTextAsync("{{{text1}}}")) /* "{{{text1}}}" has completed rendering */
                using (var result2 = await synthesizer.SpeakTextAsync("{{{text2}}}")) /* "{{{text2}}}" has completed rendering */
                {
                }
            }
        }

        [TestMethod]
        [TestCategory("SpeechSynthesisMockTest")]
        public async Task SpeakOutputInStreamsMock()
        {
            using (var synthesizer = new SpeechSynthesizer(mockConfig, null)) /* null indicates to do nothing with synthesizer audio by default */
            {
                using (var result1 = await synthesizer.SpeakTextAsync("{{{text1}}}")) /* "{{{text1}}}" has completed rendering, and available in result1 */
                {
                    Assert.AreEqual(ResultReason.SynthesizingAudioCompleted, result1.Reason);
                    Assert.IsTrue(result1.AudioData.Length > 0);
                    using (var stream1 = AudioDataStream.FromResult(result1)) /* of type AudioDataStream, does not block */
                    {
                        var expectedAudioData1 = BuildMockSynthesizedAudio("{{{text1}}}", DefaultLanguage, DefaultVoice);
                        DoSomethingWithAudioInDataStream(stream1, true, expectedAudioData1);

                        using (var result2 = await synthesizer.SpeakTextAsync("{{{text2}}}")) /* "{{{text2}}}" has completed rendering, and available in result2 */
                        {
                            Assert.AreEqual(ResultReason.SynthesizingAudioCompleted, result2.Reason);
                            Assert.IsTrue(result2.AudioData.Length > 0);

                            using (var stream2 = AudioDataStream.FromResult(result2)) /* of type AudioDataStream, does not block */
                            {
                                var expectedAudioData2 = BuildMockSynthesizedAudio("{{{text2}}}", DefaultLanguage, DefaultVoice);
                                DoSomethingWithAudioInDataStream(stream2, true, expectedAudioData2);

                                stream1.SetPosition(0);
                                DoSomethingWithAudioInDataStream(stream1, true, expectedAudioData1); /* re-check stream1 to make sure it's not impacted by stream2 */
                            }
                        }
                    }
                }
            }
        }

        [TestMethod]
        [TestCategory("SpeechSynthesisMockTest")]
        public async Task SpeakOutputInStreamsBeforeDoneFromEventSynthesisStartedMock()
        {
            using (var synthesizer = new SpeechSynthesizer(mockConfig, null)) /* null indicates to do nothing with synthesizer audio by default */
            {
                var expectedAudioData = new List<byte[]>();
                expectedAudioData.Add(BuildMockSynthesizedAudio("{{{text1}}}", DefaultLanguage, DefaultVoice));
                expectedAudioData.Add(BuildMockSynthesizedAudio("{{{text2}}}", DefaultLanguage, DefaultVoice));
                expectedAudioData.Add(BuildMockSynthesizedAudio("{{{text3}}}", DefaultLanguage, DefaultVoice));

                var futureList = new List<Task>();
                var streamList = new List<AudioDataStream>();
                int requestOrder = -1;
                synthesizer.SynthesisStarted += (s, e) => {
                    var resultReason = e.Result.Reason;
                    Assert.AreEqual(ResultReason.SynthesizingAudioStarted, resultReason);

                    var audioLength = e.Result.AudioData.Length;
                    Assert.AreEqual(0, audioLength);

                    requestOrder++;

                    var stream = AudioDataStream.FromResult(e.Result); /* of type AudioDataStream, does not block */
                    futureList.Add(DoSomethingWithAudioInDataStreamInBackground(stream, false, expectedAudioData[requestOrder]));
                    streamList.Add(stream);
                };

                using (var result1 = await synthesizer.SpeakTextAsync("{{{text1}}}")) /* "{{{text1}}}" has completed rendering */
                using (var result2 = await synthesizer.SpeakTextAsync("{{{text2}}}")) /* "{{{text2}}}" has completed rendering */
                {
                }

                var future3 = synthesizer.SpeakTextAsync("{{{text3}}}"); /* "{{{text3}}}" synthesis might have started */
                using (var result3 = await future3) /* "{{{text3}}}" synthesis has completed */
                {
                }

                foreach (var future in futureList)
                {
                    await future;
                }

                foreach (var stream in streamList)
                {
                    stream.Dispose();
                }
            }
        }

        [TestMethod]
        [TestCategory("SpeechSynthesisMockTest")]
        public async Task SpeakOutputInStreamsBeforeDoneFromMethodStartSpeakingTextAsyncMock()
        {
            using (var synthesizer = new SpeechSynthesizer(mockConfig, null)) /* null indicates to do nothing with synthesizer audio by default */
            {
                using (var result1 = await synthesizer.StartSpeakingTextAsync("{{{text1}}}")) /* "{{{text1}}}" synthesis has started, likely not finished */
                {
                    Assert.AreEqual(ResultReason.SynthesizingAudioStarted, result1.Reason);
                    Assert.AreEqual(0, result1.AudioData.Length);

                    using (var stream1 = AudioDataStream.FromResult(result1)) /* of type AudioDataStream, does not block */
                    {
                        var expectedAudioData1 = BuildMockSynthesizedAudio("{{{text1}}}", DefaultLanguage, DefaultVoice);
                        var future1 = DoSomethingWithAudioInDataStreamInBackground(stream1, false, expectedAudioData1); /* does not block, just spins a thread up */

                        using (var result2 = await synthesizer.StartSpeakingTextAsync("{{{text2}}}")) /* "{{{text2}}}" synthesis has started, likely not finished */
                        {
                            Assert.AreEqual(ResultReason.SynthesizingAudioStarted, result2.Reason);
                            Assert.AreEqual(0, result2.AudioData.Length);

                            using (var stream2 = AudioDataStream.FromResult(result2)) /* of type AudioDataStream, does not block */
                            {
                                var expectedAudioData2 = BuildMockSynthesizedAudio("{{{text2}}}", DefaultLanguage, DefaultVoice);
                                var future2 = DoSomethingWithAudioInDataStreamInBackground(stream2, false, expectedAudioData2); /* does not block, just spins a thread up */

                                await future1;
                                await future2;
                            }
                        }
                    }
                }
            }
        }

        [TestMethod]
        [TestCategory("SpeechSynthesisMockTest")]
        public async Task SpeakOutputInStreamsBeforeDoneQueuedMock()
        {
            using (var synthesizer = new SpeechSynthesizer(mockConfig, null)) /* null indicates to do nothing with synthesizer audio by default */
            {
                int startedRequests = 0;
                synthesizer.SynthesisStarted += (s, e) =>
                {
                    startedRequests++;
                };

                int completedRequests = 0;
                synthesizer.SynthesisCompleted += (s, e) =>
                {
                    completedRequests++;
                    if (completedRequests == 1)
                    {
                        // This is to check the requests is queued
                        // When one request is already completed, the other one is still not started
                        Assert.AreEqual(1, startedRequests);
                    }
                };

                var futureResult1 = synthesizer.StartSpeakingTextAsync("{{{text1}}}"); /* "{{{text1}}}" synthesis might have started, likely not finished */
                var futureResult2 = synthesizer.StartSpeakingTextAsync("{{{text2}}}"); /* "{{{text2}}}" synthesis might have started (very unlikely) */

                var expectedAudioData1 = BuildMockSynthesizedAudio("{{{text1}}}", DefaultLanguage, DefaultVoice);
                var future1 = DoSomethingWithAudioInResultInBackground(futureResult1, false, expectedAudioData1); /* does not block, just spins a thread up */
                var expectedAudioData2 = BuildMockSynthesizedAudio("{{{text2}}}", DefaultLanguage, DefaultVoice);
                var future2 = DoSomethingWithAudioInResultInBackground(futureResult2, false, expectedAudioData2); /* does not block, just spins a thread up */

                await future1;
                await future2;
            }
        }

        [TestMethod]
        [TestCategory("SpeechSynthesisMockTest")]
        public async Task SpeakOutputInStreamsWithAllDataGetOnSynthesisStartedResultMock()
        {
            using (var synthesizer = new SpeechSynthesizer(mockConfig, null)) /* null indicates to do nothing with synthesizer audio by default */
            {
                using (var result = await synthesizer.StartSpeakingTextAsync("{{{text1}}}"))
                {
                    Assert.IsTrue(result.Reason == ResultReason.SynthesizingAudioStarted);
                    Assert.IsTrue(result.AudioData.Length == 0);

                    bool synthesisCompleted = false;
                    synthesizer.SynthesisCompleted += (s, e) =>
                    {
                        synthesisCompleted = true;
                    };

                    while (!synthesisCompleted)
                    {
                        Thread.Sleep(100); /* wait for the synthesis to be done */
                    }

                    using (var stream = AudioDataStream.FromResult(result))
                    {
                        Thread.Sleep(500); /* wait for more time for synthesis completed event to be sent to stream */
                        var expectedAudioData = BuildMockSynthesizedAudio("{{{text1}}}", DefaultLanguage, DefaultVoice);
                        DoSomethingWithAudioInDataStream(stream, true, expectedAudioData);
                    }
                }
            }
        }

        [TestMethod]
        [TestCategory("SpeechSynthesisMockTest")]
        public async Task SpeakOutputInStreamsWithAllDataGetSinceSynthesizingResultMock()
        {
            using (var synthesizer = new SpeechSynthesizer(mockConfig, null)) /* null indicates to do nothing with synthesizer audio by default */
            {
                bool firstChunkSkipped = false;
                AudioDataStream stream = null;
                synthesizer.Synthesizing += (s, e) =>
                {
                    if (firstChunkSkipped && stream == null)
                    {
                        stream = AudioDataStream.FromResult(e.Result);
                    }

                    firstChunkSkipped = true;
                };

                using (var result = await synthesizer.SpeakTextAsync("{{{text1}}}"))
                {
                    // Check result
                    Assert.IsTrue(result.Reason == ResultReason.SynthesizingAudioCompleted);

                    // Check stream
                    Assert.AreEqual(StreamStatus.PartialData, stream.GetStatus());

                    var fullAudioData = BuildMockSynthesizedAudio("{{{text1}}}", DefaultLanguage, DefaultVoice);
                    var expectedAudioData = new byte[fullAudioData.Length - MockAudioChunkSize];
                    Array.Copy(fullAudioData, MockAudioChunkSize, expectedAudioData, 0, expectedAudioData.Length);
                    CheckAudioInDataStream(stream, expectedAudioData);

                    stream.Dispose();
                }
            }
        }

        private void DoSomethingWithAudioInPullStream(PullAudioOutputStream stream)
        {
            DoSomethingWithAudioInPullStream(stream, null);
        }

        private void DoSomethingWithAudioInPullStream(PullAudioOutputStream stream, byte[] expectedData)
        {
            byte[] audioData = new byte[0];
            byte[] buffer = new byte[1024];
            uint totalSize = 0;
            uint filledSize = 0;

            while ((filledSize = stream.Read(buffer)) > 0)
            { /* blocks until atleast 1024 bytes are available */
              /* do something with buffer */
                totalSize += filledSize;
                Array.Resize(ref audioData, (int)totalSize);
                for (int i = 0; i < filledSize; ++i)
                {
                    audioData[totalSize - filledSize + i] = buffer[i];
                }
            }

            Assert.IsTrue(totalSize > 0);

            if (expectedData != null)
            {
                Assert.IsTrue(AreBinaryEqual(expectedData, audioData));
            }
        }

        private Task DoSomethingWithAudioInPullStreamInBackground(PullAudioOutputStream stream)
        {
            return DoSomethingWithAudioInPullStreamInBackground(stream, null);
        }

        private Task DoSomethingWithAudioInPullStreamInBackground(PullAudioOutputStream stream, byte[] expectedData)
        {
            return Task.Run(() =>
            {
                DoSomethingWithAudioInPullStream(stream, expectedData);
            });
        }

        private void DoSomethingWithAudioInVector(byte[] audioData)
        {
            Assert.IsTrue(audioData.Length > 0);
        }

        private void DoSomethingWithAudioInDataStream(AudioDataStream stream, bool afterSynthesisDone)
        {
            DoSomethingWithAudioInDataStream(stream, afterSynthesisDone, null);
        }

        private void DoSomethingWithAudioInDataStream(AudioDataStream stream, bool afterSynthesisDone, byte[] expectedData)
        {
            if (afterSynthesisDone)
            {
                Assert.AreEqual(StreamStatus.AllData, stream.GetStatus());
            }

            CheckAudioInDataStream(stream, expectedData);

            if (!afterSynthesisDone)
            {
                Assert.AreEqual(StreamStatus.AllData, stream.GetStatus());
            }
        }

        private void CheckAudioInDataStream(AudioDataStream stream, byte[] expectedData)
        {
            byte[] audioData = new byte[0];
            byte[] buffer = new byte[1024];
            uint totalSize = 0;
            uint filledSize = 0;

            while ((filledSize = stream.ReadData(buffer)) > 0)
            { /* blocks until atleast 1024 bytes are available */
              /* do something with buffer */
                totalSize += filledSize;
                Array.Resize(ref audioData, (int)totalSize);
                for (int i = 0; i < filledSize; ++i)
                {
                    audioData[totalSize - filledSize + i] = buffer[i];
                }
            }

            if (expectedData != null)
            {
                Assert.IsTrue(AreBinaryEqual(expectedData, audioData));
            }

            if (stream.GetStatus() == StreamStatus.Canceled)
            {
                Assert.IsTrue(totalSize == 0);

                var cancelDetails = SpeechSynthesisCancellationDetails.FromStream(stream);
                /* do something with cancellation details */
                Assert.AreEqual(CancellationReason.Error, cancelDetails.Reason);
                Assert.AreNotEqual(CancellationErrorCode.NoError, cancelDetails.ErrorCode);
            }
            else
            {
                Assert.IsTrue(totalSize > 0);
            }
        }

        private Task DoSomethingWithAudioInDataStreamInBackground(AudioDataStream stream, bool afterSynthesisDone)
        {
            return DoSomethingWithAudioInDataStreamInBackground(stream, afterSynthesisDone, null);
        }

        private Task DoSomethingWithAudioInDataStreamInBackground(AudioDataStream stream, bool afterSynthesisDone, byte[] expectedData)
        {
            return Task.Run(() =>
            {
                DoSomethingWithAudioInDataStream(stream, afterSynthesisDone, expectedData);
            });
        }

        private Task DoSomethingWithAudioInResultInBackground(Task<SpeechSynthesisResult> futureResult, bool afterSynthesisDone)
        {
            return DoSomethingWithAudioInResultInBackground(futureResult, afterSynthesisDone, null);
        }

        private Task DoSomethingWithAudioInResultInBackground(Task<SpeechSynthesisResult> futureResult, bool afterSynthesisDone, byte[] expectedData)
        {
            return Task.Run(async () =>
            {
                using (var result = await futureResult)
                using (var stream = AudioDataStream.FromResult(result))
                {
                    DoSomethingWithAudioInDataStream(stream, afterSynthesisDone, expectedData);
                }
            });
        }

        private bool AreBinaryEqual(byte[] expectedData, byte[] actualData)
        {
            if (expectedData == null || actualData == null || expectedData.Length != actualData.Length)
            {
                return false;
            }

            for (int i = 0; i < expectedData.Length; ++i)
            {
                var error = expectedData[i] > actualData[i] ? expectedData[i] - actualData[i] : actualData[i] - expectedData[i];
                if (error > BinaryCompareTolerance) // Accept some tolerance for some platforms
                {
                    return false;
                }
            }

            return true;
        }

        private byte[] BuildMockSynthesizedAudio(string text, string language, string voice)
        {
            var ssml = BuildSsml(text, language, voice);
            var ssmlBytes = Encoding.UTF8.GetBytes(ssml);
            var audio = new byte[MockAudioSize + ssmlBytes.Length];
            for (int i = 0; i < MockAudioSize / 2; ++i)
            {
                double x = i * 3.14159265359 * 2 * 400 / 16000; // 400Hz
                double y = Math.Sin(x);

                audio[i * 2] = (byte)((ushort)(y * 16384) % 256);
                audio[i * 2 + 1] = (byte)((ushort)(y * 16384) / 256);
            }

            for (int i = 0; i < ssmlBytes.Length; ++i)
            {
                audio[MockAudioSize + i] = ssmlBytes[i];
            }

            return audio;
        }

        private byte[] MergeBinary(byte[] binary1, byte[] binary2)
        {
            var mergedBinary = new byte[binary1.Length + binary2.Length];
            for (int i = 0; i < binary1.Length; ++i)
            {
                mergedBinary[i] = binary1[i];
            }

            for (int i = 0; i < binary2.Length; ++i)
            {
                mergedBinary[binary1.Length + i] = binary2[i];
            }

            return mergedBinary;
        }

        private byte[] LoadWaveFileData(string waveFile)
        {
            var fileData = File.ReadAllBytes(waveFile);
            if (fileData.Length < EmptyWaveFileSize)
            {
                return new byte[0];
            }

            var waveData = new byte[fileData.Length - EmptyWaveFileSize];
            for (int i = 0; i < fileData.Length - EmptyWaveFileSize; ++i)
            {
                waveData[i] = fileData[EmptyWaveFileSize + i];
            }

            return waveData;
        }

        private string BuildSsml(string text, string language, string voice)
        {
            return string.Format(SsmlTemplate, language, voice, XmlEncode(text));
        }

        private string XmlEncode(string text)
        {
            string ssml = string.Empty;
            foreach (char c in text)
            {
                if (c == '&')
                {
                    ssml += "&amp;";
                }
                else if (c == '<')
                {
                    ssml += "&lt;";
                }
                else if (c == '>')
                {
                    ssml += "&gt;";
                }
                else if (c == '\'')
                {
                    ssml += "&apos;";
                }
                else if (c == '"')
                {
                    ssml += "&quot;";
                }
                else
                {
                    ssml += c;
                }
            }

            return ssml;
        }
    }

    public class PushAudioOutputStreamTestCallback : PushAudioOutputStreamCallback
    {
        private byte[] audioData;
        private bool closed = false;

        public PushAudioOutputStreamTestCallback()
        {
            audioData = new byte[0];
        }

        public override uint Write(byte[] dataBuffer)
        {
            int oldSize = audioData.Length;
            Array.Resize(ref audioData, oldSize + dataBuffer.Length);
            for (int i = 0; i < dataBuffer.Length; ++i)
            {
                audioData[oldSize + i] = dataBuffer[i];
            }

            return (uint)dataBuffer.Length;
        }

        public override void Close()
        {
            closed = true;
        }

        public byte[] GetAudioData()
        {
            return audioData;
        }

        public int GetAudioLength()
        {
            return audioData.Length;
        }

        public bool IsClosed()
        {
            return closed;
        }
    }
}
