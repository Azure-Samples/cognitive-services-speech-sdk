// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

  // <code>
  
  // pull in the required packages.
  import * as sdk from "microsoft-cognitiveservices-speech-sdk";
  import * as fs from "fs";
  
  // replace with your own subscription key,
  // service region (e.g., "westus"), and
  // the name of the file you want to run
  // through the speech recognizer.
  var subscriptionKey = "YourSubscriptionKey";
  var serviceRegion = "YourServiceRegion"; // e.g., "westus"
  var filename = "YourAudioFile.wav"; // 16000 Hz, Mono
  
  // create the push stream we need for the speech sdk.
  var pushStream = sdk.AudioInputStream.createPushStream();
  
  // open the file and push it to the push stream.
  fs.createReadStream(filename).on('data', function(arrayBuffer) {
    pushStream.write(arrayBuffer.slice());
  }).on('end', function() {
    pushStream.close();
  });
  
  // we are done with the setup
  console.log("Now recognizing from: " + filename);
  
  // now create the audio-config pointing to our stream and
  // the speech config specifying the language.
  var audioConfig = sdk.AudioConfig.fromStreamInput(pushStream);
  var speechConfig = sdk.SpeechConfig.fromSubscription(subscriptionKey, serviceRegion);
  
  // setting the recognition language to English.
  speechConfig.speechRecognitionLanguage = "en-US";
  
  // create the speech recognizer.
  var recognizer = new sdk.SpeechRecognizer(speechConfig, audioConfig);
  
  // start the recognizer and wait for a result.
  recognizer.recognizeOnceAsync(
    function (result) {
      console.log(result);
  
      recognizer.close();
      recognizer = undefined;
    },
    function (err) {
      console.trace("err - " + err);
  
      recognizer.close();
      recognizer = undefined;
    });
  // </code>
  
  