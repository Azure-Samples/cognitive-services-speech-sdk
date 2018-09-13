package tests.endtoend;
//
//Copyright (c) Microsoft. All rights reserved.
//Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import java.util.concurrent.Future;

import com.microsoft.cognitiveservices.speech.audio.AudioConfig;
import com.microsoft.cognitiveservices.speech.SpeechConfig;
import com.microsoft.cognitiveservices.speech.SpeechRecognitionResult;
import com.microsoft.cognitiveservices.speech.SpeechRecognizer;

import tests.Settings;

public class SampleSimpleRecognize implements Runnable {

    private String recognitionResult;
    public String getResult()
    {
        return recognitionResult;
    }
    
    ///////////////////////////////////////////////////
    // recognize
    ///////////////////////////////////////////////////
    @Override
    public void run() {
        // Note: the config is SHARED IN ALL END2END tests
        //       this is on purpose, to check if the config can be reused!
        //       therefore, do NOT CLOSE the config at the end of the test!
        SpeechConfig config = Settings.getSpeechConfig();

        try {
            // Note: to use the microphone, use "AudioConfig.fromDefaultMicrophoneInput()"
            AudioConfig audioInput = AudioConfig.fromWavFileInput(Settings.WavFile);
            SpeechRecognizer reco = new SpeechRecognizer(config, audioInput);

            Future<SpeechRecognitionResult> task = reco.recognizeOnceAsync();

            SpeechRecognitionResult result = task.get();
            recognitionResult = result.getText();

            System.out.println("Recognizer returned: " + recognitionResult);
            
            // Note: do not close the config as it is shared between tests.
            reco.close();
            audioInput.close();
        } catch (Exception ex) {
            recognitionResult = ex.toString();
            
            Settings.displayException(ex);
            throw new IllegalAccessError(ex.toString());
        }
    }
}
