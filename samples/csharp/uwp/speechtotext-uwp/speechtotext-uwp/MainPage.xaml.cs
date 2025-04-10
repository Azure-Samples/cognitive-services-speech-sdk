//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using System;
using System.Globalization;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net;
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
using Microsoft.CognitiveServices.Speech;
using Microsoft.CognitiveServices.Speech.Audio;
using Windows.Storage;
// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409
namespace MicrosoftSpeechSDKSamples.UwpSpeechRecognitionSample
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
            get
            {
                return this.subscriptionKey;
            }
            set
            {
                this.subscriptionKey = value?.Trim();
            }
        }

        /// <summary>
        /// Gets or sets endpoint of the service
        /// </summary>
        public string Endpoint { get; set; }

        /// <summary>
        /// Gets or sets recognition language
        /// </summary>
        public string RecognitionLanguage { get; set; }
        // Private properties
        private string subscriptionKey;
        #endregion
        public MainPage()
        {
            this.InitializeComponent();
            this.RecognitionLanguage = ((ComboBoxItem)LanguageComboBox.SelectedItem).Tag.ToString();
        }
        private async void SpeechRecognitionFromMicrophone_ButtonClicked(object sender, RoutedEventArgs e)
        {
            if (!AreKeysValid())
            {
                NotifyUser("Subscription Key is missing!", NotifyType.ErrorMessage);
                return;
            }
            // Creates an instance of a speech config with specified subscription key and endpoint.
            var config = SpeechConfig.FromEndpoint(new Uri(this.Endpoint), this.SubscriptionKey);
            config.SpeechRecognitionLanguage = this.RecognitionLanguage;

            // Creates a speech recognizer using microphone as audio input.
            using (var recognizer = new SpeechRecognizer(config))
            {
                // Starts speech recognition, and returns after a single utterance is recognized. The end of a
                // single utterance is determined by listening for silence at the end or until a maximum of about 30
                // seconds of audio is processed.  The task returns the recognition text as result.
                // Note: Since RecognizeOnceAsync() returns only a single utterance, it is suitable only for single
                // shot recognition like command or query.
                // For long-running multi-utterance recognition, use StartContinuousRecognitionAsync() instead.
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                // Checks result.
                string str;
                if (result.Reason != ResultReason.RecognizedSpeech)
                {
                    str = $"Speech Recognition Failed. '{result.Reason.ToString()}'";
                }
                else
                {
                    str = $"Recognized: '{result.Text}'";
                }
                NotifyUser(str, NotifyType.StatusMessage);
            }
        }
        // The TaskCompletionSource to stop recognition.
        private TaskCompletionSource<int> stopRecognitionTaskCompletionSource;
        private async void SpeechRecognitionFromFile_ButtonClicked()
        {
            stopRecognitionTaskCompletionSource = new TaskCompletionSource<int>(TaskCreationOptions.RunContinuationsAsynchronously);
            if (!AreKeysValid())
            {
                NotifyUser("Subscription Key is missing!", NotifyType.ErrorMessage);
                return;
            }
            else
            {
                NotifyUser(" ", NotifyType.StatusMessage);
            }
            // User can also specify files under the ApplicationData.LocalFolder or Package.InstalledLocation
            string filename = "ms-appx:///Assets/whatstheweatherlike.wav";
            StorageFile file = await StorageFile.GetFileFromApplicationUriAsync(new Uri(filename));
            if (file != null)
            {
                // Creates an instance of a speech config with specified and service region (e.g., "westus").
                var config = SpeechConfig.FromEndpoint(new Uri(this.Endpoint), this.SubscriptionKey);
                config.SpeechRecognitionLanguage = this.RecognitionLanguage;

                // Creates a speech recognizer using file as audio input.
                using (var audioInput = AudioConfig.FromWavFileInput(file.Path))
                {
                    using (var recognizer = new SpeechRecognizer(config, audioInput))
                    {
                        // Subscribes to events.
                        recognizer.Recognizing += (s, e) =>
                        {
                            NotifyUser(e.Result.Text, NotifyType.StatusMessage);
                        };
                        recognizer.Recognized += (s, e) =>
                        {
                            string str = "";
                            if (e.Result.Reason == ResultReason.RecognizedSpeech)
                            {
                                str = $"RECOGNIZED: Text={e.Result.Text}";
                            }
                            else if (e.Result.Reason == ResultReason.NoMatch)
                            {
                                str = $"NOMATCH: Speech could not be recognized.";
                            }
                            NotifyUser(str, NotifyType.StatusMessage);
                        };
                        recognizer.Canceled += (s, e) =>
                        {
                            StringBuilder sb = new StringBuilder();
                            sb.AppendLine($"CANCELED: Reason={e.Reason}");

                            if (e.Reason == CancellationReason.Error)
                            {
                                sb.AppendLine($"CANCELED: ErrorCode={e.ErrorCode}");
                                sb.AppendLine($"CANCELED: ErrorDetails={e.ErrorDetails}");
                                sb.AppendLine($"CANCELED: Did you update the subscription info?");
                            }

                            NotifyUser(sb.ToString(), NotifyType.StatusMessage);
                        };
                        recognizer.SessionStarted += (s, e) =>
                        {
                            NotifyUser("Session started event.", NotifyType.StatusMessage);
                        };
                        recognizer.SessionStopped += (s, e) =>
                        {
                            NotifyUser("Session stopped event.", NotifyType.StatusMessage);
                            NotifyUser("Stop recognition.", NotifyType.StatusMessage);
                            stopRecognitionTaskCompletionSource.TrySetResult(0);
                        };
                        // Starts continuous recognition. Uses StopContinuousRecognitionAsync() to stop recognition.
                        await recognizer.StartContinuousRecognitionAsync().ConfigureAwait(false);
                        // Waits for completion.
                        await stopRecognitionTaskCompletionSource.Task.ConfigureAwait(false);
                        // Stops recognition.
                        await recognizer.StopContinuousRecognitionAsync().ConfigureAwait(false);
                    }
                }
            }
            else
            {
                NotifyUser($"Can't open {filename} !", NotifyType.ErrorMessage);
            }
        }
        private async void SpeechRecognitionFromStream_ButtonClicked(object sender, RoutedEventArgs e)
        {
            stopRecognitionTaskCompletionSource = new TaskCompletionSource<int>(TaskCreationOptions.RunContinuationsAsynchronously);
            AudioConfig audioInput = null;
            BinaryReader reader = null;
            Stream stream = null;

            if (!AreKeysValid())
            {
                NotifyUser("Subscription Key is missing!", NotifyType.ErrorMessage);
                return;
            }
            else
            {
                NotifyUser(" ", NotifyType.StatusMessage);
            }

            var picker = new Windows.Storage.Pickers.FileOpenPicker();
            picker.FileTypeFilter.Add(".wav");
            StorageFile file = await picker.PickSingleFileAsync();
            if (file == null)
            {
                // When you click "Cancel" in the file picker Window
                string s = string.Format(CultureInfo.InvariantCulture, "You need to select a WAV file!");
                NotifyUser(s, NotifyType.ErrorMessage);
                return;
            }
            try
            {
                stream = (await file.OpenReadAsync()).AsStreamForRead();
                reader = new BinaryReader(stream);

                // Create an audio stream from a wav file.
                audioInput = MicrosoftSpeechSDKSamples.Helper.OpenWavFile(reader);

                // Creates an instance of a speech config with specified subscription key and endpoint.
                var config = SpeechConfig.FromEndpoint(new Uri(this.Endpoint), this.SubscriptionKey);
                config.SpeechRecognitionLanguage = this.RecognitionLanguage;

                // Creates a speech recognizer using file as audio input.
                using (var recognizer = new SpeechRecognizer(config, audioInput))
                {
                    // Subscribes to events.
                    recognizer.Recognizing += (s, ee) =>
                    {
                        NotifyUser(ee.Result.Text, NotifyType.StatusMessage);
                    };
                    recognizer.Recognized += (s, ee) =>
                    {
                        string str = "";
                        if (ee.Result.Reason == ResultReason.RecognizedSpeech)
                        {
                            str = $"RECOGNIZED: Text={ee.Result.Text}";
                        }
                        else if (ee.Result.Reason == ResultReason.NoMatch)
                        {
                            str = $"NOMATCH: Speech could not be recognized.";
                        }
                        NotifyUser(str, NotifyType.StatusMessage);
                    };
                    recognizer.Canceled += (s, ee) =>
                    {
                        StringBuilder sb = new StringBuilder();
                        sb.AppendLine($"CANCELED: Reason={ee.Reason}");

                        if (ee.Reason == CancellationReason.Error)
                        {
                            sb.AppendLine($"CANCELED: ErrorCode={ee.ErrorCode}");
                            sb.AppendLine($"CANCELED: ErrorDetails={ee.ErrorDetails}");
                            sb.AppendLine($"CANCELED: Did you update the subscription info?");
                        }

                        NotifyUser(sb.ToString(), NotifyType.StatusMessage);
                    };
                    recognizer.SessionStarted += (s, ee) =>
                    {
                        NotifyUser("Session started event.", NotifyType.StatusMessage);
                    };
                    recognizer.SessionStopped += (s, ee) =>
                    {
                        NotifyUser("Session stopped event.", NotifyType.StatusMessage);
                        NotifyUser("Stop recognition.", NotifyType.StatusMessage);
                        stopRecognitionTaskCompletionSource.TrySetResult(0);
                    };
                    // Starts continuous recognition. Uses StopContinuousRecognitionAsync() to stop recognition.
                    await recognizer.StartContinuousRecognitionAsync().ConfigureAwait(false);
                    // Waits for completion.
                    await stopRecognitionTaskCompletionSource.Task.ConfigureAwait(false);
                    // Stops recognition.
                    await recognizer.StopContinuousRecognitionAsync().ConfigureAwait(false);
                }
            }
            catch (System.FormatException ex)
            {
                NotifyUser(ex.ToString(), NotifyType.ErrorMessage);
            }
            finally
            {
                if (reader != null)
                {
                    reader.Dispose();
                }
                if (audioInput != null)
                {
                    audioInput.Dispose();
                }
                if (stream != null)
                {
                    stream.Dispose();
                }
            }
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
        private void Endpoint_TextChanged(object sender, TextChangedEventArgs e)
        {
            this.Endpoint = endpointTextBox.Text;
        }
        private void LanguageCombox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            this.RecognitionLanguage = ((ComboBoxItem)LanguageComboBox.SelectedItem).Tag.ToString();
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
            if (string.IsNullOrEmpty(this.subscriptionKey) || string.IsNullOrEmpty(this.RecognitionLanguage))
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
            if (peer != null)
            {
                peer.RaiseAutomationEvent(Windows.UI.Xaml.Automation.Peers.AutomationEvents.LiveRegionChanged);
            }
        }
    }
}
