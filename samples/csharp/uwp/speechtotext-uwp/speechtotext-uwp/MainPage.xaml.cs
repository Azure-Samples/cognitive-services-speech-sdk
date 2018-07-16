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
using System.Threading.Tasks;
using Microsoft.CognitiveServices.Speech;
using Windows.Storage;
// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409
namespace CSharpUWPSample
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
        /// Gets or sets region name of the service
        /// </summary>
        public string Region { get; set; }
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
            this.Region = ((ComboBoxItem)RegionComboBox.SelectedItem).Tag.ToString();
            this.RecognitionLanguage = ((ComboBoxItem)LanguageComboBox.SelectedItem).Tag.ToString();
        }
        private async void SpeechRecognitionFromMicrophone_ButtonClicked(object sender, RoutedEventArgs e)
        {
            if (!AreKeysValid())
            {
                NotifyUser("Subscription Key is missing!", NotifyType.ErrorMessage);
                return;
            }
            // Creates an instance of a speech factory with specified
            var factory = SpeechFactory.FromSubscription(this.SubscriptionKey, this.Region);
            // Creates a speech recognizer using microphone as audio input. The default language is "en-us".
            using (var recognizer = factory.CreateSpeechRecognizer(this.RecognitionLanguage))
            {
                // Starts recognition. It returns when the first utterance has been recognized.
                var result = await recognizer.RecognizeAsync().ConfigureAwait(false);
                // Checks result.
                string str;
                if (result.RecognitionStatus != RecognitionStatus.Recognized)
                {
                    str = $"Speech Recognition Failed. '{result.RecognitionStatus.ToString()}'";
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
        private async void SpeechRecogntionFromFile_ButtonClicked()
        {
            stopRecognitionTaskCompletionSource = new TaskCompletionSource<int>();
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
                // Creates an instance of a speech factory with specified and service region (e.g., "westus").
                var factory = SpeechFactory.FromSubscription(this.SubscriptionKey, this.Region);
                // Creates a speech recognizer using file as audio input. The default language is "en-us".
                using (var recognizer = factory.CreateSpeechRecognizerWithFileInput(file.Path, this.RecognitionLanguage))
                {
                    // Subscribes to events.
                    recognizer.IntermediateResultReceived += (s, e) =>
                    {
                        NotifyUser(e.Result.Text, NotifyType.StatusMessage);
                    };
                    recognizer.FinalResultReceived += (s, e) =>
                    {
                        string str;
                        if (e.Result.RecognitionStatus == RecognitionStatus.Recognized)
                        {
                            str = $"Final result: Status: {e.Result.RecognitionStatus.ToString()}, Text: {e.Result.Text}.";
                        }
                        else
                        {
                            str = $"Final result: Status: {e.Result.RecognitionStatus.ToString()}, FailureReason: {e.Result.RecognitionFailureReason}.";
                        }
                        NotifyUser(str, NotifyType.StatusMessage);
                    };
                    recognizer.RecognitionErrorRaised += (s, e) =>
                    {
                        NotifyUser($"An error occurred. Status: {e.Status.ToString()}, FailureReason: {e.FailureReason}", NotifyType.StatusMessage);
                    };
                    recognizer.OnSessionEvent += (s, e) =>
                    {
                        NotifyUser($"Session event. Event: {e.EventType.ToString()}.", NotifyType.StatusMessage);
                        // Stops translation when session stop is detected.
                        if (e.EventType == SessionEventType.SessionStoppedEvent)
                        {
                            NotifyUser($"Stop recognition.", NotifyType.StatusMessage);
                            stopRecognitionTaskCompletionSource.TrySetResult(0);
                        }
                    };
                    // Starts continuous recognition. Uses StopContinuousRecognitionAsync() to stop recognition.
                    await recognizer.StartContinuousRecognitionAsync().ConfigureAwait(false);
                    // Waits for completion.
                    await stopRecognitionTaskCompletionSource.Task.ConfigureAwait(false);
                    // Stops recognition.
                    await recognizer.StopContinuousRecognitionAsync().ConfigureAwait(false);
                }
            }
            else
            {
                NotifyUser($"Can't open {filename} !", NotifyType.ErrorMessage);
            }
        }
        private async void SpeechRecogntionFromStream_ButtonClicked(object sender, RoutedEventArgs e)
        {
            stopRecognitionTaskCompletionSource = new TaskCompletionSource<int>();
            AudioInputStream audioStream = null;
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
                string s = string.Format("Can't open {0} !", file.Path);
                NotifyUser(s, NotifyType.ErrorMessage);
                return;
            }
            try
            {
                stream = (await file.OpenReadAsync()).AsStreamForRead();
                reader = new BinaryReader(stream);

                // Create an audio stream from a wav file.
                audioStream = MicrosoftSpeechSDKSamples.Helper.OpenWaveFile(reader);

                // Creates an instance of a speech factory with specified and service region (e.g., "westus").
                var factory = SpeechFactory.FromSubscription(this.SubscriptionKey, this.Region);
                // Creates a speech recognizer using file as audio input. The default language is "en-us".
                using (var recognizer = factory.CreateSpeechRecognizerWithStream(audioStream, this.RecognitionLanguage))
                {
                    // Subscribes to events.
                    recognizer.IntermediateResultReceived += (s, ee) =>
                    {
                        NotifyUser(ee.Result.Text, NotifyType.StatusMessage);
                    };
                    recognizer.FinalResultReceived += (s, ee) =>
                    {
                        string str;
                        if (ee.Result.RecognitionStatus == RecognitionStatus.Recognized)
                        {
                            str = $"Final result: Status: {ee.Result.RecognitionStatus.ToString()}, Text: {ee.Result.Text}.";
                        }
                        else
                        {
                            str = $"Final result: Status: {ee.Result.RecognitionStatus.ToString()}, FailureReason: {ee.Result.RecognitionFailureReason}.";
                        }
                        NotifyUser(str, NotifyType.StatusMessage);
                    };
                    recognizer.RecognitionErrorRaised += (s, ee) =>
                    {
                        NotifyUser($"An error occurred. Status: {ee.Status.ToString()}, FailureReason: {ee.FailureReason}", NotifyType.StatusMessage);
                    };
                    recognizer.OnSessionEvent += (s, ee) =>
                    {
                        NotifyUser($"Session event. Event: {ee.EventType.ToString()}.", NotifyType.StatusMessage);
                        // Stops translation when session stop is detected.
                        if (ee.EventType == SessionEventType.SessionStoppedEvent)
                        {
                            NotifyUser($"Stop recognition.", NotifyType.StatusMessage);
                            stopRecognitionTaskCompletionSource.TrySetResult(0);
                        }
                    };
                    // Starts continuous recognition. Uses StopContinuousRecognitionAsync() to stop recognition.
                    await recognizer.StartContinuousRecognitionAsync().ConfigureAwait(false);
                    // Waits for completion.
                    await stopRecognitionTaskCompletionSource.Task.ConfigureAwait(false);
                    // Stops recognition.
                    await recognizer.StopContinuousRecognitionAsync().ConfigureAwait(false);
                }
            }
            catch(System.FormatException ex)
            {
                NotifyUser(ex.ToString(), NotifyType.ErrorMessage);
            }
            finally
            {
                if (reader != null)
                {
                    reader.Dispose();
                }
                if (audioStream != null)
                {
                    audioStream.Dispose();
                }
                if(stream != null)
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
        private void RegionCombox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            this.Region = ((ComboBoxItem)RegionComboBox.SelectedItem).Tag.ToString();
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
            if (String.IsNullOrEmpty(this.subscriptionKey) || String.IsNullOrEmpty(this.RecognitionLanguage))
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