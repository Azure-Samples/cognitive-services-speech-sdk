//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using Microsoft.CognitiveServices.Speech;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Xamarin.Forms;

namespace kws_xamarin
{
    // Learn more about making custom code visible in the Xamarin.Forms previewer
    // by visiting https://aka.ms/xamarinforms-previewer
    [DesignTimeVisible(false)]
    public partial class MainPage : ContentPage
    {
        private string kwsModelDir;
        private string kwsModelFile = "kws.table";

        public MainPage()
        {
            InitializeComponent();
        }

        private async void OnRecognitionButtonWithKeywordClicked(object sender, EventArgs args)
        {
            try
            {
                // Creates an instance of a speech config with specified subscription key and service region.
                // Replace with your own subscription key and service region (e.g., "westus").
                var config = SpeechConfig.FromSubscription("YourSubscriptionKey", "YourRegion");

                kwsModelDir = DependencyService.Get<IAssetService>().GetAssetPath(kwsModelFile);
                var model = KeywordRecognitionModel.FromFile(kwsModelDir);

                // The phrase your keyword recognition model triggers on.
                var keyword = "Computer";

                var stopRecognition = new TaskCompletionSource<int>(TaskCreationOptions.RunContinuationsAsynchronously);
                var resultStr = "";

                // Creates a speech recognizer using microphone as audio input.
                using (var recognizer = new SpeechRecognizer(config))
                {
                    // Subscribes to events.
                    recognizer.Recognized += (s, e) =>
                    {
                        if (e.Result.Reason == ResultReason.RecognizedKeyword)
                        {
                            resultStr = $"RECOGNIZED KEYWORD: '{e.Result.Text}'";
                        }
                        else if (e.Result.Reason == ResultReason.RecognizedSpeech)
                        {
                            resultStr = $"RECOGNIZED: '{e.Result.Text}'";
                        }
                        else if (e.Result.Reason == ResultReason.NoMatch)
                        {
                            resultStr = "NOMATCH: Speech could not be recognized.";
                        }
                        Debug.WriteLine(resultStr);
                        UpdateUI(resultStr);
                    };

                    recognizer.Canceled += (s, e) =>
                    {
                        var cancellation = CancellationDetails.FromResult(e.Result);
                        resultStr = $"CANCELED: Reason={cancellation.Reason} ErrorDetails={cancellation.ErrorDetails}";
                        if (cancellation.Reason == CancellationReason.Error)
                        {
                            UpdateUI(resultStr);
                        }
                        Debug.WriteLine(resultStr);
                        stopRecognition.TrySetResult(0);
                    };

                    recognizer.SessionStarted += (s, e) =>
                    {
                        Debug.WriteLine("\nSession started event.");
                    };

                    recognizer.SessionStopped += (s, e) =>
                    {
                        Debug.WriteLine("\nSession stopped event.");
                        Debug.WriteLine("\nStop recognition.");
                        stopRecognition.TrySetResult(0);
                    };

                    Debug.WriteLine($"Say something starting with the keyword '{keyword}' followed by whatever you want...");

                    // Starts continuous recognition using the keyword model. Use StopKeywordRecognitionAsync() to stop recognition.
                    await recognizer.StartKeywordRecognitionAsync(model).ConfigureAwait(false);

                    // Waits for a single successful keyword-triggered speech recognition (or error).
                    // Use Task.WaitAny to keep the task rooted.
                    Task.WaitAny(new[] { stopRecognition.Task });

                    await recognizer.StopKeywordRecognitionAsync().ConfigureAwait(false);
                }
            }
            catch (Exception ex)
            {
                UpdateUI("Exception: " + ex.ToString());
            }
        }

        private async void OnEnableMicrophoneButtonClicked(object sender, EventArgs e)
        {
            bool micAccessGranted = await DependencyService.Get<IMicrophoneService>().GetPermissionsAsync();
            if (!micAccessGranted)
            {
                UpdateUI("Please give access to microphone");
            }
        }

        private void UpdateUI(String message)
        {
            Device.BeginInvokeOnMainThread(() =>
            {
                RecognitionText.Text = message;
            });
        }
    }
}
