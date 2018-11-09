//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace MicrosoftSpeechSDKSamples.WpfTranslationSample
{
    using System;
    using System.Globalization;
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
            this.InitializeVoiceMap();
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
            if (this.config != null)
            {
                this.recognizer.Dispose();
                this.config = null;
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
            this.WriteLine(log, "\n--- Start speech translation using " + recoSource + " ----\n\n");
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
            var formattedStr = string.Format(CultureInfo.InvariantCulture, format, args);
            Trace.WriteLine(formattedStr);
            Dispatcher.Invoke(() =>
            {
                log.AppendText((formattedStr + "\n"));
                log.ScrollToEnd();
            });
        }


        private void SaveKeyClick(object sender, RoutedEventArgs e)
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

        /// <summary>
        /// Sets corresponding voice parameter for set of languages used in this app
        /// Complete list of available voices can be found here: https://docs.microsoft.com/en-us/azure/cognitive-services/speech-service/supported-languages
        /// </summary>
        private void InitializeVoiceMap()
        {
            voiceMap = new Dictionary<string, string>();
            voiceMap.Add("en-US", "Microsoft Server Speech Text to Speech Voice (en-US, JessaRUS)");
            voiceMap.Add("ar-EG", "Microsoft Server Speech Text to Speech Voice (ar-EG, Hoda)");
            voiceMap.Add("zh-CN", "Microsoft Server Speech Text to Speech Voice (zh-CN, Yaoyao, Apollo)");
            voiceMap.Add("fr-FR", "Microsoft Server Speech Text to Speech Voice (fr-FR, Julie, Apollo)");
            voiceMap.Add("de-DE", "Microsoft Server Speech Text to Speech Voice (de-DE, Hedda)");
            voiceMap.Add("it-IT", "Microsoft Server Speech Text to Speech Voice (it-IT, Cosimo, Apollo)");
            voiceMap.Add("ja-JP", "Microsoft Server Speech Text to Speech Voice (ja-JP, Ayumi, Apollo)");
            voiceMap.Add("pt-BR", "Microsoft Server Speech Text to Speech Voice (pt-BR, Daniel, Apollo)");
            voiceMap.Add("ru-RU", "Microsoft Server Speech Text to Speech Voice (ru-RU, Irina, Apollo)");
            voiceMap.Add("es-ES", "Microsoft Server Speech Text to Speech Voice (es-ES, Laura, Apollo)");
        }

        #endregion

        private TranslationRecognizer recognizer;
        private SpeechTranslationConfig config;
        private string subscriptionKey;
        private const string SubscriptionKeyFileName = "SubscriptionKey.txt";
        private bool started;
        private string voice;
        private Dictionary<string, string> voiceMap;

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
                this.OnPropertyChanged<string>();
            }
        }

        /// <summary>
        /// Gets or sets Region
        /// </summary>
        public string Region { get; set; }

        /// <summary>
        /// Gets or sets From Language
        /// </summary>
        public string FromLanguage { get; set; }

        /// <summary>
        /// Gets or sets To Languages
        /// </summary>
        public List<string> ToLanguages { get; set; }

        /// <summary>
        /// Gets or sets Voice Parameter's Language
        /// </summary>
        public string VoiceLanguage { get; set; }

        /// <summary>
        /// Handles the Click event of the _startButton control.
        /// Disables UI of Language/Region settings
        /// Sets Region, FromLanguage, ToLanguages and VoiceLanguage as specified by user input
        /// Adds selected VoiceLanguage first to ToLanguages
        /// Checks if Subscription Key is valid
        /// Calls CreateRecognizer()
        /// Waits on a thread which is running the recognition
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="RoutedEventArgs"/> instance containing the event data.</param>
        private async void StartButtonClick(object sender, RoutedEventArgs e)
        {

            this.settingsPanel.IsEnabled = false;
            if (string.IsNullOrEmpty(this.SubscriptionKey))
            {
                this.SubscriptionKey = this.GetSubscriptionKeyFromFile(SubscriptionKeyFileName);
            }
            this.Region = ((ComboBoxItem)regionComboBox.SelectedItem).Tag.ToString();
            this.FromLanguage = ((ComboBoxItem)fromLanguageComboBox.SelectedItem).Tag.ToString();
            this.ToLanguages = new List<string>();
            this.VoiceLanguage = ((ComboBoxItem)voiceComboBox.SelectedItem).Tag.ToString();

            this.voice = voiceMap[this.VoiceLanguage];
            this.ToLanguages.Add(this.VoiceLanguage);
            foreach (ListBoxItem selectedLanguage in toLanguagesListBox.SelectedItems)
            {
                string languageCode = selectedLanguage.Tag.ToString();
                if (!this.ToLanguages.Contains(languageCode))
                {
                    this.ToLanguages.Add(languageCode);
                }
            }

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
        private async void StopButtonClick(object sender, RoutedEventArgs e)
        {
            if (this.started)
            {
                this.settingsPanel.IsEnabled = true;
                await Task.Run(async () => { await this.recognizer.StopContinuousRecognitionAsync().ConfigureAwait(false); });
                this.started = false;
            }
        }

        /// <summary>
        /// Initializes the config object with subscription key and region
        /// Initializes the recognizer object with a TranslationRecognizer
        /// Subscribes the recognizer to recognition Event Handlers
        /// If recognition is running, starts a thread which stops the recognition
        /// </summary>
        private void CreateRecognizer()
        {
            this.config = SpeechTranslationConfig.FromSubscription(SubscriptionKey, Region);
            this.config.SpeechRecognitionLanguage = FromLanguage;
            this.config.VoiceName = voice;
            ToLanguages.ForEach(l => this.config.AddTargetLanguage(l));

            this.recognizer = new TranslationRecognizer(this.config);

            this.recognizer.Recognizing += this.OnRecognizingEventHandler;
            this.recognizer.Recognized += this.OnRecognizedEventHandler;
            this.recognizer.Synthesizing += this.OnSynthesizingEventHandler;
            this.recognizer.Canceled += this.OnCanceledEventHandler;
        }

        #region Recognition Event Handlers

        /// <summary>
        /// Called when a partial response is received.
        /// </summary>
        /// <param name="sender">The sender.</param>
        /// <param name="e">The <see cref="TranslationRecognitionEventArgs"/> instance containing the event data.</param>
        private void OnRecognizingEventHandler(object sender, TranslationRecognitionEventArgs e)
        {
            string text = e.Result.Text;
            foreach (var t in e.Result.Translations)
            {
                text += $"\nSame in {t.Key}: {t.Value}";
            }

            this.SetCurrentText(this.crisCurrentText, text);
        }

        /// <summary>
        /// Called on final response.
        /// </summary>
        /// <param name="sender">The sender.</param>
        /// <param name="e">The <see cref="TranslationRecognitionEventArgs"/> instance containing the event data.</param>
        private void OnRecognizedEventHandler(object sender, TranslationRecognitionEventArgs e)
        {
            if (e.Result.Text.Length == 0)
            {
                this.WriteLine(this.crisLogText, "Reason: " + e.Result.Reason);
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
        /// Called when translation is canceled.
        /// </summary>
        /// <param name="sender">The sender.</param>
        /// <param name="e">The <see cref="TranslationRecognitionCanceledEventArgs"/> instance containing the event data.</param>
        private void OnCanceledEventHandler(object sender, TranslationRecognitionCanceledEventArgs e)
        {
            string text = $"Speech recognition: canceled. Reason: {e.Reason}, ErrorDetails: {e.ErrorDetails}";
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
        private void OnSynthesizingEventHandler(object sender, TranslationSynthesisEventArgs e)
        {
            var audio = e.Result.GetAudio();
            if (audio.Length > 0)
            {
                using (var m = new MemoryStream(audio))
                {
                    SoundPlayer simpleSound = new SoundPlayer(m);
                    simpleSound.Play();
                }
            }
        }

        #endregion
    }
}
