package com.microsoft.cognitiveservices.speech.samples;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import java.util.concurrent.Future;

import com.microsoft.cognitiveservices.speech.SpeechFactory;
import com.microsoft.cognitiveservices.speech.SpeechRecognitionResult;
import com.microsoft.cognitiveservices.speech.SpeechRecognizer;

public class SampleSimpleRecognize implements Runnable {
    private String _result;
    
    ///////////////////////////////////////////////////
    // recognize
    ///////////////////////////////////////////////////
    @Override
    public void run() {
        // create factory
        SpeechFactory factory = SampleSettings.getFactory();

        try {
            // Note: to use the microphone, replace the parameter with "new MicrophoneAudioInputStream()"
            SpeechRecognizer reco = factory.createSpeechRecognizer(SampleSettings.WaveFile);

            Future<SpeechRecognitionResult> task = reco.recognizeAsync();

            SpeechRecognitionResult result = task.get();
            _result = result.getText();

            System.out.println("Recognizer returned: " + _result);
            
            reco.close();
            factory.close();
        } catch (Exception ex) {
            _result = ex.toString();
            
            SampleSettings.displayException(ex);
            throw new IllegalAccessError(ex.toString());
        }
    }
}
