package tests.unit;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import static org.junit.Assert.*;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.Ignore;

import java.io.File;
import java.net.URI;
import java.util.ArrayList;

import com.microsoft.cognitiveservices.speech.audio.AudioConfig;
import com.microsoft.cognitiveservices.speech.audio.AudioInputStream;
import com.microsoft.cognitiveservices.speech.audio.PullAudioInputStreamCallback;
import com.microsoft.cognitiveservices.speech.PropertyId;
import com.microsoft.cognitiveservices.speech.Recognizer;
import com.microsoft.cognitiveservices.speech.SpeechConfig;
import com.microsoft.cognitiveservices.speech.SpeechRecognizer;
import com.microsoft.cognitiveservices.speech.intent.IntentRecognizer;
import com.microsoft.cognitiveservices.speech.translation.TranslationRecognizer;
import com.microsoft.cognitiveservices.speech.translation.SpeechTranslationConfig;
                                                          

import tests.Settings;

@SuppressWarnings({"unused", "resource"})
public class SpeechConfigTests {

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
        SpeechConfig s = SpeechConfig.fromSubscription(null, null);
    }

    @Test(expected = NullPointerException.class)
    public void testFromSubscription2() {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, null);
    }

    @Test(expected = NullPointerException.class)
    public void testFromSubscription3() {
        SpeechConfig s = SpeechConfig.fromSubscription(null, Settings.SpeechRegion);
    }

    @Ignore("TODO why does illegal key succeed?")
    @Test(expected = NullPointerException.class)
    public void testFromSubscription4() {
        SpeechConfig s = SpeechConfig.fromSubscription("illegal", "illegal");
    }
    
    @Test() 
    public void testFromSubscriptionSuccess() {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        assertNotNull(s);
        
        s.close();
    }

    @Test
    public void testFromAuthorizationToken() {
        SpeechConfig s = SpeechConfig.fromAuthorizationToken(Settings.SpeechAuthorizationToken, Settings.SpeechRegion);
        assertNotNull(s);
        s.close();
    }

    @Test
    public void testGetAuthorizationToken() {
        String token1 = "token1";
        SpeechConfig s = SpeechConfig.fromAuthorizationToken(token1, Settings.SpeechRegion);
        assertNotNull(s);
        String actualToken = s.getAuthorizationToken();
        assertEquals(token1, actualToken);
        s.close();
    }

    @Test
    public void testSetAuthorizationToken() {
        String token1 = "token1";
        String token2 = "token2";
        SpeechConfig s = SpeechConfig.fromAuthorizationToken(token1, Settings.SpeechRegion);
        assertNotNull(s);
        String actualToken = s.getAuthorizationToken();
        assertEquals(token1, actualToken);
        s.setAuthorizationToken(token2);
        actualToken = s.getAuthorizationToken();
        assertEquals(token2, actualToken);
        s.close();
    }

    // -----------------------------------------------------------------------
    // --- 
    // -----------------------------------------------------------------------
    
    @Test(expected = NullPointerException.class)
    public void testFromEndpoint1() {
        SpeechConfig s = SpeechConfig.fromEndpoint(null, null);
    }

    @Test(expected = NullPointerException.class)
    public void testFromEndpoint2() {
        SpeechConfig s = SpeechConfig.fromEndpoint(null, Settings.SpeechRegion);
    }

    @Test(expected = NullPointerException.class)
    public void testFromEndpoint3() {
        SpeechConfig s = SpeechConfig.fromEndpoint(URI.create("http://www.example.com"), null);
    }

    @Ignore("TODO why does illegal token not fail?")
    @Test(expected = RuntimeException.class)
    public void testFromEndpoint4() {
        SpeechConfig s = SpeechConfig.fromEndpoint(URI.create("http://www.example.com"), "illegal-subscription");
    }
    
    @Test
    public void testFromEndpointSuccess() {
        SpeechConfig s = SpeechConfig.fromEndpoint(URI.create("http://www.example.com"), Settings.SpeechSubscriptionKey);
        
        assertNotNull(s);
        
        s.close();
    }

    // -----------------------------------------------------------------------
    // --- 
    // -----------------------------------------------------------------------

    @Ignore("TODO: enable. however, this will crash the java vm at shutdown due to COM issues.")
    @Test
    public void testCreateSpeechRecognizer() {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        
        SpeechRecognizer r = new SpeechRecognizer(s, null);
        
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
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            SpeechRecognizer r = new SpeechRecognizer(s, null);
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
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            SpeechRecognizer r = new SpeechRecognizer(s, null);
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
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        s.setSpeechRecognitionLanguage("en-US");

        SpeechRecognizer r = new SpeechRecognizer(s, null);
        
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
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromWavFileInput((String)null));            
            fail("not expected");
        }
        catch(NullPointerException ex) {
            // expected
        }
        
        s.close();
    }

    @Test
    public void testCreateSpeechRecognizerString2() {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromWavFileInput("illegal-" + Settings.WavFile));            
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }

    @Test
    public void testCreateSpeechRecognizerStringSuccess() {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));
        
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);
        
        r.close();
        s.close();
    }

    @Test
    public void testCreateSpeechRecognizerStringString1() {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            SpeechRecognizer r = new SpeechRecognizer(s,AudioConfig.fromWavFileInput((String)null));
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }

    @Test
    public void testCreateSpeechRecognizerStringString2() {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        s.setSpeechRecognitionLanguage("en-EN");
        try {
            SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromWavFileInput((String)null));
            
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
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        s.setSpeechRecognitionLanguage("illegal-language");

        try {
            SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));

            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }

    @Test
    public void testCreateSpeechRecognizerStringStringSuccess() {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        s.setSpeechRecognitionLanguage("en-EN");

        SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));
        
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
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
 
        try {
            SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromStreamInput((AudioInputStream)null));
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }

    @Test
    public void testCreateSpeechRecognizerAudioInputStreamSuccess() {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.WavFile);
        SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromStreamInput(ais));
        
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);

        s.close();
    }
    
    @Test
    public void testCreateSpeechRecognizerAudioInputStreamString1() {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromStreamInput((AudioInputStream)null));

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
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        s.setSpeechRecognitionLanguage("en-US");

        try {
            SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromStreamInput((AudioInputStream)null));
            
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }
    
    @Test
    @Ignore("TODO: Check, now we can pass language as part of config, and it can be null")
    public void testCreateSpeechRecognizerAudioInputStreamString3() {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.WavFile);

            SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromStreamInput(ais));
            
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
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        s.setSpeechRecognitionLanguage("illegal-language");

        try {
            WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.WavFile);
            SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromStreamInput(ais));
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }

    @Test
    public void testCreateSpeechRecognizerAudioInputStreamStringSuccess() {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        s.setSpeechRecognitionLanguage("en-US");

        WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.WavFile);
        SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromStreamInput(ais));

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
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        IntentRecognizer r = new IntentRecognizer(s, null);

        s.close();
    }

    @Ignore("TODO: enable. however, this will crash the java vm at shutdown due to COM issues.")
    @Test
    public void testCreateIntentRecognizerLanguage1() {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            IntentRecognizer r = new IntentRecognizer(s, AudioConfig.fromStreamInput((AudioInputStream)null));
            
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
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        s.setSpeechRecognitionLanguage("illegal-language");

        try {
            IntentRecognizer r = new IntentRecognizer(s, (AudioConfig)null);
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
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        s.setSpeechRecognitionLanguage("en-US");

        IntentRecognizer r = new IntentRecognizer(s, (AudioConfig)null);
        
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);

        r.close();
        s.close();
    }

    @Test
    public void testCreateIntentRecognizerString1() {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            IntentRecognizer r = new IntentRecognizer(s, AudioConfig.fromWavFileInput((String)null));
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }

        s.close();
    }
    
    @Test
    public void testCreateIntentRecognizerString2() {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            IntentRecognizer r = new IntentRecognizer(s, AudioConfig.fromWavFileInput("illegal-" + Settings.WavFile));
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }

        s.close();
    }
       
    @Test
    public void testCreateIntentRecognizerStringSuccess() {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        IntentRecognizer r = new IntentRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));      
        
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);

        r.close();
        s.close();
    }
   

    @Ignore("TODO why can create with null stream?")
    @Test
    public void testCreateIntentRecognizerAudioInputStream1() {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            IntentRecognizer r = new IntentRecognizer(s, AudioConfig.fromStreamInput((AudioInputStream)null));
            
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }

    @Test
    public void testCreateIntentRecognizerAudioInputStreamSuccess() {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.WavFile);
        IntentRecognizer r = new IntentRecognizer(s, AudioConfig.fromStreamInput(ais));

        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);

        s.close();
    }

    @Test
    public void testCreateIntentRecognizerAudioInputStreamString1() {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            IntentRecognizer r = new IntentRecognizer(s, AudioConfig.fromStreamInput((AudioInputStream)null));
            
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
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        s.setSpeechRecognitionLanguage("en-US");

        try {
            IntentRecognizer r = new IntentRecognizer(s, AudioConfig.fromStreamInput((AudioInputStream)null));            
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
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        s.setSpeechRecognitionLanguage("illegal-language");

        try {
            WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.WavFile);
            IntentRecognizer r = new IntentRecognizer(s, AudioConfig.fromStreamInput(ais));

            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }

    @Test
    public void testCreateIntentRecognizerAudioInputStreamStringSuccess() {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        s.setSpeechRecognitionLanguage("en-US");

        WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.WavFile);
        IntentRecognizer r = new IntentRecognizer(s, AudioConfig.fromStreamInput(ais));
        
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
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            TranslationRecognizer r = new TranslationRecognizer(s, null);
        
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
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        s.setSpeechRecognitionLanguage("illegal");

        try {
            TranslationRecognizer r = new TranslationRecognizer(s, null);
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
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        s.setSpeechRecognitionLanguage("en-EN");

        try {
            TranslationRecognizer r = new TranslationRecognizer(s, null);
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
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        s.setSpeechRecognitionLanguage("en-EN");
        s.addTargetLanguage("");

        try {

            TranslationRecognizer r = new TranslationRecognizer(s,null);
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
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        s.addTargetLanguage("illegal");

        try {
        
            TranslationRecognizer r = new TranslationRecognizer(s, null);
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
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        s.addTargetLanguage("en-US");
        s.setSpeechRecognitionLanguage("en-EN");

        TranslationRecognizer r = new TranslationRecognizer(s, null);
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
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            TranslationRecognizer r = new TranslationRecognizer(null,null);
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
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        s.setSpeechRecognitionLanguage("illegal");

        try {
            TranslationRecognizer r = new TranslationRecognizer(s, null);
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
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        s.setSpeechRecognitionLanguage("en-US");

        try {
            TranslationRecognizer r = new TranslationRecognizer(s, null);
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
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        s.setSpeechRecognitionLanguage("en-US");

        try {
            ArrayList<String> targets = new ArrayList<>();

            TranslationRecognizer r = new TranslationRecognizer(s,null);
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
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        s.setSpeechRecognitionLanguage("en-US");
        s.addTargetLanguage("illegal");

        try {
            TranslationRecognizer r = new TranslationRecognizer(s, null);
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
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        s.setSpeechRecognitionLanguage("en-US");
        s.addTargetLanguage("en-US");

        try {
            ArrayList<String> targets = new ArrayList<>();
            targets.add("en-US");

            TranslationRecognizer r = new TranslationRecognizer(s, null);
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
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        s.setSpeechRecognitionLanguage("en-US");
        s.addTargetLanguage("en-US");
        s.setVoiceName("illegal");

        try {
            ArrayList<String> targets = new ArrayList<>();
            targets.add("en-US");

            TranslationRecognizer r = new TranslationRecognizer(s, null);
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
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        s.setSpeechRecognitionLanguage("en-US");
        s.addTargetLanguage("en-US");
        s.setVoiceName("en-US");

        TranslationRecognizer r = new TranslationRecognizer(s, null);
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
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            TranslationRecognizer r = new TranslationRecognizer(null,null);
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }

    @Test
    public void testCreateTranslationRecognizerWithFileInputStringStringArrayListOfString2() {
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            TranslationRecognizer r = new TranslationRecognizer(s, AudioConfig.fromWavFileInput("illegal-" + Settings.WavFile));
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }

    @Test
    public void testCreateTranslationRecognizerWithFileInputStringStringArrayListOfString3() {
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            TranslationRecognizer r = new TranslationRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }
    
    @Test
    public void testCreateTranslationRecognizerWithFileInputStringStringArrayListOfString4() {
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        s.setSpeechRecognitionLanguage("illegal");
        
        try {
            TranslationRecognizer r = new TranslationRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }
    
    @Test
    public void testCreateTranslationRecognizerWithFileInputStringStringArrayListOfString5() {
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        s.setSpeechRecognitionLanguage("en-US");
        
        try {
            TranslationRecognizer r = new TranslationRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }
    
    @Test
    public void testCreateTranslationRecognizerWithFileInputStringStringArrayListOfString6() {
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        s.setSpeechRecognitionLanguage("en-US");
        
        try {
            ArrayList<String> targets = new ArrayList<>();
            
            TranslationRecognizer r = new TranslationRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));
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
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        s.setSpeechRecognitionLanguage("en-US");
        s.addTargetLanguage("illegal");     
        
        try {
            TranslationRecognizer r = new TranslationRecognizer(s,null);
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }

    @Test
    public void testCreateTranslationRecognizerWithFileInputStringStringArrayListOfStringSuccess() {
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        s.setSpeechRecognitionLanguage("en-US");
        s.addTargetLanguage("en-US");
        
        TranslationRecognizer r = new TranslationRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));
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
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            TranslationRecognizer r = new TranslationRecognizer(s, null);
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }
    
    @Test
    public void testCreateTranslationRecognizerWithFileInputStringStringArrayListOfStringString2() {
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            TranslationRecognizer r = new TranslationRecognizer(s, AudioConfig.fromWavFileInput("illegal-" + Settings.WavFile));
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }

    @Test
    public void testCreateTranslationRecognizerWithFileInputStringStringArrayListOfStringString3() {
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            TranslationRecognizer r = new TranslationRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }

    @Test
    public void testCreateTranslationRecognizerWithFileInputStringStringArrayListOfStringString4() {
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        s.setSpeechRecognitionLanguage("illegal");
        
        try {
            TranslationRecognizer r = new TranslationRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }
    
    @Test
    public void testCreateTranslationRecognizerWithFileInputStringStringArrayListOfStringString5() {
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        s.setSpeechRecognitionLanguage("en-US");
        
        try {
            TranslationRecognizer r = new TranslationRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }

    @Test
    public void testCreateTranslationRecognizerWithFileInputStringStringArrayListOfStringString6() {
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        s.setSpeechRecognitionLanguage("en-US");
        
        try {
            TranslationRecognizer r = new TranslationRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }

    @Test
    @Ignore("TODO: Check, now we can pass language as part of config, and it can be null")
    public void testCreateTranslationRecognizerWithFileInputStringStringArrayListOfStringString7() {
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        s.setSpeechRecognitionLanguage("en-US");
        s.addTargetLanguage("illegal");
        
        try {
            
            TranslationRecognizer r = new TranslationRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }

    @Test
    @Ignore("TODO: Check, now we can pass language as part of config, and it can be null")
    public void testCreateTranslationRecognizerWithFileInputStringStringArrayListOfStringString8() {
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        s.setSpeechRecognitionLanguage("en-US");
        s.addTargetLanguage("en-US");
        
        try {
            TranslationRecognizer r = new TranslationRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));
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
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        s.setSpeechRecognitionLanguage("en-US");
        s.addTargetLanguage("en-US");
        s.setVoiceName("illegal");
        
        try {
            TranslationRecognizer r = new TranslationRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }

    @Test
    public void testCreateTranslationRecognizerWithFileInputStringStringArrayListOfStringStringSuccess() {
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        s.setSpeechRecognitionLanguage("en-US");
        s.addTargetLanguage("en-US");
        s.setVoiceName("en-US");
        
        TranslationRecognizer r = new TranslationRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));
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
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            TranslationRecognizer r = new TranslationRecognizer(s, null);
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }

    @Test
    public void testCreateTranslationRecognizerWithStreamAudioInputStreamStringArrayListOfString2() {
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.WavFile);
            
            TranslationRecognizer r = new TranslationRecognizer(s, AudioConfig.fromStreamInput(ais));
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }

    @Test
    public void testCreateTranslationRecognizerWithStreamAudioInputStreamStringArrayListOfString3() {
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        s.setSpeechRecognitionLanguage("illegal");
        
        try {
            WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.WavFile);
            
            TranslationRecognizer r = new TranslationRecognizer(s, AudioConfig.fromStreamInput(ais));
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }
    
    @Test
    public void testCreateTranslationRecognizerWithStreamAudioInputStreamStringArrayListOfString4() {
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        s.setSpeechRecognitionLanguage("en-US");
        
        try {
            WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.WavFile);
            
            TranslationRecognizer r = new TranslationRecognizer(s, AudioConfig.fromStreamInput(ais));
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }

    @Test
    public void testCreateTranslationRecognizerWithStreamAudioInputStreamStringArrayListOfString5() {
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        s.setSpeechRecognitionLanguage("en-US");
        
        try {
            ArrayList<String> targets = new ArrayList<>();
            
            WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.WavFile);
            
            TranslationRecognizer r = new TranslationRecognizer(s, AudioConfig.fromStreamInput(ais));
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
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        s.addTargetLanguage("illegal");
        s.setSpeechRecognitionLanguage("en-US");
        
        try {
            
            WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.WavFile);
            
            TranslationRecognizer r = new TranslationRecognizer(s, AudioConfig.fromStreamInput(ais));
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }
        
        s.close();
    }

    @Test
    public void testCreateTranslationRecognizerWithStreamAudioInputStreamStringArrayListOfStringSuccess() {
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        s.addTargetLanguage("en-US");
        s.setSpeechRecognitionLanguage("en-US");
        
        WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.WavFile);
        
        TranslationRecognizer r = new TranslationRecognizer(s, AudioConfig.fromStreamInput(ais));
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
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
         
        try {
            TranslationRecognizer r = new TranslationRecognizer(s,  null);
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }

        s.close();
    }
    
    @Test
    public void testCreateTranslationRecognizerWithStreamAudioInputStreamStringArrayListOfStringString2() {
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

        try {
            WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.WavFile);
            
            TranslationRecognizer r = new TranslationRecognizer(s, AudioConfig.fromStreamInput(ais));
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }

        s.close();
    }

    @Test
    public void testCreateTranslationRecognizerWithStreamAudioInputStreamStringArrayListOfStringString3() {
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        s.setSpeechRecognitionLanguage("illegal");
        
        try {
            WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.WavFile);
            
            TranslationRecognizer r = new TranslationRecognizer(s, AudioConfig.fromStreamInput(ais));
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }

        s.close();
    }

    @Test
    public void testCreateTranslationRecognizerWithStreamAudioInputStreamStringArrayListOfStringString4() {
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        s.setSpeechRecognitionLanguage("en-US");
        
        try {
            WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.WavFile);
            
            TranslationRecognizer r = new TranslationRecognizer(s,AudioConfig.fromStreamInput(ais));
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }

        s.close();
    }
    
    @Test
    public void testCreateTranslationRecognizerWithStreamAudioInputStreamStringArrayListOfStringString5() {
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        s.setSpeechRecognitionLanguage("en-US");
        
        try {
            ArrayList<String> targets = new ArrayList<>();
            
            WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.WavFile);
            
            TranslationRecognizer r = new TranslationRecognizer(s, AudioConfig.fromStreamInput(ais));
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }

        s.close();
    }

    @Test
    @Ignore("TODO: Check, now we can pass language as part of config, and it can be null")
    public void testCreateTranslationRecognizerWithStreamAudioInputStreamStringArrayListOfStringString6() {
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        s.setSpeechRecognitionLanguage("en-US");
        s.addTargetLanguage("illegal");
        
        try {
            WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.WavFile);
            
            TranslationRecognizer r = new TranslationRecognizer(s, AudioConfig.fromStreamInput(ais));
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }

        s.close();
    }

    @Test
    @Ignore("TODO: Check, now we can pass language as part of config, and it can be null")
    public void testCreateTranslationRecognizerWithStreamAudioInputStreamStringArrayListOfStringString7() {
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        s.setSpeechRecognitionLanguage("en-US");
        s.addTargetLanguage("en-US");
        
        try {
            
            WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.WavFile);
            
            TranslationRecognizer r = new TranslationRecognizer(s, AudioConfig.fromStreamInput(ais));
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
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        s.setSpeechRecognitionLanguage("en-US");
        s.addTargetLanguage("en-US");
        s.setVoiceName("illegal");
        
        try {
            ArrayList<String> targets = new ArrayList<>();
            targets.add("en-US");
            
            WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.WavFile);
            
            TranslationRecognizer r = new TranslationRecognizer(s, AudioConfig.fromStreamInput(ais));
            fail("not expected");
        }
        catch(RuntimeException ex) {
            // expected
        }

        s.close();
    }

    @Test
    public void testCreateTranslationRecognizerWithStreamAudioInputStreamStringArrayListOfStringStringSuccess() {
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        s.setSpeechRecognitionLanguage("en-US");
        s.addTargetLanguage("en-US");
        s.setVoiceName("en-US");
        
        
        WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.WavFile);
        
        TranslationRecognizer r = new TranslationRecognizer(s, AudioConfig.fromStreamInput(ais));
        
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
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        
        s.close();
    }

    // -----------------------------------------------------------------------
    // --- 
    // -----------------------------------------------------------------------

    @Test
    public void testGetConfigImpl() {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        
        assertNotNull(s);
        assertNotNull(s.getImpl());
    }
    
    @Test
    public void testSetProperty() {
        String auth_token = "token";
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        s.setProperty(PropertyId.SpeechServiceAuthorization_Token, auth_token);
        assertEquals(auth_token, s.getProperty(PropertyId.SpeechServiceAuthorization_Token));
        assertEquals(auth_token, s.getAuthorizationToken());
    }

    @Test(expected = NullPointerException.class)
    public void testSetProperty1() {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        s.setProperty(PropertyId.SpeechServiceConnection_Endpoint, null);
    }

    @Test(expected = IllegalArgumentException.class)
    public void testSetProperty2() {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        s.setProperty(PropertyId.SpeechServiceConnection_Endpoint, "");
    }
    
    @Test(expected = IllegalArgumentException.class)
    public void testSetProperty3() {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        s.setProperty(PropertyId.SpeechServiceConnection_Endpoint, " ");
    }

    @Test
    public void testSetLogFilename() {
        SpeechTranslationConfig s = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        s.setSpeechRecognitionLanguage("en-US");
        s.addTargetLanguage("de");
        String logFilename = new java.io.File(System.getProperty("java.io.tmpdir", ""), "test_filename.txt").getAbsolutePath();

        s.setProperty(PropertyId.SpeechServiceLog_Filename, logFilename);
        assertEquals(s.getProperty(PropertyId.SpeechServiceLog_Filename), logFilename);
        TranslationRecognizer r = new TranslationRecognizer(s, AudioConfig.fromWavFileInput(Settings.WavFile));
        assertTrue(new File(logFilename).length() > 0);
    }
}
