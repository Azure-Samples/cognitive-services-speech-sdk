package tests.endtoend;
//
//Copyright (c) Microsoft. All rights reserved.
//Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import java.util.concurrent.Future;

import com.microsoft.cognitiveservices.speech.audio.AudioConfig;
import com.microsoft.cognitiveservices.speech.SpeechFactory;
import com.microsoft.cognitiveservices.speech.SpeechRecognitionResult;
import com.microsoft.cognitiveservices.speech.SpeechRecognitionResultEventArgs;
import com.microsoft.cognitiveservices.speech.SpeechRecognizer;

import tests.Settings;

public class SampleRecognizeWithIntermediateResults implements Runnable {
    
    private SpeechRecognitionResult recognitionResult;
    public SpeechRecognitionResult getResult()
    {
        return recognitionResult;
    }
    
    private SpeechRecognitionResultEventArgs speechRecognitionResultEventArgs2;
    public SpeechRecognitionResultEventArgs getSpeechRecognitionResultEventArgs() {
        return speechRecognitionResultEventArgs2;
    }
    
    
    ///////////////////////////////////////////////////
    // recognize with intermediate results
    ///////////////////////////////////////////////////
    public void run() {
        // Note: the factory is SHARED IN ALL END2END tests
        //       this is on purpose, to check if the factory can be reused!
        //       therefore, do NOT CLOSE the factory at the end of the test!
        SpeechFactory factory = Settings.getFactory();

        try {
            // Note: to use the microphone, use "AudioConfig.fromDefaultMicrophoneInput()"
            AudioConfig audioInput = AudioConfig.fromWavFileInput(Settings.WavFile);
            SpeechRecognizer reco = factory.createSpeechRecognizerFromConfig(audioInput);

            reco.IntermediateResultReceived.addEventListener((o, speechRecognitionResultEventArgs) -> {
                speechRecognitionResultEventArgs2 = speechRecognitionResultEventArgs;
                String s = speechRecognitionResultEventArgs.getResult().getText();

                System.out.println("Intermediate result received: " + s);
            });

            Future<SpeechRecognitionResult> task = reco.recognizeAsync();

            recognitionResult = task.get();
            String s = recognitionResult.getText();
            System.out.println("Recognizer returned: " + s);

            // Note: dont close factory as it is shared
            reco.close();
            audioInput.close();
        } catch (Exception ex) {
            Settings.displayException(ex);
            
            throw new IllegalAccessError(ex.toString());
        }
    }
}
