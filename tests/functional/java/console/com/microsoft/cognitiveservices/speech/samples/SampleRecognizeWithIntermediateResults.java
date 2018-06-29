package com.microsoft.cognitiveservices.speech.samples;
//
//Copyright (c) Microsoft. All rights reserved.
//Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import java.util.concurrent.Future;

import com.microsoft.cognitiveservices.speech.SpeechFactory;
import com.microsoft.cognitiveservices.speech.SpeechRecognitionResult;
import com.microsoft.cognitiveservices.speech.SpeechRecognitionResultEventArgs;
import com.microsoft.cognitiveservices.speech.SpeechRecognizer;

public class SampleRecognizeWithIntermediateResults implements Runnable {
    
    ///////////////////////////////////////////////////
    // recognize with intermediate results
    ///////////////////////////////////////////////////
    public void run() {
        // create factory
        SpeechFactory factory = SampleSettings.getFactory();
        SpeechRecognizer reco = null;

        try {
            // Note: to use the microphone, replace the parameter with "new MicrophoneAudioInputStream()"
            reco = factory.createSpeechRecognizerWithFileInput(SampleSettings.WaveFile);

            reco.IntermediateResultReceived.addEventListener((o, speechRecognitionResultEventArgs) -> {
                String s = speechRecognitionResultEventArgs.getResult().getText();

                System.out.println("Intermediate result received: " + s);
            });

            Future<SpeechRecognitionResult> task = reco.recognizeAsync();

            SpeechRecognitionResult recognitionResult = task.get();
            String s = recognitionResult.getText();
            System.out.println("Recognizer returned: " + s);
        } catch (Exception ex) {
            SampleSettings.displayException(ex);
            
            throw new IllegalAccessError(ex.toString());
        }
        finally {
            if(reco != null) reco.close();
        }
    }
}
