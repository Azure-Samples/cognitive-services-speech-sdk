package tests.endtoend;

import static org.junit.Assert.*;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

import tests.Settings;
import tests.endtoend.SampleRecognizeIntent;

public class SampleRecognizeIntentTest {
    SampleRecognizeIntent test;

    @BeforeClass
    static public void setUpBeforeClass() throws Exception {
        // Override inputs, if necessary
        Settings.LoadSettings();
    }

    @AfterClass
    static public void tearDownAfterClass() throws Exception {
    }

    @Before
    public void setUp() throws Exception {
    }

    @After
    public void tearDown() throws Exception {
    }

    @Test
    public void test() {
        test = new SampleRecognizeIntent();
        
        test.run();
       
        assertNotNull(test.getResult()); // we got the final result
        assertEquals("What's the weather like?", test.getResult().getRecognizedText());
        
        String s = test.getResult().getLanguageUnderstanding();
        assertNotNull(s);
        assertTrue(s.length() > 0);
        assertTrue(s.indexOf("\"topScoringIntent\":") > 0);
        assertTrue(s.indexOf("\"score\":") > 0);
        assertTrue(s.indexOf("\"entities\":") > 0);
    }
}
