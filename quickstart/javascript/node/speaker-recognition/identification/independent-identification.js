(function() {
  "use strict";
  
  // pull in the required packages.
  var sdk = require("microsoft-cognitiveservices-speech-sdk");
  var fs = require("fs");

  let getAudioConfigFromFile = (file) => {
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
  
  // replace with your own subscription key,
  // service region (e.g., "westus"), and
  // the name of the files you want to use
  // to enroll and then identify the speaker.
  let subscriptionKey = "YourSubscriptionKey";
  let serviceRegion = "YourSubscriptionRegion"; // e.g., "westus"
  let enrollFile = "aboutSpeechSdk.wav"; // 16000 Hz, Mono
  let identificationFile = "myVoiceIsMyPassportVerifyMe01.wav"; // 16000 Hz, Mono
  
  // now create the speech config with the credentials for the subscription
  let speechConfig = sdk.SpeechConfig.fromSubscription(subscriptionKey, serviceRegion);
  let client = new sdk.VoiceProfileClient(speechConfig);
  let locale = "en-us";

  // we are done with the setup
  client.createProfileAsync(
    sdk.VoiceProfileType.TextIndependentIdentification,
    locale,
    function (result) {
      let profile = result;
      let enrollConfig = getAudioConfigFromFile(enrollFile);

      console.log("Profile id: " + profile.profileId +" created, now enrolling using file: " + enrollFile);

      client.enrollProfileAsync(
        profile, 
        enrollConfig,
        function(enrollResult) {
          console.log("(Enrollment result) Reason: " + sdk.ResultReason[enrollResult.reason]); 
          let identificationConfig = getAudioConfigFromFile(identificationFile);
          let recognizer = new sdk.SpeakerRecognizer(speechConfig, identificationConfig);
          let model = sdk.SpeakerIdentificationModel.fromProfiles([profile]);
          recognizer.recognizeOnceAsync(
            model,
            function(identificationResult) {
              let reason = identificationResult.reason; 
              console.log("(Identification result) Reason: " + sdk.ResultReason[reason]); 

              if( reason === sdk.ResultReason.Canceled ) {
                let cancellationDetails = sdk.SpeakerRecognitionCancellationDetails.fromResult(identificationResult);
                console.log("(Identification canceled) Error Details: " + cancellationDetails.errorDetails); 
                console.log("(Identification canceled) Error Code: " + cancellationDetails.errorCode);
              } else {
                console.log("(Identification result) Profile Id: " + identificationResult.profileId); 
                console.log("(Identification result) Score: " + identificationResult.score);
              }

              client.deleteProfileAsync(
                profile,
                function(deleteResult) {
                  console.log("(Delete profile result) Reason: " + sdk.ResultReason[deleteResult.reason]); 
                },
                function(err) {
                  console.log("ERROR deleting profile: " + err); 
                });
            },
            function(err) {
              console.log("ERROR recognizing speaker: " + err); 
            });
        },
        function(err) {
          console.log("ERROR enrolling profile: " + err); 
        });
    },
    function (err) {
      console.log("ERROR creating profile: " + err); 
    });
  
}());
