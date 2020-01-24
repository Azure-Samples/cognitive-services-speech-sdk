//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

package tests.endtoend;

import java.util.concurrent.Future;
import java.util.concurrent.atomic.AtomicInteger;

import com.microsoft.cognitiveservices.speech.audio.AudioConfig;
import com.microsoft.cognitiveservices.speech.SpeechConfig;
import com.microsoft.cognitiveservices.speech.SpeechRecognitionResult;
import com.microsoft.cognitiveservices.speech.SpeechRecognizer;
import com.microsoft.cognitiveservices.speech.Connection;

import tests.AudioUtterancesKeys;
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

    // This is to keep the connection alive.
    private Connection connection;

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
            AudioConfig audioInput = AudioConfig.fromWavFileInput(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath));
            SpeechRecognizer reco = new SpeechRecognizer(config, audioInput);
            connection = Connection.fromRecognizer(reco);

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

            AtomicInteger sessionStoppedCount = new AtomicInteger(0);
            reco.sessionStopped.addEventListener((o, SessionEventArgs) -> {
                sessionStoppedCount.getAndIncrement();
            });

            Future<SpeechRecognitionResult> task = reco.recognizeOnceAsync();

            SpeechRecognitionResult result = task.get();
            recognitionResult = result.getText();

            System.out.println("Recognizer returned: " + recognitionResult);

            // wait until we get the SessionStopped event.
            long now = System.currentTimeMillis();
            while(((System.currentTimeMillis() - now) < 30000) && (sessionStoppedCount.get() == 0)) {
                Thread.sleep(200);
            }

            // Note: do not close the config as it is shared between tests.
            connection.closeConnection();
            connection.close();
            reco.close();
            audioInput.close();
        } catch (Exception ex) {
            recognitionResult = ex.toString();

            Settings.displayException(ex);
            throw new IllegalAccessError(ex.toString());
        }
    }
}
