// pull in the required packages.
import * as sdk from "microsoft-cognitiveservices-speech-sdk";
import * as fs from "fs";

// replace with your own subscription key,
// service region (e.g., "westus"), and
// the name of the files you want to use
// to enroll and then identify the speaker.
const subscriptionKey = "YourSubscriptionKey";
const serviceRegion = "YourSubscriptionRegion"; // e.g., "westus"
const enrollFile = "aboutSpeechSdk.wav"; // 16000 Hz, Mono
const identificationFile = "TalkForAFewSeconds16.wav"; // 16000 Hz, Mono

// now create the speech config with the credentials for the subscription
const speechConfig = sdk.SpeechConfig.fromSubscription(subscriptionKey, serviceRegion);
const client = new sdk.VoiceProfileClient(speechConfig);
const locale = "en-us";

// we are done with the setup
try {
  const profile = await client.createProfileAsync(sdk.VoiceProfileType.TextIndependentIdentification, locale);
  const audioConfig = sdk.AudioConfig.fromWavFileInput(fs.readFileSync(enrollFile));

  console.log("Profile id: " + profile.profileId +" created, now enrolling using file: " + enrollFile);
  const enrollResult = await client.enrollProfileAsync(profile, audioConfig);

  console.log("(Enrollment result) Reason: " + sdk.ResultReason[enrollResult.reason]); 
  const idConfig = sdk.AudioConfig.fromWavFileInput(fs.readFileSync(identificationFile));
  const recognizer = new sdk.SpeakerRecognizer(speechConfig, idConfig);
  const model = sdk.SpeakerIdentificationModel.fromProfiles([profile]);
  const identificationResult = await recognizer.recognizeOnceAsync(model);

  var reason = identificationResult.reason; 
  console.log("(Identification result) Reason: " + sdk.ResultReason[reason]); 

  if( reason === sdk.ResultReason.Canceled ) {
    const cancellationDetails = sdk.SpeakerRecognitionCancellationDetails.fromResult(identificationResult);
    console.log("(Identification canceled) Error Details: " + cancellationDetails.errorDetails); 
    console.log("(Identification canceled) Error Code: " + cancellationDetails.errorCode);
  } else {
    console.log("(Identification result) Profile Id: " + identificationResult.profileId); 
    console.log("(Identification result) Score: " + identificationResult.score);
  }

  const deleteResult = await client.deleteProfileAsync(profile);
  console.log("(Delete profile result) Reason: " + sdk.ResultReason[deleteResult.reason]); 

} catch (err) {
  console.log("ERROR during operation: " + err); 
}
