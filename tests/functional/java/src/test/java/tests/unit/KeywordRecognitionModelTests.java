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

@SuppressWarnings("unused")
public class KeywordRecognitionModelTests {

    @BeforeClass
    static public void setUpBeforeClass() throws Exception {
        Settings.LoadSettings();
    }

    // -----------------------------------------------------------------------
    // ---
    // -----------------------------------------------------------------------

    @Ignore("TODO: requires android/ARM")
    @Test
    public void testFromFile() {
        KeywordRecognitionModel s = KeywordRecognitionModel.fromFile(Settings.KeywordModel);
        assertNotNull(s);
    }

    // -----------------------------------------------------------------------
    // ---
    // -----------------------------------------------------------------------

    @Ignore("TODO: requires android/ARM")
    @Test
    public void testClose() {
        KeywordRecognitionModel s = KeywordRecognitionModel.fromFile(Settings.KeywordModel);
        assertNotNull(s);

        s.close();

        assertNull(s.getImpl());
    }


    // -----------------------------------------------------------------------
    // ---
    // -----------------------------------------------------------------------

    @Ignore("TODO: requires android/ARM")
    @Test
    public void testGetmodelImpl() {
        KeywordRecognitionModel s = KeywordRecognitionModel.fromFile(Settings.KeywordModel);

        assertNotNull(s);
        assertNotNull(s.getImpl());
    }

}
