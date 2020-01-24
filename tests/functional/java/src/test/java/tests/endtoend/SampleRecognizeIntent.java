//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

package tests.endtoend;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.Future;
import com.microsoft.cognitiveservices.speech.audio.AudioConfig;
import com.microsoft.cognitiveservices.speech.SpeechConfig;
import com.microsoft.cognitiveservices.speech.intent.IntentRecognitionResult;
import com.microsoft.cognitiveservices.speech.intent.IntentRecognizer;
import com.microsoft.cognitiveservices.speech.intent.LanguageUnderstandingModel;
import tests.AudioUtterancesKeys;
import tests.DefaultSettingsKeys;
import tests.Settings;
import tests.SubscriptionsRegionsKeys;
import tests.TestHelper;

public class SampleRecognizeIntent implements Runnable {
    private IntentRecognitionResult recognitionResult;
    public IntentRecognitionResult getResult()
    {
        return recognitionResult;
    }

    private String intentId;
    public String getIntentId()
    {
        return intentId;
    }

    private String intentName;
    public String getIntentName()
    {
        return intentName;
    }

    ///////////////////////////////////////////////////
    // recognize intent
    ///////////////////////////////////////////////////
    @Override
    public void run() {
        // create config
        SpeechConfig config = SpeechConfig.fromSubscription(Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.LANGUAGE_UNDERSTANDING_SUBSCRIPTION).Key,
            Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.LANGUAGE_UNDERSTANDING_SUBSCRIPTION).Region);

        List<String> content = new ArrayList<>();

        content.add("");
        try {
            // TODO: to use the microphone, replace the parameter with "new MicrophoneAudioInputStream()"
            AudioConfig audioInput = AudioConfig.fromWavFileInput(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.INTENT_UTTERANCE).FilePath));
            IntentRecognizer reco = new IntentRecognizer(config, audioInput);

            HashMap<String, String> intentIdMap = new HashMap<>();
            intentIdMap.put("1", "HomeAutomation.TurnOn");
            intentIdMap.put("2", "HomeAutomation.TurnOff");

            LanguageUnderstandingModel intentModel = LanguageUnderstandingModel.fromAppId(Settings.DefaultSettingsMap.get(DefaultSettingsKeys.LANGUAGE_UNDERSTANDING_HOME_AUTOMATION_APP_ID));
            System.out.println("LuisAppId: " + Settings.DefaultSettingsMap.get(DefaultSettingsKeys.LANGUAGE_UNDERSTANDING_HOME_AUTOMATION_APP_ID));

            for (Map.Entry<String, String> entry : intentIdMap.entrySet()) {
                reco.addIntent(intentModel, entry.getValue(), entry.getKey());
            }

            reco.recognizing.addEventListener((o, intentRecognitionResultEventArgs) -> {
                String s = intentRecognitionResultEventArgs.getResult().getText();
                System.out.println("Intermediate result received: " + s);
                content.add(s);
            });

            Future<IntentRecognitionResult> task = reco.recognizeOnceAsync();
            recognitionResult = task.get();

            TestHelper.OutputResult(recognitionResult);

            intentId = recognitionResult.getIntentId();
            intentName = "";
            if (intentIdMap.containsKey(intentId)) {
                intentName = intentIdMap.get(intentId);
            }

            System.out.println("result received: " + recognitionResult.getText() + ", intentid: " + intentId + ", intentName: " + intentName);
            content.add(recognitionResult.getText());
            content.add(" [intent: " + intentName + "]");

            System.out.println(content.toString() + "\n");

            reco.close();
            config.close();
            audioInput.close();
        } catch (Exception ex) {
            Settings.displayException(ex);

            throw new IllegalAccessError(ex.toString());
        }
    }
}
