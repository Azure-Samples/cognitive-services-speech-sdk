//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// carbon_csharp_console.cs: A console application for testing Carbon C# client library.
//

using System;
using System.Globalization;
using System.Threading.Tasks;
using Microsoft.CognitiveServices.Speech;
using Microsoft.CognitiveServices.Speech.Audio;
using Microsoft.CognitiveServices.Speech.Intent;

namespace MicrosoftSpeechSDKSamples
{
    public class IntentRecognitionSamples
    {
        private static void MyRecognizingEventHandler(object sender, IntentRecognitionEventArgs e)
        {
            Console.WriteLine(String.Format(CultureInfo.InvariantCulture, "Intent recognition: intermediate result: {0} ", e.ToString()));
        }

        private static void MyRecognizedEventHandler(object sender, IntentRecognitionEventArgs e)
        {
            if (e.Result.Reason == ResultReason.RecognizedIntent)
            {
                Console.WriteLine($"Intent Recognized: IntentId={e.Result.IntentId}, Text={e.Result.Text}, Offset={e.Result.OffsetInTicks}, Duration={e.Result.Duration}");
            }
            else if (e.Result.Reason == ResultReason.RecognizedSpeech)
            {
                Console.WriteLine($"ONLY Speech recognized : Text={e.Result.Text}, Offset={e.Result.OffsetInTicks}, Duration={e.Result.Duration}");
            }
            else if (e.Result.Reason == ResultReason.NoMatch)
            {
                Console.WriteLine($"NOMATCH: Speech could not be recognized. Reason={NoMatchDetails.FromResult(e.Result).Reason}, Offset={e.Result.OffsetInTicks}, Duration={e.Result.Duration}");
            }
            else
            {
                Console.WriteLine($"Unexpected result. Reason={e.Result.Reason}, result={e.Result}");
            }
        }

        private static void MyCanceledEventHandler(object sender, IntentRecognitionCanceledEventArgs e)
        {
            Console.WriteLine($"CANCELED: Reason={e.Reason}");

            if (e.Reason == CancellationReason.Error)
            {
                Console.WriteLine($"CANCELED: ErrorCode={e.ErrorCode}");
                Console.WriteLine($"CANCELED: ErrorDetails={e.ErrorDetails}");
            }
        }

        private static void MySpeechStartDetectedEventHandler(object sender, RecognitionEventArgs e)
        { 
            Console.WriteLine($"SpeechStartDetected received: offset: {e.Offset}.");
        }

        private static void MySpeechEndDetectedEventHandler(object sender, RecognitionEventArgs e)
        {
            Console.WriteLine($"SpeechEndDetected received: offset: {e.Offset}.");
        }

        private static void MySessionStartedEventHandler(object sender, SessionEventArgs e)
        {
            Console.WriteLine(String.Format(CultureInfo.InvariantCulture, "Intent recognition: Session started event: {0}.", e.ToString()));
        }

        public static async Task IntentRecognitionBaseModelAsync(string keySpeech, string region, string fileName, bool useContinuousRecognition)
        {
            Console.WriteLine("Intent Recognition using base speech model.");

            var config = SpeechConfig.FromSubscription(keySpeech, region);
            if ((fileName == null) || String.Compare(fileName, "mic", true) == 0)
            {
                using (var reco = new IntentRecognizer(config))
                {
                    await DoIntentRecognitionAsync(reco, useContinuousRecognition).ConfigureAwait(false);
                }
            }
            else
            {
                var audioInput = AudioConfig.FromWavFileInput(fileName);
                using (var reco = new IntentRecognizer(config, audioInput))
                {
                    await DoIntentRecognitionAsync(reco, useContinuousRecognition).ConfigureAwait(false);
                }
            }
        }

