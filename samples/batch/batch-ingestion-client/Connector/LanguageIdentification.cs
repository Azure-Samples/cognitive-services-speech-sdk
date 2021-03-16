// <copyright file="LanguageIdentification.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector
{
    using System;
    using System.Collections.Generic;
    using System.IO;
    using System.Linq;
    using System.Threading;
    using System.Threading.Tasks;
    using Microsoft.CognitiveServices.Speech;
    using Microsoft.CognitiveServices.Speech.Audio;
    using NAudio.Wave;

    public class LanguageIdentification
    {
        private const int UtteranceCount = 15;

        private const int TimeoutInSeconds = 30;

        private string SubscriptionKey;

        private string SubscriptionRegion;

        public LanguageIdentification(string subscriptionKey, string subscriptionRegion)
        {
            SubscriptionKey = subscriptionKey;
            SubscriptionRegion = subscriptionRegion;
        }

        public async Task<string> DetectLanguage(byte[] audioBytes, string fileExtension, string locale1, string locale2)
        {
            var wavBytes = ConvertToWaveBytes(audioBytes, fileExtension);

            var autoDetectSourceLanguageConfig = AutoDetectSourceLanguageConfig.FromLanguages(new string[] { locale1, locale2 });

            var config = SpeechConfig.FromSubscription(SubscriptionKey, SubscriptionRegion);
            var stopRecognition = new TaskCompletionSource<int>();
            var detected = new List<string>();

            using var pushStream = AudioInputStream.CreatePushStream();
            using (var audioInput = AudioConfig.FromStreamInput(pushStream))
            {
                using var recognizer = new SpeechRecognizer(
                    config,
                    autoDetectSourceLanguageConfig,
                    audioInput);
                pushStream.Write(wavBytes);
                pushStream.Close();

                recognizer.Recognized += (s, e) =>
                {
                    var autoDetectSourceLanguageResult = AutoDetectSourceLanguageResult.FromResult(e.Result);
                    var detectedLanguage = autoDetectSourceLanguageResult.Language;
                    detected.Add(detectedLanguage);
                    if (detected.Count > UtteranceCount)
                    {
                        stopRecognition.TrySetResult(0);
                    }
                };

                recognizer.SessionStopped += (s, e) =>
                {
                    stopRecognition.TrySetResult(0);
                };

                await recognizer.StartContinuousRecognitionAsync().ConfigureAwait(false);

                var t = Task.Factory.StartNew(async () => { await SetTimeOutForRecognition(stopRecognition).ConfigureAwait(false); }, CancellationToken.None, TaskCreationOptions.None, TaskScheduler.Default);

                Task.WaitAny(new[] { stopRecognition.Task });

                await recognizer.StopKeywordRecognitionAsync().ConfigureAwait(false);
            }

            if (detected.Count == 0)
            {
                throw new TimeoutException("Did not get any language identification results back in time.");
            }

            var detectedByCount = detected.GroupBy(i => i);
            var mostFreq = detectedByCount.OrderBy(t => t.Count()).LastOrDefault().Key;

            if (string.IsNullOrEmpty(mostFreq) || (!mostFreq.Equals(locale1, StringComparison.OrdinalIgnoreCase) && !mostFreq.Equals(locale2, StringComparison.OrdinalIgnoreCase)))
            {
                return locale1;
            }

            return mostFreq;
        }

        private static async Task SetTimeOutForRecognition(TaskCompletionSource<int> taskCompletionSource)
        {
            await Task.Delay(TimeoutInSeconds * 1000).ConfigureAwait(false);

            if (taskCompletionSource.Task.Status != TaskStatus.RanToCompletion)
            {
                taskCompletionSource.TrySetResult(0);
            }
        }

        private static byte[] ConvertToWaveBytes(byte[] fileBytes, string fileNameExtension)
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
