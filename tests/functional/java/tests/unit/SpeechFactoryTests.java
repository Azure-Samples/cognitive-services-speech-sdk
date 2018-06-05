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

import com.microsoft.cognitiveservices.speech.AudioInputStream;
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

    @Ignore // TODO why does illegal key succeed? 
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

    @Test
    public void testSetRegion() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        assertNotNull(s);
        assertEquals(Settings.SpeechRegion, s.getRegion());

        String newRegion = Settings.SpeechRegion + Settings.LuisSubscriptionKey;
        s.setRegion(newRegion);
        
        assertEquals(newRegion, s.getRegion());
        
        s.close();
    }


    // -----------------------------------------------------------------------
    // --- 
    // -----------------------------------------------------------------------
    
    @Test(expected = NullPointerException.class)
    public void testFromAuthorizationToken1() {
        SpeechFactory s = SpeechFactory.fromAuthorizationToken(null, null);
    }

    @Test(expected = NullPointerException.class)
    public void testFromAuthorizationToken2() {
        SpeechFactory s = SpeechFactory.fromAuthorizationToken(null, Settings.SpeechRegion);
    }

    @Test(expected = NullPointerException.class)
    public void testFromAuthorizationToken3() {
        SpeechFactory s = SpeechFactory.fromAuthorizationToken(Settings.SpeechAuthorizationToken, null);
    }

    @Ignore // TODO why does illegal token not fail? 
    @Test(expected = RuntimeException.class)
    public void testFromAuthorizationToken4() {
        SpeechFactory s = SpeechFactory.fromAuthorizationToken("illegal-token", "illegal-region");
    }
    
    @Test
    public void testFromAuthorizationTokenSuccess() {
        SpeechFactory s = SpeechFactory.fromAuthorizationToken(Settings.SpeechAuthorizationToken, Settings.SpeechRegion);
        
        assertNotNull(s);
        
        s.close();
    }

    
    // -----------------------------------------------------------------------
    // --- 
    // -----------------------------------------------------------------------
    
    @Test
    public void testGetAuthorizationToken() {
        SpeechFactory s = SpeechFactory.fromAuthorizationToken(Settings.SpeechAuthorizationToken, Settings.SpeechRegion);

        assertEquals(Settings.SpeechAuthorizationToken, s.getAuthorizationToken());
        assertEquals(Settings.SpeechRegion, s.getRegion());
        
        s.close();
    }

    @Test
    public void testSetAuthorizationToken() {
        SpeechFactory s = SpeechFactory.fromAuthorizationToken(Settings.SpeechAuthorizationToken, Settings.SpeechRegion);

        String newToken = "new-" + Settings.SpeechAuthorizationToken;
        s.setAuthorizationToken(newToken);
        
        assertEquals(newToken, s.getAuthorizationToken());
        
        s.close();
    }

    // -----------------------------------------------------------------------
    // --- 
    // -----------------------------------------------------------------------

    @Test
    public void testGetEndpoint() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        assertNotNull(s.getEndpoint());
        
        s.close();
    }

    @Test
    public void testSetEndpoint() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        URI newEndpoint = URI.create("https://www.example.com/api/v1/");
        s.setEndpoint(newEndpoint);

        assertEquals(newEndpoint, s.getEndpoint());
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
        assertEquals(s.getEndpoint(), URI.create(s.getParameters().getString(FactoryParameterNames.Endpoint)));
        assertEquals(s.getSubscriptionKey(), s.getParameters().getString(FactoryParameterNames.SubscriptionKey));
        
        s.close();
    }

    @Test
    public void testGetParameters2() {
        SpeechFactory s = SpeechFactory.fromAuthorizationToken(Settings.SpeechAuthorizationToken, Settings.SpeechRegion);
        
        assertNotNull(s.getParameters());
        assertEquals(s.getRegion(), s.getParameters().getString(FactoryParameterNames.Region));
        assertEquals(s.getAuthorizationToken(), s.getParameters().getString(FactoryParameterNames.AuthorizationToken));
        
        s.close();
    }

    @Ignore // TODO why is a string an int? 
    @Test
    public void testGetParametersString() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        String name = "stringName";
        String value = "stringValue";
        ParameterCollection<?> p = s.getParameters();
        
        p.set(name, value);
        
        assertTrue(p.isString(name));
        assertFalse(p.isInt(name));
        assertFalse(p.isBool(name));
        
        assertEquals(value, p.getString(name));
        assertEquals(value, p.getString(name, "some-other-default-" + value));

        try {
            p.getInt(name);
            fail("unexpected");
        }
        catch(RuntimeException ex) {
            // expected
        }

        try {
            p.getBool(name);
            fail("unexpected");
        }
        catch(RuntimeException ex) {
            // expected
        }

        s.close();
    }

    @Ignore // TODO why is a an int a bool
    @Test
    public void testGetParametersInt() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        String name = "stringName";
        int value = 42;
        ParameterCollection<?> p = s.getParameters();
        
        p.set(name, value);
        
        assertFalse(p.isString(name));
        assertTrue(p.isInt(name));
        assertFalse(p.isBool(name));
        
        assertEquals(value, p.getInt(name));
        assertEquals(value, p.getInt(name, 2 * value));

        try {
            p.getString(name);
            fail("unexpected");
        }
        catch(RuntimeException ex) {
            // expected
        }

        try {
            p.getBool(name);
            fail("unexpected");
        }
        catch(RuntimeException ex) {
            // expected
        }

        s.close();
    }

    @Ignore // TODO why is a string a bool? 
    @Test
    public void testGetParametersBool() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        String name = "stringName";
        Boolean value = true;
        ParameterCollection<?> p = s.getParameters();
        
        p.set(name, value);
        
        assertFalse(p.isString(name));
        assertFalse(p.isInt(name));
        assertTrue(p.isBool(name));
        
        assertEquals(value, p.getBool(name));
        assertEquals(value, p.getBool(name, !value));
        
        try {
            p.getString(name);
            fail("unexpected");
        }
        catch(RuntimeException ex) {
            // expected
        }

        try {
            p.getInt(name);
            fail("unexpected");
        }
        catch(RuntimeException ex) {
            // expected
        }

        s.close();
    }

    // -----------------------------------------------------------------------
    // --- 
    // -----------------------------------------------------------------------

    @Ignore // TODO: enable. however, this will crash the java vm at shutdown due to COM issues.
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

    @Test
    public void testCreateSpeechRecognizerString1() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            SpeechRecognizer r = s.createSpeechRecognizerWithFileInput((String)null);
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
            SpeechRecognizer r = s.createSpeechRecognizerWithFileInput("illegal-" + Settings.WaveFile);
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
        
        SpeechRecognizer r = s.createSpeechRecognizerWithFileInput(Settings.WaveFile);
        
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
            SpeechRecognizer r = s.createSpeechRecognizerWithFileInput((String)null, null);
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
            SpeechRecognizer r = s.createSpeechRecognizerWithFileInput((String)null, "en-EN");
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }

    @Ignore // TODO why can create illegal language 
    @Test
    public void testCreateSpeechRecognizerStringString3() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            SpeechRecognizer r = s.createSpeechRecognizerWithFileInput(Settings.WaveFile, "illegal-language");
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

        SpeechRecognizer r = s.createSpeechRecognizerWithFileInput(Settings.WaveFile, "en-EN");
        
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        
        s.close();
    }

    // -----------------------------------------------------------------------
    // --- 
    // -----------------------------------------------------------------------

    @Ignore // TODO can create with null stream? 
    @Test
    public void testCreateSpeechRecognizerAudioInputStream1() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            SpeechRecognizer r = s.createSpeechRecognizerWithStream((AudioInputStream)null);
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

        WaveFileAudioInputStream ais = new WaveFileAudioInputStream(Settings.WaveFile);
        SpeechRecognizer r = s.createSpeechRecognizerWithStream(ais);
        
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);

        s.close();
    }
    
    @Test
    public void testCreateSpeechRecognizerAudioInputStreamString1() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            SpeechRecognizer r = s.createSpeechRecognizerWithStream((AudioInputStream)null, null);
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }
    
    @Ignore // TODO why does null stream not fail 
    @Test
    public void testCreateSpeechRecognizerAudioInputStreamString2() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            SpeechRecognizer r = s.createSpeechRecognizerWithStream((AudioInputStream)null, "en-US");
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
            WaveFileAudioInputStream ais = new WaveFileAudioInputStream(Settings.WaveFile);
            SpeechRecognizer r = s.createSpeechRecognizerWithStream(ais, null);
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }

    @Ignore // TODO why does illegal language not fail 
    @Test
    public void testCreateSpeechRecognizerAudioInputStreamString4() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            WaveFileAudioInputStream ais = new WaveFileAudioInputStream(Settings.WaveFile);
            SpeechRecognizer r = s.createSpeechRecognizerWithStream(ais, "illegal-language");
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

        WaveFileAudioInputStream ais = new WaveFileAudioInputStream(Settings.WaveFile);
        
        SpeechRecognizer r = s.createSpeechRecognizerWithStream(ais, "en-US");
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);

        ais.close();
        s.close();
    }

    // -----------------------------------------------------------------------
    // --- 
    // -----------------------------------------------------------------------

    @Ignore // TODO: enable. however, this will crash the java vm at shutdown due to COM issues.
    @Test
    public void testCreateIntentRecognizer() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        IntentRecognizer r = s.createIntentRecognizer();

        s.close();
    }

    @Test
    public void testCreateIntentRecognizerString1() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            IntentRecognizer r = s.createIntentRecognizerWithFileInput((String)null);
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
            IntentRecognizer r = s.createIntentRecognizerWithFileInput("illegal-" + Settings.WaveFile);
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

        IntentRecognizer r = s.createIntentRecognizerWithFileInput(Settings.WaveFile);
        
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);

        r.close();
        s.close();
    }
   

    @Ignore // TODO why can create with null stream? 
    @Test
    public void testCreateIntentRecognizerAudioInputStream1() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            IntentRecognizer r = s.createIntentRecognizerWithStream((AudioInputStream)null);
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

        WaveFileAudioInputStream ais = new WaveFileAudioInputStream(Settings.WaveFile);
        IntentRecognizer r = s.createIntentRecognizerWithStream(ais);
        
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);

        s.close();
    }

    @Test
    public void testCreateIntentRecognizerAudioInputStreamString1() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            IntentRecognizer r = s.createIntentRecognizerWithStream((AudioInputStream)null, null);
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }

    @Ignore // TODO why does null stream not fail 
    @Test
    public void testCreateIntentRecognizerAudioInputStreamString2() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            IntentRecognizer r = s.createIntentRecognizerWithStream((AudioInputStream)null, "en-US");
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }

    @Ignore // TODO why does illegal language not fail 
    @Test
    public void testCreateIntentRecognizerAudioInputStreamString3() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            WaveFileAudioInputStream ais = new WaveFileAudioInputStream(Settings.WaveFile);
            IntentRecognizer r = s.createIntentRecognizerWithStream(ais, "illegal-language");
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

        WaveFileAudioInputStream ais = new WaveFileAudioInputStream(Settings.WaveFile);
        IntentRecognizer r = s.createIntentRecognizerWithStream(ais, "en-US");
        
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);
        
        s.close();
    }

    // -----------------------------------------------------------------------
    // --- 
    // -----------------------------------------------------------------------

    @Ignore // TODO: enable. however, this will crash the java vm at shutdown due to COM issues.
    @Test
    public void testCreateTranslationRecognizerStringArrayListOfString1() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            TranslationRecognizer r = s.createTranslationRecognizer(null, null);
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }
    
    @Ignore // TODO: enable. however, this will crash the java vm at shutdown due to COM issues.
    @Test
    public void testCreateTranslationRecognizerStringArrayListOfString2() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            TranslationRecognizer r = s.createTranslationRecognizer("illegal", null);
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }
    
    @Ignore // TODO: enable. however, this will crash the java vm at shutdown due to COM issues.
    @Test
    public void testCreateTranslationRecognizerStringArrayListOfString3() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            TranslationRecognizer r = s.createTranslationRecognizer("en-EN", null);
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }

    @Ignore // TODO: enable. however, this will crash the java vm at shutdown due to COM issues.
    @Test
    public void testCreateTranslationRecognizerStringArrayListOfString4() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            ArrayList<String> targets = new ArrayList<>();

            TranslationRecognizer r = s.createTranslationRecognizer("en-EN", targets);
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }
    
    @Ignore // TODO: enable. however, this will crash the java vm at shutdown due to COM issues.
    @Test
    public void testCreateTranslationRecognizerStringArrayListOfString5() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            ArrayList<String> targets = new ArrayList<>();
            targets.add("illegal");

            TranslationRecognizer r = s.createTranslationRecognizer("en-EN", targets);
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }


    @Ignore // TODO: enable. however, this will crash the java vm at shutdown due to COM issues.
    @Test
    public void testCreateTranslationRecognizerStringArrayListOfStringSuccess() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        ArrayList<String> targets = new ArrayList<>();
        targets.add("en-US");
        
        TranslationRecognizer r = s.createTranslationRecognizer("en-EN", targets);
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);

        r.close();
        s.close();
    }

    // -----------------------------------------------------------------------
    // --- 
    // -----------------------------------------------------------------------

    @Ignore // TODO: enable. however, this will crash the java vm at shutdown due to COM issues.
    @Test
    public void testCreateTranslationRecognizerStringArrayListOfStringString1() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            TranslationRecognizer r = s.createTranslationRecognizer(null, null, null);
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }
    
    @Ignore // TODO: enable. however, this will crash the java vm at shutdown due to COM issues.
    @Test
    public void testCreateTranslationRecognizerStringArrayListOfStringString2() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            TranslationRecognizer r = s.createTranslationRecognizer("illegal", null, null);
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }

    @Ignore // TODO: enable. however, this will crash the java vm at shutdown due to COM issues.
    @Test
    public void testCreateTranslationRecognizerStringArrayListOfStringString3() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            TranslationRecognizer r = s.createTranslationRecognizer("en-US", null, null);
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }

    @Ignore // TODO: enable. however, this will crash the java vm at shutdown due to COM issues.
    @Test
    public void testCreateTranslationRecognizerStringArrayListOfStringString4() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            ArrayList<String> targets = new ArrayList<>();

            TranslationRecognizer r = s.createTranslationRecognizer("en-US", targets, null);
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }

    @Ignore // TODO: enable. however, this will crash the java vm at shutdown due to COM issues.
    @Test
    public void testCreateTranslationRecognizerStringArrayListOfStringString5() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            ArrayList<String> targets = new ArrayList<>();
            targets.add("illegal");

            TranslationRecognizer r = s.createTranslationRecognizer("en-US", targets, null);
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }

    @Ignore // TODO: enable. however, this will crash the java vm at shutdown due to COM issues.
    @Test
    public void testCreateTranslationRecognizerStringArrayListOfStringString6() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            ArrayList<String> targets = new ArrayList<>();
            targets.add("en-US");

            TranslationRecognizer r = s.createTranslationRecognizer("en-US", targets, null);
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }

    @Ignore // TODO: enable. however, this will crash the java vm at shutdown due to COM issues.
    @Test
    public void testCreateTranslationRecognizerStringArrayListOfStringString7() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            ArrayList<String> targets = new ArrayList<>();
            targets.add("en-US");

            TranslationRecognizer r = s.createTranslationRecognizer("en-US", targets, "illegal");
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }

    @Ignore // TODO: enable. however, this will crash the java vm at shutdown due to COM issues.
    @Test
    public void testCreateTranslationRecognizerStringArrayListOfStringStringSuccess() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        ArrayList<String> targets = new ArrayList<>();
        targets.add("en-US");

        TranslationRecognizer r = s.createTranslationRecognizer("en-US", targets, "en-US");
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
            TranslationRecognizer r = s.createTranslationRecognizerWithFileInput(null, null, null);
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
            TranslationRecognizer r = s.createTranslationRecognizerWithFileInput("illegal-" + Settings.WaveFile, null, null);
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
            TranslationRecognizer r = s.createTranslationRecognizerWithFileInput(Settings.WaveFile, null, null);
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
            TranslationRecognizer r = s.createTranslationRecognizerWithFileInput(Settings.WaveFile, "illegal", null);
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
            TranslationRecognizer r = s.createTranslationRecognizerWithFileInput(Settings.WaveFile, "en-US", null);
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
            
            TranslationRecognizer r = s.createTranslationRecognizerWithFileInput(Settings.WaveFile, "en-US", targets);
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }

    @Ignore // TODO why can create with illegal source language 
    @Test
    public void testCreateTranslationRecognizerWithFileInputStringStringArrayListOfString7() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            ArrayList<String> targets = new ArrayList<>();
            targets.add("illegal");
            
            TranslationRecognizer r = s.createTranslationRecognizerWithFileInput(Settings.WaveFile, "en-US", targets);
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
        
        TranslationRecognizer r = s.createTranslationRecognizerWithFileInput(Settings.WaveFile, "en-US", targets);
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
            TranslationRecognizer r = s.createTranslationRecognizerWithFileInput(null, null, null, null);
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
            TranslationRecognizer r = s.createTranslationRecognizerWithFileInput("illegal-" + Settings.WaveFile, null, null, null);
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
            TranslationRecognizer r = s.createTranslationRecognizerWithFileInput(Settings.WaveFile, null, null, null);
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
            TranslationRecognizer r = s.createTranslationRecognizerWithFileInput(Settings.WaveFile, "illegal", null, null);
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
            TranslationRecognizer r = s.createTranslationRecognizerWithFileInput(Settings.WaveFile, "en-US", null, null);
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
            
            TranslationRecognizer r = s.createTranslationRecognizerWithFileInput(Settings.WaveFile, "en-US", targets, null);
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
            
            TranslationRecognizer r = s.createTranslationRecognizerWithFileInput(Settings.WaveFile, "en-US", targets, null);
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
            
            TranslationRecognizer r = s.createTranslationRecognizerWithFileInput(Settings.WaveFile, "en-US", targets, null);
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }

    @Ignore // TODO why can create with illegal voice? 
    @Test
    public void testCreateTranslationRecognizerWithFileInputStringStringArrayListOfStringString9() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            ArrayList<String> targets = new ArrayList<>();
            targets.add("en-US");
            
            TranslationRecognizer r = s.createTranslationRecognizerWithFileInput(Settings.WaveFile, "en-US", targets, "illegal");
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
        
        TranslationRecognizer r = s.createTranslationRecognizerWithFileInput(Settings.WaveFile, "en-US", targets, "en-US");
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
            TranslationRecognizer r = s.createTranslationRecognizerWithStream(null, null, null);
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
            WaveFileAudioInputStream ais = new WaveFileAudioInputStream(Settings.WaveFile);
            
            TranslationRecognizer r = s.createTranslationRecognizerWithStream(ais, null, null);
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
            WaveFileAudioInputStream ais = new WaveFileAudioInputStream(Settings.WaveFile);
            
            TranslationRecognizer r = s.createTranslationRecognizerWithStream(ais, "illegal", null);
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
            WaveFileAudioInputStream ais = new WaveFileAudioInputStream(Settings.WaveFile);
            
            TranslationRecognizer r = s.createTranslationRecognizerWithStream(ais, "en-US", null);
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
            
            WaveFileAudioInputStream ais = new WaveFileAudioInputStream(Settings.WaveFile);
            
            TranslationRecognizer r = s.createTranslationRecognizerWithStream(ais, "en-US", targets);
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }

    @Ignore // TODO why does illegal target language not fail 
    @Test
    public void testCreateTranslationRecognizerWithStreamAudioInputStreamStringArrayListOfString6() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            ArrayList<String> targets = new ArrayList<>();
            targets.add("illegal");
            
            WaveFileAudioInputStream ais = new WaveFileAudioInputStream(Settings.WaveFile);
            
            TranslationRecognizer r = s.createTranslationRecognizerWithStream(ais, "en-US", targets);
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
        
        WaveFileAudioInputStream ais = new WaveFileAudioInputStream(Settings.WaveFile);
        
        TranslationRecognizer r = s.createTranslationRecognizerWithStream(ais, "en-US", targets);
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
            TranslationRecognizer r = s.createTranslationRecognizerWithStream(null, null, null, null);
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
            WaveFileAudioInputStream ais = new WaveFileAudioInputStream(Settings.WaveFile);
            
            TranslationRecognizer r = s.createTranslationRecognizerWithStream(ais, null, null, null);
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
            WaveFileAudioInputStream ais = new WaveFileAudioInputStream(Settings.WaveFile);
            
            TranslationRecognizer r = s.createTranslationRecognizerWithStream(ais, "illegal", null, null);
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
            WaveFileAudioInputStream ais = new WaveFileAudioInputStream(Settings.WaveFile);
            
            TranslationRecognizer r = s.createTranslationRecognizerWithStream(ais, "en-US", null, null);
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
            
            WaveFileAudioInputStream ais = new WaveFileAudioInputStream(Settings.WaveFile);
            
            TranslationRecognizer r = s.createTranslationRecognizerWithStream(ais, "en-US", targets, null);
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
            
            WaveFileAudioInputStream ais = new WaveFileAudioInputStream(Settings.WaveFile);
            
            TranslationRecognizer r = s.createTranslationRecognizerWithStream(ais, "en-US", targets, null);
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
            
            WaveFileAudioInputStream ais = new WaveFileAudioInputStream(Settings.WaveFile);
            
            TranslationRecognizer r = s.createTranslationRecognizerWithStream(ais, "en-US", targets, null);
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }

        s.close();
    }

    @Ignore // TODO create with stream on illegal voice does not fail?
    @Test
    public void testCreateTranslationRecognizerWithStreamAudioInputStreamStringArrayListOfStringString8() {
        SpeechFactory s = SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            ArrayList<String> targets = new ArrayList<>();
            targets.add("en-US");
            
            WaveFileAudioInputStream ais = new WaveFileAudioInputStream(Settings.WaveFile);
            
            TranslationRecognizer r = s.createTranslationRecognizerWithStream(ais, "en-US", targets, "illegal");
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
        
        WaveFileAudioInputStream ais = new WaveFileAudioInputStream(Settings.WaveFile);
        
        TranslationRecognizer r = s.createTranslationRecognizerWithStream(ais, "en-US", targets, "en-US");
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
