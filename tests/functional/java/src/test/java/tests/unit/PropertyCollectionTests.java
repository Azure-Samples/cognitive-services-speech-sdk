//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

package tests.unit;

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
import com.microsoft.cognitiveservices.speech.translation.SpeechTranslationConfig;
import com.microsoft.cognitiveservices.speech.PropertyId;

import tests.AudioUtterancesKeys;
import tests.Settings;
import tests.SubscriptionsRegionsKeys;

@SuppressWarnings("unused")
public class PropertyCollectionTests {

    @BeforeClass
    public static void setUpBeforeClass() throws Exception {
        Settings.LoadSettings();
    }

    // -----------------------------------------------------------------------
    // ---
    // -----------------------------------------------------------------------

    @SuppressWarnings("resource")
    @Test
    public void testPropertyCollection() {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Key,
            Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Region);

        assertNotNull(s);

        PropertyCollection crp  = new IntentRecognizer(s, AudioConfig.fromWavFileInput(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath))).getProperties();
        assertNotNull(crp);

        PropertyCollection srp  = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath))).getProperties();
        assertNotNull(srp);

        SpeechTranslationConfig st = SpeechTranslationConfig.fromSubscription(Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Key,
            Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Region);

        st.setSpeechRecognitionLanguage("en-US");
        st.addTargetLanguage("en-US");
        PropertyCollection trp  = new TranslationRecognizer(st, AudioConfig.fromWavFileInput(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath))).getProperties();
        assertNotNull(crp);
    }

    // -----------------------------------------------------------------------
    // ---
    // -----------------------------------------------------------------------

    @SuppressWarnings("resource")
    @Test
    public void testPropertyCollectionSetpropertyId() {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Key,
            Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Region);

        assertNotNull(s);

        PropertyCollection crp  = new IntentRecognizer(s, AudioConfig.fromWavFileInput(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath))).getProperties();
        assertNotNull(crp);

        String value = "seattle";
        crp.setProperty(PropertyId.SpeechServiceConnection_IntentRegion, value);
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

    @SuppressWarnings("resource")
    @Test
    public void testIsString() {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Key,
            Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Region);

        assertNotNull(s);

        PropertyCollection crp  = new IntentRecognizer(s, AudioConfig.fromWavFileInput(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath))).getProperties();
        assertNotNull(crp);
        testIsParametersString(crp);

        PropertyCollection srp  = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath))).getProperties();
        assertNotNull(srp);
        testIsParametersString(srp);

        SpeechTranslationConfig st = SpeechTranslationConfig.fromSubscription(Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Key,
            Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Region);

        st.setSpeechRecognitionLanguage("en-US");
        st.addTargetLanguage("en-US");
        PropertyCollection trp  = new TranslationRecognizer(st, AudioConfig.fromWavFileInput(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath))).getProperties();
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

    @SuppressWarnings("resource")
    @Test
    public void testGetSetStringString() {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Key,
            Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Region);

        assertNotNull(s);

        PropertyCollection crp  = new IntentRecognizer(s, AudioConfig.fromWavFileInput(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath))).getProperties();
        assertNotNull(crp);
        testIsParametersString(crp);

        PropertyCollection srp  = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath))).getProperties();
        assertNotNull(srp);
        testIsParametersString(srp);

        SpeechTranslationConfig st = SpeechTranslationConfig.fromSubscription(Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Key,
            Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Region);

        st.setSpeechRecognitionLanguage("en-US");
        st.addTargetLanguage("en-US");
        PropertyCollection trp  = new TranslationRecognizer(st, AudioConfig.fromWavFileInput(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath))).getProperties();
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

    @SuppressWarnings("resource")
    @Ignore("TODO string string tests fail")
    @Test
    public void testGetStringStringString() {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Key,
            Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Region);

        assertNotNull(s);

        PropertyCollection crp  = new IntentRecognizer(s, AudioConfig.fromWavFileInput(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath))).getProperties();
        assertNotNull(crp);
        testGetParametersStringString(crp);

        PropertyCollection srp  = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath))).getProperties();
        assertNotNull(srp);
        testGetParametersStringString(srp);

        SpeechTranslationConfig st = SpeechTranslationConfig.fromSubscription(Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Key,
            Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Region);

        st.setSpeechRecognitionLanguage("en-US");
        st.addTargetLanguage("en-US");
        PropertyCollection trp  = new TranslationRecognizer(st, AudioConfig.fromWavFileInput(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath))).getProperties();
        assertNotNull(trp);
        testGetParametersStringString(trp);
    }

    // -----------------------------------------------------------------------
    // ---
    // -----------------------------------------------------------------------

    @SuppressWarnings("resource")
    @Test
    public void testClose() {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Key,
            Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Region);

        assertNotNull(s);

        PropertyCollection crp  = new IntentRecognizer(s, AudioConfig.fromWavFileInput(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath))).getProperties();
        assertNotNull(crp);

        PropertyCollection srp  = new SpeechRecognizer(s, AudioConfig.fromWavFileInput(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath))).getProperties();
        assertNotNull(srp);

        SpeechTranslationConfig st = SpeechTranslationConfig.fromSubscription(Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Key,
            Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Region);

        st.setSpeechRecognitionLanguage("en-US");
        st.addTargetLanguage("en-US");
        PropertyCollection trp  = new TranslationRecognizer(st, AudioConfig.fromWavFileInput(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath))).getProperties();
        assertNotNull(trp);

        s.close();
    }

}
