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

namespace MicrosoftSpeechSDKSamples
{
    public class SpeechRecognitionSamples
    {
        private static TaskCompletionSource<int> tcs;

        private static void MyIntermediateResultEventHandler(object sender, SpeechRecognitionResultEventArgs e)
        {
            Console.WriteLine(string.Format(CultureInfo.InvariantCulture, "Speech recognition: intermediate result: {0} ", e.ToString()));
        }

        private static void MyFinalResultEventHandler(object sender, SpeechRecognitionResultEventArgs e)
        {
            Console.WriteLine(string.Format(CultureInfo.InvariantCulture, "Speech recognition: final result: {0} ", e.ToString()));
        }

        private static void MyErrorEventHandler(object sender, RecognitionErrorEventArgs e)
        {
            Console.WriteLine(string.Format(CultureInfo.InvariantCulture, "Speech recognition: error occurred. SessionId: {0}, Reason: {1}", e.SessionId, e.Status));
        }

        private static void MySessionEventHandler(object sender, SessionEventArgs e)
        {
            Console.WriteLine(string.Format(CultureInfo.InvariantCulture, "Speech recognition: Session event: {0}.", e.ToString()));
        }

        public static async Task SpeechRecognitionBaseModelAsync(string key, string lang, string fileName, bool useStream, bool useToken, bool useContinuousRecognition)
        {
            Console.WriteLine("Speech Recognition using base model.");
            SpeechFactory factory;
            if (useToken)
            {
                factory = SpeechFactory.FromAuthorizationToken(key, "westus");
            }
            else
            {
                factory = SpeechFactory.FromSubscription(key, "westus");
            }

            await RecognizeAsync(factory, lang, null, fileName, useStream, useContinuousRecognition).ConfigureAwait(false);
        }

        public static async Task SpeechRecognitionCustomizedModelAsync(string key, string lang, string model, string fileName, bool useStream, bool useToken, bool useContinuousRecognition)
        {
            Console.WriteLine("Speech Recognition using customized model.");
            SpeechFactory factory;
            if (useToken)
            {
                factory = SpeechFactory.FromAuthorizationToken(key, "westus");
            }
            else
            {
                factory = SpeechFactory.FromSubscription(key, "westus");
            }

            await RecognizeAsync(factory, lang, model, fileName, useStream, useContinuousRecognition).ConfigureAwait(false);
        }

        public static async Task SpeechRecognitionByEndpointAsync(string subscriptionKey, string endpoint, string lang, string model, string fileName, bool useStream, bool useContinuousRecognition)
        {
            Console.WriteLine(string.Format(CultureInfo.InvariantCulture, "Speech Recognition using endopoint:{0}.", endpoint));

            SpeechFactory factory = SpeechFactory.FromEndPoint(new Uri(endpoint), subscriptionKey);
            await RecognizeAsync(factory, lang, model, fileName, useStream, useContinuousRecognition).ConfigureAwait(false);
        }

        public static async Task RecognizeAsync(SpeechFactory factory, string lang, string model, string fileName, bool useStream, bool useContinuousRecognition)
        {
            if (string.IsNullOrEmpty(fileName) || String.Compare(fileName, "mic", true) == 0)
            {
                if (string.IsNullOrEmpty(lang))
                {
                    using (var reco = factory.CreateSpeechRecognizer())
                    {
                        await StartRecognitionAsync(reco, model, useContinuousRecognition);
                    }
                }
                else
                {
                    using (var reco = factory.CreateSpeechRecognizer(lang))
                    {
                        await StartRecognitionAsync(reco, model, useContinuousRecognition);
                    }
                }
            }
            else
            {
                if (string.IsNullOrEmpty(lang))
                {
                    if (useStream)
                    {
                        Console.WriteLine("Using stream input.");
                        var stream = Util.OpenWaveFile(fileName);
                        using (var reco = factory.CreateSpeechRecognizerWithStream(stream))
                        {
                            await StartRecognitionAsync(reco, model, useContinuousRecognition);
                        }
                    }
                    else
                    {
                        using (var reco = factory.CreateSpeechRecognizerWithFileInput(fileName))
                        {
                            await StartRecognitionAsync(reco, model, useContinuousRecognition);
                        }
                    }
                }
                else
                {
                    if (useStream)
                    {
                        Console.WriteLine("Using stream input.");
                        var stream = Util.OpenWaveFile(fileName);
                        using (var reco = factory.CreateSpeechRecognizerWithStream(stream, lang))
                        {
                            await StartRecognitionAsync(reco, model, useContinuousRecognition);
                        }
                    }
                    else
                    {
                        using (var reco = factory.CreateSpeechRecognizerWithFileInput(fileName, lang))
                        {
                            await StartRecognitionAsync(reco, model, useContinuousRecognition);
                        }
                    }
                }
            }
        }

