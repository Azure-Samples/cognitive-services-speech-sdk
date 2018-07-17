package tests.endtoend;
//
//Copyright (c) Microsoft. All rights reserved.
//Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import static org.junit.Assert.*;

import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.Ignore;

import tests.Settings;
import tests.endtoend.SampleRecognizeIntent;

public class SampleRecognizeIntentTest {
    SampleRecognizeIntent test;

    @BeforeClass
    static public void setUpBeforeClass() throws Exception {
        // Override inputs, if necessary
        Settings.LoadSettings();
    }

    @Ignore("TODO why does that not return the expected result")
    @Test
    public void test() {
        test = new SampleRecognizeIntent();
        
        test.run();
       
        assertNotNull(test.getResult()); // we got the final result
        assertEquals("What's the weather like?", test.getResult().getText());
        
        String s = test.getResult().getLanguageUnderstanding();
        assertNotNull(s);
        assertTrue(s.length() > 0);
        assertTrue(s.indexOf("\"topScoringIntent\":") > 0);
        assertTrue(s.indexOf("\"score\":") > 0);
        assertTrue(s.indexOf("\"entities\":") > 0);
    }
}
