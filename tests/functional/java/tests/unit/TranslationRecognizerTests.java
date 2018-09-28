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
import com.microsoft.cognitiveservices.speech.SpeechRecognizer;
import com.microsoft.cognitiveservices.speech.Recognizer;
import com.microsoft.cognitiveservices.speech.PropertyId;
import com.microsoft.cognitiveservices.speech.translation.SpeechTranslationConfig;
import com.microsoft.cognitiveservices.speech.translation.TranslationRecognizer;
import com.microsoft.cognitiveservices.speech.translation.TranslationRecognitionResult;

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

    @Ignore("TODO not working with microphone")
    @Test
    public void testTranslationRecognizer1() {
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);
        s.setSpeechRecognitionLanguage("en-US");
        s.addTargetLanguage("de");

        TranslationRecognizer r = new TranslationRecognizer(s);
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);

        r.close();
        s.close();
    }

    @Test
    public void testTranslationRecognizer2() throws InterruptedException, ExecutionException {
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.WavFile);
        assertNotNull(ais);

        s.setSpeechRecognitionLanguage("en-US");
        s.addTargetLanguage("de");

        TranslationRecognizer r = new TranslationRecognizer(s, AudioConfig.fromStreamInput(ais));
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
    public void testGetSpeechRecognitionLanguage() {
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.WavFile);
        assertNotNull(ais);

        String language = "en-US";
        s.setSpeechRecognitionLanguage(language);
        s.addTargetLanguage("de");

        TranslationRecognizer r = new TranslationRecognizer(s, AudioConfig.fromStreamInput(ais));
        assertTrue(!r.getSpeechRecognitionLanguage().isEmpty());
        assertEquals(language, r.getSpeechRecognitionLanguage());

        r.close();
        s.close();
    }

    @Ignore("TODO why is number translations not 1 (FIX JAVA LIB FORWARD PROPERTY)")
    @Test
    public void testGetTargetLanguages() {
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.WavFile);
        assertNotNull(ais);

        String language = "en-US";
        s.setSpeechRecognitionLanguage(language);
        s.addTargetLanguage(language);

        TranslationRecognizer r = new TranslationRecognizer(s, AudioConfig.fromStreamInput(ais));
        assertEquals(1, r.getTargetLanguages().size());
        assertEquals(language, r.getTargetLanguages().get(0));

        r.close();
        s.close();
    }

    // -----------------------------------------------------------------------
    // ---
    // -----------------------------------------------------------------------

    @Test
    public void testGetOutputVoiceNameNoSetting() {
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.WavFile);
        assertNotNull(ais);

        String language = "en-US";
        s.setSpeechRecognitionLanguage(language);
        s.addTargetLanguage(language);
        TranslationRecognizer r = new TranslationRecognizer(s, AudioConfig.fromStreamInput(ais));

        assertTrue(r.getVoiceName().isEmpty());

        r.close();
        s.close();
    }

    @Test
    public void testGetVoiceName() {
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.WavFile);
        assertNotNull(ais);

        String language = "en-US";
        String voice = "de-DE-Katja";
        s.setSpeechRecognitionLanguage(language);
        s.addTargetLanguage(language);
        s.setVoiceName(voice);

        TranslationRecognizer r = new TranslationRecognizer(s, AudioConfig.fromStreamInput(ais));

        assertEquals(r.getVoiceName(), voice);

        r.close();
        s.close();
    }

    // -----------------------------------------------------------------------
    // ---
    // -----------------------------------------------------------------------

    @Ignore("TODO why is number translations not 1 (FIX JAVA LIB FORWARD PROPERTY)")
    @Test
    public void testGetParameters() {
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        String language = "en-US";
        s.setSpeechRecognitionLanguage(language);
        s.addTargetLanguage(language);

        TranslationRecognizer r = new TranslationRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));
        assertNotNull(r);

        assertNotNull(r.getProperties());
        assertEquals(r.getSpeechRecognitionLanguage(), r.getProperties().getProperty(PropertyId.SpeechServiceConnection_RecoLanguage));

        // TODO this cannot be true, right? comparing an array with a string parameter???
        assertEquals(1, r.getTargetLanguages().size());
        assertEquals(r.getTargetLanguages().get(0), r.getProperties().getProperty(PropertyId.SpeechServiceConnection_TranslationToLanguages));

        r.close();
        s.close();
    }

    // -----------------------------------------------------------------------
    // ---
    // -----------------------------------------------------------------------

    @Ignore("TODO why is number translations not 1 (FIX JAVA LIB FORWARD PROPERTY)")
    @Test
    public void testRecognizeOnceAsync1() throws InterruptedException, ExecutionException, TimeoutException {
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        String language = "en-US";
        s.setSpeechRecognitionLanguage(language);
        s.addTargetLanguage(language);

        TranslationRecognizer r = new TranslationRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);

        Future<TranslationRecognitionResult> future = r.recognizeOnceAsync();
        assertNotNull(future);

        // Wait for max 30 seconds
        future.get(30, TimeUnit.SECONDS);

        assertFalse(future.isCancelled());
        assertTrue(future.isDone());

        TranslationRecognitionResult res = future.get();
        assertNotNull(res);
        assertTrue(ResultReason.RecognizedSpeech == res.getReason() ||
                ResultReason.RecognizedIntent == res.getReason());
        assertEquals("What's the weather like?", res.getText());

        assertNotNull(res.getProperties());
        assertEquals("What's the weather like?", res.getText()); // original text
        assertEquals(1, res.getTranslations().size());
        assertEquals("What's the weather like?", res.getTranslations().get("en-US")); // translated text

        r.close();
        s.close();
    }

    @Ignore("TODO why is event order wrong?")
    @Test
    public void testRecognizeOnceAsync2() throws InterruptedException, ExecutionException {
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        String language = "en-US";
        s.setSpeechRecognitionLanguage(language);
        s.addTargetLanguage(language);

        TranslationRecognizer r = new TranslationRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));
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

        // TODO there is no guarantee that SessionStoppedEvent comes before the recognizeOnceAsync() call returns?!
        //      this is why below SessionStoppedEvent checks are conditional
        TranslationRecognitionResult res = r.recognizeOnceAsync().get();
        assertNotNull(res);
        assertTrue(res.getReason() != ResultReason.Canceled);
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
        assertEquals((Integer)(LAST_RECORDED_EVENT_ID - 1), eventsMap.get("speechEndDetected"));

        // recognition events come after session start but before session end events
        assertTrue(eventsMap.get("sessionStarted") < eventsMap.get("speechStartDetected"));
        if(eventsMap.containsKey("sessionStopped"))
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
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        String language = "en-US";
        s.setSpeechRecognitionLanguage(language);
        s.addTargetLanguage(language);

        TranslationRecognizer r = new TranslationRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));
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
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        String language = "en-US";
        s.setSpeechRecognitionLanguage(language);
        s.addTargetLanguage(language);

        TranslationRecognizer r = new TranslationRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);

        Future<?> future = r.startContinuousRecognitionAsync();
        assertNotNull(future);

        // Wait for max 30 seconds
        future.get(30, TimeUnit.SECONDS);

        assertFalse(future.isCancelled());
        assertTrue(future.isDone());

        // just wait ten seconds
        Thread.sleep(10000);

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
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        String language = "en-US";
        s.setSpeechRecognitionLanguage(language);
        s.addTargetLanguage(language);

        TranslationRecognizer r = new TranslationRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));
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

    @Ignore("TODO not working with microphone")
    @Test
    public void testGetRecoImpl() {
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        String language = "en-US";
        s.setSpeechRecognitionLanguage(language);
        s.addTargetLanguage(language);

        TranslationRecognizer r = new TranslationRecognizer(s, null);
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);

        r.close();
        s.close();
    }
}
