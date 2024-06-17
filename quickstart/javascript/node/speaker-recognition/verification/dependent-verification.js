// pull in the required packages.
import * as sdk from "microsoft-cognitiveservices-speech-sdk";
import * as fs from "fs";

// replace with your own subscription key,
// service region (e.g., "westus"), and
// the name of the files you want to use
// to enroll and then verify the speaker.
// Note that three different samples are 
// necessary to enroll for verification.
const subscriptionKey = "YourSubscriptionKey";
const serviceRegion = "YourSubscriptionRegion"; // e.g., "westus"
const enrollFiles = ["myVoiceIsMyPassportVerifyMe01.wav","myVoiceIsMyPassportVerifyMe02.wav","myVoiceIsMyPassportVerifyMe03.wav"]; // 16000 Hz, Mono
const verificationFile = "myVoiceIsMyPassportVerifyMe04.wav"; // 16000 Hz, Mono

// now create the speech config with the credentials for the subscription
const speechConfig = sdk.SpeechConfig.fromSubscription(subscriptionKey, serviceRegion);
const client = new sdk.VoiceProfileClient(speechConfig);
const locale = "en-us";

const getAudioConfigFromFile = function (file) {
  return sdk.AudioConfig.fromWavFileInput(fs.readFileSync(file));
};

try {
  // we are done with the setup, so create a dependent verification specific profile
  const profile = await client.createProfileAsync(sdk.VoiceProfileType.TextDependentVerification, locale);

  console.log("Profile id: " + profile.profileId +" created, now enrolling using files beginning with: " + enrollFiles[0]);
  // create audio configs for each of the enrollment files to use for each of the enrollment steps
  for (const enrollFile of enrollFiles) {
    const enrollConfig = getAudioConfigFromFile(enrollFile);
    const enrollResult = await client.enrollProfileAsync(profile, enrollConfig);
    console.log("(Enrollment result) Reason: " + sdk.ResultReason[enrollResult.reason]); 
  }
  const verificationConfig = getAudioConfigFromFile(verificationFile);
  const recognizer = new sdk.SpeakerRecognizer(speechConfig, verificationConfig);
  
  // For verification scenarios, create a SpeakerVerificationModel. (Note that identification scenarios use a different type and API here.)
  const model = sdk.SpeakerVerificationModel.fromProfile(profile);
  const verificationResult = await recognizer.recognizeOnceAsync(model);
  const reason = verificationResult.reason; 
  console.log("(Verification result) Reason: " + sdk.ResultReason[reason]); 
  if( reason === sdk.ResultReason.Canceled ) {
    const cancellationDetails = sdk.SpeakerRecognitionCancellationDetails.fromResult(verificationResult);
    console.log("(Verification canceled) Error Details: " + cancellationDetails.errorDetails); 
    console.log("(Verification canceled) Error Code: " + cancellationDetails.errorCode);
  } else {
    console.log("(Verification result) Profile Id: " + verificationResult.profileId); 
    console.log("(Verification result) Score: " + verificationResult.score);
  }

  // Delete voice profile after we're done with this scenario 
  const deleteResult = await client.deleteProfileAsync(profile);
  console.log("(Delete profile result) Reason: " + sdk.ResultReason[deleteResult.reason]); 
} catch (err) {
  console.log("ERROR: " + err); 
}
