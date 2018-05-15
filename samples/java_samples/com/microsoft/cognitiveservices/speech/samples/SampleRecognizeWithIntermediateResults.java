package com.microsoft.cognitiveservices.speech.samples;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import java.util.concurrent.Future;

import com.microsoft.cognitiveservices.speech.SpeechFactory;
import com.microsoft.cognitiveservices.speech.SpeechRecognitionResult;
import com.microsoft.cognitiveservices.speech.SpeechRecognitionResultEventArgs;
import com.microsoft.cognitiveservices.speech.SpeechRecognizer;

public class SampleRecognizeWithIntermediateResults implements Runnable {
    private SpeechRecognitionResult _result;
    private SpeechRecognitionResultEventArgs _speechRecognitionResultEventArgs;

    ///////////////////////////////////////////////////
    // recognize with intermediate results
    ///////////////////////////////////////////////////
    public void run() {
        // create factory
        SpeechFactory factory = SampleSettings.getFactory();

        try {
            // Note: to use the microphone, replace the parameter with "new MicrophoneAudioInputStream()"
            SpeechRecognizer reco = factory.createSpeechRecognizer(SampleSettings.WaveFile);

            reco.IntermediateResultReceived.addEventListener((o, speechRecognitionResultEventArgs) -> {
                _speechRecognitionResultEventArgs = speechRecognitionResultEventArgs;
                String s = _speechRecognitionResultEventArgs.getResult().getRecognizedText();

                System.out.println("Intermediate result received: " + s);
            });

            Future<SpeechRecognitionResult> task = reco.recognizeAsync();

            _result = task.get();
            String s = _result.getRecognizedText();
            System.out.println("Recognizer returned: " + s);
            
            reco.close();
            factory.close();
        } catch (Exception ex) {
            SampleSettings.displayException(ex);
            
            throw new IllegalAccessError(ex.toString());
        }
    }
}
