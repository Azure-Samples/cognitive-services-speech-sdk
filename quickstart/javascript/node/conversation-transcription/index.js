// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

// pull in the required packages.
import * as sdk from "microsoft-cognitiveservices-speech-sdk";
import * as fs from "fs";

// replace with your own subscription key,
// service region (e.g., "centralus"), and
// the name of the file you want to transcribe
// through the conversation transcriber.
var subscriptionKey = "YourSubscriptionKey";
var serviceRegion = "YourServiceRegion"; // e.g., "centralus"
var filename = "YourAudioFile.wav";

// create the push stream we need for the speech sdk.
var pushStream = sdk.AudioInputStream.createPushStream();

// open the file and push it to the push stream.
fs.createReadStream(filename).on('data', function(arrayBuffer) {
pushStream.write(arrayBuffer.slice());
}).on('end', function() {
pushStream.close();
});

// we are done with the setup
console.log("Transcribing from: " + filename);
// now create the audio-config pointing to our stream and
// the speech config specifying the language.
var speechConfig = sdk.SpeechConfig.fromSubscription(subscriptionKey, serviceRegion);
var audioConfig = sdk.AudioConfig.fromStreamInput(pushStream);

// create the conversation transcriber.
var transcriber = new sdk.ConversationTranscriber(speechConfig, audioConfig);

transcriber.sessionStarted = function(s, e) {
    console.log("(sessionStarted) SessionId:" + e.sessionId);
};
transcriber.sessionStopped = function(s, e) {
    console.log("(sessionStopped) SessionId:" + e.sessionId);
};
transcriber.canceled = function(s, e) {
    console.log("(canceled) " + e.errorDetails);
};
transcriber.transcribed = function(s, e) {
    console.log("(transcribed) text: " + e.result.text);
    console.log("(transcribed) speakerId: " + e.result.speakerId);
};

// Begin conversation transcription
transcriber.startTranscribingAsync(
    function () {},
    function (err) {
        console.trace("err - starting transcription: " + err);
    }
);
