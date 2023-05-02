//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for full license information.
//

using Microsoft.CognitiveServices.Speech;
using System.Text;

namespace EmbeddedSpeechSample;

public partial class MainPage : ContentPage
{
    /**********************************
     * START OF CONFIGURABLE SETTINGS *
     **********************************/
    private static readonly string recognitionModelName = ""; // e.g. "en-US" or "Microsoft Speech Recognizer en-US FP Model V8.1"
    private static readonly string recognitionModelKey  = ""; // model decryption key
    private static readonly string synthesisVoiceName   = ""; // e.g. "en-US-AriaNeural" or "Microsoft Server Speech Text to Speech Voice (en-US, AriaNeural)"
    private static readonly string synthesisVoiceKey    = ""; // voice decryption key

    // Embedded speech recognition models and synthesis voices must reside
    // as normal individual files on the device filesystem and they need to
    // be readable by the application process.

    // Model files bundled into the app package need to be copied to the
    // application data folder before they can be used. Note that modern
    // Android systems may restrict access to the external storage, ref.
    // https://developer.android.com/about/versions/11/privacy/storage

    // In case it is necessary to optimize storage space consumption on the
    // device, consider options:
    // * Copy files on demand i.e. only the files of a model/voice that's
    //   needed for a recognizer/synthesizer at a time; delete after use.
    // * Do not include models/voices in the application package, instead
    //   download additional data in a post-installation step (cf. games
    //   with large assets or navigation apps with offline maps).

    // List the files to be copied in the arrays below, with relative paths
    // as they appear under Resources\Raw in this project. If you don't need
    // either recognition or synthesis, leave the corresponding array empty.
    private static readonly string[] recognitionModelFiles =
    {
        /*
        "relative/path/to/file1",
        "relative/path/to/file2",
         ...
        "relative/path/to/fileN"
        */
    };

    private static readonly string[] synthesisVoiceFiles =
    {
        /*
        "relative/path/to/file1",
        "relative/path/to/file2",
         ...
        "relative/path/to/fileN"
        */
    };
    /********************************
     * END OF CONFIGURABLE SETTINGS *
     ********************************/

    private static SpeechRecognizer recognizer = null;
    private static SpeechSynthesizer synthesizer = null;

    private static readonly string modelSubFolder = "EmbeddedSpeechModels"; // in AppDataDirectory

    public MainPage()
    {
        InitializeComponent();
    }

    private async void ContentPage_Loaded(object sender, EventArgs e)
    {
        await CheckAndRequestMicrophonePermission();
    }

    // Check and request a permission to use the microphone for speech recognition input.
    // Ref. https://learn.microsoft.com/dotnet/maui/platform-integration/appmodel/permissions
    // Android needs also android.permission.RECORD_AUDIO in Platforms\Android\AndroidManifest.xml
    private async Task<PermissionStatus> CheckAndRequestMicrophonePermission()
    {
        PermissionStatus status = await Permissions.CheckStatusAsync<Permissions.Microphone>();

        if (status == PermissionStatus.Granted)
        {
            return status;
        }

        if (Permissions.ShouldShowRationale<Permissions.Microphone>())
        {
            // Prompt the user with additional information as to why the permission is needed
        }

        status = await Permissions.RequestAsync<Permissions.Microphone>();

        return status;
    }

    private void ContentPage_Unloaded(object sender, EventArgs e)
    {
        if (recognizer != null)
        {
            recognizer.Dispose();
        }
        if (synthesizer != null)
        {
            synthesizer.Dispose();
        }
    }

    private async void OnCopyFilesButtonClicked(object sender, EventArgs e)
    {
        // Copy embedded speech model files from the app package to the app
        // data folder on the device filesystem.
        try
        {
            var modelRootPath = FileSystem.Current.AppDataDirectory + Path.DirectorySeparatorChar + modelSubFolder;

            string[] filesToCopy = recognitionModelFiles.Concat(synthesisVoiceFiles).ToArray();
            if (filesToCopy.Length == 0)
            {
                UpdateStatus("No files to copy! Embedded speech will not work.");
                return;
            }

            foreach (string file in filesToCopy)
            {
                UpdateStatus($"Copying {file}...");

                // Open the source file.
                using Stream fileStream = await FileSystem.Current.OpenAppPackageFileAsync(file);

                // Ensure that all directories in a target file path exist
                // before trying to write the file.
                string targetFile = System.IO.Path.Combine(modelRootPath, file);
                FileInfo fileInfo = new FileInfo(targetFile);
                fileInfo.Directory.Create();

                // Copy the file contents to the app data directory.
                using FileStream outputStream = System.IO.File.OpenWrite(targetFile);
                await fileStream.CopyToAsync(outputStream);
            }
            UpdateStatus($"Copied files to {modelRootPath}");
        }
        catch (Exception ex)
        {
            UpdateStatus("Exception: " + ex.ToString());
        }
    }

