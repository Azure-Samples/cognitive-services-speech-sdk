//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

package tests.unit;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.Future;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.TimeoutException;
import java.util.concurrent.atomic.AtomicInteger;
import java.net.URI;

import static org.junit.Assert.*;

import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.Ignore;

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

import tests.Settings;
import tests.TestHelper;

public class SpeechRecognizerTests {
    private final Integer FIRST_EVENT_ID = 1;
    private AtomicInteger eventIdentifier = new AtomicInteger(FIRST_EVENT_ID);
    private static String authorizationToken;

    @BeforeClass
    static public void setUpBeforeClass() throws Exception {
        Settings.LoadSettings();
        authorizationToken = TestHelper.getAuthorizationToken(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
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
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);

        r.close();
        s.close();
    }

    @Test
    public void testSpeechRecognizer2() throws InterruptedException, ExecutionException {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.WavFile);
        assertNotNull(ais);

        SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromStreamInput(ais));
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);

        SpeechRecognitionResult res = r.recognizeOnceAsync().get();
        assertNotNull(res);
        assertEquals("What's the weather like?", res.getText());

        r.close();
        s.close();
    }

    @Test
    public void testSpeechRecognizerInterleaved() throws InterruptedException, ExecutionException {

        //
        // Start a legal request
        //

        SpeechConfig s2 = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s2);

        WavFileAudioInputStream ais2 = new WavFileAudioInputStream(Settings.WavFile);
        assertNotNull(ais2);

        AudioConfig ac2 = AudioConfig.fromStreamInput(ais2);
        assertNotNull(ac2);

        //
        // Start: make an illegal request
        //

        SpeechConfig s = SpeechConfig.fromSubscription("illegal", "illegal");
        assertNotNull(s);

        WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.WavFile);
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
        assertEquals("What's the weather like?", res2.getText());

        r2.close();
        s2.close();
    }

    // -----------------------------------------------------------------------
    // ---
    // -----------------------------------------------------------------------

    @Test
    public void testGetEndpointId() {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.WavFile);
        assertNotNull(ais);

        SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromStreamInput(ais));
        assertNotNull(r);

        assertNotNull(r.getEndpointId());

        r.close();
        s.close();
    }

    @Test
    // TODO make a config test? no setEndpointId anymore
    public void testSetEndpointId() {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.WavFile);
        assertNotNull(ais);

        SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromStreamInput(ais));
        assertNotNull(r);

        assertNotNull(r.getEndpointId());

        //String newEndpointId = "new-" + r.getEndpointId();
        //r.setEndpointId(newEndpointId);

        //assertEquals(newEndpointId, r.getEndpointId());

        r.close();
        s.close();
    }

    // -----------------------------------------------------------------------
    // ---
    // -----------------------------------------------------------------------

    @Test
    public void testGetLanguage1() {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.WavFile);
        assertNotNull(ais);

        SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromStreamInput(ais));
        assertNotNull(r);

        assertNotNull(r.getSpeechRecognitionLanguage());

        r.close();
        s.close();
    }

    @Test
    public void testGetLanguage2() {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.WavFile);
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
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));
        assertNotNull(r);

        assertEquals(r.getOutputFormat(), OutputFormat.Simple);

        r.close();
        s.close();
    }

    @Test
    public void testGetOutputFormatDetailed() {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        String language = "de-DE";
        s.setSpeechRecognitionLanguage(language);
        s.setOutputFormat(OutputFormat.Detailed);
        SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));
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
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));
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
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));
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
        assertNotNull(future);

        // Wait for max 30 seconds
        future.get(30, TimeUnit.SECONDS);

        assertFalse(future.isCancelled());
        assertTrue(future.isDone());

        SpeechRecognitionResult res = future.get();
        assertNotNull(res);
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

        r.close();
        s.close();
    }

    public void testRecognizeOnceAsyncWithLaterSubscribe1(boolean usingPreConnection) throws InterruptedException, ExecutionException, TimeoutException {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));
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
        assertNotNull(future);

        // Wait for max 30 seconds
        future.get(30, TimeUnit.SECONDS);

        assertFalse(future.isCancelled());
        assertTrue(future.isDone());

        SpeechRecognitionResult res = future.get();
        assertNotNull(res);
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
                    SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
                    SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));

                    Future<SpeechRecognitionResult> future = r.recognizeOnceAsync();

                    future.get(30, TimeUnit.SECONDS);
                    SpeechRecognitionResult res = future.get();

                    r.close();
                    s.close();

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


    @Ignore("TODO why does not get whats the weather like")
    @Test
    public void testRecognizeOnceAsync2() throws InterruptedException, ExecutionException {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));
        Connection connection = Connection.fromRecognizer(r);
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);

        final Map<String, Integer> eventsMap = new HashMap<String, Integer>();

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
            int now = eventIdentifier.getAndIncrement();
            eventsMap.put("recognizing-" + System.currentTimeMillis(), now);
            eventsMap.put("recognizing" , now);
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

        r.sessionStopped.addEventListener((o, e) -> {
            sessionStoppedCount.getAndIncrement();
            int now = eventIdentifier.getAndIncrement();
            eventsMap.put("sessionStopped-" + System.currentTimeMillis(), now);
            eventsMap.put("sessionStopped", now);
        });

        SpeechRecognitionResult res = r.recognizeOnceAsync().get();
        assertNotNull(res);
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
        final Integer LAST_RECORDED_EVENT_ID = eventIdentifier.get();
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
        assertTrue(eventsMap.get("recognized") < eventsMap.get("speechEndDetected"));
        assertTrue(eventsMap.get("recognizing") < eventsMap.get("recognized"));

        // make sure events we don't expect, don't get raised
        assertFalse(eventsMap.containsKey("canceled"));

        r.close();
        s.close();
    }

    @Ignore("TODO why does not get whats the weather like")
    @Test
    public void testRecognizeOnceAsyncWithLateSubscription2() throws InterruptedException, ExecutionException {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));
        Connection connection = Connection.fromRecognizer(r);
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);

        final Map<String, Integer> eventsMap = new HashMap<String, Integer>();

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
            int now = eventIdentifier.getAndIncrement();

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

            eventsMap.put("recognizing-" + System.currentTimeMillis(), now);
            eventsMap.put("recognizing" , now);
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

        r.sessionStopped.addEventListener((o, e) -> {
            sessionStoppedCount.getAndIncrement();
            int now = eventIdentifier.getAndIncrement();
            eventsMap.put("sessionStopped-" + System.currentTimeMillis(), now);
            eventsMap.put("sessionStopped", now);
        });

        SpeechRecognitionResult res = r.recognizeOnceAsync().get();
        assertNotNull(res);
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
        final Integer LAST_RECORDED_EVENT_ID = eventIdentifier.get();
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
        assertTrue(eventsMap.get("recognized") < eventsMap.get("speechEndDetected"));
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
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);

        Future<?> future = r.startContinuousRecognitionAsync();
        assertNotNull(future);

        // Wait for max 30 seconds
        future.get(30, TimeUnit.SECONDS);

        assertFalse(future.isCancelled());
        assertTrue(future.isDone());

        r.close();
        s.close();
    }

    @Test
    public void testStopContinuousRecognitionAsyncWithPreConnection() throws InterruptedException, ExecutionException, TimeoutException {
        testStopContinuousRecognitionAsync(true);
    }

    public void testStopContinuousRecognitionAsyncWithoutPreConnection() throws InterruptedException, ExecutionException, TimeoutException {
        testStopContinuousRecognitionAsync(false);
    }

    public void testStopContinuousRecognitionAsync(boolean usingPreConnection) throws InterruptedException, ExecutionException, TimeoutException {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);
        Connection connection = Connection.fromRecognizer(r);
        if (usingPreConnection)
        {
            connection.openConnection(true);
        }

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
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));
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

        // test that we got one result
        // TODO multi-phrase test with several phrases in one session
        assertEquals(1, rEvents.size());

        future = r.stopContinuousRecognitionAsync();
        assertNotNull(future);

        // Wait for max 30 seconds
        future.get(30, TimeUnit.SECONDS);

        assertFalse(future.isCancelled());
        assertTrue(future.isDone());

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
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));
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
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));
        assertNotNull(r);
        assertTrue(r instanceof Recognizer);

        r.close();
        s.close();
    }

    @Test
    public void testAuthorizationToken() throws InterruptedException, ExecutionException, TimeoutException {

        SpeechConfig s = SpeechConfig.fromAuthorizationToken(authorizationToken, Settings.SpeechRegion);
        assertNotNull(s);

        SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);

        SpeechRecognitionResult res = r.recognizeOnceAsync().get();
        assertNotNull(res);
        assertEquals("What's the weather like?", res.getText());

        r.close();
        s.close();
    }

    @Test
    public void testEndpointWithToken() throws InterruptedException, ExecutionException, TimeoutException {
        String endpoint = "wss://" + Settings.SpeechRegion + ".stt.speech.microsoft.com/speech/recognition/conversation/cognitiveservices/v1";

        SpeechConfig s = SpeechConfig.fromEndpoint(URI.create(endpoint));
        assertNotNull(s);

        SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));
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

        // test that we got one result
        // TODO multi-phrase test with several phrases in one session
        assertEquals(1, rEvents.size());

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
    public void testEndpointWithSubscriptionKeyAndInvalidToken() throws InterruptedException, ExecutionException, TimeoutException {
        String endpoint = "wss://" + Settings.SpeechRegion + ".stt.speech.microsoft.com/speech/recognition/conversation/cognitiveservices/v1";
        String invalidToken = "InvalidToken";

        SpeechConfig s = SpeechConfig.fromEndpoint(URI.create(endpoint), Settings.SpeechSubscriptionKey);
        assertNotNull(s);
        s.setAuthorizationToken(invalidToken);

        SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));
        assertEquals(Settings.SpeechSubscriptionKey, r.getProperties().getProperty(PropertyId.SpeechServiceConnection_Key));
        assertEquals(invalidToken, r.getAuthorizationToken());

        SpeechRecognitionResult res = r.recognizeOnceAsync().get();
        assertNotNull(res);
        assertTrue(ResultReason.RecognizedSpeech == res.getReason());
        assertEquals("What's the weather like?", res.getText());

        r.close();
        s.close();
    }

    @Test
    public void testEndpointWithInvalidSubscriptionKeyAndValidToken() throws InterruptedException, ExecutionException, TimeoutException {
        String endpoint = "wss://" + Settings.SpeechRegion + ".stt.speech.microsoft.com/speech/recognition/conversation/cognitiveservices/v1";
        String invalidKey = "InvalidSubscriptionKey";

        SpeechConfig s = SpeechConfig.fromEndpoint(URI.create(endpoint), invalidKey);
        assertNotNull(s);
        s.setAuthorizationToken(authorizationToken);

        SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));
        assertEquals(authorizationToken, r.getAuthorizationToken());
        assertEquals(invalidKey, r.getProperties().getProperty(PropertyId.SpeechServiceConnection_Key));

        SpeechRecognitionResult res = r.recognizeOnceAsync().get();
        assertNotNull(res);
        assertTrue(ResultReason.RecognizedSpeech == res.getReason());
        assertEquals("What's the weather like?", res.getText());

        r.close();
        s.close();
    }

    @Test
    public void testSetServiceProperty() throws InterruptedException, ExecutionException, TimeoutException {

        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        s.setServiceProperty("format", "detailed", ServicePropertyChannel.UriQueryParameter);

        SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));

        SpeechRecognitionResult res = r.recognizeOnceAsync().get();
        assertNotNull(res);
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
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
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

        SpeechRecognizer recognizer = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));
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
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        s.setProfanity(ProfanityOption.Removed);
        s.enableAudioLogging();
        s.requestWordLevelTimestamps();
        s.enableDictation();

        SpeechRecognizer recognizer = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));
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
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        s.setProfanity(ProfanityOption.Masked);
        SpeechRecognizer recognizer = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));
        assertEquals("masked", s.getProperty(PropertyId.SpeechServiceResponse_ProfanityOption));
        assertEquals("masked", recognizer.getProperties().getProperty(PropertyId.SpeechServiceResponse_ProfanityOption));
        recognizer.close();

        s.setProfanity(ProfanityOption.Removed);
        recognizer = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));
        assertEquals("removed", s.getProperty(PropertyId.SpeechServiceResponse_ProfanityOption));
        assertEquals("removed", recognizer.getProperties().getProperty(PropertyId.SpeechServiceResponse_ProfanityOption));
        recognizer.close();

        s.setProfanity(ProfanityOption.Raw);
        recognizer = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));
        assertEquals("raw", s.getProperty(PropertyId.SpeechServiceResponse_ProfanityOption));
        assertEquals("raw", recognizer.getProperties().getProperty(PropertyId.SpeechServiceResponse_ProfanityOption));
        recognizer.close();
        s.close();
    }

}
