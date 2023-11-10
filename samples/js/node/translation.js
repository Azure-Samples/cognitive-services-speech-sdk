// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

// pull in the required packages.
import * as sdk from "microsoft-cognitiveservices-speech-sdk";

export const main = (settings) => {

  // now create the audio-config pointing to our stream and
  // the speech config specifying the language.
  var audioStream = filePushStream.openPushStream(settings.filename);
  var audioConfig = sdk.AudioConfig.fromStreamInput(audioStream);
  var translationConfig = sdk.SpeechTranslationConfig.fromSubscription(settings.subscriptionKey, settings.serviceRegion);

  // setting the recognition language to English.
  translationConfig.speechRecognitionLanguage = settings.language;

  // target language is German.
  translationConfig.addTargetLanguage("de-DE");

  // create the translation recognizer.
  var recognizer = new sdk.TranslationRecognizer(translationConfig, audioConfig);

  // Before beginning speech recognition, setup the callbacks to be invoked when an event occurs.

  // The event recognizing signals that an intermediate recognition result is received.
  // You will receive one or more recognizing events as a speech phrase is recognized, with each containing
  // more recognized speech. The event will contain the text for the recognition since the last phrase was recognized.
  // Both the source language text and the translation text(s) are available.
  recognizer.recognizing = function (s, e) {
      var str = ("(recognizing) Reason: " + sdk.ResultReason[e.result.reason] + " Text: " + e.result.text + " Translations:");

      var language = "de";
      str += " [" + language + "] " + e.result.translations.get(language);

      console.log(str);
  };

  // The event recognized signals that a final recognition result is received.
  // This is the final event that a phrase has been recognized.
  // For continuous recognition, you will get one recognized event for each phrase recognized.
  // Both the source language text and the translation text(s) are available.
  recognizer.recognized = function (s, e) {
      var str = "\r\n(recognized)  Reason: " + sdk.ResultReason[e.result.reason] + " Text: " + e.result.text + " Translations:";

      var language = "de";
      str += " [" + language + "] " + e.result.translations.get(language);
      str += "\r\n";

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

