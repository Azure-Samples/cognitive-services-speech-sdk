package com.microsoft.cognitiveservices.speech.samples;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.Future;

import com.microsoft.cognitiveservices.speech.SpeechFactory;
import com.microsoft.cognitiveservices.speech.intent.IntentRecognitionResult;
import com.microsoft.cognitiveservices.speech.intent.IntentRecognitionResultEventArgs;
import com.microsoft.cognitiveservices.speech.intent.IntentRecognizer;
import com.microsoft.cognitiveservices.speech.intent.LanguageUnderstandingModel;

public class SampleRecognizeIntent implements Runnable {
    private IntentRecognitionResult _result;
    private IntentRecognitionResultEventArgs _intentRecognitionResultEventArgs;

    ///////////////////////////////////////////////////
    // recognize intent
    ///////////////////////////////////////////////////
    @Override
    public void run() {
        // create factory
        SpeechFactory factory = SpeechFactory.fromSubscription(SampleSettings.LuisSubscriptionKey, SampleSettings.LuisRegion);

        List<String> content = new ArrayList<>();

        content.add("");
        try {
            // Note: to use the microphone, replace the parameter with "new MicrophoneAudioInputStream()"
            IntentRecognizer reco = factory.createIntentRecognizer(SampleSettings.WaveFile);

            HashMap<String, String> intentIdMap = new HashMap<>();
            intentIdMap.put("1", "play music");
            intentIdMap.put("2", "stop");
            intentIdMap.put("any", "");

            LanguageUnderstandingModel intentModel = LanguageUnderstandingModel.fromAppId(SampleSettings.LuisAppId);
            
            for (Map.Entry<String, String> entry : intentIdMap.entrySet()) {
                reco.addIntent(entry.getKey(), intentModel, entry.getValue());
            }

            reco.IntermediateResultReceived.addEventListener((o, intentRecognitionResultEventArgs) -> {
                _intentRecognitionResultEventArgs = intentRecognitionResultEventArgs;
                
                String s = _intentRecognitionResultEventArgs.getResult().getText();
                System.out.println("Intermediate result received: " + s);
                content.add(s);

                System.out.println(String.join("\n", content));
            });

            Future<IntentRecognitionResult> task = reco.recognizeAsync();
            _result = task.get();
            
            System.out.println("Continuous recognition stopped.");
            String s = _result.getText();
            String intentId = _result.getIntentId();
            String intent = "";
            if (intentIdMap.containsKey(intentId)) {
                intent = intentIdMap.get(intentId);
            }

            System.out.println("result received: " + s + ", intent: " + intent);
            content.add(s);
            content.add(" [intent: " + intent + "]");

            System.out.println(String.join("\n", content));

            reco.close();
            factory.close();
        } catch (Exception ex) {
            SampleSettings.displayException(ex);
            
            throw new IllegalAccessError(ex.toString());
        }
    }
}
