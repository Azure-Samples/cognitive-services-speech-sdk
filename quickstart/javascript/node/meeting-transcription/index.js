// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

  // pull in the required packages.
import * as sdk from "microsoft-cognitiveservices-speech-sdk";
import * as fs from "fs";

// replace with your own subscription key,
// service region (e.g., "centralus"), and
// the name of the file you want to transcribe
// through the meeting transcriber.
var subscriptionKey = "YourSubscriptionKey";
var serviceRegion = "YourServiceRegion"; // e.g., "centralus"
var filename = "YourAudioFile.wav"; // 8-channel audio

// create the push stream we need for the speech sdk.
var pushStream = sdk.AudioInputStream.createPushStream(sdk.AudioStreamFormat.getWaveFormatPCM(16000, 16, 8))

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
var speechTranslationConfig = sdk.SpeechTranslationConfig.fromSubscription(subscriptionKey, serviceRegion);
var audioConfig = sdk.AudioConfig.fromStreamInput(pushStream);

// setting the recognition language to English.
speechTranslationConfig.speechRecognitionLanguage = "en-US";

// create the meeting object tracking participants
var meeting = sdk.Meeting.createMeetingAsync(speechTranslationConfig, "myMeeting");

// create the meeting transcriber.
var transcriber = new sdk.MeetingTranscriber(audioConfig);

// attach the transcriber to the meeting
transcriber.joinMeetingAsync(meeting,
  function () {
    // add first participant with voice signature from enrollment step
    var voiceSignatureUser1 = "{" +
      "Version: 0," +
      "Tag: \"<<VOICE_TAG_HERE>>\"," +
      "Data: \"<<VOICE_DATA_HERE>>\"" +
      "}";
    var user1 = sdk.Participant.From("user1@example.com", "en-us", voiceSignatureUser1);
    meeting.addParticipantAsync(user1,
      function () {
        // add second participant with voice signature from enrollment step
        var voiceSignatureUser2 = "{" +
          "Version: 0," +
          "Tag: \"<<VOICE_TAG_HERE>>\"," +
          "Data: \"<<VOICE_DATA_HERE>>\"" +
          "}";
        var user2 = sdk.Participant.From("user2@example.com", "en-us", voiceSignatureUser2);
        meeting.addParticipantAsync(user2,
          function () {
            transcriber.sessionStarted = function(s, e) {
              console.log("(sessionStarted)");
            };
            transcriber.sessionStopped = function(s, e) {
              console.log("(sessionStopped)");
            };
            transcriber.canceled = function(s, e) {
              console.log("(canceled)");
            };
            transcriber.transcribed = function(s, e) {
              console.log("(transcribed) text: " + e.result.text);
              console.log("(transcribed) speakerId: " + e.result.speakerId);
            };

            // Begin meeting transcription
            transcriber.startTranscribingAsync(
              function () { },
              function (err) {
                console.trace("err - starting transcription: " + err);
              });
    },
    function (err) {
        console.trace("err - adding user1: " + err);
    });
  },
  function (err) {
      console.trace("err - adding user2: " + err);
  });
},
function (err) {
  console.trace("err - " + err);
});
