//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using System;
using System.Threading.Tasks;
using System.Collections;
using System.Collections.Generic;
using Microsoft.CognitiveServices.Speech;
using Microsoft.CognitiveServices.Speech.Audio;
using Microsoft.CognitiveServices.Speech.Intent;
using UnityEngine;
using UnityEngine.UI;
#if PLATFORM_ANDROID
using UnityEngine.Android;
#endif
using IntentRecognitionResults;
using TinyJson;
using Debug = UnityEngine.Debug;

/// <summary>
/// The IntentRecognition class lets the user dictate voice commands via speech recognition and
/// Natural Language Understanding (NLU). Once captured, the voice command are interpreted based
/// on intents and entities returned by the LUIS service, and then executed against the various
/// shapes in the scene.
/// </summary>
public class IntentRecognition : MonoBehaviour {

    // Public fields in the Unity inspector
    [Tooltip("Unity UI Text component used to report potential errors on screen.")]
    public Text RecognizedText;
    [Tooltip("Unity UI Text component used to post recognition results on screen.")]
    public Text ErrorText;

    // Speech recognition key, not required when the IntentRecognizer is used
    [Tooltip("Azure API key for Cognitive Services Speech.")]
    public string SpeechServiceAPIKey = string.Empty;
    [Tooltip("Region for your Cognitive Services Speech instance (must match the key).")]
    public string SpeechServiceRegion = string.Empty;

    // LUIS AppId and service key
    [Tooltip("AppId for your LUIS model.")]
    public string LUISAppId = string.Empty;
    [Tooltip("Azure API key for your LUIS service.")]
    public string LUISAppKey = string.Empty;
    [Tooltip("Region for your LUIS service (must match the key).")]
    public string LUISRegion = string.Empty;

    // Assign these GameObjects in the Unity inspector to manipulate them via voice commands
    public GameObject Sphere;
    public GameObject Cube;
    public GameObject Cylinder;

    // Used to show live messages on screen, must be locked to avoid threading deadlocks since
    // the recognition events are raised in a separate thread
    private string recognizedString = "";
    private string errorString = "";
    private System.Object threadLocker = new System.Object();

    // Cognitive Services Speech objects used for Intent Recognition
    private IntentRecognizer intentreco;
    private IntentResult intent = null;
    private bool isIntentReady = false;

    private bool micPermissionGranted = false;
#if PLATFORM_ANDROID
    // Required to manifest microphone permission, cf.
    // https://docs.unity3d.com/Manual/android-manifest.html
    private Microphone mic;
#endif

    private void Awake()
    {
        // IMPORTANT INFO BEFORE YOU CAN USE THIS SAMPLE:
        // Get your own Cognitive Services LUIS subscription key for free by following the
        // instructions under the section titled 'Get LUIS key' in the article found at
        // https://docs.microsoft.com/azure/cognitive-services/luis/luis-get-started-cs-get-intent.
        // Use the inspector fields to manually set these values with your subscription info.
        // If you prefer to manually set your LUIS AppId, Key and Region in code,
        // then uncomment the three lines below and set the values to your own.
        //LUISAppId = "YourLUISAppId";
        //LUISAppKey = "YourLUISAppKey";
        //LUISRegion = "YourLUISServiceRegion";
    }

    // Use this for initialization
    void Start () {
#if PLATFORM_ANDROID
        // Request to use the microphone, cf.
        // https://docs.unity3d.com/Manual/android-RequestingPermissions.html
        recognizedString = "Waiting for microphone permission...";
        if (!Permission.HasUserAuthorizedPermission(Permission.Microphone))
        {
            Permission.RequestUserPermission(Permission.Microphone);
        }
#else
        micPermissionGranted = true;
#endif
    }

    /// <summary>
    /// Attach to button component used to launch continuous intent recognition
    /// </summary>
    public void StartContinuous()
    {
        if (micPermissionGranted)
        {
            StartContinuousIntentRecognition();
        }
        else
        {
            recognizedString = "This app cannot function without access to the microphone.";
        }
    }

