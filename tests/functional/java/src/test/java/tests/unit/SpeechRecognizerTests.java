//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

package tests.unit;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.Future;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.TimeoutException;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.atomic.AtomicBoolean;
import java.net.URI;

import static org.junit.Assert.*;

import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.Ignore;
import org.junit.Rule;
import org.junit.rules.ExpectedException;

import com.microsoft.cognitiveservices.speech.audio.AudioConfig;
import com.microsoft.cognitiveservices.speech.CancellationReason;
import com.microsoft.cognitiveservices.speech.ResultReason;
import com.microsoft.cognitiveservices.speech.ServicePropertyChannel;
import com.microsoft.cognitiveservices.speech.Recognizer;
import com.microsoft.cognitiveservices.speech.SpeechConfig;
import com.microsoft.cognitiveservices.speech.SpeechRecognitionResult;
import com.microsoft.cognitiveservices.speech.SpeechRecognizer;
import com.microsoft.cognitiveservices.speech.PropertyId;
import com.microsoft.cognitiveservices.speech.OutputFormat;
import com.microsoft.cognitiveservices.speech.Connection;
import com.microsoft.cognitiveservices.speech.ProfanityOption;
import com.microsoft.cognitiveservices.speech.SourceLanguageConfig;
import com.microsoft.cognitiveservices.speech.AutoDetectSourceLanguageConfig;
import com.microsoft.cognitiveservices.speech.AutoDetectSourceLanguageResult;

import tests.AudioUtterancesKeys;
import tests.Settings;
import tests.SubscriptionsRegionsKeys;
import tests.TestHelper;

public class SpeechRecognizerTests {
    private final Integer FIRST_EVENT_ID = 1;
    private static String authorizationToken;

