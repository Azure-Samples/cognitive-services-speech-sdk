//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// <code>
package speechsdk.quickstart;

import java.util.concurrent.Future;
import com.microsoft.cognitiveservices.speech.*;

/**
 * Quickstart: recognize speech using the Speech SDK for Java.
 */
public class Main {

    /**
     * @param args Arguments are ignored in this sample.
     */
    public static void main(String[] args) {

        // Replace below with your own subscription key
        String speechSubscriptionKey = "YourSubscriptionKey";
        // Replace below with your own service region (e.g., "westus").
        String serviceRegion = "YourServiceRegion";

        // Creates an instance of a speech recognizer using speech configuration with specified
        // subscription key and service region and microphone as default audio input.
        try (SpeechConfig config = SpeechConfig.fromSubscription(speechSubscriptionKey, serviceRegion);
             SpeechRecognizer reco = new SpeechRecognizer(config)) {

            assert(config != null);
            assert(reco != null);
            int exitCode = 1;

            System.out.println("Say something...");

            Future<SpeechRecognitionResult> task = reco.recognizeOnceAsync();
            assert(task != null);

            SpeechRecognitionResult result = task.get();
            assert(result != null);

            if (result.getReason() == ResultReason.RecognizedSpeech) {
                System.out.println("We recognized: " + result.getText());
                exitCode = 0;
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
            
            System.exit(exitCode);
        } catch (Exception ex) {
            System.out.println("Unexpected exception: " + ex.getMessage());

            assert(false);
            System.exit(1);
        }
    }
}
// </code>
