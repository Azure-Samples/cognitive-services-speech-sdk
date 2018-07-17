package tests.unit;
//
//Copyright (c) Microsoft. All rights reserved.
//Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import static org.junit.Assert.*;

import java.util.ArrayList;

import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.Ignore;

import com.microsoft.cognitiveservices.speech.Recognizer;
import com.microsoft.cognitiveservices.speech.SpeechFactory;

import tests.Settings;

public class RecognizerParameterNamesTests {

    @BeforeClass
    public static void setUpBeforeClass() throws Exception {
        // Override inputs, if necessary
        Settings.LoadSettings();
    }

    // -----------------------------------------------------------------------
    // --- 
    // -----------------------------------------------------------------------

    @Test
    public void testRecognizerParameterNames1() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        Recognizer r = s.createIntentRecognizerWithFileInput(Settings.WaveFile);
        assertNotNull(r);
        assertTrue(r instanceof Recognizer);

        r.close();
        s.close();
    }

    @Test
    public void testRecognizerParameterNames2() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        Recognizer r = s.createSpeechRecognizerWithFileInput(Settings.WaveFile);
        assertNotNull(r);
        assertTrue(r instanceof Recognizer);

        r.close();
        s.close();
    }
    
    @Ignore("TODO why is number translations not 1 (FIX JAVA LIB FORWARD PROPERTY)")
    @Test
    public void testRecognizerParameterNames3() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        ArrayList<String> targets = new ArrayList<>();
        targets.add("en-US");
        
        Recognizer r = s.createTranslationRecognizer("en-US", targets);
        assertNotNull(r);
        assertTrue(r instanceof Recognizer);

        r.close();
        s.close();
    }

}
