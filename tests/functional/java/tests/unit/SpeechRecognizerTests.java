package tests.unit;
//
//Copyright (c) Microsoft. All rights reserved.
//Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.Future;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.TimeoutException;
import java.util.concurrent.atomic.AtomicInteger;


import static org.junit.Assert.*;

import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.Ignore;


import com.microsoft.cognitiveservices.speech.audio.AudioConfig;
import com.microsoft.cognitiveservices.speech.CancellationReason;
import com.microsoft.cognitiveservices.speech.ResultReason;
import com.microsoft.cognitiveservices.speech.Recognizer;
import com.microsoft.cognitiveservices.speech.SpeechConfig;
import com.microsoft.cognitiveservices.speech.SpeechRecognitionResult;
import com.microsoft.cognitiveservices.speech.SpeechRecognizer;
import com.microsoft.cognitiveservices.speech.PropertyId;
import com.microsoft.cognitiveservices.speech.OutputFormat;

import tests.Settings;

public class SpeechRecognizerTests {
    private final Integer FIRST_EVENT_ID = 1;
    private AtomicInteger eventIdentifier = new AtomicInteger(FIRST_EVENT_ID);

    @BeforeClass
    static public void setUpBeforeClass() throws Exception {
        // Override inputs, if necessary
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
    public void testRecognizeOnceAsync1() throws InterruptedException, ExecutionException, TimeoutException {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);

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
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);

        final Map<String, Integer> eventsMap = new HashMap<String, Integer>();

        r.recognized.addEventListener((o, e) -> {
            eventsMap.put("recognized", eventIdentifier.getAndIncrement());
        });

        r.recognizing.addEventListener((o, e) -> {
            int now = eventIdentifier.getAndIncrement();
            eventsMap.put("recognizing-" + System.currentTimeMillis(), now);
            eventsMap.put("recognizing" , now);
        });

        r.canceled.addEventListener((o, e) -> {
            if (e.getReason() == CancellationReason.Error) {
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
            int now = eventIdentifier.getAndIncrement();
            eventsMap.put("sessionStopped-" + System.currentTimeMillis(), now);
            eventsMap.put("sessionStopped", now);
        });

        // Note: TODO session stopped event not necessarily raised before async operation returns!
        //       this makes this test flaky
        SpeechRecognitionResult res = r.recognizeOnceAsync().get();
        assertNotNull(res);
        assertEquals(ResultReason.RecognizedSpeech, res.getReason());
        assertEquals("What's the weather like?", res.getText());

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
    public void testStopContinuousRecognitionAsync() throws InterruptedException, ExecutionException, TimeoutException {
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
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);

        final ArrayList<String> rEvents = new ArrayList<>();

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
}
