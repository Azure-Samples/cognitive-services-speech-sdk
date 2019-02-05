//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// <code>
﻿using System;
using System.Threading.Tasks;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;
using Microsoft.CognitiveServices.Speech;
using Microsoft.CognitiveServices.Speech.Translation;

namespace helloworld
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainPage : Page
    {
        public MainPage()
        {
            this.InitializeComponent();
        }

        private async void EnableMicrophone_ButtonClicked(object sender, RoutedEventArgs e)
        {
            bool isMicAvailable = true;
            try
            {
                var mediaCapture = new Windows.Media.Capture.MediaCapture();
                var settings = new Windows.Media.Capture.MediaCaptureInitializationSettings();
                settings.StreamingCaptureMode = Windows.Media.Capture.StreamingCaptureMode.Audio;
                await mediaCapture.InitializeAsync(settings);
            }
            catch (Exception)
            {
                isMicAvailable = false;
            }
            if (!isMicAvailable)
            {
                await Windows.System.Launcher.LaunchUriAsync(new Uri("ms-settings:privacy-microphone"));
            }
            else
            {
                NotifyUser("Microphone was enabled", NotifyType.StatusMessage);
            }
        }

        private async void SpeechTranslationFromMicrophone_ButtonClicked(object sender, RoutedEventArgs e)
        {
            // Creates an instance of a speech config with specified subscription key and service region.
            // Replace with your own subscription key and service region (e.g., "westus").
            var config = SpeechTranslationConfig.FromSubscription("YourSubscriptionKey", "YourSubcriptionRegion");

            // Sets source and target languages.
            string fromLanguage = "en-US";
            config.SpeechRecognitionLanguage = fromLanguage;
            config.AddTargetLanguage("de");

            try
            {
                // Creates a speech recognizer using microphone as audio input.
                using (var recognizer = new TranslationRecognizer(config))
                {
                    // The TaskCompletionSource to stop recognition.
                    var stopRecognition = new TaskCompletionSource<int>();

                    // Subscribes to events.
                    recognizer.Recognizing += (s, ee) =>
                    {
                        NotifyUser($"RECOGNIZING in '{fromLanguage}': Text={ee.Result.Text}", NotifyType.StatusMessage);
                        foreach (var element in ee.Result.Translations)
                        {
                            NotifyUser($"    TRANSLATING into '{element.Key}': {element.Value}", NotifyType.StatusMessage);
                        }
                    };

                    recognizer.Recognized += (s, ee) =>
                    {
                        if (ee.Result.Reason == ResultReason.TranslatedSpeech)
                        {
                            NotifyUser($"\nFinal result: Reason: {ee.Result.Reason.ToString()}, recognized text in {fromLanguage}: {ee.Result.Text}.", NotifyType.StatusMessage);
                            foreach (var element in ee.Result.Translations)
                            {
                                NotifyUser($"    TRANSLATING into '{element.Key}': {element.Value}", NotifyType.StatusMessage);
                            }
                        }
                    };

                    recognizer.Canceled += (s, ee) =>
                    {
                        NotifyUser($"\nRecognition canceled. Reason: {ee.Reason}; ErrorDetails: {ee.ErrorDetails}", NotifyType.StatusMessage);
                    };

                    recognizer.SessionStarted += (s, ee) =>
                    {
                        NotifyUser("\nSession started event.", NotifyType.StatusMessage);
                    };

                    recognizer.SessionStopped += (s, ee) =>
                    {
                        NotifyUser("\nSession stopped event.", NotifyType.StatusMessage);
                        stopRecognition.TrySetResult(0);
                    };

                    // Starts continuous recognition. Uses StopContinuousRecognitionAsync() to stop recognition.
                    await recognizer.StartContinuousRecognitionAsync().ConfigureAwait(false);

                    // Waits for completion.
                    // Use Task.WaitAny to keep the task rooted.
                    Task.WaitAny(new[] { stopRecognition.Task });

                    // Stops continuous recognition.
                    await recognizer.StopContinuousRecognitionAsync().ConfigureAwait(false);
                }
            }
            catch (Exception ex)
            {
                NotifyUser($"{ex.ToString()}", NotifyType.ErrorMessage);
            }
        }

        private enum NotifyType
        {
            StatusMessage,
            ErrorMessage
        };

        private void NotifyUser(string strMessage, NotifyType type)
        {
            // If called from the UI thread, then update immediately.
            // Otherwise, schedule a task on the UI thread to perform the update.
            if (Dispatcher.HasThreadAccess)
            {
                UpdateStatus(strMessage, type);
            }
            else
            {
                var task = Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () => UpdateStatus(strMessage, type));
            }
        }

        private void UpdateStatus(string strMessage, NotifyType type)
        {
            switch (type)
            {
                case NotifyType.StatusMessage:
                    StatusBorder.Background = new SolidColorBrush(Windows.UI.Colors.Green);
                    break;
                case NotifyType.ErrorMessage:
                    StatusBorder.Background = new SolidColorBrush(Windows.UI.Colors.Red);
                    break;
            }
            StatusBlock.Text += string.IsNullOrEmpty(StatusBlock.Text) ? strMessage : "\n" + strMessage;

            // Collapse the StatusBlock if it has no text to conserve real estate.
            StatusBorder.Visibility = !string.IsNullOrEmpty(StatusBlock.Text) ? Visibility.Visible : Visibility.Collapsed;
            if (!string.IsNullOrEmpty(StatusBlock.Text))
            {
                StatusBorder.Visibility = Visibility.Visible;
                StatusPanel.Visibility = Visibility.Visible;
            }
            else
            {
                StatusBorder.Visibility = Visibility.Collapsed;
                StatusPanel.Visibility = Visibility.Collapsed;
            }
            // Raise an event if necessary to enable a screen reader to announce the status update.
            var peer = Windows.UI.Xaml.Automation.Peers.FrameworkElementAutomationPeer.FromElement(StatusBlock);
            if (peer != null)
            {
                peer.RaiseAutomationEvent(Windows.UI.Xaml.Automation.Peers.AutomationEvents.LiveRegionChanged);
            }
        }
    }
}
// </code>
