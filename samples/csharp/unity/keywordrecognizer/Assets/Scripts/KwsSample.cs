//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using UnityEngine;
using UnityEngine.UI;
using Microsoft.CognitiveServices.Speech;
using System.Threading.Tasks;
using System.IO;
using System;
using System.Collections;
#if PLATFORM_ANDROID
using UnityEngine.Android;
using UnityEngine.Networking;
#endif
#if PLATFORM_IOS
using UnityEngine.iOS;
using System.Collections;
#endif

public class KwsSample : MonoBehaviour
{
    // Hook up the two properties below with a Text and Button object in your UI.
    public Text outputText;
    public Button startRecoButton;

    private object threadLocker = new object();
    private bool waitingForReco;
    private string message;

    private bool micPermissionGranted = false;
    private string kwsModelDir;
    private string kwsModelFile = "kws.table";
    private const string keyword = "Computer";

#if PLATFORM_ANDROID || PLATFORM_IOS
    // Required to manifest microphone permission, cf.
    // https://docs.unity3d.com/Manual/android-manifest.html
    private Microphone mic;
#endif

    public async void ButtonClick()
    {
        try
        {
            lock (threadLocker)
            {
                waitingForReco = true;
            }            
            // Creates an instance of a speech config with specified subscription key and service region.
            // Replace with your own subscription key and service region (e.g., "westus").
            var config = SpeechConfig.FromSubscription("YourSubscriptionKey", "YourServiceRegion");

#if PLATFORM_ANDROID
            kwsModelDir = Application.persistentDataPath + Path.DirectorySeparatorChar + kwsModelFile;
#else
            kwsModelDir = Application.streamingAssetsPath + Path.DirectorySeparatorChar + kwsModelFile;
#endif
            UnityEngine.Debug.Log(kwsModelDir);
            var model = KeywordRecognitionModel.FromFile(kwsModelDir);

            var stopRecognition = new TaskCompletionSource<int>(TaskCreationOptions.RunContinuationsAsynchronously);
            var resultStr = "";

            // Creates a speech recognizer using microphone as audio input.
            using (var recognizer = new SpeechRecognizer(config))
            {
                // Subscribes to events.
                recognizer.Recognized += (s, e) =>
                {
                    if (e.Result.Reason == ResultReason.RecognizedKeyword)
                    {
                        resultStr = $"RECOGNIZED KEYWORD: '{e.Result.Text}'";
                    }
                    else if (e.Result.Reason == ResultReason.RecognizedSpeech)
                    {
                        resultStr = $"RECOGNIZED: '{e.Result.Text}'";
                    }
                    else if (e.Result.Reason == ResultReason.NoMatch)
                    {
                        resultStr = "NOMATCH: Speech could not be recognized.";
                    }
                    UnityEngine.Debug.Log(resultStr);
                    lock (threadLocker)
                    {
                        message = resultStr;
                    }
                };

                recognizer.Canceled += (s, e) =>
                {
                    var cancellation = CancellationDetails.FromResult(e.Result);
                    resultStr = $"CANCELED: Reason={cancellation.Reason} ErrorDetails={cancellation.ErrorDetails}";
                    if (cancellation.Reason == CancellationReason.Error)
                    {
                        lock (threadLocker)
                        {
                            message = resultStr;
                        }
                    }
                    UnityEngine.Debug.Log(resultStr);
                    stopRecognition.TrySetResult(0);
                };

                recognizer.SessionStarted += (s, e) =>
                {
                    UnityEngine.Debug.Log("\nSession started event.");
                };

                recognizer.SessionStopped += (s, e) =>
                {
                    UnityEngine.Debug.Log("\nSession stopped event.");
                    UnityEngine.Debug.Log("\nStop recognition.");
                    stopRecognition.TrySetResult(0);
                };

                UnityEngine.Debug.Log($"Say something starting with the keyword '{keyword}' followed by whatever you want...");

                // Starts continuous recognition using the keyword model. Use StopKeywordRecognitionAsync() to stop recognition.
                await recognizer.StartKeywordRecognitionAsync(model).ConfigureAwait(false);

                // Waits for a single successful keyword-triggered speech recognition (or error).
                // Use Task.WaitAny to keep the task rooted.
                Task.WaitAny(new[] { stopRecognition.Task });

                await recognizer.StopKeywordRecognitionAsync().ConfigureAwait(false);

                lock (threadLocker)
                {
                    waitingForReco = false;
                }
            }
        }
        catch (Exception ex)
        {
            lock (threadLocker)
            {
                message = "Exception: " + ex.ToString();
                waitingForReco = false;
            }
        }
    }

#if PLATFORM_ANDROID
    IEnumerator CopyKwsModelFileFromStreamingAssetsAsync()
    {
        string fromFile = Application.streamingAssetsPath + Path.DirectorySeparatorChar + kwsModelFile;
        string toFile = Application.persistentDataPath + Path.DirectorySeparatorChar + kwsModelFile;

        UnityWebRequest downloader = UnityWebRequest.Get(fromFile);
        yield return downloader.SendWebRequest();
        DownloadHandler handler = downloader.downloadHandler;
        File.WriteAllBytes(toFile, handler.data);
    }
#endif

    void Start()
    {
        Debug.Log("Start enter");

        if (outputText == null)
        {
            UnityEngine.Debug.LogError("outputText property is null! Assign a UI Text element to it.");
        }
        else if (startRecoButton == null)
        {
            message = "startRecoButton property is null! Assign a UI Button to it.";
            UnityEngine.Debug.LogError(message);
        }
        else
        {
            // Continue with normal initialization, Text and Button objects are present.
#if PLATFORM_ANDROID
            // Request to use the microphone, cf.
            // https://docs.unity3d.com/Manual/android-RequestingPermissions.html
            message = "Waiting for mic permission";
            if (!Permission.HasUserAuthorizedPermission(Permission.Microphone))
            {
                Permission.RequestUserPermission(Permission.Microphone);
            }
            StartCoroutine(CopyKwsModelFileFromStreamingAssetsAsync());

#elif PLATFORM_IOS
            if (!Application.HasUserAuthorization(UserAuthorization.Microphone))
            {
                Application.RequestUserAuthorization(UserAuthorization.Microphone);
            }
#else
            micPermissionGranted = true;
            message = "Click button to recognize speech using a keyword";
#endif
            startRecoButton.onClick.AddListener(ButtonClick);
        }
        Debug.Log("Start exit");
    }

    void Update()
    {
#if PLATFORM_ANDROID
        if (!micPermissionGranted && Permission.HasUserAuthorizedPermission(Permission.Microphone))
        {
            micPermissionGranted = true;
            message = "Click button to recognize speech";
        }
#elif PLATFORM_IOS
        if (!micPermissionGranted && Application.HasUserAuthorization(UserAuthorization.Microphone))
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
