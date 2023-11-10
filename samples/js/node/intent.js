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
    var intentConfig = sdk.SpeechConfig.fromSubscription(settings.luSubscriptionKey, settings.luServiceRegion);

    // setting the recognition language to English.
    intentConfig.speechRecognitionLanguage = settings.language;

    // create the translation recognizer.
    var recognizer = new sdk.IntentRecognizer(intentConfig, audioConfig);

    // Set up a Language Understanding Model from Language Understanding Intelligent Service (LUIS).
    // See https://www.luis.ai/home for more information on LUIS.
    if (settings.luAppId !== "" && settings.luAppId !== "YourLanguageUnderstandingAppId") {
        var lm = sdk.LanguageUnderstandingModel.fromAppId(settings.luAppId);

        recognizer.addAllIntents(lm);
    }

    // Before beginning speech recognition, setup the callbacks to be invoked when an event occurs.

    // The event recognizing signals that an intermediate recognition result is received.
    // You will receive one or more recognizing events as a speech phrase is recognized, with each containing
    // more recognized speech. The event will contain the text for the recognition since the last phrase was recognized.
    recognizer.recognizing = function (s, e) {
        var str = "(recognizing) Reason: " + sdk.ResultReason[e.result.reason] + " Text: " + e.result.text;
        console.log(str);
    };

    // The event signals that the service has stopped processing speech.
    // https://docs.microsoft.com/javascript/api/microsoft-cognitiveservices-speech-sdk/speechrecognitioncanceledeventargs?view=azure-node-latest
    // This can happen for two broad classes or reasons.
    // 1. An error is encountered.
    //    In this case the .errorDetails property will contain a textual representation of the error.
    // 2. Speech was detected to have ended.
    //    This can be caused by the end of the specified file being reached, or ~20 seconds of silence from a microphone input.
    recognizer.canceled = function (s, e) {
        var str = "(cancel) Reason: " + sdk.CancellationReason[e.reason];
        if (e.reason === sdk.CancellationReason.Error) {
            str += ": " + e.errorDetails;
        }
        console.log(str);
    };

    // The event recognized signals that a final recognition result is received.
    // This is the final event that a phrase has been recognized.
    // For continuous recognition, you will get one recognized event for each phrase recognized.
    recognizer.recognized = function (s, e) {
        var str = "(recognized)  Reason: " + sdk.ResultReason[e.result.reason];

        // Depending on what result reason is returned, different properties will be populated.
        switch (e.result.reason) {
            // This case occurs when speech was successfully recognized, but the speech did not match an intent from the Language Understanding Model.
            case sdk.ResultReason.RecognizedSpeech:
                str += " Text: " + e.result.text;
                break;

            // Both speech an intent from the model was recognized.
            case sdk.ResultReason.RecognizedIntent:
                str += " Text: " + e.result.text + " IntentId: " + e.result.intentId;

                // The actual JSON returned from Language Understanding is a bit more complex to get to, but it is available for things like
                // the entity name and type if part of the intent.
                str += " Intent JSON: " + e.result.properties.getProperty(sdk.PropertyId.LanguageUnderstandingServiceResponse_JsonResult);
                break;

            // No match was found.
            case sdk.ResultReason.NoMatch:
                var noMatchDetail = sdk.NoMatchDetails.fromResult(e.result);
                str += " NoMatchReason: " + sdk.NoMatchReason[noMatchDetail.reason];
                break;
        }
        console.log(str);
    };

    // Signals that a new session has started with the speech service
    recognizer.sessionStarted = function (s, e) {
        var str = "(sessionStarted) SessionId: " + e.sessionId;
        console.log(str);
    };

    // Signals the end of a session with the speech service.
    recognizer.sessionStopped = function (s, e) {
        str = "(sessionStopped) SessionId: " + e.sessionId;
        console.log(str);
    };

    // Signals that the speech service has started to detect speech.
    recognizer.speechStartDetected = function (s, e) {
        var str = "(speechStartDetected) SessionId: " + e.sessionId;
        console.log(str);
    };

    // Signals that the speech service has detected that speech has stopped.
    recognizer.speechEndDetected = function (s, e) {
        var str = "(speechEndDetected) SessionId: " + e.sessionId;
        console.log(str);
    };
    
    // start the recognizer and wait for a result.
    recognizer.recognizeOnceAsync(
        function (result) {
            recognizer.close();
            recognizer = undefined;
        },
        function (err) {
            recognizer.close();
            recognizer = undefined;
        });
};
