package com.microsoft.cognitiveservices.speech.samples;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.Future;

import com.microsoft.cognitiveservices.speech.KeywordRecognitionModel;
import com.microsoft.cognitiveservices.speech.SessionEventType;
import com.microsoft.cognitiveservices.speech.SpeechFactory;
import com.microsoft.cognitiveservices.speech.SpeechRecognizer;

public class SampleRecognizeWithWakeWord implements Runnable, Stoppable {

    private static final String delimiter = "\n";
    private final List<String> content = new ArrayList<>();
    private boolean continuousListeningStarted = false;
    private SpeechRecognizer reco = null;
    private String buttonText = "";

    @Override
    public void stop() {
        if (continuousListeningStarted) {
            if (reco != null) {
                Future<?> task = reco.stopKeywordRecognitionAsync();
                SampleSettings.setOnTaskCompletedListener(task, result -> {
                    // reco.close();

                    System.out.println("Continuous recognition stopped.");
                    System.out.println(buttonText);

                    continuousListeningStarted = false;
                });
            } else {
                continuousListeningStarted = false;
            }

            return;
        }
    }

    ///////////////////////////////////////////////////
    // recognize with wake word
    ///////////////////////////////////////////////////
    @Override
    public void run() {
        if (continuousListeningStarted) {
            return;
        }

        // create factory
        final SpeechFactory factory = SampleSettings.getFactory();

        content.clear();
        content.add("");
        try {
            // Note: to use the microphone, replace the parameter with "new MicrophoneAudioInputStream()"
            reco = factory.createSpeechRecognizer(SampleSettings.WaveFile);

            reco.SessionEvent.addEventListener((o, sessionEventArgs) -> {

                System.out.println("got a session (" + sessionEventArgs.getSessionId() + ")event: "
                        + sessionEventArgs.getEventType());
                if (sessionEventArgs.getEventType() == SessionEventType.SessionStartedEvent) {

                    content.set(0, "KeywordModel `" + SampleSettings.Keyword + "` detected");
                    System.out.println(String.join(delimiter, content));
                    content.add("");
                }
            });

            reco.IntermediateResultReceived.addEventListener((o, intermediateResultEventArgs) -> {
                String s = intermediateResultEventArgs.getResult().getText();

                System.out.println("got an intermediate result: " + s);
                Integer index = content.size() - 2;
                content.set(index + 1, index.toString() + ". " + s);
                System.out.println(String.join(delimiter, content));
            });

            reco.FinalResultReceived.addEventListener((o, finalResultEventArgs) -> {
                String s = finalResultEventArgs.getResult().getText();

                System.out.println("got a final result: " + s);
                if (!s.isEmpty()) {
                    Integer index = content.size() - 2;
                    content.set(index + 1, index.toString() + ". " + s);
                    content.set(0, "say `" + SampleSettings.Keyword + "`...");
                    System.out.println(String.join(delimiter, content));
                }
            });

            Future<?> task = reco
                    .startKeywordRecognitionAsync(KeywordRecognitionModel.fromFile(SampleSettings.KeywordModel));
            SampleSettings.setOnTaskCompletedListener(task, result -> {
                content.set(0, "say `" + SampleSettings.Keyword + "`...");

                System.out.println(String.join(delimiter, content));
            });

            continuousListeningStarted = true;
        } catch (Exception ex) {
            System.out.println(ex.getMessage());
            SampleSettings.displayException(ex);
        }
    }
}
