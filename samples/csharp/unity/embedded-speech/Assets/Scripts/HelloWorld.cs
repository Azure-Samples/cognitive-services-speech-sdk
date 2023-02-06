//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//

using UnityEngine;
using UnityEngine.UI;
using Microsoft.CognitiveServices.Speech;
#if PLATFORM_ANDROID
using UnityEngine.Android;
#endif
using System.IO;
using System.Collections;
using UnityEngine.Networking; // for UnityWebRequest

public class HelloWorld : MonoBehaviour
{
    // Hook up the two properties below with a Text and Button object in your UI.
    public Text outputText;
    public Button startRecoButton;

    private object threadLocker = new object();
    private bool waitingForReco;
    private string message;

    private bool micPermissionGranted = false;

#if PLATFORM_ANDROID
    // Required to manifest microphone permission, cf.
    // https://docs.unity3d.com/Manual/android-manifest.html
    private Microphone mic;
#endif

    private static SpeechRecognizer recognizer = null;

    private static readonly string modelName = "YourEmbeddedSpeechRecognitionModelName";
    private static readonly string modelKey = "YourEmbeddedSpeechRecognitionModelKey";

#if PLATFORM_ANDROID
    // Embedded speech recognition models (and synthesis voices) must be
    // located as normal individual files on the device filesystem.
    // On Android, the model files in streaming assets are contained in a
    // compressed .jar file and need to be copied to e.g. the application
    // persistent data path before they can be used. List the files to be
    // copied in the array below, with relative paths as they appear under
    // StreamingAssets in the Unity Editor.
    private static readonly string[] modelFiles =
    {
        /*
        "relative/path/to/file1",
        "relative/path/to/file2",
         ...
        "relative/path/to/fileN"
        */
    };
#endif

    public async void ButtonClick()
    {
        lock (threadLocker)
        {
            waitingForReco = true;
        }

        // Starts speech recognition and returns after a single utterance is
        // recognized. The task returns the recognition text (or NoMatch) as
        // result.
        // Note: Since RecognizeOnceAsync() returns only a single utterance,
        // it is best suited for single-shot recognition, like a command or
        // query. For long-running multi-utterance recognition, use
        // StartContinuousRecognitionAsync() instead (see general C# samples).
        var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);

        // Check the result.
        string newMessage = string.Empty;
        if (result.Reason == ResultReason.RecognizedSpeech)
        {
            newMessage = result.Text;
        }
        else if (result.Reason == ResultReason.NoMatch)
        {
            newMessage = "NO MATCH: Speech could not be recognized.";
        }
        else if (result.Reason == ResultReason.Canceled)
        {
            var cancellation = CancellationDetails.FromResult(result);
            newMessage = $"CANCELED: Reason={cancellation.Reason} ErrorDetails={cancellation.ErrorDetails}";
        }

        lock (threadLocker)
        {
            message = newMessage;
            waitingForReco = false;
        }
    }

    IEnumerator InitRecognizerAsync()
    {
#if PLATFORM_ANDROID
        // Copy model files from streaming assets in the compressed .jar file
        // to the application persistent data path (on the device filesystem).
        foreach (string file in modelFiles)
        {
            string fromFilePath = Application.streamingAssetsPath + Path.DirectorySeparatorChar + file;
            string toFilePath = Application.persistentDataPath + Path.DirectorySeparatorChar + file;
            UnityEngine.Debug.Log($"EmbeddedSpeechSamples fromFilePath: {fromFilePath}");
            UnityEngine.Debug.Log($"EmbeddedSpeechSamples toFilePath:   {toFilePath}");

            // Copy files only the first time the sample is run.
            if (File.Exists(toFilePath))
            {
                UnityEngine.Debug.Log($"EmbeddedSpeechSamples file {file} already exists, skipped");
                continue;
            }

            UnityWebRequest downloader = UnityWebRequest.Get(fromFilePath);
            yield return downloader.SendWebRequest();
            DownloadHandler handler = downloader.downloadHandler;

            // Ensure that all directories in a target file path exist
            // before trying to write the file.
            FileInfo fileInfo = new FileInfo(toFilePath);
            fileInfo.Directory.Create();

            File.WriteAllBytes(toFilePath, handler.data);
        }

        string modelRootPath = Application.persistentDataPath;
#else
        yield return new WaitForSeconds(0.0f); // coroutine must have a yield statement
        string modelRootPath = Application.streamingAssetsPath;
#endif

        // Creates an instance of an embedded speech config.
        var config = EmbeddedSpeechConfig.FromPath(modelRootPath);
        config.SetSpeechRecognitionModel(modelName, modelKey);

        // Creates an instance of a speech recognizer.
        // When embedded speech config is used, this can take a moment due to
        // loading of the embedded model. Therefore it should be done well in
        // advance before recognition is started.
        try
        {
            recognizer = new SpeechRecognizer(config);
        }
        catch (System.Exception e)
        {
            message = e.ToString();
            UnityEngine.Debug.LogError(message);
            yield break;
        }

        // Ready to start recognition.
        message = "Click the button to recognize speech.";
        waitingForReco = false;
    }

    void Start()
    {
        UnityEngine.Debug.Log($"EmbeddedSpeechSamples streamingAssetsPath: {Application.streamingAssetsPath}");
        UnityEngine.Debug.Log($"EmbeddedSpeechSamples persistentDataPath:  {Application.persistentDataPath}");

        if (outputText == null)
        {
            message = "outputText property is null! Assign a UI Text element to it.";
            UnityEngine.Debug.LogError(message);
        }
        else if (startRecoButton == null)
        {
            message = "startRecoButton property is null! Assign a UI Button to it.";
            UnityEngine.Debug.LogError(message);
        }
#if PLATFORM_ANDROID
        else if (modelFiles.Length == 0)
        {
            message = "modelFiles is empty! Specify the model files to be copied.";
            UnityEngine.Debug.LogError(message);
        }
#endif
        else
        {
            // Continue with normal initialization, Text and Button objects are present.
#if PLATFORM_ANDROID
            // Request to use the microphone, cf.
            // https://docs.unity3d.com/Manual/android-RequestingPermissions.html
            message = "Waiting for microphone permission";
            if (!Permission.HasUserAuthorizedPermission(Permission.Microphone))
            {
                Permission.RequestUserPermission(Permission.Microphone);
            }
#else
            micPermissionGranted = true;
#endif
            waitingForReco = true;
            message = "Initializing the recognizer, please wait...";
            StartCoroutine(InitRecognizerAsync());

            startRecoButton.onClick.AddListener(ButtonClick);
        }
    }

    void OnDestroy()
    {
        if (recognizer != null)
        {
            recognizer.Dispose();
        }
    }

    void Update()
    {
#if PLATFORM_ANDROID
        if (!micPermissionGranted && Permission.HasUserAuthorizedPermission(Permission.Microphone))
        {
            micPermissionGranted = true;
            message = "Click button to recognize speech";
        }
#endif

        lock (threadLocker)
        {
            if (startRecoButton != null)
            {
                startRecoButton.interactable = !waitingForReco && micPermissionGranted;
            }
            if (outputText != null)
            {
                outputText.text = message;
            }
        }
    }
}
