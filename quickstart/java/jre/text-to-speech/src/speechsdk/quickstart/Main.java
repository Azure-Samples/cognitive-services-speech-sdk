//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// <code>
package speechsdk.quickstart;

import java.util.Scanner;
import java.util.concurrent.Future;
import com.microsoft.cognitiveservices.speech.*;

/**
 * Quickstart: synthesize speech using the Speech SDK for Java.
 */
public class Main {

    /**
     * @param args Arguments are ignored in this sample.
     */
    public static void main(String[] args) {

        // Replace below with your own subscription key
        String speechSubscriptionKey = "YourSubscriptionKey";
        // Replace below with your own endpoint URL (e.g., "https://westus.api.cognitive.microsoft.com/")
        String endpointUrl = "YourEndpointUrl";

        // Creates an instance of a speech synthesizer using speech configuration with
        // specified
        // endpoint and subscription key and default speaker as audio output.
        try (SpeechConfig config = SpeechConfig.fromEndpoint(new java.net.URI(endpointUrl), speechSubscriptionKey)) {
            // Configure the voice for speech synthesis. For a complete list of available voices,
            // visit https://aka.ms/speech/voices/neural
            // 
            // To use the latest LLM-based HD neural voice, set the voice name to "en-us-Ava:DragonHDLatestNeural"
            // (available in regions such as East US). Alternatively, use standard neural voices like
            // "en-US-AriaNeural" or "en-US-AvaMultilingualNeural".
            // 
            // For HD neural voice region availability, see https://aka.ms/speech/regions
            // config.setSpeechSynthesisVoiceName("en-US-AriaNeural");
            config.setSpeechSynthesisVoiceName("en-us-Ava:DragonHDLatestNeural");
            try (SpeechSynthesizer synth = new SpeechSynthesizer(config)) {

                assert (config != null);
                assert (synth != null);

                int exitCode = 1;

                System.out.println("Type some text that you want to speak...");
                System.out.print("> ");
                String text = new Scanner(System.in).nextLine();

                Future<SpeechSynthesisResult> task = synth.SpeakTextAsync(text);
                assert (task != null);

                SpeechSynthesisResult result = task.get();
                assert (result != null);

                if (result.getReason() == ResultReason.SynthesizingAudioCompleted) {
                    System.out.println("Speech synthesized to speaker for text [" + text + "]");
                    exitCode = 0;
                } else if (result.getReason() == ResultReason.Canceled) {
                    SpeechSynthesisCancellationDetails cancellation = SpeechSynthesisCancellationDetails
                            .fromResult(result);
                    System.out.println("CANCELED: Reason=" + cancellation.getReason());

                    if (cancellation.getReason() == CancellationReason.Error) {
                        System.out.println("CANCELED: ErrorCode=" + cancellation.getErrorCode());
                        System.out.println("CANCELED: ErrorDetails=" + cancellation.getErrorDetails());
                        System.out.println("CANCELED: Did you update the subscription info?");
                    }
                }

                System.exit(exitCode);
            }
        } catch (Exception ex) {
            System.out.println("Unexpected exception: " + ex.getMessage());

            assert (false);
            System.exit(1);
        }
    }
}
// </code>
