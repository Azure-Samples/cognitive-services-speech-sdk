// <copyright file="MainWindow.xaml.cs" company="Microsoft">
// Copyright (c) Microsoft Corporation
// Licensed under the MIT license.
//
// MIT License:
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED ""AS IS"", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
// </copyright>

namespace Microsoft.CognitiveServices.SpeechRecognition
{
    using System;
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

    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window, INotifyPropertyChanged
    {
      
        #region Properties
       
        private const string CrisModelIdFileName = "CrisModelId.txt";
        private const string SubscriptionKeyFileName = "SubscriptionKey.txt";
   
        /// <summary>
        /// Gets the default locale.
        /// </summary>
        /// <value>
        /// The default locale.
        /// </value>
        private string DefaultLocale
        {
            get { return "en-US"; }
        }

        /// <summary>
        /// True, if audio source is mic
        /// </summary>
        public bool IsMicrophoneClient { get; set; }

        /// <summary>
        /// True, if audio source is audio file
        /// </summary>
        public bool IsDataClient { get; set; }

        /// <summary>
        /// Only baseline model used for recognition
        /// </summary>
        public bool IsBasicReco { get; set; }

        /// <summary>
        /// Only custom model used for recognition
        /// </summary>
        public bool IsCustomReco { get; set; }

        /// <summary>
        /// Both models used for recognition
        /// </summary>
        public bool IsBothReco { get; set; }
        
        private string subscriptionKey;
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

        private string crisModelId;
        /// <summary>
        /// Gets or sets Cris model ID
        /// </summary>
        public string CrisModelId
        {
            get
            {
                return this.crisModelId;
            }

            set
            {
                this.crisModelId = value?.Trim();
                this.OnPropertyChanged<string>();
            }
        }

        #endregion

        /// <summary>
        /// For this app there are two recognizers, one with the baseline model (Basic), one with CRIS model (Custom)
        /// </summary>
        enum RecoType
        {
            Basic = 1,
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
            this.IsMicrophoneClient= false;
            this.IsDataClient = true;

            this.IsBasicReco = false;
            this.IsCustomReco = false;
            this.IsBothReco = true;

            // Set the default choice for radio buttons.
            this.dataRadioButton.IsChecked = true;
            this.bothRadioButton.IsChecked = true;

            this.SubscriptionKey = this.GetValueFromIsolatedStorage(SubscriptionKeyFileName);
            this.CrisModelId = this.GetValueFromIsolatedStorage(CrisModelIdFileName);
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
              this.radioGroup.IsEnabled = false;
              this.optionPanel.IsEnabled = false;
              this.LogRecognitionStart(this.crisLogText, this.crisCurrentText);
              this.LogRecognitionStart(this.bingLogText, this.bingCurrentText);
              string wavFileName = "";

              if (!AreKeysValid())
              {
                  MessageBox.Show("Subscription Key or Model ID is wrong or missing!");
                  if (this.IsBasicReco || this.IsBothReco)
                  {
                      this.WriteLine(this.bingLogText, "--- Error : Subscription Key or Model Key is wrong or missing! ---");
                  }

                  if (this.IsCustomReco || this.IsBothReco)
                  {
                      this.WriteLine(this.crisLogText, "--- Error : Subscription Key or Model Key is wrong or missing! ---");
                  }

                  this.EnableButtons();
                  return;
              }

              if (!this.IsMicrophoneClient)
              {
                  wavFileName = GetFile();
                  if (wavFileName.Length <= 0) return;
                  Task.Run(() => this.PlayAudioFile(wavFileName));
              }

              if (this.IsCustomReco || this.IsBothReco)
              {
                  Task.Run(async ()=> { await CreateCustomReco(wavFileName).ConfigureAwait(false); });
              }

              if (this.IsBasicReco || this.IsBothReco)
              {
                  Task.Run(async() => { await CreateBasicReco(wavFileName).ConfigureAwait(false); });
              }
          }

          /// <summary>
          /// Creates Recognizer with baseline model:
          /// Creates a factory with subscription key, region West US
          /// If input source is audio file, creates recognizer with audio file otherwise with default mic
          /// Waits on RunRecognition
          /// </summary>
          /// <param name="wavFileName">file</param>
          private async Task CreateBasicReco(string wavFileName)
          {
              var basicFactory =  SpeechFactory.FromSubscription(this.SubscriptionKey,"westus");
              SpeechRecognizer basicRecognizer;

              if (this.IsMicrophoneClient)
              {
                  using (basicRecognizer = basicFactory.CreateSpeechRecognizer())
                  {
                      await this.RunRecognizer(basicRecognizer, RecoType.Basic).ConfigureAwait(false);
                  }
              }
              else
              {
                  using (basicRecognizer = basicFactory.CreateSpeechRecognizerWithFileInput(wavFileName))
                  {
                      await this.RunRecognizer(basicRecognizer, RecoType.Basic).ConfigureAwait(false);
                  }
              }
          }

          /// <summary>
          /// Creates Recognizer with custom model:
          /// Creates a factory with subscription key, region West US
          /// If input source is audio file, creates recognizer with audio file otherwise with default mic
          /// Waits on RunRecognition
          /// </summary>
          /// <param name="wavFileName">The source of the event.</param>
          private async Task CreateCustomReco(string wavFileName)
          {
              var customFactory = SpeechFactory.FromSubscription(this.SubscriptionKey, "westus");
              SpeechRecognizer customRecognizer;

              if (this.IsMicrophoneClient)
              {
                  using (customRecognizer = customFactory.CreateSpeechRecognizer())
                  {
                      customRecognizer.DeploymentId = this.CrisModelId;
                      await this.RunRecognizer(customRecognizer, RecoType.Custom).ConfigureAwait(false);
                  }
              }
              else
              {
                  using (customRecognizer = customFactory.CreateSpeechRecognizerWithFileInput(wavFileName))
                  {
                      customRecognizer.DeploymentId = this.CrisModelId;
                      await this.RunRecognizer(customRecognizer, RecoType.Custom).ConfigureAwait(false);
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
        ///   <c>Basic</c> if Baseline model; otherwise, <c>Custom</c>.
        /// </value>
        private async Task RunRecognizer(SpeechRecognizer recognizer, RecoType recoType)
        {
            //subscribe to events
            bool isChecked = false;
            this.Dispatcher.Invoke(() =>
            {
                isChecked = this.immediateResultsCheckBox.IsChecked == true;
            });
            var taskCompletionSource = new TaskCompletionSource<int>();
            if (isChecked)
                recognizer.IntermediateResultReceived += (sender, e) => IntermediateResultEventHandler(e, recoType);
            recognizer.FinalResultReceived += (sender, e) => FinalResultEventHandler(e, recoType, taskCompletionSource);
            recognizer.RecognitionErrorRaised += (sender, e) => ErrorEventHandler(e, recoType, taskCompletionSource);
            recognizer.OnSessionEvent += (sender, e) => SessionEventHandler(e, recoType, taskCompletionSource);
            recognizer.OnSpeechDetectedEvent += (sender, e) => SpeechDetectedEventHandler(e, recoType, taskCompletionSource);

            //start,wait,stop recognition
            await recognizer.StartContinuousRecognitionAsync().ConfigureAwait(false);
            await taskCompletionSource.Task.ConfigureAwait(false);
            await recognizer.StopContinuousRecognitionAsync().ConfigureAwait(false);

            this.EnableButtons();

            // unsubscribe from events
            if (isChecked)
                recognizer.IntermediateResultReceived -= (sender, e) => IntermediateResultEventHandler(e, recoType);
            recognizer.FinalResultReceived -= (sender, e) => FinalResultEventHandler(e, recoType, taskCompletionSource);
            recognizer.RecognitionErrorRaised -= (sender, e) => ErrorEventHandler(e, recoType, taskCompletionSource);
            recognizer.OnSessionEvent -= (sender, e) => SessionEventHandler(e, recoType, taskCompletionSource);
            recognizer.OnSpeechDetectedEvent -= (sender, e) => SpeechDetectedEventHandler(e, recoType, taskCompletionSource);
        }

        #region Recognition Event Handlers

        /// <summary>
        /// Logs Intermediate Recognition results
        /// </summary>
        private void IntermediateResultEventHandler(SpeechRecognitionResultEventArgs e, RecoType rt)
        {
            TextBox log;
            if (rt == RecoType.Basic)
            {
                log = this.bingLogText;
            }
            else
            {
                log = this.crisLogText;
            }

            this.WriteLine(log, "Intermediate result: {0} ", e.Result.Text);
        }

        /// <summary>
        /// Logs the Final result
        /// If input source is mic: sets the TaskCompletionSource to 0, in order to trigger Recognition Stop
        /// </summary>
        private void FinalResultEventHandler(SpeechRecognitionResultEventArgs e, RecoType rt, TaskCompletionSource<int> tcs)
        {
            TextBox log;
            if (rt == RecoType.Basic)
            {
                log = this.bingLogText;
                this.SetCurrentText(this.bingCurrentText, e.Result.Text);
            }
            else
            {
                log = this.crisLogText;
                this.SetCurrentText(this.crisCurrentText, e.Result.Text);
            }

            this.WriteLine(log);
            this.WriteLine(log, " --- Final result received --- ");
            this.WriteLine(log,  e.Result.Text);
        }

        /// <summary>
        /// Logs Error events
        /// And sets the TaskCompletionSource to 0, in order to trigger Recognition Stop
        /// </summary>
        private void ErrorEventHandler(RecognitionErrorEventArgs e, RecoType rt, TaskCompletionSource<int> tcs)
        {
            TextBox log;
            if (rt == RecoType.Basic)
            {
                log = this.bingLogText;
            }
            else
            {
                log = this.crisLogText;
            }
            tcs.TrySetResult(0);
            this.WriteLine(log, "--- Error received ---");
            this.WriteLine(log, "Reason {0}", e.Status);
            this.WriteLine(log);
        }

        /// <summary>
        /// If SessionStoppedEvent is received, sets the TaskCompletionSource to 0, in order to trigger Recognition Stop
        /// </summary>
        private void SessionEventHandler(SessionEventArgs e, RecoType rt, TaskCompletionSource<int> tcs)
        {
            if(e.EventType == SessionEventType.SessionStoppedEvent )
            {
                tcs.TrySetResult(0);
            }
            Console.WriteLine(String.Format("Speech recognition: Session event: {0}.", e.ToString()));
        }


        private void SpeechDetectedEventHandler(RecognitionEventArgs e, RecoType rt, TaskCompletionSource<int> tcs)
        {
            if (this.IsMicrophoneClient && e.EventType == RecognitionEventType.SpeechEndDetectedEvent)
            {
                var log = (rt == RecoType.Basic) ? this.bingLogText : this.crisLogText;
                this.WriteLine(log, "Speech end detected.");
                tcs.TrySetResult(0);
            }
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
            if(fileName != null && key != null)
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
                SaveKeyToIsolatedStorage(SubscriptionKeyFileName, this.SubscriptionKey);
                SaveKeyToIsolatedStorage(CrisModelIdFileName, this.CrisModelId);
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
                ((this.IsCustomReco || this.IsBothReco) && (this.crisModelId == null || this.crisModelId.Length <= 0)))
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
            string wavFileName = "";
            this.Dispatcher.Invoke(() =>
            {
                wavFileName = this.fileNameTextBox.Text;
            });
            if (!File.Exists(wavFileName))
            {
                MessageBox.Show("File does not exist!");
                this.WriteLine(this.bingLogText, "--- Error : File does not exist! ---");
                this.WriteLine(this.crisLogText, "--- Error : File does not exist! ---");
                this.EnableButtons();
                return "";
            }
            return wavFileName;
        }

        /// <summary>
        /// Plays the audio file
        /// </summary>
        private void PlayAudioFile(string filePath)
        {
            SoundPlayer player = new SoundPlayer(filePath);
            player.Load();
            player.Play();
        }

        /// <summary>
        /// Logs the recognition start.
        /// </summary>
        private void LogRecognitionStart(TextBox log, TextBlock currentText)
        {
            string recoSource;
            recoSource = this.IsMicrophoneClient ? "microphone" : "wav file";

            this.SetCurrentText(currentText, string.Empty);
            log.Clear();
            this.WriteLine(log, "\n--- Start speech recognition using " + recoSource + " in " + this.DefaultLocale + " language ----\n\n");
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
            var formattedStr = string.Format(format, args);
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