    /// <summary>
    /// Creates and initializes the IntentRecognizer
    /// </summary>
    void CreateIntentRecognizer()
    {
        if (LUISAppKey.Length == 0 || LUISAppKey == "YourLUISAppKey")
        {
            recognizedString = "You forgot to obtain Cognitive Services LUIS credentials and inserting them in this app." + Environment.NewLine +
                               "See the README file and/or the instructions in the Awake() function for more info before proceeding.";
            errorString = "ERROR: Missing service credentials";
            Debug.LogError(errorString);
            return;
        }
        Debug.Log("Creating Intent Recognizer.");
        recognizedString = "Initializing intent recognition, please wait...";

        if (intentreco == null)
        {
            // Creates an instance of a speech config with specified subscription key
            // and service region. Note that in contrast to other services supported by
            // the Cognitive Services Speech SDK, the Language Understanding service
            // requires a specific subscription key from https://www.luis.ai/.
            // The Language Understanding service calls the required key 'endpoint key'.
            // Once you've obtained it, replace with below with your own Language Understanding subscription key
            // and service region (e.g., "westus").
            // The default language is "en-us".
            var config = SpeechConfig.FromSubscription(LUISAppKey, LUISRegion);
            // Creates an intent recognizer using microphone as audio input.
            intentreco = new IntentRecognizer(config);

            // Creates a Language Understanding model using the app id, and adds specific intents from your model
            var model = LanguageUnderstandingModel.FromAppId(LUISAppId);
            intentreco.AddIntent(model, "ChangeColor", "color");
            intentreco.AddIntent(model, "Transform", "transform");
            intentreco.AddIntent(model, "Help", "help");
            intentreco.AddIntent(model, "None", "none");

            // Subscribes to speech events.
            intentreco.Recognizing += RecognizingHandler;
            intentreco.Recognized += RecognizedHandler;
            intentreco.SpeechStartDetected += SpeechStartDetectedHandler;
            intentreco.SpeechEndDetected += SpeechEndDetectedHandler;
            intentreco.Canceled += CanceledHandler;
            intentreco.SessionStarted += SessionStartedHandler;
            intentreco.SessionStopped += SessionStoppedHandler;
        }
        Debug.Log("CreateIntentRecognizer exit");
    }

    /// <summary>
    /// Starts the IntentRecognizer which will remain active until stopped
    /// </summary>
    private async void StartContinuousIntentRecognition()
    {
        if (LUISAppId.Length == 0 || LUISAppKey.Length == 0 || LUISRegion.Length == 0)
        {
            errorString = "One or more LUIS subscription parameters are missing. Check your values and try again.";
            return;
        }

        Debug.Log("Starting Continuous Intent Recognition.");
        CreateIntentRecognizer();

        if (intentreco != null)
        {
            Debug.Log("Starting Intent Recognizer.");

            // Starts continuous intent recognition.
            await intentreco.StartContinuousRecognitionAsync().ConfigureAwait(false);

            recognizedString = "Intent Recognizer is now running.";
            Debug.Log("Intent Recognizer is now running.");
        }
        Debug.Log("Start Continuous Intent Recognition exit");
    }

    #region Intent Recognition Event Handlers
    private void SessionStartedHandler(object sender, SessionEventArgs e)
    {
        Debug.Log($"\n    Session started event. Event: {e.ToString()}.");
    }

    private void SessionStoppedHandler(object sender, SessionEventArgs e)
    {
        Debug.Log($"\n    Session event. Event: {e.ToString()}.");
        Debug.Log($"Session Stop detected. Stop the recognition.");
    }

    private void SpeechStartDetectedHandler(object sender, RecognitionEventArgs e)
    {
        Debug.Log($"SpeechStartDetected received: offset: {e.Offset}.");
    }

