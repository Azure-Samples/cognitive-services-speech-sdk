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
using Windows.Storage;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;
using Microsoft.CognitiveServices.Speech;
using System.Threading.Tasks;
using Windows.Storage.Pickers;
using Microsoft.CognitiveServices.Speech.Audio;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

namespace helloworld
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainPage : Page
    {
        private KeywordRecognitionModel model = null;
        private KeywordRecognitionResult result = null;
        private KeywordRecognizer recognizer = null;

        private enum NotifyType
        {
            StatusMessage,
            ErrorMessage
        };

        public MainPage()
        {
            this.InitializeComponent();
            this.EnableMicrophoneButton.IsEnabled = true;
            this.RecognizeKeywordButton.IsEnabled = false;
            this.SaveToFileButton.IsEnabled = false;
        }

        private async Task InitializeKeywordModel()
        {
            const string keywordFilePath = "ms-appx:///Keyword/kws.table";
            var file = await StorageFile.GetFileFromApplicationUriAsync(new Uri(keywordFilePath));
            this.model = KeywordRecognitionModel.FromFile(file.Path);
        }

        private async void EnableMicrophoneButton_Click(object sender, RoutedEventArgs e)
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
                this.EnableMicrophoneButton.IsEnabled = false;
                this.RecognizeKeywordButton.IsEnabled = true;
            }
        }

        private async void RecognizeKeywordButton_Click(object sender, RoutedEventArgs e)
        {
            RecognizeKeywordButton.IsEnabled = false;

            if (recognizer == null)
            {
                recognizer = new KeywordRecognizer(AudioConfig.FromDefaultMicrophoneInput());
            }
            if (model == null)
            {
                await InitializeKeywordModel();
            }

            NotifyUser("Say \"Computer\"", NotifyType.StatusMessage);
            result = await recognizer.RecognizeOnceAsync(model);
            NotifyUser("Got a keyword, now you can keep talking...", NotifyType.StatusMessage);
            SaveToFileButton.IsEnabled = true;

        }

        private async void SaveToFileButton_Click(object sender, RoutedEventArgs e)
        {
            var stream = AudioDataStream.FromResult(result);
            stream.DetachInput();
            RecognizeKeywordButton.IsEnabled = true;
            SaveToFileButton.IsEnabled = false;
            var savePicker = new FileSavePicker();
            savePicker.SuggestedStartLocation = PickerLocationId.DocumentsLibrary;
            savePicker.FileTypeChoices.Add("WAV File", new List<string>() { ".wav" });
            savePicker.SuggestedFileName = "audio";
            var file = await savePicker.PickSaveFileAsync();
            if (file != null)
            {
                var tempFilePath = $"{ApplicationData.Current.TemporaryFolder.Path}\\audio.wav";
                await stream.SaveToWaveFileAsync(tempFilePath);
                var tempFile = await StorageFile.GetFileFromPathAsync(tempFilePath);
                await tempFile.MoveAndReplaceAsync(file);
            }
        }

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
