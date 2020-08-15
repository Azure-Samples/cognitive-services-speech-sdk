//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using Microsoft.CognitiveServices.Speech.Audio;
using MicrosoftSpeechSDKSamples;
using System;
using System.Collections.Generic;
using System.IO;
using System.Threading.Tasks;

namespace Microsoft.CognitiveServices.Speech.Tests.EndToEnd
{
    using Microsoft.VisualStudio.TestTools.UnitTesting;
    using static SpeechRecognitionTestsHelper;

    public class ClientPullStreamCallback : PullAudioInputStreamCallback
    {
        FileStream fs;

        public ClientPullStreamCallback(string filename)
        {
            fs = File.OpenRead(filename);
        }

        public override int Read(byte[] dataBuffer, uint size)
        {
            return fs.Read(dataBuffer, 0, (int)size);
        }
    }

    sealed class SpeechWithStreamHelper
    {
        private readonly TimeSpan timeout;

        public SpeechWithStreamHelper()
        {
            timeout = TimeSpan.FromMinutes(6);
        }

        SpeechRecognizer CreateSpeechRecognizerWithStream(SpeechConfig config, string audioFile)
        {
            var audioInput = Util.OpenWavFile(audioFile);
            return new SpeechRecognizer(config, audioInput);
        }

        public async Task<SpeechRecognitionResult> GetFinalRecoAsync(
            SpeechConfig config,
            string audioFile,
            bool usePreconnect = false)
        {
            using (var recognizer = TrackSessionId(new SpeechRecognizer(config, AudioConfig.FromWavFileInput(audioFile))))
            using (var connection = Connection.FromRecognizer(recognizer))
            {
                if (usePreconnect)
                {
                    connection.Open(forContinuousRecognition: false);
                }

                SpeechRecognitionResult result = null;
                await Task.WhenAny(recognizer.RecognizeOnceAsync().ContinueWith(t => result = t.Result), Task.Delay(timeout));
                return result;
            }
        }

        public async Task<SpeechRecognitionResult> GetSpeechFinalRecognitionResultPullStreamWithCompressedFile(SpeechConfig config, string audioFile, AudioStreamContainerFormat containerType)
        {
            using (var recognizer = TrackSessionId(new SpeechRecognizer(config,
                AudioConfig.FromStreamInput(new PullAudioInputStream(new ClientPullStreamCallback(audioFile),
                                                    AudioStreamFormat.GetCompressedFormat(containerType))))))
            {
                SpeechRecognitionResult result = null;
                await Task.WhenAny(recognizer.RecognizeOnceAsync().ContinueWith(t => result = t.Result), Task.Delay(timeout));
                return result;
            }
        }

        public async Task<List<SpeechRecognitionEventArgs>> GetFinalRecoEventsForContinuousAsync(
            SpeechConfig config,
            string audioFile,
            bool usePreconnect = false)
        {
            using (var recognizer = TrackSessionId(CreateSpeechRecognizerWithStream(config, audioFile)))
            using (var connection = Connection.FromRecognizer(recognizer))
            {
                if (usePreconnect)
                {
                    connection.Open(forContinuousRecognition: true);
                }

                var tcs = new TaskCompletionSource<bool>();
                var textResultEvents = new List<SpeechRecognitionEventArgs>();

                recognizer.Recognized += (s, e) =>
                {
                    Console.WriteLine($"Received result {e.Result.ToString()}");
                    if (e.Result.Reason == ResultReason.RecognizedSpeech && !string.IsNullOrEmpty(e.Result.Text))
                    {
                        textResultEvents.Add(e);
                    }
                };

                recognizer.SessionStopped += (s, e) =>
                {
                    tcs.TrySetResult(true);
                };
                string canceled = string.Empty;
                recognizer.Canceled += (s, e) => { canceled = e.ErrorDetails; };

                await recognizer.StartContinuousRecognitionAsync();
                await Task.WhenAny(tcs.Task, Task.Delay(timeout));
                await recognizer.StopContinuousRecognitionAsync();

                if (!string.IsNullOrEmpty(canceled))
                {
                    Assert.Fail($"Recognition canceled: {canceled}");
                }

                return textResultEvents;
            }
        }
    }
}