        private static async Task StartRecognitionAsync(SpeechRecognizer reco, string model, bool useContinuousRecognition)
        {
            if (useContinuousRecognition)
            {
                tcs = new TaskCompletionSource<int>();
                await ContinuousRecognitionAsync(reco, model, tcs).ConfigureAwait(false);
            }
            else
            {
                await SingleShotRecognitionAsync(reco, model).ConfigureAwait(false);
            }
        }

        private static async Task SingleShotRecognitionAsync(SpeechRecognizer reco, string modelId)
        {
            Console.WriteLine("Single-shot recognition.");
            // Sets deployment id of a customized model if needed.
            if (!string.IsNullOrEmpty(modelId))
            {
                reco.DeploymentId = modelId;
            }

            // Subscribes to events.
            reco.IntermediateResultReceived += MyIntermediateResultEventHandler;
            reco.FinalResultReceived += MyFinalResultEventHandler;
            reco.RecognitionErrorRaised += MyErrorEventHandler;
            reco.OnSessionEvent += MySessionEventHandler;

            // Starts recognition.
            var result = await reco.RecognizeAsync().ConfigureAwait(false);

            Console.WriteLine("Speech Recognition: Recognition result: " + result);

            // Unsubscribe to events.
            reco.IntermediateResultReceived -= MyIntermediateResultEventHandler;
            reco.FinalResultReceived -= MyFinalResultEventHandler;
            reco.RecognitionErrorRaised -= MyErrorEventHandler;
            reco.OnSessionEvent -= MySessionEventHandler;
        }

        private static async Task ContinuousRecognitionAsync(SpeechRecognizer reco, string modelId, TaskCompletionSource<int> tcs)
        {
            Console.WriteLine("Continuous recognition.");
            // Sets deployment id of a customized model if needed.
            if (!string.IsNullOrEmpty(modelId))
            {
                reco.DeploymentId = modelId;
            }

            reco.FinalResultReceived += (s, e) =>
            {
                if (e.Result.RecognitionStatus == RecognitionStatus.Recognized)
                {
                    Console.WriteLine($"\n    Final result: Status: {e.Result.RecognitionStatus.ToString()}, Text: {e.Result.Text}.");
                }
                else
                {
                    Console.WriteLine($"\n    Final result: Status: {e.Result.RecognitionStatus.ToString()}, FailureReason: {e.Result.RecognitionFailureReason}.");
                }
            };

            reco.OnSpeechDetectedEvent += (s, e) =>
            {
                Console.WriteLine($"SpeechDetectedEvent received: {e.EventType.ToString()}");
                if (e.EventType == RecognitionEventType.SpeechEndDetectedEvent)
                {
                    Console.WriteLine($"Speech end detected.");
                }
            };

            reco.RecognitionErrorRaised += (s, e) =>
            {
                Console.WriteLine($"\n    An error occurred. Status: {e.Status.ToString()}, FailureReason: {e.FailureReason}");
            };

            reco.OnSessionEvent += (s, e) =>
            {
                Console.WriteLine($"\n    Session event. Event: {e.EventType.ToString()}.");
                if (e.EventType == SessionEventType.SessionStoppedEvent)
                {
                    Console.WriteLine($"Session Stop detected. Stop the recognition.");
                    tcs.TrySetResult(0);
                }
            };

            // Starts continuos recognition. Uses StopContinuousRecognitionAsync() to stop recognition.
            await reco.StartContinuousRecognitionAsync().ConfigureAwait(false);

            // Waits for completion.
            await tcs.Task.ConfigureAwait(false);

            // Stops translation.
            await reco.StopContinuousRecognitionAsync().ConfigureAwait(false);
        }
    }
}
