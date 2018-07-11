//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;
using Microsoft.CognitiveServices.Speech;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

namespace CsharpHelloSpeech
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

        private async void SpeechRecognitionFromMicrophone_ButtonClicked(object sender, RoutedEventArgs e)
        {
            // Creates an instance of a speech factory with specified subscription key and service region.
            // Replace with your own subscription key and service region (e.g., "westus").
            var factory = SpeechFactory.FromSubscription("YourSubscriptionKey", "YourServiceRegion");

            try
            {
                // Creates a speech recognizer using microphone as audio input. The default language is "en-us".
                using (var recognizer = factory.CreateSpeechRecognizer())
                {
                    // Starts recognition. It returns when the first utterance has been recognized.
                    var result = await recognizer.RecognizeAsync().ConfigureAwait(false);
                    // Checks result.
                    string str;
                    if (result.RecognitionStatus != RecognitionStatus.Recognized)
                    {
                        str = $"Recognition status: {result.RecognitionStatus.ToString()}";
                        if (result.RecognitionStatus == RecognitionStatus.Canceled)
                        {
                            str = $"There was an error, reason: {result.RecognitionFailureReason}";
                        }
                        else
                        {
                            str = "No speech could be recognized.";
                        }
                    }
                    else
                    {
                        str = $"We recognized: {result.Text}, Offset: {result.OffsetInTicks}, Duration: {result.Duration}.";
                    }
                    NotifyUser(str, NotifyType.StatusMessage);
                }
            }
            catch(Exception ex)
            {
                NotifyUser($"Enable Microphone First.\n {ex.ToString()}", NotifyType.ErrorMessage);
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
