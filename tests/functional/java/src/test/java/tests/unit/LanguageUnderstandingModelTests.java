//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

package tests.unit;

import static org.junit.Assert.*;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.Ignore;
import org.junit.Rule;

import java.net.URI;
import java.util.ArrayList;

import com.microsoft.cognitiveservices.speech.KeywordRecognitionModel;
import com.microsoft.cognitiveservices.speech.Recognizer;
import com.microsoft.cognitiveservices.speech.SpeechConfig;
import com.microsoft.cognitiveservices.speech.SpeechRecognizer;
import com.microsoft.cognitiveservices.speech.intent.IntentRecognizer;
import com.microsoft.cognitiveservices.speech.intent.LanguageUnderstandingModel;
import com.microsoft.cognitiveservices.speech.translation.TranslationRecognizer;

import tests.BaseJUnit;
import tests.DefaultSettingsKeys;
import tests.Settings;
import tests.SubscriptionsRegionsKeys;
import tests.Retry;

@SuppressWarnings("unused")
public class LanguageUnderstandingModelTests extends BaseJUnit {

    @Rule
    public Retry retry = new Retry(Settings.TestRetryCount);

    @BeforeClass
    static public void setUpBeforeClass() throws Exception {
        Settings.LoadSettings();
    }

    // -----------------------------------------------------------------------
    // ---
    // -----------------------------------------------------------------------

    @Test
    public void testFromEndpoint() {
        LanguageUnderstandingModel s = LanguageUnderstandingModel.fromAppId(Settings.DefaultSettingsMap.get(DefaultSettingsKeys.LANGUAGE_UNDERSTANDING_HOME_AUTOMATION_APP_ID));
        assertNotNull(s);
    }

    // -----------------------------------------------------------------------
    // ---
    // -----------------------------------------------------------------------

    @Test
    public void testFromAppId() {
        LanguageUnderstandingModel s = LanguageUnderstandingModel.fromAppId(Settings.DefaultSettingsMap.get(DefaultSettingsKeys.LANGUAGE_UNDERSTANDING_HOME_AUTOMATION_APP_ID));
        assertNotNull(s);
    }

    // -----------------------------------------------------------------------
    // ---
    // -----------------------------------------------------------------------

    @Test
    public void testFromSubscription() {
        LanguageUnderstandingModel s = LanguageUnderstandingModel.fromSubscription(Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.LANGUAGE_UNDERSTANDING_SUBSCRIPTION).Key, 
            Settings.DefaultSettingsMap.get(DefaultSettingsKeys.LANGUAGE_UNDERSTANDING_HOME_AUTOMATION_APP_ID),
            Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.LANGUAGE_UNDERSTANDING_SUBSCRIPTION).Region);

        assertNotNull(s);
    }

    // -----------------------------------------------------------------------
    // ---
    // -----------------------------------------------------------------------

    @Ignore("TODO implement")
    @Test
    public void testLanguageUnderstandingModel() {
        fail("Not yet implemented");
    }

    // -----------------------------------------------------------------------
    // ---
    // -----------------------------------------------------------------------

    @Test
    public void testGetModelImpl1() {
        LanguageUnderstandingModel s = LanguageUnderstandingModel.fromAppId(Settings.DefaultSettingsMap.get(DefaultSettingsKeys.LANGUAGE_UNDERSTANDING_HOME_AUTOMATION_APP_ID));

        assertNotNull(s);
        assertNotNull(s.getImpl());
    }

    @Test
    public void testGetModelImpl2() {
        LanguageUnderstandingModel s = LanguageUnderstandingModel.fromEndpoint("https://www.example.com/api/v1");

        assertNotNull(s);
        assertNotNull(s.getImpl());
    }

    @Test
    public void testGetModelImpl3() {
        LanguageUnderstandingModel s = LanguageUnderstandingModel.fromSubscription(Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.LANGUAGE_UNDERSTANDING_SUBSCRIPTION).Key,
            Settings.DefaultSettingsMap.get(DefaultSettingsKeys.LANGUAGE_UNDERSTANDING_HOME_AUTOMATION_APP_ID),
            Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.LANGUAGE_UNDERSTANDING_SUBSCRIPTION).Region);

        assertNotNull(s);
        assertNotNull(s.getImpl());
    }

}
