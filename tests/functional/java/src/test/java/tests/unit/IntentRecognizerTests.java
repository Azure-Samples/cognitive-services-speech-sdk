//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

package tests.unit;

import static org.junit.Assert.*;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.Future;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.TimeoutException;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.CountDownLatch;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.Ignore;

import com.microsoft.cognitiveservices.speech.audio.AudioConfig;
import com.microsoft.cognitiveservices.speech.CancellationReason;
import com.microsoft.cognitiveservices.speech.ResultReason;
import com.microsoft.cognitiveservices.speech.Recognizer;
import com.microsoft.cognitiveservices.speech.SpeechConfig;
import com.microsoft.cognitiveservices.speech.PropertyId;
import com.microsoft.cognitiveservices.speech.intent.IntentRecognitionResult;
import com.microsoft.cognitiveservices.speech.intent.IntentRecognizer;
import com.microsoft.cognitiveservices.speech.intent.LanguageUnderstandingModel;
import com.microsoft.cognitiveservices.speech.Connection;
import com.microsoft.cognitiveservices.speech.util.EventHandler;
import com.microsoft.cognitiveservices.speech.ConnectionEventArgs;

import tests.AudioUtterancesKeys;
import tests.DefaultSettingsKeys;
import tests.Settings;
import tests.SubscriptionsRegionsKeys;
import tests.TestHelper;

public class IntentRecognizerTests {
    private final Integer FIRST_EVENT_ID = 1;
    private AtomicInteger eventIdentifier = new AtomicInteger(FIRST_EVENT_ID);

    @BeforeClass
    static public void setUpBeforeClass() throws Exception {
        Settings.LoadSettings();
    }

    // -----------------------------------------------------------------------
    // ---
    // -----------------------------------------------------------------------

    @Ignore
    @Test
    public void testDispose() {
        // TODO: make dispose method public
        fail("dispose not yet public");
    }

    // -----------------------------------------------------------------------
    // ---
    // -----------------------------------------------------------------------

