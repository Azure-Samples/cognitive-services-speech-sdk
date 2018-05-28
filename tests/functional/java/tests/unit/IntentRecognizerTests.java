package tests.unit;

import static org.junit.Assert.*;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.Future;
import java.util.concurrent.atomic.AtomicInteger;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

import com.microsoft.cognitiveservices.speech.RecognitionEventType;
import com.microsoft.cognitiveservices.speech.RecognitionStatus;
import com.microsoft.cognitiveservices.speech.Recognizer;
import com.microsoft.cognitiveservices.speech.RecognizerParameterNames;
import com.microsoft.cognitiveservices.speech.SessionEventType;
import com.microsoft.cognitiveservices.speech.SpeechFactory;
import com.microsoft.cognitiveservices.speech.intent.IntentRecognitionResult;
import com.microsoft.cognitiveservices.speech.intent.IntentRecognizer;
import com.microsoft.cognitiveservices.speech.intent.LanguageUnderstandingModel;

import tests.Settings;

public class IntentRecognizerTests {
    private final Integer FIRST_EVENT_ID = 1;
    private AtomicInteger _eventId = new AtomicInteger(FIRST_EVENT_ID);
    
    @BeforeClass
    static public void setUpBeforeClass() throws Exception {
        // Override inputs, if necessary
        Settings.LoadSettings();
    }

    @AfterClass
    static public void tearDownAfterClass() throws Exception {
    }

    @Before
    public void setUp() throws Exception {
    }

    @After
    public void tearDown() throws Exception {
    }

    // -----------------------------------------------------------------------
    // --- 
    // -----------------------------------------------------------------------
    
    @Test
    public void testDispose() {
        // TODO: make dispose method public
    }

    // -----------------------------------------------------------------------
    // --- 
    // -----------------------------------------------------------------------
    
