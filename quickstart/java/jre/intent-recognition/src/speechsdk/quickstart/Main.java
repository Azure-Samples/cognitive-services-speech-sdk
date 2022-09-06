//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

// <skeleton_1>
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
    // </skeleton_1>
        // <create_speech_configuration>
        // Replace below with with specified subscription key (called 'endpoint key' by the Language Understanding service)
        String languageUnderstandingSubscriptionKey = "YourLanguageUnderstandingSubscriptionKey";
        // Replace below with your own service region (e.g., "westus").
        String languageUnderstandingServiceRegion = "YourLanguageUnderstandingServiceRegion";

        // Creates an instance of intent recognizer with a given speech configuration.
        // Recognizer is created with the default microphone audio input and default language "en-us".
        try (SpeechConfig config = SpeechConfig.fromSubscription(languageUnderstandingSubscriptionKey, languageUnderstandingServiceRegion);
        // </create_speech_configuration>
            // <create_intent_recognizer>
            IntentRecognizer recognizer = new IntentRecognizer(config)) {
            // </create_intent_recognizer>

            // <add_intents>
            // Creates a language understanding model using the app id, and adds specific intents from your model
            LanguageUnderstandingModel model = LanguageUnderstandingModel.fromAppId("YourLanguageUnderstandingAppId");
            recognizer.addIntent(model, "YourLanguageUnderstandingIntentName1", "id1");
            recognizer.addIntent(model, "YourLanguageUnderstandingIntentName2", "id2");
            recognizer.addIntent(model, "YourLanguageUnderstandingIntentName3", "any-IntentId-here");
            // </add_intents>

            // To add all of the possible intents from a LUIS model to the recognizer, uncomment the line below:
            // recognizer.addAllIntents(model);

            // <recognize_intent>
            System.out.println("Say something...");

            // Starts recognition. It returns when the first utterance has been recognized.
            IntentRecognitionResult result = recognizer.recognizeOnceAsync().get();
            // </recognize_intent>

            // <print_result>
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
            // </print_result>
        // <skeleton_2>
        } catch (Exception ex) {
            System.out.println("Unexpected exception: " + ex.getMessage());

            assert(false);
            System.exit(1);
        }
    }
}
// </skeleton_2>
