package tests.unit;
//
//Copyright (c) Microsoft. All rights reserved.
//Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import static org.junit.Assert.*;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.Future;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.TimeoutException;
import java.util.concurrent.atomic.AtomicInteger;

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

import tests.Settings;

public class IntentRecognizerTests {
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
    public void testIntentRecognizer1() {
        SpeechConfig config = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(config);

        IntentRecognizer r = new IntentRecognizer(config, AudioConfig.fromWavFileInput(Settings.WavFile));
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);

        r.close();
        config.close();
    }

    @Ignore("TODO why does not get phrase")
    @Test
    public void testIntentRecognizer2() throws InterruptedException, ExecutionException {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.WavFile);
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
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        s.setSpeechRecognitionLanguage("en-US");
        assertNotNull(s);

        WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.WavFile);
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

    @Ignore("TODO check if language can be set to german")
    @Test
    public void testSetLanguage() {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        s.setSpeechRecognitionLanguage("en-US");
        assertNotNull(s);

        WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.WavFile);
        assertNotNull(ais);

        IntentRecognizer r = new IntentRecognizer(s, AudioConfig.fromStreamInput(ais));
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);

        assertFalse(r.getSpeechRecognitionLanguage().isEmpty());
        assertEquals("en-US", r.getSpeechRecognitionLanguage());

        // TODO not supported on recognizer anymore:
        // String language2 = "de-DE";
        // r.setLanguage(language2);

        // assertFalse(r.getSpeechRecognitionLanguage().isEmpty());
        // assertEquals(language2, r.getSpeechRecognitionLanguage());

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

        IntentRecognizer r = new IntentRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));
        assertNotNull(r);

        assertNotNull(r.getProperties());
        assertEquals(r.getSpeechRecognitionLanguage(), r.getProperties().getProperty(PropertyId.SpeechServiceConnection_RecoLanguage));

        r.close();
        s.close();
    }

    // -----------------------------------------------------------------------
    // ---
    // -----------------------------------------------------------------------

    @Ignore("TODO why is canceled reported instead of success")
    @Test
    public void testRecognizeOnceAsync1() throws InterruptedException, ExecutionException, TimeoutException {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        IntentRecognizer r = new IntentRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);

        Future<IntentRecognitionResult> future = r.recognizeOnceAsync();
        assertNotNull(future);

        // Wait for max 30 seconds
        future.get(30, TimeUnit.SECONDS);

        assertFalse(future.isCancelled());
        assertTrue(future.isDone());

        IntentRecognitionResult res = future.get();
        assertNotNull(res);
        assertTrue(ResultReason.RecognizedSpeech == res.getReason() ||
                ResultReason.RecognizedIntent == res.getReason());
        assertEquals("What's the weather like?", res.getText());

        // TODO: check for specific json parameters
        assertTrue(res.getProperties().getProperty(PropertyId.LanguageUnderstandingServiceResponse_JsonResult).length() > 0);

        r.close();
        s.close();
    }

    @Ignore("TODO why are error details not empty")
    @Test
    public void testRecognizeOnceAsync2() throws InterruptedException, ExecutionException {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        IntentRecognizer r = new IntentRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));
        Connection connection = Connection.fromRecognizer(r);
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);

        final Map<String, Integer> eventsMap = new HashMap<String, Integer>();

        final AtomicInteger connectedEventCount = new AtomicInteger(0);;
        final AtomicInteger disconnectedEventCount = new AtomicInteger(0);;
        connection.connected.addEventListener((o, connectionEventArgs) -> {
            connectedEventCount.getAndIncrement();
        });
        connection.disconnected.addEventListener((o, connectionEventArgs) -> {
            disconnectedEventCount.getAndIncrement();
        });

        r.recognized.addEventListener((o, e) -> {
            assertTrue(connectedEventCount.get() > 0);
            assertTrue(connectedEventCount.get() > disconnectedEventCount.get());
            eventsMap.put("recognized", eventIdentifier.getAndIncrement());
        });

        r.recognizing.addEventListener((o, e) -> {
            assertTrue(connectedEventCount.get() > 0);
            assertTrue(connectedEventCount.get() > disconnectedEventCount.get());
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
            int now = eventIdentifier.getAndIncrement();
            eventsMap.put("sessionStopped-" + System.currentTimeMillis(), now);
            eventsMap.put("sessionStopped", now);
        });

        IntentRecognitionResult res = r.recognizeOnceAsync().get();
        assertNotNull(res);
        assertTrue(res.getReason() != ResultReason.Canceled);
        assertEquals("What's the weather like?", res.getText());

        assertTrue(connectedEventCount.get() > 0);
        assertTrue(connectedEventCount.get() == disconnectedEventCount.get() + 1);

        // session events are first and last event
        final Integer LAST_RECORDED_EVENT_ID = eventIdentifier.get();
        assertTrue(LAST_RECORDED_EVENT_ID > FIRST_EVENT_ID);
        assertEquals(FIRST_EVENT_ID, eventsMap.get("speechStartDetected"));
        assertEquals(LAST_RECORDED_EVENT_ID, eventsMap.get("speechEndDetected"));

        // end events come after start events.
        assertTrue(eventsMap.get("sessionStarted") < eventsMap.get("sessionStopped"));
        assertTrue(eventsMap.get("speechStartDetected") < eventsMap.get("speechEndDetected"));

        // recognition events come after session start but before session end events
        assertTrue(eventsMap.get("sessionStarted") < eventsMap.get("speechStartDetected"));
        assertTrue(eventsMap.get("speechEndDetected") < eventsMap.get("sessionStopped"));

        // there is no partial result reported after the final result
        // (and check that we have intermediate and final results recorded)
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

        IntentRecognizer r = new IntentRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));
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

        IntentRecognizer r = new IntentRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));
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

    @Ignore("TODO why number of events not 1")
    @Test
    public void testStartStopContinuousRecognitionAsync() throws InterruptedException, ExecutionException, TimeoutException {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        IntentRecognizer r = new IntentRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));
        Connection connection = Connection.fromRecognizer(r);
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);

        final ArrayList<String> rEvents = new ArrayList<>();

        final AtomicInteger connectedEventCount = new AtomicInteger(0);;
        final AtomicInteger disconnectedEventCount = new AtomicInteger(0);;
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

        assertTrue(connectedEventCount.get() > 0);
        assertTrue(connectedEventCount.get() == disconnectedEventCount.get() + 1);

        r.close();
        s.close();
    }

    // -----------------------------------------------------------------------
    // ---
    // -----------------------------------------------------------------------

    @Ignore("TODO why is mapsize not 2")
    @Test
    public void testAddIntentStringString() throws InterruptedException, ExecutionException {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        IntentRecognizer r = new IntentRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));
        assertNotNull(r);

        // TODO check if intent is recognized
        r.addIntent("all intents");

        final Map<String, Integer> eventsMap = new HashMap<String, Integer>();

        r.recognized.addEventListener((o, e) -> {
            eventsMap.put("recognized", eventIdentifier.getAndIncrement());
            if(!e.getResult().getProperties().getProperty(PropertyId.LanguageUnderstandingServiceResponse_JsonResult).isEmpty()) {
                eventsMap.put("IntentReceived", eventIdentifier.getAndIncrement());
            }
        });

        // TODO why does this call require exceptions?
        // TODO check for specific intent.
        IntentRecognitionResult res = r.recognizeOnceAsync().get();
        assertNotNull(res);
        assertEquals(2, eventsMap.size());
        assertTrue(res.getProperties().getProperty(PropertyId.LanguageUnderstandingServiceResponse_JsonResult).length() > 0);
        assertTrue(ResultReason.RecognizedSpeech == res.getReason() ||
                ResultReason.RecognizedIntent == res.getReason());


        r.close();
        s.close();
    }

    @Ignore("TODO why is mapsize not 2")
    @Test
    public void testAddIntentStringLanguageUnderstandingModelString() throws InterruptedException, ExecutionException {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        IntentRecognizer r = new IntentRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));
        assertNotNull(r);

        LanguageUnderstandingModel model = LanguageUnderstandingModel.fromSubscription(Settings.LuisSubscriptionKey, Settings.LuisAppId, Settings.LuisRegion);
        assertNotNull(model);

        // TODO check if intent is recognized
        // TODO what is the intent name?
        r.addAllIntents(model, "any-id-you-want-here");

        final Map<String, Integer> eventsMap = new HashMap<String, Integer>();

        r.recognized.addEventListener((o, e) -> {
            eventsMap.put("recognized", eventIdentifier.getAndIncrement());
            if(!e.getResult().getProperties().getProperty(PropertyId.LanguageUnderstandingServiceResponse_JsonResult).isEmpty()) {
                eventsMap.put("IntentReceived", eventIdentifier.getAndIncrement());
            }
        });

        // TODO why does this call require exceptions?
        // TODO check for specific intent.
        IntentRecognitionResult res = r.recognizeOnceAsync().get();
        assertNotNull(res);
        assertEquals(2, eventsMap.size());
        assertTrue(res.getProperties().getProperty(PropertyId.LanguageUnderstandingServiceResponse_JsonResult).length() > 0);
        assertTrue(ResultReason.RecognizedSpeech == res.getReason() ||
                ResultReason.RecognizedIntent == res.getReason());


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

        IntentRecognizer r = new IntentRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);

        r.close();
        s.close();
    }
}
