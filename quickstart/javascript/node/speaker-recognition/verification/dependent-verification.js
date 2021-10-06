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
  var subscriptionKey = "YourSubscriptionKey";
  var serviceRegion = "YourSubscriptionRegion"; // e.g., "westus"
  var enrollFiles = ["myVoiceIsMyPassportVerifyMe01.wav","myVoiceIsMyPassportVerifyMe02.wav","myVoiceIsMyPassportVerifyMe03.wav"]; // 16000 Hz, Mono
  var verificationFile = "myVoiceIsMyPassportVerifyMe04.wav"; // 16000 Hz, Mono
  
  // now create the speech config with the credentials for the subscription
  var speechConfig = sdk.SpeechConfig.fromSubscription(subscriptionKey, serviceRegion);
  var client = new sdk.VoiceProfileClient(speechConfig);
  var locale = "en-us";

  // we are done with the setup, so create a dependent verification specific profile
  client.createProfileAsync(
    sdk.VoiceProfileType.TextDependentVerification,
    locale,
    async function (result) {
      var getAudioConfigFromFile = function (file) {
          return sdk.AudioConfig.fromWavFileInput(fs.readFileSync(file));
      };
      var profile = result;
      var resultReason: sdk.ResultReason = sdk.ResultReason.EnrollingVoiceProfile;
      
      console.log("Profile id: " + profile.profileId +" created, now enrolling using files beginning with: " + enrollFiles[0]);
      // create audio configs for each of the enrollment files to use for each of the enrollment steps
      for (const enrollFile of enrollFiles) {
        var enrollConfig = getAudioConfigFromFile(enrollFile);
        try {
          var enrollResult = await client.enrollProfileAsync(profile, enrollConfig);
          console.log("(Enrollment result) Reason: " + sdk.ResultReason[enrollResult.reason]); 
          resultReason = enrollResult.reason;
        } catch(err) {
          console.log(`Enrollment file ${f} ERROR: ${err}`); 
          break;
        }
      }

      if (resultReason === sdk.ResultReason.EnrolledVoiceProfile) {
        var verificationConfig = getAudioConfigFromFile(verificationFile);
        var recognizer = new sdk.SpeakerRecognizer(speechConfig, verificationConfig);
                  
        // For verification scenarios, create a SpeakerVerificationModel. (Note that identification scenarios use a different type and API here.)
        var model = sdk.SpeakerVerificationModel.fromProfile(profile);
        recognizer.recognizeOnceAsync(
          model,
          function(verificationResult) {
            var reason = verificationResult.reason; 
            console.log("(Verification result) Reason: " + sdk.ResultReason[reason]); 
            if( reason === sdk.ResultReason.Canceled ) {
              var cancellationDetails = sdk.SpeakerRecognitionCancellationDetails.fromResult(verificationResult);
              console.log("(Verification canceled) Error Details: " + cancellationDetails.errorDetails); 
              console.log("(Verification canceled) Error Code: " + cancellationDetails.errorCode);
            } else {
              console.log("(Verification result) Profile Id: " + verificationResult.profileId); 
              console.log("(Verification result) Score: " + verificationResult.score);
            }
                      
            // Delete voice profile after we're done with this scenario 
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
      }
    },
    function (err) {
      console.log("Create Profile ERROR: " + err); 
    });
  
}());
