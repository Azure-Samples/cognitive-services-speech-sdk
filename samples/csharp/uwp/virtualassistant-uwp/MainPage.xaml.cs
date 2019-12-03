//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using Microsoft.Bot.Schema;
using Microsoft.CognitiveServices.Speech;
using Microsoft.CognitiveServices.Speech.Audio;
using Microsoft.CognitiveServices.Speech.Dialog;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using System;
using System.Collections.ObjectModel;
using System.Runtime.InteropServices;
using System.Threading.Tasks;
using Windows.Foundation;
using Windows.Media;
using Windows.Media.Audio;
using Windows.Media.MediaProperties;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace VirtualAssistantPreview
{
    // We are initializing a COM interface for use within the namespace
    // This interface allows access to memory at the byte level which we need to populate audio data that is generated
    [ComImport]
    [Guid("5B0D3235-4DBA-4D44-865E-8F1D0E4FD04D")]
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    unsafe interface IMemoryBufferByteAccess
    {
        void GetBuffer(out byte* buffer, out uint capacity);
    }

    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainPage : Page
    {
        public DialogServiceConnector dialogServiceConnector;
        // The collection of messages to the user from the bot or app
        public ObservableCollection<MessageDisplay> Messages { get; private set; } = new ObservableCollection<MessageDisplay>();

        // Collection of activities received from the bot
        public ObservableCollection<ActivityDisplay> Activities { get; private set; } = new ObservableCollection<ActivityDisplay>();

        private AudioGraph audioGraph;
        private AudioDeviceOutputNode outputNode;
        private AudioFrameInputNode frameInputNode;
        private PullAudioOutputStream audioStream;

        public MainPage()
        {
            this.InitializeComponent();
            this.DataContext = this;
        }

        protected override async void OnNavigatedTo(NavigationEventArgs e)
        {
            base.OnNavigatedTo(e);
            await CreateAudioGraph();
        }
        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            if (audioGraph != null)
            {
                audioGraph.Dispose();
            }
        }

        /// <summary>
        /// Create a DialogServiceConnector from the user-provided input
        /// </summary>
        public void InitDialogServiceConnector()
        {
            DialogServiceConfig dialogServiceConfig = null;

            dialogServiceConfig = BotFrameworkConfig.FromSubscription(SubscriptionTB.Text, RegionTB.Text);

            if (dialogServiceConnector != null)
            {
                dialogServiceConnector.SessionStarted -= DialogServiceConnector_SessionStarted;
                dialogServiceConnector.SessionStopped -= DialogServiceConnector_SessionStopped;
                dialogServiceConnector.Recognizing -= DialogServiceConnector_Recognizing;
                dialogServiceConnector.Recognized -= DialogServiceConnector_Recognized;
                dialogServiceConnector.ActivityReceived -= DialogServiceConnector_ActivityReceived;
                dialogServiceConnector.Canceled -= DialogServiceConnector_Canceled;
            }

            var audioConfig = AudioConfig.FromDefaultMicrophoneInput();
            dialogServiceConnector = new DialogServiceConnector(dialogServiceConfig, audioConfig);
            dialogServiceConnector.SessionStarted += DialogServiceConnector_SessionStarted;
            dialogServiceConnector.SessionStopped += DialogServiceConnector_SessionStopped;
            dialogServiceConnector.Recognizing += DialogServiceConnector_Recognizing;
            dialogServiceConnector.Recognized += DialogServiceConnector_Recognized;
            dialogServiceConnector.ActivityReceived += DialogServiceConnector_ActivityReceived;
            dialogServiceConnector.Canceled += DialogServiceConnector_Canceled;

            SendActivityButton.IsEnabled = true;
            StartButton.IsEnabled = true;
        }

        private void DialogServiceConnector_SessionStarted(object sender, SessionEventArgs e)
        {
            UpdateUI(() =>
            {
                this.Messages.Add(new MessageDisplay("Session started, begin speaking", Sender.Other));
            });
        }

        private void DialogServiceConnector_SessionStopped(object sender, SessionEventArgs e)
        {
            UpdateUI(() =>
            {
                this.Messages.Add(new MessageDisplay("Session stopped", Sender.Other));
            });
        }

        private void DialogServiceConnector_Recognizing(object sender, SpeechRecognitionEventArgs e)
        {
            //throw new NotImplementedException();
        }

        private void DialogServiceConnector_Recognized(object sender, SpeechRecognitionEventArgs e)
        {
            var result = e.Result.Text;

            if (e.Result.Reason == ResultReason.RecognizedSpeech)
            {
                result = $"Recognized: {e.Result.Text}";
            }
            else
            {
                result = $"Result reason \"{e.Result.Reason}\" received";
            }

            UpdateUI(() =>
            {
                this.Messages.Add(new MessageDisplay(result, Sender.Bot));
            });
        }

        private async void DialogServiceConnector_ActivityReceived(object sender, ActivityReceivedEventArgs e)
        {
            var json = e.Activity;
            var activity = JsonConvert.DeserializeObject<Activity>(json);
            if (e.HasAudio)
            {
                UpdateUI(() =>
                {
                    this.Messages.Add(new MessageDisplay("Audio received", Sender.Other));
                });

                audioStream = e.Audio;
                frameInputNode.Start();
            }

            await UpdateActivity(json);
            UpdateUI(() =>
            {
                this.Activities.Add(new ActivityDisplay(json, activity, Sender.Bot));
            });
        }

        private void DialogServiceConnector_Canceled(object sender, SpeechRecognitionCanceledEventArgs e)
        {
            string cancelText = $"Canceled with reason: {e.Reason}";

            if (e.Reason.Equals(CancellationReason.Error))
            {
                cancelText += "\n" + $"Cancellation error code: {e.ErrorCode}";
                cancelText += "\n" + $"Cancellation details: {e.ErrorDetails}";
            }

            UpdateUI(() =>
            {
                this.Messages.Add(new MessageDisplay(cancelText, Sender.Other));
            });
        }

        private void StartButton_Click(object sender, RoutedEventArgs e)
        {
            dialogServiceConnector.ListenOnceAsync();
        }

        private async void ConfigureButton_Click(object sender, RoutedEventArgs e)
        {
            await CheckAndEnableMic();
            InitDialogServiceConnector();
        }

        // Check if the app has mic permissions. If not, query user for access
        private async Task CheckAndEnableMic()
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
                UpdateUI(() =>
                {
                    this.Messages.Add(new MessageDisplay("Microphone enabled", Sender.Other));
                });
            }
        }

        private void SendActivity_Click(object sender, RoutedEventArgs e)
        {
            string speakActivity = CustomActivityTextbox.Text;
            dialogServiceConnector.SendActivityAsync(speakActivity);

            var activity = JsonConvert.DeserializeObject<Activity>(speakActivity);
            UpdateUI(() =>
            {
                this.Activities.Add(new ActivityDisplay(speakActivity, activity, Sender.User));
            });

        }

        private async void UpdateUI(Action action)
        {
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                action.Invoke();
            });
        }

        async Task<bool> UpdateActivity(string text)
        {
            await Windows.ApplicationModel.Core.CoreApplication.MainView.CoreWindow.Dispatcher.RunAsync(CoreDispatcherPriority.Normal,
                () =>
                {
                    BuildTreeFromJsonString(text);

                    foreach (var node in JSONTreeView.RootNodes)
                    {
                        JSONTreeView.Expand(node);
                    }
                });
            return true;
        }

        /// <summary>
        /// Setup an AudioGraph with PCM input node and output for media playback
        /// </summary>
        private async Task CreateAudioGraph()
        {
            AudioGraphSettings graphSettings = new AudioGraphSettings(Windows.Media.Render.AudioRenderCategory.Media);
            CreateAudioGraphResult graphResult = await AudioGraph.CreateAsync(graphSettings);
            if (graphResult.Status != AudioGraphCreationStatus.Success)
            {
                UpdateUI(() =>
                {
                    this.Messages.Add(new MessageDisplay($"Error in AudioGraph construction: {graphResult.Status.ToString()}", Sender.Other));
                });
            }

            audioGraph = graphResult.Graph;

            CreateAudioDeviceOutputNodeResult outputResult = await audioGraph.CreateDeviceOutputNodeAsync();
            if (outputResult.Status != AudioDeviceNodeCreationStatus.Success)
            {
                UpdateUI(() =>
                {
                    this.Messages.Add(new MessageDisplay($"Error in audio OutputNode construction: {outputResult.Status.ToString()}", Sender.Other));
                });
            }

            outputNode = outputResult.DeviceOutputNode;

            // Create the FrameInputNode using PCM format; 16kHz, 1 channel, 16 bits per sample
            AudioEncodingProperties nodeEncodingProperties = AudioEncodingProperties.CreatePcm(16000, 1, 16);
            frameInputNode = audioGraph.CreateFrameInputNode(nodeEncodingProperties);
            frameInputNode.AddOutgoingConnection(outputNode);

            // Initialize the FrameInputNode in the stopped state
            frameInputNode.Stop();

            // Hook up an event handler so we can start generating samples when needed
            // This event is triggered when the node is required to provide data
            frameInputNode.QuantumStarted += node_QuantumStarted;

            audioGraph.Start();
        }

        private void node_QuantumStarted(AudioFrameInputNode sender, FrameInputNodeQuantumStartedEventArgs args)
        {
            uint numSamplesNeeded = (uint)args.RequiredSamples;

            if (numSamplesNeeded != 0)
            {
                AudioFrame audioData = ReadAudioData(numSamplesNeeded);
                frameInputNode.AddFrame(audioData);
            }
        }

        private unsafe AudioFrame ReadAudioData(uint samples)
        {
            // Buffer size is (number of samples) * (size of each sample)
            uint bufferSize = samples * sizeof(byte) * 2;
            AudioFrame frame = new Windows.Media.AudioFrame(bufferSize);

            using (AudioBuffer buffer = frame.LockBuffer(AudioBufferAccessMode.Write))
            using (IMemoryBufferReference reference = buffer.CreateReference())
            {
                byte* dataInBytes;
                uint capacityInBytes;

                // Get the buffer from the AudioFrame
                ((IMemoryBufferByteAccess)reference).GetBuffer(out dataInBytes, out capacityInBytes);

                // Read audio data from the stream and copy it to the AudioFrame buffer
                var readBytes = new byte[capacityInBytes];
                uint bytesRead = audioStream.Read(readBytes);

                if (bytesRead == 0)
                {
                    frameInputNode.Stop();
                }

                for (int i = 0; i < bytesRead; i++)
                {
                    dataInBytes[i] = readBytes[i];
                }
            }

            return frame;
        }

        // Given a well-formed json string, provides a tree view of the object
        public void BuildTreeFromJsonString(string jsonString)
        {
            if (string.IsNullOrWhiteSpace(jsonString))
                return;

            JSONTreeView.RootNodes.Clear();

            JContainer json;
            try
            {
                if (jsonString.StartsWith("["))
                {
                    json = JArray.Parse(jsonString);
                    JSONTreeView.RootNodes.Add(JsonTreeBuilder.JsonToTreeNode((JArray)json, "{ }"));
                }
                else
                {
                    json = JObject.Parse(jsonString);
                    JSONTreeView.RootNodes.Add(JsonTreeBuilder.JsonToTreeNode((JObject)json, "{ }"));
                }
            }
            catch (JsonReaderException)
            {
                UpdateUI(() =>
                {
                    this.Messages.Add(new MessageDisplay("Invalid JSON", Sender.Other));
                });
            }
        }

        private async void ActivitiesListView_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            // Populate treeview with appropriate item
            await UpdateActivity(Activities[ActivitiesListView.SelectedIndex].Json);
        }
    }
}