    @Test
    public void testIntentRecognizer1() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        IntentRecognizer r = s.createIntentRecognizer(Settings.WaveFile);
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);
                
        r.close();
        s.close();
    }
    
    @Test
    public void testIntentRecognizer2() throws InterruptedException, ExecutionException {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        WaveFileAudioInputStream ais = new WaveFileAudioInputStream(Settings.WaveFile);
        assertNotNull(ais);
        
        IntentRecognizer r = s.createIntentRecognizer(ais);
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
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        WaveFileAudioInputStream ais = new WaveFileAudioInputStream(Settings.WaveFile);
        assertNotNull(ais);

        String language = "en-US";
        IntentRecognizer r = s.createIntentRecognizer(ais, language);
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);

        assertTrue(r.getLanguage().isEmpty());
        assertEquals(language, r.getLanguage());
        
        r.close();
        s.close();
    }

    @Test
    public void testSetLanguage() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        WaveFileAudioInputStream ais = new WaveFileAudioInputStream(Settings.WaveFile);
        assertNotNull(ais);

        String language = "en-US";
        IntentRecognizer r = s.createIntentRecognizer(ais, language);
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);

        assertTrue(r.getLanguage().isEmpty());
        assertEquals(language, r.getLanguage());

        String language2 = "de-DE";
        r.setLanguage(language2);

        assertTrue(r.getLanguage().isEmpty());
        assertEquals(language2, r.getLanguage());

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

        IntentRecognizer r = s.createIntentRecognizer(Settings.WaveFile);
        assertNotNull(r);

        assertNotNull(r.getParameters());
        assertEquals(r.getLanguage(), r.getParameters().getString(RecognizerParameterNames.SpeechRecognitionLanguage));
        
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

        IntentRecognizer r = s.createIntentRecognizer(Settings.WaveFile);
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);
        
        Future<IntentRecognitionResult> future = r.recognizeAsync();
        assertNotNull(future);

        // Wait for max 10 seconds
        long now = System.currentTimeMillis();
        while(((System.currentTimeMillis() - now) < 10000) &&
              (!future.isDone() || !future.isCancelled())) {
            Thread.sleep(200);
        }

        assertFalse(future.isCancelled());
        assertTrue(future.isDone());

        IntentRecognitionResult res = future.get();
        assertNotNull(res);
        assertEquals(RecognitionStatus.Success, res.getReason());
        assertEquals("What's the weather like?", res.getText());

        // TODO: check for specific json parameters
        assertTrue(res.getLanguageUnderstanding().length() > 0);
        assertEquals(RecognitionStatus.Success, res.getReason());
        
        r.close();
        s.close();
    }

    @Test
    public void testRecognizeAsync2() throws InterruptedException, ExecutionException {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        IntentRecognizer r = s.createIntentRecognizer(Settings.WaveFile);
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);

        final Map<String, Integer> eventsMap = new HashMap<String, Integer>();
        
        r.FinalResultReceived.addEventListener((o, e) -> {
            eventsMap.put("FinalResultReceived", _eventId.getAndIncrement());
        });

        r.IntermediateResultReceived.addEventListener((o, e) -> {
            int now = _eventId.getAndIncrement();
            eventsMap.put("IntermediateResultReceived-" + System.currentTimeMillis(), now);
            eventsMap.put("IntermediateResultReceived" , now);
        });
        
        r.RecognitionErrorRaised.addEventListener((o, e) -> {
            eventsMap.put("RecognitionErrorRaised", _eventId.getAndIncrement());
        });

        // TODO eventType should be renamed and be a function getEventType()
        r.RecognitionEvent.addEventListener((o, e) -> {
            int now = _eventId.getAndIncrement();
            eventsMap.put(e.eventType.name() + "-" + System.currentTimeMillis(), now);
            eventsMap.put(e.eventType.name(), now);
        });

        r.SessionEvent.addEventListener((o, e) -> {
            int now = _eventId.getAndIncrement();
            eventsMap.put(e.getEventType().name() + "-" + System.currentTimeMillis(), now);
            eventsMap.put(e.getEventType().name(), now);
        });
        
        IntentRecognitionResult res = r.recognizeAsync().get();
        assertNotNull(res);
        assertTrue(res.getErrorDetails().isEmpty());
        assertEquals("What's the weather like?", res.getText());

        // session events are first and last event
        final Integer LAST_RECORDED_EVENT_ID = _eventId.get();
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
    public void testStartContinuousRecognitionAsync() throws InterruptedException {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        IntentRecognizer r = s.createIntentRecognizer(Settings.WaveFile);
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

        IntentRecognizer r = s.createIntentRecognizer(Settings.WaveFile);
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

        IntentRecognizer r = s.createIntentRecognizer(Settings.WaveFile);
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
        assertTrue(rEvents.size() == 1);

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
    public void testAddIntentStringString() throws InterruptedException, ExecutionException {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        IntentRecognizer r = s.createIntentRecognizer(Settings.WaveFile);
        assertNotNull(r);

        // TODO check if intent is recognized
        r.addIntent("all intents", "");

        final Map<String, Integer> eventsMap = new HashMap<String, Integer>();
        
        r.FinalResultReceived.addEventListener((o, e) -> {
            eventsMap.put("FinalResultReceived", _eventId.getAndIncrement());
            if(!e.getResult().getLanguageUnderstanding().isEmpty()) {
                eventsMap.put("IntentReceived", _eventId.getAndIncrement());
            }
        });

        // TODO why does this call require exceptions?
        // TODO check for specific intent.
        IntentRecognitionResult res = r.recognizeAsync().get();
        assertNotNull(res);
        assertEquals(2, eventsMap.size());
        assertTrue(res.getLanguageUnderstanding().length() > 0);
        assertEquals(RecognitionStatus.Success, res.getReason());
        
        r.close();
        s.close();
    }

    @Test
    public void testAddIntentStringLanguageUnderstandingModelString() throws InterruptedException, ExecutionException {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        IntentRecognizer r = s.createIntentRecognizer(Settings.WaveFile);
        assertNotNull(r);

        LanguageUnderstandingModel model = LanguageUnderstandingModel.fromSubscription(Settings.LuisSubscriptionKey, Settings.LuisAppId, Settings.LuisRegion);
        assertNotNull(model);
        
        // TODO check if intent is recognized
        // TODO what is the intent name?
        r.addIntent("all intents", model, "");

        final Map<String, Integer> eventsMap = new HashMap<String, Integer>();
        
        r.FinalResultReceived.addEventListener((o, e) -> {
            eventsMap.put("FinalResultReceived", _eventId.getAndIncrement());
            if(!e.getResult().getLanguageUnderstanding().isEmpty()) {
                eventsMap.put("IntentReceived", _eventId.getAndIncrement());
            }
        });

        // TODO why does this call require exceptions?
        // TODO check for specific intent.
        IntentRecognitionResult res = r.recognizeAsync().get();
        assertNotNull(res);
        assertEquals(2, eventsMap.size());
        assertTrue(res.getLanguageUnderstanding().length() > 0);
        assertEquals(RecognitionStatus.Success, res.getReason());
        
        r.close();
        s.close();
    }

    // -----------------------------------------------------------------------
    // --- 
    // -----------------------------------------------------------------------
/* TODO does not work outside of android for now    
    @Test
    public void testStartKeywordRecognitionAsync() {
        fail("Not yet implemented");
    }

    @Test
    public void testStopKeywordRecognitionAsync() {
        fail("Not yet implemented");
    }
*/
    // -----------------------------------------------------------------------
    // --- 
    // -----------------------------------------------------------------------
    
    @Test
    public void testGetRecoImpl() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        IntentRecognizer r = s.createIntentRecognizer(Settings.WaveFile);
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);
                
        r.close();
        s.close();
    }
}
