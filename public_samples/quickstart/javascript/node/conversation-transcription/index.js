// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

(function() {
  "use strict";
  
  // pull in the required packages.
  var sdk = require("microsoft-cognitiveservices-speech-sdk");
  var fs = require("fs");
  
  // replace with your own subscription key,
  // service region (e.g., "centralus"), and
  // the name of the file you want to transcribe
  // through the conversation transcriber.
  var subscriptionKey = "YourSubscriptionKey";
  var serviceRegion = "YourServiceRegion"; // e.g., "centralus"
  var filename = "YourAudioFile.wav"; // 8-channel audio
  
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
  var speechTranslationConfig = sdk.SpeechTranslationConfig.fromSubscription(subscriptionKey, serviceRegion);
  var audioConfig = sdk.AudioConfig.fromStreamInput(pushStream);

  // setting the recognition language to English.
  speechTranslationConfig.speechRecognitionLanguage = "en-US";

  // create the conversation object tracking participants
  var conversation = sdk.Conversation.createConversationAsync(speechTranslationConfig, "myConversation");

  // create the speech recognizer.
  var transcriber = new sdk.ConversationTranscriber(audioConfig);

  // attach the transcriber to the conversation
  transcriber.joinConversationAsync(conversation,
    function () {
      // add first participant with voice signature from enrollment step
      var voiceSignatureUser1 = "{" +
        "Version: 0," +
        "Tag: \"<<VOICE_TAG_HERE>>\"" +
        "Data: \"<<VOICE_DATA_HERE>>\"" +
       "}";
      var user1 = sdk.Participant.From("user1@example.com", "en-us", voiceSignatureUser1);
      conversation.addParticipantAsync(user1,
        function () {
          // add second participant with voice signature from enrollment step
          var voiceSignatureUser2 = "{" +
            "Version: 0," +
            "Tag: \"<<VOICE_TAG_HERE>>\"," +
            "Data: \"<<VOICE_DATA_HERE>>\"" +
           "}";
          var user2 = sdk.Participant.From("user2@example.com", "en-us", voiceSignatureUser2);
          conversation.addParticipantAsync(user2,
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

              // Begin conversation transcription
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

}()); 
