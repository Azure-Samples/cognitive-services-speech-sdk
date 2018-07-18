package tests.unit;
//
//Copyright (c) Microsoft. All rights reserved.
//Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.Future;
import java.util.concurrent.atomic.AtomicInteger;


import static org.junit.Assert.*;

import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.Ignore;


import com.microsoft.cognitiveservices.speech.RecognitionEventType;
import com.microsoft.cognitiveservices.speech.RecognitionStatus;
import com.microsoft.cognitiveservices.speech.Recognizer;
import com.microsoft.cognitiveservices.speech.RecognizerParameterNames;
import com.microsoft.cognitiveservices.speech.SessionEventType;
import com.microsoft.cognitiveservices.speech.SpeechFactory;
import com.microsoft.cognitiveservices.speech.SpeechRecognitionResult;
import com.microsoft.cognitiveservices.speech.SpeechRecognizer;
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
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        SpeechRecognizer r = s.createSpeechRecognizerWithFileInput(Settings.WaveFile);
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);
        
        r.close();
        s.close();
    }

    @Test
    public void testSpeechRecognizer2() throws InterruptedException, ExecutionException {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        WaveFileAudioInputStream ais = new WaveFileAudioInputStream(Settings.WaveFile);
        assertNotNull(ais);
        
        SpeechRecognizer r = s.createSpeechRecognizerWithStream(ais);
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);
        
        SpeechRecognitionResult res = r.recognizeAsync().get();
        assertNotNull(res);
        assertEquals("What's the weather like?", res.getText());
                
        r.close();
        s.close();
    }
    
    // -----------------------------------------------------------------------
    // --- 
    // -----------------------------------------------------------------------

    @Test
    public void testGetDeploymentId() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        WaveFileAudioInputStream ais = new WaveFileAudioInputStream(Settings.WaveFile);
        assertNotNull(ais);
        
        SpeechRecognizer r = s.createSpeechRecognizerWithStream(ais);
        assertNotNull(r);

        assertNotNull(r.getDeploymentId());
        
        r.close();
        s.close();
    }

    @Test
    public void testSetDeploymentId() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        WaveFileAudioInputStream ais = new WaveFileAudioInputStream(Settings.WaveFile);
        assertNotNull(ais);
        
        SpeechRecognizer r = s.createSpeechRecognizerWithStream(ais);
        assertNotNull(r);

        assertNotNull(r.getDeploymentId());

        String newDeploymentId = "new-" + r.getDeploymentId();
        r.setDeploymentId(newDeploymentId);
        
        assertEquals(newDeploymentId, r.getDeploymentId());
        
        r.close();
        s.close();
    }

    // -----------------------------------------------------------------------
    // --- 
    // -----------------------------------------------------------------------

    @Test
    public void testGetLanguage1() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        WaveFileAudioInputStream ais = new WaveFileAudioInputStream(Settings.WaveFile);
        assertNotNull(ais);

        SpeechRecognizer r = s.createSpeechRecognizerWithStream(ais);
        assertNotNull(r);

        assertNotNull(r.getLanguage());
        
        r.close();
        s.close();
    }

    @Test
    public void testGetLanguage2() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        WaveFileAudioInputStream ais = new WaveFileAudioInputStream(Settings.WaveFile);
        assertNotNull(ais);

        String language = "de-DE";
        SpeechRecognizer r = s.createSpeechRecognizerWithStream(ais, language);
        assertNotNull(r);

        assertNotNull(r.getLanguage());
        assertEquals(language, r.getLanguage());

        r.close();
        s.close();
    }

    // -----------------------------------------------------------------------
    // ---
    // -----------------------------------------------------------------------

    @Test
    public void testGetOutputFormatDefault() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        SpeechRecognizer r = s.createSpeechRecognizerWithFileInput(Settings.WaveFile);
        assertNotNull(r);

        assertEquals(r.getOutputFormat(), OutputFormat.Simple);

        r.close();
        s.close();
    }

    @Test
    public void testGetOutputFormatDetailed() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        String language = "de-DE";
        SpeechRecognizer r = s.createSpeechRecognizerWithFileInput(Settings.WaveFile, language, OutputFormat.Detailed);
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
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        SpeechRecognizer r = s.createSpeechRecognizerWithFileInput(Settings.WaveFile);
        assertNotNull(r);

        assertNotNull(r.getParameters());
        assertEquals(r.getLanguage(), r.getParameters().getString(RecognizerParameterNames.SpeechRecognitionLanguage));
        assertEquals(r.getDeploymentId(), r.getParameters().getString(RecognizerParameterNames.SpeechModelId)); // TODO: is this really the correct mapping?
        
        r.close();
        s.close();
    }

    // -----------------------------------------------------------------------
    // --- 
    // -----------------------------------------------------------------------

    @Test
    public void testRecognizeAsync1() throws InterruptedException, ExecutionException {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        SpeechRecognizer r = s.createSpeechRecognizerWithFileInput(Settings.WaveFile);
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);
        
        Future<SpeechRecognitionResult> future = r.recognizeAsync();
        assertNotNull(future);

        // Wait for max 10 seconds
        long now = System.currentTimeMillis();
        while(((System.currentTimeMillis() - now) < 10000) &&
              (!future.isDone() || !future.isCancelled())) {
            Thread.sleep(200);
        }

        assertFalse(future.isCancelled());
        assertTrue(future.isDone());

        SpeechRecognitionResult res = future.get();
        assertNotNull(res);
        assertEquals(RecognitionStatus.Recognized, res.getReason());
        assertEquals("What's the weather like?", res.getText());

        r.close();
        s.close();
    }

    @Ignore("TODO why does not get whats the weather like")
    @Test
    public void testRecognizeAsync2() throws InterruptedException, ExecutionException {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        SpeechRecognizer r = s.createSpeechRecognizerWithFileInput(Settings.WaveFile);
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);

        final Map<String, Integer> eventsMap = new HashMap<String, Integer>();
        
        r.FinalResultReceived.addEventListener((o, e) -> {
            eventsMap.put("FinalResultReceived", eventIdentifier.getAndIncrement());
        });

        r.IntermediateResultReceived.addEventListener((o, e) -> {
            int now = eventIdentifier.getAndIncrement();
            eventsMap.put("IntermediateResultReceived-" + System.currentTimeMillis(), now);
            eventsMap.put("IntermediateResultReceived" , now);
        });
        
        r.RecognitionErrorRaised.addEventListener((o, e) -> {
            eventsMap.put("RecognitionErrorRaised", eventIdentifier.getAndIncrement());
        });

        // TODO eventType should be renamed and be a function getEventType()
        r.RecognitionEvent.addEventListener((o, e) -> {
            int now = eventIdentifier.getAndIncrement();
            eventsMap.put(e.eventType.name() + "-" + System.currentTimeMillis(), now);
            eventsMap.put(e.eventType.name(), now);
        });

        r.SessionEvent.addEventListener((o, e) -> {
            int now = eventIdentifier.getAndIncrement();
            eventsMap.put(e.getEventType().name() + "-" + System.currentTimeMillis(), now);
            eventsMap.put(e.getEventType().name(), now);
        });
        
        // Note: TODO session stopped event not necessarily raised before async operation returns!
        //       this makes this test flaky
        SpeechRecognitionResult res = r.recognizeAsync().get();
        assertNotNull(res);
        assertEquals(RecognitionStatus.Recognized, res.getReason());
        assertTrue(res.getErrorDetails().isEmpty());
        assertEquals("What's the weather like?", res.getText());

        // session events are first and last event
        final Integer LAST_RECORDED_EVENT_ID = eventIdentifier.get();
        assertTrue(LAST_RECORDED_EVENT_ID > FIRST_EVENT_ID);
        assertEquals(FIRST_EVENT_ID, eventsMap.get(SessionEventType.SessionStartedEvent.name()));
        if(eventsMap.containsKey(SessionEventType.SessionStoppedEvent.name()))
            assertEquals(LAST_RECORDED_EVENT_ID, eventsMap.get(SessionEventType.SessionStoppedEvent.name()));
        
        // end events come after start events.
        if(eventsMap.containsKey(SessionEventType.SessionStoppedEvent.name()))
            assertTrue(eventsMap.get(SessionEventType.SessionStartedEvent.name()) < eventsMap.get(SessionEventType.SessionStoppedEvent.name()));
        assertTrue(eventsMap.get(RecognitionEventType.SpeechStartDetectedEvent.name()) < eventsMap.get(RecognitionEventType.SpeechEndDetectedEvent.name()));
        assertEquals((Integer)(FIRST_EVENT_ID + 1), eventsMap.get(RecognitionEventType.SpeechStartDetectedEvent.name()));
        
        // make sure, first end of speech, then final result
        assertEquals((Integer)(LAST_RECORDED_EVENT_ID - 2), eventsMap.get(RecognitionEventType.SpeechEndDetectedEvent.name()));
        assertEquals((Integer)(LAST_RECORDED_EVENT_ID - 1), eventsMap.get("FinalResultReceived"));

        // recognition events come after session start but before session end events
        assertTrue(eventsMap.get(SessionEventType.SessionStartedEvent.name()) < eventsMap.get(RecognitionEventType.SpeechStartDetectedEvent.name()));
        if(eventsMap.containsKey(SessionEventType.SessionStoppedEvent.name()))
            assertTrue(eventsMap.get(RecognitionEventType.SpeechEndDetectedEvent.name()) < eventsMap.get(SessionEventType.SessionStoppedEvent.name()));

        // there is no partial result reported after the final result
        // (and check that we have intermediate and final results recorded)
        if(eventsMap.containsKey("IntermediateResultReceived"))
            assertTrue(eventsMap.get("IntermediateResultReceived") > eventsMap.get(RecognitionEventType.SpeechStartDetectedEvent.name()));
        assertTrue(eventsMap.get("FinalResultReceived") < eventsMap.get(RecognitionEventType.SpeechEndDetectedEvent.name()));
        assertTrue(eventsMap.get("IntermediateResultReceived") < eventsMap.get("FinalResultReceived"));

        // make sure events we don't expect, don't get raised
        assertFalse(eventsMap.containsKey("RecognitionErrorRaised"));
        
        r.close();
        s.close();
    }

    // -----------------------------------------------------------------------
    // --- 
    // -----------------------------------------------------------------------

    @Test
    public void testStartContinuousRecognitionAsync() throws InterruptedException {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        SpeechRecognizer r = s.createSpeechRecognizerWithFileInput(Settings.WaveFile);
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);
        
        Future<?> future = r.startContinuousRecognitionAsync();
        assertNotNull(future);

        // Wait for max 10 seconds
        long now = System.currentTimeMillis();
        while(((System.currentTimeMillis() - now) < 10000) &&
              (!future.isDone() || !future.isCancelled())) {
            Thread.sleep(200);
        }

        assertFalse(future.isCancelled());
        assertTrue(future.isDone());

        r.close();
        s.close();
    }

    @Test
    public void testStopContinuousRecognitionAsync() throws InterruptedException {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        SpeechRecognizer r = s.createSpeechRecognizerWithFileInput(Settings.WaveFile);
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);
        
        Future<?> future = r.startContinuousRecognitionAsync();
        assertNotNull(future);

        // Wait for max 10 seconds
        long now = System.currentTimeMillis();
        while(((System.currentTimeMillis() - now) < 10000) &&
              (!future.isDone() || !future.isCancelled())) {
            Thread.sleep(200);
        }

        assertFalse(future.isCancelled());
        assertTrue(future.isDone());

        // just wait one second
        Thread.sleep(1000);

        future = r.stopContinuousRecognitionAsync();
        assertNotNull(future);

        // Wait for max 10 seconds
        now = System.currentTimeMillis();
        while(((System.currentTimeMillis() - now) < 10000) &&
              (!future.isDone() || !future.isCancelled())) {
            Thread.sleep(200);
        }

        assertFalse(future.isCancelled());
        assertTrue(future.isDone());

        r.close();
        s.close();
    }

    @Test
    public void testStartStopContinuousRecognitionAsync() throws InterruptedException {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        SpeechRecognizer r = s.createSpeechRecognizerWithFileInput(Settings.WaveFile);
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);
        
        final ArrayList<String> rEvents = new ArrayList<>();

        r.FinalResultReceived.addEventListener((o, e) -> {
            rEvents.add("Result@" + System.currentTimeMillis());
        });
        
        Future<?> future = r.startContinuousRecognitionAsync();
        assertNotNull(future);

        // Wait for max 10 seconds
        long now = System.currentTimeMillis();
        while(((System.currentTimeMillis() - now) < 10000) &&
              (!future.isDone() || !future.isCancelled())) {
            Thread.sleep(200);
        }

        assertFalse(future.isCancelled());
        assertTrue(future.isDone());

        // wait until we get at least on final result
        now = System.currentTimeMillis();
        while(((System.currentTimeMillis() - now) < 10000) &&
              (rEvents.isEmpty())) {
            Thread.sleep(200);
        }

        // test that we got one result
        // TODO multi-phrase test with several phrases in one session
        assertEquals(1, rEvents.size());

        future = r.stopContinuousRecognitionAsync();
        assertNotNull(future);

        // Wait for max 10 seconds
        now = System.currentTimeMillis();
        while(((System.currentTimeMillis() - now) < 10000) &&
              (!future.isDone() || !future.isCancelled())) {
            Thread.sleep(200);
        }

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
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        SpeechRecognizer r = s.createSpeechRecognizerWithFileInput(Settings.WaveFile);
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
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        SpeechRecognizer r = s.createSpeechRecognizerWithFileInput(Settings.WaveFile);
        assertNotNull(r);
        assertTrue(r instanceof Recognizer);
                
        r.close();
        s.close();
    }
}
