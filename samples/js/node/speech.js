// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

// pull in the required packages.
import * as sdk from "microsoft-cognitiveservices-speech-sdk";
import * as filePushStream from "./filePushStream.js";

export const main = (settings) => {

  // now create the audio-config pointing to our stream and
  // the speech config specifying the language.

  var audioStream = filePushStream.openPushStream(settings.filename);
  var audioConfig = sdk.AudioConfig.fromStreamInput(audioStream);
  var speechConfig = sdk.SpeechConfig.fromSubscription(settings.subscriptionKey, settings.serviceRegion);

  // setting the recognition language to English.
  speechConfig.speechRecognitionLanguage = settings.language;

  // create the speech recognizer.
  var reco = new sdk.SpeechRecognizer(speechConfig, audioConfig);

  // Before beginning speech recognition, setup the callbacks to be invoked when an event occurs.

  // The event recognizing signals that an intermediate recognition result is received.
  // You will receive one or more recognizing events as a speech phrase is recognized, with each containing
  // more recognized speech. The event will contain the text for the recognition since the last phrase was recognized.
  reco.recognizing = function (s, e) {
    var str = "(recognizing) Reason: " + sdk.ResultReason[e.result.reason] + " Text: " + e.result.text;
    console.log(str);
  };

// The event recognized signals that a final recognition result is received.
// This is the final event that a phrase has been recognized.
// For continuous recognition, you will get one recognized event for each phrase recognized.
reco.recognized = function (s, e) {
    // Indicates that recognizable speech was not detected, and that recognition is done.
    if (e.result.reason === sdk.ResultReason.NoMatch) {
        var noMatchDetail = sdk.NoMatchDetails.fromResult(e.result);
        console.log("\r\n(recognized)  Reason: " + sdk.ResultReason[e.result.reason] + " NoMatchReason: " + sdk.NoMatchReason[noMatchDetail.reason]);
    } else {
      console.log("\r\n(recognized)  Reason: " + sdk.ResultReason[e.result.reason] + " Text: " + e.result.text);
    }
};

// The event signals that the service has stopped processing speech.
// https://docs.microsoft.com/javascript/api/microsoft-cognitiveservices-speech-sdk/speechrecognitioncanceledeventargs?view=azure-node-latest
// This can happen for two broad classes of reasons.
// 1. An error is encountered.
//    In this case the .errorDetails property will contain a textual representation of the error.
// 2. Speech was detected to have ended.
//    This can be caused by the end of the specified file being reached, or ~20 seconds of silence from a microphone input.
reco.canceled = function (s, e) {
    var str = "(cancel) Reason: " + sdk.CancellationReason[e.reason];
    if (e.reason === sdk.CancellationReason.Error) {
        str += ": " + e.errorDetails;
    }
    console.log(str);
  };

// Signals that a new session has started with the speech service
reco.sessionStarted = function (s, e) {
    var str = "(sessionStarted) SessionId: " + e.sessionId;
    console.log(str);
};

// Signals the end of a session with the speech service.
reco.sessionStopped = function (s, e) {
    var str = "(sessionStopped) SessionId: " + e.sessionId;
    console.log(str);
};

// Signals that the speech service has started to detect speech.
reco.speechStartDetected = function (s, e) {
    var str = "(speechStartDetected) SessionId: " + e.sessionId;
    console.log(str);
};

// Signals that the speech service has detected that speech has stopped.
reco.speechEndDetected = function (s, e) {
    var str = "(speechEndDetected) SessionId: " + e.sessionId;
    console.log(str);
};

  // start the recognizer and wait for a result.
  reco.recognizeOnceAsync(
    function (result) {
      reco.close();
      reco = undefined;
    },
    function (err) {
      reco.close();
      reco = undefined;
    });
};

