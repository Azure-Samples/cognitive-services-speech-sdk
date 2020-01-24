//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

package tests.endtoend;

import com.google.gson.JsonIOException;
import com.google.gson.JsonSyntaxException;
import com.microsoft.cognitiveservices.speech.PropertyId;
import com.microsoft.cognitiveservices.speech.audio.AudioConfig;
import com.microsoft.cognitiveservices.speech.dialog.DialogServiceConfig;
import com.microsoft.cognitiveservices.speech.dialog.BotFrameworkConfig;
import com.microsoft.cognitiveservices.speech.dialog.DialogServiceConnector;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

import tests.AudioUtterancesKeys;
import tests.DefaultSettingsKeys;
import tests.Settings;
import tests.SubscriptionsRegionsKeys;

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
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

    private DialogServiceConnector dialogServiceConnector;
    private EventRecord eventRecord;

    private static String readFileAsString(String path) throws IOException {
        BufferedReader br = new BufferedReader(new InputStreamReader(new FileInputStream(path), "UTF-8"));
        String result = "";
        String line;

        while ((line = br.readLine()) != null) {
            result += line;
        }
        br.close();
        return result;
    }

    @BeforeClass
    public static void setupBeforeClass() {
        String operatingSystem = ("" + System.getProperty("os.name")).toLowerCase();
        System.out.println("Current operation system: " + operatingSystem);
        boolean isMac = operatingSystem.contains("mac") || operatingSystem.contains("darwin");
        org.junit.Assume.assumeFalse(isMac);
        try {
            Settings.LoadSettings();
        } catch (JsonIOException | JsonSyntaxException | FileNotFoundException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
    }

    @Before
    public void setUp() {
        // Bot secret is not required when using AutoReply connection type
        final DialogServiceConfig dialogServiceConfig = BotFrameworkConfig.fromSubscription(SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.DIALOG_SUBSCRIPTION).Key, 
            SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.DIALOG_SUBSCRIPTION).Region);

        dialogServiceConfig.setProperty(PropertyId.Conversation_ApplicationId, DefaultSettingsMap.get(DefaultSettingsKeys.DIALOG_FUNCTIONAL_TEST_BOT));
        dialogServiceConfig.setProperty(COMMUNICATION_TYPE_STRING, AUTO_REPLY_CONNECTION_TYPE);
        dialogServiceConfig.setLanguage(EN_US);

        // For tests we are using the wav file. For manual testing use fromDefaultMicrophone.
        final AudioConfig audioConfig = AudioConfig.fromWavFileInput(Settings.GetRootRelativePath(AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath));

        dialogServiceConnector = new DialogServiceConnector(dialogServiceConfig, audioConfig);
        registerEventHandlers(dialogServiceConnector);
        eventRecord = new EventRecord();
    }

    @Test
    public void testSendActivity() throws IOException {
        assertNotNull("dialogServiceConnector should not be null.", dialogServiceConnector);
        try {
            String inputFile = Settings.GetRootRelativePath(Settings.DefaultSettingsMap.get(DefaultSettingsKeys.SERIALIZED_SPEECH_ACTIVITY_FILE));
            final String activity = readFileAsString(inputFile);
            // Connect to the dialog
            dialogServiceConnector.connectAsync();
            dialogServiceConnector.sendActivityAsync(activity);

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
            dialogServiceConnector.close();
        }
    }

    @Test
    public void testListenOnce() {
        assertNotNull("dialogServiceConnector should not be null.", dialogServiceConnector);
        try {
            // Connect to the dialog
            dialogServiceConnector.connectAsync();
            // Start listening.
            dialogServiceConnector.listenOnceAsync();
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
            dialogServiceConnector.close();
        }
    }

    private void registerEventHandlers(final DialogServiceConnector dialogServiceConnector) {

        dialogServiceConnector.recognized.addEventListener((o, speechRecognitionResultEventArgs) -> {
            eventRecord.recognizedEventReceived = true;
        });

        dialogServiceConnector.sessionStarted.addEventListener((o, sessionEventArgs) -> {
            eventRecord.sessionStartedEventReceived = true;
        });

        dialogServiceConnector.sessionStopped.addEventListener((o, sessionEventArgs) -> {
            eventRecord.sessionStoppedEventReceived = true;
        });

        dialogServiceConnector.canceled.addEventListener((o, canceledEventArgs) -> {
            eventRecord.cancelledEventReceived = true;
            eventRecord.cancellationErrorDetails = canceledEventArgs.getErrorDetails();
        });

        dialogServiceConnector.activityReceived.addEventListener((o, activityEventArgs) -> {
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
