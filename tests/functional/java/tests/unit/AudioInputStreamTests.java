package tests.unit;
//
//Copyright (c) Microsoft. All rights reserved.
//Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import static org.junit.Assert.*;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.Ignore;

import com.microsoft.cognitiveservices.speech.AudioInputStream;
import com.microsoft.cognitiveservices.speech.AudioInputStreamFormat;

import tests.Settings;

public class AudioInputStreamTests {
    @BeforeClass
    public static void setUpBeforeClass() throws Exception {
        // Override inputs, if necessary
        Settings.LoadSettings();
    }

    // -----------------------------------------------------------------------
    // --- 
    // -----------------------------------------------------------------------

    @Test
    public void testGetFormatAudioInputStreamFormatInt() {
        WaveFileAudioInputStream s = new WaveFileAudioInputStream(Settings.WaveFile);
        assertNotNull(s);
        
        long r = s.getFormat(null, 0);
        assertTrue(r > 0);
        assertTrue(r == 24);

        r = s.getFormat(null, 128);
        assertTrue(r > 0);
        assertTrue(r == 24);
        
        com.microsoft.cognitiveservices.speech.internal.AudioInputStreamFormat f = new com.microsoft.cognitiveservices.speech.internal.AudioInputStreamFormat();
        r = s.getFormat(f, 0);
        assertTrue(r > 0);
        assertTrue(r == 24);

        r = s.getFormat(f, 128);
        assertTrue(r > 0);
        assertTrue(r == 24);
        
        // check known parameters
        assertEquals(32000, f.getAvgBytesPerSec());
        assertEquals(2, f.getBlockAlign());
        assertEquals(1, f.getChannels());
        assertEquals(16000, f.getSamplesPerSec());
        assertEquals(16, f.getBitsPerSample());
        assertEquals(1, f.getFormatTag());
        
        s.close();
    }

    // -----------------------------------------------------------------------
    // --- 
    // -----------------------------------------------------------------------

    @Test
    public void testGetFormat() {
        WaveFileAudioInputStream s = new WaveFileAudioInputStream(Settings.WaveFile);
        assertNotNull(s);

        AudioInputStreamFormat f = s.getFormat();
        assertNotNull(f);

        // check known parameters
        assertEquals(32000, f.AvgBytesPerSec);
        assertEquals(2, f.BlockAlign);
        assertEquals(1, f.Channels);
        assertEquals(16000, f.SamplesPerSec);
        assertEquals(16, f.BitsPerSample);
        assertEquals(1, f.FormatTag);

        s.close();
    }

    // -----------------------------------------------------------------------
    // --- 
    // -----------------------------------------------------------------------

    @Test
    public void testRead1() {
        WaveFileAudioInputStream s = new WaveFileAudioInputStream(Settings.WaveFile);
        assertNotNull(s);

        try {
            s.read(null);
            fail("not expected");
        }
        catch(NullPointerException ex) {
            // expected
        }
        
        s.close();
    }

    @Test
    public void testRead2() {
        WaveFileAudioInputStream s = new WaveFileAudioInputStream(Settings.WaveFile);
        assertNotNull(s);

        byte[] b = new byte[0];
        
        long n = s.read(b);
        assertEquals(0, n);
        
        s.close();
    }

    @Test
    public void testRead3() {
        WaveFileAudioInputStream s = new WaveFileAudioInputStream(Settings.WaveFile);
        assertNotNull(s);

        byte[] b = new byte[16];

        // note: might be less, but must be > 0
        long n = s.read(b);
        assertTrue(n > 0);
        assertTrue(n <= b.length);
        
        s.close();
    }

    @Test
    public void testRead4() {
        WaveFileAudioInputStream s = new WaveFileAudioInputStream(Settings.WaveFile);
        assertNotNull(s);

        byte[] b = new byte[3];

        // note: might be rounded to sample size (which is 2 bytes in our case!)
        long n = s.read(b);
        assertTrue(b.length == n || n == b.length-1);
        
        s.close();
    }
    
    @Test
    public void testRead5() {
        WaveFileAudioInputStream s = new WaveFileAudioInputStream(Settings.WaveFile);
        assertNotNull(s);

        // know for sure that file is smaller than 8MB
        byte[] b = new byte[1024*1024*8];

        // must return real size, not just the length of the passed in array.
        long n = s.read(b);
        assertTrue(n < b.length);
        
        s.close();
    }
    
    @Test
    public void testRead6() {
        WaveFileAudioInputStream s = new WaveFileAudioInputStream(Settings.WaveFile);
        assertNotNull(s);

        // know for sure that file is smaller than 8MB
        byte[] b = new byte[1024*1024*8];

        // drain the file
        long n = s.read(b);
        assertTrue(n < b.length);

        // since we hit the end of the file, must return 0
        n = s.read(b);
        assertEquals(0,  n);
        
        s.close();
    }
    
    // -----------------------------------------------------------------------
    // --- 
    // -----------------------------------------------------------------------

    @Test
    public void testClose() {
        WaveFileAudioInputStream s = new WaveFileAudioInputStream(Settings.WaveFile);
        assertNotNull(s);

        s.close();
    }

    // -----------------------------------------------------------------------
    // --- 
    // -----------------------------------------------------------------------

    @Test
    public void testAudioInputStream() {
        WaveFileAudioInputStream s = new WaveFileAudioInputStream(Settings.WaveFile);
        assertTrue(s instanceof AudioInputStream);
        
        s.close();
    }

}
