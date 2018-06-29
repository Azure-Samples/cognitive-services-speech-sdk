package com.microsoft.cognitiveservices.speech.samples;
//
//Copyright (c) Microsoft. All rights reserved.
//Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import java.util.concurrent.Future;

import com.microsoft.cognitiveservices.speech.SpeechFactory;
import com.microsoft.cognitiveservices.speech.SpeechRecognitionResult;
import com.microsoft.cognitiveservices.speech.SpeechRecognizer;

public class SampleSimpleRecognize implements Runnable {
    
    ///////////////////////////////////////////////////
    // recognize
    ///////////////////////////////////////////////////
    @Override
    public void run() {
        // create factory
        SpeechFactory factory = SampleSettings.getFactory();
        SpeechRecognizer reco = null;

        try {
            // Note: to use the microphone, replace the parameter with "new MicrophoneAudioInputStream()"
            reco = factory.createSpeechRecognizerWithFileInput(SampleSettings.WaveFile);

            Future<SpeechRecognitionResult> task = reco.recognizeAsync();

            SpeechRecognitionResult result = task.get();
            String text = result.getText();

            System.out.println("Recognizer returned: " + text);
        } catch (Exception ex) {
            SampleSettings.displayException(ex);
            throw new IllegalAccessError(ex.toString());
        }
        finally {
            if(reco != null ) reco.close();
            factory.close();
        }
    }
}
