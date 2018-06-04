package tests.endtoend;


import java.util.Random;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import static org.junit.Assert.*;

import tests.Settings;
import com.microsoft.cognitiveservices.speech.AudioInputStreamFormat;

import tests.unit.WaveFileAudioInputStream;

@SuppressWarnings("unused")
public class WaveFileAudioInputStreamTest {
    WaveFileAudioInputStream _stream;
    
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
    public void testCreateWithNullFilename() {
        try {
            new WaveFileAudioInputStream(null);
            fail("Not expected");
        }
        catch(Exception ex) {
            // expected
        }
    }
    
    @Test
    public void testCreateWithEmptyFilename() {
        try {
            WaveFileAudioInputStream s = new WaveFileAudioInputStream("");
            fail("unexpected");
        }
        catch(Exception ex) {
            // expected
        }
    }

    @Test
    public void testCreateWithIllegalFilename() {
        try {
            new WaveFileAudioInputStream("illegal-filename-" + (new Random().nextInt()));
            fail("Not expected");
        }
        catch(Exception ex) {
            // expected
        }
    }

    @Test
    public void testCreateWithLegalFilename() {
        new WaveFileAudioInputStream(Settings.WaveFile);
    }

    @Test
    public void testTestFormatOverload() {
        WaveFileAudioInputStream s = new WaveFileAudioInputStream(Settings.WaveFile);
        assertNotNull(s);
        
        AudioInputStreamFormat f1 = s.getFormat();
        assertNotNull(f1);

        long len = s.getFormat(null, 0);
        assertEquals(24, len);

        len = s.getFormat(null, 555);
        assertEquals(24, len);
    }

    @Test
    public void testTestFormatParameters() {
        WaveFileAudioInputStream s = new WaveFileAudioInputStream(Settings.WaveFile);
        assertNotNull(s);
        
        AudioInputStreamFormat f1 = s.getFormat();
        assertNotNull(f1);
        assertEquals(1, f1.Channels);
        assertEquals(16,  f1.BitsPerSample);
        assertEquals(2, f1.BlockAlign);
        assertEquals(16000, f1.SamplesPerSec);
        assertEquals(32000, f1.AvgBytesPerSec);
        assertEquals(1, f1.FormatTag); // pcm
    }


    @Test
    public void testTestClose1() {
        WaveFileAudioInputStream s = new WaveFileAudioInputStream(Settings.WaveFile);
        assertNotNull(s);

        s.close();
    }

    @Test
    public void testTestClose2() {
        WaveFileAudioInputStream s = new WaveFileAudioInputStream(Settings.WaveFile);
        assertNotNull(s);

        s.close();
        
        try {
            s.close();
            fail("not expected");
        }
        catch(IllegalAccessError ex) {
            // expected
        }
    }

    @Test
    public void testTestRead1() {
        WaveFileAudioInputStream s = new WaveFileAudioInputStream(Settings.WaveFile);
        assertNotNull(s);

        try {
            s.read(null);
            fail("not expected");
        }
        catch(NullPointerException ex) {
            // expected
        }
    }

    @Test
    public void testTestRead2() {
        WaveFileAudioInputStream s = new WaveFileAudioInputStream(Settings.WaveFile);
        assertNotNull(s);

        byte[]buf = new byte[0];
        long numread = s.read(buf);
        assertEquals(0, numread);
    }

    @Test
    public void testTestRead3() {
        WaveFileAudioInputStream s = new WaveFileAudioInputStream(Settings.WaveFile);
        assertNotNull(s);

        byte[]buf = new byte[16];
        long numread = s.read(buf);
        assertEquals(16, numread);
    }

    @Test
    public void testTestRead4() {
        WaveFileAudioInputStream s = new WaveFileAudioInputStream(Settings.WaveFile);
        assertNotNull(s);

        byte[]buf = new byte[15];
        long numread = s.read(buf);
        assertTrue(numread > 0); // must read at least one sample since file is not empty 
        assertTrue(numread <= buf.length); // might not read full number of bytes!
    }

    @Test
    public void testTestRead5() {
        WaveFileAudioInputStream s = new WaveFileAudioInputStream(Settings.WaveFile);
        assertNotNull(s);

        // can read before close
        byte[]buf = new byte[2];
        long numread = s.read(buf);
        assertTrue(numread == 2);

        // close underlying stream
        s.close();

        // no read after close
        try {
            numread = s.read(buf);
            fail("not expected");
        }
        catch(IllegalAccessError ex) {
            // expected
        }
    }
}
