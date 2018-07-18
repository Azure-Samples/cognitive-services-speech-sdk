//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package speechsdk.quickstart;

import java.util.concurrent.Future;
import com.microsoft.cognitiveservices.speech.*;

/**
 * Java-Jre Quickstart
 */
public class Main {

    /**
     * @param args Arguments are ignored in this Quickstart sample.
     */
    public static void main(String[] args) {
        try {
            // Replace below with your own subscription key
            String speechSubscriptionKey = "YourSubscriptionKey";
            // Replace below with your own service region (e.g., "westus").
            String serviceRegion = "YourServiceRegion";

            SpeechFactory factory = SpeechFactory.fromSubscription(speechSubscriptionKey, serviceRegion);
            assert(factory!= null);

            SpeechRecognizer reco = factory.createSpeechRecognizer();
            assert(reco != null);

            Future<SpeechRecognitionResult> task = reco.recognizeAsync();
            assert(task != null);

            SpeechRecognitionResult result = task.get();
            assert(result != null);

            if (result.getReason() == RecognitionStatus.Recognized) {
                System.out.println("SpeechSDKDemo: " + result.getText());
            }
            else if (result.getReason() == RecognitionStatus.Canceled) {
                System.out.println("SpeechSDKDemo: The request was Canceled. Did you update the subscription info?" +
                                   System.lineSeparator() +
                                   result.getErrorDetails());
            }
            else {
                System.out.println("SpeechSDKDemo: No speech could be recognized. " +
                                   System.lineSeparator() +
                                   result.toString());
            }

            reco.close();
            factory.close();
            
            System.exit(0);
        } catch (Exception ex) {
            System.out.println("SpeechSDKDemo: Unexpected " + ex.getMessage());

            assert(false);
            System.exit(1);
        }
    }
}
