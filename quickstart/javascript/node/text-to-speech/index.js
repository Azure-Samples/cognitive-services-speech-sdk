//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

(function() {

  "use strict";

  var sdk = require("microsoft-cognitiveservices-speech-sdk");
  var readline = require("readline");

  var key = "YourSubscriptionKey";
  var region = "YourServiceRegion";
  var audioFile = "YourAudioFile.wav";

  const speechConfig = sdk.SpeechConfig.fromSubscription(key, region);
  const audioConfig = sdk.AudioConfig.fromAudioFileOutput(audioFile);

  // The language of the voice that speaks.
  speechConfig.speechSynthesisVoiceName = "en-US-JennyNeural"; 

  // Create the speech synthesizer.
  var synthesizer = new sdk.SpeechSynthesizer(speechConfig, audioConfig);

  var rl = readline.createInterface({
    input: process.stdin,
    output: process.stdout
  });

  rl.question("Enter some text that you want to speak >\n> ", function (text) {
    rl.close();
    // Start the synthesizer and wait for a result.
    synthesizer.speakTextAsync(text,
        function (result) {
      if (result.reason === sdk.ResultReason.SynthesizingAudioCompleted) {
        console.log("synthesis finished.");
      } else {
        console.error("Speech synthesis canceled, " + result.errorDetails +
            "\nDid you set the speech resource key and region values?");
      }
      synthesizer.close();
      synthesizer = null;
    },
        function (err) {
      console.trace("err - " + err);
      synthesizer.close();
      synthesizer = null;
    });
    console.log("Now synthesizing to: " + audioFile);
  });
}());
  