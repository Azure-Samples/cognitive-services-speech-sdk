// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

// <code>

// pull in the required packages.
import * as sdk from "microsoft-cognitiveservices-speech-sdk";
import * as readline from "readline";

// replace with your own subscription key,
// service region (e.g., "westus"), and
// the name of the file you save the synthesized audio.
var subscriptionKey = "YourSubscriptionKey";
var serviceRegion = "YourServiceRegion"; // e.g., "westus"
var filename = "YourAudioFile.wav";

// we are done with the setup

// now create the audio-config pointing to our stream and
// the speech config specifying the language.
var audioConfig = sdk.AudioConfig.fromAudioFileOutput(filename);
var speechConfig = sdk.SpeechConfig.fromSubscription(subscriptionKey, serviceRegion);

// create the speech synthesizer.
var synthesizer = new sdk.SpeechSynthesizer(speechConfig, audioConfig);

var rl = readline.createInterface({
  input: process.stdin,
  output: process.stdout
});

rl.question("Type some text that you want to speak...\n> ", function (text) {
  rl.close();
  // start the synthesizer and wait for a result.
  synthesizer.speakTextAsync(text,
      function (result) {
    if (result.reason === sdk.ResultReason.SynthesizingAudioCompleted) {
      console.log("synthesis finished.");
    } else {
      console.error("Speech synthesis canceled, " + result.errorDetails +
          "\nDid you update the subscription info?");
    }
    synthesizer.close();
    synthesizer = undefined;
  },
      function (err) {
    console.trace("err - " + err);
    synthesizer.close();
    synthesizer = undefined;
  });
  console.log("Now synthesizing to: " + filename);
});
// </code>
