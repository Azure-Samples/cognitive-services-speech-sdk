package tests.unit;
//
//Copyright (c) Microsoft. All rights reserved.
//Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import static org.junit.Assert.*;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.Ignore;


import java.net.URI;
import java.util.ArrayList;

import com.microsoft.cognitiveservices.speech.audio.AudioConfig;
import com.microsoft.cognitiveservices.speech.audio.AudioInputStream;
import com.microsoft.cognitiveservices.speech.audio.PullAudioInputStreamCallback;
import com.microsoft.cognitiveservices.speech.FactoryParameterNames;
import com.microsoft.cognitiveservices.speech.ParameterCollection;
import com.microsoft.cognitiveservices.speech.Recognizer;
import com.microsoft.cognitiveservices.speech.SpeechFactory;
import com.microsoft.cognitiveservices.speech.SpeechRecognizer;
import com.microsoft.cognitiveservices.speech.intent.IntentRecognizer;
import com.microsoft.cognitiveservices.speech.translation.TranslationRecognizer;

import tests.Settings;

@SuppressWarnings("unused")
public class SpeechFactoryTests {

    @BeforeClass
    static public void setUpBeforeClass() throws Exception {
        // Override inputs, if necessary
        Settings.LoadSettings();
    }

    // -----------------------------------------------------------------------
    // --- 
    // -----------------------------------------------------------------------

    @Test(expected = NullPointerException.class)
    public void testFromSubscription1() {
        SpeechFactory s = SpeechFactory.fromSubscription(null, null);
    }

