package tests.endtoend;

import static org.junit.Assert.*;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

import tests.Settings;
import tests.endtoend.SampleRecognizeWithIntermediateResults;

public class SampleRecognizeWithIntermediateResultsTest {
    SampleRecognizeWithIntermediateResults test;
    
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
        test = new SampleRecognizeWithIntermediateResults();
        
        test.run();
       
        assertNotNull(test.getSpeechRecognitionResultEventArgs()); // there was at least on intermediate result
        assertNotNull(test.getResult()); // we got the final result
        assertEquals("What's the weather like?", test.getResult().getText()); // we get the text expected
    }
}
