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


import java.net.URI;
import java.util.ArrayList;

import com.microsoft.cognitiveservices.speech.KeywordRecognitionModel;
import com.microsoft.cognitiveservices.speech.Recognizer;
import com.microsoft.cognitiveservices.speech.SpeechConfig;
import com.microsoft.cognitiveservices.speech.SpeechRecognizer;
import com.microsoft.cognitiveservices.speech.intent.IntentRecognizer;
import com.microsoft.cognitiveservices.speech.translation.TranslationRecognizer;

import tests.Settings;
import tests.BaseJUnit;
import tests.DefaultSettingsKeys;

@SuppressWarnings("unused")
public class KeywordRecognitionModelTests extends BaseJUnit {

    @BeforeClass
    static public void setUpBeforeClass() throws Exception {
        String operatingSystem = ("" + System.getProperty("os.name")).toLowerCase();
        System.out.println("Current operation system: " + operatingSystem);
        boolean isMac = operatingSystem.contains("mac") || operatingSystem.contains("darwin");
        org.junit.Assume.assumeFalse(isMac);
        Settings.LoadSettings();
    }

    // -----------------------------------------------------------------------
    // ---
    // -----------------------------------------------------------------------

    @Test
    public void testFromFile() {
        KeywordRecognitionModel s = KeywordRecognitionModel.fromFile(Settings.GetRootRelativePath(Settings.DefaultSettingsMap.get(DefaultSettingsKeys.KEYWORD_MODEL)));
        assertNotNull(s);
    }

    // -----------------------------------------------------------------------
    // ---
    // -----------------------------------------------------------------------

    @Test
    public void testClose() {
        KeywordRecognitionModel s = KeywordRecognitionModel.fromFile(Settings.GetRootRelativePath(Settings.DefaultSettingsMap.get(DefaultSettingsKeys.KEYWORD_MODEL)));
        assertNotNull(s);

        s.close();

        assertNull(s.getImpl());
    }


    // -----------------------------------------------------------------------
    // ---
    // -----------------------------------------------------------------------

    @Test
    public void testGetmodelImpl() {
        KeywordRecognitionModel s = KeywordRecognitionModel.fromFile(Settings.GetRootRelativePath(Settings.DefaultSettingsMap.get(DefaultSettingsKeys.KEYWORD_MODEL)));

        assertNotNull(s);
        assertNotNull(s.getImpl());
    }

}