    private void SpeechEndDetectedHandler(object sender, RecognitionEventArgs e)
    {
        Debug.Log($"SpeechEndDetected received: offset: {e.Offset}.");
        Debug.Log($"Speech end detected.");
    }

    private void RecognizingHandler(object sender, IntentRecognitionEventArgs e)
    {
        if (e.Result.Reason == ResultReason.RecognizingSpeech)
        {
            Debug.Log($"HYPOTHESIS: Text={e.Result.Text}");
            lock (threadLocker)
            {
                recognizedString = $"HYPOTHESIS: {Environment.NewLine}{e.Result.Text}";
            }
        }
    }

    private void RecognizedHandler(object sender, IntentRecognitionEventArgs e)
    {
        if (e.Result.Reason == ResultReason.RecognizedIntent)
        {
            //Console.WriteLine($"    Language Understanding JSON: {e.Result.Properties.GetProperty(PropertyId.LanguageUnderstandingServiceResponse_JsonResult)}.");
            Debug.Log($"RECOGNIZED: Intent={e.Result.IntentId} Text={e.Result.Text}");
            lock (threadLocker)
            {
                recognizedString = $"RESULT: Intent={e.Result.IntentId}";
                string json = e.Result.Properties.GetProperty(PropertyId.LanguageUnderstandingServiceResponse_JsonResult);
                var result = json.FromJson<IntentResult>();
                if (result != null)
                {
                    recognizedString += $" [Confidence={result.topScoringIntent.score.ToString("0.000")}]";
                    if (result.entities.Count > 0)
                    {
                        recognizedString += $"{Environment.NewLine}Entities=";
                        for (int i = 0; i < result.entities.Count; i++)
                        {
                            recognizedString += $"[{result.entities[i].type}: {result.entities[i].entity}] ";
                        }
                    }
                    lock (threadLocker)
                    {
                        intent = result;
                        isIntentReady = true;   
                    }
                }
                recognizedString += $"{Environment.NewLine}{e.Result.Text}";
            }
        }
        if (e.Result.Reason == ResultReason.RecognizedSpeech)
        {
            Debug.Log($"RECOGNIZED: Text={e.Result.Text}");
            lock (threadLocker)
            {
                recognizedString = $"RESULT: {Environment.NewLine}{e.Result.Text}";
            }
        }
        else if (e.Result.Reason == ResultReason.NoMatch)
        {
            Debug.Log($"NOMATCH: Speech could not be recognized.");
        }
    }

    private void CanceledHandler(object sender, IntentRecognitionCanceledEventArgs e)
    {
        Debug.Log($"CANCELED: Reason={e.Reason}");

        errorString = e.ToString();
        if (e.Reason == CancellationReason.Error)
        {
            Debug.LogError($"CANCELED: ErrorDetails={e.ErrorDetails}");
            Debug.LogError("CANCELED: Did you update the subscription info?");
        }
    }
    #endregion

    // Update is called once per frame
    void Update () {
#if PLATFORM_ANDROID
        if (!micPermissionGranted && Permission.HasUserAuthorizedPermission(Permission.Microphone))
        {
            micPermissionGranted = true;
        }
#endif
        // Used to update results on screen during updates
        lock (threadLocker)
        {
            RecognizedText.text = recognizedString;
            ErrorText.text = errorString;
            if (isIntentReady && intent != null)
            {
                ProcessIntent(intent);
                isIntentReady = false;
                intent = null;
            }
        }
    }

    void OnDisable()
    {
        StopIntentRecognition();
    }

