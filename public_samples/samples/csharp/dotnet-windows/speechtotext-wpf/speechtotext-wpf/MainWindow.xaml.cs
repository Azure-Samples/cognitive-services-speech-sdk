//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace MicrosoftSpeechSDKSamples.WpfSpeechRecognitionSample
{
    using System;
    using System.Globalization;
    using System.ComponentModel;
    using System.Diagnostics;
    using System.IO;
    using System.Media;
    using System.Runtime.CompilerServices;
    using System.Threading.Tasks;
    using System.Windows;
    using System.Windows.Controls;
    using Forms = System.Windows.Forms;
    using System.IO.IsolatedStorage;

    using Microsoft.CognitiveServices.Speech;
    using Microsoft.CognitiveServices.Speech.Audio;

    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window, INotifyPropertyChanged
    {
        #region Properties

        /// <summary>
        /// True, if audio source is mic
        /// </summary>
        public bool UseMicrophone { get; set; }

        /// <summary>
        /// True, if audio source is audio file
        /// </summary>
        public bool UseFileInput { get; set; }

        /// <summary>
        /// Only baseline model used for recognition
        /// </summary>
        public bool UseBaseModel { get; set; }

        /// <summary>
        /// Only custom model used for recognition
        /// </summary>
        public bool UseCustomModel { get; set; }

        /// <summary>
        /// Both models used for recognition
        /// </summary>
        public bool UseBaseAndCustomModels { get; set; }

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
        /// Gets or sets region name of the service
        /// </summary>
        public string Region { get; set; }

        /// <summary>
        /// Gets or sets recognition language
        /// </summary>
        public string RecognitionLanguage { get; set; }

        /// <summary>
        /// Gets or sets deployment ID of the custom model
        /// </summary>
        public string CustomModelDeploymentId
        {
            get
            {
                return this.deploymentId;
            }

            set
            {
                this.deploymentId = value?.Trim();
                this.OnPropertyChanged<string>();
            }
        }

        // Private properties
        private const string defaultLocale = "en-US";
        private string deploymentId;
        private string subscriptionKey;
        private const string deploymentIdFileName = "CustomModelDeploymentId.txt";
        private const string subscriptionKeyFileName = "SubscriptionKey.txt";
        private string wavFileName;

        // The TaskCompletionSource must be rooted.
        // See https://blogs.msdn.microsoft.com/pfxteam/2011/10/02/keeping-async-methods-alive/ for details.
        private TaskCompletionSource<int> stopBaseRecognitionTaskCompletionSource;
        private TaskCompletionSource<int> stopCustomRecognitionTaskCompletionSource;

        #endregion

        /// <summary>
        /// For this app there are two recognizers, one with the baseline model (Base), one with CRIS model (Custom)
        /// </summary>
        enum RecoType
        {
            Base = 1,
            Custom = 2
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="MainWindow"/> class.
        /// </summary>
        public MainWindow()
        {
            this.InitializeComponent();
            this.Initialize();
        }

        /// <summary>
        /// Initializes a fresh audio session.
        /// </summary>
        private void Initialize()
        {
            this.UseMicrophone = false;
            this.UseFileInput = true;

            this.UseBaseModel = true;
            this.UseCustomModel = false;
            this.UseBaseAndCustomModels = false;

            // Set the default values for UI
            this.fileInputRadioButton.IsChecked = true;
            this.basicRadioButton.IsChecked = true;
            this.stopButton.IsEnabled = false;

            this.SubscriptionKey = this.GetValueFromIsolatedStorage(subscriptionKeyFileName);
            this.CustomModelDeploymentId = this.GetValueFromIsolatedStorage(deploymentIdFileName);
        }

        /// <summary>
        /// Handles the Click event of the StartButton:
        /// Disables Settings Panel in UI to prevent Click Events during Recognition
        /// Checks if keys are valid
        /// Plays audio if input source is a valid audio file
        /// Triggers Creation of specified Recognizers
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="RoutedEventArgs"/> instance containing the event data.</param>
        private void StartButton_Click(object sender, RoutedEventArgs e)
        {
            this.startButton.IsEnabled = false;
            this.stopButton.IsEnabled = true;
            this.radioGroup.IsEnabled = false;
            this.optionPanel.IsEnabled = false;
            this.LogRecognitionStart(this.customModelLogText, this.customModelCurrentText);
            this.LogRecognitionStart(this.baseModelLogText, this.baseModelCurrentText);
            wavFileName = "";

            this.Region = ((ComboBoxItem)regionComboBox.SelectedItem).Tag.ToString();
            this.RecognitionLanguage = ((ComboBoxItem)languageComboBox.SelectedItem).Tag.ToString();

            if (!AreKeysValid())
            {
                if (this.UseBaseModel)
                {
                    MessageBox.Show("Subscription Key is wrong or missing!");
                    this.WriteLine(this.baseModelLogText, "--- Error : Subscription Key is wrong or missing! ---");
                }
                else if (this.UseCustomModel)
                {
                    MessageBox.Show("Subscription Key or Custom Model DeploymentId is missing or wrong! If you do not need the custom model, please change Settings->Recognition Type.");
                    this.WriteLine(this.customModelLogText, "--- Error : Subscription Key or Custom Model DeploymentId is wrong or missing! ---");
                }
                else if (this.UseBaseAndCustomModels)
                {
                    MessageBox.Show("Subscription Key or Custom Model DeploymentId is missing or wrong! If you do not need the custom model, please change Settings->Recognition Type.");
                    this.WriteLine(this.baseModelLogText, "--- Error : Subscription Key or Custom Model DeploymentId is wrong or missing! ---");
                    this.WriteLine(this.customModelLogText, "--- Error : Subscription Key or Custom Model DeploymentId is wrong or missing! ---");
                }

                this.EnableButtons();
                return;
            }

            if (!this.UseMicrophone)
            {
                wavFileName = GetFile();
                if (wavFileName.Length <= 0) return;
                Task.Run(() => this.PlayAudioFile());
            }

            if (this.UseCustomModel || this.UseBaseAndCustomModels)
            {
                stopCustomRecognitionTaskCompletionSource = new TaskCompletionSource<int>();
                Task.Run(async () => { await CreateCustomReco().ConfigureAwait(false); });
            }

            if (this.UseBaseModel || this.UseBaseAndCustomModels)
            {
                stopBaseRecognitionTaskCompletionSource = new TaskCompletionSource<int>();
                Task.Run(async () => { await CreateBaseReco().ConfigureAwait(false); });
            }
        }


        /// <summary>
        /// Handles the Click event of the StopButton:
        /// Stops Recognition and enables Settings Panel in UI
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="RoutedEventArgs"/> instance containing the event data.</param>
        private void StopButton_Click(object sender, RoutedEventArgs e)
        {
            this.stopButton.IsEnabled = false;
            if (this.UseBaseModel || this.UseBaseAndCustomModels)
            {
                stopBaseRecognitionTaskCompletionSource.TrySetResult(0);
            }
            if (this.UseCustomModel || this.UseBaseAndCustomModels)
            {
                stopCustomRecognitionTaskCompletionSource.TrySetResult(0);
            }

            EnableButtons();
        }

        /// <summary>
        /// Creates Recognizer with baseline model and selected language:
        /// Creates a factory with subscription key and selected region
        /// If input source is audio file, creates recognizer with audio file otherwise with default mic
        /// Waits on RunRecognition
        /// </summary>
        private async Task CreateBaseReco()
        {
            // Todo: suport users to specifiy a different region.
            var basicFactory = SpeechFactory.FromSubscription(this.SubscriptionKey, this.Region);
            SpeechRecognizer basicRecognizer;

            if (this.UseMicrophone)
            {
                using (basicRecognizer = basicFactory.CreateSpeechRecognizer(this.RecognitionLanguage))
                {
                    await this.RunRecognizer(basicRecognizer, RecoType.Base, stopBaseRecognitionTaskCompletionSource).ConfigureAwait(false);
                }
            }
            else
            {
                using (var audioInput = AudioConfig.FromWavFileInput(wavFileName))
                {
                    using (basicRecognizer = basicFactory.CreateSpeechRecognizerFromConfig(audioInput, this.RecognitionLanguage))
                    {
                        await this.RunRecognizer(basicRecognizer, RecoType.Base, stopBaseRecognitionTaskCompletionSource).ConfigureAwait(false);
                    }
                }
            }
        }

        /// <summary>
        /// Creates Recognizer with custom model deploymentId and selected language:
        /// Creates a factory with subscription key and selected region
        /// If input source is audio file, creates recognizer with audio file otherwise with default mic
        /// Waits on RunRecognition
        /// </summary>
        private async Task CreateCustomReco()
        {
            // Todo: suport users to specifiy a different region.
            var customFactory = SpeechFactory.FromSubscription(this.SubscriptionKey, this.Region);
            SpeechRecognizer customRecognizer;

            if (this.UseMicrophone)
            {
                using (customRecognizer = customFactory.CreateSpeechRecognizer(this.RecognitionLanguage))
                {
                    customRecognizer.DeploymentId = this.CustomModelDeploymentId;
                    await this.RunRecognizer(customRecognizer, RecoType.Custom, stopCustomRecognitionTaskCompletionSource).ConfigureAwait(false);
                }
            }
            else
            {
                using (var audioInput = AudioConfig.FromWavFileInput(wavFileName))
                {
                    using (customRecognizer = customFactory.CreateSpeechRecognizerFromConfig(audioInput, this.RecognitionLanguage))
                    {
                        customRecognizer.DeploymentId = this.CustomModelDeploymentId;
                        await this.RunRecognizer(customRecognizer, RecoType.Custom, stopCustomRecognitionTaskCompletionSource).ConfigureAwait(false);
                    }
                }
            }
        }

        /// <summary>
        /// Subscribes to Recognition Events
        /// Starts the Recognition and waits until Final Result is received, then Stops recognition
        /// </summary>
        /// <param name="recognizer">Recognizer object</param>
        /// <param name="recoType">Type of Recognizer</param>
        ///  <value>
        ///   <c>Base</c> if Baseline model; otherwise, <c>Custom</c>.
        /// </value>
        private async Task RunRecognizer(SpeechRecognizer recognizer, RecoType recoType, TaskCompletionSource<int> source)
        {
            //subscribe to events
            bool isChecked = false;
            this.Dispatcher.Invoke(() =>
            {
                isChecked = this.immediateResultsCheckBox.IsChecked == true;
            });

            if (isChecked)
            {
                recognizer.IntermediateResultReceived += (sender, e) => IntermediateResultEventHandler(e, recoType);
            }
            recognizer.FinalResultReceived += (sender, e) => FinalResultEventHandler(e, recoType);
            recognizer.RecognitionErrorRaised += (sender, e) => ErrorEventHandler(e, recoType, source);
            recognizer.OnSessionEvent += (sender, e) => SessionEventHandler(e, recoType, source);
            recognizer.OnSpeechDetectedEvent += (sender, e) => SpeechDetectedEventHandler(e, recoType);

            //start,wait,stop recognition
            await recognizer.StartContinuousRecognitionAsync().ConfigureAwait(false);
            await source.Task.ConfigureAwait(false);
            await recognizer.StopContinuousRecognitionAsync().ConfigureAwait(false);

            this.EnableButtons();

            // unsubscribe from events
            if (isChecked)
            {
                recognizer.IntermediateResultReceived -= (sender, e) => IntermediateResultEventHandler(e, recoType);
            }
            recognizer.FinalResultReceived -= (sender, e) => FinalResultEventHandler(e, recoType);
            recognizer.RecognitionErrorRaised -= (sender, e) => ErrorEventHandler(e, recoType, source);
            recognizer.OnSessionEvent -= (sender, e) => SessionEventHandler(e, recoType, source);
            recognizer.OnSpeechDetectedEvent -= (sender, e) => SpeechDetectedEventHandler(e, recoType);
        }

        #region Recognition Event Handlers

        /// <summary>
        /// Logs Intermediate Recognition results
        /// </summary>
        private void IntermediateResultEventHandler(SpeechRecognitionResultEventArgs e, RecoType rt)
        {
            var log = (rt == RecoType.Base) ? this.baseModelLogText : this.customModelLogText;
            this.WriteLine(log, "Intermediate result: {0} ", e.Result.Text);
        }

        /// <summary>
        /// Logs the Final result
        /// </summary>
        private void FinalResultEventHandler(SpeechRecognitionResultEventArgs e, RecoType rt)
        {
            TextBox log;
            if (rt == RecoType.Base)
            {
                log = this.baseModelLogText;
                this.SetCurrentText(this.baseModelCurrentText, e.Result.Text);
            }
            else
            {
                log = this.customModelLogText;
                this.SetCurrentText(this.customModelCurrentText, e.Result.Text);
            }

            this.WriteLine(log);
            this.WriteLine(log, $" --- Final result received. Status: {e.Result.RecognitionStatus.ToString()}. --- ");
            if (!string.IsNullOrEmpty(e.Result.Text))
            {
                this.WriteLine(log, e.Result.Text);
            }
        }

        /// <summary>
        /// Logs Error events
        /// And sets the TaskCompletionSource to 0, in order to trigger Recognition Stop
        /// </summary>
        private void ErrorEventHandler(RecognitionErrorEventArgs e, RecoType rt, TaskCompletionSource<int> source)
        {
            var log = (rt == RecoType.Base) ? this.baseModelLogText : this.customModelLogText;
            source.TrySetResult(0);
            this.WriteLine(log, "--- Error received ---");
            this.WriteLine(log, $"Status: {e.Status.ToString()}. Reason: {e.FailureReason}.");
            this.WriteLine(log);
        }

        /// <summary>
        /// If SessionStoppedEvent is received, sets the TaskCompletionSource to 0, in order to trigger Recognition Stop
        /// </summary>
        private void SessionEventHandler(SessionEventArgs e, RecoType rt, TaskCompletionSource<int> source)
        {
            var log = (rt == RecoType.Base) ? this.baseModelLogText : this.customModelLogText;
            this.WriteLine(log, String.Format(CultureInfo.InvariantCulture, "Speech recognition: Session event: {0}.", e.ToString()));
            if (e.EventType == SessionEventType.SessionStoppedEvent)
            {
                source.TrySetResult(0);
            }
        }

        private void SpeechDetectedEventHandler(RecognitionEventArgs e, RecoType rt)
        {
            var log = (rt == RecoType.Base) ? this.baseModelLogText : this.customModelLogText;
            this.WriteLine(log, String.Format(CultureInfo.InvariantCulture, "Speech recognition: Speech event: {0}.", e.ToString()));
        }

        #endregion

        #region Helper Functions
        /// <summary>
        /// Retrieves Key from File
        /// </summary>
        /// <param name="fileName">Name of file which contains key</param>
        private string GetValueFromIsolatedStorage(string fileName)
        {
            string value = null;
            using (IsolatedStorageFile isoStore = IsolatedStorageFile.GetStore(IsolatedStorageScope.User | IsolatedStorageScope.Assembly, null, null))
            {
                try
                {
                    using (var iStream = new IsolatedStorageFileStream(fileName, FileMode.Open, isoStore))
                    {
                        using (var reader = new StreamReader(iStream))
                        {
                            value = reader.ReadLine();
                        }
                    }
                }
                catch (FileNotFoundException)
                {
                    value = null;
                }
            }

            return value;
        }

        /// <summary>
        /// Writes Key to File
        /// </summary>
        /// <param name="fileName">Name of file where key should be stored</param>
        /// <param name="key">The key which should be stored</param>
        private static void SaveKeyToIsolatedStorage(string fileName, string key)
        {
            if (fileName != null && key != null)
            {
                using (IsolatedStorageFile isoStore = IsolatedStorageFile.GetStore(IsolatedStorageScope.User | IsolatedStorageScope.Assembly, null, null))
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
        }

        /// <summary>
        /// Save Button Click triggers the three entered keys to be saved to their corresponding files.
        /// </summary>
        private void SaveKey_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                SaveKeyToIsolatedStorage(subscriptionKeyFileName, this.SubscriptionKey);
                SaveKeyToIsolatedStorage(deploymentIdFileName, this.CustomModelDeploymentId);
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

        /// <summary>
        /// Checks if keys are valid
        /// </summary>
        private bool AreKeysValid()
        {
            if (this.subscriptionKey == null || this.subscriptionKey.Length <= 0 ||
                ((this.UseCustomModel || this.UseBaseAndCustomModels) && (this.deploymentId == null || this.deploymentId.Length <= 0)))
            {
                return false;
            }

            return true;
        }

        /// <summary>
        /// Checks if specified audio file exists and returns it
        /// </summary>
        public string GetFile()
        {
            string filePath = "";
            this.Dispatcher.Invoke(() =>
            {
                filePath = this.fileNameTextBox.Text;
            });
            if (!File.Exists(filePath))
            {
                MessageBox.Show("File does not exist!");
                this.WriteLine(this.baseModelLogText, "--- Error : File does not exist! ---");
                this.WriteLine(this.customModelLogText, "--- Error : File does not exist! ---");
                this.EnableButtons();
                return "";
            }
            return filePath;
        }

        /// <summary>
        /// Plays the audio file
        /// </summary>
        private void PlayAudioFile()
        {
            SoundPlayer player = new SoundPlayer(wavFileName);
            player.Load();
            player.Play();
        }

        /// <summary>
        /// Logs the recognition start.
        /// </summary>
        private void LogRecognitionStart(TextBox log, TextBlock currentText)
        {
            string recoSource;
            recoSource = this.UseMicrophone ? "microphone" : "wav file";

            this.SetCurrentText(currentText, string.Empty);
            log.Clear();
            this.WriteLine(log, "\n--- Start speech recognition using " + recoSource + " in " + defaultLocale + " language ----\n\n");
        }

        /// <summary>
        /// Writes the line.
        /// </summary>
        private void WriteLine(TextBox log)
        {
            this.WriteLine(log, string.Empty);
        }

        private void WriteLine(TextBox log, string format, params object[] args)
        {
            var formattedStr = string.Format(CultureInfo.InvariantCulture, format, args);
            Trace.WriteLine(formattedStr);
            this.Dispatcher.Invoke(() =>
            {
                log.AppendText((formattedStr + "\n"));
                log.ScrollToEnd();
            });
        }

        private void SetCurrentText(TextBlock textBlock, string text)
        {
            this.Dispatcher.Invoke(() =>
            {
                textBlock.Text = text;
            });
        }

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

        private void RadioButton_Click(object sender, RoutedEventArgs e)
        {
            this.EnableButtons();
        }

        private void SelectFileButton_Click(object sender, RoutedEventArgs e)
        {
            Forms.FileDialog fileDialog = new Forms.OpenFileDialog();
            fileDialog.ShowDialog();
            this.fileNameTextBox.Text = fileDialog.FileName;
        }

        private void HelpButton_Click(object sender, RoutedEventArgs e)
        {
            Process.Start("https://azure.microsoft.com/en-us/services/cognitive-services/");
        }

        private void EnableButtons()
        {
            this.Dispatcher.Invoke(() =>
            {
                this.startButton.IsEnabled = true;
                this.radioGroup.IsEnabled = true;
                this.optionPanel.IsEnabled = true;
            });
        }
        #endregion

        #region Events
        /// <summary>
        /// Implement INotifyPropertyChanged interface
        /// </summary>
        public event PropertyChangedEventHandler PropertyChanged;

        /// <summary>
        /// Raises the System.Windows.Window.Closed event.
        /// </summary>
        /// <param name="e">An System.EventArgs that contains the event data.</param>
        protected override void OnClosed(EventArgs e)
        {
            base.OnClosed(e);
        }

        #endregion Events

    }
}