    @BeforeClass
    static public void setUpBeforeClass() throws Exception {
        Settings.LoadSettings();
        authorizationToken = TestHelper.getAuthorizationToken(Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Key,
            Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Region);
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
    public void testSpeechRecognizer1() throws InterruptedException, ExecutionException {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Key,
            Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Region);

        assertNotNull(s);

        SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath)));
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);

        r.close();
        s.close();
    }

    @Test
    public void testSpeechRecognizer2() throws InterruptedException, ExecutionException {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Key,
            Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Region);

        assertNotNull(s);

        WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath));
        assertNotNull(ais);

        SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromStreamInput(ais));
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);

        SpeechRecognitionResult res = r.recognizeOnceAsync().get();
        assertNotNull(res);
        TestHelper.OutputResult(res);
        assertEquals("What's the weather like?", res.getText());

        r.close();
        s.close();
    }

    @Test
    public void testSpeechRecognizerInterleaved() throws InterruptedException, ExecutionException {

        //
        // Start a legal request
        //

        SpeechConfig s2 = SpeechConfig.fromSubscription(Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Key,
            Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Region);

        assertNotNull(s2);

        WavFileAudioInputStream ais2 = new WavFileAudioInputStream(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath));
        assertNotNull(ais2);

        AudioConfig ac2 = AudioConfig.fromStreamInput(ais2);
        assertNotNull(ac2);

        //
        // Start: make an illegal request
        //

        SpeechConfig s = SpeechConfig.fromSubscription("illegal", "illegal");
        assertNotNull(s);

        WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath));
        assertNotNull(ais);

        AudioConfig ac = AudioConfig.fromStreamInput(ais);
        assertNotNull(ac);

        SpeechRecognizer r = new SpeechRecognizer(s, ac);
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);

        SpeechRecognitionResult res = r.recognizeOnceAsync().get();
        assertNotNull(res);
        assertTrue(res.getReason() == ResultReason.Canceled);

        r.close();
        s.close();

        //
        // End: make an illegal request
        //

        //
        // Continue legal request
        //

        SpeechRecognizer r2 = new SpeechRecognizer(s2, ac2);
        assertNotNull(r2);
        assertNotNull(r2.getRecoImpl());
        assertTrue(r2 instanceof Recognizer);

        SpeechRecognitionResult res2 = r2.recognizeOnceAsync().get();
        assertNotNull(res2);
        TestHelper.OutputResult(res2);
        assertEquals("What's the weather like?", res2.getText());

        r2.close();
        s2.close();
    }

    // -----------------------------------------------------------------------
    // ---
    // -----------------------------------------------------------------------

    @Test
    public void testGetEndpointId() {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Key,
            Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Region);

        assertNotNull(s);

        WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath));
        assertNotNull(ais);

        SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromStreamInput(ais));
        assertNotNull(r);

        assertNotNull(r.getEndpointId());

        r.close();
        s.close();
    }

    @Test
    public void testSetEndpointId() {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Key,
            Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Region);

        assertNotNull(s);
        s.setEndpointId("newEndpointId");

        WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath));
        assertNotNull(ais);

        SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromStreamInput(ais));
        assertNotNull(r);
        assertEquals("incorrect endpoint id:" + r.getEndpointId(), "newEndpointId", r.getEndpointId());

        r.close();
        s.close();
    }

    // -----------------------------------------------------------------------
    // ---
    // -----------------------------------------------------------------------

    @Test
    @Ignore("test is crashing and terminating the test harness.")
    public void testGetLanguage1() {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Key,
            Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Region);

        assertNotNull(s);

        WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath));
        assertNotNull(ais);

        SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromStreamInput(ais));
        assertNotNull(r);
        assertNotNull(r.getSpeechRecognitionLanguage());

        r.close();
        s.close();
    }

    @Test
    public void testGetLanguage2() {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Key,
            Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Region);

        assertNotNull(s);

        WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath));
        assertNotNull(ais);

        String language = "de-DE";
        s.setSpeechRecognitionLanguage(language);
        SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromStreamInput(ais));
        assertNotNull(r);
        assertNotNull(r.getSpeechRecognitionLanguage());
        assertEquals(language, r.getSpeechRecognitionLanguage());

        r.close();
        s.close();
    }

    // -----------------------------------------------------------------------
    // ---
    // -----------------------------------------------------------------------

    @Test
    public void testGetOutputFormatDefault() {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Key,
            Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Region);

        assertNotNull(s);

        SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath)));
        assertNotNull(r);
        assertEquals(r.getOutputFormat(), OutputFormat.Simple);

        r.close();
        s.close();
    }

    @Test
    public void testGetOutputFormatDetailed() {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Key,
            Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Region);

        assertNotNull(s);

        String language = "de-DE";
        s.setSpeechRecognitionLanguage(language);
        s.setOutputFormat(OutputFormat.Detailed);
        SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath)));
        assertNotNull(r);
        assertEquals(r.getOutputFormat(), OutputFormat.Detailed);

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

        SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath)));
        assertNotNull(r);
        assertNotNull(r.getProperties());
        assertEquals(r.getSpeechRecognitionLanguage(), r.getProperties().getProperty(PropertyId.SpeechServiceConnection_RecoLanguage));
        assertEquals(r.getEndpointId(), r.getProperties().getProperty(PropertyId.SpeechServiceConnection_EndpointId)); // TODO: is this really the correct mapping?

        r.close();
        s.close();
    }

    // -----------------------------------------------------------------------
    // ---
    // -----------------------------------------------------------------------

    @Test
    public void testRecognizeOnceAsyncWithPreConnection() throws InterruptedException, ExecutionException, TimeoutException {
        testRecognizeOnceAsync1(true);
    }

    @Test
    public void testRecognizeAsyncWithoutPreConnection() throws InterruptedException, ExecutionException, TimeoutException {
        testRecognizeOnceAsync1(false);
    }

    @Test
    public void testRecognizeOnceAsyncWithLateSubscribePreConnection() throws InterruptedException, ExecutionException, TimeoutException {
        testRecognizeOnceAsyncWithLaterSubscribe1(true);
    }

    @Test
    public void testRecognizeAsyncWithoutWithLateSubscribePreConnection() throws InterruptedException, ExecutionException, TimeoutException {
        testRecognizeOnceAsyncWithLaterSubscribe1(false);
    }

    public void testRecognizeOnceAsync1(boolean usingPreConnection) throws InterruptedException, ExecutionException, TimeoutException {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Key,
            Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Region);

        assertNotNull(s);

        SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath)));
        Connection connection = Connection.fromRecognizer(r);
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);

        if (usingPreConnection)
        {
            connection.openConnection(false);
        }
        AtomicInteger connectedEventCount = new AtomicInteger(0);
        AtomicInteger disconnectedEventCount = new AtomicInteger(0);
        AtomicInteger sessionStoppedCount = new AtomicInteger(0);
        connection.connected.addEventListener((o, connectionEventArgs) -> {
            connectedEventCount.getAndIncrement();
        });

        connection.disconnected.addEventListener((o, connectionEventArgs) -> {
            disconnectedEventCount.getAndIncrement();
        });

        r.sessionStopped.addEventListener((o, SessionEventArgs) -> {
            sessionStoppedCount.getAndIncrement();
        });

        Future<SpeechRecognitionResult> future = r.recognizeOnceAsync();
        assertNotNull("future is null.", future);

        // Wait for max 30 seconds
        future.get(30, TimeUnit.SECONDS);

        assertFalse("future is canceled.", future.isCancelled());
        assertTrue("future is not done.", future.isDone());

        SpeechRecognitionResult res = future.get();
        assertNotNull(res);
        TestHelper.OutputResult(res);
        assertEquals(ResultReason.RecognizedSpeech, res.getReason());
        assertEquals("What's the weather like?", res.getText());

        // wait until we get the SessionStopped event.
        long now = System.currentTimeMillis();
        while(((System.currentTimeMillis() - now) < 30000) && (sessionStoppedCount.get() == 0)) {
            Thread.sleep(200);
        }

        connection.close();
        r.close();
        s.close();
        TestHelper.AssertConnectionCountMatching(connectedEventCount.get(), disconnectedEventCount.get());
    }

    public void testRecognizeOnceAsyncWithLaterSubscribe1(boolean usingPreConnection) throws InterruptedException, ExecutionException, TimeoutException {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Key,
            Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Region);

        assertNotNull(s);

        SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath)));
        Connection connection = Connection.fromRecognizer(r);
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);

        if (usingPreConnection)
        {
            connection.openConnection(false);
        }
        AtomicInteger connectedEventCount = new AtomicInteger(0);
        AtomicInteger disconnectedEventCount = new AtomicInteger(0);
        AtomicInteger sessionStoppedCount = new AtomicInteger(0);
        connection.connected.addEventListener((o, connectionEventArgs) -> {
            connectedEventCount.getAndIncrement();

            connection.disconnected.addEventListener((o2, e) -> {
                disconnectedEventCount.getAndIncrement();
            });

            r.sessionStopped.addEventListener((o2, e) -> {
                sessionStoppedCount.getAndIncrement();
            });
        });

        Future<SpeechRecognitionResult> future = r.recognizeOnceAsync();
        // Wait for max 30 seconds
        future.get(30, TimeUnit.SECONDS);

        SpeechRecognitionResult res = future.get();
        assertNotNull(res);
        TestHelper.OutputResult(res);
        assertEquals(ResultReason.RecognizedSpeech, res.getReason());
        assertEquals("What's the weather like?", res.getText());

        // wait until we get the SessionStopped event.
        long now = System.currentTimeMillis();
        while(((System.currentTimeMillis() - now) < 30000) && (sessionStoppedCount.get() == 0)) {
            Thread.sleep(200);
        }

        connection.closeConnection();
        TestHelper.AssertConnectionCountMatching(connectedEventCount.get(), disconnectedEventCount.get());
        connection.close();
        r.close();
        s.close();
    }

    @Test
    public void testRecognizeOnceAsync1Concurrent() throws InterruptedException, ExecutionException, TimeoutException {
        Thread threads[] = new Thread[4];

        final ConcurrentLinkedQueue<String> failures = new ConcurrentLinkedQueue<String>();
        final ConcurrentLinkedQueue<String> successes = new ConcurrentLinkedQueue<String>();
        for(int n=0; n<threads.length; n++) {
            threads[n] = new Thread(() -> {
                try {
                    SpeechConfig s = SpeechConfig.fromSubscription(Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Key,
                        Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Region);

                    SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath)));

                    Future<SpeechRecognitionResult> future = r.recognizeOnceAsync();

                    future.get(30, TimeUnit.SECONDS);
                    SpeechRecognitionResult res = future.get();

                    r.close();
                    s.close();

                    TestHelper.OutputResult(res);
                    if (ResultReason.RecognizedSpeech != res.getReason()) {
                        throw new IllegalArgumentException("unexpected reason");
                    }

                    if (!res.getText().equals("What's the weather like?")) {
                        throw new IllegalArgumentException("unexpected reason");
                    }

                    successes.add(Thread.currentThread().getName());
                } catch (Exception e) {
                    failures.add(e.getMessage());
                }
            });

            threads[n].setName("WorkerThreadForTestConcurrent-" + n);
        }

        for(int n=0; n<threads.length; n++) {
            threads[n].start();
        }

        for(int n=0; n<threads.length; n++) {
            threads[n].join(30000);
        }

        if(!failures.isEmpty()) {
            fail(String.join("\n", failures));
        }

        if(successes.size() != threads.length) {
            fail(String.join("\n", successes));
        }
    }

    @Test
    public void testRecognizeOnceAsync2() throws InterruptedException, ExecutionException {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Key,
            Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Region);

        assertNotNull(s);

        SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath)));
        Connection connection = Connection.fromRecognizer(r);
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);

        final Map<String, Integer> eventsMap = new HashMap<String, Integer>();
        AtomicInteger eventIdentifier = new AtomicInteger(FIRST_EVENT_ID);
        AtomicInteger connectedEventCount = new AtomicInteger(0);
        AtomicInteger disconnectedEventCount = new AtomicInteger(0);
        AtomicInteger sessionStoppedCount = new AtomicInteger(0);
        connection.connected.addEventListener((o, connectionEventArgs) -> {
            connectedEventCount.getAndIncrement();
        });

        connection.disconnected.addEventListener((o, connectionEventArgs) -> {
            disconnectedEventCount.getAndIncrement();
        });

        r.recognized.addEventListener((o, e) -> {
            eventsMap.put("recognized", eventIdentifier.getAndIncrement());
        });

        r.recognizing.addEventListener((o, e) -> {
            int seqno = eventIdentifier.getAndIncrement();
            eventsMap.put("recognizing-" + System.currentTimeMillis(), seqno);
            eventsMap.put("recognizing" , seqno);
        });

        r.canceled.addEventListener((o, e) -> {
            if (e.getReason() != CancellationReason.EndOfStream) {
                eventsMap.put("canceled", eventIdentifier.getAndIncrement());
            }
        });

        r.speechStartDetected.addEventListener((o, e) -> {
            int seqno = eventIdentifier.getAndIncrement();
            eventsMap.put("speechStartDetected-" + System.currentTimeMillis(), seqno);
            eventsMap.put("speechStartDetected", seqno);
        });

        r.speechEndDetected.addEventListener((o, e) -> {
            int seqno = eventIdentifier.getAndIncrement();
            eventsMap.put("speechEndDetected-" + System.currentTimeMillis(), seqno);
            eventsMap.put("speechEndDetected", seqno);
        });

        r.sessionStarted.addEventListener((o, e) -> {
            int seqno = eventIdentifier.getAndIncrement();
            eventsMap.put("sessionStarted-" + System.currentTimeMillis(), seqno);
            eventsMap.put("sessionStarted", seqno);
        });

        r.sessionStopped.addEventListener((o, e) -> {
            sessionStoppedCount.getAndIncrement();
            int seqno = eventIdentifier.getAndIncrement();
            eventsMap.put("sessionStopped-" + System.currentTimeMillis(), seqno);
            eventsMap.put("sessionStopped", seqno);
        });

        SpeechRecognitionResult res = r.recognizeOnceAsync().get();
        assertNotNull(res);
        TestHelper.OutputResult(res);
        assertEquals(ResultReason.RecognizedSpeech, res.getReason());
        assertEquals("What's the weather like?", res.getText());

        // wait until we get the SessionStopped event.
        long now = System.currentTimeMillis();
        while(((System.currentTimeMillis() - now) < 30000) && (sessionStoppedCount.get() == 0)) {
            Thread.sleep(200);
        }
        // It is not required to explictly close the connection. This is also used to keep the connection object alive.
        connection.closeConnection();
        TestHelper.AssertConnectionCountMatching(connectedEventCount.get(), disconnectedEventCount.get());

        // session events are first and last event
        final Integer LAST_RECORDED_EVENT_ID = eventIdentifier.get() - 1;
        System.out.println("Events received. Last recorded_event_id: "+ LAST_RECORDED_EVENT_ID);
        for (Map.Entry<String, Integer> entry : eventsMap.entrySet()) {
            System.out.println(entry.getKey() + ":" + entry.getValue());
        };
        assertTrue(LAST_RECORDED_EVENT_ID > FIRST_EVENT_ID);
        assertEquals(FIRST_EVENT_ID, eventsMap.get("sessionStarted"));
        if(eventsMap.containsKey("sessionStopped"))
            assertEquals(LAST_RECORDED_EVENT_ID, eventsMap.get("sessionStopped"));

        // end events come after start events.
        if(eventsMap.containsKey("sessionStopped"))
            assertTrue(eventsMap.get("sessionStarted") < eventsMap.get("sessionStopped"));
        assertTrue(eventsMap.get("speechStartDetected") < eventsMap.get("speechEndDetected"));
        assertEquals((Integer)(FIRST_EVENT_ID + 1), eventsMap.get("speechStartDetected"));

        // make sure, first end of speech, then final result
        assertEquals((Integer)(LAST_RECORDED_EVENT_ID - 2), eventsMap.get("speechEndDetected"));
        assertEquals((Integer)(LAST_RECORDED_EVENT_ID - 1), eventsMap.get("recognized"));

        // recognition events come after session start but before session end events
        assertTrue(eventsMap.get("sessionStarted") < eventsMap.get("speechStartDetected"));
        if(eventsMap.containsKey("sessionStopped"))
            assertTrue(eventsMap.get("speechEndDetected") < eventsMap.get("sessionStopped"));

        // there is no partial result reported after the final result
        // (and check that we have intermediate and final results recorded)
        if(eventsMap.containsKey("recognizing"))
            assertTrue(eventsMap.get("recognizing") > eventsMap.get("speechStartDetected"));
        assertTrue(eventsMap.get("speechEndDetected") < eventsMap.get("recognized"));
        assertTrue(eventsMap.get("recognizing") < eventsMap.get("recognized"));

        // make sure events we don't expect, don't get raised
        assertFalse(eventsMap.containsKey("canceled"));

        r.close();
        s.close();
    }

    @Test
    public void testRecognizeOnceAsyncWithLateSubscription2() throws InterruptedException, ExecutionException {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Key,
            Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Region);

        assertNotNull(s);

        SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath)));
        Connection connection = Connection.fromRecognizer(r);
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);

        final Map<String, Integer> eventsMap = new HashMap<String, Integer>();

        AtomicInteger eventIdentifier = new AtomicInteger(FIRST_EVENT_ID);
        AtomicInteger connectedEventCount = new AtomicInteger(0);
        AtomicInteger disconnectedEventCount = new AtomicInteger(0);
        AtomicInteger sessionStoppedCount = new AtomicInteger(0);
        connection.connected.addEventListener((o, connectionEventArgs) -> {
            connectedEventCount.getAndIncrement();
        });

        connection.disconnected.addEventListener((o, connectionEventArgs) -> {
            disconnectedEventCount.getAndIncrement();
        });

        r.recognizing.addEventListener((o, e) -> {
            int seqno = eventIdentifier.getAndIncrement();

            // on first intermediate result, register for the final result.
            if (!eventsMap.containsKey("recognizing"))
            {
                r.recognized.addEventListener((o2, e2) -> {
                    eventsMap.put("recognized", eventIdentifier.getAndIncrement());
                });

                r.canceled.addEventListener((o2, e2) -> {
                    if (e2.getReason() != CancellationReason.EndOfStream) {
                        eventsMap.put("canceled", eventIdentifier.getAndIncrement());
                    }
                });
            }

            eventsMap.put("recognizing-" + System.currentTimeMillis(), seqno);
            eventsMap.put("recognizing" , seqno);
        });

        r.speechStartDetected.addEventListener((o, e) -> {
            int seqno = eventIdentifier.getAndIncrement();
            eventsMap.put("speechStartDetected-" + System.currentTimeMillis(), seqno);
            eventsMap.put("speechStartDetected", seqno);
        });

        r.speechEndDetected.addEventListener((o, e) -> {
            int seqno = eventIdentifier.getAndIncrement();
            eventsMap.put("speechEndDetected-" + System.currentTimeMillis(), seqno);
            eventsMap.put("speechEndDetected", seqno);
        });

        r.sessionStarted.addEventListener((o, e) -> {
            int seqno = eventIdentifier.getAndIncrement();
            eventsMap.put("sessionStarted-" + System.currentTimeMillis(), seqno);
            eventsMap.put("sessionStarted", seqno);
        });

        r.sessionStopped.addEventListener((o, e) -> {
            sessionStoppedCount.getAndIncrement();
            int seqno = eventIdentifier.getAndIncrement();
            eventsMap.put("sessionStopped-" + System.currentTimeMillis(), seqno);
            eventsMap.put("sessionStopped", seqno);
        });

        SpeechRecognitionResult res = r.recognizeOnceAsync().get();

        TestHelper.OutputResult(res);
        assertEquals(ResultReason.RecognizedSpeech, res.getReason());
        assertEquals("What's the weather like?", res.getText());

        // wait until we get the SessionStopped event.
        long now = System.currentTimeMillis();
        while(((System.currentTimeMillis() - now) < 30000) && (sessionStoppedCount.get() == 0)) {
            Thread.sleep(200);
        }
        // It is not required to explictly close the connection. This is also used to keep the connection object alive.
        connection.closeConnection();
        TestHelper.AssertConnectionCountMatching(connectedEventCount.get(), disconnectedEventCount.get());

        // session events are first and last event
        final Integer LAST_RECORDED_EVENT_ID = eventIdentifier.get() - 1;

        System.out.println("Events received. Last recorded_event_id: "+ LAST_RECORDED_EVENT_ID);
        for (Map.Entry<String, Integer> entry : eventsMap.entrySet()) {
            System.out.println(entry.getKey() + ":" + entry.getValue());
        };
        assertTrue(LAST_RECORDED_EVENT_ID > FIRST_EVENT_ID);
        assertEquals(FIRST_EVENT_ID, eventsMap.get("sessionStarted"));
        if(eventsMap.containsKey("sessionStopped"))
            assertEquals(LAST_RECORDED_EVENT_ID, eventsMap.get("sessionStopped"));

        // end events come after start events.
        if(eventsMap.containsKey("sessionStopped"))
            assertTrue(eventsMap.get("sessionStarted") < eventsMap.get("sessionStopped"));
        assertTrue(eventsMap.get("speechStartDetected") < eventsMap.get("speechEndDetected"));
        assertEquals((Integer)(FIRST_EVENT_ID + 1), eventsMap.get("speechStartDetected"));

        // make sure, first end of speech, then final result
        assertEquals((Integer)(LAST_RECORDED_EVENT_ID - 2), eventsMap.get("speechEndDetected"));
        assertEquals((Integer)(LAST_RECORDED_EVENT_ID - 1), eventsMap.get("recognized"));

        // recognition events come after session start but before session end events
        assertTrue(eventsMap.get("sessionStarted") < eventsMap.get("speechStartDetected"));
        if(eventsMap.containsKey("sessionStopped"))
            assertTrue(eventsMap.get("speechEndDetected") < eventsMap.get("sessionStopped"));

        // there is no partial result reported after the final result
        // (and check that we have intermediate and final results recorded)
        if(eventsMap.containsKey("recognizing"))
            assertTrue(eventsMap.get("recognizing") > eventsMap.get("speechStartDetected"));
        assertTrue(eventsMap.get("speechEndDetected") < eventsMap.get("recognized"));
        assertTrue(eventsMap.get("recognizing") < eventsMap.get("recognized"));

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
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Key,
            Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Region);

        assertNotNull(s);

        SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath)));
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);

        Future<?> future = r.startContinuousRecognitionAsync();
        assertNotNull("future is null.", future);

        // Wait for max 30 seconds
        future.get(30, TimeUnit.SECONDS);
        assertFalse("future is canceled.", future.isCancelled());
        assertTrue("future is not done.", future.isDone());

        r.close();
        s.close();
    }

    @Test
    public void testStopContinuousRecognitionAsyncWithPreConnection() throws InterruptedException, ExecutionException, TimeoutException {
        testStopContinuousRecognitionAsync(true);
    }

    @Test
    public void testStopContinuousRecognitionAsyncWithoutPreConnection() throws InterruptedException, ExecutionException, TimeoutException {
        testStopContinuousRecognitionAsync(false);
    }

    public void testStopContinuousRecognitionAsync(boolean usingPreConnection) throws InterruptedException, ExecutionException, TimeoutException {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Key,
            Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Region);

        assertNotNull(s);

        SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath)));
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);

        AtomicInteger connectedEventCount = new AtomicInteger(0);
        Connection connection = Connection.fromRecognizer(r);
        if (usingPreConnection)
        {
            connection.openConnection(true);

            connection.connected.addEventListener((o, connectionEventArgs) -> {
                connectedEventCount.getAndIncrement();
            });
        }

        Future<?> future = r.startContinuousRecognitionAsync();

        // Wait for max 30 seconds
        future.get(30, TimeUnit.SECONDS);

        // just wait one second
        Thread.sleep(1000);

        future = r.stopContinuousRecognitionAsync();
        assertNotNull("future is null.", future);

        // Wait for max 30 seconds
        future.get(30, TimeUnit.SECONDS);

        assertFalse("future is canceled.", future.isCancelled());
        assertTrue("future is not done.", future.isDone());

        // Not calling connection.closeConnection() or connection.close() by purpose, in order to
        // test that no JVM crash happens even the connection object might be garbage collected.
        r.close();
        s.close();
    }

    @Test
    public void testStartStopContinuousRecognitionAsync() throws InterruptedException, ExecutionException, TimeoutException {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Key,
            Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Region);

        assertNotNull(s);

        SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath)));
        Connection connection = Connection.fromRecognizer(r);
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);

        final ArrayList<String> rEvents = new ArrayList<>();

        AtomicInteger connectedEventCount = new AtomicInteger(0);;
        AtomicInteger disconnectedEventCount = new AtomicInteger(0);;
        connection.connected.addEventListener((o, connectionEventArgs) -> {
            connectedEventCount.getAndIncrement();
        });
        connection.disconnected.addEventListener((o, connectionEventArgs) -> {
            disconnectedEventCount.getAndIncrement();
        });

        r.recognized.addEventListener((o, e) -> {
            rEvents.add("Result@" + System.currentTimeMillis());
        });

        Future<?> future = r.startContinuousRecognitionAsync();
        assertNotNull("future is null.", future);

        // Wait for max 30 seconds
        future.get(30, TimeUnit.SECONDS);

        assertFalse("future is canceled.", future.isCancelled());
        assertTrue("future is not done.", future.isDone());

        // wait until we get at least on final result
        long now = System.currentTimeMillis();
        while(((System.currentTimeMillis() - now) < 30000) &&
                (rEvents.isEmpty())) {
            Thread.sleep(200);
        }

        // test that we got one result
        // TODO multi-phrase test with several phrases in one session
        assertEquals(1, rEvents.size());

        future = r.stopContinuousRecognitionAsync();
        assertNotNull("future is null.", future);

        // Wait for max 30 seconds
        future.get(30, TimeUnit.SECONDS);

        assertFalse("future is canceled.", future.isCancelled());
        assertTrue("future is not done.", future.isDone());

        // It is not required to explictly close the connection. This is also used to keep the connection object alive.
        connection.closeConnection();
        TestHelper.AssertConnectionCountMatching(connectedEventCount.get(), disconnectedEventCount.get());

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

        SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath)));
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);

        r.close();
        s.close();
    }

    // -----------------------------------------------------------------------
    // ---
    // -----------------------------------------------------------------------

    @Ignore
    @Test
    public void testRecognizer() {
        // TODO: constructor is protected,
        fail("Not yet implemented");
    }

    @Test
    public void testClose() {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Key,
            Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Region);

        assertNotNull(s);

        SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath)));
        assertNotNull(r);
        assertTrue(r instanceof Recognizer);

        r.close();
        s.close();
    }

    @Test
    public void testAuthorizationToken() throws InterruptedException, ExecutionException, TimeoutException {

        SpeechConfig s = SpeechConfig.fromAuthorizationToken(authorizationToken, Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Region);
        assertNotNull(s);

        SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath)));
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);

        SpeechRecognitionResult res = r.recognizeOnceAsync().get();
        assertNotNull(res);
        TestHelper.OutputResult(res);
        assertEquals("What's the weather like?", res.getText());

        r.close();
        s.close();
    }

    @Test
    public void testEndpointWithToken() throws InterruptedException, ExecutionException, TimeoutException {
        String endpoint = "wss://" + Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Region + ".stt.speech.microsoft.com/speech/recognition/conversation/cognitiveservices/v1";

        SpeechConfig s = SpeechConfig.fromEndpoint(URI.create(endpoint));
        assertNotNull(s);

        SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath)));
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);

        final ArrayList<String> rEvents = new ArrayList<>();

        r.setAuthorizationToken(authorizationToken);
        assertTrue(r.getProperties().getProperty(PropertyId.SpeechServiceConnection_Key).isEmpty());

        r.recognized.addEventListener((o, e) -> {
            rEvents.add("Result@" + System.currentTimeMillis());
            System.out.println("recognized:" + e.toString());
        });

        Future<?> future = r.startContinuousRecognitionAsync();
        // Wait for max 30 seconds
        future.get(30, TimeUnit.SECONDS);

        // wait until we get at least on final result
        long now = System.currentTimeMillis();
        while(((System.currentTimeMillis() - now) < 30000) &&
                (rEvents.isEmpty())) {
            Thread.sleep(200);
        }

        // test that we got one result
        // TODO multi-phrase test with several phrases in one session
        assertEquals(1, rEvents.size());

        future = r.stopContinuousRecognitionAsync();
        assertNotNull("future is null.", future);

        // Wait for max 30 seconds
        future.get(30, TimeUnit.SECONDS);

        assertFalse("future is canceled.", future.isCancelled());
        assertTrue("future is not done.", future.isDone());

        r.close();
        s.close();
    }

    @Test
    public void testEndpointWithSubscriptionKeyAndInvalidToken() throws InterruptedException, ExecutionException, TimeoutException {
        String endpoint = "wss://" + Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Region + ".stt.speech.microsoft.com/speech/recognition/conversation/cognitiveservices/v1";
        String invalidToken = "InvalidToken";

        SpeechConfig s = SpeechConfig.fromEndpoint(URI.create(endpoint), Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Key);
        assertNotNull(s);
        s.setAuthorizationToken(invalidToken);

        SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath)));
        assertEquals(Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Key, r.getProperties().getProperty(PropertyId.SpeechServiceConnection_Key));
        assertEquals(invalidToken, r.getAuthorizationToken());

        SpeechRecognitionResult res = r.recognizeOnceAsync().get();
        assertNotNull(res);
        TestHelper.OutputResult(res);
        assertTrue(ResultReason.RecognizedSpeech == res.getReason());
        assertEquals("What's the weather like?", res.getText());

        r.close();
        s.close();
    }

    @Test
    public void testEndpointWithInvalidSubscriptionKeyAndValidToken() throws InterruptedException, ExecutionException, TimeoutException {
        String endpoint = "wss://" + Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Region + ".stt.speech.microsoft.com/speech/recognition/conversation/cognitiveservices/v1";
        String invalidKey = "InvalidSubscriptionKey";

        SpeechConfig s = SpeechConfig.fromEndpoint(URI.create(endpoint), invalidKey);
        assertNotNull(s);
        s.setAuthorizationToken(authorizationToken);

        SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath)));
        assertEquals(authorizationToken, r.getAuthorizationToken());
        assertEquals(invalidKey, r.getProperties().getProperty(PropertyId.SpeechServiceConnection_Key));

        SpeechRecognitionResult res = r.recognizeOnceAsync().get();
        assertNotNull(res);
        TestHelper.OutputResult(res);
        assertTrue(ResultReason.RecognizedSpeech == res.getReason());
        assertEquals("What's the weather like?", res.getText());

        r.close();
        s.close();
    }

    @Test
    public void testSetServiceProperty() throws InterruptedException, ExecutionException, TimeoutException {

        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Key,
            Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Region);

        s.setServiceProperty("format", "detailed", ServicePropertyChannel.UriQueryParameter);

        SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath)));

        SpeechRecognitionResult res = r.recognizeOnceAsync().get();
        assertNotNull(res);
        TestHelper.OutputResult(res);
        assertTrue(ResultReason.RecognizedSpeech == res.getReason());
        assertEquals("What's the weather like?", res.getText());

        String detailedResult = res.getProperties().getProperty(PropertyId.SpeechServiceResponse_JsonResult);
        System.out.println("Detailed result:" + detailedResult);
        assertTrue(detailedResult.contains("NBest"));
        assertTrue(detailedResult.contains("ITN"));
        assertTrue(detailedResult.contains("Lexical"));
        assertTrue(detailedResult.contains("MaskedITN"));
        assertTrue(detailedResult.contains("Display"));

        r.close();
        s.close();
    }

    @Test
    public void testPropertiesSetAndGet() throws NumberFormatException
    {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Key,
            Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Region);

        int initialSilenceTimeout = 6000;
        int endSilenceTimeout = 10000;
        s.setProperty(PropertyId.SpeechServiceConnection_InitialSilenceTimeoutMs, Integer.toString(initialSilenceTimeout));
        s.setProperty(PropertyId.SpeechServiceConnection_EndSilenceTimeoutMs, Integer.toString(endSilenceTimeout));
        assertEquals(initialSilenceTimeout, Integer.parseInt(s.getProperty(PropertyId.SpeechServiceConnection_InitialSilenceTimeoutMs)));
        assertEquals(endSilenceTimeout, Integer.parseInt(s.getProperty(PropertyId.SpeechServiceConnection_EndSilenceTimeoutMs)));

        int threshold = 15;
        s.setProperty(PropertyId.SpeechServiceResponse_StablePartialResultThreshold, Integer.toString(threshold));
        assertEquals(threshold, Integer.parseInt(s.getProperty(PropertyId.SpeechServiceResponse_StablePartialResultThreshold)));

        String valStr = "detailed";
        s.setProperty(PropertyId.SpeechServiceResponse_OutputFormatOption, valStr);
        assertEquals(valStr, s.getProperty(PropertyId.SpeechServiceResponse_OutputFormatOption));

        String profanity = "raw";
        s.setProperty(PropertyId.SpeechServiceResponse_ProfanityOption, profanity);
        assertEquals(profanity, s.getProperty(PropertyId.SpeechServiceResponse_ProfanityOption));

        String falseStr = "false";
        s.setProperty(PropertyId.SpeechServiceConnection_EnableAudioLogging, falseStr);
        assertEquals(falseStr, s.getProperty(PropertyId.SpeechServiceConnection_EnableAudioLogging));

        s.setProperty(PropertyId.SpeechServiceResponse_RequestWordLevelTimestamps, falseStr);
        assertEquals(falseStr, s.getProperty(PropertyId.SpeechServiceResponse_RequestWordLevelTimestamps));

        s.setProperty(PropertyId.SpeechServiceResponse_TranslationRequestStablePartialResult, falseStr);
        assertEquals(falseStr, s.getProperty(PropertyId.SpeechServiceResponse_TranslationRequestStablePartialResult));

        String trueText = "TrueText";
        s.setProperty(PropertyId.SpeechServiceResponse_PostProcessingOption, trueText);
        assertEquals(trueText, s.getProperty(PropertyId.SpeechServiceResponse_PostProcessingOption));

        SpeechRecognizer recognizer = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath)));
        assertEquals(initialSilenceTimeout, Integer.parseInt(recognizer.getProperties().getProperty(PropertyId.SpeechServiceConnection_InitialSilenceTimeoutMs)));
        assertEquals(endSilenceTimeout, Integer.parseInt(recognizer.getProperties().getProperty(PropertyId.SpeechServiceConnection_EndSilenceTimeoutMs)));
        assertEquals(threshold, Integer.parseInt(recognizer.getProperties().getProperty(PropertyId.SpeechServiceResponse_StablePartialResultThreshold)));
        assertEquals(valStr, recognizer.getProperties().getProperty(PropertyId.SpeechServiceResponse_OutputFormatOption));
        assertEquals(profanity, recognizer.getProperties().getProperty(PropertyId.SpeechServiceResponse_ProfanityOption));
        assertEquals(falseStr, recognizer.getProperties().getProperty(PropertyId.SpeechServiceConnection_EnableAudioLogging));
        assertEquals(falseStr, recognizer.getProperties().getProperty(PropertyId.SpeechServiceResponse_RequestWordLevelTimestamps));
        assertEquals(falseStr, recognizer.getProperties().getProperty(PropertyId.SpeechServiceResponse_TranslationRequestStablePartialResult));
        assertEquals(trueText, recognizer.getProperties().getProperty(PropertyId.SpeechServiceResponse_PostProcessingOption));

        recognizer.close();
        s.close();
    }

    @Test
    public void PropertiesDirectSetAndGet()
    {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Key,
            Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Region);

        s.setProfanity(ProfanityOption.Removed);
        s.enableAudioLogging();
        s.requestWordLevelTimestamps();
        s.enableDictation();

        SpeechRecognizer recognizer = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath)));
        assertEquals("DICTATION", s.getProperty(PropertyId.SpeechServiceConnection_RecoMode));
        assertEquals("DICTATION", recognizer.getProperties().getProperty(PropertyId.SpeechServiceConnection_RecoMode));
        assertEquals("removed", s.getProperty(PropertyId.SpeechServiceResponse_ProfanityOption));
        assertEquals("removed", recognizer.getProperties().getProperty(PropertyId.SpeechServiceResponse_ProfanityOption));
        assertEquals("true", s.getProperty(PropertyId.SpeechServiceConnection_EnableAudioLogging));
        assertEquals("true", recognizer.getProperties().getProperty(PropertyId.SpeechServiceConnection_EnableAudioLogging));
        assertEquals("true", s.getProperty(PropertyId.SpeechServiceResponse_RequestWordLevelTimestamps));
        assertEquals("true", recognizer.getProperties().getProperty(PropertyId.SpeechServiceResponse_RequestWordLevelTimestamps));

        s.close();
        recognizer.close();
    }

    @Test
    public void ProfanityPropertySetAndGet()
    {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Key,
            Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Region);

        s.setProfanity(ProfanityOption.Masked);
        SpeechRecognizer recognizer = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath)));
        assertEquals("masked", s.getProperty(PropertyId.SpeechServiceResponse_ProfanityOption));
        assertEquals("masked", recognizer.getProperties().getProperty(PropertyId.SpeechServiceResponse_ProfanityOption));
        recognizer.close();

        s.setProfanity(ProfanityOption.Removed);
        recognizer = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath)));
        assertEquals("removed", s.getProperty(PropertyId.SpeechServiceResponse_ProfanityOption));
        assertEquals("removed", recognizer.getProperties().getProperty(PropertyId.SpeechServiceResponse_ProfanityOption));
        recognizer.close();

        s.setProfanity(ProfanityOption.Raw);
        recognizer = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath)));
        assertEquals("raw", s.getProperty(PropertyId.SpeechServiceResponse_ProfanityOption));
        assertEquals("raw", recognizer.getProperties().getProperty(PropertyId.SpeechServiceResponse_ProfanityOption));
        recognizer.close();
        s.close();
    }

    @Test
    public void testExceptionsDuringEventsRecognizeOnce() throws InterruptedException, ExecutionException {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Key,
            Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Region);

        assertNotNull(s);

        SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath)));
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

        SpeechRecognitionResult res = r.recognizeOnceAsync().get();
        assertNotNull(res);
        TestHelper.OutputResult(res);
        assertEquals(ResultReason.RecognizedSpeech, res.getReason());
        assertEquals("What's the weather like?", res.getText());

        // wait until we get the SessionStopped event.
        long now = System.currentTimeMillis();
        while(((System.currentTimeMillis() - now) < 30000) && (sessionStopped.get() == true)) {
            Thread.sleep(200);
        }
        // It is not required to explictly close the connection. This is also used to keep the connection object alive.
        connection.closeConnection();

        r.close();
        s.close();
    }

    @Test
    public void testExceptionsDuringEventsContinuousRecognition() throws InterruptedException, ExecutionException {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Key,
            Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Region);

        assertNotNull(s);

        SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath)));
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

        // It is not required to explictly close the connection. This is also used to keep the connection object alive.
        connection.closeConnection();

        r.close();
        s.close();
    }

    @Test
    public void DictationCorrections() throws InterruptedException, ExecutionException, TimeoutException {

        String endpoint = "wss://officespeech.platform.bing.com/speech/recognition/dictation/office/v1";
        SpeechConfig s = SpeechConfig.fromEndpoint(URI.create(endpoint));
        assertNotNull(s);
        s.enableDictation();
        s.setServiceProperty("format", "corrections", ServicePropertyChannel.UriQueryParameter);

        SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath)));
        r.setAuthorizationToken("abc");
        Connection connection = Connection.fromRecognizer(r);
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);

        connection.openConnection(false);

        AtomicInteger connectedEventCount = new AtomicInteger(0);
        AtomicInteger disconnectedEventCount = new AtomicInteger(0);
        AtomicInteger sessionStoppedCount = new AtomicInteger(0);
        connection.connected.addEventListener((o, connectionEventArgs) -> {
            connectedEventCount.getAndIncrement();
        });

        connection.disconnected.addEventListener((o, connectionEventArgs) -> {
            disconnectedEventCount.getAndIncrement();
        });

        r.sessionStopped.addEventListener((o, SessionEventArgs) -> {
            sessionStoppedCount.getAndIncrement();
        });

        String data = "{\"Id\": \"Corrections\",\"Name\": \"Telemetry\",\"ClientSessionId\": \"DADAAAC4-019C-4D23-9301-7FD619BE68AB\",\"CorrectionEvents\": [{\"PhraseId\": \"AEDC2194-019C-4D23-9301-7FD619BE68A9\",\"CorrectionId\": 0,\"AlternateId\": 1,\"TreatedInUX\": \"true\",\"TriggerType\": \"click\",\"EditType\": \"alternate\"},{\"PhraseId\": \"BEDC2194-019C-4D23-9301-7FD619BE68AA\",\"CorrectionId\": 0,\"AlternateId\": 2,\"TriggerType\": \"hover\",\"TreatedInUX\": \"false\",\"EditType\": \"alternate\"}] }";
        Future<?> messageFuture = connection.sendMessageAsync("event", data);
        assertNotNull("future is null.", messageFuture);

        // Wait for max 30 seconds
        messageFuture.get(30, TimeUnit.SECONDS);

        Future<SpeechRecognitionResult> future = r.recognizeOnceAsync();

        // Wait for max 30 seconds
        SpeechRecognitionResult res = future.get(30, TimeUnit.SECONDS);

        assertFalse("future is canceled.", future.isCancelled());
        assertTrue("future is not done.", future.isDone());
        assertNotNull("future is null.", future);

        assertNotNull(res);
        TestHelper.OutputResult(res);
        assertEquals(ResultReason.RecognizedSpeech, res.getReason());

        String detailedResult = res.getProperties().getProperty(PropertyId.SpeechServiceResponse_JsonResult);
        assertTrue(detailedResult.contains("Corrections"));
        System.out.println("DictationCorrections: detailed result: " + detailedResult);

        connection.closeConnection();

        r.close();
        s.close();
    }

    @Test
    public void verfiyLanguageIdDetection() throws InterruptedException, ExecutionException, TimeoutException {
        SpeechConfig speechConfig = SpeechConfig.fromSubscription(Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Key,
            Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Region);

        assertNotNull(speechConfig);
        List<SourceLanguageConfig> sourceLanguageConfigs = new ArrayList<SourceLanguageConfig>();
        sourceLanguageConfigs.add(SourceLanguageConfig.fromLanguage("en-US"));
        sourceLanguageConfigs.add(SourceLanguageConfig.fromLanguage("zh-CN"));
        AutoDetectSourceLanguageConfig autoDetectSourceLanguageConfig = AutoDetectSourceLanguageConfig.fromSourceLanguageConfigs(sourceLanguageConfigs);
        assertNotNull(autoDetectSourceLanguageConfig);

        SpeechRecognizer recognizer = new SpeechRecognizer(speechConfig, autoDetectSourceLanguageConfig, AudioConfig.fromWavFileInput(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath)));
        assertNotNull(recognizer);
        assertNotNull(recognizer.getRecoImpl());
        assertTrue(recognizer instanceof Recognizer);
        Future<SpeechRecognitionResult> future = recognizer.recognizeOnceAsync();
        // Wait for max 30 seconds
        SpeechRecognitionResult result = future.get(30, TimeUnit.SECONDS);

        assertFalse("future is canceled.", future.isCancelled());
        assertTrue("future is not done.", future.isDone());
        assertNotNull("future is null.", future);

        assertNotNull(result);
        TestHelper.OutputResult(result);
        assertEquals(ResultReason.RecognizedSpeech, result.getReason());
        AutoDetectSourceLanguageResult autoDetectSourceLanguageResult = AutoDetectSourceLanguageResult.fromResult(result);
        assertEquals("en-US", autoDetectSourceLanguageResult.getLanguage());

        recognizer.close();
        speechConfig.close();
        result.close();
        for (SourceLanguageConfig sourceLanguageConfig : sourceLanguageConfigs)
        {
            sourceLanguageConfig.close();
        }
        autoDetectSourceLanguageConfig.close();
    }

    @Rule
    public ExpectedException exceptionRule = ExpectedException.none();
 
    @Test
    public void verifySetEndpointIdNotAllowedForLanguageId() throws ExecutionException {
        SpeechConfig speechConfig = SpeechConfig.fromSubscription(Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Key,
            Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Region);

        assertNotNull(speechConfig);
        speechConfig.setEndpointId("customEndpoint");
        AutoDetectSourceLanguageConfig autoDetectSourceLanguageConfig = AutoDetectSourceLanguageConfig.fromLanguages(Arrays.asList("en-US", "de-DE"));

        exceptionRule.expect(RuntimeException.class);
        exceptionRule.expectMessage("Invalid argument exception: EndpointId on SpeechConfig is unsupported for auto detection source language scenario. Please set per language endpointId through SourceLanguageConfig and use it to construct AutoDetectSourceLanguageConfig.");
        SpeechRecognizer recognizer = new SpeechRecognizer(speechConfig, autoDetectSourceLanguageConfig, AudioConfig.fromWavFileInput(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath)));
    }

    @Test(expected = NullPointerException.class)
    public void verifyNullSourceLanguageConfig()  throws ExecutionException {
        SpeechConfig speechConfig = SpeechConfig.fromSubscription(Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Key,
            Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Region);

        assertNotNull(speechConfig);
        SourceLanguageConfig sourceLanguageConfig = null;
        SpeechRecognizer recognizer = new SpeechRecognizer(speechConfig, sourceLanguageConfig, AudioConfig.fromWavFileInput(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath)));
    }

    @Test(expected = IllegalArgumentException.class)
    public void verifyNullSourceLanguageConfigForAutoDetectSourceLanguageConfig()  throws ExecutionException {
        AutoDetectSourceLanguageConfig autoDetectSourceLanguageConfig = AutoDetectSourceLanguageConfig.fromLanguages(null);
    }

    @Test
    public void verifyAutoDetectSourceLanguageConfig() throws InterruptedException, ExecutionException, TimeoutException {
        SpeechConfig speechConfig = SpeechConfig.fromSubscription(Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Key,
            Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Region);

        assertNotNull(speechConfig);
        
        List sourceLanguageConfigs = new ArrayList<SourceLanguageConfig>();
        String customEndpoint1 = "6264ab43-c854-4d9f-84fc-5f33621935f3";
        String customEndpoint2 = "6264ab43-c854-4d9f-84fc-5f33621935f7";
        sourceLanguageConfigs.add(SourceLanguageConfig.fromLanguage("en-US", customEndpoint1));
        sourceLanguageConfigs.add(SourceLanguageConfig.fromLanguage("zh-CN", customEndpoint2));
        AutoDetectSourceLanguageConfig autoDetectSourceLanguageConfig = AutoDetectSourceLanguageConfig.fromSourceLanguageConfigs(sourceLanguageConfigs);
        assertNotNull(autoDetectSourceLanguageConfig);

        SpeechRecognizer recognizer = new SpeechRecognizer(speechConfig, autoDetectSourceLanguageConfig, AudioConfig.fromWavFileInput(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath)));
        assertNotNull(recognizer);
        assertNotNull(recognizer.getRecoImpl());
        assertTrue(recognizer instanceof Recognizer);
        assertEquals(customEndpoint1, recognizer.getProperties().getProperty("en-USSPEECH-ModelId"));
        assertEquals(customEndpoint2, recognizer.getProperties().getProperty("zh-CNSPEECH-ModelId"));
        assertEquals("en-US,zh-CN", recognizer.getProperties().getProperty(PropertyId.SpeechServiceConnection_AutoDetectSourceLanguages));
    }
}