    private void OnInitObjectsButtonClicked(object sender, EventArgs e)
    {
        try
        {
            UpdateStatus("Initializing...");

            // Creates an instance of embedded speech config.
            var modelRootPath = FileSystem.Current.AppDataDirectory + Path.DirectorySeparatorChar + modelSubFolder;
            var config = EmbeddedSpeechConfig.FromPath(modelRootPath);

            // Selects embedded speech models to use.
            config.SetSpeechRecognitionModel(recognitionModelName, recognitionModelKey);
            config.SetSpeechSynthesisVoice(synthesisVoiceName, synthesisVoiceKey);

            if (synthesisVoiceName.Contains("Neural"))
            {
                // Embedded neural voices only support 24 kHz sample rate.
                config.SetSpeechSynthesisOutputFormat(SpeechSynthesisOutputFormat.Riff24Khz16BitMonoPcm);
            }

            StringBuilder sb = new StringBuilder();
            sb.Append("Initialized");

            // Creates a speech recognizer instance using the device default
            // microphone for audio input.
            // With embedded speech, this can take a moment due to loading
            // of the model. To avoid unnecessary delays when recognition is
            // started, create the recognizer well in advance.
            if (!string.IsNullOrEmpty(recognitionModelName) && !string.IsNullOrEmpty(recognitionModelKey))
            {
                recognizer = new SpeechRecognizer(config);
                sb.Append(" recognizer");
            }

            // Creates a speech synthesizer instance using the device default
            // speaker for audio output.
            if (!string.IsNullOrEmpty(synthesisVoiceName) && !string.IsNullOrEmpty(synthesisVoiceKey))
            {
                synthesizer = new SpeechSynthesizer(config);
                if (recognizer != null)
                {
                    sb.Append(" and");
                }
                sb.Append(" synthesizer");
            }

            if (recognizer == null && synthesizer == null)
            {
                UpdateStatus("Cannot initialize recognizer or synthesizer!");
            }
            else
            {
                sb.Append(".");
                UpdateStatus(sb.ToString());
            }
        }
        catch (Exception ex)
        {
            UpdateStatus("Exception: " + ex.ToString());
        }
    }

    private async void OnRecognitionButtonClicked(object sender, EventArgs e)
    {
        if (recognizer == null)
        {
            UpdateStatus("Recognizer is not initialized!");
            return;
        }

        try
        {
            UpdateStatus("Recognizing...");

            // Starts speech recognition and returns after a single utterance is
            // recognized. The task returns the recognition text as result.
            // Note: Since RecognizeOnceAsync() returns only a single utterance,
            // it is best suited for single-shot recognition, like a command or
            // short query. For long-running multi-utterance recognition, use
            // StartContinuousRecognitionAsync() instead (see general C# samples).
            var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);

            // Checks result.
            StringBuilder sb = new StringBuilder();
            if (result.Reason == ResultReason.RecognizedSpeech)
            {
                sb.Append($"RECOGNIZED: Text={result.Text}");
            }
            else if (result.Reason == ResultReason.NoMatch)
            {
                var nomatch = NoMatchDetails.FromResult(result);
                sb.Append($"NO MATCH: Reason={nomatch.Reason}");
            }
            else if (result.Reason == ResultReason.Canceled)
            {
                var cancellation = CancellationDetails.FromResult(result);
                sb.Append($"CANCELED: Reason={cancellation.Reason}");

                if (cancellation.Reason == CancellationReason.Error)
                {
                    sb.Append($" ErrorCode={cancellation.ErrorCode}");
                    sb.Append($" ErrorDetails={cancellation.ErrorDetails}");
                }
            }

            UpdateStatus(sb.ToString());
        }
        catch (Exception ex)
        {
            UpdateStatus("Exception: " + ex.ToString());
        }
    }

    private async void OnSynthesisButtonClicked(object sender, EventArgs e)
    {
        if (synthesizer == null)
        {
            UpdateStatus("Synthesizer is not initialized!");
            return;
        }
        if (string.IsNullOrEmpty(SynthesisInputEntry.Text))
        {
            UpdateStatus("Missing text input for synthesis!");
            return;
        }

        try
        {
            UpdateStatus("Synthesizing...");

            // Synthesizes speech.
            using var result = await synthesizer.SpeakTextAsync(SynthesisInputEntry.Text).ConfigureAwait(false);

            // Checks result.
            StringBuilder sb = new StringBuilder();
            if (result.Reason == ResultReason.SynthesizingAudioCompleted)
            {
                sb.Append("Synthesis completed.");
            }
            else if (result.Reason == ResultReason.Canceled)
            {
                var cancellation = SpeechSynthesisCancellationDetails.FromResult(result);
                sb.Append($"CANCELED: Reason={cancellation.Reason}");

                if (cancellation.Reason == CancellationReason.Error)
                {
                    sb.Append($" ErrorCode={cancellation.ErrorCode}");
                    sb.Append($" ErrorDetails={cancellation.ErrorDetails}");
                }
            }

            UpdateStatus(sb.ToString());
        }
        catch (Exception ex)
        {
            UpdateStatus("Exception: " + ex.ToString());
        }
    }

    private void UpdateStatus(String message)
    {
        MainThread.BeginInvokeOnMainThread(() =>
        {
            StatusText.Text = message;
        });
    }
}

