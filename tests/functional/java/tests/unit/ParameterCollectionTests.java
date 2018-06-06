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
        
        ParameterCollection<?> crp  = s.createIntentRecognizerWithFileInput(Settings.WaveFile).getParameters();
        assertNotNull(crp);
        
        ParameterCollection<?> srp  = s.createSpeechRecognizerWithFileInput(Settings.WaveFile).getParameters();
        assertNotNull(srp);

        ArrayList<String> targetLanguages = new ArrayList<>();
        targetLanguages.add("en-US");
        ParameterCollection<?> trp  = s.createTranslationRecognizerWithFileInput(Settings.WaveFile, "en-US", targetLanguages).getParameters();
        assertNotNull(crp);
    }

    // -----------------------------------------------------------------------
    // --- 
    // -----------------------------------------------------------------------

    private void testIsParametersString(ParameterCollection<?> p) {
        String name = "stringName";
        String value = "stringValue";
        
        p.set(name, value);
        
        assertTrue(p.isString(name));
        assertFalse(p.isInt(name));
        assertFalse(p.isBool(name));
    }

    private void testIsParametersInt(ParameterCollection<?> p) {
        String name = "intName";
        int value = 42;
        
        p.set(name, value);
        
        assertFalse(p.isString(name));
        assertTrue(p.isInt(name));
        assertFalse(p.isBool(name));
    }
    
    private void testIsParametersBool(ParameterCollection<?> p) {
        String name = "boolName";
        Boolean value = true;
        
        p.set(name, value);
        
        assertFalse(p.isString(name));
        assertFalse(p.isInt(name));
        assertTrue(p.isBool(name));
    }

    @Test
    public void testIsString() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);
        testIsParametersString(s.getParameters());

        ParameterCollection<?> sfp = s.getParameters();
        assertNotNull(sfp);
        testIsParametersString(sfp);
        
        ParameterCollection<?> crp  = s.createIntentRecognizerWithFileInput(Settings.WaveFile).getParameters();
        assertNotNull(crp);
        testIsParametersString(crp);
        
        ParameterCollection<?> srp  = s.createSpeechRecognizerWithFileInput(Settings.WaveFile).getParameters();
        assertNotNull(srp);
        testIsParametersString(srp);
        
        ArrayList<String> targetLanguages = new ArrayList<>();
        targetLanguages.add("en-US");
        ParameterCollection<?> trp  = s.createTranslationRecognizerWithFileInput(Settings.WaveFile, "en-US", targetLanguages).getParameters();
        assertNotNull(trp);
        testIsParametersString(trp);
    }

    @Ignore // TODO bool tests fail
    @Test
    public void testIsInt() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);
        testIsParametersInt(s.getParameters());

        ParameterCollection<?> sfp = s.getParameters();
        assertNotNull(sfp);
        testIsParametersInt(sfp);
        
        ParameterCollection<?> crp  = s.createIntentRecognizerWithFileInput(Settings.WaveFile).getParameters();
        assertNotNull(crp);
        testIsParametersInt(crp);
        
        ParameterCollection<?> srp  = s.createSpeechRecognizerWithFileInput(Settings.WaveFile).getParameters();
        assertNotNull(srp);
        testIsParametersInt(srp);
        
        ArrayList<String> targetLanguages = new ArrayList<>();
        targetLanguages.add("en-US");
        ParameterCollection<?> trp  = s.createTranslationRecognizerWithFileInput(Settings.WaveFile, "en-US", targetLanguages).getParameters();
        assertNotNull(trp);
        testIsParametersInt(trp);
    }

    @Ignore // TODO isbool tests fail
    @Test
    public void testIsBool() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);
        testIsParametersBool(s.getParameters());

        ParameterCollection<?> sfp = s.getParameters();
        assertNotNull(sfp);
        testIsParametersBool(sfp);
        
        ParameterCollection<?> crp  = s.createIntentRecognizerWithFileInput(Settings.WaveFile).getParameters();
        assertNotNull(crp);
        testIsParametersBool(crp);
        
        ParameterCollection<?> srp  = s.createSpeechRecognizerWithFileInput(Settings.WaveFile).getParameters();
        assertNotNull(srp);
        testIsParametersBool(srp);
        
        ArrayList<String> targetLanguages = new ArrayList<>();
        targetLanguages.add("en-US");
        ParameterCollection<?> trp  = s.createTranslationRecognizerWithFileInput(Settings.WaveFile, "en-US", targetLanguages).getParameters();
        assertNotNull(trp);
        testIsParametersBool(trp);
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

        try {
            p.getInt(name);
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }

        try {
            p.getBool(name);
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
    }

    private void testGetParametersInt(ParameterCollection<?> p) {
        String name = "intName";
        int value = 42;
        
        p.set(name, value);
        
        assertEquals(value, p.getInt(name));
        assertEquals(value, p.getInt(name, 2 + value));

        try {
            p.getString(name);
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }

        try {
            p.getBool(name);
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
    }
    
    private void testGetParametersBool(ParameterCollection<?> p) {
        String name = "boolName";
        Boolean value = true;
        
        p.set(name, value);
        
        assertEquals(value, p.getBool(name));
        assertEquals(value, p.getBool(name, !value));

        try {
            p.getString(name);
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }

        try {
            p.getInt(name);
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
    }
    

    @Test
    public void testGetSetStringString() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);
        testIsParametersString(s.getParameters());

        ParameterCollection<?> sfp = s.getParameters();
        assertNotNull(sfp);
        testIsParametersString(sfp);
        
        ParameterCollection<?> crp  = s.createIntentRecognizerWithFileInput(Settings.WaveFile).getParameters();
        assertNotNull(crp);
        testIsParametersString(crp);
        
        ParameterCollection<?> srp  = s.createSpeechRecognizerWithFileInput(Settings.WaveFile).getParameters();
        assertNotNull(srp);
        testIsParametersString(srp);
        
        ArrayList<String> targetLanguages = new ArrayList<>();
        targetLanguages.add("en-US");
        ParameterCollection<?> trp  = s.createTranslationRecognizerWithFileInput(Settings.WaveFile, "en-US", targetLanguages).getParameters();
        assertNotNull(trp);
        testIsParametersString(trp);
    }

    @Ignore // TODO int tests fail
    @Test
    public void testGetSetIntString() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);
        testGetParametersInt(s.getParameters());

        ParameterCollection<?> sfp = s.getParameters();
        assertNotNull(sfp);
        testGetParametersInt(sfp);
        
        ParameterCollection<?> crp  = s.createIntentRecognizerWithFileInput(Settings.WaveFile).getParameters();
        assertNotNull(crp);
        testGetParametersInt(crp);
        
        ParameterCollection<?> srp  = s.createSpeechRecognizerWithFileInput(Settings.WaveFile).getParameters();
        assertNotNull(srp);
        testGetParametersInt(srp);
        
        ArrayList<String> targetLanguages = new ArrayList<>();
        targetLanguages.add("en-US");
        ParameterCollection<?> trp  = s.createTranslationRecognizerWithFileInput(Settings.WaveFile, "en-US", targetLanguages).getParameters();
        assertNotNull(trp);
        testGetParametersInt(trp);
    }

    @Ignore // TODO getbool tests fail
    @Test
    public void testGetSetBoolString() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);
        testGetParametersBool(s.getParameters());

        ParameterCollection<?> sfp = s.getParameters();
        assertNotNull(sfp);
        testGetParametersBool(sfp);
        
        ParameterCollection<?> crp  = s.createIntentRecognizerWithFileInput(Settings.WaveFile).getParameters();
        assertNotNull(crp);
        testGetParametersBool(crp);
        
        ParameterCollection<?> srp  = s.createSpeechRecognizerWithFileInput(Settings.WaveFile).getParameters();
        assertNotNull(srp);
        testGetParametersBool(srp);
        
        ArrayList<String> targetLanguages = new ArrayList<>();
        targetLanguages.add("en-US");
        ParameterCollection<?> trp  = s.createTranslationRecognizerWithFileInput(Settings.WaveFile, "en-US", targetLanguages).getParameters();
        assertNotNull(trp);
        testGetParametersBool(trp);
    }

    // -----------------------------------------------------------------------
    // --- 
    // -----------------------------------------------------------------------

    private void testGetParametersStringString(ParameterCollection<?> p) {
        String name = "stringName3";
        String value = "stringValue";

        assertFalse(p.isString(name));
        assertEquals("some-other-default-" + value, p.getString(name, "some-other-default-" + value));
        
        p.set(name, value);
        assertEquals(value, p.getString(name));
        assertEquals(value, p.getString(name, "some-other-default-" + value));
    }

    private void testGetParametersStringInt(ParameterCollection<?> p) {
        String name = "intName3";
        int value = 42;
        
        assertFalse(p.isInt(name));
        assertEquals(2 + value, p.getInt(name, 2 + value));

        p.set(name, value);
        assertEquals(value, p.getInt(name));
        assertEquals(value, p.getInt(name, 2 + value));
    }
    
    private void testGetParametersStringBool(ParameterCollection<?> p) {
        String name = "boolName3";
        Boolean value = true;
        
        assertFalse(p.isBool(name));
        assertEquals(!value, p.getBool(name, !value));

        p.set(name, value);
        assertEquals(value, p.getBool(name));
        assertEquals(value, p.getBool(name, !value));
    }
    
    @Ignore // TODO string string tests fail
    @Test
    public void testGetStringStringString() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);
        testGetParametersStringString(s.getParameters());

        ParameterCollection<?> sfp = s.getParameters();
        assertNotNull(sfp);
        testGetParametersStringString(sfp);
        
        ParameterCollection<?> crp  = s.createIntentRecognizerWithFileInput(Settings.WaveFile).getParameters();
        assertNotNull(crp);
        testGetParametersStringString(crp);
        
        ParameterCollection<?> srp  = s.createSpeechRecognizerWithFileInput(Settings.WaveFile).getParameters();
        assertNotNull(srp);
        testGetParametersStringString(srp);
        
        ArrayList<String> targetLanguages = new ArrayList<>();
        targetLanguages.add("en-US");
        ParameterCollection<?> trp  = s.createTranslationRecognizerWithFileInput(Settings.WaveFile, "en-US", targetLanguages).getParameters();
        assertNotNull(trp);
        testGetParametersStringString(trp);
    }

    @Ignore // TODO string int tests fail.
    @Test
    public void testGetIntStringInt() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);
        testGetParametersStringInt(s.getParameters());

        ParameterCollection<?> sfp = s.getParameters();
        assertNotNull(sfp);
        testGetParametersStringInt(sfp);
        
        ParameterCollection<?> crp  = s.createIntentRecognizerWithFileInput(Settings.WaveFile).getParameters();
        assertNotNull(crp);
        testGetParametersStringInt(crp);
        
        ParameterCollection<?> srp  = s.createSpeechRecognizerWithFileInput(Settings.WaveFile).getParameters();
        assertNotNull(srp);
        testGetParametersStringInt(srp);
        
        ArrayList<String> targetLanguages = new ArrayList<>();
        targetLanguages.add("en-US");
        ParameterCollection<?> trp  = s.createTranslationRecognizerWithFileInput(Settings.WaveFile, "en-US", targetLanguages).getParameters();
        assertNotNull(trp);
        testGetParametersStringInt(trp);
    }

    @Ignore // TODO string bool tests fail
    @Test
    public void testGetBoolStringBoolean() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);
        testGetParametersStringBool(s.getParameters());

        ParameterCollection<?> sfp = s.getParameters();
        assertNotNull(sfp);
        testGetParametersStringBool(sfp);
        
        ParameterCollection<?> crp  = s.createIntentRecognizerWithFileInput(Settings.WaveFile).getParameters();
        assertNotNull(crp);
        testGetParametersStringBool(crp);
        
        ParameterCollection<?> srp  = s.createSpeechRecognizerWithFileInput(Settings.WaveFile).getParameters();
        assertNotNull(srp);
        testGetParametersStringBool(srp);
        
        ArrayList<String> targetLanguages = new ArrayList<>();
        targetLanguages.add("en-US");
        ParameterCollection<?> trp  = s.createTranslationRecognizerWithFileInput(Settings.WaveFile, "en-US", targetLanguages).getParameters();
        assertNotNull(trp);
        testGetParametersStringBool(trp);
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
        
        ParameterCollection<?> crp  = s.createIntentRecognizerWithFileInput(Settings.WaveFile).getParameters();
        assertNotNull(crp);
        
        ParameterCollection<?> srp  = s.createSpeechRecognizerWithFileInput(Settings.WaveFile).getParameters();
        assertNotNull(srp);
        
        ArrayList<String> targetLanguages = new ArrayList<>();
        targetLanguages.add("en-US");
        ParameterCollection<?> trp  = s.createTranslationRecognizerWithFileInput(Settings.WaveFile, "en-US", targetLanguages).getParameters();
        assertNotNull(trp);
        
        trp.close();
        srp.close();
        crp.close();
        sfp.close();
        s.close();
    }

}
