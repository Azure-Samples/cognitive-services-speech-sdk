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
import java.net.URI;

import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.Ignore;

import com.microsoft.cognitiveservices.speech.audio.AudioConfig;
import com.microsoft.cognitiveservices.speech.CancellationDetails;
import com.microsoft.cognitiveservices.speech.CancellationReason;
import com.microsoft.cognitiveservices.speech.ResultReason;
import com.microsoft.cognitiveservices.speech.ServicePropertyChannel;
import com.microsoft.cognitiveservices.speech.Recognizer;
import com.microsoft.cognitiveservices.speech.PropertyId;
import com.microsoft.cognitiveservices.speech.translation.SpeechTranslationConfig;
import com.microsoft.cognitiveservices.speech.translation.TranslationRecognizer;
import com.microsoft.cognitiveservices.speech.translation.TranslationRecognitionResult;
import com.microsoft.cognitiveservices.speech.Connection;

import tests.Settings;
import tests.TestHelper;

public class TranslationRecognizerTests {
    private final Integer FIRST_EVENT_ID = 1;
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
        String voice = "Microsoft Server Speech Text to Speech Voice (de-DE, Hedda)";
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

        assertEquals(1, r.getTargetLanguages().size());
        assertEquals(r.getTargetLanguages().get(0), r.getProperties().getProperty(PropertyId.SpeechServiceConnection_TranslationToLanguages));

