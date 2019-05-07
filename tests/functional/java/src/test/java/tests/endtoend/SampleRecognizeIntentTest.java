//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

package tests.endtoend;

import static org.junit.Assert.*;

import org.junit.BeforeClass;
import org.junit.Test;

import com.microsoft.cognitiveservices.speech.PropertyId;

import org.junit.Ignore;

import tests.Settings;
import tests.TestHelper;
import tests.endtoend.SampleRecognizeIntent;

public class SampleRecognizeIntentTest {
    SampleRecognizeIntent test;

    @BeforeClass
    static public void setUpBeforeClass() throws Exception {
        Settings.LoadSettings();
    }

    @Ignore("TODO why does that not return the expected result")
    @Test
    public void test() {
        test = new SampleRecognizeIntent();

        test.run();
        TestHelper.AssertConnectionCountMatching(test.getConnectedEventCount(), test.getDisconnectedEventCount());
        assertNotNull(test.getResult()); // we got the final result
        assertEquals("What's the weather like?", test.getResult().getText());

        String s = test.getResult().getProperties().getProperty(PropertyId.LanguageUnderstandingServiceResponse_JsonResult);
        assertNotNull(s);
        assertTrue(s.length() > 0);
        assertTrue(s.indexOf("\"topScoringIntent\":") > 0);
        assertTrue(s.indexOf("\"score\":") > 0);
        assertTrue(s.indexOf("\"entities\":") > 0);
    }
}
