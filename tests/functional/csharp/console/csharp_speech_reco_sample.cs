//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// carbon_csharp_console.cs: A console application for testing Carbon C# client library.
//

using System;
using System.Diagnostics;
using System.Globalization;
using System.IO;
using System.Threading.Tasks;
using Microsoft.CognitiveServices.Speech;
using Microsoft.CognitiveServices.Speech.Audio;

namespace MicrosoftSpeechSDKSamples
{
    public class SpeechRecognitionSamples
    {
        private static void MyIntermediateResultEventHandler(object sender, SpeechRecognitionResultEventArgs e)
        {
            Console.WriteLine(string.Format(CultureInfo.InvariantCulture, "Speech recognition: intermediate result: {0}, Offset: {1}, Duration: {2} ", e.ToString(), e.Result.OffsetInTicks, e.Result.Duration));
        }

        private static void MyFinalResultEventHandler(object sender, SpeechRecognitionResultEventArgs e)
        {
            Console.WriteLine(string.Format(CultureInfo.InvariantCulture, "Speech recognition: final result: {0}, Offset: {1}, Duration: {2} ", e.ToString(), e.Result.OffsetInTicks, e.Result.Duration));
        }

        private static void MyCanceledEventHandler(object sender, SpeechRecognitionCanceledEventArgs e)
        {
            Console.WriteLine(string.Format(CultureInfo.InvariantCulture, "Speech recognition: canceled. SessionId: {0}, Reason: {1}", e.SessionId, e.Reason));
        }

        private static void MySessionEventHandler(object sender, SessionEventArgs e)
        {
            Console.WriteLine(string.Format(CultureInfo.InvariantCulture, "Speech recognition: Session event: {0}.", e.ToString()));
        }

        public static async Task SpeechRecognitionBaseModelAsync(string key, string lang, string fileName, bool useStream, bool useToken, bool useContinuousRecognition)
        {
            Console.WriteLine("Speech Recognition using base model.");
            SpeechConfig config = null;
            if (useToken)
            {
                config = SpeechConfig.FromAuthorizationToken(key, "westus");
            }
            else
            {
                config = SpeechConfig.FromSubscription(key, "westus");
            }

            await RecognizeAsync(config, fileName, useStream, useContinuousRecognition).ConfigureAwait(false);
        }

        public static async Task SpeechRecognitionCustomizedModelAsync(string key, string lang, string model, string fileName, bool useStream, bool useToken, bool useContinuousRecognition)
        {
            Console.WriteLine("Speech Recognition using customized model.");
            SpeechConfig config = null;
            if (useToken)
            {
                config = SpeechConfig.FromAuthorizationToken(key, "westus");
            }
            else
            {
                config = SpeechConfig.FromSubscription(key, "westus");
            }
            config.EndpointId = model;

            await RecognizeAsync(config, fileName, useStream, useContinuousRecognition).ConfigureAwait(false);
        }

        public static async Task SpeechRecognitionByEndpointAsync(string subscriptionKey, string endpoint, string lang, string model, string fileName, bool useStream, bool useContinuousRecognition)
        {
            Console.WriteLine(string.Format(CultureInfo.InvariantCulture, "Speech Recognition using endopoint:{0}.", endpoint));

            SpeechConfig config = SpeechConfig.FromEndpoint(new Uri(endpoint), subscriptionKey);
            if (!string.IsNullOrEmpty(lang))
            {
                config.SpeechRecognitionLanguage = lang;
            }

            if (!string.IsNullOrEmpty(model))
            {
                config.EndpointId = model;
            }

            await RecognizeAsync(config, fileName, useStream, useContinuousRecognition).ConfigureAwait(false);
        }

