package tests.endtoend;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import com.microsoft.cognitiveservices.speech.audio.AudioConfig;
import com.microsoft.cognitiveservices.speech.dialog.BotConnectorActivity;
import com.microsoft.cognitiveservices.speech.dialog.BotConnectorConfig;
import com.microsoft.cognitiveservices.speech.dialog.SpeechBotConnector;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Ignore;
import org.junit.Test;
import tests.Settings;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Paths;

import static java.nio.charset.StandardCharsets.UTF_8;
import static org.junit.Assert.*;
import static tests.Settings.*;

public class VirtualAssistantTests {

    public static final String COMMUNICATION_TYPE_STRING = "Conversation_Communication_Type";
    public static final String AUTO_REPLY_CONNECTION_TYPE = "AutoReply";
    public static final String EN_US = "en-US";
    private SpeechBotConnector botConnector;
    private EventRecord eventRecord;

    @BeforeClass
    public static void setupBeforeClass() {
        // Override inputs, if necessary
        Settings.LoadSettings();
    }

    //@Before
    public void setUp() {
        // Bot secret is not required when using AutoReply connection type
        final BotConnectorConfig botConnectorConfig = BotConnectorConfig.fromSecretKey(SpeechChannelSecretForVirtualAssistant, SpeechSubscriptionKeyForVirtualAssistant, SpeechRegionForVirtualAssistant);
        botConnectorConfig.setProperty(COMMUNICATION_TYPE_STRING, AUTO_REPLY_CONNECTION_TYPE);
        botConnectorConfig.setSpeechRecognitionLanguage(EN_US);

        // For tests we are using the wav file. For manual testing use fromDefaultMicrophone.
        final AudioConfig audioConfig = AudioConfig.fromWavFileInput(WavFile);

        botConnector = new SpeechBotConnector(botConnectorConfig, audioConfig);
        registerEventHandlers(botConnector);
        eventRecord = new EventRecord();
    }

    @Ignore
    @Test
    public void testSendActivity() throws IOException {
        assertNotNull(botConnector);
        try {
            final String activity = new String(Files.readAllBytes(Paths.get(SerializedSpeechActivityFile)), UTF_8);
            // Connect to the bot
            botConnector.connectAsync();
            botConnector.sendActivityAsync(BotConnectorActivity.fromSerializedActivity(activity));

            // Add a sleep for responses to arrive.
            for (int n = 1; n <= 20; n++) {
                try {
                    assertTrue(eventRecord.activityEventReceived);
                    assertTrue(eventRecord.eventWithAudioReceived);
                    assertFalse(eventRecord.cancelledEventReceived);
                } catch (AssertionError e) {
                    if (n == 20) {
                        throw e;
                    }
                    try {
                        Thread.sleep(1000 * n);
                    } catch (InterruptedException ignored) {
                    }
                }
            }
        } finally {
            // disconnect bot.
            botConnector.disconnectAsync();
        }
    }

    @Ignore
    @Test
    public void testListenOnce() {
        try {
            // Connect to the bot
            botConnector.connectAsync();
            // Start listening.
            botConnector.listenOnceAsync();
            // Add a sleep for responses to arrive.
            for (int n = 1; n <= 20; n++) {
                try {
                    assertTrue(eventRecord.recognizedEventReceived);
                    assertTrue(eventRecord.sessionStartedEventReceived);
                    assertTrue(eventRecord.sessionStoppedEventReceived);
                    assertTrue(eventRecord.activityEventReceived);
                    assertFalse(eventRecord.cancelledEventReceived);
                } catch (AssertionError e) {
                    if (n == 20) {
                        throw e;
                    }
                    try {
                        Thread.sleep(1000 * n);
                    } catch (InterruptedException ignored) {
                    }
                }
            }

        } finally {
            // disconnect bot.
            botConnector.disconnectAsync();
        }
    }

    private void registerEventHandlers(final SpeechBotConnector botConnector) {

        botConnector.recognized.addEventListener((o, speechRecognitionResultEventArgs) -> {
            eventRecord.recognizedEventReceived = true;
        });

        botConnector.sessionStarted.addEventListener((o, sessionEventArgs) -> {
            eventRecord.sessionStartedEventReceived = true;
        });

        botConnector.sessionStopped.addEventListener((o, sessionEventArgs) -> {
            eventRecord.sessionStoppedEventReceived = true;
        });

        botConnector.canceled.addEventListener((o, canceledEventArgs) -> {
            eventRecord.cancelledEventReceived = true;
        });

        botConnector.activityReceived.addEventListener((o, activityEventArgs) -> {
            eventRecord.activityEventReceived = true;
            eventRecord.eventWithAudioReceived = activityEventArgs.hasAudio();
        });
    }

    private static class EventRecord {
        boolean sessionStartedEventReceived;
        boolean sessionStoppedEventReceived;
        boolean recognizedEventReceived;
        boolean cancelledEventReceived;
        boolean activityEventReceived;
        boolean eventWithAudioReceived;
    }
}
