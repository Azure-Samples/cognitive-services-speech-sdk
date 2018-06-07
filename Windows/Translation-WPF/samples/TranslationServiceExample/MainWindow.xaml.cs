//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.CognitiveServices.SpeechRecognition
{
    using System;
    using System.Collections.Generic;
    using System.ComponentModel;
    using System.Diagnostics;
    using System.IO;
    using System.IO.IsolatedStorage;
    using System.Media;
    using System.Runtime.CompilerServices;
    using System.Threading.Tasks;
    using System.Windows;
    using System.Windows.Controls;

    using Microsoft.CognitiveServices.Speech;
    using Microsoft.CognitiveServices.Speech.Translation;

    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window, INotifyPropertyChanged
    {
        #region Events

        /// <summary>
        /// Implement INotifyPropertyChanged interface
        /// </summary>
        public event PropertyChangedEventHandler PropertyChanged;

        /// <summary>
        /// Helper function for INotifyPropertyChanged interface 
        /// </summary>
        /// <typeparam name="T">Property type</typeparam>
        /// <param name="caller">Property name</param>
        private void OnPropertyChanged<T>([CallerMemberName]string caller = null)
        {
            var handler = this.PropertyChanged;
            if (handler != null)
            {
                handler(this, new PropertyChangedEventArgs(caller));
            }
        }

        #endregion Events

        #region Helper functions

        /// <summary>
        /// Initializes a new instance of the <see cref="MainWindow"/> class.
        /// </summary>
        public MainWindow()
        {
            this.InitializeComponent();
        }

        private void SetCurrentText(TextBlock textBlock, string text)
        {
            this.Dispatcher.Invoke(
                () =>
                {
                    textBlock.Text = text;
                });
        }

        /// <summary>
        /// Raises the System.Windows.Window.Closed event.
        /// </summary>
        /// <param name="e">An System.EventArgs that contains the event data.</param>
        protected override void OnClosed(EventArgs e)
        {
            if (this.factory != null)
            {
                this.recognizer.Dispose();
                this.factory = null;
            }

            base.OnClosed(e);
        }

        /// <summary>
        /// Logs the recognition start.
        /// </summary>
        private void LogRecognitionStart(TextBox log, TextBlock currentText)
        {
            string recoSource = "microphone";
            this.SetCurrentText(currentText, string.Empty);
            log.Clear();
            this.WriteLine(log, "\n--- Start speech recognition using " + recoSource + " ----\n\n");
        }

        /// <summary>
        /// Writes the line.
        /// </summary>
        private void WriteLine(TextBox log)
        {
            this.WriteLine(log, string.Empty);
        }

        /// <summary>
        /// Writes the line.
        /// </summary>
        /// <param name="format">The format.</param>
        /// <param name="args">The arguments.</param>
        private void WriteLine(TextBox log, string format, params object[] args)
        {
            var formattedStr = string.Format(format, args);
            Trace.WriteLine(formattedStr);
            Dispatcher.Invoke(() =>
            {
                log.AppendText((formattedStr + "\n"));
                log.ScrollToEnd();
            });
        }


        private void SaveKey_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                SaveKeyToFile(SubscriptionKeyFileName, this.SubscriptionKey);
                MessageBox.Show("Keys are saved to your disk.\nYou do not need to paste it next time.", "Keys");
            }
            catch (Exception exception)
            {
                MessageBox.Show(
                    "Fail to save Keys. Error message: " + exception.Message,
                    "Keys",
                    MessageBoxButton.OK,
                    MessageBoxImage.Error);
            }
        }

        private static void SaveKeyToFile(string fileName, string key)
        {
            using (System.IO.IsolatedStorage.IsolatedStorageFile isoStore = IsolatedStorageFile.GetStore(IsolatedStorageScope.User | IsolatedStorageScope.Assembly, null, null))
            {
                using (var oStream = new IsolatedStorageFileStream(fileName, FileMode.Create, isoStore))
                {
                    using (var writer = new StreamWriter(oStream))
                    {
                        writer.WriteLine(key);
                    }
                }
            }
        }

        private string GetSubscriptionKeyFromFile(string fileName)
        {
            string subscriptionKey = null;

            using (IsolatedStorageFile isoStore = IsolatedStorageFile.GetStore(IsolatedStorageScope.User | IsolatedStorageScope.Assembly, null, null))
            {
                try
                {
                    using (var iStream = new IsolatedStorageFileStream(fileName, FileMode.Open, isoStore))
                    {
                        using (var reader = new StreamReader(iStream))
                        {
                            subscriptionKey = reader.ReadLine();
                        }
                    }
                }
                catch (FileNotFoundException)
                {
                    subscriptionKey = null;
                }
            }
            return subscriptionKey;
        }

        #endregion

        private TranslationRecognizer recognizer;
        private SpeechFactory factory;
        private string subscriptionKey;
        private const string SubscriptionKeyFileName = "SubscriptionKey.txt";
        private bool started;
        public string SubscriptionKey
        {
            get
            {
                return this.subscriptionKey;
            }

            set
            {
                this.subscriptionKey = value?.Trim();
                this.OnPropertyChanged<string>();
            }
        }

        /// <summary>
        /// Handles the Click event of the _startButton control.
        /// Checks if Subscription Key is valid
        /// Calls CreateRecognizer()
        /// Waits on a thread which is running the recognition
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="RoutedEventArgs"/> instance containing the event data.</param>
        private async void StartButton_Click(object sender, RoutedEventArgs e)
        {
            this.SubscriptionKey = this.GetSubscriptionKeyFromFile(SubscriptionKeyFileName);

            if (this.subscriptionKey == null || this.subscriptionKey.Length <= 0)
            {
                MessageBox.Show("Subscription Key is wrong or missing!");
                this.WriteLine(this.crisLogText, "--- Error : Subscription Key is wrong or missing! ---");
                return;
            }

            if (!this.started)
            {
                this.started = true;
                this.LogRecognitionStart(this.crisLogText, this.crisCurrentText);
                this.CreateRecognizer();
                await Task.Run(async () => { await this.recognizer.StartContinuousRecognitionAsync().ConfigureAwait(false); });
            }
        }

        /// <summary>
        /// Handles the Click event of the _stopButton control.
        /// If recognition is running, starts a thread which stops the recognition
        /// </summary>
        private async void StopButton_Click(object sender, RoutedEventArgs e)
        {
            if (this.started)
            {
                await Task.Run(async () => { await this.recognizer.StopContinuousRecognitionAsync().ConfigureAwait(false); });
                this.started = false;
            }
        }

        /// <summary>
        /// Initializes the factory object with subscription key and region
        /// Initializes the recognizer object with a TranslationRecognizer
        /// Subscribes the recognizer to recognition Event Handlers
        /// If recognition is running, starts a thread which stops the recognition
        /// </summary>
        private void CreateRecognizer()
        {
            string region = "westus";
            string fromLanguage = "en-US";
            var toLanguages = new List<string>() { "zh", "de" };
            var voiceChinese = "zh-CN-Yaoyao";

            this.factory = SpeechFactory.FromSubscription(SubscriptionKey, region);

            this.recognizer = this.factory.CreateTranslationRecognizer(fromLanguage, toLanguages, voiceChinese);

            this.recognizer.IntermediateResultReceived += this.OnPartialResponseReceivedHandler;
            this.recognizer.FinalResultReceived += this.OnFinalResponse;
            this.recognizer.SynthesisResultReceived += this.OnSynthesis;
            this.recognizer.RecognitionErrorRaised += this.OnError;
        }

        #region Recognition Event Handlers

        /// <summary>
        /// Called when a partial response is received.
        /// </summary>
        /// <param name="sender">The sender.</param>
        /// <param name="e">The <see cref="TranslationTextResultEventArgs"/> instance containing the event data.</param>
        private void OnPartialResponseReceivedHandler(object sender, TranslationTextResultEventArgs e)
        {
            string text = e.Result.Text;
            foreach(var t in e.Result.Translations)
            {
                text += $"\nSame in {t.Key}: {t.Value}";
            }

            this.SetCurrentText(this.crisCurrentText, text);
        }

        /// <summary>
        /// Called on final response.
        /// </summary>
        /// <param name="sender">The sender.</param>
        /// <param name="e">The <see cref="TranslationTextResultEventArgs"/> instance containing the event data.</param>
        private void OnFinalResponse(object sender, TranslationTextResultEventArgs e)
        {
            if (e.Result.Text.Length == 0)
            {
                this.WriteLine(this.crisLogText, "Status: " + e.Result.RecognitionStatus);
                this.WriteLine(this.crisLogText, "No phrase response is available.");
            }
            else
            {
                string text = e.Result.Text;
                foreach (var t in e.Result.Translations)
                {
                    text += $"\nSame in {t.Key}: {t.Value}";
                }

                this.SetCurrentText(this.crisCurrentText, text);
                text += "\n";
                this.WriteLine(this.crisLogText, text);
            }
        }

        /// <summary>
        /// Called when error occurs.
        /// </summary>
        /// <param name="sender">The sender.</param>
        /// <param name="e">The <see cref="RecognitionErrorEventArgs"/> instance containing the event data.</param>
        private void OnError(object sender, RecognitionErrorEventArgs e)
        {
            string text = $"Speech recognition: error occurred. Status: {e.Status}, FailureReason: {e.FailureReason}";
            this.SetCurrentText(this.crisCurrentText, text);
            text += "\n";
            this.WriteLine(this.crisLogText, text);
            if (this.started)
            {
                this.recognizer.StopContinuousRecognitionAsync().Wait();
                this.started = false;
            }
        }

        /// <summary>
        /// Called on availability of synthesized data.
        /// </summary>
        /// <param name="sender">The sender.</param>
        /// <param name="e">The <see cref="TranslationSynthesisEventArgs"/> instance containing the event data.</param>
        private void OnSynthesis(object sender, TranslationSynthesisResultEventArgs e)
        {
            if (e.Result.Status == SynthesisStatus.Success)
            {
                using (var m = new MemoryStream(e.Result.Audio))
                {
                    SoundPlayer simpleSound = new SoundPlayer(m);
                    simpleSound.Play();
                }
            }
        }

#endregion 
    }
}
