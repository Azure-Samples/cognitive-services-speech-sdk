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
import java.util.concurrent.atomic.AtomicInteger;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.Ignore;

import com.microsoft.cognitiveservices.speech.RecognitionEventType;
import com.microsoft.cognitiveservices.speech.RecognitionStatus;
import com.microsoft.cognitiveservices.speech.Recognizer;
import com.microsoft.cognitiveservices.speech.RecognizerParameterNames;
import com.microsoft.cognitiveservices.speech.SessionEventType;
import com.microsoft.cognitiveservices.speech.SpeechFactory;
import com.microsoft.cognitiveservices.speech.translation.TranslationRecognizer;
import com.microsoft.cognitiveservices.speech.translation.TranslationStatus;
import com.microsoft.cognitiveservices.speech.translation.TranslationTextResult;

import tests.Settings;

public class TranslationRecognizerTests {
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

    @Ignore // TODO not working with microphone
    @Test
    public void testTranslationRecognizer1() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        ArrayList<String> targets = new ArrayList<>();
        targets.add("en-US");
        
        TranslationRecognizer r = s.createTranslationRecognizer("en-US", targets);
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);
                
        r.close();
        s.close();
    }

    @Test
    public void testTranslationRecognizer2() throws InterruptedException, ExecutionException {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        WaveFileAudioInputStream ais = new WaveFileAudioInputStream(Settings.WaveFile);
        assertNotNull(ais);
        
        ArrayList<String> targets = new ArrayList<>();
        targets.add("en-US");        
        
        TranslationRecognizer r = s.createTranslationRecognizerWithStream(ais, "en-US", targets);
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);
        
        r.close();
        s.close();
    }

    // -----------------------------------------------------------------------
    // --- 
    // -----------------------------------------------------------------------

    @Test
    public void testGetSourceLanguage() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        WaveFileAudioInputStream ais = new WaveFileAudioInputStream(Settings.WaveFile);
        assertNotNull(ais);
        
        ArrayList<String> targets = new ArrayList<>();
        targets.add("en-US");        
        
        String language = "en-US";
        TranslationRecognizer r = s.createTranslationRecognizerWithStream(ais, language, targets);
        assertTrue(!r.getSourceLanguage().isEmpty());
        assertEquals(language, r.getSourceLanguage());

        r.close();
        s.close();
    }

    @Ignore // TODO why is number translations not 1 (FIX JAVA LIB FORWARD PROPERTY)
    @Test
    public void testGetTargetLanguages() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        WaveFileAudioInputStream ais = new WaveFileAudioInputStream(Settings.WaveFile);
        assertNotNull(ais);
        
        ArrayList<String> targets = new ArrayList<>();
        targets.add("en-US");        
        
        String language = "en-US";
        TranslationRecognizer r = s.createTranslationRecognizerWithStream(ais, language, targets);
        assertEquals(1, r.getTargetLanguages().size());
        assertEquals(targets.get(0), r.getTargetLanguages().get(0));

        r.close();
        s.close();
    }

    // -----------------------------------------------------------------------
    // --- 
    // -----------------------------------------------------------------------

    @Test
    public void testIsVoiceOutputDesired1() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        WaveFileAudioInputStream ais = new WaveFileAudioInputStream(Settings.WaveFile);
        assertNotNull(ais);
        
        ArrayList<String> targets = new ArrayList<>();
        targets.add("en-US");        
        
        String language = "en-US";
        TranslationRecognizer r = s.createTranslationRecognizerWithStream(ais, language, targets);

        assertFalse(r.isVoiceOutputDesired());

        r.close();
        s.close();
    }

    @Ignore // TODO why is voiceoutput desired not true (FIX JAVA LIB IMPL!!)
    @Test
    public void testIsVoiceOutputDesired2() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        WaveFileAudioInputStream ais = new WaveFileAudioInputStream(Settings.WaveFile);
        assertNotNull(ais);
        
        ArrayList<String> targets = new ArrayList<>();
        targets.add("en-US");        
        
        String language = "en-US";
        TranslationRecognizer r = s.createTranslationRecognizerWithStream(ais, language, targets, "voice");

        assertTrue(r.isVoiceOutputDesired());

        r.close();
        s.close();
    }

    @Test
    public void testIsVoiceOutputDesiredBoolean() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        WaveFileAudioInputStream ais = new WaveFileAudioInputStream(Settings.WaveFile);
        assertNotNull(ais);
        
        ArrayList<String> targets = new ArrayList<>();
        targets.add("en-US");        
        
        String language = "en-US";
        TranslationRecognizer r = s.createTranslationRecognizerWithStream(ais, language, targets);

        assertFalse(r.isVoiceOutputDesired());

        r.isVoiceOutputDesired(true);

        assertTrue(r.isVoiceOutputDesired());

        r.close();
        s.close();
    }

    // -----------------------------------------------------------------------
    // --- 
    // -----------------------------------------------------------------------

    @Ignore // TODO why is number translations not 1 (FIX JAVA LIB FORWARD PROPERTY)
    @Test
    public void testGetParameters() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        ArrayList<String> targets = new ArrayList<>();
        targets.add("en-US");        
        
        TranslationRecognizer r = s.createTranslationRecognizerWithFileInput(Settings.WaveFile, "en-US", targets);
        assertNotNull(r);

        assertNotNull(r.getParameters());
        assertEquals(r.getSourceLanguage(), r.getParameters().getString(RecognizerParameterNames.TranslationFromLanguage));
        
        // TODO this cannot be true, right? comparing an array with a string parameter???
        assertEquals(1, r.getTargetLanguages().size());
        assertEquals(r.getTargetLanguages().get(0), r.getParameters().getString(RecognizerParameterNames.TranslationToLanguage));
        
        r.close();
        s.close();
    }

    // -----------------------------------------------------------------------
    // --- 
    // -----------------------------------------------------------------------

    @Ignore // TODO why is number translations not 1 (FIX JAVA LIB FORWARD PROPERTY)
    @Test
    public void testRecognizeAsync1() throws InterruptedException, ExecutionException {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        ArrayList<String> targets = new ArrayList<>();
        targets.add("en-US");        
        
        TranslationRecognizer r = s.createTranslationRecognizerWithFileInput(Settings.WaveFile, "en-US", targets);
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);
        
        Future<TranslationTextResult> future = r.recognizeAsync();
        assertNotNull(future);

        // Wait for max 10 seconds
        long now = System.currentTimeMillis();
        while(((System.currentTimeMillis() - now) < 10000) &&
              (!future.isDone() || !future.isCancelled())) {
            Thread.sleep(200);
        }

        assertFalse(future.isCancelled());
        assertTrue(future.isDone());

        TranslationTextResult res = future.get();
        assertNotNull(res);
        assertEquals(RecognitionStatus.Success, res.getReason());
        assertEquals("What's the weather like?", res.getText());

        assertTrue(res.getErrorDetails().isEmpty());
        assertEquals(RecognitionStatus.Success, res.getReason());
        assertNotNull(res.getProperties());
        assertEquals("What's the weather like?", res.getText()); // original text
        assertEquals(1, res.getTranslations().size());
        assertEquals("What's the weather like?", res.getTranslations().get("en-US")); // translated text
        assertEquals(TranslationStatus.Success, res.getTranslationStatus());

        r.close();
        s.close();
    }

    @Ignore // TODO why is event order wrong?
    @Test
    public void testRecognizeAsync2() throws InterruptedException, ExecutionException {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        ArrayList<String> targets = new ArrayList<>();
        targets.add("en-US");        
        
        TranslationRecognizer r = s.createTranslationRecognizerWithFileInput(Settings.WaveFile, "en-US", targets);
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
        
        TranslationTextResult res = r.recognizeAsync().get();
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
    public void testStartContinuousRecognitionAsync() throws InterruptedException {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        ArrayList<String> targets = new ArrayList<>();
        targets.add("en-US");        
        
        TranslationRecognizer r = s.createTranslationRecognizerWithFileInput(Settings.WaveFile, "en-US", targets);
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

        ArrayList<String> targets = new ArrayList<>();
        targets.add("en-US");        
        
        TranslationRecognizer r = s.createTranslationRecognizerWithFileInput(Settings.WaveFile, "en-US", targets);
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

    @Ignore // TODO why is the event number not 1
    @Test
    public void testStartStopContinuousRecognitionAsync() throws InterruptedException {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        ArrayList<String> targets = new ArrayList<>();
        targets.add("en-US");        
        
        TranslationRecognizer r = s.createTranslationRecognizerWithFileInput(Settings.WaveFile, "en-US", targets);
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

    @Ignore // TODO not working with microphone
    @Test
    public void testGetRecoImpl() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        ArrayList<String> targets = new ArrayList<>();
        targets.add("en-US");
        
        TranslationRecognizer r = s.createTranslationRecognizer("en-US", targets);
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);
                
        r.close();
        s.close();
    }
}