        public static async Task IntentRecognitionByEndpointAsync(string subKey, string endpoint, string fileName, bool useContinuousRecognition)
        {
            var config = SpeechConfig.FromEndpoint(new Uri(endpoint), subKey);

            Console.WriteLine(String.Format(CultureInfo.InvariantCulture, "Intent Recognition using endpoint:{0}.", endpoint));

            if ((fileName == null) || String.Compare(fileName, "mic", true) == 0)
            {
                using (var reco = new IntentRecognizer(config))
                {
                    await DoIntentRecognitionAsync(reco, useContinuousRecognition).ConfigureAwait(false);
                }
            }
            else
            {
                var audioInput = AudioConfig.FromWavFileInput(fileName);
                using (var reco = new IntentRecognizer(config, audioInput))
                {
                    await DoIntentRecognitionAsync(reco, useContinuousRecognition).ConfigureAwait(false);
                }
            }
        }

        private static async Task DoIntentRecognitionAsync(IntentRecognizer reco, bool useContinuousRecognition)
        {
            string intentAppId = "b687b851-56c5-4d31-816f-35a741a3f0be";
            var model = LanguageUnderstandingModel.FromAppId(intentAppId);
            reco.AddIntent(model, "HomeAutomation.TurnOn", "HomeAutomationTurnOn");

            if (useContinuousRecognition)
            {
                await ContinuousIntentRecognitionAsync(reco);
            }
            else
            {
                await SingleShotIntentRecognitionAsync(reco);
            }
        }

        private static async Task SingleShotIntentRecognitionAsync(IntentRecognizer reco)
        { 
            // Subscribes to events.
            reco.Recognizing += MyRecognizingEventHandler;
            reco.Recognized += MyRecognizedEventHandler;
            reco.Canceled += MyCanceledEventHandler;
            reco.SpeechStartDetected += MySpeechStartDetectedEventHandler;
            reco.SpeechEndDetected += MySpeechEndDetectedEventHandler;
            reco.SessionStarted += MySessionStartedEventHandler;
            reco.SessionStopped += (s, e) =>
            {
                Console.WriteLine(String.Format(CultureInfo.InvariantCulture, "Intent recognition: Session stopped event: {0}.", e.ToString()));
            };

            // Starts recognition.
            var result = await reco.RecognizeOnceAsync().ConfigureAwait(false);

            Console.WriteLine("Intent Recognition: recognition result: " + result);

            // Unsubscribe to events.
            reco.Recognizing -= MyRecognizingEventHandler;
            reco.Recognized -= MyRecognizedEventHandler;
            reco.Canceled -= MyCanceledEventHandler;
            reco.SessionStarted -= MySessionStartedEventHandler;
            reco.SpeechStartDetected -= MySpeechStartDetectedEventHandler;
            reco.SpeechEndDetected -= MySpeechEndDetectedEventHandler;
        }

        private static async Task ContinuousIntentRecognitionAsync(IntentRecognizer reco)
        {
            Console.WriteLine("Continuous recognition.");
            var tcsLocal = new TaskCompletionSource<bool>();

            reco.Recognized += MyRecognizedEventHandler;
            reco.Canceled += (s, e) =>
            {
                MyCanceledEventHandler(s, e);
                tcsLocal.TrySetResult(false);
            };

            reco.SpeechStartDetected += MySpeechStartDetectedEventHandler;
            reco.SpeechEndDetected += MySpeechEndDetectedEventHandler;
            reco.SessionStarted += MySessionStartedEventHandler;
            reco.SessionStopped += (s, e) =>
            {
                Console.WriteLine($"Session Stop detected. Event: {e.ToString()}. Stop the recognition.");
                tcsLocal.TrySetResult(true);
            };

            // Starts continuos recognition. Uses StopContinuousRecognitionAsync() to stop recognition.
            await reco.StartContinuousRecognitionAsync().ConfigureAwait(false);

            // Waits for completion.
            // Use Task.WaitAny to make sure that the task is rooted.
            Task.WaitAny(new[] { tcsLocal.Task });

            // Stops translation.
            await reco.StopContinuousRecognitionAsync().ConfigureAwait(false);
        }

    }

}
