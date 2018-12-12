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
import com.microsoft.cognitiveservices.speech.Connection;

import tests.Settings;

public class SampleSimpleRecognize implements Runnable {

    private String recognitionResult;
    public String getResult()
    {
        return recognitionResult;
    }

    private int connectedEventCount;
    public int getConnectedEventCount() {
        return connectedEventCount;
    }

    private int disconnectedEventCount;
    public int getDisconnectedEventCount() {
        return disconnectedEventCount;
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
            Connection connection = Connection.fromRecognizer(reco);

            this.connectedEventCount = 0;
            this.disconnectedEventCount = 0;
            connection.connected.addEventListener((o, connectionEventArgs) -> {
                System.out.println("Connected event received.");
                this.connectedEventCount++;
            });
            connection.disconnected.addEventListener((o, connectionEventArgs) -> {
                System.out.println("Disconnected event received.");
                this.disconnectedEventCount++;
            });

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
