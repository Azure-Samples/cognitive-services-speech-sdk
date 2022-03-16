//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using Microsoft.CognitiveServices.Speech;
using Microsoft.CognitiveServices.Speech.Audio;
using Microsoft.CognitiveServices.Speech.Dialog;
using System;
using System.Diagnostics;
using System.IO;
using System.Text;
using Windows.Foundation;
using Windows.Storage.Streams;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;

namespace helloworld
{
    public sealed partial class MainPage : Page
    {
        private DialogServiceConnector connector;

        private enum NotifyType
        {
            StatusMessage,
            ErrorMessage
        };

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

        private void NotifyUser(string strMessage, NotifyType type = NotifyType.StatusMessage)
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

        // Waits for accumulates all audio associated with a given PullAudioOutputStream and then plays it to the
        // MediaElement. Long spoken audio will create extra latency and a streaming playback solution (that plays
        // audio while it continues to be received) should be used -- see the samples for examples of this.
        private void SynchronouslyPlayActivityAudio(PullAudioOutputStream activityAudio)
        {
            var playbackStreamWithHeader = new MemoryStream();
            playbackStreamWithHeader.Write(Encoding.ASCII.GetBytes("RIFF"), 0, 4); // ChunkID
            playbackStreamWithHeader.Write(BitConverter.GetBytes(UInt32.MaxValue), 0, 4); // ChunkSize: max
            playbackStreamWithHeader.Write(Encoding.ASCII.GetBytes("WAVE"), 0, 4); // Format
            playbackStreamWithHeader.Write(Encoding.ASCII.GetBytes("fmt "), 0, 4); // Subchunk1ID
            playbackStreamWithHeader.Write(BitConverter.GetBytes(16), 0, 4); // Subchunk1Size: PCM
            playbackStreamWithHeader.Write(BitConverter.GetBytes(1), 0, 2); // AudioFormat: PCM
            playbackStreamWithHeader.Write(BitConverter.GetBytes(1), 0, 2); // NumChannels: mono
            playbackStreamWithHeader.Write(BitConverter.GetBytes(16000), 0, 4); // SampleRate: 16kHz
            playbackStreamWithHeader.Write(BitConverter.GetBytes(32000), 0, 4); // ByteRate
            playbackStreamWithHeader.Write(BitConverter.GetBytes(2), 0, 2); // BlockAlign
            playbackStreamWithHeader.Write(BitConverter.GetBytes(16), 0, 2); // BitsPerSample: 16-bit
            playbackStreamWithHeader.Write(Encoding.ASCII.GetBytes("data"), 0, 4); // Subchunk2ID
            playbackStreamWithHeader.Write(BitConverter.GetBytes(UInt32.MaxValue), 0, 4); // Subchunk2Size

            byte[] pullBuffer = new byte[2056];

            uint lastRead = 0;
            do
            {
                lastRead = activityAudio.Read(pullBuffer);
                playbackStreamWithHeader.Write(pullBuffer, 0, (int)lastRead);
            }
            while (lastRead == pullBuffer.Length);

            var task = Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
            {
                mediaElement.SetSource(playbackStreamWithHeader.AsRandomAccessStream(), "audio/wav");
                mediaElement.Play();
            });
        }

        private void InitializeDialogServiceConnector()
        {
            // Create a DialogServiceConfig by providing a Cognitive Services subscription key and key region.
            // The RecoLanguage property is optional (default en-US).
            const string speechSubscriptionKey = "YourSpeechSubscriptionKey"; // Your subscription key
            const string region = "YourServiceRegion"; // Your subscription service region. Note: only 'westus2' is currently supported

            var botConfig = BotFrameworkConfig.FromSubscription(speechSubscriptionKey, region);
            botConfig.SetProperty(PropertyId.SpeechServiceConnection_RecoLanguage, "en-US");
            connector = new DialogServiceConnector(botConfig);

            // ActivityReceived is the main way your bot will communicate with the client and uses bot framework activities
            connector.ActivityReceived += (sender, activityReceivedEventArgs) =>
            {
                NotifyUser($"Activity received, hasAudio={activityReceivedEventArgs.HasAudio} activity={activityReceivedEventArgs.Activity}");

                if (activityReceivedEventArgs.HasAudio)
                {
                    SynchronouslyPlayActivityAudio(activityReceivedEventArgs.Audio);
                }
            };
            // Canceled will be signaled when a turn is aborted or experiences an error condition
            connector.Canceled += (sender, canceledEventArgs) =>
            {
                NotifyUser($"Canceled, reason={canceledEventArgs.Reason}");
                if (canceledEventArgs.Reason == CancellationReason.Error)
                {
                    NotifyUser($"Error: code={canceledEventArgs.ErrorCode}, details={canceledEventArgs.ErrorDetails}");
                }
            };
            // Recognizing (not 'Recognized') will provide the intermediate recognized text while an audio stream is being processed
            connector.Recognizing += (sender, recognitionEventArgs) =>
            {
                NotifyUser($"Recognizing! in-progress text={recognitionEventArgs.Result.Text}");
            };
            // Recognized (not 'Recognizing') will provide the final recognized text once audio capture is completed
            connector.Recognized += (sender, recognitionEventArgs) =>
            {
                NotifyUser($"Final speech-to-text result: '{recognitionEventArgs.Result.Text}'");
            };
            // SessionStarted will notify when audio begins flowing to the service for a turn
            connector.SessionStarted += (sender, sessionEventArgs) =>
            {
                NotifyUser($"Now Listening! Session started, id={sessionEventArgs.SessionId}");
            };
            // SessionStopped will notify when a turn is complete and it's safe to begin listening again
            connector.SessionStopped += (sender, sessionEventArgs) =>
            {
                NotifyUser($"Listening complete. Session ended, id={sessionEventArgs.SessionId}");
            };
        }

        private async void ListenButton_ButtonClicked(object sender, RoutedEventArgs e)
        {
            if (connector == null)
            {
                InitializeDialogServiceConnector();
                // Optional step to speed up first interaction: if not called, connection happens automatically on first use
                var connectTask = connector.ConnectAsync();
            }

            try
            {
                // Start sending audio to your speech-enabled bot
                var listenTask = connector.ListenOnceAsync();

                // You can also send activities to your bot as JSON strings -- Microsoft.Bot.Schema can simplify this
                string speakActivity = @"{""type"":""message"",""text"":""Greeting Message"", ""speak"":""Hello there!""}";
                await connector.SendActivityAsync(speakActivity);

            }
            catch (Exception ex)
            {
                NotifyUser($"Exception: {ex.ToString()}", NotifyType.ErrorMessage);
            }
        }
    }
}