        public static async Task RecognizeAsync(SpeechConfig config, string fileName, bool useStream, bool useContinuousRecognition)
        {
            if (string.IsNullOrEmpty(fileName) || String.Compare(fileName, "mic", true) == 0)
            {
                using (var reco = new SpeechRecognizer(config))
                {
                    await StartRecognitionAsync(reco, useContinuousRecognition);
                }
            }
            else
            {
                if (useStream)
                {
                    Console.WriteLine("Using stream input.");
                    var audioInput = Util.OpenWavFile(fileName);
                    using (var reco = new SpeechRecognizer(config, audioInput))
                    {
                        await StartRecognitionAsync(reco, useContinuousRecognition);
                    }
                }
                else
                {
                    using (var reco = new SpeechRecognizer(config, AudioConfig.FromWavFileInput(fileName)))
                    {
                        await StartRecognitionAsync(reco, useContinuousRecognition);
                    }
                }
            }
        }

        private static async Task StartRecognitionAsync(SpeechRecognizer reco, bool useContinuousRecognition)
        {
            if (useContinuousRecognition)
            {
                await ContinuousRecognitionAsync(reco).ConfigureAwait(false);
            }
            else
            {
                await SingleShotRecognitionAsync(reco).ConfigureAwait(false);
            }
        }

        private static async Task SingleShotRecognitionAsync(SpeechRecognizer reco)
        {
            Console.WriteLine("Single-shot recognition.");

            // Subscribes to events.
            reco.IntermediateResultReceived += MyIntermediateResultEventHandler;
            reco.FinalResultReceived += MyFinalResultEventHandler;
            reco.Canceled += MyCanceledEventHandler;
            reco.OnSessionEvent += MySessionEventHandler;

            // Starts recognition.
            var result = await reco.RecognizeAsync().ConfigureAwait(false);

            Console.WriteLine("Speech Recognition: Recognition result: " + result);

            // Unsubscribe to events.
            reco.IntermediateResultReceived -= MyIntermediateResultEventHandler;
            reco.FinalResultReceived -= MyFinalResultEventHandler;
            reco.Canceled -= MyCanceledEventHandler;
            reco.OnSessionEvent -= MySessionEventHandler;
        }

        private static async Task ContinuousRecognitionAsync(SpeechRecognizer reco)
        {
            Console.WriteLine("Continuous recognition.");
            var tcsLocal = new TaskCompletionSource<int>();

            reco.FinalResultReceived += (s, e) =>
            {
                if (e.Result.Reason == ResultReason.RecognizedSpeech)
                {
                    Console.WriteLine($"RECOGNIZED: Text={e.Result.Text}");
                }
                else if (e.Result.Reason == ResultReason.NoMatch)
                {
                    Console.WriteLine($"NOMATCH: Speech could not be recognized.");
                }
            };

            reco.OnSpeechDetectedEvent += (s, e) =>
            {
                Console.WriteLine($"SpeechDetectedEvent received: {e.EventType.ToString()}, offset: {e.Offset}.");
                if (e.EventType == RecognitionEventType.SpeechEndDetectedEvent)
                {
                    Console.WriteLine($"Speech end detected.");
                }
            };

            reco.Canceled += (s, e) =>
            {
                Console.WriteLine($"CANCELED: Reason={e.Reason}");

                if (e.Reason == CancellationReason.Error)
                {
                    Console.WriteLine($"CANCELED: ErrorDetails={e.ErrorDetails}");
                    Console.WriteLine($"CANCELED: Did you update the subscription info?");
                }
            };

            reco.OnSessionEvent += (s, e) =>
            {
                Console.WriteLine($"\n    Session event. Event: {e.EventType.ToString()}.");
                if (e.EventType == SessionEventType.SessionStoppedEvent)
                {
                    Console.WriteLine($"Session Stop detected. Stop the recognition.");
                    tcsLocal.TrySetResult(0);
                }
            };

            // Starts continuos recognition. Uses StopContinuousRecognitionAsync() to stop recognition.
            await reco.StartContinuousRecognitionAsync().ConfigureAwait(false);

            // Waits for completion.
            // Use Task.WaitAny to make sure that the task is rooted.
            Task.WaitAny(new []{ tcsLocal.Task });

            // Stops translation.
            await reco.StopContinuousRecognitionAsync().ConfigureAwait(false);
        }
    }
}
