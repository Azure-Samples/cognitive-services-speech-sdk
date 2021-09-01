/**
 * Copyright (c) Microsoft. All rights reserved. Licensed under the MIT license.
 * See LICENSE.md file in the project root for full license information.
 */
// <code>
package com.speechsdk.quickstart;

import com.fasterxml.jackson.annotation.JsonInclude;
import com.fasterxml.jackson.databind.DeserializationFeature;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.datatype.joda.JodaModule;
import com.fasterxml.jackson.datatype.jsr310.JavaTimeModule;
import com.microsoft.bot.schema.Activity;
import com.microsoft.cognitiveservices.speech.audio.AudioConfig;
import com.microsoft.cognitiveservices.speech.audio.PullAudioOutputStream;
import com.microsoft.cognitiveservices.speech.dialog.BotFrameworkConfig;
import com.microsoft.cognitiveservices.speech.dialog.DialogServiceConfig;
import com.microsoft.cognitiveservices.speech.dialog.DialogServiceConnector;
import org.apache.commons.lang3.StringUtils;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import javax.sound.sampled.AudioFormat;
import javax.sound.sampled.AudioSystem;
import javax.sound.sampled.DataLine;
import javax.sound.sampled.SourceDataLine;
import java.io.IOException;
import java.io.InputStream;
import java.text.SimpleDateFormat;
import java.util.concurrent.*;


public class Main {

    public static final int DEFAULT_TIMEOUT_FOR_BOT_RESPONSE_IN_SECONDS = 10;
    public static final int WAIT_INTERVAL_IN_MILLIS = 1000;

    private static final Logger log = LoggerFactory.getLogger(Main.class);
    private static final ObjectMapper mapper = new ObjectMapper()
    {
        {
            this.registerModule(new JodaModule());
            this.registerModule(new JavaTimeModule());
            this.findAndRegisterModules();
            this.configure(DeserializationFeature.FAIL_ON_UNKNOWN_PROPERTIES, false);
            this.setSerializationInclusion(JsonInclude.Include.NON_NULL);
            this.setDateFormat(new SimpleDateFormat());
        }
    };

    private static Boolean receivedResponse;

    public static void main(String[] args) {

        // Please replace below with your speech channel secret, speech
        // subscription key, and service region.
        //
        // Note: In preview, the Direct Line Speech channel currently supports only the `westus2` region.
        final String subscriptionKey = "YourSubscriptionKey";
        final String region = "YourServiceRegion";

        assert !subscriptionKey.equals("YourSubscriptionKey") : "Replace the string \"YourSubscriptionKey\" with your speech subscription key.";
        assert !region.equals("YourServiceRegion") : "Replace the string \"YourServiceRegion\" with your service region.";

        // Create a DialogServiceConfig instance from channel secret, subscription key and region
        final DialogServiceConfig dialogServiceConfig = BotFrameworkConfig.fromSubscription(subscriptionKey, region);
        if (dialogServiceConfig == null) {
            log.error("DialogServiceConfig should not be null");
        }

        // Set audio input from microphone.
        final AudioConfig audioConfig = AudioConfig.fromDefaultMicrophoneInput();

        // Create a DialogServiceConnector instance
        final DialogServiceConnector dialogServiceConnector = new DialogServiceConnector(dialogServiceConfig, audioConfig);

        // Configure all event listeners.
        registerEventListeners(dialogServiceConnector);
        ExecutorService executorService = Executors.newSingleThreadExecutor();

        try {
            // Connect to the backing dialog.
            dialogServiceConnector.connectAsync().get();
            log.info("DialogServiceConnector is successfully connected");

            // Start listening.
            System.out.println("Say something ...");
            dialogServiceConnector.listenOnceAsync().get();
            log.info("DialogServiceConnector is listening...");

            // Start a tak to ait until a response is received from the bot.
            executorService.submit(() ->
            {
                while (receivedResponse == null) {
                    try {
                        Thread.sleep(WAIT_INTERVAL_IN_MILLIS);
                    } catch (InterruptedException e) {
                        log.error("Received interrupted exception", e);
                        return false;
                    }
                }
                return receivedResponse;
            }).get(DEFAULT_TIMEOUT_FOR_BOT_RESPONSE_IN_SECONDS, TimeUnit.SECONDS);

            // Did not receive response
            if (!receivedResponse) {
                log.error("Did not receive any response fom bot.");
            }
        } catch (TimeoutException ex) {
            log.error("Timeout exception received. Try increasing the time limit set in variable DEFAULT_TIMEOUT_FOR_BOT_RESPONSE_IN_SECONDS.", ex);
        } catch (Exception e) {
            log.error("Exception thrown when connecting to DialogServiceConnector. ErrorMessage:", e.getMessage(), e);
        } finally {
            log.info("Closing connection.");
            dialogServiceConnector.close();
            executorService.shutdownNow();
        }
    }

