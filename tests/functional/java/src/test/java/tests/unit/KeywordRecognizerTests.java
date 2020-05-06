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
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.CountDownLatch;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.Ignore;

import com.microsoft.cognitiveservices.speech.audio.AudioConfig;
import com.microsoft.cognitiveservices.speech.AudioDataStream;
import com.microsoft.cognitiveservices.speech.CancellationReason;
import com.microsoft.cognitiveservices.speech.ResultReason;
import com.microsoft.cognitiveservices.speech.Recognizer;
import com.microsoft.cognitiveservices.speech.SpeechConfig;
import com.microsoft.cognitiveservices.speech.PropertyId;
import com.microsoft.cognitiveservices.speech.KeywordRecognitionResult;
import com.microsoft.cognitiveservices.speech.KeywordRecognizer;
import com.microsoft.cognitiveservices.speech.KeywordRecognitionModel;
import com.microsoft.cognitiveservices.speech.util.EventHandler;

import tests.AudioUtterancesKeys;
import tests.DefaultSettingsKeys;
import tests.Settings;
import tests.SubscriptionsRegionsKeys;
import tests.TestHelper;

public class KeywordRecognizerTests {
    private final Integer FIRST_EVENT_ID = 1;
    private AtomicInteger eventIdentifier = new AtomicInteger(FIRST_EVENT_ID);

    @BeforeClass
    static public void setUpBeforeClass() throws Exception {
        String operatingSystem = ("" + System.getProperty("os.name")).toLowerCase();
        System.out.println("Current operation system: " + operatingSystem);
        boolean isMac = operatingSystem.contains("mac") || operatingSystem.contains("darwin");
        org.junit.Assume.assumeFalse(isMac);
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
    public void testKeywordRecognizer1() {
        KeywordRecognizer r = new KeywordRecognizer(AudioConfig.fromWavFileInput(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.COMPUTER_KEYWORD_WITH_SINGLE_UTTERANCE_1).FilePath)));
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        r.close();
    }

    @Test
    public void testGetParameters() {
        KeywordRecognizer r = new KeywordRecognizer(AudioConfig.fromWavFileInput(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.COMPUTER_KEYWORD_WITH_SINGLE_UTTERANCE_1).FilePath)));
        assertNotNull(r);
        assertNotNull(r.getProperties());
        r.close();
    }

    @Test
    public void testKeywordRecognizer2() throws InterruptedException, ExecutionException {

        WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.COMPUTER_KEYWORD_WITH_SINGLE_UTTERANCE_1).FilePath));
        KeywordRecognizer r = new KeywordRecognizer(AudioConfig.fromStreamInput(ais));
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());

        String keywordModelPath = Settings.GetRootRelativePath(Settings.DefaultSettingsMap.get(DefaultSettingsKeys.KEYWORD_MODEL));
        System.out.println("keywordModelPath: " + keywordModelPath);
        KeywordRecognitionModel model = KeywordRecognitionModel.fromFile(keywordModelPath);
        assertNotNull(model);

        final Map<String, Integer> eventsMap = new HashMap<String, Integer>();
        r.recognized.addEventListener((o, e) -> {
            eventsMap.put("recognized", eventIdentifier.getAndIncrement());
        });

        KeywordRecognitionResult res = r.recognizeOnceAsync(model).get();
        assertNotNull(res);
        System.out.println("res.getReason(): " + String.valueOf(res.getReason()));
        assertEquals(ResultReason.RecognizedKeyword, res.getReason());

        r.close();
    }

    @Test
    public void testExceptionsDuringEventsRecognizeOnce() throws InterruptedException, ExecutionException {

        KeywordRecognizer r = new KeywordRecognizer(AudioConfig.fromWavFileInput(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.COMPUTER_KEYWORD_WITH_SINGLE_UTTERANCE_1).FilePath)));
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());

        r.recognized.addEventListener((o, e) -> {
            System.out.println("Now Recognized. preparing throw exception");
            throw new IllegalArgumentException("Recognized");
        });

        r.canceled.addEventListener((o, e) -> {
            System.out.println("Now Canceled. preparing throw exception");
            throw new IllegalArgumentException("canceled");
        });

        String keywordModelPath = Settings.GetRootRelativePath(Settings.DefaultSettingsMap.get(DefaultSettingsKeys.KEYWORD_MODEL));
        System.out.println("keywordModelPath: " + keywordModelPath);
        KeywordRecognitionModel model = KeywordRecognitionModel.fromFile(keywordModelPath);
        assertNotNull(model);

        KeywordRecognitionResult res = r.recognizeOnceAsync(model).get();
        assertNotNull(res);
        assertEquals(ResultReason.RecognizedKeyword, res.getReason());
        r.close();
    }

}
