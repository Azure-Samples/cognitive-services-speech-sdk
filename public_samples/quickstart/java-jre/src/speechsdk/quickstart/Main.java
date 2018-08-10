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
        try {
            // Replace below with your own subscription key
            String speechSubscriptionKey = "YourSubscriptionKey";
            // Replace below with your own service region (e.g., "westus").
            String serviceRegion = "YourServiceRegion";

            int exitCode = 1;

            SpeechFactory factory = SpeechFactory.fromSubscription(speechSubscriptionKey, serviceRegion);
            assert(factory != null);

            SpeechRecognizer reco = factory.createSpeechRecognizer();
            assert(reco != null);

            System.out.println("Say something...");

            Future<SpeechRecognitionResult> task = reco.recognizeAsync();
            assert(task != null);

            SpeechRecognitionResult result = task.get();
            assert(result != null);

            if (result.getReason() == RecognitionStatus.Recognized) {
                System.out.println("We recognized: " + result.getText());
                exitCode = 0;
            }
            else if (result.getReason() == RecognitionStatus.Canceled) {
                System.out.println("The request was canceled. Did you update the subscription info?" +
                                   System.lineSeparator() +
                                   result.getErrorDetails());
            }
            else {
                System.out.println("No speech could be recognized. " +
                                   System.lineSeparator() +
                                   result.toString());
            }

            reco.close();
            factory.close();
            
            System.exit(exitCode);
        } catch (Exception ex) {
            System.out.println("Unexpected exception: " + ex.getMessage());

            assert(false);
            System.exit(1);
        }
    }
}
// </code>
