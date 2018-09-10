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
import com.microsoft.cognitiveservices.speech.ParameterCollection;
import com.microsoft.cognitiveservices.speech.SpeechFactory;

import tests.Settings;

@SuppressWarnings("unused")
public class ParameterCollectionTests {

    @BeforeClass
    public static void setUpBeforeClass() throws Exception {
        // Override inputs, if necessary
        Settings.LoadSettings();
    }

    // -----------------------------------------------------------------------
    // --- 
    // -----------------------------------------------------------------------

    @Test
    public void testParameterCollection() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        ParameterCollection<?> sfp = s.getParameters();
        assertNotNull(sfp);
        
        ParameterCollection<?> crp  = s.createIntentRecognizerFromConfig(AudioConfig.fromWavFileInput(Settings.WavFile)).getParameters();
        assertNotNull(crp);
        
        ParameterCollection<?> srp  = s.createSpeechRecognizerFromConfig(AudioConfig.fromWavFileInput(Settings.WavFile)).getParameters();
        assertNotNull(srp);

        ArrayList<String> targetLanguages = new ArrayList<>();
        targetLanguages.add("en-US");
        ParameterCollection<?> trp  = s.createTranslationRecognizerFromConfig(AudioConfig.fromWavFileInput(Settings.WavFile), "en-US", targetLanguages).getParameters();
        assertNotNull(crp);
    }

    // -----------------------------------------------------------------------
    // --- 
    // -----------------------------------------------------------------------

    private void testIsParametersString(ParameterCollection<?> p) {
        String name = "stringName";
        String value = "stringValue";
        
        p.set(name, value);
    }

    @Test
    public void testIsString() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);
        testIsParametersString(s.getParameters());

        ParameterCollection<?> sfp = s.getParameters();
        assertNotNull(sfp);
        testIsParametersString(sfp);
        
        ParameterCollection<?> crp  = s.createIntentRecognizerFromConfig(AudioConfig.fromWavFileInput(Settings.WavFile)).getParameters();
        assertNotNull(crp);
        testIsParametersString(crp);
        
        ParameterCollection<?> srp  = s.createSpeechRecognizerFromConfig(AudioConfig.fromWavFileInput(Settings.WavFile)).getParameters();
        assertNotNull(srp);
        testIsParametersString(srp);
        
        ArrayList<String> targetLanguages = new ArrayList<>();
        targetLanguages.add("en-US");
        ParameterCollection<?> trp  = s.createTranslationRecognizerFromConfig(AudioConfig.fromWavFileInput(Settings.WavFile), "en-US", targetLanguages).getParameters();
        assertNotNull(trp);
        testIsParametersString(trp);
    }

    // -----------------------------------------------------------------------
    // --- 
    // -----------------------------------------------------------------------

    private void testGetParametersString(ParameterCollection<?> p) {
        String name = "stringName";
        String value = "stringValue";
        
        p.set(name, value);
        
        assertEquals(value, p.getString(name));
        assertEquals(value, p.getString(name, "some-other-default-" + value));
    }

    @Test
    public void testSetStringStringTwoTimes() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);
        
        String value1 = "value1";
        String value2 = "value2";
        String key = "stringKey";
        String valueDefault = "valueDefault";
        
        ParameterCollection<?> sfp = s.getParameters();
        assertNotNull(sfp);
        
        String actual = sfp.getString(key, valueDefault);
        assertEquals(valueDefault, actual);
        
        sfp.set(key, value1);
        actual = sfp.getString(key, valueDefault);
        assertEquals(value1, actual);
        
        sfp.set(key, value2);
        actual = sfp.getString(key, valueDefault);
        assertEquals(value2, actual);
    }

    @Test
    public void testGetSetStringString() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);
        testIsParametersString(s.getParameters());

        ParameterCollection<?> sfp = s.getParameters();
        assertNotNull(sfp);
        testIsParametersString(sfp);
        
        ParameterCollection<?> crp  = s.createIntentRecognizerFromConfig(AudioConfig.fromWavFileInput(Settings.WavFile)).getParameters();
        assertNotNull(crp);
        testIsParametersString(crp);
        
        ParameterCollection<?> srp  = s.createSpeechRecognizerFromConfig(AudioConfig.fromWavFileInput(Settings.WavFile)).getParameters();
        assertNotNull(srp);
        testIsParametersString(srp);
        
        ArrayList<String> targetLanguages = new ArrayList<>();
        targetLanguages.add("en-US");
        ParameterCollection<?> trp  = s.createTranslationRecognizerFromConfig(AudioConfig.fromWavFileInput(Settings.WavFile), "en-US", targetLanguages).getParameters();
        assertNotNull(trp);
        testIsParametersString(trp);
    }


    // -----------------------------------------------------------------------
    // --- 
    // -----------------------------------------------------------------------

    private void testGetParametersStringString(ParameterCollection<?> p) {
        String name = "stringName3";
        String value = "stringValue";

        assertEquals("some-other-default-" + value, p.getString(name, "some-other-default-" + value));
        
        p.set(name, value);
        assertEquals(value, p.getString(name));
        assertEquals(value, p.getString(name, "some-other-default-" + value));
    }

    @Ignore("TODO string string tests fail")
    @Test
    public void testGetStringStringString() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);
        testGetParametersStringString(s.getParameters());

        ParameterCollection<?> sfp = s.getParameters();
        assertNotNull(sfp);
        testGetParametersStringString(sfp);
        
        ParameterCollection<?> crp  = s.createIntentRecognizerFromConfig(AudioConfig.fromWavFileInput(Settings.WavFile)).getParameters();
        assertNotNull(crp);
        testGetParametersStringString(crp);
        
        ParameterCollection<?> srp  = s.createSpeechRecognizerFromConfig(AudioConfig.fromWavFileInput(Settings.WavFile)).getParameters();
        assertNotNull(srp);
        testGetParametersStringString(srp);
        
        ArrayList<String> targetLanguages = new ArrayList<>();
        targetLanguages.add("en-US");
        ParameterCollection<?> trp  = s.createTranslationRecognizerFromConfig(AudioConfig.fromWavFileInput(Settings.WavFile), "en-US", targetLanguages).getParameters();
        assertNotNull(trp);
        testGetParametersStringString(trp);
    }
    
    // -----------------------------------------------------------------------
    // --- 
    // -----------------------------------------------------------------------

    @Test
    public void testClose() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        ParameterCollection<?> sfp = s.getParameters();
        assertNotNull(sfp);
        
        ParameterCollection<?> crp  = s.createIntentRecognizerFromConfig(AudioConfig.fromWavFileInput(Settings.WavFile)).getParameters();
        assertNotNull(crp);
        
        ParameterCollection<?> srp  = s.createSpeechRecognizerFromConfig(AudioConfig.fromWavFileInput(Settings.WavFile)).getParameters();
        assertNotNull(srp);
        
        ArrayList<String> targetLanguages = new ArrayList<>();
        targetLanguages.add("en-US");
        ParameterCollection<?> trp  = s.createTranslationRecognizerFromConfig(AudioConfig.fromWavFileInput(Settings.WavFile), "en-US", targetLanguages).getParameters();
        assertNotNull(trp);
        
        trp.close();
        srp.close();
        crp.close();
        sfp.close();
        s.close();
    }

}
