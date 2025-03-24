//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using System;
using System.Globalization;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Text;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;
using System.Threading.Tasks;
using Windows.Devices.Enumeration;
using Microsoft.CognitiveServices.Speech;
using Microsoft.CognitiveServices.Speech.Audio;
using Windows.Storage;
// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409
namespace MicrosoftSpeechSDKSamples.UwpSpeechSynthesisSample
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainPage : Page
    {
        #region Properties
        /// <summary>
        /// Gets or sets Subscription Key
        /// </summary>
        public string SubscriptionKey
        {
            get => this.subscriptionKey;
            set => this.subscriptionKey = value?.Trim();
        }
        /// <summary>
        /// Gets or sets region name of the service
        /// </summary>
        public string Region { get; set; }

        /// <summary>
        /// Gets or sets synthesis endpoint
        /// </summary>
        public string Endpoint { get; set; }

        /// <summary>
        /// Gets or sets synthesis language
        /// </summary>
        public string SynthesisLanguage { get; set; }
        // Private properties
        private string subscriptionKey;
        #endregion
        public MainPage()
        {
            this.InitializeComponent();
            this.Region = ((ComboBoxItem)RegionComboBox.SelectedItem).Tag.ToString();
            this.SynthesisLanguage = ((ComboBoxItem)LanguageComboBox.SelectedItem).Tag.ToString();
        }
        private async void SpeechSynthesisToSpeaker_ButtonClicked(object sender, RoutedEventArgs e)
        {
            if (!AreKeysValid())
            {
                NotifyUser("Subscription Key is missing!", NotifyType.ErrorMessage);
                return;
            }
            // Creates an instance of a speech config with specified subscription key and endpoint.
            var config = SpeechConfig.FromEndpoint(new Uri(this.Endpoint), this.SubscriptionKey);
            config.SetSpeechSynthesisOutputFormat(SpeechSynthesisOutputFormat.Riff24Khz16BitMonoPcm);
            config.SpeechSynthesisLanguage = this.SynthesisLanguage;

            var deviceConfig = AudioConfig.FromDefaultSpeakerOutput();
            // Using following codes to specify an output device for rendering 
            // var devices = await DeviceInformation.FindAllAsync(DeviceClass.AudioRender);
            // var deviceConfig = AudioConfig.FromSpeakerOutput(devices[0].Id);

            // Creates a speech synthesizer.
            using (var synthesizer = new SpeechSynthesizer(config, deviceConfig))
            {
                // Receive a text from "Text for Synthesizing" text box and synthesize it to speaker.
                using (var result = await synthesizer.SpeakTextAsync(this.TextForSynthesizingTextBox.Text).ConfigureAwait(false))
                {
                    // Checks result.
                    if (result.Reason == ResultReason.SynthesizingAudioCompleted)
                    {
                        NotifyUser($"Speech Synthesis Succeeded.", NotifyType.StatusMessage);
                    }
                    else if (result.Reason == ResultReason.Canceled)
                    {
                        var cancellation = SpeechSynthesisCancellationDetails.FromResult(result);

                        StringBuilder sb = new StringBuilder();
                        sb.AppendLine($"CANCELED: Reason={cancellation.Reason}");
                        sb.AppendLine($"CANCELED: ErrorCode={cancellation.ErrorCode}");
                        sb.AppendLine($"CANCELED: ErrorDetails=[{cancellation.ErrorDetails}]");

                        NotifyUser(sb.ToString(), NotifyType.ErrorMessage);
                    }
                }
            }
        }

        private async void SpeechSynthesisToFile_ButtonClicked()
        {
            if (!AreKeysValid())
            {
                NotifyUser("Subscription Key is missing!", NotifyType.ErrorMessage);
                return;
            }
            else
            {
                NotifyUser(" ", NotifyType.StatusMessage);
            }

            // User can also specify another under the ApplicationData.LocalFolder or Package.InstalledLocation
            var filePath = Path.Combine(ApplicationData.Current.LocalFolder.Path, "outputaudio.wav");

            // Creates an instance of a speech config with specified subscription key and endpoint.
            var config = SpeechConfig.FromEndpoint(new Uri(this.Endpoint), this.SubscriptionKey);

            config.SpeechSynthesisLanguage = this.SynthesisLanguage;

            // Creates a speech synthesizer using file as audio output.
            using (var audioOutput = AudioConfig.FromWavFileOutput(filePath))
            using (var synthesizer = new SpeechSynthesizer(config, audioOutput))
            {
                // Subscribes to events.
                synthesizer.SynthesisStarted += (s, e) =>
                {
                    NotifyUser($"Speech synthesis started.", NotifyType.StatusMessage);
                };

                synthesizer.Synthesizing += (s, e) =>
                {
                    NotifyUser($"{e.Result.AudioData.Length} bytes received.", NotifyType.StatusMessage);
                };

                synthesizer.SynthesisCompleted += (s, e) =>
                {
                    NotifyUser($"Speech synthesis completed.", NotifyType.StatusMessage);
                };

                synthesizer.SynthesisCanceled += (s, e) =>
                {
                    var cancellation = SpeechSynthesisCancellationDetails.FromResult(e.Result);

                    StringBuilder sb = new StringBuilder();
                    sb.AppendLine($"CANCELED: Reason={cancellation.Reason}");
                    sb.AppendLine($"CANCELED: ErrorCode={cancellation.ErrorCode}");
                    sb.AppendLine($"CANCELED: ErrorDetails=[{cancellation.ErrorDetails}]");

                    NotifyUser(sb.ToString(), NotifyType.ErrorMessage);
                };

                // Waits for completion.
                using (var result = await synthesizer.SpeakTextAsync(this.TextForSynthesizingTextBox.Text).ConfigureAwait(false))
                {
                    if (result.Reason == ResultReason.SynthesizingAudioCompleted)
                    {
                        NotifyUser($"Speech synthesis completed. The audio has been saved to {filePath}.", NotifyType.StatusMessage);
                    }
                }
            }
        }

        private async void SpeechSynthesisToStream_ButtonClicked()
        {
            if (!AreKeysValid())
            {
                NotifyUser("Subscription Key is missing!", NotifyType.ErrorMessage);
                return;
            }
            else
            {
                NotifyUser(" ", NotifyType.StatusMessage);
            }

            // Creates an instance of a speech config with specified subscription key and endpoint.
            var config = SpeechConfig.FromEndpoint(new Uri(this.Endpoint), this.SubscriptionKey);

            config.SpeechSynthesisLanguage = this.SynthesisLanguage;

            // Creates a speech synthesizer using the config.
            using (var synthesizer = new SpeechSynthesizer(config, null))
            {
                // Subscribes to events.
                synthesizer.SynthesisStarted += (s, e) =>
                {
                    NotifyUser($"Speech synthesis started.", NotifyType.StatusMessage);
                };

                synthesizer.Synthesizing += (s, e) =>
                {
                    NotifyUser($"{e.Result.AudioData.Length} bytes received.", NotifyType.StatusMessage);
                };

                synthesizer.SynthesisCompleted += (s, e) =>
                {
                    NotifyUser($"Speech synthesis completed.", NotifyType.StatusMessage);
                };

                synthesizer.SynthesisCanceled += (s, e) =>
                {
                    var cancellation = SpeechSynthesisCancellationDetails.FromResult(e.Result);

                    StringBuilder sb = new StringBuilder();
                    sb.AppendLine($"CANCELED: Reason={cancellation.Reason}");
                    sb.AppendLine($"CANCELED: ErrorCode={cancellation.ErrorCode}");
                    sb.AppendLine($"CANCELED: ErrorDetails=[{cancellation.ErrorDetails}]");
                    sb.AppendLine($"CANCELED: Did you update the subscription info?");

                    NotifyUser(sb.ToString(), NotifyType.ErrorMessage);
                };

                var text = this.TextForSynthesizingTextBox.Text;

                // Waits for completion.
                using (var result = await synthesizer.SpeakTextAsync(text).ConfigureAwait(false))
                {
                    using (var stream = AudioDataStream.FromResult(result))
                    {
                        byte[] buffer = new byte[16000];
                        uint totalSize = 0;
                        uint filledSize = 0;

                        while ((filledSize = stream.ReadData(buffer)) > 0)
                        {
                            NotifyUser($"{filledSize} bytes received.", NotifyType.StatusMessage);
                            totalSize += filledSize;
                        }

                        NotifyUser($"{totalSize} bytes of audio data received for text [{text}]", NotifyType.StatusMessage);
                    }
                }
            }
        }

        private void RegionCombox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            this.Region = ((ComboBoxItem)RegionComboBox.SelectedItem).Tag.ToString();
        }
        private void LanguageCombox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            this.SynthesisLanguage = ((ComboBoxItem)LanguageComboBox.SelectedItem).Tag.ToString();
        }
        private void SubscriptionKey_TextChanged(object sender, TextChangedEventArgs e)
        {
            this.subscriptionKey = subscriptionKeyTextBox.Text;
        }
        private enum NotifyType
        {
            StatusMessage,
            ErrorMessage
        };
        private bool AreKeysValid()
        {
            if (string.IsNullOrEmpty(this.subscriptionKey) || string.IsNullOrEmpty(this.SynthesisLanguage))
            {
                return false;
            }
            return true;
        }
        /// <summary>
        /// Display a message to the user.
        /// This method may be called from any thread.
        /// </summary>
        /// <param name="strMessage"></param>
        /// <param name="type"></param>
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
            StatusBlock.Text = strMessage;
            // Collapse the StatusBlock if it has no text to conserve real estate.
            StatusBorder.Visibility = (StatusBlock.Text != String.Empty) ? Visibility.Visible : Visibility.Collapsed;
            if (StatusBlock.Text != String.Empty)
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
            peer?.RaiseAutomationEvent(Windows.UI.Xaml.Automation.Peers.AutomationEvents.LiveRegionChanged);
        }
    }
}