    @Test
    public void testIntentRecognizer1() {
        SpeechConfig config = SpeechConfig.fromSubscription(Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Key,
            Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Region);

        assertNotNull(config);

        IntentRecognizer r = new IntentRecognizer(config, AudioConfig.fromWavFileInput(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath)));
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);

        r.close();
        config.close();
    }

    @Test
    public void testIntentRecognizer2() throws InterruptedException, ExecutionException {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.LANGUAGE_UNDERSTANDING_SUBSCRIPTION).Key,
            Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.LANGUAGE_UNDERSTANDING_SUBSCRIPTION).Region);

        assertNotNull(s);

        WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath));
        IntentRecognizer r = new IntentRecognizer(s, AudioConfig.fromStreamInput(ais));
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);

        IntentRecognitionResult res = r.recognizeOnceAsync().get();
        assertNotNull(res);
        assertEquals("What's the weather like?", res.getText());

        r.close();
        s.close();
    }

    // -----------------------------------------------------------------------
    // ---
    // -----------------------------------------------------------------------

    @Test
    public void testGetLanguage() {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Key,
            Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Region);

        s.setSpeechRecognitionLanguage("en-US");
        assertNotNull(s);

        WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath));
        assertNotNull(ais);


        IntentRecognizer r = new IntentRecognizer(s, AudioConfig.fromStreamInput(ais));
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);

        assertFalse(r.getSpeechRecognitionLanguage().isEmpty());
        assertEquals("en-US", r.getSpeechRecognitionLanguage());

        r.close();
        s.close();
    }

    // -----------------------------------------------------------------------
    // ---
    // -----------------------------------------------------------------------

    @Test
    public void testGetParameters() {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Key,
            Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Region);

        assertNotNull(s);

        IntentRecognizer r = new IntentRecognizer(s, AudioConfig.fromWavFileInput(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath)));
        assertNotNull(r);

        assertNotNull(r.getProperties());
        assertEquals(r.getSpeechRecognitionLanguage(), r.getProperties().getProperty(PropertyId.SpeechServiceConnection_RecoLanguage));

        r.close();
        s.close();
    }

    // -----------------------------------------------------------------------
    // ---
    // -----------------------------------------------------------------------

    @Test
    public void testRecognizeOnceAsync2() throws InterruptedException, ExecutionException {
        CountDownLatch countDownLatch = new CountDownLatch(1);
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.LANGUAGE_UNDERSTANDING_SUBSCRIPTION).Key,
            Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.LANGUAGE_UNDERSTANDING_SUBSCRIPTION).Region);

        assertNotNull(s);

        IntentRecognizer r = new IntentRecognizer(s, AudioConfig.fromWavFileInput(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath)));
        Connection connection = Connection.fromRecognizer(r);
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);

        final Map<String, Integer> eventsMap = new HashMap<String, Integer>();

        AtomicInteger connectedEventCount = new AtomicInteger(0);
        AtomicInteger disconnectedEventCount = new AtomicInteger(0);
        EventHandler<ConnectionEventArgs> myConnectedHandler = (o, connectionEventArgs) -> {
            connectedEventCount.getAndIncrement();
        };
        EventHandler<ConnectionEventArgs> myDisconnectedHandler = (o, connectionEventArgs) -> {
            disconnectedEventCount.getAndIncrement();
        };

        connection.connected.addEventListener(myConnectedHandler);
        connection.disconnected.addEventListener(myDisconnectedHandler);

        r.recognized.addEventListener((o, e) -> {
            TestHelper.AssertConnectionCountMatching(connectedEventCount.get(), disconnectedEventCount.get());
            eventsMap.put("recognized", eventIdentifier.getAndIncrement());
        });

        r.recognizing.addEventListener((o, e) -> {
            TestHelper.AssertConnectionCountMatching(connectedEventCount.get(), disconnectedEventCount.get());
            // take only first recogizing event
            if (eventsMap.get("recognizing") == null) {
                int now = eventIdentifier.getAndIncrement();
                eventsMap.put("recognizing-" + System.currentTimeMillis(), now);
                eventsMap.put("recognizing" , now);    
            }
        });

        r.canceled.addEventListener((o, e) -> {
            if (e.getReason() != CancellationReason.EndOfStream) {
                eventsMap.put("canceled", eventIdentifier.getAndIncrement());
            }
        });

        r.speechStartDetected.addEventListener((o, e) -> {
            int now = eventIdentifier.getAndIncrement();
            eventsMap.put("speechStartDetected-" + System.currentTimeMillis(), now);
            eventsMap.put("speechStartDetected", now);
        });

        r.speechEndDetected.addEventListener((o, e) -> {
            int now = eventIdentifier.getAndIncrement();
            eventsMap.put("speechEndDetected-" + System.currentTimeMillis(), now);
            eventsMap.put("speechEndDetected", now);
        });

        r.sessionStarted.addEventListener((o, e) -> {
            int now = eventIdentifier.getAndIncrement();
            eventsMap.put("sessionStarted-" + System.currentTimeMillis(), now);
            eventsMap.put("sessionStarted", now);
        });

        AtomicInteger sessionStoppedCount = new AtomicInteger(0);
        r.sessionStopped.addEventListener((o, e) -> {
            sessionStoppedCount.getAndIncrement();
            // Do not increment after last event
            int now = eventIdentifier.get();
            eventsMap.put("sessionStopped-" + System.currentTimeMillis(), now);
            eventsMap.put("sessionStopped", now);
            countDownLatch.countDown();
        });

        IntentRecognitionResult res = r.recognizeOnceAsync().get();
        countDownLatch.await();
        assertNotNull(res);
        assertTrue(res.getReason() != ResultReason.Canceled);
        assertEquals("What's the weather like?", res.getText());

        connection.connected.removeEventListener(myConnectedHandler);
        connection.disconnected.removeEventListener(myDisconnectedHandler);
        TestHelper.AssertConnectionCountMatching(connectedEventCount.get(), disconnectedEventCount.get());

        // session events are first and last event
        final Integer LAST_RECORDED_EVENT_ID = eventIdentifier.get();
        assertTrue(LAST_RECORDED_EVENT_ID > FIRST_EVENT_ID);
        assertEquals(FIRST_EVENT_ID, eventsMap.get("sessionStarted"));
        assertEquals(LAST_RECORDED_EVENT_ID, eventsMap.get("sessionStopped"));

        // end events come after start events.
        assertTrue(eventsMap.get("sessionStarted") < eventsMap.get("sessionStopped"));
        assertTrue(eventsMap.get("speechStartDetected") < eventsMap.get("speechEndDetected"));

        // recognition events come after session start but before session end events
        assertTrue(eventsMap.get("sessionStarted") < eventsMap.get("speechStartDetected"));
        assertTrue(eventsMap.get("speechEndDetected") < eventsMap.get("sessionStopped"));

        // expect that there is always recognized result, sometimes (very rarely ~1 out of 50 trials) there is no intermediate results, but just recognized.
        if (eventsMap.get("recognizing") != null) {
            assertTrue(eventsMap.get("recognizing") < eventsMap.get("recognized"));
        }

        // make sure events we don't expect, don't get raised
        assertFalse(eventsMap.containsKey("canceled"));

        r.close();
        s.close();
    }

    // -----------------------------------------------------------------------
    // ---
    // -----------------------------------------------------------------------
    @Test
    public void testStartContinuousRecognitionAsync() throws InterruptedException, ExecutionException, TimeoutException {
        System.out.println("Using key: " + Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Key);
        System.out.println("Using regions: " + Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Region);
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Key,
            Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Region);

        assertNotNull(s);

        IntentRecognizer r = new IntentRecognizer(s, AudioConfig.fromWavFileInput(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath)));
        assertNotNull("Failed to create IntentRecognizer", r);
        assertNotNull("RecoImpl not found", r.getRecoImpl());
        assertTrue("r is not of type Recognizer", r instanceof Recognizer);

        Future<?> future = r.startContinuousRecognitionAsync();
        assertNotNull("Failed to start continuous recognition", future);

        // Wait for max 30 seconds
        future.get(30, TimeUnit.SECONDS);

        assertFalse("Recognition was cancelled", future.isCancelled());
        assertTrue("Recognition did not complete correctly", future.isDone());

        r.close();
        s.close();
    }

    @Test
    public void testStopContinuousRecognitionAsync() throws InterruptedException, ExecutionException, TimeoutException {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Key,
            Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Region);

        assertNotNull(s);

        IntentRecognizer r = new IntentRecognizer(s, AudioConfig.fromWavFileInput(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath)));
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);

        Future<?> future = r.startContinuousRecognitionAsync();
        assertNotNull(future);

        // Wait for max 30 seconds
        future.get(30, TimeUnit.SECONDS);

        assertFalse(future.isCancelled());
        assertTrue(future.isDone());

        // just wait one second
        Thread.sleep(1000);

        future = r.stopContinuousRecognitionAsync();
        assertNotNull(future);

        // Wait for max 30 seconds
        future.get(30, TimeUnit.SECONDS);

        assertFalse(future.isCancelled());
        assertTrue(future.isDone());

        r.close();
        s.close();
    }

    @Test
    public void testStartStopContinuousRecognitionAsync() throws InterruptedException, ExecutionException, TimeoutException {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.LANGUAGE_UNDERSTANDING_SUBSCRIPTION).Key,
        Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.LANGUAGE_UNDERSTANDING_SUBSCRIPTION).Region);
        assertNotNull(s);

        IntentRecognizer r = new IntentRecognizer(s, AudioConfig.fromWavFileInput(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath)));
        Connection connection = Connection.fromRecognizer(r);
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);

        final ArrayList<String> rEvents = new ArrayList<>();

        AtomicInteger connectedEventCount = new AtomicInteger(0);
        AtomicInteger disconnectedEventCount = new AtomicInteger(0);
        EventHandler<ConnectionEventArgs> myConnectedHandler = (o, connectionEventArgs) -> {
            connectedEventCount.getAndIncrement();
        };
        EventHandler<ConnectionEventArgs> myDisconnectedHandler = (o, connectionEventArgs) -> {
            disconnectedEventCount.getAndIncrement();
        };

        connection.connected.addEventListener(myConnectedHandler);
        connection.disconnected.addEventListener(myDisconnectedHandler);

        r.recognized.addEventListener((o, e) -> {
            rEvents.add("Result@" + System.currentTimeMillis());
        });

        Future<?> future = r.startContinuousRecognitionAsync();
        assertNotNull(future);

        // Wait for max 30 seconds
        future.get(30, TimeUnit.SECONDS);

        assertFalse(future.isCancelled());
        assertTrue(future.isDone());

        // wait until we get at least on final result
        long now = System.currentTimeMillis();
        while(((System.currentTimeMillis() - now) < 30000) &&
                (rEvents.isEmpty())) {
            Thread.sleep(200);
        }

        connection.connected.removeEventListener(myConnectedHandler);
        connection.disconnected.removeEventListener(myDisconnectedHandler);

        // test that we got atleast one recognized event
        assertTrue(rEvents.size() >= 1);

        future = r.stopContinuousRecognitionAsync();
        assertNotNull(future);

        // Wait for max 30 seconds
        future.get(30, TimeUnit.SECONDS);

        assertFalse(future.isCancelled());
        assertTrue(future.isDone());

        TestHelper.AssertConnectionCountMatching(connectedEventCount.get(), disconnectedEventCount.get());

        r.close();
        s.close();
    }

    // -----------------------------------------------------------------------
    // ---
    // -----------------------------------------------------------------------

    @Test
    public void testAddIntentStringString() throws InterruptedException, ExecutionException {
        CountDownLatch countDownLatch = new CountDownLatch(1);
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.LANGUAGE_UNDERSTANDING_SUBSCRIPTION).Key,
            Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.LANGUAGE_UNDERSTANDING_SUBSCRIPTION).Region);

        assertNotNull(s);

        IntentRecognizer r = new IntentRecognizer(s, AudioConfig.fromWavFileInput(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.INTENT_UTTERANCE).FilePath)));
        assertNotNull(r);

        r.addIntent("Turn on the lamp");
        r.addIntent("Turn off the lamp");

        final Map<String, Integer> eventsMap = new HashMap<String, Integer>();

        r.recognized.addEventListener((o, e) -> {
            eventsMap.put("recognized", eventIdentifier.getAndIncrement());
            countDownLatch.countDown();
        });

        IntentRecognitionResult res = r.recognizeOnceAsync().get();
        countDownLatch.await();
        assertNotNull(res);
        assertEquals(1, eventsMap.size());
        assertTrue(ResultReason.RecognizedIntent == res.getReason());
        r.close();
        s.close();
    }

    @Test
    public void testAddIntentStringLanguageUnderstandingModelStringFromAppId() throws InterruptedException, ExecutionException {
        CountDownLatch countDownLatch = new CountDownLatch(1);
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.LANGUAGE_UNDERSTANDING_SUBSCRIPTION).Key,
            Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.LANGUAGE_UNDERSTANDING_SUBSCRIPTION).Region);

        assertNotNull(s);

        IntentRecognizer r = new IntentRecognizer(s, AudioConfig.fromWavFileInput(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.INTENT_UTTERANCE).FilePath)));
        assertNotNull(r);

        LanguageUnderstandingModel model = LanguageUnderstandingModel.fromSubscription(Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.LANGUAGE_UNDERSTANDING_SUBSCRIPTION).Key,
            Settings.DefaultSettingsMap.get(DefaultSettingsKeys.LANGUAGE_UNDERSTANDING_HOME_AUTOMATION_APP_ID),
            Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.LANGUAGE_UNDERSTANDING_SUBSCRIPTION).Region);

        assertNotNull(model);

        r.addIntent(model, "HomeAutomation.TurnOff", "off");
        r.addIntent(model, "HomeAutomation.TurnOn", "on");

        final Map<String, Integer> eventsMap = new HashMap<String, Integer>();

        r.recognized.addEventListener((o, e) -> {
            eventsMap.put("recognized", eventIdentifier.getAndIncrement());
            if(!e.getResult().getProperties().getProperty(PropertyId.LanguageUnderstandingServiceResponse_JsonResult).isEmpty()) {
                eventsMap.put("IntentReceived", eventIdentifier.getAndIncrement());
            }
            countDownLatch.countDown();
        });

        IntentRecognitionResult res = r.recognizeOnceAsync().get();
        countDownLatch.await();
        assertNotNull(res);
        assertEquals(2, eventsMap.size());
        assertTrue(res.getProperties().getProperty(PropertyId.LanguageUnderstandingServiceResponse_JsonResult).length() > 0);
        assertTrue(ResultReason.RecognizedIntent == res.getReason());
        r.close();
        s.close();
    }

    // -----------------------------------------------------------------------
    // ---
    // -----------------------------------------------------------------------

    @Test
    public void testGetRecoImpl() {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Key,
            Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Region);

        assertNotNull(s);

        IntentRecognizer r = new IntentRecognizer(s, AudioConfig.fromWavFileInput(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath)));
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);

        r.close();
        s.close();
    }

     @Test
    public void testExceptionsDuringEventsRecognizeOnce() throws InterruptedException, ExecutionException {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.LANGUAGE_UNDERSTANDING_SUBSCRIPTION).Key,
            Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.LANGUAGE_UNDERSTANDING_SUBSCRIPTION).Region);

        assertNotNull(s);

        IntentRecognizer r = new IntentRecognizer(s, AudioConfig.fromWavFileInput(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath)));
        Connection connection = Connection.fromRecognizer(r);
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);

       AtomicBoolean sessionStopped = new AtomicBoolean(false);

        connection.connected.addEventListener((o, connectionEventArgs) -> {
            System.out.println("Now Connected. preparing throw exception");
            throw new IllegalArgumentException("Connected");
        });

        connection.disconnected.addEventListener((o, connectionEventArgs) -> {
            System.out.println("Now Disconnected. preparing throw exception");
            throw new IllegalArgumentException("disconnected");
        });

        r.recognized.addEventListener((o, e) -> {
            System.out.println("Now Recognized. preparing throw exception");
            throw new IllegalArgumentException("Recognized");
        });

        r.recognizing.addEventListener((o, e) -> {
            System.out.println("Now recognizing. preparing throw exception");
            throw new IllegalArgumentException("recognizing");
        });

        r.canceled.addEventListener((o, e) -> {
            System.out.println("Now Canceled. preparing throw exception");
            throw new IllegalArgumentException("canceled");
        });

        r.speechStartDetected.addEventListener((o, e) -> {
            System.out.println("Now SpeechStartDetected. preparing throw exception");
            throw new IllegalArgumentException("SpeechStartDetected");
        });

        r.speechEndDetected.addEventListener((o, e) -> {
            System.out.println("Now speechEndDetected. preparing throw exception");
            throw new IllegalArgumentException("speechEndDetected");
        });

        r.sessionStarted.addEventListener((o, e) -> {
            System.out.println("Now sessionStarted. preparing throw exception");
            throw new IllegalArgumentException("sessionStarted");
        });

        r.sessionStopped.addEventListener((o, e) -> {
            sessionStopped.set(true);
            System.out.println("Now SessionStopped. preparing throw exception");
            throw new IllegalArgumentException("sessionStopped");
        });

        IntentRecognitionResult res = r.recognizeOnceAsync().get();
        assertNotNull(res);
        TestHelper.OutputResult(res);
        assertEquals(ResultReason.RecognizedSpeech, res.getReason());
        assertEquals("What's the weather like?", res.getText());

        // wait until we get the SessionStopped event.
        long now = System.currentTimeMillis();
        while(((System.currentTimeMillis() - now) < 30000) && (sessionStopped.get() == true)) {
            Thread.sleep(200);
        }
        
        r.close();
        s.close();
    }

    @Test
    public void testExceptionsDuringEventsContinuousRecognition() throws InterruptedException, ExecutionException {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.LANGUAGE_UNDERSTANDING_SUBSCRIPTION).Key,
            Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.LANGUAGE_UNDERSTANDING_SUBSCRIPTION).Region);
            
        assertNotNull(s);

        IntentRecognizer r = new IntentRecognizer(s, AudioConfig.fromWavFileInput(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath)));
        Connection connection = Connection.fromRecognizer(r);
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);

        AtomicBoolean sessionStopped = new AtomicBoolean(false);

        connection.connected.addEventListener((o, connectionEventArgs) -> {
            System.out.println("Now Connected. preparing throw exception");
            throw new IllegalArgumentException("Connected");
        });

        connection.disconnected.addEventListener((o, connectionEventArgs) -> {
            System.out.println("Now Disconnected. preparing throw exception");
            throw new IllegalArgumentException("disconnected");
        });

        r.recognized.addEventListener((o, e) -> {
            System.out.println("Now Recognized. preparing throw exception");
            throw new IllegalArgumentException("Recognized");
        });

        r.recognizing.addEventListener((o, e) -> {
            System.out.println("Now recognizing. preparing throw exception");
            throw new IllegalArgumentException("recognizing");
        });

        r.canceled.addEventListener((o, e) -> {
            System.out.println("Now Canceled. preparing throw exception");
            throw new IllegalArgumentException("canceled");
        });

        r.speechStartDetected.addEventListener((o, e) -> {
            System.out.println("Now SpeechStartDetected. preparing throw exception");
            throw new IllegalArgumentException("SpeechStartDetected");
        });

        r.speechEndDetected.addEventListener((o, e) -> {
            System.out.println("Now speechEndDetected. preparing throw exception");
            throw new IllegalArgumentException("speechEndDetected");
        });

        r.sessionStarted.addEventListener((o, e) -> {
            System.out.println("Now sessionStarted. preparing throw exception");
            throw new IllegalArgumentException("sessionStarted");
        });

        r.sessionStopped.addEventListener((o, e) -> {
            sessionStopped.set(true);
            System.out.println("Now SessionStopped. preparing throw exception");
            throw new IllegalArgumentException("sessionStopped");
        });

        r.startContinuousRecognitionAsync().get();
        
        // wait until we get the SessionStopped event.
        long now = System.currentTimeMillis();
        while(((System.currentTimeMillis() - now) < 30000) && (sessionStopped.get() == true)) {
            Thread.sleep(200);
        }

        r.stopContinuousRecognitionAsync().get();

        r.close();
        s.close();
    }
}
