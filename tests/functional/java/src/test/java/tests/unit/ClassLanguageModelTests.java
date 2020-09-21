//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

package tests.unit;

import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertTrue;
import static org.junit.Assert.fail;

import java.util.concurrent.ExecutionException;

import com.microsoft.cognitiveservices.speech.*;
import com.microsoft.cognitiveservices.speech.audio.AudioConfig;

import org.junit.BeforeClass;
import org.junit.Ignore;
import org.junit.Test;
import org.junit.Rule;

import tests.AudioUtterancesKeys;
import tests.Settings;
import tests.SubscriptionsRegionsKeys;
import tests.Retry;

public class ClassLanguageModelTests {

    @Rule
    public Retry retry = new Retry(Settings.TestRetryCount);

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
    public void testConjureClassLanguageModelFromStorageId() throws InterruptedException, ExecutionException {
        ClassLanguageModel clm = ClassLanguageModel.fromStorageId("71C8B766-2746-4DC4-8825-8968F5F1B25A");
        assertNotNull(clm);
        clm.close();
    }

    @Test
    public void testConjureClassLanguageModelFromStorageIdNonGuid() throws InterruptedException, ExecutionException {
        ClassLanguageModel clm = ClassLanguageModel.fromStorageId("foo");
        assertNotNull(clm);
        clm.close();
    }
    
    @Test
    public void testAddGrammarToClassLanguageModel() throws InterruptedException, ExecutionException {
        Grammar g = Grammar.fromStorageId("71C8B766-2746-4DC4-8825-8968F5F1B25A");
        assertNotNull(g);

        ClassLanguageModel clm = ClassLanguageModel.fromStorageId("E37229FA-370B-4E4D-95DE-1EBDF6181E83");
        assertNotNull(clm);
     
        clm.assignClass("contact", g);

        clm.close();
        g.close();
    }

    @Test
    public void testAddTwoGrammarsToClassLanguageModel() throws InterruptedException, ExecutionException {
        Grammar g1 = Grammar.fromStorageId("71C8B766-2746-4DC4-8825-8968F5F1B25A");
        assertNotNull(g1);

        Grammar g2 = Grammar.fromStorageId("71C8B766-2746-4DC4-8825-8968F5F1B25A");
        assertNotNull(g2);

        ClassLanguageModel clm = ClassLanguageModel.fromStorageId("E37229FA-370B-4E4D-95DE-1EBDF6181E83");
        assertNotNull(clm);
     
        clm.assignClass("contact", g1);
        clm.assignClass("location", g2);

        clm.close();
        g1.close();
        g2.close();
    }

    @Test
    public void testAddClassLanguageModelToRecoEngineAndRun() throws InterruptedException, ExecutionException {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Key, 
            Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Region);

        assertNotNull(s);

        SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath)));
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);

        GrammarList gl = GrammarList.fromRecognizer(r);
        assertNotNull(gl);

        Grammar g1 = Grammar.fromStorageId("71C8B766-2746-4DC4-8825-8968F5F1B25A");
        assertNotNull(g1);

        Grammar g2 = Grammar.fromStorageId("71C8B766-2746-4DC4-8825-8968F5F1B25A");
        assertNotNull(g2);

        ClassLanguageModel clm = ClassLanguageModel.fromStorageId("E37229FA-370B-4E4D-95DE-1EBDF6181E83");
        assertNotNull(clm);
     
        clm.assignClass("contact", g1);
        clm.assignClass("location", g2);

        gl.add(clm);

        r.recognizeOnceAsync().get();

        r.close();
        gl.close();

        clm.close();
        g1.close();
        g2.close();
    }

}