    /// <summary>
    /// IntentRecognizer & event handlers cleanup after use
    /// </summary>
    public async void StopIntentRecognition()
    {
        if (intentreco != null)
        {
            await intentreco.StopContinuousRecognitionAsync().ConfigureAwait(false);
            intentreco.Recognizing -= RecognizingHandler;
            intentreco.Recognized -= RecognizedHandler;
            intentreco.SpeechStartDetected -= SpeechStartDetectedHandler;
            intentreco.SpeechEndDetected -= SpeechEndDetectedHandler;
            intentreco.Canceled -= CanceledHandler;
            intentreco.SessionStarted -= SessionStartedHandler;
            intentreco.SessionStopped -= SessionStoppedHandler;
            intentreco.Dispose();
            intentreco = null;
            recognizedString = "Intent Recognizer is now stopped.";
            Debug.Log("Intent Recognizer is now stopped.");
        }
    }

    /// <summary>
    /// Processes the user's voice command captured via speech recognition based on the 
    /// intent & entities returned by the LUIS service.
    /// To learn more about a more advanced approach for processing LUIS results from
    /// within Unity projects, check out http://aka.ms/MrLuis (external blog link)
    /// </summary>
    /// <param name="intent"></param>
    void ProcessIntent(IntentResult intent)
    {
        string target = "";
        string attribute = "";

        // Extract the entities if any, this can be the target shape or the 
        // transformation such as colors or scaling.
        if (intent.entities.Count > 0)
        {
            recognizedString += $"{Environment.NewLine}Entities=";
            for (int i = 0; i < intent.entities.Count; i++)
            {
                if (intent.entities[i].type.ToLower() == "target")
                {
                    target = intent.entities[i].entity;
                }
                if (intent.entities[i].type.ToLower() == "color")
                {
                    attribute = intent.entities[i].entity;
                }
                if (intent.entities[i].type.ToLower() == "scaling")
                {
                    attribute = intent.entities[i].entity;
                }
            }
        }

        // We support many names to refer to the various shapes in the scene.
        GameObject targetObject;
        switch (target)
        {
            case "cube":
            case "box":
            case "square":
                targetObject = Cube;
                break;
            case "ball":
            case "sphere":
            case "circle":
                targetObject = Sphere;
                break;
            case "cylinder":
            case "tube":
            case "pipe":
                targetObject = Cylinder;
                break;
            default:
                targetObject = null;
                break;
        }

        // Identify the target color change if applicable
        Color color = Color.clear;
        float scalingratio = 0.0f;
        switch (attribute)
        {
            case "increase":
            case "large":
            case "big":
            case "bigger":
            case "larger":
            case "bigly":
            case "huge":
            case "enlarge":
                scalingratio = 0.1f;
                break;
            case "reduce":
            case "small":
            case "smaller":
            case "minimize":
            case "reduction":
            case "tiny":
            case "decrease":
                scalingratio = -0.1f;
                break;
            case "yellow":
                color = Color.yellow;
                break;
            case "black":
                color = Color.black;
                break;
            case "blue":
                color = Color.blue;
                break;
            case "cyan":
            case "turquoise":
            case "scion":   // common confusion in the recognizer
                color = Color.cyan;
                break;
            case "gray":
                color = Color.gray;
                break;
            case "green":
                color = Color.green;
                break;
            case "magenta":
            case "purple":
                color = Color.magenta;
                break;
            case "red":
            case "read":  // common confusion in the recognizer
                color = Color.red;
                break;
            case "orange":
                color = new Color(255, 112, 0);
                break;
            case "white":
                color = Color.white;
                break;
            default:
                color = Color.white;   
                break;
        }

        // Process the intent: ChangeColor or Transform (scaling)
        switch (intent.topScoringIntent.intent)
        {
            case "color":
            case "ChangeColor":
                if (targetObject != null)
                {
                    targetObject.GetComponent<Renderer>().material.color = color;
                }
                break;
            case "transform":
            case "Transform":
                if (targetObject != null)
                {
                    targetObject.transform.localScale += new Vector3(scalingratio, scalingratio, scalingratio);
                }
                break;
            case "help":
                // For future use
                break;
            case "none":
            default:
                break;
        }
    }
}