    @Test(expected = NullPointerException.class)
    public void testFromSubscription2() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, null);
    }

    @Test(expected = NullPointerException.class)
    public void testFromSubscription3() {
        SpeechFactory s = SpeechFactory.fromSubscription(null, Settings.SpeechRegion);
    }

    @Ignore("TODO why does illegal key succeed?")
    @Test(expected = NullPointerException.class)
    public void testFromSubscription4() {
        SpeechFactory s = SpeechFactory.fromSubscription("illegal", "illegal");
    }
    
    @Test() 
    public void testFromSubscriptionSuccess() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        assertNotNull(s);
        
        s.close();
    }

    // -----------------------------------------------------------------------
    // --- 
    // -----------------------------------------------------------------------

    @Test
    public void testGetSubscriptionKey() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        assertNotNull(s);
        assertEquals(Settings.SpeechSubscriptionKey, s.getSubscriptionKey());
        
        s.close();
    }

    // -----------------------------------------------------------------------
    // --- 
    // -----------------------------------------------------------------------

    @Test
    public void testGetRegion() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        assertNotNull(s);
        assertEquals(Settings.SpeechRegion, s.getRegion());
        
        s.close();
    }

    // -----------------------------------------------------------------------
    // --- 
    // -----------------------------------------------------------------------
    
    @Test(expected = NullPointerException.class)
    public void testFromEndpoint1() {
        SpeechFactory s = SpeechFactory.fromEndPoint(null, null);
    }

    @Test(expected = NullPointerException.class)
    public void testFromEndpoint2() {
        SpeechFactory s = SpeechFactory.fromEndPoint(null, Settings.SpeechRegion);
    }

    @Test(expected = NullPointerException.class)
    public void testFromEndpoint3() {
        SpeechFactory s = SpeechFactory.fromEndPoint(URI.create("http://www.example.com"), null);
    }

    @Ignore("TODO why does illegal token not fail?")
    @Test(expected = RuntimeException.class)
    public void testFromEndpoint4() {
        SpeechFactory s = SpeechFactory.fromEndPoint(URI.create("http://www.example.com"), "illegal-subscription");
    }
    
    @Test
    public void testFromEndpointSuccess() {
        SpeechFactory s = SpeechFactory.fromEndPoint(URI.create("http://www.example.com"), Settings.SpeechSubscriptionKey);
        
        assertNotNull(s);
        
        s.close();
    }

    // -----------------------------------------------------------------------
    // --- 
    // -----------------------------------------------------------------------

    @Test
    public void testGetParameters1() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        
        assertNotNull(s.getParameters());
        assertEquals(s.getRegion(), s.getParameters().getString(FactoryParameterNames.Region));
        assertEquals(s.getSubscriptionKey(), s.getParameters().getString(FactoryParameterNames.SubscriptionKey));
        
        s.close();
    }

    @Ignore("TODO why is a string an int?")
    @Test
    public void testGetParametersString() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        String name = "stringName";
        String value = "stringValue";
        ParameterCollection<?> p = s.getParameters();
        
        p.set(name, value);
        
        assertEquals(value, p.getString(name));
        assertEquals(value, p.getString(name, "some-other-default-" + value));

        s.close();
    }

    // -----------------------------------------------------------------------
    // --- 
    // -----------------------------------------------------------------------

    @Ignore("TODO: enable. however, this will crash the java vm at shutdown due to COM issues.")
    @Test
    public void testCreateSpeechRecognizer() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        
        SpeechRecognizer r = s.createSpeechRecognizer();
        
        assertNotNull(r);
        assertTrue(r instanceof Recognizer);
        
        r.close();
        s.close();
    }

    // -----------------------------------------------------------------------
    // --- 
    // -----------------------------------------------------------------------

    @Ignore("TODO: enable. however, this will crash the java vm at shutdown due to COM issues.")
    @Test
    public void testCreateSpeechRecognizerLanguage1() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            SpeechRecognizer r = s.createSpeechRecognizer(null);
            fail("not expected");
        }
        catch(NullPointerException ex) {
            // expected
        }
        
        s.close();
    }

    @Ignore("TODO: enable. however, this will crash the java vm at shutdown due to COM issues.")
    @Test
    public void testCreateSpeechRecognizerLanguage2() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            SpeechRecognizer r = s.createSpeechRecognizer("illegal-language");
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }
    
    @Ignore("TODO: enable. however, this will crash the java vm at shutdown due to COM issues.")
    @Test
    public void testCreateSpeechRecognizerLanguageSuccess() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        
        SpeechRecognizer r = s.createSpeechRecognizer("en-US");
        
        assertNotNull(r);
        assertTrue(r instanceof Recognizer);
        
        r.close();
        s.close();
    }


    // -----------------------------------------------------------------------
    // --- 
    // -----------------------------------------------------------------------

    @Test
    public void testCreateSpeechRecognizerString1() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            SpeechRecognizer r = s.createSpeechRecognizerFromConfig(AudioConfig.fromWavFileInput((String)null));
            fail("not expected");
        }
        catch(NullPointerException ex) {
            // expected
        }
        
        s.close();
    }

    @Test
    public void testCreateSpeechRecognizerString2() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            SpeechRecognizer r = s.createSpeechRecognizerFromConfig(AudioConfig.fromWavFileInput("illegal-" + Settings.WavFile));
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }

    @Test
    public void testCreateSpeechRecognizerStringSuccess() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        
        SpeechRecognizer r = s.createSpeechRecognizerFromConfig(AudioConfig.fromWavFileInput(Settings.WavFile));
        
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);
        
        r.close();
        s.close();
    }

    @Test
    public void testCreateSpeechRecognizerStringString1() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            SpeechRecognizer r = s.createSpeechRecognizerFromConfig(AudioConfig.fromWavFileInput((String)null), null);
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }

    @Test
    public void testCreateSpeechRecognizerStringString2() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            SpeechRecognizer r = s.createSpeechRecognizerFromConfig(AudioConfig.fromWavFileInput((String)null), "en-EN");
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }

    @Ignore("TODO why can create illegal language")
    @Test
    public void testCreateSpeechRecognizerStringString3() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            SpeechRecognizer r = s.createSpeechRecognizerFromConfig(AudioConfig.fromWavFileInput(Settings.WavFile), "illegal-language");
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }

    @Test
    public void testCreateSpeechRecognizerStringStringSuccess() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        SpeechRecognizer r = s.createSpeechRecognizerFromConfig(AudioConfig.fromWavFileInput(Settings.WavFile), "en-EN");
        
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        
        s.close();
    }

    // -----------------------------------------------------------------------
    // --- 
    // -----------------------------------------------------------------------

    @Ignore("TODO can create with null stream?")
    @Test
    public void testCreateSpeechRecognizerAudioInputStream1() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            SpeechRecognizer r = s.createSpeechRecognizerFromConfig(AudioConfig.fromStreamInput((AudioInputStream)null));
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }

    @Test
    public void testCreateSpeechRecognizerAudioInputStreamSuccess() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.WavFile);
        SpeechRecognizer r = s.createSpeechRecognizerFromConfig(AudioConfig.fromStreamInput(ais));
        
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);

        s.close();
    }
    
    @Test
    public void testCreateSpeechRecognizerAudioInputStreamString1() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            SpeechRecognizer r = s.createSpeechRecognizerFromConfig(AudioConfig.fromStreamInput((AudioInputStream)null), null);
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }
    
    @Ignore("TODO why does null stream not fail")
    @Test
    public void testCreateSpeechRecognizerAudioInputStreamString2() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            SpeechRecognizer r = s.createSpeechRecognizerFromConfig(AudioConfig.fromStreamInput((AudioInputStream)null), "en-US");
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }
    
    @Test
    public void testCreateSpeechRecognizerAudioInputStreamString3() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.WavFile);
            SpeechRecognizer r = s.createSpeechRecognizerFromConfig(AudioConfig.fromStreamInput(ais), null);
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }

    @Ignore("TODO why does illegal language not fail")
    @Test
    public void testCreateSpeechRecognizerAudioInputStreamString4() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.WavFile);
            SpeechRecognizer r = s.createSpeechRecognizerFromConfig(AudioConfig.fromStreamInput(ais), "illegal-language");
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }

    @Test
    public void testCreateSpeechRecognizerAudioInputStreamStringSuccess() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.WavFile);
        
        SpeechRecognizer r = s.createSpeechRecognizerFromConfig(AudioConfig.fromStreamInput(ais), "en-US");
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);

        ais.close();
        s.close();
    }

    // -----------------------------------------------------------------------
    // --- 
    // -----------------------------------------------------------------------

    @Ignore("TODO: enable. however, this will crash the java vm at shutdown due to COM issues.")
    @Test
    public void testCreateIntentRecognizer() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        IntentRecognizer r = s.createIntentRecognizer();

        s.close();
    }

    @Ignore("TODO: enable. however, this will crash the java vm at shutdown due to COM issues.")
    @Test
    public void testCreateIntentRecognizerLanguage1() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            IntentRecognizer r = s.createIntentRecognizer((String)null);
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }

        s.close();
    }
    
    @Ignore("TODO: enable. however, this will crash the java vm at shutdown due to COM issues.")
    @Test
    public void testCreateIntentRecognizerLanguage2() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            IntentRecognizer r = s.createIntentRecognizer("illegal-language");
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }

        s.close();
    }
       
    @Ignore("TODO: enable. however, this will crash the java vm at shutdown due to COM issues.")
    @Test
    public void testCreateIntentRecognizerLanguageSuccess() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        IntentRecognizer r = s.createIntentRecognizer("en-US");
        
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);

        r.close();
        s.close();
    }

    @Test
    public void testCreateIntentRecognizerString1() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            IntentRecognizer r = s.createIntentRecognizerFromConfig(AudioConfig.fromWavFileInput((String)null));
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }

        s.close();
    }
    
    @Test
    public void testCreateIntentRecognizerString2() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            IntentRecognizer r = s.createIntentRecognizerFromConfig(AudioConfig.fromWavFileInput("illegal-" + Settings.WavFile));
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }

        s.close();
    }
       
    @Test
    public void testCreateIntentRecognizerStringSuccess() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        IntentRecognizer r = s.createIntentRecognizerFromConfig(AudioConfig.fromWavFileInput(Settings.WavFile));
        
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);

        r.close();
        s.close();
    }
   

    @Ignore("TODO why can create with null stream?")
    @Test
    public void testCreateIntentRecognizerAudioInputStream1() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            IntentRecognizer r = s.createIntentRecognizerFromConfig(AudioConfig.fromStreamInput((AudioInputStream)null));
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }

    @Test
    public void testCreateIntentRecognizerAudioInputStreamSuccess() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.WavFile);
        IntentRecognizer r = s.createIntentRecognizerFromConfig(AudioConfig.fromStreamInput(ais));
        
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);

        s.close();
    }

    @Test
    public void testCreateIntentRecognizerAudioInputStreamString1() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            IntentRecognizer r = s.createIntentRecognizerFromConfig(AudioConfig.fromStreamInput((AudioInputStream)null), null);
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }

    @Ignore("TODO why does null stream not fail")
    @Test
    public void testCreateIntentRecognizerAudioInputStreamString2() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            IntentRecognizer r = s.createIntentRecognizerFromConfig(AudioConfig.fromStreamInput((AudioInputStream)null), "en-US");
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }

    @Ignore("TODO why does illegal language not fail")
    @Test
    public void testCreateIntentRecognizerAudioInputStreamString3() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.WavFile);
            IntentRecognizer r = s.createIntentRecognizerFromConfig(AudioConfig.fromStreamInput(ais), "illegal-language");
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }

    @Test
    public void testCreateIntentRecognizerAudioInputStreamStringSuccess() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.WavFile);
        IntentRecognizer r = s.createIntentRecognizerFromConfig(AudioConfig.fromStreamInput(ais), "en-US");
        
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);
        
        s.close();
    }

    // -----------------------------------------------------------------------
    // --- 
    // -----------------------------------------------------------------------

    @Ignore("TODO: enable. however, this will crash the java vm at shutdown due to COM issues.")
    @Test
    public void testCreateTranslationRecognizerStringArrayListOfString1() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            TranslationRecognizer r = s.createTranslationRecognizerFromConfig(null, null);
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }
    
    @Ignore("TODO: enable. however, this will crash the java vm at shutdown due to COM issues.")
    @Test
    public void testCreateTranslationRecognizerStringArrayListOfString2() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            TranslationRecognizer r = s.createTranslationRecognizerFromConfig("illegal", null);
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }
    
    @Ignore("TODO: enable. however, this will crash the java vm at shutdown due to COM issues.")
    @Test
    public void testCreateTranslationRecognizerStringArrayListOfString3() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            TranslationRecognizer r = s.createTranslationRecognizerFromConfig("en-EN", null);
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }

    @Ignore("TODO: enable. however, this will crash the java vm at shutdown due to COM issues.")
    @Test
    public void testCreateTranslationRecognizerStringArrayListOfString4() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            ArrayList<String> targets = new ArrayList<>();

            TranslationRecognizer r = s.createTranslationRecognizerFromConfig("en-EN", targets);
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }
    
    @Ignore("TODO: enable. however, this will crash the java vm at shutdown due to COM issues.")
    @Test
    public void testCreateTranslationRecognizerStringArrayListOfString5() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            ArrayList<String> targets = new ArrayList<>();
            targets.add("illegal");

            TranslationRecognizer r = s.createTranslationRecognizerFromConfig("en-EN", targets);
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }


    @Ignore("TODO: enable. however, this will crash the java vm at shutdown due to COM issues.")
    @Test
    public void testCreateTranslationRecognizerStringArrayListOfStringSuccess() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        ArrayList<String> targets = new ArrayList<>();
        targets.add("en-US");
        
        TranslationRecognizer r = s.createTranslationRecognizerFromConfig("en-EN", targets);
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);

        r.close();
        s.close();
    }

    // -----------------------------------------------------------------------
    // --- 
    // -----------------------------------------------------------------------

    @Ignore("TODO: enable. however, this will crash the java vm at shutdown due to COM issues.")
    @Test
    public void testCreateTranslationRecognizerStringArrayListOfStringString1() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            TranslationRecognizer r = s.createTranslationRecognizerFromConfig(((String)null), null, null);
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }
    
    @Ignore("TODO: enable. however, this will crash the java vm at shutdown due to COM issues.")
    @Test
    public void testCreateTranslationRecognizerStringArrayListOfStringString2() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            TranslationRecognizer r = s.createTranslationRecognizerFromConfig("illegal", null, null);
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }

    @Ignore("TODO: enable. however, this will crash the java vm at shutdown due to COM issues.")
    @Test
    public void testCreateTranslationRecognizerStringArrayListOfStringString3() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            TranslationRecognizer r = s.createTranslationRecognizerFromConfig("en-US", null, null);
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }

    @Ignore("TODO: enable. however, this will crash the java vm at shutdown due to COM issues.")
    @Test
    public void testCreateTranslationRecognizerStringArrayListOfStringString4() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            ArrayList<String> targets = new ArrayList<>();

            TranslationRecognizer r = s.createTranslationRecognizerFromConfig("en-US", targets, null);
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }

    @Ignore("TODO: enable. however, this will crash the java vm at shutdown due to COM issues.")
    @Test
    public void testCreateTranslationRecognizerStringArrayListOfStringString5() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            ArrayList<String> targets = new ArrayList<>();
            targets.add("illegal");

            TranslationRecognizer r = s.createTranslationRecognizerFromConfig("en-US", targets, null);
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }

    @Ignore("TODO: enable. however, this will crash the java vm at shutdown due to COM issues.")
    @Test
    public void testCreateTranslationRecognizerStringArrayListOfStringString6() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            ArrayList<String> targets = new ArrayList<>();
            targets.add("en-US");

            TranslationRecognizer r = s.createTranslationRecognizerFromConfig("en-US", targets, null);
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }

    @Ignore("TODO: enable. however, this will crash the java vm at shutdown due to COM issues.")
    @Test
    public void testCreateTranslationRecognizerStringArrayListOfStringString7() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            ArrayList<String> targets = new ArrayList<>();
            targets.add("en-US");

            TranslationRecognizer r = s.createTranslationRecognizerFromConfig("en-US", targets, "illegal");
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }

    @Ignore("TODO: enable. however, this will crash the java vm at shutdown due to COM issues.")
    @Test
    public void testCreateTranslationRecognizerStringArrayListOfStringStringSuccess() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        ArrayList<String> targets = new ArrayList<>();
        targets.add("en-US");

        TranslationRecognizer r = s.createTranslationRecognizerFromConfig("en-US", targets, "en-US");
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
    public void testCreateTranslationRecognizerWithFileInputStringStringArrayListOfString1() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            TranslationRecognizer r = s.createTranslationRecognizerFromConfig(AudioConfig.fromWavFileInput(null), null, null);
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }

    @Test
    public void testCreateTranslationRecognizerWithFileInputStringStringArrayListOfString2() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            TranslationRecognizer r = s.createTranslationRecognizerFromConfig(AudioConfig.fromWavFileInput("illegal-" + Settings.WavFile), null, null);
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }

    @Test
    public void testCreateTranslationRecognizerWithFileInputStringStringArrayListOfString3() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            TranslationRecognizer r = s.createTranslationRecognizerFromConfig(AudioConfig.fromWavFileInput(Settings.WavFile), null, null);
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }
    
    @Test
    public void testCreateTranslationRecognizerWithFileInputStringStringArrayListOfString4() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            TranslationRecognizer r = s.createTranslationRecognizerFromConfig(AudioConfig.fromWavFileInput(Settings.WavFile), "illegal", null);
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }
    
    @Test
    public void testCreateTranslationRecognizerWithFileInputStringStringArrayListOfString5() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            TranslationRecognizer r = s.createTranslationRecognizerFromConfig(AudioConfig.fromWavFileInput(Settings.WavFile), "en-US", null);
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }
    
    @Test
    public void testCreateTranslationRecognizerWithFileInputStringStringArrayListOfString6() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            ArrayList<String> targets = new ArrayList<>();
            
            TranslationRecognizer r = s.createTranslationRecognizerFromConfig(AudioConfig.fromWavFileInput(Settings.WavFile), "en-US", targets);
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }

    @Ignore("TODO why can create with illegal source language")
    @Test
    public void testCreateTranslationRecognizerWithFileInputStringStringArrayListOfString7() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            ArrayList<String> targets = new ArrayList<>();
            targets.add("illegal");
            
            TranslationRecognizer r = s.createTranslationRecognizerFromConfig(AudioConfig.fromWavFileInput(Settings.WavFile), "en-US", targets);
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }

    @Test
    public void testCreateTranslationRecognizerWithFileInputStringStringArrayListOfStringSuccess() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        ArrayList<String> targets = new ArrayList<>();
        targets.add("en-US");
        
        TranslationRecognizer r = s.createTranslationRecognizerFromConfig(AudioConfig.fromWavFileInput(Settings.WavFile), "en-US", targets);
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
    public void testCreateTranslationRecognizerWithFileInputStringStringArrayListOfStringString1() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            TranslationRecognizer r = s.createTranslationRecognizerFromConfig(AudioConfig.fromWavFileInput(null), null, null, null);
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }
    
    @Test
    public void testCreateTranslationRecognizerWithFileInputStringStringArrayListOfStringString2() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            TranslationRecognizer r = s.createTranslationRecognizerFromConfig(AudioConfig.fromWavFileInput("illegal-" + Settings.WavFile), null, null, null);
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }

    @Test
    public void testCreateTranslationRecognizerWithFileInputStringStringArrayListOfStringString3() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            TranslationRecognizer r = s.createTranslationRecognizerFromConfig(AudioConfig.fromWavFileInput(Settings.WavFile), null, null, null);
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }

    @Test
    public void testCreateTranslationRecognizerWithFileInputStringStringArrayListOfStringString4() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            TranslationRecognizer r = s.createTranslationRecognizerFromConfig(AudioConfig.fromWavFileInput(Settings.WavFile), "illegal", null, null);
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }
    
    @Test
    public void testCreateTranslationRecognizerWithFileInputStringStringArrayListOfStringString5() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            TranslationRecognizer r = s.createTranslationRecognizerFromConfig(AudioConfig.fromWavFileInput(Settings.WavFile), "en-US", null, null);
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }

    @Test
    public void testCreateTranslationRecognizerWithFileInputStringStringArrayListOfStringString6() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            ArrayList<String> targets = new ArrayList<>();
            
            TranslationRecognizer r = s.createTranslationRecognizerFromConfig(AudioConfig.fromWavFileInput(Settings.WavFile), "en-US", targets, null);
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }

    @Test
    public void testCreateTranslationRecognizerWithFileInputStringStringArrayListOfStringString7() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            ArrayList<String> targets = new ArrayList<>();
            targets.add("illegal");
            
            TranslationRecognizer r = s.createTranslationRecognizerFromConfig(AudioConfig.fromWavFileInput(Settings.WavFile), "en-US", targets, null);
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }

    @Test
    public void testCreateTranslationRecognizerWithFileInputStringStringArrayListOfStringString8() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            ArrayList<String> targets = new ArrayList<>();
            targets.add("en-US");
            
            TranslationRecognizer r = s.createTranslationRecognizerFromConfig(AudioConfig.fromWavFileInput(Settings.WavFile), "en-US", targets, null);
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }

    @Ignore("TODO why can create with illegal voice?")
    @Test
    public void testCreateTranslationRecognizerWithFileInputStringStringArrayListOfStringString9() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            ArrayList<String> targets = new ArrayList<>();
            targets.add("en-US");
            
            TranslationRecognizer r = s.createTranslationRecognizerFromConfig(AudioConfig.fromWavFileInput(Settings.WavFile), "en-US", targets, "illegal");
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }

    @Test
    public void testCreateTranslationRecognizerWithFileInputStringStringArrayListOfStringStringSuccess() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        ArrayList<String> targets = new ArrayList<>();
        targets.add("en-US");
        
        TranslationRecognizer r = s.createTranslationRecognizerFromConfig(AudioConfig.fromWavFileInput(Settings.WavFile), "en-US", targets, "en-US");
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
    public void testCreateTranslationRecognizerWithStreamAudioInputStreamStringArrayListOfString1() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            TranslationRecognizer r = s.createTranslationRecognizerFromConfig(AudioConfig.fromStreamInput((PullAudioInputStreamCallback)null), ((String)null), null);
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }

    @Test
    public void testCreateTranslationRecognizerWithStreamAudioInputStreamStringArrayListOfString2() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.WavFile);
            
            TranslationRecognizer r = s.createTranslationRecognizerFromConfig(AudioConfig.fromStreamInput(ais), ((String)null), null);
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }

    @Test
    public void testCreateTranslationRecognizerWithStreamAudioInputStreamStringArrayListOfString3() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.WavFile);
            
            TranslationRecognizer r = s.createTranslationRecognizerFromConfig(AudioConfig.fromStreamInput(ais), "illegal", null);
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }
    
    @Test
    public void testCreateTranslationRecognizerWithStreamAudioInputStreamStringArrayListOfString4() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.WavFile);
            
            TranslationRecognizer r = s.createTranslationRecognizerFromConfig(AudioConfig.fromStreamInput(ais), "en-US", null);
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }

    @Test
    public void testCreateTranslationRecognizerWithStreamAudioInputStreamStringArrayListOfString5() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            ArrayList<String> targets = new ArrayList<>();
            
            WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.WavFile);
            
            TranslationRecognizer r = s.createTranslationRecognizerFromConfig(AudioConfig.fromStreamInput(ais), "en-US", targets);
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }

    @Ignore("TODO why does illegal target language not fail")
    @Test
    public void testCreateTranslationRecognizerWithStreamAudioInputStreamStringArrayListOfString6() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            ArrayList<String> targets = new ArrayList<>();
            targets.add("illegal");
            
            WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.WavFile);
            
            TranslationRecognizer r = s.createTranslationRecognizerFromConfig(AudioConfig.fromStreamInput(ais), "en-US", targets);
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }

    @Test
    public void testCreateTranslationRecognizerWithStreamAudioInputStreamStringArrayListOfStringSuccess() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        ArrayList<String> targets = new ArrayList<>();
        targets.add("en-US");
        
        WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.WavFile);
        
        TranslationRecognizer r = s.createTranslationRecognizerFromConfig(AudioConfig.fromStreamInput(ais), "en-US", targets);
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
    public void testCreateTranslationRecognizerWithStreamAudioInputStreamStringArrayListOfStringString1() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            TranslationRecognizer r = s.createTranslationRecognizerFromConfig(AudioConfig.fromStreamInput((PullAudioInputStreamCallback)null), ((String)null), null, null);
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }

        s.close();
    }
    
    @Test
    public void testCreateTranslationRecognizerWithStreamAudioInputStreamStringArrayListOfStringString2() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.WavFile);
            
            TranslationRecognizer r = s.createTranslationRecognizerFromConfig(AudioConfig.fromStreamInput(ais), ((String)null), null, null);
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }

        s.close();
    }

    @Test
    public void testCreateTranslationRecognizerWithStreamAudioInputStreamStringArrayListOfStringString3() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.WavFile);
            
            TranslationRecognizer r = s.createTranslationRecognizerFromConfig(AudioConfig.fromStreamInput(ais), "illegal", null, null);
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }

        s.close();
    }

    @Test
    public void testCreateTranslationRecognizerWithStreamAudioInputStreamStringArrayListOfStringString4() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.WavFile);
            
            TranslationRecognizer r = s.createTranslationRecognizerFromConfig(AudioConfig.fromStreamInput(ais), "en-US", null, null);
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }

        s.close();
    }
    
    @Test
    public void testCreateTranslationRecognizerWithStreamAudioInputStreamStringArrayListOfStringString5() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            ArrayList<String> targets = new ArrayList<>();
            
            WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.WavFile);
            
            TranslationRecognizer r = s.createTranslationRecognizerFromConfig(AudioConfig.fromStreamInput(ais), "en-US", targets, null);
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }

        s.close();
    }

    @Test
    public void testCreateTranslationRecognizerWithStreamAudioInputStreamStringArrayListOfStringString6() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            ArrayList<String> targets = new ArrayList<>();
            targets.add("illegal");
            
            WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.WavFile);
            
            TranslationRecognizer r = s.createTranslationRecognizerFromConfig(AudioConfig.fromStreamInput(ais), "en-US", targets, null);
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }

        s.close();
    }

    @Test
    public void testCreateTranslationRecognizerWithStreamAudioInputStreamStringArrayListOfStringString7() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            ArrayList<String> targets = new ArrayList<>();
            targets.add("en-US");
            
            WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.WavFile);
            
            TranslationRecognizer r = s.createTranslationRecognizerFromConfig(AudioConfig.fromStreamInput(ais), "en-US", targets, null);
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }

        s.close();
    }

    @Ignore("TODO create with stream on illegal voice does not fail?")
    @Test
    public void testCreateTranslationRecognizerWithStreamAudioInputStreamStringArrayListOfStringString8() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            ArrayList<String> targets = new ArrayList<>();
            targets.add("en-US");
            
            WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.WavFile);
            
            TranslationRecognizer r = s.createTranslationRecognizerFromConfig(AudioConfig.fromStreamInput(ais), "en-US", targets, "illegal");
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }

        s.close();
    }

    @Test
    public void testCreateTranslationRecognizerWithStreamAudioInputStreamStringArrayListOfStringStringSuccess() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        ArrayList<String> targets = new ArrayList<>();
        targets.add("en-US");
        
        WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.WavFile);
        
        TranslationRecognizer r = s.createTranslationRecognizerFromConfig(AudioConfig.fromStreamInput(ais), "en-US", targets, "en-US");
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
    public void testClose() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        
        s.close();
    }

    // -----------------------------------------------------------------------
    // --- 
    // -----------------------------------------------------------------------

    @Test
    public void testGetFactoryImpl() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        
        assertNotNull(s);
        assertNotNull(s.getFactoryImpl());
    }

}
