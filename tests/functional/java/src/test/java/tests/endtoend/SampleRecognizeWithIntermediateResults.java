//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

package tests.endtoend;

import java.util.concurrent.Future;

import com.microsoft.cognitiveservices.speech.audio.AudioConfig;
import com.microsoft.cognitiveservices.speech.SpeechConfig;
import com.microsoft.cognitiveservices.speech.SpeechRecognitionResult;
import com.microsoft.cognitiveservices.speech.SpeechRecognitionEventArgs;
import com.microsoft.cognitiveservices.speech.SpeechRecognizer;

import tests.Settings;

public class SampleRecognizeWithIntermediateResults implements Runnable {

    private SpeechRecognitionResult recognitionResult;
    public SpeechRecognitionResult getResult()
    {
        return recognitionResult;
    }

    private SpeechRecognitionEventArgs speechRecognitionEventArgs2;
    public SpeechRecognitionEventArgs getSpeechRecognitionEventArgs() {
        return speechRecognitionEventArgs2;
    }


    ///////////////////////////////////////////////////
    // recognize with intermediate results
    ///////////////////////////////////////////////////
    public void run() {
        // Note: the config is SHARED IN ALL END2END tests
        //       this is on purpose, to check if the config can be reused!
        //       therefore, do NOT CLOSE the config at the end of the test!
        SpeechConfig config = Settings.getSpeechConfig();

        try {

            AudioConfig audioInput = AudioConfig.fromWavFileInput(Settings.WavFile);
            SpeechRecognizer reco = new SpeechRecognizer(config, audioInput);

            reco.recognizing.addEventListener((o, speechRecognitionEventArgs) -> {
                speechRecognitionEventArgs2 = speechRecognitionEventArgs;
                String s = speechRecognitionEventArgs.getResult().getText();

                System.out.println("Intermediate result received: " + s);
            });

            Future<SpeechRecognitionResult> task = reco.recognizeOnceAsync();

            recognitionResult = task.get();
            String s = recognitionResult.getText();
            System.out.println("Recognizer returned: " + s);

            // Note: dont close config as it is shared
            reco.close();
            audioInput.close();
        } catch (Exception ex) {
            Settings.displayException(ex);

            throw new IllegalAccessError(ex.toString());
        }
    }
}
