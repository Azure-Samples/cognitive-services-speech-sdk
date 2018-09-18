package tests.endtoend;
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
import com.microsoft.cognitiveservices.speech.SpeechConfig;
import com.microsoft.cognitiveservices.speech.intent.IntentRecognitionResult;
import com.microsoft.cognitiveservices.speech.intent.IntentRecognitionEventArgs;
import com.microsoft.cognitiveservices.speech.intent.IntentRecognizer;
import com.microsoft.cognitiveservices.speech.intent.LanguageUnderstandingModel;

import tests.Settings;

public class SampleRecognizeIntent implements Runnable {
    private IntentRecognitionResult recognitionResult;
    public IntentRecognitionResult getResult()
    {
        return recognitionResult;
    }
    
    private IntentRecognitionEventArgs intentRecognitionResultEventArgs2;
    public IntentRecognitionEventArgs getRecognizing() {
        return intentRecognitionResultEventArgs2;
    }
    
    ///////////////////////////////////////////////////
    // recognize intent
    ///////////////////////////////////////////////////
    @Override
    public void run() {
        // create config 
        SpeechConfig config = SpeechConfig.fromSubscription(Settings.LuisSubscriptionKey, Settings.LuisRegion);

        List<String> content = new ArrayList<>();

        content.add("");
        try {
            // TODO: to use the microphone, replace the parameter with "new MicrophoneAudioInputStream()"
            AudioConfig audioInput = AudioConfig.fromWavFileInput(Settings.WavFile);
            IntentRecognizer reco = new IntentRecognizer(config, audioInput);

            HashMap<String, String> intentIdMap = new HashMap<>();
            intentIdMap.put("1", "play music");
            intentIdMap.put("2", "stop");
            intentIdMap.put("any", "");

            LanguageUnderstandingModel intentModel = LanguageUnderstandingModel.fromAppId(Settings.LuisAppId);
            
            for (Map.Entry<String, String> entry : intentIdMap.entrySet()) {
                reco.addIntent(intentModel, entry.getValue(), entry.getKey());
            }

            reco.recognizing.addEventListener((o, intentRecognitionResultEventArgs) -> {
                intentRecognitionResultEventArgs2 = intentRecognitionResultEventArgs;
                
                String s = intentRecognitionResultEventArgs.getResult().getText();
                System.out.println("Intermediate result received: " + s);
                content.add(s);

                System.out.println(String.join("\n", content));
            });

            Future<IntentRecognitionResult> task = reco.recognizeOnceAsync();
            recognitionResult = task.get();
            
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

            reco.close();
            config.close();
            audioInput.close();
        } catch (Exception ex) {
            Settings.displayException(ex);
            
            throw new IllegalAccessError(ex.toString());
        }
    }
}
