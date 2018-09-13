package tests.unit;
//
//Copyright (c) Microsoft. All rights reserved.
//Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import static org.junit.Assert.*;

import java.util.ArrayList;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.Ignore;

import com.microsoft.cognitiveservices.speech.audio.AudioConfig;
import com.microsoft.cognitiveservices.speech.PropertyCollection;
import com.microsoft.cognitiveservices.speech.SpeechConfig;
import com.microsoft.cognitiveservices.speech.SpeechRecognizer;
import com.microsoft.cognitiveservices.speech.intent.IntentRecognizer;
import com.microsoft.cognitiveservices.speech.translation.TranslationRecognizer;
import com.microsoft.cognitiveservices.speech.translation.SpeechTranslatorConfig;
import com.microsoft.cognitiveservices.speech.SpeechPropertyId;

import tests.Settings;

@SuppressWarnings("unused")
public class PropertyCollectionTests {

    @BeforeClass
    public static void setUpBeforeClass() throws Exception {
        // Override inputs, if necessary
        Settings.LoadSettings();
    }

    // -----------------------------------------------------------------------
    // --- 
    // -----------------------------------------------------------------------

    @Test
    public void testPropertyCollection() {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        PropertyCollection crp  = new IntentRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile)).getParameters();
        assertNotNull(crp);
        
        PropertyCollection srp  = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile)).getParameters();
        assertNotNull(srp);

        SpeechTranslatorConfig st = SpeechTranslatorConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        st.setSpeechRecognitionLanguage("en-US");
        st.addTargetLanguage("en-US");
        PropertyCollection trp  = new TranslationRecognizer(st, AudioConfig.fromWavFileInput(Settings.WavFile)).getParameters();
        assertNotNull(crp);
    }

    // -----------------------------------------------------------------------
    // --- 
    // -----------------------------------------------------------------------

    @Test
    public void testPropertyCollectionSetpropertyId() {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        PropertyCollection crp  = new IntentRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile)).getParameters();
        assertNotNull(crp);

        String value = "seattle";
        crp.setProperty(SpeechPropertyId.SpeechServiceConnection_IntentRegion, value);
        assertEquals(value, crp.getProperty("INTENT-region", ""));
    }
    // -----------------------------------------------------------------------
    // --- 
    // -----------------------------------------------------------------------

    private void testIsParametersString(PropertyCollection p) {
        String name = "stringName";
        String value = "stringValue";
        
        p.setProperty(name, value);
    }

    @Test
    public void testIsString() {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        PropertyCollection crp  = new IntentRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile)).getParameters();
        assertNotNull(crp);
        testIsParametersString(crp);
        
        PropertyCollection srp  = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile)).getParameters();
        assertNotNull(srp);
        testIsParametersString(srp);
        
        SpeechTranslatorConfig st = SpeechTranslatorConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        st.setSpeechRecognitionLanguage("en-US");
        st.addTargetLanguage("en-US");
        PropertyCollection trp  = new TranslationRecognizer(st, AudioConfig.fromWavFileInput(Settings.WavFile)).getParameters();
        assertNotNull(trp);
        testIsParametersString(trp);
    }

    // -----------------------------------------------------------------------
    // --- 
    // -----------------------------------------------------------------------

    private void testGetParametersString(PropertyCollection p) {
        String name = "stringName";
        String value = "stringValue";
        
        p.setProperty(name, value);
        
        assertEquals(value, p.getProperty(name));
        assertEquals(value, p.getProperty(name, "some-other-default-" + value));
    }

    @Test
    public void testGetSetStringString() {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        PropertyCollection crp  = new IntentRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile)).getParameters();
        assertNotNull(crp);
        testIsParametersString(crp);
        
        PropertyCollection srp  = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile)).getParameters();
        assertNotNull(srp);
        testIsParametersString(srp);
        
        SpeechTranslatorConfig st = SpeechTranslatorConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        st.setSpeechRecognitionLanguage("en-US");
        st.addTargetLanguage("en-US");
        PropertyCollection trp  = new TranslationRecognizer(st, AudioConfig.fromWavFileInput(Settings.WavFile)).getParameters();
        assertNotNull(trp);
        testIsParametersString(trp);
    }


    // -----------------------------------------------------------------------
    // --- 
    // -----------------------------------------------------------------------

    private void testGetParametersStringString(PropertyCollection p) {
        String name = "stringName3";
        String value = "stringValue";

        assertEquals("some-other-default-" + value, p.getProperty(name, "some-other-default-" + value));
        
        p.setProperty(name, value);
        assertEquals(value, p.getProperty(name));
        assertEquals(value, p.getProperty(name, "some-other-default-" + value));
    }

    @Ignore("TODO string string tests fail")
    @Test
    public void testGetStringStringString() {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        PropertyCollection crp  = new IntentRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile)).getParameters();
        assertNotNull(crp);
        testGetParametersStringString(crp);
        
        PropertyCollection srp  = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile)).getParameters();
        assertNotNull(srp);
        testGetParametersStringString(srp);
        
        SpeechTranslatorConfig st = SpeechTranslatorConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        st.setSpeechRecognitionLanguage("en-US");
        st.addTargetLanguage("en-US");
        PropertyCollection trp  = new TranslationRecognizer(st, AudioConfig.fromWavFileInput(Settings.WavFile)).getParameters();
        assertNotNull(trp);
        testGetParametersStringString(trp);
    }
    
    // -----------------------------------------------------------------------
    // --- 
    // -----------------------------------------------------------------------

    @Test
    public void testClose() {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        PropertyCollection crp  = new IntentRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile)).getParameters();
        assertNotNull(crp);
        
        PropertyCollection srp  = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile)).getParameters();
        assertNotNull(srp);
        
        SpeechTranslatorConfig st = SpeechTranslatorConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        st.setSpeechRecognitionLanguage("en-US");
        st.addTargetLanguage("en-US");
        PropertyCollection trp  = new TranslationRecognizer(st, AudioConfig.fromWavFileInput(Settings.WavFile)).getParameters();
        assertNotNull(trp);
        
        s.close();
    }

}
