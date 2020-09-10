(function() {
  "use strict";
  
  // pull in the required packages.
  var sdk = require("microsoft-cognitiveservices-speech-sdk");
  var fs = require("fs");

  // replace with your own subscription key,
  // service region (e.g., "westus"), and
  // the name of the files you want to use
  // to enroll and then verify the speaker.
  // Note that three different samples are 
  // necessary to enroll for verification.
  let subscriptionKey = "YourSubscriptionKey";
  let serviceRegion = "YourSubscriptionRegion"; // e.g., "westus"
  let enrollFiles = ["myVoiceIsMyPassportVerifyMe01.wav","myVoiceIsMyPassportVerifyMe02.wav","myVoiceIsMyPassportVerifyMe03.wav"]; // 16000 Hz, Mono
  let verificationFile = "myVoiceIsMyPassportVerifyMe04.wav"; // 16000 Hz, Mono
  
  // now create the speech config with the credentials for the subscription
  let speechConfig = sdk.SpeechConfig.fromSubscription(subscriptionKey, serviceRegion);
  let client = new sdk.VoiceProfileClient(speechConfig);
  let locale = "en-us";

  // we are done with the setup
  client.createProfileAsync(
    sdk.VoiceProfileType.TextDependentVerification,
    locale,
    function (result) {
      let getAudioConfigFromFile = function (file) {
          // Create the push stream we need for the speech sdk.
          let pushStream = sdk.AudioInputStream.createPushStream();

          // Open the file and push it to the push stream.
          fs.createReadStream(file).on("data", function(arrayBuffer) {
            pushStream.write(arrayBuffer.buffer);
          }).on("end", function() {
            pushStream.close();
          });
          return sdk.AudioConfig.fromStreamInput(pushStream);
      };
      let profile = result;
      let enrollConfigs = [];
      enrollFiles.forEach(f => {
        enrollConfigs.push(getAudioConfigFromFile(f));
      });

      console.log("Profile id: " + profile.profileId +" created, now enrolling using files beginning with: " + enrollFiles[0]);

      client.enrollProfileAsync(
        profile, 
        enrollConfigs[0],
        function(enrollResult) {
          console.log("(Enrollment result) Reason: " + sdk.ResultReason[enrollResult.reason]); 
          client.enrollProfileAsync(
            profile, 
            enrollConfigs[1],
            function(enrollResult) {
              console.log("(Enrollment result) Reason: " + sdk.ResultReason[enrollResult.reason]); 
              client.enrollProfileAsync(
                profile, 
                enrollConfigs[2],
                function(enrollResult) {
                  console.log("(Enrollment result) Reason: " + sdk.ResultReason[enrollResult.reason]); 
                  let verificationConfig = getAudioConfigFromFile(verificationFile);
                  let recognizer = new sdk.SpeakerRecognizer(speechConfig, verificationConfig);
                  let model = sdk.SpeakerVerificationModel.fromProfile(profile);
                  recognizer.recognizeOnceAsync(
                    model,
                    function(verificationResult) {
                      let reason = verificationResult.reason; 
                      console.log("(Verification result) Reason: " + sdk.ResultReason[reason]); 
                      if( reason === sdk.ResultReason.Canceled ) {
                        let cancellationDetails = sdk.SpeakerRecognitionCancellationDetails.fromResult(verificationResult);
                        console.log("(Verification canceled) Error Details: " + cancellationDetails.errorDetails); 
                        console.log("(Verification canceled) Error Code: " + cancellationDetails.errorCode);
                      } else {
                        console.log("(Verification result) Profile Id: " + verificationResult.profileId); 
                        console.log("(Verification result) Score: " + verificationResult.score);
                      }
                      client.deleteProfileAsync(
                        profile,
                        function(deleteResult) {
                          console.log("(Delete profile result) Reason: " + sdk.ResultReason[deleteResult.reason]); 
                        },
                        function(err) {
                          console.log("Delete Profile ERROR: " + err); 
                        });
                    },
                    function(err) {
                      console.log("Recognize Once ERROR: " + err); 
                    });
                },
                function(err) {
                  console.log("Third Enrollment file ERROR: " + err); 
                });
            },
            function(err) {
              console.log("Second Enrollment file ERROR: " + err); 
            });
        },
        function(err) {
          console.log("First Enrollment file ERROR: " + err); 
        });
    },
    function (err) {
      console.log("Create Profile ERROR: " + err); 
    });
  
}());

