//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// <code>
package speechsdk.quickstart;

import java.nio.file.Files;
import java.nio.file.Paths;
import java.io.IOException;
import java.nio.file.Path;
import org.json.JSONObject;
import java.util.Map;
import java.util.Scanner;
import java.util.concurrent.ExecutionException;
import com.microsoft.cognitiveservices.speech.*;
import com.microsoft.cognitiveservices.speech.translation.*;

public class Main {

    public static void translationWithMicrophoneAsync() throws InterruptedException, ExecutionException, IOException, java.net.URISyntaxException
    {
        Path configPath = Paths.get(System.getProperty("user.dir"), "config.json");
        String content = new String(Files.readAllBytes(configPath));
        JSONObject jsonObject = new JSONObject(content);

        // Replace with your own subscription key.
        String speechSubscriptionKey = jsonObject.getString("SubscriptionKey");
        // Replace below with your own service region (e.g., "westus").
        String speechServiceRegion = jsonObject.getString("ServiceRegion");

        // Replace below with your own endpoint URL (e.g., "https://westus.api.cognitive.microsoft.com")
        String endpointUrl = "https://" + speechServiceRegion + ".api.cognitive.microsoft.com/";
        
        // Creates an instance of a translation recognizer using speech translation configuration with specified
        // endpoint and subscription key and microphone as default audio input.
        try (SpeechTranslationConfig config = SpeechTranslationConfig.fromEndpoint(new java.net.URI(endpointUrl), speechSubscriptionKey)) {
            
            // Sets source and target language(s).
            String fromLanguage = "en-US";
            config.setSpeechRecognitionLanguage(fromLanguage);
            config.addTargetLanguage("de");

            // Sets voice name of synthesis output.
            String GermanVoice = "de-DE-AmalaNeural";
            config.setVoiceName(GermanVoice);

            try (TranslationRecognizer recognizer = new TranslationRecognizer(config)) {
                // Subscribes to events.
                recognizer.recognizing.addEventListener((s, e) -> {
                    System.out.println("RECOGNIZING in '" + fromLanguage + "': Text=" + e.getResult().getText());

                    Map<String, String> map = e.getResult().getTranslations();
                    for(String element : map.keySet()) {
                        System.out.println("    TRANSLATING into '" + element + "': " + map.get(element));
                    }
                });

                recognizer.recognized.addEventListener((s, e) -> {
                    if (e.getResult().getReason() == ResultReason.TranslatedSpeech) {
                        System.out.println("RECOGNIZED in '" + fromLanguage + "': Text=" + e.getResult().getText());

                        Map<String, String> map = e.getResult().getTranslations();
                        for(String element : map.keySet()) {
                            System.out.println("    TRANSLATED into '" + element + "': " + map.get(element));
                        }
                    }
                    if (e.getResult().getReason() == ResultReason.RecognizedSpeech) {
                        System.out.println("RECOGNIZED: Text=" + e.getResult().getText());
                        System.out.println("    Speech not translated.");
                    }
                    else if (e.getResult().getReason() == ResultReason.NoMatch) {
                        System.out.println("NOMATCH: Speech could not be recognized.");
                    }
                });

                recognizer.synthesizing.addEventListener((s, e) -> {
                    System.out.println("Synthesis result received. Size of audio data: " + e.getResult().getAudio().length);
                });

                recognizer.canceled.addEventListener((s, e) -> {
                    System.out.println("CANCELED: Reason=" + e.getReason());

                    if (e.getReason() == CancellationReason.Error) {
                        System.out.println("CANCELED: ErrorCode=" + e.getErrorCode());
                        System.out.println("CANCELED: ErrorDetails=" + e.getErrorDetails());
                        System.out.println("CANCELED: Did you update the subscription info?");
                    }
                });

                recognizer.sessionStarted.addEventListener((s, e) -> {
                    System.out.println("\nSession started event.");
                });

                recognizer.sessionStopped.addEventListener((s, e) -> {
                    System.out.println("\nSession stopped event.");
                });

                // Starts continuous recognition. Uses StopContinuousRecognitionAsync() to stop recognition.
                System.out.println("Say something...");
                recognizer.startContinuousRecognitionAsync().get();

                System.out.println("Press any key to stop");
                new Scanner(System.in).nextLine();

                recognizer.stopContinuousRecognitionAsync().get();
            }
        }
    }

    public static void main(String[] args) {
        try {
            translationWithMicrophoneAsync();
        } catch (Exception ex) {
            System.out.println("Unexpected exception: " + ex.getMessage());
            assert(false);
            System.exit(1);
        }
    }
}
// </code>
