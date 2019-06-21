//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.IO;
using UnityEngine;
using UnityEngine.UI;
using Microsoft.CognitiveServices.Speech;
using Microsoft.CognitiveServices.Speech.Dialog;
using Microsoft.CognitiveServices.Speech.Audio;
using System.Text;

/// <summary>
/// An example usage of SpeechBotConnector with the Speech SDK Unity plugin.
/// </summary>
public class SpeechBotConnectorSample : MonoBehaviour
{
    public string subscriptionKey;
    public string region;
    public string connectionId;

    public Button listenOnceButton;
    public Text recognizedText;
    public Text errorText;
    public Text stateIndicator;
    private string recognizedString = "";
    private string errorString = "";
    private string stateIndicatorString = "Initializing..";

    private object threadLocker = new object();

    private SpeechBotConnector botConnector;
    private BotConnectorConfig botConnectorConfig;

    private WaveAudioData audioData = null;
    private AudioSource ttsAudio;
    private MemoryStream byteStream;
    private MemoryStream finalStream;

    bool listenStarted = false;
    bool audioStarted = false;

    private async void Awake()
    {
        Debug.Log($"Awake enter");
        if (botConnector == null)
        {
            Debug.Log($"BotConnector is null, creating now");
            CreateBotConnector();
        }
        Debug.Log($"Connecting to Bot", this);
        await botConnector.ConnectAsync();

        Debug.Log($"Awake exit");
    }

    /// <summary>
    /// Explicitly calls ConnectAsync on the BotConnector and begins listening interation
    /// </summary>
    private void Start()
    {
        if (listenOnceButton == null)
        {
            stateIndicator.text = "listenOnceButton property is null! Assign a UI Button to it.";
            UnityEngine.Debug.LogError(stateIndicator.text);
        }
        listenOnceButton.onClick.AddListener(ListenOnceButtonClick);

        byteStream = new MemoryStream();
        finalStream = new MemoryStream();
    }

    // Update is called once per frame
    private void Update()
    {
        lock (threadLocker)
        {
            recognizedText.text = recognizedString;
            errorText.text = errorString;
            stateIndicator.text = stateIndicatorString;
        }

        if (audioData != null)
        {
            Debug.Log($"Creating and playing audio clip");

            var clip = AudioClip.Create("TTS_Audio", audioData.AudioSamples, 1, 16000, false);

            clip.SetData(audioData.AudioData, 0);
            ttsAudio.clip = clip;
            ttsAudio.Play();

            audioStarted = true;
            audioData = null;
        }

        if (audioStarted && !ttsAudio.isPlaying)
        {
            audioStarted = false;
        }
    }

    private void ListenOnceButtonClick()
    {
        StartListening();
    }

    /// <summary>
    /// Uses the provided properties to create a connector from config and register callbacks
    /// </summary>
    private void CreateBotConnector()
    {
        Debug.Log($"CreateBotConnector enter");

        if (botConnector == null)
        {
            if (connectionId == string.Empty || subscriptionKey == string.Empty || region == string.Empty)
            {
                Debug.Log($"One or more input fields weren't provided. Check the fields in the Canvas object or in the script source");
                throw new InvalidOperationException("BotConnectorConfig creation failed");
            }

            // Creates an instance of a BotConnectorConfig with your bot connection ID, subscription key, and service region.
            // Replace in the editor on the Canvas object OR directly in the code, above in the member declarations
            botConnectorConfig = BotConnectorConfig.FromSecretKey(connectionId, subscriptionKey, region);
            if (botConnectorConfig == null)
            {
                Debug.Log($"One or more input fields weren't provided. Check the fields in the Canvas object or in the script source");
                throw new InvalidOperationException("BotConnectorConfig creation failed");
            }

            AudioConfig audioConfig = AudioConfig.FromDefaultMicrophoneInput();
            botConnector = new SpeechBotConnector(botConnectorConfig, audioConfig);

            botConnector.ActivityReceived += BotConnector_ActivityReceived;
            botConnector.Canceled += BotConnector_Canceled;
            botConnector.Recognized += BotConnector_Recognized;
        }

        listenOnceButton.interactable = true;
        stateIndicatorString = "SpeechBotConnector created";

        ttsAudio = GetComponent<AudioSource>();

        Debug.Log($"CreateBotConnector exit");
    }

