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
import com.microsoft.cognitiveservices.speech.RecognitionEventType;
import com.microsoft.cognitiveservices.speech.RecognitionStatus;
import com.microsoft.cognitiveservices.speech.Recognizer;
import com.microsoft.cognitiveservices.speech.SessionEventType;
import com.microsoft.cognitiveservices.speech.SpeechConfig;
import com.microsoft.cognitiveservices.speech.SpeechPropertyId;
import com.microsoft.cognitiveservices.speech.intent.IntentRecognitionResult;
import com.microsoft.cognitiveservices.speech.intent.IntentRecognizer;
import com.microsoft.cognitiveservices.speech.intent.LanguageUnderstandingModel;

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
        
        IntentRecognitionResult res = r.recognizeAsync().get();
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

        assertNotNull(r.getParameters());
        assertEquals(r.getSpeechRecognitionLanguage(), r.getParameters().getProperty(SpeechPropertyId.SpeechServiceConnection_RecoLanguage));
        
        r.close();
        s.close();
    }

    // -----------------------------------------------------------------------
    // --- 
    // -----------------------------------------------------------------------

    @Ignore("TODO why is Canceled reported instead of success")
    @Test
    public void testRecognizeAsync1() throws InterruptedException, ExecutionException, TimeoutException {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        IntentRecognizer r = new IntentRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);
        
        Future<IntentRecognitionResult> future = r.recognizeAsync();
        assertNotNull(future);

        // Wait for max 30 seconds
        future.get(30, TimeUnit.SECONDS);

        assertFalse(future.isCancelled());
        assertTrue(future.isDone());

        IntentRecognitionResult res = future.get();
        assertNotNull(res);
        assertEquals(RecognitionStatus.Recognized, res.getReason());
        assertEquals("What's the weather like?", res.getText());

        // TODO: check for specific json parameters
        assertTrue(res.getLanguageUnderstanding().length() > 0);
        assertEquals(RecognitionStatus.Recognized, res.getReason());
        
        r.close();
        s.close();
    }

    @Ignore("TODO why are error details not empty")
    @Test
    public void testRecognizeAsync2() throws InterruptedException, ExecutionException {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        IntentRecognizer r = new IntentRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));
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
        
        IntentRecognitionResult res = r.recognizeAsync().get();
        assertNotNull(res);
        assertTrue(res.getErrorDetails().isEmpty());
        assertEquals("What's the weather like?", res.getText());

        // session events are first and last event
        final Integer LAST_RECORDED_EVENT_ID = eventIdentifier.get();
        assertTrue(LAST_RECORDED_EVENT_ID > FIRST_EVENT_ID);
        assertEquals(FIRST_EVENT_ID, eventsMap.get(RecognitionEventType.SpeechStartDetectedEvent.name()));
        assertEquals(LAST_RECORDED_EVENT_ID, eventsMap.get(RecognitionEventType.SpeechEndDetectedEvent.name()));
        
        // end events come after start events.
        assertTrue(eventsMap.get(SessionEventType.SessionStartedEvent.name()) < eventsMap.get(SessionEventType.SessionStoppedEvent.name()));
        assertTrue(eventsMap.get(RecognitionEventType.SpeechStartDetectedEvent.name()) < eventsMap.get(RecognitionEventType.SpeechEndDetectedEvent.name()));

        // recognition events come after session start but before session end events
        assertTrue(eventsMap.get(SessionEventType.SessionStartedEvent.name()) < eventsMap.get(RecognitionEventType.SpeechStartDetectedEvent.name()));
        assertTrue(eventsMap.get(RecognitionEventType.SpeechEndDetectedEvent.name()) < eventsMap.get(SessionEventType.SessionStoppedEvent.name()));

        // there is no partial result reported after the final result
        // (and check that we have intermediate and final results recorded)
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
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);
        
        final ArrayList<String> rEvents = new ArrayList<>();

        r.FinalResultReceived.addEventListener((o, e) -> {
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

    @Ignore("TODO why is mapsize not 2")
    @Test
    public void testAddIntentStringString() throws InterruptedException, ExecutionException {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        IntentRecognizer r = new IntentRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));
        assertNotNull(r);

        // TODO check if intent is recognized
        r.addIntent("all intents", "");

        final Map<String, Integer> eventsMap = new HashMap<String, Integer>();
        
        r.FinalResultReceived.addEventListener((o, e) -> {
            eventsMap.put("FinalResultReceived", eventIdentifier.getAndIncrement());
            if(!e.getResult().getLanguageUnderstanding().isEmpty()) {
                eventsMap.put("IntentReceived", eventIdentifier.getAndIncrement());
            }
        });

        // TODO why does this call require exceptions?
        // TODO check for specific intent.
        IntentRecognitionResult res = r.recognizeAsync().get();
        assertNotNull(res);
        assertEquals(2, eventsMap.size());
        assertTrue(res.getLanguageUnderstanding().length() > 0);
        assertEquals(RecognitionStatus.Recognized, res.getReason());
        
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
        r.addIntent("all intents", model, "");

        final Map<String, Integer> eventsMap = new HashMap<String, Integer>();
        
        r.FinalResultReceived.addEventListener((o, e) -> {
            eventsMap.put("FinalResultReceived", eventIdentifier.getAndIncrement());
            if(!e.getResult().getLanguageUnderstanding().isEmpty()) {
                eventsMap.put("IntentReceived", eventIdentifier.getAndIncrement());
            }
        });

        // TODO why does this call require exceptions?
        // TODO check for specific intent.
        IntentRecognitionResult res = r.recognizeAsync().get();
        assertNotNull(res);
        assertEquals(2, eventsMap.size());
        assertTrue(res.getLanguageUnderstanding().length() > 0);
        assertEquals(RecognitionStatus.Recognized, res.getReason());
        
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
