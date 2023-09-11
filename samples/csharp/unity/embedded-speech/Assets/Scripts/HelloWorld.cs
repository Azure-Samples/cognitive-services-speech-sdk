//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//

using Microsoft.CognitiveServices.Speech;
using System;
using System.Collections;
using System.IO;
using System.Linq;
using System.Text;
using UnityEngine;
#if PLATFORM_ANDROID
using UnityEngine.Android;
#endif
using UnityEngine.Networking; // for UnityWebRequest
using UnityEngine.UI;

public class HelloWorld : MonoBehaviour
{
    /**********************************
     * START OF CONFIGURABLE SETTINGS *
     **********************************/
    private static readonly string recognitionModelName = ""; // e.g. "en-US" or "Microsoft Speech Recognizer en-US FP Model V8.1"
    private static readonly string recognitionModelKey  = ""; // model decryption key
    private static readonly string synthesisVoiceName   = ""; // e.g. "en-US-AriaNeural" or "Microsoft Server Speech Text to Speech Voice (en-US, AriaNeural)"
    private static readonly string synthesisVoiceKey    = ""; // voice decryption key

#if PLATFORM_ANDROID
    // Embedded speech recognition models and synthesis voices must reside
    // as normal individual files on the device filesystem and they need to
    // be readable by the application process.

    // On Android, the model files in streaming assets are contained in a
    // compressed .jar file and need to be copied to e.g. the application
    // persistent data path before they can be used. Note that modern
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
    // as they appear under StreamingAssets in the Unity Editor. If either
    // recognition or synthesis is not needed, leave the corresponding array
    // empty.
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
#endif
    /********************************
     * END OF CONFIGURABLE SETTINGS *
     ********************************/

    private static SpeechRecognizer recognizer = null;
    private static SpeechSynthesizer synthesizer = null;

#if PLATFORM_ANDROID
    private static readonly string modelSubFolder = "EmbeddedSpeechModels"; // in Application.persistentDataPath
#endif

    // Hook up the properties below with objects in your UI.
    public Text statusOutput;
    public Button copyFilesButton;
    public Button initObjectsButton;
    public Button recognizeButton;
    public InputField synthesisInput;
    public Button synthesizeButton;

    private string statusMessage;
    private bool enableCopyFilesButton;
    private bool enableInitObjectsButton;
    private bool enableRecognitionButton;
    private bool enableSynthesisButton;

    private object threadLocker = new object();

    private bool micPermissionGranted = false;

#if PLATFORM_ANDROID
    // Required to manifest microphone permission, cf.
    // https://docs.unity3d.com/Manual/android-manifest.html
    private Microphone mic;
#endif

    public void OnCopyFilesButtonClicked()
    {
        lock (threadLocker)
        {
            enableCopyFilesButton = false;
        }
        StartCoroutine(CopyFilesAsync());
    }

    IEnumerator CopyFilesAsync()
    {
#if PLATFORM_ANDROID
        // Copy embedded speech model files from streaming assets in the
        // compressed .jar file to the application persistent data path
        // on the device filesystem.

        string[] filesToCopy = recognitionModelFiles.Concat(synthesisVoiceFiles).ToArray();
        if (filesToCopy.Length == 0)
        {
            statusMessage = "No files to copy! Embedded speech will not work.";
            UnityEngine.Debug.LogError(statusMessage);
            yield break;
        }

        var modelRootPath = Application.persistentDataPath + Path.DirectorySeparatorChar + modelSubFolder;

        foreach (string file in filesToCopy)
        {
            statusMessage = $"Copying {file}...";
            string fromFilePath = Application.streamingAssetsPath + Path.DirectorySeparatorChar + file;
            string toFilePath = modelRootPath + Path.DirectorySeparatorChar + file;
            UnityEngine.Debug.Log($"EmbeddedSpeechSamples Copying {fromFilePath} to {toFilePath}");

            UnityWebRequest downloader = UnityWebRequest.Get(fromFilePath);
            yield return downloader.SendWebRequest();
            DownloadHandler handler = downloader.downloadHandler;

            // Ensure that all directories in a target file path exist
            // before trying to write the file.
            FileInfo fileInfo = new FileInfo(toFilePath);
            fileInfo.Directory.Create();

            File.WriteAllBytes(toFilePath, handler.data);
        }

        statusMessage = $"Copied files to {modelRootPath}";
#else
        statusMessage = "No need to copy files on this platform.";
        yield return new WaitForSeconds(0.0f); // coroutine must have a yield statement
#endif

        enableCopyFilesButton = true;
        enableInitObjectsButton = true; // in case initialization was tried before copying and it failed
    }

