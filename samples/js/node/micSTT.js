// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

import * as MicToSpeech from "mic-to-speech";
import * as sdk from "microsoft-cognitiveservices-speech-sdk";
import * as wav from "wav";
import * as settings from "./settings.js";

const API_KEY = settings.subscriptionKey;
const region = settings.serviceRegion;

const speechConfig = sdk.SpeechConfig.fromSubscription(
  API_KEY,
  region
);
speechConfig.speechRecognitionLanguage = "en-US";

function recognizeFromMic(buffer) {
  /** Writing WAV Headers into the buffer received. */
  let writer = new wav.Writer({
    sampleRate: 32000,
    channels: 1,
    bitDepth: 16,
  });
  writer.write(buffer);
  writer.end();
  writer.on("finish", ()=>{
    /** On finish, read the WAV stream using configuration the SDK provides. */
    let audioConfig = sdk.AudioConfig.fromWavFileInput(writer.read());
    let recognizer = new sdk.SpeechRecognizer(speechConfig, audioConfig);
    recognizer.recognizeOnceAsync((result) => {
      console.log(result);
      console.log(`RECOGNIZED: Text=${result.text}`);
      recognizer.close();
    });
  })
}

/** Configuration for the mic */
let micToSpeech = new MicToSpeech({
  channels: 1,
});

/** Receive the audio buffer from mic */
micToSpeech.on("speech", function (buffer) {
  console.log("buffer of speech received");
  recognizeFromMic(buffer);
});

/** Start listening to speech. */
micToSpeech.start();
console.log("Listening for speech");
