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

import com.microsoft.cognitiveservices.speech.SpeechConfig;
import com.microsoft.cognitiveservices.speech.audio.AudioConfig;
import com.microsoft.cognitiveservices.speech.intent.IntentRecognitionResult;
import com.microsoft.cognitiveservices.speech.intent.IntentRecognitionResultEventArgs;
import com.microsoft.cognitiveservices.speech.intent.IntentRecognizer;
import com.microsoft.cognitiveservices.speech.intent.LanguageUnderstandingModel;

public class SampleRecognizeIntent implements Runnable {
    
    ///////////////////////////////////////////////////
    // recognize intent
    ///////////////////////////////////////////////////
    @Override
    public void run() {
        SpeechConfig config = null;
        AudioConfig audioInput = null;
        IntentRecognizer reco = null;

        List<String> content = new ArrayList<>();

        content.add("");
        SpeechConfig sc = null;
        try {
            // create config 
            sc = SpeechConfig.fromSubscription(SampleSettings.LuisSubscriptionKey, SampleSettings.LuisRegion);
            audioInput = AudioConfig.fromWavFileInput(SampleSettings.WavFile);
            reco = new IntentRecognizer(sc, audioInput);

            HashMap<String, String> intentIdMap = new HashMap<>();
            intentIdMap.put("1", "play music");
            intentIdMap.put("2", "stop");
            intentIdMap.put("any", "");

            LanguageUnderstandingModel intentModel = LanguageUnderstandingModel.fromAppId(SampleSettings.LuisAppId);
            
            for (Map.Entry<String, String> entry : intentIdMap.entrySet()) {
                reco.addIntent(intentModel, entry.getValue(), entry.getKey());
            }

            reco.IntermediateResultReceived.addEventListener((o, intentRecognitionResultEventArgs) -> {
                String s = intentRecognitionResultEventArgs.getResult().getText();
                System.out.println("Intermediate result received: " + s);
                content.add(s);

                System.out.println(String.join("\n", content));
            });

            Future<IntentRecognitionResult> task = reco.recognizeAsync();
            IntentRecognitionResult recognitionResult = task.get();
            
            System.out.println("Continuous recognition stopped.");
            String s = recognitionResult.getText();
            String intentId = recognitionResult.getIntentId();
            String intent = "";
            if (intentIdMap.containsKey(intentId)) {
                intent = intentIdMap.get(intentId);
            }

            System.out.println("result received: " + s + ", intent: " + intent);
            content.add(s);
            content.add(" [intent: " + intent + "]");

            System.out.println(String.join("\n", content));
        } catch (Exception ex) {
            SampleSettings.displayException(ex);
            
            throw new IllegalAccessError(ex.toString());
        }
        finally {
            if(reco != null) reco.close();
            if(sc != null) sc.close();
            if(audioInput != null) audioInput.close();
        }
    }
}
