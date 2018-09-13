package com.microsoft.cognitiveservices.speech.samples;
//
//Copyright (c) Microsoft. All rights reserved.
//Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import java.util.concurrent.Future;

import com.microsoft.cognitiveservices.speech.audio.AudioConfig;
import com.microsoft.cognitiveservices.speech.SpeechConfig;
import com.microsoft.cognitiveservices.speech.SpeechRecognitionResult;
import com.microsoft.cognitiveservices.speech.SpeechRecognizer;

public class SampleSimpleRecognize implements Runnable {
    
    ///////////////////////////////////////////////////
    // recognize
    ///////////////////////////////////////////////////
    @Override
    public void run() {
        // create config 
        SpeechConfig config = null;
        SpeechRecognizer reco = null;
        AudioConfig audioInput = null;

        try {

            config = SampleSettings.getSpeechConfig();
            audioInput = AudioConfig.fromWavFileInput(SampleSettings.WavFile);   
            reco = new SpeechRecognizer(config, audioInput);

            Future<SpeechRecognitionResult> task = reco.recognizeOnceAsync();

            SpeechRecognitionResult result = task.get();
            String text = result.getText();

            System.out.println("Recognizer returned: " + text);
        } catch (Exception ex) {
            SampleSettings.displayException(ex);
            throw new IllegalAccessError(ex.toString());
        }
        finally {
            if(reco != null) reco.close();
            if(audioInput != null) audioInput.close();
            if(config != null) config.close();
        }
    }
}