        r.close();
        s.close();
    }

    // -----------------------------------------------------------------------
    // ---
    // -----------------------------------------------------------------------

    @Test
    public void testRecognizeOnceAsync1() throws InterruptedException, ExecutionException, TimeoutException {
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        String language = "en-US";
        s.setSpeechRecognitionLanguage(language);
        s.addTargetLanguage(language);

        TranslationRecognizer r = new TranslationRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));
        Connection connection = Connection.fromRecognizer(r);
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);

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

        Future<TranslationRecognitionResult> future = r.recognizeOnceAsync();
        // Wait for max 30 seconds
        TranslationRecognitionResult res = future.get(30, TimeUnit.SECONDS);
        assertNotNull(res);
        TestHelper.OutputResult(res);
        assertTrue(ResultReason.TranslatedSpeech == res.getReason());
        assertEquals("What's the weather like?", res.getText());

        assertNotNull(res.getProperties());
        assertEquals(1, res.getTranslations().size());
        assertEquals("What's the weather like?", res.getTranslations().get("en")); // translated text

        // wait until we get the SessionStopped event.
        long now = System.currentTimeMillis();
        while(((System.currentTimeMillis() - now) < 30000) && (sessionStoppedCount.get() == 0)) {
            Thread.sleep(200);
        }
        TestHelper.AssertConnectionCountMatching(connectedEventCount.get(), disconnectedEventCount.get());

        // It is not required to explictly close the connection. This is also used to keep the connection object alive.
        connection.closeConnection();
        connection.close();
        r.close();
        s.close();
    }

    @Test
    public void testRecognizeOnceAsync2() throws InterruptedException, ExecutionException {
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        String language = "en-US";
        s.setSpeechRecognitionLanguage(language);
        s.addTargetLanguage(language);

        TranslationRecognizer r = new TranslationRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));
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

        TranslationRecognitionResult res = r.recognizeOnceAsync().get();
        assertNotNull(res);
        TestHelper.OutputResult(res);
        assertTrue(res.getReason() == ResultReason.TranslatedSpeech);
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
        assertEquals(LAST_RECORDED_EVENT_ID, eventsMap.get("sessionStopped"));

        // end events come after start events.
        assertTrue(eventsMap.get("sessionStarted") < eventsMap.get("sessionStopped"));
        assertTrue(eventsMap.get("speechStartDetected") < eventsMap.get("speechEndDetected"));
        assertEquals((Integer)(FIRST_EVENT_ID + 1), eventsMap.get("speechStartDetected"));
        assertEquals((Integer)(LAST_RECORDED_EVENT_ID - 1), eventsMap.get("speechEndDetected"));
        assertEquals((Integer)(LAST_RECORDED_EVENT_ID - 2), eventsMap.get("recognized"));

        // recognition events come after session start but before session end events
        assertTrue(eventsMap.get("sessionStarted") < eventsMap.get("speechStartDetected"));
        assertTrue(eventsMap.get("speechEndDetected") < eventsMap.get("sessionStopped"));

        // there is no partial result reported after the final result
        // (and check that we have intermediate and final results recorded)
        assertTrue(eventsMap.get("recognizing") < eventsMap.get("recognized"));

        // make sure events we don't expect, don't get raised
        assertFalse(eventsMap.containsKey("canceled"));

        connection.close();
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
        assertNotNull("future is null.", future);
        // Wait for max 30 seconds
        future.get(30, TimeUnit.SECONDS);
        assertFalse("future is canceled.", future.isCancelled());
        assertTrue("future is not done.", future.isDone());

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
        assertNotNull("future is null.", future);
        // Wait for max 30 seconds
        future.get(30, TimeUnit.SECONDS);
        assertFalse("future is canceled.", future.isCancelled());
        assertTrue("future is not done.", future.isDone());

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
        // Wait for max 30 seconds
        future.get(30, TimeUnit.SECONDS);

        System.out.println("Start continuous recognition, waiting for final result.");
        // wait until we get at least on final result
        long now = System.currentTimeMillis();
        while(((System.currentTimeMillis() - now) < 30000) && (rEvents.isEmpty())) {
            Thread.sleep(200);
        }
        System.out.println("Stop recognition.");
        future = r.stopContinuousRecognitionAsync();
        assertNotNull("future is null.", future);
        // Wait for max 30 seconds
        future.get(30, TimeUnit.SECONDS);
        assertFalse("future is canceled.", future.isCancelled());
        assertTrue("future is not done.", future.isDone());

        // test that we got one result
        // TODO multi-phrase test with several phrases in one session
        assertEquals("wrong number of received results: "+ rEvents.size(), 1, rEvents.size());

        connection.closeConnection();
        TestHelper.AssertConnectionCountMatching(connectedEventCount.get(), disconnectedEventCount.get());

        r.close();
        s.close();
    }

    @Test
    public void testCreateTranslationRecognizerWithVoice() throws InterruptedException, ExecutionException, TimeoutException {
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        s.setSpeechRecognitionLanguage("en-US");
        s.addTargetLanguage("de");
        s.setVoiceName("Microsoft Server Speech Text to Speech Voice (de-DE, Hedda)");

        AtomicInteger synthesizingEventCount = new AtomicInteger(0);
        AtomicInteger sessionStoppedCount = new AtomicInteger(0);
        AtomicInteger audioLength = new AtomicInteger(0);
        TranslationRecognizer r = new TranslationRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));
        r.synthesizing.addEventListener((o, e) -> {
            synthesizingEventCount.getAndIncrement();
            audioLength.getAndAdd(e.getResult().getAudio().length);
        });
        r.sessionStopped.addEventListener((o, e) -> {
            sessionStoppedCount.getAndIncrement();
        });
        TranslationRecognitionResult res = r.recognizeOnceAsync().get();
        // wait until we get the SessionStopped event.
        long now = System.currentTimeMillis();
        while(((System.currentTimeMillis() - now) < 30000) && (sessionStoppedCount.get() == 0)) {
            Thread.sleep(200);
        }
        TestHelper.OutputResult(res);
        assertEquals(2, synthesizingEventCount.get());
        assertTrue(audioLength.get() > 0);
        assertEquals(1, sessionStoppedCount.get());
        assertEquals(ResultReason.TranslatedSpeech, res.getReason());
        assertEquals("What's the weather like?", res.getText());
        assertEquals(1, res.getTranslations().size());
        assertEquals("Wie ist das Wetter?", res.getTranslations().get("de")); // translated text
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

    @Test
    public void testEndpointWithAuthTokenInTranslation() throws InterruptedException, ExecutionException, TimeoutException {
        String endpoint = "wss://" + Settings.SpeechRegion + ".s2s.speech.microsoft.com/speech/translation/cognitiveservices/v1?format=simple&from=en-us&to=de-DE";
        SpeechTranslationConfig s = SpeechTranslationConfig.fromEndpoint(URI.create(endpoint), "");

        assertNotNull(s);

        String language = "en-US";
        s.setSpeechRecognitionLanguage(language);
        s.addTargetLanguage("de");

        TranslationRecognizer r = new TranslationRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);

        final ArrayList<String> rEvents = new ArrayList<>();

        r.setAuthorizationToken(authorizationToken);
        System.out.println("AuthToken:" + r.getAuthorizationToken());

        r.recognized.addEventListener((o, e) -> {
            rEvents.add("Result@" + System.currentTimeMillis());
            System.out.println("Translated: SessionId:" + e.getSessionId()+ ", recognized:" + e.getResult().getText());
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
    public void testSetServiceProperty() throws InterruptedException, ExecutionException, TimeoutException {
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        s.setServiceProperty("from", "en-US", ServicePropertyChannel.UriQueryParameter);
        s.setServiceProperty("to", "de", ServicePropertyChannel.UriQueryParameter);

        TranslationRecognizer r = new TranslationRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));

        TranslationRecognitionResult res = r.recognizeOnceAsync().get();

        assertEquals(ResultReason.TranslatedSpeech, res.getReason());
        assertEquals("What's the weather like?", res.getText());
        assertEquals(1, res.getTranslations().size());
        assertEquals("Wie ist das Wetter?", res.getTranslations().get("de")); // translated text

        r.close();
        s.close();
    }

    @Test
    public void testChangeTargetLanguages() throws InterruptedException, ExecutionException, TimeoutException {
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        String language = "en-US";
        s.setSpeechRecognitionLanguage(language);
        s.addTargetLanguage("de");
        String targetLangsFromConfigProperty = s.getProperty(PropertyId.SpeechServiceConnection_TranslationToLanguages);
        assertEquals("de", targetLangsFromConfigProperty);

        s.addTargetLanguage("fr");
        s.addTargetLanguage("es");
        targetLangsFromConfigProperty = s.getProperty(PropertyId.SpeechServiceConnection_TranslationToLanguages);
        assertEquals("de,fr,es", targetLangsFromConfigProperty);

        s.removeTargetLanguage("de");
        targetLangsFromConfigProperty = s.getProperty(PropertyId.SpeechServiceConnection_TranslationToLanguages);
        assertEquals("fr,es", targetLangsFromConfigProperty);
        ArrayList<String> targetLangListFromConfig = s.getTargetLanguages();
        assertEquals(2, targetLangListFromConfig.size());
        assertEquals("fr", targetLangListFromConfig.get(0));
        assertEquals("es", targetLangListFromConfig.get(1));

        TranslationRecognizer r = new TranslationRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);

        String targetLangsFromRecoProperty = r.getProperties().getProperty(PropertyId.SpeechServiceConnection_TranslationToLanguages);
        assertEquals("fr,es", targetLangsFromRecoProperty);
        ArrayList<String> targetLangListFromReco = r.getTargetLanguages();
        assertEquals(2, targetLangListFromReco.size());
        assertEquals("fr", targetLangListFromReco.get(0));
        assertEquals("es", targetLangListFromReco.get(1));

        r.addTargetLanguage("nl");
        r.removeTargetLanguage("es");
        targetLangsFromRecoProperty = r.getProperties().getProperty(PropertyId.SpeechServiceConnection_TranslationToLanguages);
        assertEquals("fr,nl", targetLangsFromRecoProperty);
        targetLangListFromReco = r.getTargetLanguages();
        assertEquals(2, targetLangListFromReco.size());
        assertEquals("fr", targetLangListFromReco.get(0));
        assertEquals("nl", targetLangListFromReco.get(1));

        Future<TranslationRecognitionResult> future = r.recognizeOnceAsync();

        // Wait for max 30 seconds
        future.get(30, TimeUnit.SECONDS);
        TranslationRecognitionResult res = future.get();
        assertTrue(ResultReason.TranslatedSpeech == res.getReason());
        assertEquals(2, res.getTranslations().size());
        assertTrue(res.getTranslations().get("fr").length() > 0);
        assertTrue(res.getTranslations().get("nl").length() > 0);
        assertFalse(res.getTranslations().containsKey("de"));
        assertFalse(res.getTranslations().containsKey("es"));

        r.close();
        s.close();
    }
}
