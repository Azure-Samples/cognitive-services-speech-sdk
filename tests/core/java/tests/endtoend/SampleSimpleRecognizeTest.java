package tests.endtoend;

import static org.junit.Assert.*;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

import tests.Settings;
import tests.endtoend.SampleSimpleRecognize;

public class SampleSimpleRecognizeTest {
    SampleSimpleRecognize test;
    
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
        test = new SampleSimpleRecognize();
        
        test.run();
        
        assertEquals("What's the weather like?", test.getResult());
    }

}
