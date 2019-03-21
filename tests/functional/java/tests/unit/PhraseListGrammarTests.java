package tests.unit;
//
//Copyright (c) Microsoft. All rights reserved.
//Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.Future;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.TimeoutException;
import java.util.concurrent.atomic.AtomicInteger;


import static org.junit.Assert.*;

import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.Ignore;


import com.microsoft.cognitiveservices.speech.audio.AudioConfig;
import com.microsoft.cognitiveservices.speech.CancellationReason;
import com.microsoft.cognitiveservices.speech.ResultReason;
import com.microsoft.cognitiveservices.speech.SessionEventArgs;
import com.microsoft.cognitiveservices.speech.Recognizer;
import com.microsoft.cognitiveservices.speech.SpeechConfig;
import com.microsoft.cognitiveservices.speech.SpeechRecognitionResult;
import com.microsoft.cognitiveservices.speech.SpeechRecognizer;
import com.microsoft.cognitiveservices.speech.intent.IntentRecognitionResult;
import com.microsoft.cognitiveservices.speech.intent.IntentRecognizer;
import com.microsoft.cognitiveservices.speech.PropertyId;
import com.microsoft.cognitiveservices.speech.OutputFormat;
import com.microsoft.cognitiveservices.speech.Connection;
import com.microsoft.cognitiveservices.speech.PhraseListGrammar;

import tests.Settings;
import tests.TestHelper;

public class PhraseListGrammarTests {
    
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
    public void testConjurePhraseListGrammar() throws InterruptedException, ExecutionException {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));
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
    @Ignore // Until skyman supports DGI.
    public void phraselistAssistsSpeechReco() throws InterruptedException, ExecutionException {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(Settings.AudioInputDirectory + "/wreck-a-nice-beach.wav"));
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);

        PhraseListGrammar pg = PhraseListGrammar.fromRecognizer(r);
        assertNotNull(pg);
        pg.addPhrase("Wreck a nice beach.");

        SpeechRecognitionResult res = r.recognizeOnceAsync().get();
        assertEquals("Wreck a nice beach.", res.getText());

        pg.close();
        r.close();
        s.close();
    }

   @Test
   @Ignore // Until skyman supports DGI.
   public void extraPhrasesDontHurtReco() throws InterruptedException, ExecutionException {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(Settings.AudioInputDirectory + "/wreck-a-nice-beach.wav"));
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);

        PhraseListGrammar pg = PhraseListGrammar.fromRecognizer(r);
        assertNotNull(pg);
        pg.addPhrase("Wreck a nice beach.");
        pg.addPhrase("No effect");

        SpeechRecognitionResult res = r.recognizeOnceAsync().get();
        assertEquals("Wreck a nice beach.", res.getText());

        pg.close();
        r.close();
        s.close();
   }

    
   @Test
   public void phraselistAssistsSpeechRecoIntent() throws InterruptedException, ExecutionException {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.LuisSubscriptionKey, Settings.LuisRegion);
        assertNotNull(s);

        IntentRecognizer r = new IntentRecognizer(s, AudioConfig.fromWavFileInput(Settings.AudioInputDirectory + "/wreck-a-nice-beach.wav"));
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);

        PhraseListGrammar pg = PhraseListGrammar.fromRecognizer(r);
        assertNotNull(pg);
        pg.addPhrase("Wreck a nice beach.");

        IntentRecognitionResult res = r.recognizeOnceAsync().get();
        assertEquals(ResultReason.RecognizedSpeech, res.getReason());
        assertEquals("Wreck a nice beach.", res.getText());

        pg.close();
        r.close();
        s.close();
   }

   @Test
   public void extraPhrasesDontHurtRecoIntent() throws InterruptedException, ExecutionException {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.LuisSubscriptionKey, Settings.LuisRegion);
        assertNotNull(s);

        IntentRecognizer r = new IntentRecognizer(s, AudioConfig.fromWavFileInput(Settings.AudioInputDirectory + "/wreck-a-nice-beach.wav"));
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);

        PhraseListGrammar pg = PhraseListGrammar.fromRecognizer(r);
        assertNotNull(pg);
        pg.addPhrase("Wreck a nice beach.");
        pg.addPhrase("No effect");

        IntentRecognitionResult res = r.recognizeOnceAsync().get();
        assertEquals(ResultReason.RecognizedSpeech, res.getReason());
        assertEquals("Wreck a nice beach.", res.getText());

        pg.close();
        r.close();
        s.close();
   }

   @Test
   public void clearedPhraseList() throws InterruptedException, ExecutionException {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(Settings.AudioInputDirectory + "/wreck-a-nice-beach.wav"));
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);

        PhraseListGrammar pg = PhraseListGrammar.fromRecognizer(r);
        assertNotNull(pg);
        pg.addPhrase("Wreck a nice beach.");
        pg.clear();

        SpeechRecognitionResult res = r.recognizeOnceAsync().get();
        assertEquals("Recognize speech.", res.getText());

        pg.close();
        r.close();
        s.close();
   }
}
