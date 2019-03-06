package tests.endtoend;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.Future;
import java.util.concurrent.atomic.AtomicInteger;

import com.microsoft.cognitiveservices.speech.audio.AudioConfig;
import com.microsoft.cognitiveservices.speech.SpeechConfig;
import com.microsoft.cognitiveservices.speech.intent.IntentRecognitionResult;
import com.microsoft.cognitiveservices.speech.intent.IntentRecognitionEventArgs;
import com.microsoft.cognitiveservices.speech.intent.IntentRecognizer;
import com.microsoft.cognitiveservices.speech.intent.LanguageUnderstandingModel;
import com.microsoft.cognitiveservices.speech.Connection;

import tests.Settings;
import tests.TestHelper;

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

    private int connectedEventCount;
    public int getConnectedEventCount() {
        return connectedEventCount;
    }

    private int disconnectedEventCount;
    public int getDisconnectedEventCount() {
        return disconnectedEventCount;
    }

    // This is to keep the connection alive.
    private Connection connection;

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
            connection = Connection.fromRecognizer(reco);

            HashMap<String, String> intentIdMap = new HashMap<>();
            intentIdMap.put("1", "play music");
            intentIdMap.put("2", "stop");
            intentIdMap.put("any", "");

            LanguageUnderstandingModel intentModel = LanguageUnderstandingModel.fromAppId(Settings.LuisAppId);

            for (Map.Entry<String, String> entry : intentIdMap.entrySet()) {
                reco.addIntent(intentModel, entry.getValue(), entry.getKey());
            }

            this.connectedEventCount = 0;
            this.disconnectedEventCount = 0;

            connection.connected.addEventListener((o, connectionEventArgs) -> {
                System.out.println("Connected event received.");
                this.connectedEventCount++;
            });
            connection.disconnected.addEventListener((o, connectionEventArgs) -> {
                System.out.println("Disconnected event received.");
                this.disconnectedEventCount++;
            });

            reco.recognizing.addEventListener((o, intentRecognitionResultEventArgs) -> {
                intentRecognitionResultEventArgs2 = intentRecognitionResultEventArgs;
                
                String s = intentRecognitionResultEventArgs.getResult().getText();
                System.out.println("Intermediate result received: " + s);
                content.add(s);

                System.out.println(String.join("\n", content));
            });

            AtomicInteger sessionStoppedCount = new AtomicInteger(0);
            reco.sessionStopped.addEventListener((o, SessionEventArgs) -> {
                sessionStoppedCount.getAndIncrement();
            });

            Future<IntentRecognitionResult> task = reco.recognizeOnceAsync();
            recognitionResult = task.get();

            System.out.println("RecognizeOnce stopped.");
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

            // wait until we get the SessionStopped event.
            long now = System.currentTimeMillis();
            while(((System.currentTimeMillis() - now) < 30000) && (sessionStoppedCount.get() == 0)) {
                Thread.sleep(200);
            }

            reco.close();
            config.close();
            audioInput.close();
        } catch (Exception ex) {
            Settings.displayException(ex);
            
            throw new IllegalAccessError(ex.toString());
        }
    }
}
