//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

package tests.unit;

import static org.junit.Assert.*;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.Ignore;

import java.util.concurrent.ExecutionException;
import java.util.concurrent.TimeoutException;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicInteger;
import java.io.File;
import java.net.URI;
import java.util.ArrayList;

import com.microsoft.cognitiveservices.speech.audio.AudioConfig;
import com.microsoft.cognitiveservices.speech.audio.AudioInputStream;
import com.microsoft.cognitiveservices.speech.audio.PullAudioInputStreamCallback;
import com.microsoft.cognitiveservices.speech.PropertyId;
import com.microsoft.cognitiveservices.speech.Recognizer;
import com.microsoft.cognitiveservices.speech.SpeechConfig;
import com.microsoft.cognitiveservices.speech.SpeechRecognizer;
import com.microsoft.cognitiveservices.speech.CancellationReason;
import com.microsoft.cognitiveservices.speech.CancellationDetails;
import com.microsoft.cognitiveservices.speech.CancellationErrorCode;
import com.microsoft.cognitiveservices.speech.ResultReason;
import com.microsoft.cognitiveservices.speech.RecognitionResult;
import com.microsoft.cognitiveservices.speech.SpeechRecognitionResult;
import com.microsoft.cognitiveservices.speech.intent.IntentRecognizer;
import com.microsoft.cognitiveservices.speech.intent.IntentRecognitionResult;
import com.microsoft.cognitiveservices.speech.translation.TranslationRecognizer;
import com.microsoft.cognitiveservices.speech.translation.SpeechTranslationConfig;
import com.microsoft.cognitiveservices.speech.translation.TranslationRecognitionResult;

import tests.Settings;
import tests.TestHelper;

@SuppressWarnings({"unused", "resource"})
public class SpeechConfigTests {

    private static String authorizationToken;

    @BeforeClass
    static public void setUpBeforeClass() throws Exception {
        Settings.LoadSettings();
        authorizationToken = TestHelper.getAuthorizationToken(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    }

    public void checkBadRequestResult(RecognitionResult res)
    {
        assertNotNull(res);
        System.out.println("Reason:" + res.getReason());
        CancellationDetails errorDetails = CancellationDetails.fromResult(res);
        System.out.println("ErrorCode:" + errorDetails.getErrorCode());
        assertEquals(ResultReason.Canceled, res.getReason());
        assertEquals(CancellationErrorCode.BadRequest, errorDetails.getErrorCode());
    }

    public void checkInvalidTargetLanguageResult(TranslationRecognitionResult res)
    {
        assertNotNull(res);
        System.out.println("Reason:" + res.getReason());
        String errorDetails = res.getProperties().getProperty(PropertyId.SpeechServiceResponse_JsonErrorDetails);
        System.out.println("ErroDetails:" + errorDetails);
        assertEquals(ResultReason.RecognizedSpeech, res.getReason());
        assertTrue("Actual error:'" + errorDetails + "' does not contain expected string.", errorDetails.contains("400 (Bad Request)"));
    }

    // -----------------------------------------------------------------------
    // --- FromSubscription tests
    // -----------------------------------------------------------------------
    @Test
    public void testFromSubscriptionSuccess() {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);
        s.close();
    }

    @Test(expected = NullPointerException.class)
    public void testFromSubscriptionNullKeyAndNullRegion() {
        SpeechConfig s = SpeechConfig.fromSubscription(null, null);
    }