    private void OnDisable()
    {
        if (botConnector != null)
        {
            botConnector.ActivityReceived -= BotConnector_ActivityReceived;
            botConnector.Canceled -= BotConnector_Canceled;
            botConnector.Recognized -= BotConnector_Recognized;
            botConnector.Dispose();
        }

        byteStream.Close();
        finalStream.Close();
    }

    /// <summary>
    /// Begins speech recognition, single turn
    /// </summary>
    public bool StartListening()
    {
        if (botConnector == null)
        {
            throw new InvalidOperationException("BotConnector not initialized prior to starting interaction");
        }

        if (listenStarted)
        {
            Debug.LogError("StartListening called when library was already listening", this);
            return false;
        }

        botConnector.ListenOnceAsync();
        listenStarted = true;
        stateIndicatorString = "Listening...";

        Debug.Log($"StartListening: ListenOnceAsync called", this);
        return true;
    }

    /// <summary>
    /// Processes Recognized events, used here to display final result
    /// </summary>
    private void BotConnector_Recognized(object sender, SpeechRecognitionEventArgs e)
    {
        stateIndicatorString = "Final recognition:";
        if (e.Result.Reason == ResultReason.RecognizedSpeech)
        {
            Debug.LogFormat($"Recognized:\r\n {e.Result.Text}");
            lock (threadLocker)
            {
                recognizedString = e.Result.Text;
            }
        }
        else if (e.Result.Reason == ResultReason.NoMatch)
        {
            Debug.LogFormat($"Speech could not be recognized.");
        }

        listenStarted = false;
    }

    private void BotConnector_Canceled(object sender, SpeechRecognitionCanceledEventArgs e)
    {
        stateIndicatorString = "Cancellation";
        Debug.LogFormat($"Canceled with reason: {e.Reason}");

        if (e.Reason.Equals(CancellationReason.Error))
        {
            Debug.LogFormat($"Cancellation error code: {e.ErrorCode}");
            Debug.LogFormat($"Cancellation details: {e.ErrorDetails}");
        }
    }

    /// <summary>
    /// Processes ActivityReceived events.
    /// Along with various info that can be found in an event, it also contains
    /// a "HasAudio" flag that can be used to signal audio is present/ready for processing
    /// </summar>
    private void BotConnector_ActivityReceived(object sender, ActivityReceivedEventArgs e)
    {
        Debug.Log($"Activity received:\r\n {e.Activity} ");

        if (e.HasAudio)
        {
            Debug.Log($"Audio received");
            var audio = e.Audio;

            // for sample purposes, just save the entire file and playback. (can improved with streaming)
            var buffer = new byte[800];
            uint bytesRead = 0;
            while ((bytesRead = audio.Read(buffer)) > 0)
            {
                byteStream.Write(buffer, 0, (int)bytesRead);
            }

            // Write a wav header on the final stream now that we know the full length
            if (byteStream.Length > 0)
            {
                WaveAudioData.WriteWavHeader(finalStream, false, 1, 16, 16000, (int)byteStream.Length);
                byteStream.WriteTo(finalStream);
                audioData = new WaveAudioData(finalStream.ToArray());

                ClearStream(byteStream);
                ClearStream(finalStream);
            }
        }
    }

    private void ClearStream(MemoryStream stream)
    {
        byte[] buffer = stream.GetBuffer();
        Array.Clear(buffer, 0, buffer.Length);
        stream.Position = 0;
        stream.SetLength(0);
    }
}


