package tests.endtoend;
//
//Copyright (c) Microsoft. All rights reserved.
//Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import java.util.concurrent.Future;

import com.microsoft.cognitiveservices.speech.SpeechFactory;
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
        // Note: the factory is SHARED IN ALL END2END tests
        //       this is on purpose, to check if the factory can be reused!
        //       therefore, do NOT CLOSE the factory at the end of the test!
        SpeechFactory factory = Settings.getFactory();

        try {
            // TODO: to use the microphone, replace the parameter with "new MicrophoneAudioInputStream()"
            SpeechRecognizer reco = factory.createSpeechRecognizerWithFileInput(Settings.WaveFile);

            Future<SpeechRecognitionResult> task = reco.recognizeAsync();

            SpeechRecognitionResult result = task.get();
            recognitionResult = result.getText();

            System.out.println("Recognizer returned: " + recognitionResult);
            
            // Note: do not close the factory as it is shared between tests.
            reco.close();
        } catch (Exception ex) {
            recognitionResult = ex.toString();
            
            Settings.displayException(ex);
            throw new IllegalAccessError(ex.toString());
        }
    }
}