    public void OnInitObjectsButtonClicked()
    {
        try
        {
            lock (threadLocker)
            {
                enableInitObjectsButton = false;
                statusMessage = "Initializing...";
            }

#if PLATFORM_ANDROID
            var modelRootPath = Application.persistentDataPath + Path.DirectorySeparatorChar + modelSubFolder;
#else
            // Files in streaming assets can be used directly.
            var modelRootPath = Application.streamingAssetsPath;
#endif

            // Creates an instance of embedded speech config.
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
                statusMessage = "Cannot initialize recognizer or synthesizer!";
                UnityEngine.Debug.LogError(statusMessage);
            }
            else
            {
                sb.Append(".");
                statusMessage = sb.ToString();
            }

            enableInitObjectsButton = true;
        }
        catch (Exception ex)
        {
            statusMessage = "Exception: " + ex.ToString();
            UnityEngine.Debug.LogError(statusMessage);
        }
    }

    public async void OnRecognizeButtonClicked()
    {
        if (recognizer == null)
        {
            statusMessage = "Recognizer is not initialized!";
            UnityEngine.Debug.LogError(statusMessage);
            return;
        }

        try
        {
            lock (threadLocker)
            {
                enableRecognitionButton = false;
                statusMessage = "Recognizing...";
            }

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

            statusMessage = sb.ToString();
            enableRecognitionButton = true;
        }
        catch (Exception ex)
        {
            statusMessage = "Exception: " + ex.ToString();
            UnityEngine.Debug.LogError(statusMessage);
        }
    }

    public async void OnSynthesizeButtonClicked()
    {
        if (synthesizer == null)
        {
            statusMessage = "Synthesizer is not initialized!";
            UnityEngine.Debug.LogError(statusMessage);
            return;
        }
        if (string.IsNullOrEmpty(synthesisInput.text))
        {
            statusMessage = "Missing text input for synthesis!";
            UnityEngine.Debug.LogError(statusMessage);
            return;
        }

        try
        {
            lock (threadLocker)
            {
                enableSynthesisButton = false;
                statusMessage = "Synthesizing...";
            }

            // Synthesizes speech.
            using var result = await synthesizer.SpeakTextAsync(synthesisInput.text).ConfigureAwait(false);

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

            statusMessage = sb.ToString();
            enableSynthesisButton = true;
        }
        catch (Exception ex)
        {
            statusMessage = "Exception: " + ex.ToString();
            UnityEngine.Debug.LogError(statusMessage);
        }
    }

    void Start()
    {
        UnityEngine.Debug.Log($"EmbeddedSpeechSamples streamingAssetsPath: {Application.streamingAssetsPath}");
        UnityEngine.Debug.Log($"EmbeddedSpeechSamples persistentDataPath:  {Application.persistentDataPath}");

        if (statusOutput == null ||
            copyFilesButton == null ||
            initObjectsButton == null ||
            recognizeButton == null ||
            synthesisInput == null ||
            synthesizeButton == null)
        {
            statusMessage = "One or more properties are null! Assign UI objects to them.";
            UnityEngine.Debug.LogError(statusMessage);
        }
        else
        {
            // Continue with normal initialization.
#if PLATFORM_ANDROID
            // Request to use the microphone, ref.
            // https://docs.unity3d.com/Manual/android-RequestingPermissions.html
            if (!Permission.HasUserAuthorizedPermission(Permission.Microphone))
            {
                Permission.RequestUserPermission(Permission.Microphone);
            }
#else
            micPermissionGranted = true;
#endif
            enableCopyFilesButton = true;
            enableInitObjectsButton = true;
            enableRecognitionButton = true;
            enableSynthesisButton = true;

            copyFilesButton.onClick.AddListener(OnCopyFilesButtonClicked);
            initObjectsButton.onClick.AddListener(OnInitObjectsButtonClicked);
            recognizeButton.onClick.AddListener(OnRecognizeButtonClicked);
            synthesizeButton.onClick.AddListener(OnSynthesizeButtonClicked);

            statusMessage = "Welcome to the Embedded Speech sample for Unity!";
        }
    }

    void OnDestroy()
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

    void Update()
    {
#if PLATFORM_ANDROID
        if (!micPermissionGranted && Permission.HasUserAuthorizedPermission(Permission.Microphone))
        {
            micPermissionGranted = true;
        }
#endif

        lock (threadLocker)
        {
            if (copyFilesButton != null)
            {
                copyFilesButton.interactable = enableCopyFilesButton;
            }
            if (initObjectsButton != null)
            {
                initObjectsButton.interactable = enableInitObjectsButton;
            }
            if (recognizeButton != null)
            {
                recognizeButton.interactable = enableRecognitionButton && micPermissionGranted;
            }
            if (synthesizeButton != null)
            {
                synthesizeButton.interactable = enableSynthesisButton;
            }
            if (statusOutput != null)
            {
                statusOutput.text = statusMessage;
            }
        }
    }
}
