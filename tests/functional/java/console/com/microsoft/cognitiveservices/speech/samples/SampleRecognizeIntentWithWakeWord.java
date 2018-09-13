package com.microsoft.cognitiveservices.speech.samples;
//
//Copyright (c) Microsoft. All rights reserved.
//Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.Future;

import com.microsoft.cognitiveservices.speech.audio.AudioConfig;
import com.microsoft.cognitiveservices.speech.KeywordRecognitionModel;
import com.microsoft.cognitiveservices.speech.SpeechConfig;
import com.microsoft.cognitiveservices.speech.intent.IntentRecognizer;
import com.microsoft.cognitiveservices.speech.intent.LanguageUnderstandingModel;

public class SampleRecognizeIntentWithWakeWord implements Runnable, Stoppable {
    private static final String delimiter = "\n";
    private final List<String> content = new ArrayList<>();
    private boolean continuousListeningStarted = false;
    private IntentRecognizer reco = null;
    private AudioConfig audioInput = null;
    private String buttonText = "";

    @Override
    public  void stop()
    {
        if (continuousListeningStarted) {
            if (reco != null) {
                Future<?> task = reco.stopKeywordRecognitionAsync();
                SampleSettings.setOnTaskCompletedListener(task, result -> {
                    reco.close();
                    audioInput.close();

                    System.out.println("Continuous recognition stopped.");
                    System.out.println(buttonText);
                    continuousListeningStarted = false;
                });
            } else {
                continuousListeningStarted = false;
            }
        }

    }
 
    ///////////////////////////////////////////////////
    // recognize intent with wake word
    ///////////////////////////////////////////////////
    @Override
    public void run () {
        if (continuousListeningStarted) {
            return;
        }
        
        HashMap<String, String> intentIdMap = new HashMap<>();
        intentIdMap.put("1", "play music");
        intentIdMap.put("2", "stop");

        // create config 
        SpeechConfig config = SampleSettings.getSpeechConfig();

        content.clear();
        content.add("");
        content.add("");
        try {
            // Note: to use the microphone, replace the parameter with "new MicrophoneAudioInputStream()"
            audioInput = AudioConfig.fromWavFileInput(SampleSettings.WavFile);
            reco = new IntentRecognizer(config, audioInput);
            
            LanguageUnderstandingModel intentModel = LanguageUnderstandingModel.fromSubscription(SampleSettings.LuisRegion,
                    SampleSettings.LuisSubscriptionKey, SampleSettings.LuisAppId);
            for (Map.Entry<String, String> entry : intentIdMap.entrySet()) {
                reco.addIntent(intentModel, entry.getValue(), entry.getKey());
            }

            reco.sessionStarted.addEventListener((o, sessionEventArgs) -> {
                System.out.println("got a session started (" + sessionEventArgs.getSessionId() + ")event");

                content.set(0, "KeywordModel `" + SampleSettings.Keyword + "` detected");
                System.out.println(String.join(delimiter, content));
                content.add("");
            });

            reco.sessionStopped.addEventListener((o, sessionEventArgs) -> {
                System.out.println("got a session stopped (" + sessionEventArgs.getSessionId() + ")event");
            });

            reco.recognizing.addEventListener((o, intermediateResultEventArgs) -> {
                String s = intermediateResultEventArgs.getResult().getText();
                System.out.println("got an intermediate result: " + s);
                Integer index = content.size() - 2;
                content.set(index + 1, index.toString() + ". " + s);
                System.out.println(String.join(delimiter, content));
            });

            reco.recognized.addEventListener((o, finalResultEventArgs) -> {
                String s = finalResultEventArgs.getResult().getText();
                String intentId = finalResultEventArgs.getResult().getIntentId();
                String intent = "";
                if (intentIdMap.containsKey(intentId)) {
                    intent = intentIdMap.get(intentId);
                }

                System.out.println("got a result: " + s);
                if (!s.isEmpty()) {
                    Integer index = content.size() - 2;
                    content.set(index + 1, index.toString() + ". " + s + " [intent: " + intent + "]");
                    content.set(0, "say `" + SampleSettings.Keyword + "`...");
                    System.out.println(String.join(delimiter, content));
                }
            });

            Future<?> task = reco.startKeywordRecognitionAsync(KeywordRecognitionModel.fromFile(SampleSettings.KeywordModel));
            SampleSettings.setOnTaskCompletedListener(task, result -> {
                content.set(0, "say `" + SampleSettings.Keyword + "`...");
                System.out.println(String.join(delimiter, content));
            });

            continuousListeningStarted = true;
        } catch (Exception ex) {
            System.out.println(ex.getMessage());
            SampleSettings.displayException(ex);
        }
    }
}
