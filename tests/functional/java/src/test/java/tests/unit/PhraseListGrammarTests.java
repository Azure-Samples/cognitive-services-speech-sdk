//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

package tests.unit;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertTrue;
import static org.junit.Assert.fail;

import java.util.concurrent.ExecutionException;

import com.microsoft.cognitiveservices.speech.PhraseListGrammar;
import com.microsoft.cognitiveservices.speech.Recognizer;
import com.microsoft.cognitiveservices.speech.ResultReason;
import com.microsoft.cognitiveservices.speech.SpeechConfig;
import com.microsoft.cognitiveservices.speech.SpeechRecognitionResult;
import com.microsoft.cognitiveservices.speech.SpeechRecognizer;
import com.microsoft.cognitiveservices.speech.audio.AudioConfig;
import com.microsoft.cognitiveservices.speech.intent.IntentRecognitionResult;
import com.microsoft.cognitiveservices.speech.intent.IntentRecognizer;

import org.junit.BeforeClass;
import org.junit.Ignore;
import org.junit.Test;

import tests.AudioUtterancesKeys;
import tests.DefaultSettingsKeys;
import tests.Settings;
import tests.SubscriptionsRegionsKeys;

public class PhraseListGrammarTests {

    @BeforeClass
    static public void setUpBeforeClass() throws Exception {
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
    public void testConjurePhraseListGrammar() throws InterruptedException, ExecutionException {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Key,
            Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Region);

        assertNotNull(s);

        String inputFile = Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.AMBIGUOUS_SPEECH).FilePath);
        SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(inputFile));
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);

        PhraseListGrammar pg = PhraseListGrammar.fromRecognizer(r);
        assertNotNull(pg);

        pg.close();
        r.close();
        s.close();
    }

    @Test
    public void phraselistAssistsSpeechReco() throws InterruptedException, ExecutionException {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Key,
            Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Region);

        assertNotNull(s);
        boolean passed = false;

        for(int RetryLimit=10; RetryLimit>0; RetryLimit--) {

            String inputFile = Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.AMBIGUOUS_SPEECH).FilePath);
            SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(inputFile));
            assertNotNull(r);
            assertNotNull(r.getRecoImpl());
            assertTrue(r instanceof Recognizer);

            PhraseListGrammar pg = PhraseListGrammar.fromRecognizer(r);
            assertNotNull(pg);
            pg.addPhrase("Wreck a nice beach");

            SpeechRecognitionResult res = r.recognizeOnceAsync().get();

            if(res.getText().equalsIgnoreCase("Wreck a nice beach.")){
                passed = true;
                break;
            }else
            {
                Thread.sleep(200);
            }

            pg.close();
            r.close();
        }
        s.close();
        assertEquals(passed, true);

    }

   @Test
   public void extraPhrasesDontHurtReco() throws InterruptedException, ExecutionException {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Key,
            Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Region);

        assertNotNull(s);
        boolean passed = false;

        for(int RetryLimit=10; RetryLimit>0; RetryLimit--) {
            String inputFile = Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.AMBIGUOUS_SPEECH).FilePath);
            SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(inputFile));
            assertNotNull(r);
            assertNotNull(r.getRecoImpl());
            assertTrue(r instanceof Recognizer);

            PhraseListGrammar pg = PhraseListGrammar.fromRecognizer(r);
            assertNotNull(pg);
            pg.addPhrase("Wreck a nice beach");
            pg.addPhrase("No effect");

            SpeechRecognitionResult res = r.recognizeOnceAsync().get();

            if(res.getText().equalsIgnoreCase("Wreck a nice beach.")){
                passed = true;
                break;
            }else
            {
                Thread.sleep(200);
            }

            pg.close();
            r.close();
        }
        s.close();
        assertEquals(passed, true);
   }


   @Test
   public void phraselistAssistsSpeechRecoIntent() throws InterruptedException, ExecutionException {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.LANGUAGE_UNDERSTANDING_SUBSCRIPTION).Key,
            Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.LANGUAGE_UNDERSTANDING_SUBSCRIPTION).Region);

        assertNotNull(s);
        boolean passed = false;

        for(int RetryLimit=10; RetryLimit>0; RetryLimit--) {

            String inputFile = Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.AMBIGUOUS_SPEECH).FilePath);
            IntentRecognizer r = new IntentRecognizer(s, AudioConfig.fromWavFileInput(inputFile));
            assertNotNull(r);
            assertNotNull(r.getRecoImpl());
            assertTrue(r instanceof Recognizer);

            PhraseListGrammar pg = PhraseListGrammar.fromRecognizer(r);
            assertNotNull(pg);
            pg.addPhrase("Wreck a nice beach");

            IntentRecognitionResult res = r.recognizeOnceAsync().get();
            assertEquals(ResultReason.RecognizedSpeech, res.getReason());
            if(res.getText().equalsIgnoreCase("Wreck a nice beach.")){
                passed = true;
                break;
            }else
            {
                Thread.sleep(200);
            }

            pg.close();
            r.close();
        }
        s.close();
        assertEquals(passed, true);
   }

   @Test
   public void extraPhrasesDontHurtRecoIntent() throws InterruptedException, ExecutionException {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.LANGUAGE_UNDERSTANDING_SUBSCRIPTION).Key,
            Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.LANGUAGE_UNDERSTANDING_SUBSCRIPTION).Region);

        assertNotNull(s);
        boolean passed = false;

        for(int RetryLimit=10; RetryLimit>0; RetryLimit--) {
            String inputFile = Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.AMBIGUOUS_SPEECH).FilePath);
            IntentRecognizer r = new IntentRecognizer(s, AudioConfig.fromWavFileInput(inputFile));
            assertNotNull(r);
            assertNotNull(r.getRecoImpl());
            assertTrue(r instanceof Recognizer);

            PhraseListGrammar pg = PhraseListGrammar.fromRecognizer(r);
            assertNotNull(pg);
            pg.addPhrase("Wreck a nice beach");
            pg.addPhrase("No effect");

            IntentRecognitionResult res = r.recognizeOnceAsync().get();
            assertEquals(ResultReason.RecognizedSpeech, res.getReason());
            if(res.getText().equalsIgnoreCase("Wreck a nice beach.")){
                passed = true;
                break;
            }else
            {
                Thread.sleep(200);
            }

            pg.close();
            r.close();
        }
        s.close();
        assertEquals(passed, true);
   }

   @Test
   public void clearedPhraseList() throws InterruptedException, ExecutionException {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Key,
            Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Region);

        assertNotNull(s);

        String inputFile = Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.AMBIGUOUS_SPEECH).FilePath);
        SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(inputFile));
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);

        PhraseListGrammar pg = PhraseListGrammar.fromRecognizer(r);
        assertNotNull(pg);
        pg.addPhrase("Wreck a nice beach");
        pg.clear();

        SpeechRecognitionResult res = r.recognizeOnceAsync().get();
        assertEquals("Recognize speech.", res.getText());

        pg.close();
        r.close();
        s.close();
   }
}