    @Test(expected = NullPointerException.class)
    public void testFromSubscriptionNullRegion() {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, null);
    }

    @Test(expected = NullPointerException.class)
    public void testFromSubscriptionNullKey() {
        SpeechConfig s = SpeechConfig.fromSubscription(null, Settings.SpeechRegion);
    }

    @Test
    public void testFromSubscriptionInvalidKeyAndRegion()  throws InterruptedException, ExecutionException, TimeoutException  {
        SpeechConfig s = SpeechConfig.fromSubscription("illegal", "illegal");
        SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));
        SpeechRecognitionResult res = r.recognizeOnceAsync().get();
        assertNotNull(res);
        System.out.println("Reason:" + res.getReason());
        CancellationDetails errorDetails = CancellationDetails.fromResult(res);
        System.out.println("ErrorCode:" + errorDetails.getErrorCode());
        assertEquals(ResultReason.Canceled, res.getReason());
        assertEquals(CancellationErrorCode.ConnectionFailure, errorDetails.getErrorCode());
        r.close();
        s.close();
    }

    @Test
    public void testFromSubscriptionInvalidKey()  throws InterruptedException, ExecutionException, TimeoutException  {
        SpeechConfig s = SpeechConfig.fromSubscription("illegal", Settings.SpeechRegion);
        SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));
        SpeechRecognitionResult res = r.recognizeOnceAsync().get();
        assertNotNull(res);
        System.out.println("Reason:" + res.getReason());
        CancellationDetails errorDetails = CancellationDetails.fromResult(res);
        System.out.println("ErrorCode:" + errorDetails.getErrorCode());
        assertEquals(ResultReason.Canceled, res.getReason());
        assertEquals(CancellationErrorCode.AuthenticationFailure, errorDetails.getErrorCode());
        r.close();
        s.close();
    }

    // -----------------------------------------------------------------------
    // --- FromAuthorizationToken tests
    // -----------------------------------------------------------------------
    @Test
    public void testFromAuthorizationToken() {
        SpeechConfig s = SpeechConfig.fromAuthorizationToken(authorizationToken, Settings.SpeechRegion);
        assertNotNull(s);
        s.close();
    }

    @Test
    public void testGetAuthorizationToken() {
        String token1 = "token1";
        SpeechConfig s = SpeechConfig.fromAuthorizationToken(token1, Settings.SpeechRegion);
        assertNotNull(s);
        String actualToken = s.getAuthorizationToken();
        assertEquals(token1, actualToken);
        s.close();
    }

    @Test
    public void testSetAuthorizationToken() {
        String token1 = "token1";
        String token2 = "token2";
        SpeechConfig s = SpeechConfig.fromAuthorizationToken(token1, Settings.SpeechRegion);
        assertNotNull(s);
        String actualToken = s.getAuthorizationToken();
        assertEquals(token1, actualToken);
        s.setAuthorizationToken(token2);
        actualToken = s.getAuthorizationToken();
        assertEquals(token2, actualToken);
        s.close();
    }

    // -----------------------------------------------------------------------
    // --- FromEndpoint tests
    // -----------------------------------------------------------------------
    @Test
    public void testFromEndpointSuccess() {
        SpeechConfig s = SpeechConfig.fromEndpoint(URI.create("http://www.example.com"), "subscriptionkey");
        assertNotNull(s);
        s.close();
    }

    @Test
    public void testFromEndpointWithoutKeyAndToken() {
        String endpoint = "wss://" + Settings.SpeechRegion + ".stt.speech.microsoft.com/speech/recognition/conversation/cognitiveservices/v1";
        SpeechConfig sc = SpeechConfig.fromEndpoint(URI.create(endpoint));
        SpeechRecognizer sr = new SpeechRecognizer(sc, AudioConfig.fromWavFileInput(Settings.WavFile));
        // We cannot really test whether recognizer works, since there is no test endpoint available which supports no authentication.
        assertTrue(sr.getProperties().getProperty(PropertyId.SpeechServiceAuthorization_Token).isEmpty());
        assertTrue(sr.getProperties().getProperty(PropertyId.SpeechServiceConnection_Key).isEmpty());

        endpoint = "wss://" + Settings.SpeechRegion + ".s2s.speech.microsoft.com/speech/translation/cognitiveservices/v1";
        SpeechTranslationConfig tc = SpeechTranslationConfig.fromEndpoint(URI.create(endpoint));
        tc.setSpeechRecognitionLanguage("en-us");
        tc.addTargetLanguage("de");
        TranslationRecognizer tr = new TranslationRecognizer(tc, AudioConfig.fromWavFileInput(Settings.WavFile));
        // We cannot really test whether recognizer works, since there is no test endpoint available which supports no authentication.
        assertTrue(tr.getProperties().getProperty(PropertyId.SpeechServiceAuthorization_Token).isEmpty());
        assertTrue(tr.getProperties().getProperty(PropertyId.SpeechServiceConnection_Key).isEmpty());
        sr.close();
        tc.close();
    }

    @Test(expected = NullPointerException.class)
    public void testFromEndpointNullEndpointNullKey() {
        SpeechConfig s = SpeechConfig.fromEndpoint(null, null);
        s.close();
    }

    @Test(expected = NullPointerException.class)
    public void testFromEndpointNullEndpoint() {
        SpeechConfig s = SpeechConfig.fromEndpoint(null, Settings.SpeechRegion);
        s.close();
    }

    @Test(expected = NullPointerException.class)
    public void testFromEndpointNullKey() {
        SpeechConfig s = SpeechConfig.fromEndpoint(URI.create("http://www.example.com"), null);
        s.close();
    }

    // -----------------------------------------------------------------------
    // --- FromHost tests
    // -----------------------------------------------------------------------
    @Test
    public void testFromHostSuccess() {
        SpeechConfig s = SpeechConfig.fromHost(URI.create("http://www.example.com"), "subscriptionkey");
        assertNotNull(s);
        s.close();
    }

    @Test
    public void testFromHostWithoutKeyAndToken() {
        String host = "wss://" + Settings.SpeechRegion + ".stt.speech.microsoft.com";
        SpeechConfig sc = SpeechConfig.fromHost(URI.create(host));
        SpeechRecognizer sr = new SpeechRecognizer(sc, AudioConfig.fromWavFileInput(Settings.WavFile));
        // We cannot really test whether recognizer works, since there is no non-container test host available which supports no authentication.
        assertTrue(sr.getProperties().getProperty(PropertyId.SpeechServiceAuthorization_Token).isEmpty());
        assertTrue(sr.getProperties().getProperty(PropertyId.SpeechServiceConnection_Key).isEmpty());

        host = "wss://" + Settings.SpeechRegion + ".s2s.speech.microsoft.com";
        SpeechTranslationConfig tc = SpeechTranslationConfig.fromHost(URI.create(host));
        tc.setSpeechRecognitionLanguage("en-us");
        tc.addTargetLanguage("de");
        TranslationRecognizer tr = new TranslationRecognizer(tc, AudioConfig.fromWavFileInput(Settings.WavFile));
        // We cannot really test whether recognizer works, since there is no non-container test host available which supports no authentication.
        assertTrue(tr.getProperties().getProperty(PropertyId.SpeechServiceAuthorization_Token).isEmpty());
        assertTrue(tr.getProperties().getProperty(PropertyId.SpeechServiceConnection_Key).isEmpty());
        sr.close();
        tc.close();
    }

    @Test(expected = NullPointerException.class)
    public void testFromHostNullHostNullKey() {
        SpeechConfig s = SpeechConfig.fromHost(null, null);
        s.close();
    }

    @Test(expected = NullPointerException.class)
    public void testFromHostNullHost() {
        SpeechConfig s = SpeechConfig.fromHost(null, "subscriptionkey");
        s.close();
    }

    @Test(expected = NullPointerException.class)
    public void testFromHostNullKey() {
        SpeechConfig s = SpeechConfig.fromHost(URI.create("http://www.example.com"), null);
        s.close();
    }

    // -----------------------------------------------------------------------
    // --- Null AudioConfig
    // -----------------------------------------------------------------------
    @Ignore("Tests fails if there is no mic on the test machine.")
    @Test
    public void testCreateSpeechRecognizerNullAudioConfig() {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        // null audio config will use default mic.
        SpeechRecognizer r = new SpeechRecognizer(s);
        assertNotNull(r);
        assertTrue(r instanceof Recognizer);
        r.close();
        s.close();
    }

    // -----------------------------------------------------------------------
    // --- SpeechRecognizer using file input
    // -----------------------------------------------------------------------
    @Test
    public void testCreateSpeechRecognizerFromFileInputSuccess() {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);
        r.close();
        s.close();
    }

    @Test
    public void testCreateSpeechRecognizerNullFileName() {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        try {
            SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromWavFileInput((String)null));
            fail("not expected");
        }
        catch(NullPointerException ex) {
            // expected
        }
        s.close();
    }

    @Test
    public void testCreateSpeechRecognizerInvalidFileName() {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        try {
            SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromWavFileInput("illegal-" + Settings.WavFile));
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        s.close();
    }

    // -----------------------------------------------------------------------
    // --- SpeechRecognizer using language
    // -----------------------------------------------------------------------
    @Test
    public void testCreateSpeechRecognizerWithLanguageSuccess() {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        SpeechRecognizer r = new SpeechRecognizer(s, "en-EN", AudioConfig.fromWavFileInput(Settings.WavFile));
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        r.close();
        s.close();
    }

    @Test
    public void testCreateSpeechRecognizerInvalidLanguage()  throws InterruptedException, ExecutionException, TimeoutException {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        s.setSpeechRecognitionLanguage("illegal-language");
        SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));
        SpeechRecognitionResult res = r.recognizeOnceAsync().get();
        checkBadRequestResult(res);
        r.close();
        s.close();
    }

    @Test
    public void testCreateSpeechRecognizerNullLanguage() {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        try {
            s.setSpeechRecognitionLanguage(null);
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        s.close();
    }

    // -----------------------------------------------------------------------
    // --- SpeechRecognizer using AudioInputStream
    // -----------------------------------------------------------------------
    @Test
    public void testCreateSpeechRecognizerAudioInputStreamSuccess() {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.WavFile);
        SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromStreamInput(ais));
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);
        r.close();
        s.close();
    }

    @Test
    public void testAudioConfigNullAudioInputStream() {
        try {
            AudioConfig ac = AudioConfig.fromStreamInput((AudioInputStream)null);
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
    }

    // -----------------------------------------------------------------------
    // --- IntentRecognizer: null audio config; using file input
    // -----------------------------------------------------------------------
    @Test
    public void testCreateIntentRecognizerStringSuccess() {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        IntentRecognizer r = new IntentRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);
        r.close();
        s.close();
    }

    @Ignore("Tests fails if there is no mic on the test machine.")
    @Test
    public void testCreateIntentRecognizerNullAudioConfig() {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        IntentRecognizer r = new IntentRecognizer(s, null);
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);
        r.close();
        s.close();
    }

    @Test
    public void testCreateIntentRecognizerNullFileName() {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        try {
            IntentRecognizer r = new IntentRecognizer(s, AudioConfig.fromWavFileInput((String)null));
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        s.close();
    }

    @Test
    public void testCreateIntentRecognizerInvalidFileName() {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        try {
            IntentRecognizer r = new IntentRecognizer(s, AudioConfig.fromWavFileInput("illegal-" + Settings.WavFile));
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        s.close();
    }

    // -----------------------------------------------------------------------
    // --- IntentRecognizer: using AudioInputStream
    // -----------------------------------------------------------------------
    @Test
    public void testCreateIntentRecognizerAudioInputStreamSuccess() {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.WavFile);
        IntentRecognizer r = new IntentRecognizer(s, AudioConfig.fromStreamInput(ais));
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);
        r.close();
        s.close();
    }

    @Test
    public void testCreateIntentRecognizerAudioInputStreamWithLanguageSuccess() {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        s.setSpeechRecognitionLanguage("en-US");
        WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.WavFile);
        IntentRecognizer r = new IntentRecognizer(s, AudioConfig.fromStreamInput(ais));
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);
        r.close();
        s.close();
    }

    @Test
    public void testCreateIntentRecognizerAudioInputStreamInvalidLanguage() throws InterruptedException, ExecutionException, TimeoutException {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        s.setSpeechRecognitionLanguage("illegal-language");
        WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.WavFile);
        IntentRecognizer r = new IntentRecognizer(s, AudioConfig.fromStreamInput(ais));
        IntentRecognitionResult res = r.recognizeOnceAsync().get();
        assertNotNull(res);
        System.out.println("Reason:" + res.getReason());
        CancellationDetails errorDetails = CancellationDetails.fromResult(res);
        System.out.println("ErrorCode:" + errorDetails.getErrorCode());
        assertEquals(ResultReason.Canceled, res.getReason());
        // Intent service returns Authentication failure for invalid language.
        assertEquals(CancellationErrorCode.AuthenticationFailure, errorDetails.getErrorCode());
        r.close();
        s.close();
    }

    // -----------------------------------------------------------------------
    // --- TranslationRecognizer: NullConfig;
    // -----------------------------------------------------------------------
    @Ignore("Tests fails if there is no mic on the test machine.")
    @Test
    public void testCreateTranslationRecognizerNullAudioConfigNoFromNoTo() {
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        // Null audio config will use default mic.
        TranslationRecognizer r = new TranslationRecognizer(s, null);
        r.close();
        s.close();
    }

    @Test
    public void testCreateTranslationRecognizerNullConfig() {
        try {
            TranslationRecognizer r = new TranslationRecognizer(null,null);
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
    }

    // -----------------------------------------------------------------------
    // --- TranslationRecognizer: using file input.
    // -----------------------------------------------------------------------
    @Test
    public void testCreateTranslationRecognizerWithFileInputSuccess() {
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        s.setSpeechRecognitionLanguage("en-US");
        s.addTargetLanguage("en-US");

        TranslationRecognizer r = new TranslationRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);

        r.close();
        s.close();
    }

    @Test
    public void testCreateTranslationRecognizerInvalidFileInput() {
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        try {
            TranslationRecognizer r = new TranslationRecognizer(s, AudioConfig.fromWavFileInput("illegal-" + Settings.WavFile));
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        s.close();
    }

    // -----------------------------------------------------------------------
    // --- TranslationRecognizer: from and to
    // -----------------------------------------------------------------------
    @Test
    public void testCreateTranslationRecognizerNoFromNoTo() throws InterruptedException, ExecutionException, TimeoutException {
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        TranslationRecognizer r = new TranslationRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));
        TranslationRecognitionResult res = r.recognizeOnceAsync().get();
        checkBadRequestResult(res);
        r.close();
        s.close();
    }

    @Test
    public void testCreateTranslationRecognizerNoFrom() throws InterruptedException, ExecutionException, TimeoutException {
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        s.addTargetLanguage("de");
        TranslationRecognizer r = new TranslationRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));
        TranslationRecognitionResult res = r.recognizeOnceAsync().get();
        checkBadRequestResult(res);
        r.close();
        s.close();
    }

    @Test
    public void testCreateTranslationRecognizerInvalidFrom() throws InterruptedException, ExecutionException, TimeoutException {
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        s.setSpeechRecognitionLanguage("illegal");
        s.addTargetLanguage("de");
        TranslationRecognizer r = new TranslationRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));
        TranslationRecognitionResult res = r.recognizeOnceAsync().get();
        checkBadRequestResult(res);
        r.close();
        s.close();
    }

    @Test
    public void testCreateTranslationRecognizerNoTo() throws InterruptedException, ExecutionException, TimeoutException {
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        s.setSpeechRecognitionLanguage("en-US");
        TranslationRecognizer r = new TranslationRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));
        TranslationRecognitionResult res = r.recognizeOnceAsync().get();
        checkBadRequestResult(res);
        r.close();
        s.close();
    }

    @Test
    public void testCreateTranslationRecognizerInvalidTo() throws InterruptedException, ExecutionException, TimeoutException {
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        s.setSpeechRecognitionLanguage("en-US");
        s.addTargetLanguage("illegal");
        TranslationRecognizer r = new TranslationRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));
        TranslationRecognitionResult res = r.recognizeOnceAsync().get();
        checkInvalidTargetLanguageResult(res);
        r.close();
        s.close();
    }

    // -----------------------------------------------------------------------
    // --- TranslationRecognizer: voice name
    // -----------------------------------------------------------------------
    @Test
    public void testCreateTranslationRecognizerWithVoiceName() {
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        s.setSpeechRecognitionLanguage("en-US");
        s.addTargetLanguage("en-US");
        s.setVoiceName("Microsoft Server Speech Text to Speech Voice (de-DE, Hedda)");
        TranslationRecognizer r = new TranslationRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);
        r.close();
        s.close();
    }

    @Test
    public void testCreateTranslationRecognizerInvalidVoice() throws InterruptedException, ExecutionException, TimeoutException {
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        s.setSpeechRecognitionLanguage("en-US");
        s.addTargetLanguage("en-US");
        s.setVoiceName("illegal");
        AtomicInteger synthesizingEventCount = new AtomicInteger(0);
        AtomicInteger sessionStoppedCount = new AtomicInteger(0);
        AtomicBoolean expectedCancellationError = new AtomicBoolean(false);
        TranslationRecognizer r = new TranslationRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));
        r.synthesizing.addEventListener((o, e) -> {
            synthesizingEventCount.getAndIncrement();
        });
        r.sessionStopped.addEventListener((o, e) -> {
            sessionStoppedCount.getAndIncrement();
        });
        r.canceled.addEventListener((o, e) -> {
            if (e.getReason() == CancellationReason.Error)
            {
                if (CancellationErrorCode.ServiceError == e.getErrorCode() && e.getErrorDetails().contains("Synthesis service failed"))
                {
                    System.out.println("expectedCancellationError: set to true");
                    expectedCancellationError.set(true);
                }
            }
        });
        r.startContinuousRecognitionAsync().get();
        // wait until we get the SessionStopped event.
        long now = System.currentTimeMillis();
        while((System.currentTimeMillis() - now < 30000) && sessionStoppedCount.get() == 0 && expectedCancellationError.get() == false) {
            Thread.sleep(200);
        }
        r.stopContinuousRecognitionAsync().get();
        System.out.println("expectedCancellationError after:" + expectedCancellationError.get() + "sessionStoppedCount: " + sessionStoppedCount.get());
        assertEquals(0, synthesizingEventCount.get());
        assertTrue(expectedCancellationError.get());
        r.close();
        s.close();
    }

    // -----------------------------------------------------------------------
    // --- TranslationRecognizer using AudioInputStream
    // -----------------------------------------------------------------------
    @Test
    public void testCreateTranslationRecognizerWithStreamAudioInputStreamSuccess() {
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        s.addTargetLanguage("en-US");
        s.setSpeechRecognitionLanguage("en-US");
        WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.WavFile);
        TranslationRecognizer r = new TranslationRecognizer(s, AudioConfig.fromStreamInput(ais));
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);
        r.close();
        s.close();
    }

    // -----------------------------------------------------------------------
    // --- misc tests
    // -----------------------------------------------------------------------

    @Test
    public void testClose() {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        s.close();
    }

    @Test
    public void testGetConfigImpl() {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);
        assertNotNull(s.getImpl());
        s.close();
    }

    @Test
    public void testSetProperty() {
        String auth_token = "token";
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        s.setProperty(PropertyId.SpeechServiceAuthorization_Token, auth_token);
        assertEquals(auth_token, s.getProperty(PropertyId.SpeechServiceAuthorization_Token));
        assertEquals(auth_token, s.getAuthorizationToken());
        s.close();
    }

    @Test(expected = NullPointerException.class)
    public void testSetPropertyNullValue() {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        s.setProperty(PropertyId.SpeechServiceConnection_Endpoint, null);
    }

    @Test(expected = IllegalArgumentException.class)
    public void testSetPropertyEmptyValue() {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        s.setProperty(PropertyId.SpeechServiceConnection_Endpoint, "");
    }

    @Test(expected = IllegalArgumentException.class)
    public void testSetPropertyWhiteSpaceValue() {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        s.setProperty(PropertyId.SpeechServiceConnection_Endpoint, " ");
    }

    @Test
    public void testSetLogFilename() {
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        s.setSpeechRecognitionLanguage("en-US");
        s.addTargetLanguage("de");
        String logFilename = new java.io.File(System.getProperty("java.io.tmpdir", ""), "test_filename.txt").getAbsolutePath();

        s.setProperty(PropertyId.Speech_LogFilename, logFilename);
        assertEquals(s.getProperty(PropertyId.Speech_LogFilename), logFilename);
        TranslationRecognizer r = new TranslationRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));
        assertTrue(new File(logFilename).length() > 0);
        r.close();
        s.close();
    }
}
