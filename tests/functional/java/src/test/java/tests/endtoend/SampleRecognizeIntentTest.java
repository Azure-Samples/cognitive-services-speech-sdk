//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

package tests.endtoend;

import static org.junit.Assert.*;

import org.junit.BeforeClass;
import org.junit.Test;

import com.microsoft.cognitiveservices.speech.PropertyId;

import tests.BaseJUnit;
import tests.AudioUtterancesKeys;
import tests.Settings;
import tests.endtoend.SampleRecognizeIntent;

public class SampleRecognizeIntentTest extends BaseJUnit {
    SampleRecognizeIntent test;

    @BeforeClass
    static public void setUpBeforeClass() throws Exception {
        Settings.LoadSettings();
    }

    @Test
    public void test() {
        test = new SampleRecognizeIntent();

        test.run();
        assertNotNull(test.getResult()); // we got the final result
        assertEquals(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.INTENT_UTTERANCE).Utterances.get("en-US")[0].Text, test.getResult().getText());
        assertEquals("1", test.getIntentId());
        assertEquals("HomeAutomation.TurnOn", test.getIntentName());

        String s = test.getResult().getProperties().getProperty(PropertyId.LanguageUnderstandingServiceResponse_JsonResult);
        System.out.println("LUIS Json result:" + s);
        assertTrue(s.length() > 0);
        assertTrue(s.indexOf("\"topScoringIntent\":") > 0);
        assertTrue(s.indexOf("\"score\":") > 0);
        assertTrue(s.indexOf("\"entities\":") > 0);
    }
}
