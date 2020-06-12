//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// <code>
package speechsdk.quickstart;

import com.microsoft.cognitiveservices.speech.*;
import com.microsoft.cognitiveservices.speech.intent.*;

/**
 * Quickstart: recognize speech using the Speech SDK for Java.
 */
public class Main {

    /**
     * @param args Arguments are ignored in this sample.
     */
    public static void main(String[] args) {
        try {

            // Creates an instance of a speech config with specified
            // subscription key (called 'endpoint key' by the Language Understanding service)
            // and service region. Replace with your own subscription (endpoint) key
            // and service region (e.g., "westus2").
            // The default language is "en-us".
            SpeechConfig config = SpeechConfig.fromSubscription(
                "YourLanguageUnderstandingSubscriptionKey",
                "YourLanguageUnderstandingServiceRegion");

            // Creates an intent recognizer using microphone as audio input.
            IntentRecognizer recognizer = new IntentRecognizer(config);

            // Creates a language understanding model using the app id, and adds specific intents from your model
            LanguageUnderstandingModel model = LanguageUnderstandingModel.fromAppId("YourLanguageUnderstandingAppId");
            recognizer.addIntent(model, "YourLanguageUnderstandingIntentName1", "id1");
            recognizer.addIntent(model, "YourLanguageUnderstandingIntentName2", "id2");
            recognizer.addIntent(model, "YourLanguageUnderstandingIntentName3", "any-IntentId-here");

            // To add all of the possible intents from a LUIS model to the recognizer, uncomment the line below:
            // recognizer.addAllIntents(model);

            System.out.println("Say something...");

            // Starts recognition. It returns when the first utterance has been recognized.
            IntentRecognitionResult result = recognizer.recognizeOnceAsync().get();

            // Checks result.
            if (result.getReason() == ResultReason.RecognizedIntent) {
                System.out.println("RECOGNIZED: Text=" + result.getText());
                System.out.println("    Intent Id: " + result.getIntentId());
                System.out.println("    Intent Service JSON: " + result.getProperties().getProperty(PropertyId.LanguageUnderstandingServiceResponse_JsonResult));
            }
            else if (result.getReason() == ResultReason.RecognizedSpeech) {
                System.out.println("RECOGNIZED: Text=" + result.getText());
                System.out.println("    Intent not recognized.");
            }
            else if (result.getReason() == ResultReason.NoMatch) {
                System.out.println("NOMATCH: Speech could not be recognized.");
            }
            else if (result.getReason() == ResultReason.Canceled) {
                CancellationDetails cancellation = CancellationDetails.fromResult(result);
                System.out.println("CANCELED: Reason=" + cancellation.getReason());

                if (cancellation.getReason() == CancellationReason.Error) {
                    System.out.println("CANCELED: ErrorCode=" + cancellation.getErrorCode());
                    System.out.println("CANCELED: ErrorDetails=" + cancellation.getErrorDetails());
                    System.out.println("CANCELED: Did you update the subscription info?");
                }
            }
            
            result.close();
            recognizer.close();
        } catch (Exception ex) {
            System.out.println("Unexpected exception: " + ex.getMessage());

            assert(false);
            System.exit(1);
        } 
    }
}
// </code>
