//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using Microsoft.CognitiveServices.Speech.Audio;
using Microsoft.CognitiveServices.Speech.Tests.EndToEnd.Utils;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace Microsoft.CognitiveServices.Speech.Tests.EndToEnd
{
    using static SPXTEST;
    using static Config;

    [TestClass]
    public class SpeechSynthesisTests : SynthesisTestBase
    {
        [ClassInitialize]
        public static void TestClassinitialize(TestContext context)
        {
            LoggingTestBaseInit(context);
            BaseClassInit(context);
        }

        [ClassCleanup]
        new public static void TestClassCleanup()
        {
            LoggingTestBaseCleanup();
        }

        [TestInitialize]
        public void Initialize()
        {
        }

        [RetryTestMethod]
        public async Task SynthesisDefaultsREST()
        {
            using (var synthesizer = new SpeechSynthesizer(restConfig))
            {
                using (var result1 = await synthesizer.SpeakTextAsync("{{{text1}}}")) // "{{{text1}}}" has now completed rendering to default speakers
                using (var result2 = await synthesizer.SpeakTextAsync("{{{text2}}}")) // "{{{text2}}}" has now completed rendering to default speakers
                {
                    CheckSynthesisResult(result1);
                    CheckSynthesisResult(result2);
                }
            }
        }

        [RetryTestMethod]
        public async Task ExplicitlyUseDefaultSpeakersREST()
        {
            using (var deviceConfig = AudioConfig.FromDefaultSpeakerOutput())
            {
                using (var synthesizer = new SpeechSynthesizer(restConfig, deviceConfig))
                {
                    using (var result1 = await synthesizer.SpeakTextAsync("{{{text1}}}")) // "{{{text1}}}" has now completed rendering to default speakers
                    using (var result2 = await synthesizer.SpeakTextAsync("{{{text2}}}")) // "{{{text2}}}" has now completed rendering to default speakers
                    {
                        CheckSynthesisResult(result1);
                        CheckSynthesisResult(result2);
                    }
                }
            }
        }

        [RetryTestMethod]
        public async Task PickLanguageREST()
        {
            restConfig.SpeechSynthesisLanguage = "en-GB";
            using (var synthesizer = new SpeechSynthesizer(restConfig))
            {
                using (var result1 = await synthesizer.SpeakTextAsync("{{{text1}}}")) // "{{{text1}}}" has now completed rendering to default speakers
                using (var result2 = await synthesizer.SpeakTextAsync("{{{text2}}}")) // "{{{text2}}}" has now completed rendering to default speakers
                {
                    CheckSynthesisResult(result1);
                    CheckSynthesisResult(result2);
                }
            }
        }

        [RetryTestMethod]
        public async Task PickVoiceREST()
        {
            restConfig.SpeechSynthesisVoiceName = "Microsoft Server Speech Text to Speech Voice (en-GB, HazelRUS)";
            using (var synthesizer = new SpeechSynthesizer(restConfig))
            {
                using (var result1 = await synthesizer.SpeakTextAsync("{{{text1}}}")) // "{{{text1}}}" has now completed rendering to default speakers
                using (var result2 = await synthesizer.SpeakTextAsync("{{{text2}}}")) // "{{{text2}}}" has now completed rendering to default speakers
                {
                    CheckSynthesisResult(result1);
                    CheckSynthesisResult(result2);
                }
            }
        }

        [RetryTestMethod]
        public async Task SynthesizerOutputToFileREST()
        {
            using (var fileConfig = AudioConfig.FromWavFileOutput("wavefile.wav"))
            {
                using (var synthesizer = new SpeechSynthesizer(restConfig, fileConfig))
                {
                    using (var result1 = await synthesizer.SpeakTextAsync("{{{text1}}}")) // "{{{wavefile.wav}}}" now contains synthesized audio for "{{{text1}}}"
                    {
                        CheckSynthesisResult(result1);
                    }
                } // "{{{wavefile.wav}}}" is now closed

                var waveSize1 = new FileInfo("wavefile.wav").Length;
                SPXTEST_ISTRUE(waveSize1 > EmptyWaveFileSize, $"The size of output wave file 1 is unexpected. Expected: greater than {EmptyWaveFileSize}, Actual: {waveSize1}");

                // Make a second run with 2 speaks to verify that the audio can be appended to the file while speaking
                using (var synthesizer = new SpeechSynthesizer(restConfig, fileConfig))
                {
                    using (var result1 = await synthesizer.SpeakTextAsync("{{{text1}}}")) // "{{{wavefile.wav}}}" now contains synthesized audio for "{{{text1}}}"
                    using (var result2 = await synthesizer.SpeakTextAsync("{{{text2}}}")) // "{{{wavefile.wav}}}" now contains synthesized audio for both "{{{text1}}}"" and "{{{text2}}}"
                    {
                        CheckSynthesisResult(result1);
                        CheckSynthesisResult(result2);
                    }
                } // "{{{wavefile.wav}}}" is now closed

                var waveSize2 = new FileInfo("wavefile.wav").Length;
                SPXTEST_ISTRUE(waveSize2 > waveSize1, $"The size of output wave file 1 and wave file 2 are unexpected. Expected: wave size 2 > wave size 1, Actual: wave size 2 = {waveSize2}, wave size 1 = {waveSize1}");
            }
        }

        [RetryTestMethod]
        public async Task SynthesizerOutputToPushStreamREST()
        {
            using (var callback = new PushAudioOutputStreamTestCallback())
            using (var stream = AudioOutputStream.CreatePushStream(callback))
            using (var streamConfig = AudioConfig.FromStreamOutput(stream))
            {
                using (var synthesizer = new SpeechSynthesizer(restConfig, streamConfig))
                {
                    using (var result1 = await synthesizer.SpeakTextAsync("{{{text1}}}")) // "{{{text1}}}" has completed rendering to pushstream
                    using (var result2 = await synthesizer.SpeakTextAsync("{{{text2}}}")) // "{{{text2}}}" has completed rendering to pushstream
                    {
                        CheckSynthesisResult(result1);
                        CheckSynthesisResult(result2);
                    }

                    SPXTEST_ISFALSE(callback.IsClosed(), "The push audio output stream should be opened until synthesizer is released.");
                }

                var audioLength = callback.GetAudioLength();
                SPXTEST_ISTRUE(audioLength > 0, $"The collected audio size should be greater than zero, but actually is {audioLength}.");
                SPXTEST_ISTRUE(callback.IsClosed(), "The push audio output stream should be closed after synthesizer is released.");
            }
        }

        [RetryTestMethod]
        public async Task SynthesizerOutputToPullStreamUseAfterSynthesisCompletedREST()
        {
            using (var stream = AudioOutputStream.CreatePullStream())
            using (var streamConfig = AudioConfig.FromStreamOutput(stream))
            {
                using (var synthesizer = new SpeechSynthesizer(restConfig, streamConfig))
                {
                    using (var result1 = await synthesizer.SpeakTextAsync("{{{text1}}}")) // "{{{text1}}}" has completed rendering to pullstream
                    using (var result2 = await synthesizer.SpeakTextAsync("{{{text2}}}")) // "{{{text2}}}" has completed rendering to pullstream
                    {
                        CheckSynthesisResult(result1);
                        CheckSynthesisResult(result2);
                    }
                }

                DoSomethingWithAudioInPullStream(stream, new bool[] { false });
            }
        }

        [RetryTestMethod]
        public async Task SynthesizerOutputToPullStreamStartUsingBeforeDoneSynthesizingREST()
        {
            using (var stream = AudioOutputStream.CreatePullStream())
            {
                var future = DoSomethingWithAudioInPullStreamInBackground(stream, new bool[] { false });

                using (var streamConfig = AudioConfig.FromStreamOutput(stream))
                using (var synthesizer = new SpeechSynthesizer(restConfig, streamConfig))
                {
                    using (var result1 = await synthesizer.SpeakTextAsync("{{{text1}}}")) // "{{{text1}}}" has completed rendering to pullstream
                    using (var result2 = await synthesizer.SpeakTextAsync("{{{text2}}}")) // "{{{text2}}}" has completed rendering to pullstream
                    {
                        CheckSynthesisResult(result1);
                        CheckSynthesisResult(result2);
                    }
                }

                await future;
            }
        }

        [RetryTestMethod]
        public async Task SpeakOutInResultsREST()
        {
            using (var synthesizer = new SpeechSynthesizer(restConfig, null)) // null indicates to do nothing with synthesizer audio by default
            {
                using (var result1 = await synthesizer.SpeakTextAsync("{{{text1}}}")) // "{{{text1}}}" has completed rendering, and available in result1
                {
                    CheckSynthesisResult(result1);
                    SPXTEST_ARE_EQUAL(GuidLength, result1.ResultId.Length, "The length of result ID should be the length of a GUID (32).");
                    SPXTEST_ARE_EQUAL(ResultReason.SynthesizingAudioCompleted, result1.Reason, "The synthesis should be completed now.");
                    var audioDataSize = result1.AudioData.Length;
                    SPXTEST_ISTRUE(audioDataSize > 0, $"The audio data size should be greater than zero, but actually it's {audioDataSize}.");
                }

                using (var result2 = await synthesizer.SpeakTextAsync("{{{text2}}}")) // "{{{text2}}}" has completed rendering, and available in result2
                {
                    CheckSynthesisResult(result2);
                    SPXTEST_ARE_EQUAL(GuidLength, result2.ResultId.Length, "The length of result ID should be the length of a GUID (32).");
                    SPXTEST_ARE_EQUAL(ResultReason.SynthesizingAudioCompleted, result2.Reason, "The synthesis should be completed now.");
                    var audioDataSize = result2.AudioData.Length;
                    SPXTEST_ISTRUE(audioDataSize > 0, $"The audio data size should be greater than zero, but actually it's {audioDataSize}.");
                }
            }
        }

        [RetryTestMethod]
        public async Task SpeakOutputInChunksInEventSynthesizingREST()
        {
            using (var synthesizer = new SpeechSynthesizer(restConfig, null)) // null indicates to do nothing with synthesizer audio by default
            {
                var resultReasonList = new List<ResultReason>();
                var audioLengthList = new List<int>();
                synthesizer.Synthesizing += (s, e) =>
                {
                    resultReasonList.Add(e.Result.Reason);
                    audioLengthList.Add(e.Result.AudioData.Length);
                };

                using (var result1 = await synthesizer.SpeakTextAsync("{{{text1}}}")) // "{{{text1}}}" has completed rendering
                using (var result2 = await synthesizer.SpeakTextAsync("{{{text2}}}")) // "{{{text2}}}" has completed rendering
                {
                    CheckSynthesisResult(result1);
                    CheckSynthesisResult(result2);
                }

                foreach (var resultReason in resultReasonList)
                {
                    SPXTEST_ARE_EQUAL(ResultReason.SynthesizingAudio, resultReason, "The synthesis should be on going now.");
                }

                foreach (var audioLength in audioLengthList)
                {
                    SPXTEST_ISTRUE(audioLength > 0, $"The audio chunk size should be greater than zero, but actually it's {audioLength}.");
                }
            }
        }

        [RetryTestMethod]
        public async Task SpeakOutputInStreamsREST()
        {
            using (var synthesizer = new SpeechSynthesizer(restConfig, null)) // null indicates to do nothing with synthesizer audio by default
            {
                using (var result1 = await synthesizer.SpeakTextAsync("{{{text1}}}")) // "{{{text1}}}" has completed rendering, and available in result1
                {
                    CheckSynthesisResult(result1);
                    SPXTEST_ARE_EQUAL(ResultReason.SynthesizingAudioCompleted, result1.Reason, "The synthesis should be completed now.");
                    var audioDataSize = result1.AudioData.Length;
                    SPXTEST_ISTRUE(audioDataSize > 0, $"The audio data size should be greater than zero, but actually it's {audioDataSize}.");

                    using (var stream1 = AudioDataStream.FromResult(result1)) // of type AudioDataStream, does not block
                    {
                        DoSomethingWithAudioInDataStream(stream1, true);
                    }
                }

                using (var result2 = await synthesizer.SpeakTextAsync("{{{text2}}}")) // "{{{text2}}}" has completed rendering, and available in result2
                {
                    CheckSynthesisResult(result2);
                    SPXTEST_ARE_EQUAL(ResultReason.SynthesizingAudioCompleted, result2.Reason, "The synthesis should be completed now.");
                    var audioDataSize = result2.AudioData.Length;
                    SPXTEST_ISTRUE(audioDataSize > 0, $"The audio data size should be greater than zero, but actually it's {audioDataSize}.");

                    using (var stream2 = AudioDataStream.FromResult(result2)) // of type AudioDataStream, does not block
                    {
                        DoSomethingWithAudioInDataStream(stream2, true);
                    }
                }
            }
        }

        [RetryTestMethod]
        public async Task SpeakOutputInStreamsBeforeDoneFromEventSynthesisStartedREST()
        {
            using (var synthesizer = new SpeechSynthesizer(restConfig, null)) // null indicates to do nothing with synthesizer audio by default
            {
                var futureList = new List<Task>();
                var streamList = new List<AudioDataStream>();
                var resultReasonList = new List<ResultReason>();
                var audioLengthList = new List<int>();
                synthesizer.SynthesisStarted += (s, e) => {
                    resultReasonList.Add(e.Result.Reason);
                    audioLengthList.Add(e.Result.AudioData.Length);

                    var stream = AudioDataStream.FromResult(e.Result); // of type AudioDataStream, does not block
                    futureList.Add(DoSomethingWithAudioInDataStreamInBackground(stream, false));
                    streamList.Add(stream);
                };

                using (var result1 = await synthesizer.SpeakTextAsync("{{{text1}}}")) // "{{{text1}}}" has completed rendering
                using (var result2 = await synthesizer.SpeakTextAsync("{{{text2}}}")) // "{{{text2}}}" has completed rendering
                {
                    CheckSynthesisResult(result1);
                    CheckSynthesisResult(result2);
                }

                var future3 = synthesizer.SpeakTextAsync("{{{text3}}}"); // "{{{text3}}}" synthesis might have started
                using (var result3 = await future3) // "{{{text3}}}" synthesis has completed
                {
                    CheckSynthesisResult(result3);
                }

                foreach (var future in futureList)
                {
                    await future;
                }

                foreach (var stream in streamList)
                {
                    stream.Dispose();
                }

                foreach (var resultReason in resultReasonList)
                {
                    SPXTEST_ARE_EQUAL(ResultReason.SynthesizingAudioStarted, resultReason, "The synthesis should be started at this point.");
                }

                foreach (var audioLength in audioLengthList)
                {
                    SPXTEST_ARE_EQUAL(0, audioLength, "The synthesized audio should still be zero at this point, since the synthesis just started, and no audio chunk has been synthesized yet.");
                }
            }
        }

        [RetryTestMethod]
        public async Task SpeakOutputInStreamsBeforeDoneFromMethodStartSpeakingTextAsyncREST()
        {
            using (var synthesizer = new SpeechSynthesizer(restConfig, null)) // null indicates to do nothing with synthesizer audio by default
            {
                using (var result1 = await synthesizer.StartSpeakingTextAsync("{{{text1}}}")) // "{{{text1}}}" synthesis has started, likely not finished
                {
                    CheckSynthesisResult(result1);
                    SPXTEST_ARE_EQUAL(ResultReason.SynthesizingAudioStarted, result1.Reason, "The synthesis should be started now.");
                    SPXTEST_ARE_EQUAL(0, result1.AudioData.Length, "The synthesized audio should still be zero at this point, since the synthesis just started, and no audio chunk has been synthesized yet.");

                    using (var stream1 = AudioDataStream.FromResult(result1)) // of type AudioDataStream, does not block
                    {
                        var future1 = DoSomethingWithAudioInDataStreamInBackground(stream1, false); // does not block, just spins a thread up

                        using (var result2 = await synthesizer.StartSpeakingTextAsync("{{{text2}}}")) // "{{{text2}}}" synthesis has started, likely not finished
                        {
                            CheckSynthesisResult(result2);
                            SPXTEST_ARE_EQUAL(ResultReason.SynthesizingAudioStarted, result2.Reason, "The synthesis should be started now.");
                            SPXTEST_ARE_EQUAL(0, result2.AudioData.Length, "The synthesized audio should still be zero at this point, since the synthesis just started, and no audio chunk has been synthesized yet.");

                            using (var stream2 = AudioDataStream.FromResult(result2)) // of type AudioDataStream, does not block
                            {
                                var future2 = DoSomethingWithAudioInDataStreamInBackground(stream2, false); // does not block, just spins a thread up

                                await future1;
                                await future2;
                            }
                        }
                    }
                }
            }
        }

        [RetryTestMethod]
        public async Task SpeakOutputInStreamsBeforeDoneQueuedREST()
        {
            using (var synthesizer = new SpeechSynthesizer(restConfig, null)) // null indicates to do nothing with synthesizer audio by default
            {
                int startedRequests = 0;
                synthesizer.SynthesisStarted += (s, e) =>
                {
                    startedRequests++;
                };

                int completedRequests = 0;
                int startedRequestsWhenFirstRequestWasCompleted = 0;
                synthesizer.SynthesisCompleted += (s, e) =>
                {
                    completedRequests++;
                    if (completedRequests == 1)
                    {
                        startedRequestsWhenFirstRequestWasCompleted = startedRequests;
                    }
                };

                var futureResult1 = synthesizer.StartSpeakingTextAsync("{{{text1}}}"); // "{{{text1}}}" synthesis might have started, likely not finished
                var futureResult2 = synthesizer.StartSpeakingTextAsync("{{{text2}}}"); // "{{{text2}}}" synthesis might have started (very unlikely)

                var future1 = DoSomethingWithAudioInResultInBackground(futureResult1, false); // does not block, just spins a thread up
                var future2 = DoSomethingWithAudioInResultInBackground(futureResult2, false); // does not block, just spins a thread up

                await future1;
                await future2;

                // This is to check the requests is queued
                // When one request is already completed, the other one is still not started
                SPXTEST_ARE_EQUAL(1, startedRequestsWhenFirstRequestWasCompleted, "When one request is already completed, the other one should still not start yet.");
            }
        }

        [RetryTestMethod]
        public async Task SpeakOutputInStreamsWithAllDataGetOnSynthesisStartedResultREST()
        {
            using (var synthesizer = new SpeechSynthesizer(restConfig, null)) // null indicates to do nothing with synthesizer audio by default
            {
                using (var result = await synthesizer.StartSpeakingTextAsync("{{{text1}}}"))
                {
                    CheckSynthesisResult(result);
                    SPXTEST_ARE_EQUAL(ResultReason.SynthesizingAudioStarted, result.Reason, "The synthesis should be started now.");
                    SPXTEST_ARE_EQUAL(0, result.AudioData.Length, "The synthesized audio should still be zero at this point, since the synthesis just started, and no audio chunk has been synthesized yet.");

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
                        Thread.Sleep(100); // wait for the synthesis to be done
                    }

                    using (var stream = AudioDataStream.FromResult(result))
                    {
                        DoSomethingWithAudioInDataStream(stream, true);
                    }
                }
            }
        }


        [RetryTestMethod]
        public async Task SpeakOutWithAuthorizationTokenREST()
        {
            var endpoint = $"https://{region}.tts.speech.microsoft.com/cognitiveservices/v1";
            var configWithInvalidToken = SpeechConfig.FromEndpoint(new Uri(endpoint));
            configWithInvalidToken.AuthorizationToken = "InvalidToken";
            using (var synthesizer = new SpeechSynthesizer(configWithInvalidToken, null)) // null indicates to do nothing with synthesizer audio by default
            {
                SPXTEST_ARE_EQUAL("InvalidToken", synthesizer.AuthorizationToken);

                synthesizer.AuthorizationToken = await GetToken(subscriptionKey, region);

                using (var result1 = await synthesizer.SpeakTextAsync("{{{text1}}}")) // "{{{text1}}}" has completed rendering, and available in result1
                {
                    CheckSynthesisResult(result1);
                    SPXTEST_ARE_EQUAL(GuidLength, result1.ResultId.Length, "The length of result ID should be the length of a GUID (32).");
                    SPXTEST_ARE_EQUAL(ResultReason.SynthesizingAudioCompleted, result1.Reason, "The synthesis should be completed now.");
                    var audioDataSize = result1.AudioData.Length;
                    SPXTEST_ISTRUE(audioDataSize > 0, $"The audio data size should be greater than zero, but actually it's {audioDataSize}.");
                }

                using (var result2 = await synthesizer.SpeakTextAsync("{{{text2}}}")) // "{{{text2}}}" has completed rendering, and available in result2
                {
                    CheckSynthesisResult(result2);
                    SPXTEST_ARE_EQUAL(GuidLength, result2.ResultId.Length, "The length of result ID should be the length of a GUID (32).");
                    SPXTEST_ARE_EQUAL(ResultReason.SynthesizingAudioCompleted, result2.Reason, "The synthesis should be completed now.");
                    var audioDataSize = result2.AudioData.Length;
                    SPXTEST_ISTRUE(audioDataSize > 0, $"The audio data size should be greater than zero, but actually it's {audioDataSize}.");
                }
            }
        }


        [RetryTestMethod]
        public async Task SynthesisDefaultsUsp()
        {
            using (var synthesizer = new SpeechSynthesizer(uspConfig))
            {
                using (var result1 = await synthesizer.SpeakTextAsync("{{{text1}}}")) // "{{{text1}}}" has now completed rendering to default speakers
                using (var result2 = await synthesizer.SpeakTextAsync("{{{text2}}}")) // "{{{text2}}}" has now completed rendering to default speakers
                {
                    CheckSynthesisResult(result1);
                    CheckSynthesisResult(result2);
                }
            }
        }

        [RetryTestMethod]
        public async Task ExplicitlyUseDefaultSpeakersUsp()
        {
            using (var deviceConfig = AudioConfig.FromDefaultSpeakerOutput())
            {
                using (var synthesizer = new SpeechSynthesizer(uspConfig, deviceConfig))
                {
                    using (var result1 = await synthesizer.SpeakTextAsync("{{{text1}}}")) // "{{{text1}}}" has now completed rendering to default speakers
                    using (var result2 = await synthesizer.SpeakTextAsync("{{{text2}}}")) // "{{{text2}}}" has now completed rendering to default speakers
                    {
                        CheckSynthesisResult(result1);
                        CheckSynthesisResult(result2);
                    }
                }
            }
        }

        [RetryTestMethod]
        public async Task PickLanguageUsp()
        {
            uspConfig.SpeechSynthesisLanguage = "en-GB";
            using (var synthesizer = new SpeechSynthesizer(uspConfig))
            {
                using (var result1 = await synthesizer.SpeakTextAsync("{{{text1}}}")) // "{{{text1}}}" has now completed rendering to default speakers
                using (var result2 = await synthesizer.SpeakTextAsync("{{{text2}}}")) // "{{{text2}}}" has now completed rendering to default speakers
                {
                    CheckSynthesisResult(result1);
                    CheckSynthesisResult(result2);
                }
            }
        }

        [RetryTestMethod]
        public async Task PickVoiceUsp()
        {
            uspConfig.SpeechSynthesisVoiceName = "Microsoft Server Speech Text to Speech Voice (en-GB, HazelRUS)";
            using (var synthesizer = new SpeechSynthesizer(uspConfig))
            {
                using (var result1 = await synthesizer.SpeakTextAsync("{{{text1}}}")) // "{{{text1}}}" has now completed rendering to default speakers
                using (var result2 = await synthesizer.SpeakTextAsync("{{{text2}}}")) // "{{{text2}}}" has now completed rendering to default speakers
                {
                    CheckSynthesisResult(result1);
                    CheckSynthesisResult(result2);
                }
            }
        }
        
        [RetryTestMethod]
        public async Task SynthesisWithMultiVoicesUsp()
        {
            using (var synthesizer = new SpeechSynthesizer(uspConfig, null))
            {
                var ssmlWithMultiVoices = @"<speak version='1.0' xmlns='https://www.w3.org/2001/10/synthesis' xml:lang='en-US'>
                    <voice  name='en-US-AriaRUS'>Good morning!</voice>
                    <voice  name='en-US-BenjaminRUS'>Good morning to you too Aria!</voice></speak>";
                using (var result = await synthesizer.SpeakSsmlAsync(ssmlWithMultiVoices)) 
                {
                    CheckSynthesisResult(result);
                }
            }
        }

        [RetryTestMethod]
        public async Task SynthesisWithRecordedAudioUsp()
        {
            using (var synthesizer = new SpeechSynthesizer(uspConfig, null))
            {
                var ssmlWithRecordedAudio = @"<speak version='1.0' xml:lang='en-US' xmlns='http://www.w3.org/2001/10/synthesis' 
                    xmlns:mstts='http://www.w3.org/2001/mstts'><voice name='Microsoft Server Speech Text to Speech Voice (en-US, AriaRUS)'>
                    <audio src='https://speechprobesstorage.blob.core.windows.net/ttsaudios/pcm16.wav'/>text</voice></speak>";
                using (var result = await synthesizer.SpeakSsmlAsync(ssmlWithRecordedAudio)) 
                {
                    CheckSynthesisResult(result);
                }
            }
        }

        [RetryTestMethod]
        public async Task SynthesizerOutputToFileUsp()
        {
            using (var fileConfig = AudioConfig.FromWavFileOutput("wavefile.wav"))
            {
                using (var synthesizer = new SpeechSynthesizer(uspConfig, fileConfig))
                {
                    using (var result1 = await synthesizer.SpeakTextAsync("{{{text1}}}")) // "{{{wavefile.wav}}}" now contains synthesized audio for "{{{text1}}}"
                    {
                        CheckSynthesisResult(result1);
                    }
                } // "{{{wavefile.wav}}}" is now closed

                var waveSize1 = new FileInfo("wavefile.wav").Length;
                SPXTEST_ISTRUE(waveSize1 > EmptyWaveFileSize, $"The size of output wave file 1 is unexpected. Expected: greater than {EmptyWaveFileSize}, Actual: {waveSize1}");

                // Make a second run with 2 speaks to verify that the audio can be appended to the file while speaking
                using (var synthesizer = new SpeechSynthesizer(uspConfig, fileConfig))
                {
                    using (var result1 = await synthesizer.SpeakTextAsync("{{{text1}}}")) // "{{{wavefile.wav}}}" now contains synthesized audio for "{{{text1}}}"
                    using (var result2 = await synthesizer.SpeakTextAsync("{{{text2}}}")) // "{{{wavefile.wav}}}" now contains synthesized audio for both "{{{text1}}}"" and "{{{text2}}}"
                    {
                        CheckSynthesisResult(result1);
                        CheckSynthesisResult(result2);
                    }
                } // "{{{wavefile.wav}}}" is now closed

                var waveSize2 = new FileInfo("wavefile.wav").Length;
                SPXTEST_ISTRUE(waveSize2 > waveSize1, $"The size of output wave file 1 and wave file 2 are unexpected. Expected: wave size 2 > wave size 1, Actual: wave size 2 = {waveSize2}, wave size 1 = {waveSize1}");
            }
        }

        [RetryTestMethod]
        public void SynthesizerOutputToInvalidFileUsp()
        {
            var fileConfig = AudioConfig.FromWavFileOutput(Path.Combine("NonexistentDir", "InvalidFile"));
            SPXTEST_THROWS<ApplicationException>(() => new SpeechSynthesizer(uspConfig, fileConfig));
        }

        [RetryTestMethod]
        public async Task SynthesizerOutputToPushStreamUsp()
        {
            using (var callback = new PushAudioOutputStreamTestCallback())
            using (var stream = AudioOutputStream.CreatePushStream(callback))
            using (var streamConfig = AudioConfig.FromStreamOutput(stream))
            {
                using (var synthesizer = new SpeechSynthesizer(uspConfig, streamConfig))
                {
                    using (var result1 = await synthesizer.SpeakTextAsync("{{{text1}}}")) // "{{{text1}}}" has completed rendering to pushstream
                    using (var result2 = await synthesizer.SpeakTextAsync("{{{text2}}}")) // "{{{text2}}}" has completed rendering to pushstream
                    {
                        CheckSynthesisResult(result1);
                        CheckSynthesisResult(result2);
                    }

                    SPXTEST_ISFALSE(callback.IsClosed(), "The push audio output stream should be opened until synthesizer is released.");
                }

                var audioLength = callback.GetAudioLength();
                SPXTEST_ISTRUE(audioLength > 0, $"The collected audio size should be greater than zero, but actually is {audioLength}.");
                SPXTEST_ISTRUE(callback.IsClosed(), "The push audio output stream should be closed after synthesizer is released.");
            }
        }

        [RetryTestMethod]
        public async Task SynthesizerOutputToPushStreamAfterAudioConfigDisposeUsp()
        {
            using (var callback = new PushAudioOutputStreamTestCallback())
            {
                var streamConfig = AudioConfig.FromStreamOutput(callback);
                var synthesizer = new SpeechSynthesizer(uspConfig, streamConfig);
                streamConfig.Dispose();
                using (var result = await synthesizer.SpeakTextAsync("{{{text1}}}")) // "{{{text1}}}" has completed rendering to pushstream
                {
                    CheckSynthesisResult(result);
                }

                synthesizer.Dispose();
                var audioLength = callback.GetAudioLength();
                SPXTEST_ISTRUE(audioLength > 0, $"The collected audio size should be greater than zero, but actually is {audioLength}.");
            }
        }

        [RetryTestMethod]
        public async Task SynthesizerOutputToPullStreamUseAfterSynthesisCompletedUsp()
        {
            using (var stream = AudioOutputStream.CreatePullStream())
            using (var streamConfig = AudioConfig.FromStreamOutput(stream))
            {
                using (var synthesizer = new SpeechSynthesizer(uspConfig, streamConfig))
                {
                    using (var result1 = await synthesizer.SpeakTextAsync("{{{text1}}}")) // "{{{text1}}}" has completed rendering to pullstream
                    using (var result2 = await synthesizer.SpeakTextAsync("{{{text2}}}")) // "{{{text2}}}" has completed rendering to pullstream
                    {
                        CheckSynthesisResult(result1);
                        CheckSynthesisResult(result2);
                    }
                }

                DoSomethingWithAudioInPullStream(stream, new bool[] { false });
            }
        }

        [RetryTestMethod]
        public async Task SynthesizerOutputToPullStreamStartUsingBeforeDoneSynthesizingUsp()
        {
            using (var stream = AudioOutputStream.CreatePullStream())
            {
                var future = DoSomethingWithAudioInPullStreamInBackground(stream, new bool[] { false });

                using (var streamConfig = AudioConfig.FromStreamOutput(stream))
                using (var synthesizer = new SpeechSynthesizer(uspConfig, streamConfig))
                {
                    using (var result1 = await synthesizer.SpeakTextAsync("{{{text1}}}")) // "{{{text1}}}" has completed rendering to pullstream
                    using (var result2 = await synthesizer.SpeakTextAsync("{{{text2}}}")) // "{{{text2}}}" has completed rendering to pullstream
                    {
                        CheckSynthesisResult(result1);
                        CheckSynthesisResult(result2);
                    }
                }

                await future;
            }
        }

        [RetryTestMethod]
        public async Task SpeakOutInResultsUsp()
        {
            using (var synthesizer = new SpeechSynthesizer(uspConfig, null)) // null indicates to do nothing with synthesizer audio by default
            {
                using (var result1 = await synthesizer.SpeakTextAsync("{{{text1}}}")) // "{{{text1}}}" has completed rendering, and available in result1
                {
                    CheckSynthesisResult(result1);
                    SPXTEST_ARE_EQUAL(GuidLength, result1.ResultId.Length, "The length of result ID should be the length of a GUID (32).");
                    SPXTEST_ARE_EQUAL(ResultReason.SynthesizingAudioCompleted, result1.Reason, "The synthesis should be completed now.");
                    var audioDataSize = result1.AudioData.Length;
                    SPXTEST_ISTRUE(audioDataSize > 0, $"The audio data size should be greater than zero, but actually it's {audioDataSize}.");
                }

                using (var result2 = await synthesizer.SpeakTextAsync("{{{text2}}}")) // "{{{text2}}}" has completed rendering, and available in result2
                {
                    CheckSynthesisResult(result2);
                    SPXTEST_ARE_EQUAL(GuidLength, result2.ResultId.Length, "The length of result ID should be the length of a GUID (32).");
                    SPXTEST_ARE_EQUAL(ResultReason.SynthesizingAudioCompleted, result2.Reason, "The synthesis should be completed now.");
                    var audioDataSize = result2.AudioData.Length;
                    SPXTEST_ISTRUE(audioDataSize > 0, $"The audio data size should be greater than zero, but actually it's {audioDataSize}.");
                }
            }
        }

        [RetryTestMethod]
        public async Task SpeakOutputInChunksInEventSynthesizingUsp()
        {
            using (var synthesizer = new SpeechSynthesizer(uspConfig, null)) // null indicates to do nothing with synthesizer audio by default
            {
                var resultReasonList = new List<ResultReason>();
                var audioLengthList = new List<int>();
                synthesizer.Synthesizing += (s, e) =>
                {
                    resultReasonList.Add(e.Result.Reason);
                    audioLengthList.Add(e.Result.AudioData.Length);
                };

                using (var result1 = await synthesizer.SpeakTextAsync("{{{text1}}}")) // "{{{text1}}}" has completed rendering
                using (var result2 = await synthesizer.SpeakTextAsync("{{{text2}}}")) // "{{{text2}}}" has completed rendering
                {
                    CheckSynthesisResult(result1);
                    CheckSynthesisResult(result2);
                }

                foreach (var resultReason in resultReasonList)
                {
                    SPXTEST_ARE_EQUAL(ResultReason.SynthesizingAudio, resultReason, "The synthesis should be on going now.");
                }

                foreach (var audioLength in audioLengthList)
                {
                    SPXTEST_ISTRUE(audioLength > 0, $"The audio chunk size should be greater than zero, but actually it's {audioLength}.");
                }
            }
        }

        [RetryTestMethod]
        public async Task SpeakOutputInStreamsUsp()
        {
            using (var synthesizer = new SpeechSynthesizer(uspConfig, null)) // null indicates to do nothing with synthesizer audio by default
            {
                using (var result1 = await synthesizer.SpeakTextAsync("{{{text1}}}")) // "{{{text1}}}" has completed rendering, and available in result1
                {
                    CheckSynthesisResult(result1);
                    SPXTEST_ARE_EQUAL(ResultReason.SynthesizingAudioCompleted, result1.Reason, "The synthesis should be completed now.");
                    var audioDataSize = result1.AudioData.Length;
                    SPXTEST_ISTRUE(audioDataSize > 0, $"The audio data size should be greater than zero, but actually it's {audioDataSize}.");

                    using (var stream1 = AudioDataStream.FromResult(result1)) // of type AudioDataStream, does not block
                    {
                        DoSomethingWithAudioInDataStream(stream1, true);
                    }
                }

                using (var result2 = await synthesizer.SpeakTextAsync("{{{text2}}}")) // "{{{text2}}}" has completed rendering, and available in result2
                {
                    CheckSynthesisResult(result2);
                    SPXTEST_ARE_EQUAL(ResultReason.SynthesizingAudioCompleted, result2.Reason, "The synthesis should be completed now.");
                    var audioDataSize = result2.AudioData.Length;
                    SPXTEST_ISTRUE(audioDataSize > 0, $"The audio data size should be greater than zero, but actually it's {audioDataSize}.");

                    using (var stream2 = AudioDataStream.FromResult(result2)) // of type AudioDataStream, does not block
                    {
                        DoSomethingWithAudioInDataStream(stream2, true);
                    }
                }
            }
        }

        [RetryTestMethod]
        public async Task SpeakOutputInStreamsBeforeDoneFromEventSynthesisStartedUsp()
        {
            using (var synthesizer = new SpeechSynthesizer(uspConfig, null)) // null indicates to do nothing with synthesizer audio by default
            {
                var futureList = new List<Task>();
                var streamList = new List<AudioDataStream>();
                var resultReasonList = new List<ResultReason>();
                var audioLengthList = new List<int>();
                synthesizer.SynthesisStarted += (s, e) => {
                    resultReasonList.Add(e.Result.Reason);
                    audioLengthList.Add(e.Result.AudioData.Length);

                    var stream = AudioDataStream.FromResult(e.Result); // of type AudioDataStream, does not block
                    futureList.Add(DoSomethingWithAudioInDataStreamInBackground(stream, false));
                    streamList.Add(stream);
                };

                using (var result1 = await synthesizer.SpeakTextAsync("{{{text1}}}")) // "{{{text1}}}" has completed rendering
                using (var result2 = await synthesizer.SpeakTextAsync("{{{text2}}}")) // "{{{text2}}}" has completed rendering
                {
                    CheckSynthesisResult(result1);
                    CheckSynthesisResult(result2);
                }

                var future3 = synthesizer.SpeakTextAsync("{{{text3}}}"); // "{{{text3}}}" synthesis might have started
                using (var result3 = await future3) // "{{{text3}}}" synthesis has completed
                {
                    CheckSynthesisResult(result3);
                }

                foreach (var future in futureList)
                {
                    await future;
                }

                foreach (var stream in streamList)
                {
                    stream.Dispose();
                }

                foreach (var resultReason in resultReasonList)
                {
                    SPXTEST_ARE_EQUAL(ResultReason.SynthesizingAudioStarted, resultReason, "The synthesis should be started at this point.");
                }

                foreach (var audioLength in audioLengthList)
                {
                    SPXTEST_ARE_EQUAL(0, audioLength, "The synthesized audio should still be zero at this point, since the synthesis just started, and no audio chunk has been synthesized yet.");
                }
            }
        }

        [RetryTestMethod]
        public async Task SpeakOutputInStreamsBeforeDoneFromMethodStartSpeakingTextAsyncUsp()
        {
            using (var synthesizer = new SpeechSynthesizer(uspConfig, null)) // null indicates to do nothing with synthesizer audio by default
            {
                using (var result1 = await synthesizer.StartSpeakingTextAsync("{{{text1}}}")) // "{{{text1}}}" synthesis has started, likely not finished
                {
                    CheckSynthesisResult(result1);
                    SPXTEST_ARE_EQUAL(ResultReason.SynthesizingAudioStarted, result1.Reason, "The synthesis should be started now.");
                    SPXTEST_ARE_EQUAL(0, result1.AudioData.Length, "The synthesized audio should still be zero at this point, since the synthesis just started, and no audio chunk has been synthesized yet.");

                    using (var stream1 = AudioDataStream.FromResult(result1)) // of type AudioDataStream, does not block
                    {
                        var future1 = DoSomethingWithAudioInDataStreamInBackground(stream1, false); // does not block, just spins a thread up

                        using (var result2 = await synthesizer.StartSpeakingTextAsync("{{{text2}}}")) // "{{{text2}}}" synthesis has started, likely not finished
                        {
                            CheckSynthesisResult(result2);
                            SPXTEST_ARE_EQUAL(ResultReason.SynthesizingAudioStarted, result2.Reason, "The synthesis should be started now.");
                            SPXTEST_ARE_EQUAL(0, result2.AudioData.Length, "The synthesized audio should still be zero at this point, since the synthesis just started, and no audio chunk has been synthesized yet.");

                            using (var stream2 = AudioDataStream.FromResult(result2)) // of type AudioDataStream, does not block
                            {
                                var future2 = DoSomethingWithAudioInDataStreamInBackground(stream2, false); // does not block, just spins a thread up

                                await future1;
                                await future2;
                            }
                        }
                    }
                }
            }
        }

        [RetryTestMethod]
        public async Task SpeakOutputInStreamsBeforeDoneQueuedUsp()
        {
            using (var synthesizer = new SpeechSynthesizer(uspConfig, null)) // null indicates to do nothing with synthesizer audio by default
            {
                int startedRequests = 0;
                synthesizer.SynthesisStarted += (s, e) =>
                {
                    startedRequests++;
                };

                int completedRequests = 0;
                int startedRequestsWhenFirstRequestWasCompleted = 0;
                synthesizer.SynthesisCompleted += (s, e) =>
                {
                    completedRequests++;
                    if (completedRequests == 1)
                    {
                        startedRequestsWhenFirstRequestWasCompleted = startedRequests;
                    }
                };

                var futureResult1 = synthesizer.StartSpeakingTextAsync("{{{text1}}}"); // "{{{text1}}}" synthesis might have started, likely not finished
                var futureResult2 = synthesizer.StartSpeakingTextAsync("{{{text2}}}"); // "{{{text2}}}" synthesis might have started (very unlikely)

                var future1 = DoSomethingWithAudioInResultInBackground(futureResult1, false); // does not block, just spins a thread up
                var future2 = DoSomethingWithAudioInResultInBackground(futureResult2, false); // does not block, just spins a thread up

                await future1;
                await future2;

                // This is to check the requests is queued
                // When one request is already completed, the other one is still not started
                SPXTEST_ARE_EQUAL(1, startedRequestsWhenFirstRequestWasCompleted, "When one request is already completed, the other one should still not start yet.");
            }
        }

        [RetryTestMethod]
        public async Task SpeakOutputInStreamsWithAllDataGetOnSynthesisStartedResultUsp()
        {
            using (var synthesizer = new SpeechSynthesizer(uspConfig, null)) // null indicates to do nothing with synthesizer audio by default
            {
                using (var result = await synthesizer.StartSpeakingTextAsync("{{{text1}}}"))
                {
                    CheckSynthesisResult(result);
                    SPXTEST_ARE_EQUAL(ResultReason.SynthesizingAudioStarted, result.Reason, "The synthesis should be started now.");
                    SPXTEST_ARE_EQUAL(0, result.AudioData.Length, "The synthesized audio should still be zero at this point, since the synthesis just started, and no audio chunk has been synthesized yet.");

                    bool synthesisDone = false;
                    synthesizer.SynthesisCompleted += (s, e) =>
                    {
                        synthesisDone = true;
                    };

                    synthesizer.SynthesisCanceled += (s, e) =>
                    {
                        synthesisDone = true;
                    };

                    while (!synthesisDone)
                    {
                        Thread.Sleep(100); // wait for the synthesis to be done
                    }

                    using (var stream = AudioDataStream.FromResult(result))
                    {
                        DoSomethingWithAudioInDataStream(stream, true);
                    }
                }
            }
        }

        [RetryTestMethod]
        public async Task SpeakOutWithAuthorizationTokenUsp()
        {
            var configWithInvalidToken = SpeechConfig.FromAuthorizationToken("InvalidToken", region);
            using (var synthesizer = new SpeechSynthesizer(configWithInvalidToken, null)) // null indicates to do nothing with synthesizer audio by default
            {
                SPXTEST_ARE_EQUAL("InvalidToken", synthesizer.AuthorizationToken);

                synthesizer.AuthorizationToken = await GetToken(subscriptionKey, region);

                using (var result1 = await synthesizer.SpeakTextAsync("{{{text1}}}")) // "{{{text1}}}" has completed rendering, and available in result1
                {
                    CheckSynthesisResult(result1);
                    SPXTEST_ARE_EQUAL(GuidLength, result1.ResultId.Length, "The length of result ID should be the length of a GUID (32).");
                    SPXTEST_ARE_EQUAL(ResultReason.SynthesizingAudioCompleted, result1.Reason, "The synthesis should be completed now.");
                    var audioDataSize = result1.AudioData.Length;
                    SPXTEST_ISTRUE(audioDataSize > 0, $"The audio data size should be greater than zero, but actually it's {audioDataSize}.");
                }

                using (var result2 = await synthesizer.SpeakTextAsync("{{{text2}}}")) // "{{{text2}}}" has completed rendering, and available in result2
                {
                    CheckSynthesisResult(result2);
                    SPXTEST_ARE_EQUAL(GuidLength, result2.ResultId.Length, "The length of result ID should be the length of a GUID (32).");
                    SPXTEST_ARE_EQUAL(ResultReason.SynthesizingAudioCompleted, result2.Reason, "The synthesis should be completed now.");
                    var audioDataSize = result2.AudioData.Length;
                    SPXTEST_ISTRUE(audioDataSize > 0, $"The audio data size should be greater than zero, but actually it's {audioDataSize}.");
                }
            }
        }

        [RetryTestMethod]
        public async Task SynthesisFirstChunkTimeoutUsp()
        {
            using (var synthesizer = new SpeechSynthesizer(uspConfig, null))
            {
                // set timeout threshold to 0
                synthesizer.Properties.SetProperty("SpeechSynthesis_FirstChunkTimeoutMs", "0");

                // use a long sentence here
                using (var result = await synthesizer.SpeakTextAsync("I have a dream that one day this nation will rise up and live out the true meaning of its creed."))
                {
                    SPXTEST_ARE_EQUAL(ResultReason.Canceled, result.Reason);
                    var cancellation = SpeechSynthesisCancellationDetails.FromResult(result);
                    AssertHelpers.AssertStringContains(cancellation.ErrorDetails, "timeout");
                    SPXTEST_ARE_EQUAL(result.AudioData.Length, 0);
                }
            }
        }

        [RetryTestMethod]
        public async Task SynthesisAllChunkTimeoutUsp()
        {
            using (var synthesizer = new SpeechSynthesizer(uspConfig, null))
            {
                // set timeout threshold to 0
                synthesizer.Properties.SetProperty("SpeechSynthesis_AllChunkTimeoutMs", "0");

                // use a long sentence here
                using (var result = await synthesizer.SpeakTextAsync("I have a dream that one day this nation will rise up and live out the true meaning of its creed."))
                {
                    SPXTEST_ARE_EQUAL(ResultReason.Canceled, result.Reason);
                    var cancellation = SpeechSynthesisCancellationDetails.FromResult(result);
                    AssertHelpers.AssertStringContains(cancellation.ErrorDetails, "timeout");
                    SPXTEST_ISTRUE(result.AudioData.Length > 0);
                }
            }
        }

        [RetryTestMethod]
        public async Task CheckWordBoundaryEventsUsp()
        {
            uspConfig.SpeechSynthesisVoiceName = "Microsoft Server Speech Text to Speech Voice (zh-CN, HuihuiRUS)";
            using (var synthesizer = new SpeechSynthesizer(uspConfig, null))
            {
                string plainText = "您好，我是来自Microsoft的中文声音。";
                string ssml = "<speak version='1.0' xmlns='http://www.w3.org/2001/10/synthesis' xmlns:mstts='http://www.w3.org/2001/mstts' xmlns:emo='http://www.w3.org/2009/10/emotionml' xml:lang='zh-CN'><voice name='Microsoft Server Speech Text to Speech Voice (zh-CN, HuihuiRUS)'>您好，<break time='50ms'/>我是来自Microsoft的中文声音。</voice></speak>";

                UInt32[] expectedTextOffsets = { 0, 3, 4, 5, 7, 16, 17, 19 };
                UInt32[] expectedSsmlOffsets = { 251, 274, 275, 276, 278, 287, 288, 290 };
                UInt32[] expectedWordLengths = { 2, 1, 1, 2, 9, 1, 2, 2 };

                var actualAudioOffsets = new List<UInt64>();
                var actualTextOffsets = new List<UInt32>();
                var actualSsmlOffsets = new List<UInt32>();
                var actualWordLengths = new List<UInt32>();

                EventHandler<SpeechSynthesisWordBoundaryEventArgs> wordBoundaryEventHandler = (s, e) =>
                {
                    actualAudioOffsets.Add(e.AudioOffset);
                    actualTextOffsets.Add(e.TextOffset);
                    actualWordLengths.Add(e.WordLength);
                };

                synthesizer.WordBoundary += wordBoundaryEventHandler;

                using (var result = await synthesizer.SpeakTextAsync(plainText))
                {
                    CheckSynthesisResult(result);
                }

                SPXTEST_ARE_EQUAL(expectedTextOffsets.Length, actualAudioOffsets.Count, "Incorrect count of word boundary events.");
                for (var i = 0; i < expectedTextOffsets.Length; ++i)
                {
                    SPXTEST_ISTRUE(actualAudioOffsets[i] > 0, "Audio offset should be greater than zero.");
                    SPXTEST_ARE_EQUAL(expectedTextOffsets[i], actualTextOffsets[i], $"Text offset mismatch on word #{i + 1}.");
                    SPXTEST_ARE_EQUAL(expectedWordLengths[i], actualWordLengths[i], $"Word length mismatch on word #{i + 1}.");
                }

                synthesizer.WordBoundary -= wordBoundaryEventHandler;
                actualAudioOffsets.Clear();
                actualTextOffsets.Clear();
                actualWordLengths.Clear();

                wordBoundaryEventHandler = (s, e) =>
                {
                    actualAudioOffsets.Add(e.AudioOffset);
                    actualSsmlOffsets.Add(e.TextOffset);
                    actualWordLengths.Add(e.WordLength);
                };

                synthesizer.WordBoundary += wordBoundaryEventHandler;

                using (var result = await synthesizer.SpeakSsmlAsync(ssml))
                {
                    CheckSynthesisResult(result);
                }

                SPXTEST_ARE_EQUAL(expectedTextOffsets.Length, actualAudioOffsets.Count, "Incorrect count of word boundary events.");
                for (var i = 0; i < expectedTextOffsets.Length; ++i)
                {
                    SPXTEST_ISTRUE(actualAudioOffsets[i] > 0, "Audio offset should be greater than zero.");
                    SPXTEST_ARE_EQUAL(expectedSsmlOffsets[i], actualSsmlOffsets[i], $"Ssml offset mismatch on word #{i + 1}.");
                    SPXTEST_ARE_EQUAL(expectedWordLengths[i], actualWordLengths[i], $"Word length mismatch on word #{i + 1}.");
                }
            }
        }

        [RetryTestMethod]
        public async Task SynthesisWithLanguageAutoDetectionUsp()
        {
            var autoDetectSourceLanguageConfig = AutoDetectSourceLanguageConfig.FromOpenRange();
            using (var synthesizer = new SpeechSynthesizer(uspConfig, autoDetectSourceLanguageConfig, null as AudioConfig))
            {
                uint lastTextOffset = 0;
                synthesizer.WordBoundary += (s, e) => { lastTextOffset = e.TextOffset; };
                using (var result1 = await synthesizer.SpeakTextAsync(AudioUtterancesMap[AudioUtteranceKeys.SYNTHESIS_UTTERANCE_CHINESE_1].Utterances["zh-CN"][0].Text))
                {
                    CheckSynthesisResult(result1, 32000); // should longer than 1s
                    SPXTEST_ISTRUE(lastTextOffset >= AudioUtterancesMap[AudioUtteranceKeys.SYNTHESIS_UTTERANCE_CHINESE_1].Utterances["zh-CN"][0].TextOffsets.Last());
                }
                lastTextOffset = 0;
                using (var result2 = await synthesizer.SpeakTextAsync(AudioUtterancesMap[AudioUtteranceKeys.SYNTHESIS_UTTERANCE_CHINESE_2].Utterances["zh-CN"][0].Text))
                {
                    CheckSynthesisResult(result2, 32000); // should longer than 1s
                    SPXTEST_ISTRUE(lastTextOffset >= AudioUtterancesMap[AudioUtteranceKeys.SYNTHESIS_UTTERANCE_CHINESE_2].Utterances["zh-CN"][0].TextOffsets.Last());
                }
            }
        }

        [RetryTestMethod]
        public async Task StopSynthesisUsp()
        {
            using (var synthesizer = new SpeechSynthesizer(uspConfig, null as AudioConfig))
            {
                var task1 = synthesizer.SpeakTextAsync("text1");
                Thread.Sleep(100);
                await synthesizer.StopSpeakingAsync();
                using (var result1 = task1.Result)
                {
                    SPXTEST_ARE_EQUAL(ResultReason.Canceled, result1.Reason);
                    var cancelDetails = SpeechSynthesisCancellationDetails.FromResult(result1);
                    SPXTEST_ARE_EQUAL(CancellationReason.CancelledByUser, cancelDetails.Reason);
                }

                using (var result2 = await synthesizer.SpeakTextAsync("text2"))
                {
                    CheckSynthesisResult(result2);
                }
            }
        }

        [RetryTestMethod]
        [TestCategory("SpeechSynthesisMockTest")]
        public async Task SynthesisDefaultsMock()
        {
            using (var synthesizer = new SpeechSynthesizer(mockConfig))
            {
                using (var result1 = await synthesizer.SpeakTextAsync("{{{text1}}}")) // "{{{text1}}}" has now completed rendering to default speakers
                using (var result2 = await synthesizer.SpeakTextAsync("{{{text2}}}")) // "{{{text2}}}" has now completed rendering to default speakers
                {
                    CheckSynthesisResult(result1);
                    CheckSynthesisResult(result2);

                    var expectedAudioData1 = BuildMockSynthesizedAudioWithHeader("{{{text1}}}", DefaultLanguage, DefaultVoice);
                    var expectedAudioData2 = BuildMockSynthesizedAudioWithHeader("{{{text2}}}", DefaultLanguage, DefaultVoice);

                    CheckBinaryEqual(expectedAudioData1, result1.AudioData);
                    CheckBinaryEqual(expectedAudioData2, result2.AudioData);
                }
            }
        }

        [RetryTestMethod]
        [TestCategory("SpeechSynthesisMockTest")]
        public async Task ExplicitlyUseDefaultSpeakersMock()
        {
            using (var deviceConfig = AudioConfig.FromDefaultSpeakerOutput())
            {
                using (var synthesizer = new SpeechSynthesizer(mockConfig, deviceConfig))
                {
                    using (var result1 = await synthesizer.SpeakTextAsync("{{{text1}}}")) // "{{{text1}}}" has now completed rendering to default speakers
                    using (var result2 = await synthesizer.SpeakTextAsync("{{{text2}}}")) // "{{{text2}}}" has now completed rendering to default speakers
                    {
                        CheckSynthesisResult(result1);
                        CheckSynthesisResult(result2);

                        var expectedAudioData1 = BuildMockSynthesizedAudioWithHeader("{{{text1}}}", DefaultLanguage, DefaultVoice);
                        var expectedAudioData2 = BuildMockSynthesizedAudioWithHeader("{{{text2}}}", DefaultLanguage, DefaultVoice);

                        CheckBinaryEqual(expectedAudioData1, result1.AudioData);
                        CheckBinaryEqual(expectedAudioData2, result2.AudioData);
                    }
                }
            }
        }

        [RetryTestMethod]
        [TestCategory("SpeechSynthesisMockTest")]
        public async Task PickLanguageMock()
        {
            mockConfig.SpeechSynthesisLanguage = "en-GB";
            using (var synthesizer = new SpeechSynthesizer(mockConfig))
            {
                using (var result1 = await synthesizer.SpeakTextAsync("{{{text1}}}")) // "{{{text1}}}" has now completed rendering to default speakers
                using (var result2 = await synthesizer.SpeakTextAsync("{{{text2}}}")) // "{{{text2}}}" has now completed rendering to default speakers
                {
                    CheckSynthesisResult(result1);
                    CheckSynthesisResult(result2);

                    var expectedAudioData1 = BuildMockSynthesizedAudioWithHeader("{{{text1}}}", "en-GB", "Microsoft Server Speech Text to Speech Voice (en-GB, HazelRUS)");
                    var expectedAudioData2 = BuildMockSynthesizedAudioWithHeader("{{{text2}}}", "en-GB", "Microsoft Server Speech Text to Speech Voice (en-GB, HazelRUS)");

                    CheckBinaryEqual(expectedAudioData1, result1.AudioData);
                    CheckBinaryEqual(expectedAudioData2, result2.AudioData);
                }
            }
        }

        [RetryTestMethod]
        [TestCategory("SpeechSynthesisMockTest")]
        public async Task PickVoiceMock()
        {
            mockConfig.SpeechSynthesisVoiceName = "Microsoft Server Speech Text to Speech Voice (en-GB, HazelRUS)";
            using (var synthesizer = new SpeechSynthesizer(mockConfig))
            {
                using (var result1 = await synthesizer.SpeakTextAsync("{{{text1}}}")) // "{{{text1}}}" has now completed rendering to default speakers
                using (var result2 = await synthesizer.SpeakTextAsync("{{{text2}}}")) // "{{{text2}}}" has now completed rendering to default speakers
                {
                    CheckSynthesisResult(result1);
                    CheckSynthesisResult(result2);

                    var expectedAudioData1 = BuildMockSynthesizedAudioWithHeader("{{{text1}}}", DefaultLanguage, "Microsoft Server Speech Text to Speech Voice (en-GB, HazelRUS)");
                    var expectedAudioData2 = BuildMockSynthesizedAudioWithHeader("{{{text2}}}", DefaultLanguage, "Microsoft Server Speech Text to Speech Voice (en-GB, HazelRUS)");

                    CheckBinaryEqual(expectedAudioData1, result1.AudioData);
                    CheckBinaryEqual(expectedAudioData2, result2.AudioData);
                }
            }
        }

        [RetryTestMethod]
        [TestCategory("SpeechSynthesisMockTest")]
        public async Task SynthesizerOutputToFileMock()
        {
            using (var fileConfig = AudioConfig.FromWavFileOutput("wavefile.wav"))
            {
                using (var synthesizer = new SpeechSynthesizer(mockConfig, fileConfig))
                {
                    using (var result1 = await synthesizer.SpeakTextAsync("{{{text1}}}")) // "{{{wavefile.wav}}}" now contains synthesized audio for "{{{text1}}}""
                    {
                        CheckSynthesisResult(result1);
                    }
                } // "{{{wavefile.wav}}}" is now closed

                var waveData1 = LoadWaveFileData("wavefile.wav");
                var expectedAudioData1 = BuildMockSynthesizedAudio("{{{text1}}}", DefaultLanguage, DefaultVoice);
                CheckBinaryEqual(expectedAudioData1, waveData1);

                using (var synthesizer = new SpeechSynthesizer(mockConfig, fileConfig))
                {
                    using (var result1 = await synthesizer.SpeakTextAsync("{{{text1}}}")) // "{{{wavefile.wav}}}" now contains synthesized audio for "{{{text1}}}""
                    using (var result2 = await synthesizer.SpeakTextAsync("{{{text2}}}")) // "{{{wavefile.wav}}}" now contains synthesized audio for both "{{{text1}}}"" and "{{{text2}}}"
                    {
                        CheckSynthesisResult(result1);
                        CheckSynthesisResult(result2);
                    }
                } // "{{{wavefile.wav}}}" is now closed

                var waveData12 = LoadWaveFileData("wavefile.wav");
                var expectedAudioData2 = BuildMockSynthesizedAudio("{{{text2}}}", DefaultLanguage, DefaultVoice);
                var expectedAudioData12 = MergeBinary(expectedAudioData1, expectedAudioData2);
                CheckBinaryEqual(expectedAudioData12, waveData12);
            }
        }

        [RetryTestMethod]
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

                    using (var result1 = await synthesizer.SpeakTextAsync("{{{text1}}}")) // "{{{text1}}}" has completed rendering to pushstream
                    {
                        CheckSynthesisResult(result1);
                        CheckBinaryEqual(expectedAudioData1, callback.GetAudioData());
                        SPXTEST_ISFALSE(callback.IsClosed(), "The push audio output stream should be opened until synthesizer is released.");
                    }

                    using (var result2 = await synthesizer.SpeakTextAsync("{{{text2}}}")) // "{{{text2}}}" has completed rendering to pushstream
                    {
                        CheckSynthesisResult(result2);
                        CheckBinaryEqual(expectedAudioData12, callback.GetAudioData());
                        SPXTEST_ISFALSE(callback.IsClosed(), "The push audio output stream should be opened until synthesizer is released.");
                    }

                    SPXTEST_ISFALSE(callback.IsClosed(), "The push audio output stream should be opened until synthesizer is released.");
                }

                SPXTEST_ISTRUE(callback.IsClosed(), "The push audio output stream should be closed after synthesizer is released.");
            }
        }

        [RetryTestMethod]
        [TestCategory("SpeechSynthesisMockTest")]
        public async Task SynthesizerOutputToPullStreamUseAfterSynthesisCompletedMock()
        {
            using (var stream = AudioOutputStream.CreatePullStream())
            using (var streamConfig = AudioConfig.FromStreamOutput(stream))
            {
                using (var synthesizer = new SpeechSynthesizer(mockConfig, streamConfig))
                {
                    using (var result1 = await synthesizer.SpeakTextAsync("{{{text1}}}")) // "{{{text1}}}" has completed rendering to pullstream
                    using (var result2 = await synthesizer.SpeakTextAsync("{{{text2}}}")) // "{{{text2}}}" has completed rendering to pullstream
                    {
                        CheckSynthesisResult(result1);
                        CheckSynthesisResult(result2);
                    }
                }

                var expectedAudioData1 = BuildMockSynthesizedAudio("{{{text1}}}", DefaultLanguage, DefaultVoice);
                var expectedAudioData2 = BuildMockSynthesizedAudio("{{{text2}}}", DefaultLanguage, DefaultVoice);
                var expectedAudioData12 = MergeBinary(expectedAudioData1, expectedAudioData2);

                DoSomethingWithAudioInPullStream(stream, new bool[] { false }, expectedAudioData12);
            }
        }

        [RetryTestMethod]
        [TestCategory("SpeechSynthesisMockTest")]
        public async Task SynthesizerOutputToPullStreamStartUsingBeforeDoneSynthesizingMock()
        {
            using (var stream = AudioOutputStream.CreatePullStream())
            {
                var expectedAudioData1 = BuildMockSynthesizedAudio("{{{text1}}}", DefaultLanguage, DefaultVoice);
                var expectedAudioData2 = BuildMockSynthesizedAudio("{{{text2}}}", DefaultLanguage, DefaultVoice);
                var expectedAudioData12 = MergeBinary(expectedAudioData1, expectedAudioData2);

                var future = DoSomethingWithAudioInPullStreamInBackground(stream, new bool[] { false }, expectedAudioData12);

                using (var streamConfig = AudioConfig.FromStreamOutput(stream))
                using (var synthesizer = new SpeechSynthesizer(mockConfig, streamConfig))
                {
                    using (var result1 = await synthesizer.SpeakTextAsync("{{{text1}}}")) // "{{{text1}}}" has completed rendering to pullstream
                    using (var result2 = await synthesizer.SpeakTextAsync("{{{text2}}}")) // "{{{text2}}}" has completed rendering to pullstream
                    {
                        CheckSynthesisResult(result1);
                        CheckSynthesisResult(result2);
                    }
                }

                await future;
            }
        }

        [RetryTestMethod]
        [TestCategory("SpeechSynthesisMockTest")]
        public async Task SpeakOutInResultsMock()
        {
            using (var synthesizer = new SpeechSynthesizer(mockConfig, null)) // null indicates to do nothing with synthesizer audio by default
            {
                using (var result1 = await synthesizer.SpeakTextAsync("{{{text1}}}")) // "{{{text1}}}" has completed rendering, and available in result1
                {
                    CheckSynthesisResult(result1);
                    SPXTEST_ARE_EQUAL(GuidLength, result1.ResultId.Length, "The length of result ID should be the length of a GUID (32).");
                    SPXTEST_ARE_EQUAL(ResultReason.SynthesizingAudioCompleted, result1.Reason, "The synthesis should be completed now.");

                    var expectedAudioData1 = BuildMockSynthesizedAudioWithHeader("{{{text1}}}", DefaultLanguage, DefaultVoice);
                    CheckBinaryEqual(expectedAudioData1, result1.AudioData);
                }

                using (var result2 = await synthesizer.SpeakTextAsync("{{{text2}}}")) // "{{{text2}}}" has completed rendering, and available in result2
                {
                    CheckSynthesisResult(result2);
                    SPXTEST_ARE_EQUAL(GuidLength, result2.ResultId.Length, "The length of result ID should be the length of a GUID (32).");
                    SPXTEST_ARE_EQUAL(ResultReason.SynthesizingAudioCompleted, result2.Reason, "The synthesis should be completed now.");

                    var expectedAudioData2 = BuildMockSynthesizedAudioWithHeader("{{{text2}}}", DefaultLanguage, DefaultVoice);
                    CheckBinaryEqual(expectedAudioData2, result2.AudioData);
                }
            }
        }

        [RetryTestMethod]
        [TestCategory("SpeechSynthesisMockTest")]
        public async Task SpeakOutputInChunksInEventSynthesizingMock()
        {
            using (var synthesizer = new SpeechSynthesizer(mockConfig, null)) // null indicates to do nothing with synthesizer audio by default
            {
                var expectedAudioData1 = BuildMockSynthesizedAudio("{{{text1}}}", DefaultLanguage, DefaultVoice);
                var expectedAudioData2 = BuildMockSynthesizedAudio("{{{text2}}}", DefaultLanguage, DefaultVoice);
                var expectedAudioData12 = MergeBinary(expectedAudioData1, expectedAudioData2);

                int offset = 0;
                var resultReasonList = new List<ResultReason>();
                var audioLengthList = new List<int>();
                var audioChunkList = new List<byte[]>();
                var expectedAudioChunkList = new List<byte[]>();
                synthesizer.Synthesizing += (s, e) =>
                {
                    resultReasonList.Add(e.Result.Reason);

                    var audioLength = e.Result.AudioData.Length;
                    audioLengthList.Add(audioLength - (int)EmptyWaveFileSize);

                    byte[] expectedAudioChunk = new byte[audioLength - EmptyWaveFileSize];
                    Array.Copy(expectedAudioData12, offset, expectedAudioChunk, 0, audioLength - EmptyWaveFileSize);
                    expectedAudioChunkList.Add(expectedAudioChunk);

                    byte[] rawAudioData = new byte[audioLength - EmptyWaveFileSize];
                    Array.Copy(e.Result.AudioData, EmptyWaveFileSize, rawAudioData, 0, rawAudioData.Length);
                    audioChunkList.Add(rawAudioData);

                    offset += rawAudioData.Length;
                };

                using (var result1 = await synthesizer.SpeakTextAsync("{{{text1}}}")) // "{{{text1}}}" has completed rendering
                using (var result2 = await synthesizer.SpeakTextAsync("{{{text2}}}")) // "{{{text2}}}" has completed rendering
                {
                    CheckSynthesisResult(result1);
                    CheckSynthesisResult(result2);
                }

                foreach (var resultReason in resultReasonList)
                {
                    SPXTEST_ARE_EQUAL(ResultReason.SynthesizingAudio, resultReason, "The synthesis should be on going now.");
                }

                foreach (var audioLength in audioLengthList)
                {
                    SPXTEST_ISTRUE(audioLength > 0, $"The audio chunk size should be greater than zero, but actually it's {audioLength}.");
                }

                for (int i = 0; i < expectedAudioChunkList.Count; ++i)
                {
                    CheckBinaryEqual(expectedAudioChunkList[i], audioChunkList[i]);
                }
            }
        }

        [RetryTestMethod]
        [TestCategory("SpeechSynthesisMockTest")]
        public async Task SpeakOutputInStreamsMock()
        {
            using (var synthesizer = new SpeechSynthesizer(mockConfig, null)) // null indicates to do nothing with synthesizer audio by default
            {
                using (var result1 = await synthesizer.SpeakTextAsync("{{{text1}}}")) // "{{{text1}}}" has completed rendering, and available in result1
                {
                    CheckSynthesisResult(result1);
                    SPXTEST_ARE_EQUAL(ResultReason.SynthesizingAudioCompleted, result1.Reason, "The synthesis should be completed now.");
                    var audioDataSize = result1.AudioData.Length;
                    SPXTEST_ISTRUE(audioDataSize > 0, $"The audio data size should be greater than zero, but actually it's {audioDataSize}.");
                    using (var stream1 = AudioDataStream.FromResult(result1)) // of type AudioDataStream, does not block
                    {
                        var expectedAudioData1 = BuildMockSynthesizedAudio("{{{text1}}}", DefaultLanguage, DefaultVoice);
                        DoSomethingWithAudioInDataStream(stream1, true, expectedAudioData1);

                        using (var result2 = await synthesizer.SpeakTextAsync("{{{text2}}}")) // "{{{text2}}}" has completed rendering, and available in result2
                        {
                            CheckSynthesisResult(result2);
                            SPXTEST_ARE_EQUAL(ResultReason.SynthesizingAudioCompleted, result2.Reason, "The synthesis should be completed now.");
                            audioDataSize = result2.AudioData.Length;
                            SPXTEST_ISTRUE(audioDataSize > 0, $"The audio data size should be greater than zero, but actually it's {audioDataSize}.");

                            using (var stream2 = AudioDataStream.FromResult(result2)) // of type AudioDataStream, does not block
                            {
                                var expectedAudioData2 = BuildMockSynthesizedAudio("{{{text2}}}", DefaultLanguage, DefaultVoice);
                                DoSomethingWithAudioInDataStream(stream2, true, expectedAudioData2);

                                stream1.SetPosition(0);
                                DoSomethingWithAudioInDataStream(stream1, true, expectedAudioData1); // re-check stream1 to make sure it's not impacted by stream2
                            }
                        }
                    }
                }
            }
        }

        [RetryTestMethod]
        [TestCategory("SpeechSynthesisMockTest")]
        public async Task SpeakOutputInStreamsBeforeDoneFromEventSynthesisStartedMock()
        {
            using (var synthesizer = new SpeechSynthesizer(mockConfig, null)) // null indicates to do nothing with synthesizer audio by default
            {
                var expectedAudioData = new List<byte[]>
                {
                    BuildMockSynthesizedAudio("{{{text1}}}", DefaultLanguage, DefaultVoice),
                    BuildMockSynthesizedAudio("{{{text2}}}", DefaultLanguage, DefaultVoice),
                    BuildMockSynthesizedAudio("{{{text3}}}", DefaultLanguage, DefaultVoice)
                };

                var futureList = new List<Task>();
                var streamList = new List<AudioDataStream>();
                var resultReasonList = new List<ResultReason>();
                var audioLengthList = new List<int>();
                int requestOrder = -1;
                synthesizer.SynthesisStarted += (s, e) => {
                    resultReasonList.Add(e.Result.Reason);
                    audioLengthList.Add(e.Result.AudioData.Length);

                    requestOrder++;

                    var stream = AudioDataStream.FromResult(e.Result); // of type AudioDataStream, does not block
                    futureList.Add(DoSomethingWithAudioInDataStreamInBackground(stream, false, expectedAudioData[requestOrder]));
                    streamList.Add(stream);
                };

                using (var result1 = await synthesizer.SpeakTextAsync("{{{text1}}}")) // "{{{text1}}}" has completed rendering
                using (var result2 = await synthesizer.SpeakTextAsync("{{{text2}}}")) // "{{{text2}}}" has completed rendering
                {
                    CheckSynthesisResult(result1);
                    CheckSynthesisResult(result2);
                }

                var future3 = synthesizer.SpeakTextAsync("{{{text3}}}"); // "{{{text3}}}" synthesis might have started
                using (var result3 = await future3) // "{{{text3}}}" synthesis has completed
                {
                    CheckSynthesisResult(result3);
                }

                foreach (var future in futureList)
                {
                    await future;
                }

                foreach (var stream in streamList)
                {
                    stream.Dispose();
                }

                foreach (var resultReason in resultReasonList)
                {
                    SPXTEST_ARE_EQUAL(ResultReason.SynthesizingAudioStarted, resultReason, "The synthesis should be started at this point.");
                }

                foreach (var audioLength in audioLengthList)
                {
                    SPXTEST_ARE_EQUAL(0, audioLength, "The synthesized audio should still be zero at this point, since the synthesis just started, and no audio chunk has been synthesized yet.");
                }
            }
        }

        [RetryTestMethod]
        [TestCategory("SpeechSynthesisMockTest")]
        public async Task SpeakOutputInStreamsBeforeDoneFromMethodStartSpeakingTextAsyncMock()
        {
            using (var synthesizer = new SpeechSynthesizer(mockConfig, null)) // null indicates to do nothing with synthesizer audio by default
            {
                using (var result1 = await synthesizer.StartSpeakingTextAsync("{{{text1}}}")) // "{{{text1}}}" synthesis has started, likely not finished
                {
                    CheckSynthesisResult(result1);
                    SPXTEST_ARE_EQUAL(ResultReason.SynthesizingAudioStarted, result1.Reason, "The synthesis should be started now.");
                    SPXTEST_ARE_EQUAL(0, result1.AudioData.Length, "The synthesized audio should still be zero at this point, since the synthesis just started, and no audio chunk has been synthesized yet.");

                    using (var stream1 = AudioDataStream.FromResult(result1)) // of type AudioDataStream, does not block
                    {
                        var expectedAudioData1 = BuildMockSynthesizedAudio("{{{text1}}}", DefaultLanguage, DefaultVoice);
                        var future1 = DoSomethingWithAudioInDataStreamInBackground(stream1, false, expectedAudioData1); // does not block, just spins a thread up

                        using (var result2 = await synthesizer.StartSpeakingTextAsync("{{{text2}}}")) // "{{{text2}}}" synthesis has started, likely not finished
                        {
                            CheckSynthesisResult(result2);
                            SPXTEST_ARE_EQUAL(ResultReason.SynthesizingAudioStarted, result2.Reason, "The synthesis should be started now.");
                            SPXTEST_ARE_EQUAL(0, result2.AudioData.Length, "The synthesized audio should still be zero at this point, since the synthesis just started, and no audio chunk has been synthesized yet.");

                            using (var stream2 = AudioDataStream.FromResult(result2)) // of type AudioDataStream, does not block
                            {
                                var expectedAudioData2 = BuildMockSynthesizedAudio("{{{text2}}}", DefaultLanguage, DefaultVoice);
                                var future2 = DoSomethingWithAudioInDataStreamInBackground(stream2, false, expectedAudioData2); // does not block, just spins a thread up

                                await future1;
                                await future2;
                            }
                        }
                    }
                }
            }
        }

        [RetryTestMethod]
        [TestCategory("SpeechSynthesisMockTest")]
        public async Task SpeakOutputInStreamsBeforeDoneQueuedMock()
        {
            using (var synthesizer = new SpeechSynthesizer(mockConfig, null)) // null indicates to do nothing with synthesizer audio by default
            {
                int startedRequests = 0;
                synthesizer.SynthesisStarted += (s, e) =>
                {
                    startedRequests++;
                };

                int completedRequests = 0;
                int startedRequestsWhenFirstRequestWasCompleted = 0;
                synthesizer.SynthesisCompleted += (s, e) =>
                {
                    completedRequests++;
                    if (completedRequests == 1)
                    {
                        startedRequestsWhenFirstRequestWasCompleted = startedRequests;
                    }
                };

                var futureResult1 = synthesizer.StartSpeakingTextAsync("{{{text1}}}"); // "{{{text1}}}" synthesis might have started, likely not finished
                var futureResult2 = synthesizer.StartSpeakingTextAsync("{{{text2}}}"); // "{{{text2}}}" synthesis might have started (very unlikely)

                var expectedAudioData1 = BuildMockSynthesizedAudio("{{{text1}}}", DefaultLanguage, DefaultVoice);
                var future1 = DoSomethingWithAudioInResultInBackground(futureResult1, false, expectedAudioData1); // does not block, just spins a thread up
                var expectedAudioData2 = BuildMockSynthesizedAudio("{{{text2}}}", DefaultLanguage, DefaultVoice);
                var future2 = DoSomethingWithAudioInResultInBackground(futureResult2, false, expectedAudioData2); // does not block, just spins a thread up

                await future1;
                await future2;

                // This is to check the requests is queued
                // When one request is already completed, the other one is still not started
                SPXTEST_ARE_EQUAL(1, startedRequestsWhenFirstRequestWasCompleted, "When one request is already completed, the other one should still not start yet.");
            }
        }

        [RetryTestMethod]
        [TestCategory("SpeechSynthesisMockTest")]
        public async Task SpeakOutputInStreamsWithAllDataGetOnSynthesisStartedResultMock()
        {
            using (var synthesizer = new SpeechSynthesizer(mockConfig, null)) // null indicates to do nothing with synthesizer audio by default
            {
                using (var result = await synthesizer.StartSpeakingTextAsync("{{{text1}}}"))
                {
                    CheckSynthesisResult(result);
                    SPXTEST_ARE_EQUAL(ResultReason.SynthesizingAudioStarted, result.Reason, "The synthesis should be started now.");
                    SPXTEST_ARE_EQUAL(0, result.AudioData.Length, "The synthesized audio should still be zero at this point, since the synthesis just started, and no audio chunk has been synthesized yet.");

                    bool synthesisCompleted = false;
                    synthesizer.SynthesisCompleted += (s, e) =>
                    {
                        synthesisCompleted = true;
                    };

                    while (!synthesisCompleted)
                    {
                        Thread.Sleep(100); // wait for the synthesis to be done
                    }

                    using (var stream = AudioDataStream.FromResult(result))
                    {
                        Thread.Sleep(500); // wait for more time for synthesis completed event to be sent to stream
                        var expectedAudioData = BuildMockSynthesizedAudio("{{{text1}}}", DefaultLanguage, DefaultVoice);
                        DoSomethingWithAudioInDataStream(stream, true, expectedAudioData);
                    }
                }
            }
        }

        [RetryTestMethod]
        [TestCategory("SpeechSynthesisMockTest")]
        public async Task SpeakOutputInStreamsWithAllDataGetSinceSynthesizingResultMock()
        {
            using (var synthesizer = new SpeechSynthesizer(mockConfig, null)) // null indicates to do nothing with synthesizer audio by default
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
                    CheckSynthesisResult(result);
                    SPXTEST_ARE_EQUAL(ResultReason.SynthesizingAudioCompleted, result.Reason, "The synthesis should be completed now.");
                    SPXTEST_ARE_EQUAL(StreamStatus.PartialData, stream.GetStatus(), "The stream should contain partial audio data since it's created from a synthesizing result.");

                    var fullAudioData = BuildMockSynthesizedAudio("{{{text1}}}", DefaultLanguage, DefaultVoice);
                    var expectedAudioData = new byte[fullAudioData.Length - MockAudioChunkSize];
                    Array.Copy(fullAudioData, MockAudioChunkSize, expectedAudioData, 0, expectedAudioData.Length);
                    CheckAudioInDataStream(stream, expectedAudioData);

                    stream.Dispose();
                }
            }
        }

        [RetryTestMethod]
        public async Task TestSynthesisWithCustomVoice()
        {
            using (var synthesizer = new SpeechSynthesizer(customVoiceConfig, null))
            {
                using (var result = await synthesizer.SpeakTextAsync("text"))
                {
                    CheckSynthesisResult(result);
                }
            }
        }
        
        [RetryTestMethod]
        public async Task SynthesisWithSovereignCloudToken()
        {
            var mooncakeSubscriptionKey = SubscriptionsRegionsMap[SubscriptionsRegionsKeys.MOONCAKE_SUBSCRIPTION].Key;
            var mooncakeRegion = SubscriptionsRegionsMap[SubscriptionsRegionsKeys.MOONCAKE_SUBSCRIPTION].Region;
            var token = await GetToken(mooncakeSubscriptionKey, mooncakeRegion, "azure.cn");

            var restEndpoint = $"https://{mooncakeRegion}.tts.speech.azure.cn/cognitiveservices/v1?TrafficType=Test";
            var restMooncakeConfig = SpeechConfig.FromEndpoint(new Uri(restEndpoint));
            using (var synthesizer = new SpeechSynthesizer(restMooncakeConfig, null))
            {
                synthesizer.AuthorizationToken = token;
                using (var result = await synthesizer.SpeakTextAsync("{{{text}}}"))
                {
                    CheckSynthesisResult(result);
                }
            }

            var uspEndpoint = $"wss://{mooncakeRegion}.tts.speech.azure.cn/cognitiveservices/websocket/v1?TrafficType=Test";
            var uspMooncakeConfig = SpeechConfig.FromEndpoint(new Uri(uspEndpoint));
            using (var synthesizer = new SpeechSynthesizer(uspMooncakeConfig, null))
            {
                synthesizer.AuthorizationToken = token;
                // set timeout to 60s as the connection is flaky
                synthesizer.Properties.SetProperty("SpeechSynthesis_FirstChunkTimeoutMs", "60000");
                using (var result = await synthesizer.SpeakTextAsync("{{{text}}}"))
                {
                    CheckSynthesisResult(result);
                }
            }
        }

        [RetryTestMethod]
        public async Task SynthesisWithSovereignCloudFromEndpoint()
        {
            var mooncakeSubscriptionKey = SubscriptionsRegionsMap[SubscriptionsRegionsKeys.MOONCAKE_SUBSCRIPTION].Key;
            var mooncakeRegion = SubscriptionsRegionsMap[SubscriptionsRegionsKeys.MOONCAKE_SUBSCRIPTION].Region;
            var token = await GetToken(mooncakeSubscriptionKey, mooncakeRegion, "azure.cn");

            var restEndpoint = $"https://{mooncakeRegion}.tts.speech.azure.cn/cognitiveservices/v1?TrafficType=Test";
            var restMooncakeConfig = SpeechConfig.FromEndpoint(new Uri(restEndpoint), mooncakeSubscriptionKey);
            using (var synthesizer = new SpeechSynthesizer(restMooncakeConfig, null))
            {
                using (var result = await synthesizer.SpeakTextAsync("{{{text}}}"))
                {
                    CheckSynthesisResult(result);
                }
            }

            var uspEndpoint = $"wss://{mooncakeRegion}.tts.speech.azure.cn/cognitiveservices/websocket/v1?TrafficType=Test";
            var uspMooncakeConfig = SpeechConfig.FromEndpoint(new Uri(uspEndpoint), mooncakeSubscriptionKey);
            using (var synthesizer = new SpeechSynthesizer(uspMooncakeConfig, null))
            {
                // set timeout to 60s as the connection is flaky
                synthesizer.Properties.SetProperty("SpeechSynthesis_FirstChunkTimeoutMs", "60000");
                using (var result = await synthesizer.SpeakTextAsync("{{{text}}}"))
                {
                    CheckSynthesisResult(result);
                }
            }
        }

        [RetryTestMethod]
        public async Task SynthesisWithMooncakeFromSubscription()
        {
            var mooncakeSubscriptionKey = SubscriptionsRegionsMap[SubscriptionsRegionsKeys.MOONCAKE_SUBSCRIPTION].Key;
            var mooncakeRegion = SubscriptionsRegionsMap[SubscriptionsRegionsKeys.MOONCAKE_SUBSCRIPTION].Region;

            var mooncakeConfig = SpeechConfig.FromSubscription(mooncakeSubscriptionKey, mooncakeRegion);
            using (var synthesizer = new SpeechSynthesizer(mooncakeConfig, null))
            {
                // set timeout to 60s as the connection is flaky
                synthesizer.Properties.SetProperty("SpeechSynthesis_FirstChunkTimeoutMs", "60000");
                using (var result = await synthesizer.SpeakTextAsync("{{{text}}}"))
                {
                    CheckSynthesisResult(result);
                }

                var connectionUrl = synthesizer.Properties.GetProperty(PropertyId.SpeechServiceConnection_Url);
                SPXTEST_ISTRUE(connectionUrl.Contains("azure.cn"));
            }
        }

        [RetryTestMethod]
        public async Task SynthesisWithSovereignCloudFromHost()
        {
            var mooncakeSubscriptionKey = SubscriptionsRegionsMap[SubscriptionsRegionsKeys.MOONCAKE_SUBSCRIPTION].Key;
            var mooncakeRegion = SubscriptionsRegionsMap[SubscriptionsRegionsKeys.MOONCAKE_SUBSCRIPTION].Region;
            var token = await GetToken(mooncakeSubscriptionKey, mooncakeRegion, "azure.cn");

            var restHost = $"https://{mooncakeRegion}.tts.speech.azure.cn";
            var restMooncakeConfig = SpeechConfig.FromHost(new Uri(restHost), mooncakeSubscriptionKey);
            using (var synthesizer = new SpeechSynthesizer(restMooncakeConfig, null))
            {
                using (var result = await synthesizer.SpeakTextAsync("{{{text}}}"))
                {
                    CheckSynthesisResult(result);
                }
            }

            var uspHost = $"wss://{mooncakeRegion}.tts.speech.azure.cn";
            var uspMooncakeConfig = SpeechConfig.FromHost(new Uri(uspHost), mooncakeSubscriptionKey);
            using (var synthesizer = new SpeechSynthesizer(uspMooncakeConfig, null))
            {
                using (var result = await synthesizer.SpeakTextAsync("{{{text}}}"))
                {
                    CheckSynthesisResult(result);
                }
            }
        }

        [RetryTestMethod]
        public async Task TestSynthesisConfigFromHost()
        {
            using (var synthesizer = new SpeechSynthesizer(restHostConfig))
            {
                using (var result1 = await synthesizer.SpeakTextAsync("{{{text1}}}")) // "{{{text1}}}" has now completed rendering to default speakers
                {
                    CheckSynthesisResult(result1);
                }
            }
            using (var synthesizer = new SpeechSynthesizer(uspHostConfig))
            {
                using (var result2 = await synthesizer.SpeakTextAsync("{{{text2}}}")) // "{{{text2}}}" has now completed rendering to default speakers
                {
                    CheckSynthesisResult(result2);
                }
            }
        }

        public static void CheckSynthesisResult(SpeechSynthesisResult result, int minimumSize = -1, [CallerLineNumber]int line = 0, [CallerMemberName]string caller = null, [CallerFilePath]string file = null)
        {
            string cancellationDetails = string.Empty;
            if (result.Reason == ResultReason.Canceled)
            {
                cancellationDetails = SpeechSynthesisCancellationDetails.FromResult(result).ToString();
            }

            SPXTEST_ARE_NOT_EQUAL(ResultReason.Canceled, result.Reason, $"The synthesis was canceled unexpectedly, with request id = [{result.ResultId}], cancellation details = [{cancellationDetails}].", line, caller, file);
            if (minimumSize > 0)
            {
                SPXTEST_ISTRUE(result.AudioData.Length >= minimumSize, $"Audio size should be greater than {minimumSize}, but actually it's {result.AudioData.Length}.", line, caller, file);
            }
        }

        private void DoSomethingWithAudioInPullStream(PullAudioOutputStream stream, bool[] canceled)
        {
            DoSomethingWithAudioInPullStream(stream, canceled, null);
        }

        private void DoSomethingWithAudioInPullStream(PullAudioOutputStream stream, bool[] canceled, byte[] expectedData)
        {
            byte[] audioData = Array.Empty<byte>();
            byte[] buffer = new byte[1024];
            uint totalSize = 0;
            uint filledSize = 0;

            while ((filledSize = stream.Read(buffer)) > 0)
            { // blocks until atleast 1024 bytes are available
              // do something with buffer
                totalSize += filledSize;
                Array.Resize(ref audioData, (int)totalSize);
                for (int i = 0; i < filledSize; ++i)
                {
                    audioData[totalSize - filledSize + i] = buffer[i];
                }
            }

            if (!canceled[0])
            {
                SPXTEST_ISTRUE(totalSize > 0, $"Audio size should be greater than zero, but actually it's {totalSize}.");
            }

            if (expectedData != null)
            {
                CheckBinaryEqual(expectedData, audioData);
            }
        }

        private Task DoSomethingWithAudioInPullStreamInBackground(PullAudioOutputStream stream, bool[] canceled)
        {
            return DoSomethingWithAudioInPullStreamInBackground(stream, canceled, null);
        }

        private Task DoSomethingWithAudioInPullStreamInBackground(PullAudioOutputStream stream, bool[] canceled, byte[] expectedData)
        {
            return Task.Run(() =>
            {
                DoSomethingWithAudioInPullStream(stream, canceled, expectedData);
            });
        }

        private void DoSomethingWithAudioInDataStream(AudioDataStream stream, bool afterSynthesisDone)
        {
            DoSomethingWithAudioInDataStream(stream, afterSynthesisDone, null);
        }

        private void DoSomethingWithAudioInDataStream(AudioDataStream stream, bool afterSynthesisDone, byte[] expectedData)
        {
            if (afterSynthesisDone && stream.GetStatus() != StreamStatus.Canceled)
            {
                SPXTEST_ARE_EQUAL(StreamStatus.AllData, stream.GetStatus(), "Audio data stream should contain all data after a succeeded synthesis.");
            }

            CheckAudioInDataStream(stream, expectedData);

            if (!afterSynthesisDone && stream.GetStatus() != StreamStatus.Canceled)
            {
                SPXTEST_ARE_EQUAL(StreamStatus.AllData, stream.GetStatus(), "Audio data stream should contain all data after a succeeded synthesis.");
            }
        }

        private void CheckAudioInDataStream(AudioDataStream stream, byte[] expectedData)
        {
            byte[] audioData = Array.Empty<byte>();
            byte[] buffer = new byte[1024];
            uint totalSize = 0;
            uint filledSize = 0;

            while ((filledSize = stream.ReadData(buffer)) > 0)
            { // blocks until at least 1024 bytes are available
              // do something with buffer
                totalSize += filledSize;
                Array.Resize(ref audioData, (int)totalSize);
                for (int i = 0; i < filledSize; ++i)
                {
                    audioData[totalSize - filledSize + i] = buffer[i];
                }
            }

            if (expectedData != null)
            {
                CheckBinaryEqual(expectedData, audioData);
            }

            if (stream.GetStatus() == StreamStatus.Canceled)
            {
                var cancelDetails = SpeechSynthesisCancellationDetails.FromStream(stream);
                // do something with cancellation details
                SPXTEST_ARE_EQUAL(CancellationReason.Error, cancelDetails.Reason);
                SPXTEST_ARE_NOT_EQUAL(CancellationErrorCode.NoError, cancelDetails.ErrorCode);
            }
            else
            {
                SPXTEST_ISTRUE(totalSize > 0, $"Audio size should be greater than zero for a succeeded synthesis, but actually it's {totalSize}");
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

        private void CheckBinaryEqual(byte[] expectedData, byte[] actualData)
        {
            SPXTEST_ISNOTNULL(expectedData);
            SPXTEST_ISNOTNULL(actualData);
            SPXTEST_ARE_EQUAL(expectedData.Length, actualData.Length);

            for (int i = 0; i < expectedData.Length; ++i)
            {
                SPXTEST_ARE_EQUAL(expectedData[i], actualData[i]);
            }
        }

        private byte[] BuildRiffHeader(int dataSize)
        {
            var isFloatingPoint = false;
            ushort channelCount = 1;
            ushort bitDepth = 16;
            int sampleRate = 16000;

            MemoryStream stream = new MemoryStream {Position = 0};
            // RIFF header.
            // Chunk ID.
            stream.Write(Encoding.ASCII.GetBytes("RIFF"), 0, 4);
            // Chunk size.
            stream.Write(BitConverter.GetBytes(dataSize + 38), 0, 4);
            // Format.
            stream.Write(Encoding.ASCII.GetBytes("WAVE"), 0, 4);
            // Sub-chunk 1.
            // Sub-chunk 1 ID.
            stream.Write(Encoding.ASCII.GetBytes("fmt "), 0, 4);
            // Sub-chunk 1 size.
            stream.Write(BitConverter.GetBytes(18), 0, 4);
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
            // cbSize
            stream.Write(BitConverter.GetBytes(0), 0, 2);
            // Sub-chunk 2.
            // Sub-chunk 2 ID.
            stream.Write(Encoding.ASCII.GetBytes("data"), 0, 4);
            // Sub-chunk 2 size.
            stream.Write(BitConverter.GetBytes(dataSize), 0, 4);

            return stream.ToArray();
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

            Array.Copy(ssmlBytes, 0, audio, MockAudioSize, ssmlBytes.Length);

            return audio;
        }

        private byte[] BuildMockSynthesizedAudioWithHeader(string text, string language, string voice)
        {
            var data = BuildMockSynthesizedAudio(text, language, voice);
            return MergeBinary(BuildRiffHeader(data.Length), data);
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
                return Array.Empty<byte>();
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
            audioData = Array.Empty<byte>();
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