    private static void registerEventListeners(final DialogServiceConnector dialogServiceConnector) {
        // Recognizing will provide the intermediate recognized text while an audio stream is being processed
        dialogServiceConnector.recognizing.addEventListener((o, speechRecognitionResultEventArgs) -> {
            log.info("Recognizing speech event text: {}", speechRecognitionResultEventArgs.getResult().getText());
        });

        // Recognized will provide the final recognized text once audio capture is completed
        dialogServiceConnector.recognized.addEventListener((o, speechRecognitionResultEventArgs) -> {
            if (speechRecognitionResultEventArgs.getResult().getText().trim().equals("")) {
                log.warn("No speech was recognized. Try running the program again.");
                // Set receive response to false;
                receivedResponse = false;
            } else {
                log.info("Recognized speech event text: {}", speechRecognitionResultEventArgs.getResult().getText());
            }
        });

        // SessionStarted will notify when audio begins flowing to the service for a turn
        dialogServiceConnector.sessionStarted.addEventListener((o, sessionEventArgs) -> {
            log.info("Session started event. Session id: {} ", sessionEventArgs.getSessionId());
        });

        // SessionStopped will notify when a turn is complete
        dialogServiceConnector.sessionStopped.addEventListener((o, sessionEventArgs) -> {
            log.info("Session stopped event. Session id: {}", sessionEventArgs.getSessionId());
        });

        // Canceled will be signaled when a turn is aborted or experiences an error condition
        dialogServiceConnector.canceled.addEventListener((o, canceledEventArgs) -> {
            log.info("Canceled event details: {}", canceledEventArgs.getErrorDetails());
            dialogServiceConnector.disconnectAsync();
        });

        // ActivityReceived is the main way your bot will communicate with the client and uses bot framework activities
        dialogServiceConnector.activityReceived.addEventListener((o, activityEventArgs) -> {
            final String act = activityEventArgs.getActivity();
            log.info("Received activity {} audio: {}", activityEventArgs.hasAudio() ? "with" : "without", act);

            try {
                Activity activity = mapper.readValue(act, Activity.class);
                if (StringUtils.isNotBlank(activity.getText()) || StringUtils.isNotBlank(activity.getSpeak())) {
                    receivedResponse = true;
                    System.out.println(String.format("Response: \n\t Text: %s \n\t Speech: %s",
                            activity.getText(), activity.getSpeak()));
                }
            } catch (IOException e) {
                log.error("IO exception thrown when deserializing the bot response. ErrorMessage:", e.getMessage(), e);
            }
            if (activityEventArgs.hasAudio()) {
                System.out.println("Starting playback.");
                try {
                    PullAudioOutputStream response = activityEventArgs.getAudio();
                    ActivityAudioStream activityAudioStream = new ActivityAudioStream(response);

                    final ActivityAudioStream.ActivityAudioFormat audioFormat = activityAudioStream.getActivityAudioFormat();
                    final AudioFormat defaultAudioFormat = new AudioFormat(
                            AudioFormat.Encoding.PCM_SIGNED,
                            audioFormat.getSamplesPerSecond(),
                            audioFormat.getBitsPerSample(),
                            audioFormat.getChannels(),
                            audioFormat.getFrameSize(),
                            audioFormat.getSamplesPerSecond(),
                            false);
                    playStream(activityAudioStream, defaultAudioFormat);
                } catch (Exception e) {
                    log.error("Exception thrown during playback. ErrorMessage: ", e.getMessage());
                }
            }
        });
    }

    public static void playStream(final InputStream stream, final AudioFormat format) throws Exception {
        int bufferSize = format.getFrameSize();
        final byte[] data = new byte[bufferSize];

        SourceDataLine.Info info = new DataLine.Info(SourceDataLine.class, format);
        SourceDataLine line = (SourceDataLine) AudioSystem.getLine(info);
        line.open(format);

        if (line != null) {
            line.start();
            int nBytesRead = 0;
            while (nBytesRead != -1) {
                nBytesRead = stream.read(data);
                if (nBytesRead != -1) {
                    line.write(data, 0, nBytesRead);
                }
            }
            line.drain();
            line.stop();
            line.close();
        }
        stream.close();
    }
}
// </code>
