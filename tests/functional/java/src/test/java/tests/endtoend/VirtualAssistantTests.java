//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

package tests.endtoend;

import com.microsoft.cognitiveservices.speech.audio.AudioConfig;
import com.microsoft.cognitiveservices.speech.dialog.Activity;
import com.microsoft.cognitiveservices.speech.dialog.DialogConfig;
import com.microsoft.cognitiveservices.speech.dialog.DialogConnector;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import tests.Settings;

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStreamReader;

import static org.junit.Assert.*;
import static tests.Settings.*;

public class VirtualAssistantTests {

    public static final String COMMUNICATION_TYPE_STRING = "Conversation_Communication_Type";
    public static final String AUTO_REPLY_CONNECTION_TYPE = "AutoReply";
    public static final String EN_US = "en-US";
    public static final int TIMEOUT_IN_MILLIS = 1000;
    public static final int RETRY_COUNT = 20;

    private DialogConnector dialogConnector;
    private EventRecord eventRecord;

    private static String readFileAsString(String path) throws IOException
    {
        BufferedReader br = new BufferedReader(new InputStreamReader(new FileInputStream(path), "UTF-8"));
        String result = "";
        String line;

        while ((line = br.readLine()) != null)
        {
            result += line;
        }
        br.close();
        return result;
    }

    @BeforeClass
    public static void setupBeforeClass() {
        Settings.LoadSettings();
    }

    @Before
    public void setUp() {
        // Bot secret is not required when using AutoReply connection type
        final DialogConfig dialogConfig = DialogConfig.fromBotSecret(SpeechChannelSecretForVirtualAssistant, SpeechSubscriptionKeyForVirtualAssistant, SpeechRegionForVirtualAssistant);
        dialogConfig.setProperty(COMMUNICATION_TYPE_STRING, AUTO_REPLY_CONNECTION_TYPE);
        dialogConfig.setSpeechRecognitionLanguage(EN_US);

        // For tests we are using the wav file. For manual testing use fromDefaultMicrophone.
        final AudioConfig audioConfig = AudioConfig.fromWavFileInput(WavFile);

        dialogConnector = new DialogConnector(dialogConfig, audioConfig);
        registerEventHandlers(dialogConnector);
        eventRecord = new EventRecord();
    }

    @Test
    public void testSendActivity() throws IOException {
        assertNotNull("dialogConnector should not be null.", dialogConnector);
        try {
            final String activity = readFileAsString(SerializedSpeechActivityFile);
            // Connect to the dialog
            dialogConnector.connectAsync();
            dialogConnector.sendActivityAsync(Activity.fromSerializedActivity(activity));

            // Add a sleep for responses to arrive.
            for (int n = 1; n <= RETRY_COUNT; n++) {
                try {
                    assertFalse(String.format("Cancelled event received. [%s]", eventRecord.cancellationErrorDetails), eventRecord.cancelledEventReceived);
                    assertTrue("Activity event not received.", eventRecord.activityEventReceived);
                    // assertTrue(eventRecord.eventWithAudioReceived); // Commenting until we fix bug id : 1780943
                } catch (AssertionError e) {
                    if (n == RETRY_COUNT) {
                        throw e;
                    }
                    try {
                        Thread.sleep(TIMEOUT_IN_MILLIS);
                    } catch (InterruptedException ignored) {
                    }
                }
            }
        } finally {
            // disconnect from the backing dialog.
            dialogConnector.disconnectAsync();
        }
    }

    @Test
    public void testListenOnce() {
        assertNotNull("dialogConnector should not be null.", dialogConnector);
        try {
            // Connect to the dialog
            dialogConnector.connectAsync();
            // Start listening.
            dialogConnector.listenOnceAsync();
            // Add a sleep for responses to arrive.
            for (int n = 1; n <= RETRY_COUNT; n++) {
                try {
                    assertFalse(String.format("Cancelled event received. [%s]", eventRecord.cancellationErrorDetails), eventRecord.cancelledEventReceived);
                    assertTrue("Recognized event not received.", eventRecord.recognizedEventReceived);
                    assertTrue("Session Started event not received.", eventRecord.sessionStartedEventReceived);
                    assertTrue("Session Stooped event not received.", eventRecord.sessionStoppedEventReceived);
                    assertTrue("Activity event not received.", eventRecord.activityEventReceived);
                } catch (AssertionError e) {
                    if (n == RETRY_COUNT) {
                        throw e;
                    }
                    try {
                        Thread.sleep(TIMEOUT_IN_MILLIS);
                    } catch (InterruptedException ignored) {
                    }
                }
            }

        } finally {
            // disconnect backing dialog.
            dialogConnector.disconnectAsync();
        }
    }

    private void registerEventHandlers(final DialogConnector dialogConnector) {

        dialogConnector.recognized.addEventListener((o, speechRecognitionResultEventArgs) -> {
            eventRecord.recognizedEventReceived = true;
        });

        dialogConnector.sessionStarted.addEventListener((o, sessionEventArgs) -> {
            eventRecord.sessionStartedEventReceived = true;
        });

        dialogConnector.sessionStopped.addEventListener((o, sessionEventArgs) -> {
            eventRecord.sessionStoppedEventReceived = true;
        });

        dialogConnector.canceled.addEventListener((o, canceledEventArgs) -> {
            eventRecord.cancelledEventReceived = true;
            eventRecord.cancellationErrorDetails = canceledEventArgs.getErrorDetails();
        });

        dialogConnector.activityReceived.addEventListener((o, activityEventArgs) -> {
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
        String cancellationErrorDetails;
    }
}
