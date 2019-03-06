package tests.endtoend;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import java.util.Random;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.Ignore;
import static org.junit.Assert.*;

import tests.Settings;

import tests.unit.WavFileAudioInputStream;

@SuppressWarnings("unused")
public class WavFileAudioInputStreamTest {
    WavFileAudioInputStream waveStream;
    
    @BeforeClass
    static public void setUpBeforeClass() throws Exception {
        // Override inputs, if necessary
        Settings.LoadSettings();
    }

    @Test
    public void testCreateWithNullFilename() {
        try {
            new WavFileAudioInputStream(null);
            fail("Not expected");
        }
        catch(Exception ex) {
            // expected
        }
    }
    
    @Test
    public void testCreateWithEmptyFilename() {
        try {
            WavFileAudioInputStream s = new WavFileAudioInputStream("");
            fail("unexpected");
        }
        catch(Exception ex) {
            // expected
        }
    }

    @Test
    public void testCreateWithIllegalFilename() {
        try {
            new WavFileAudioInputStream("illegal-filename-" + (new Random().nextInt()));
            fail("Not expected");
        }
        catch(Exception ex) {
            // expected
        }
    }

    @Test
    public void testCreateWithLegalFilename() {
        new WavFileAudioInputStream(Settings.WavFile);
    }

    @Test
    public void testTestClose1() {
        WavFileAudioInputStream s = new WavFileAudioInputStream(Settings.WavFile);
        assertNotNull(s);

        s.close();
    }

    @Test
    public void testTestClose2() {
        WavFileAudioInputStream s = new WavFileAudioInputStream(Settings.WavFile);
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
        WavFileAudioInputStream s = new WavFileAudioInputStream(Settings.WavFile);
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
        WavFileAudioInputStream s = new WavFileAudioInputStream(Settings.WavFile);
        assertNotNull(s);

        byte[]buf = new byte[0];
        int numread = s.read(buf);
        assertEquals(0, numread);
    }

    @Test
    public void testTestRead3() {
        WavFileAudioInputStream s = new WavFileAudioInputStream(Settings.WavFile);
        assertNotNull(s);

        byte[]buf = new byte[16];
        int numread = s.read(buf);
        assertEquals(16, numread);
    }

    @Test
    public void testTestRead4() {
        WavFileAudioInputStream s = new WavFileAudioInputStream(Settings.WavFile);
        assertNotNull(s);

        byte[]buf = new byte[15];
        int numread = s.read(buf);
        assertTrue(numread > 0); // must read at least one sample since file is not empty 
        assertTrue(numread <= buf.length); // might not read full number of bytes!
    }

    @Test
    public void testTestRead5() {
        WavFileAudioInputStream s = new WavFileAudioInputStream(Settings.WavFile);
        assertNotNull(s);

        // can read before close
        byte[]buf = new byte[2];
        int numread = s.read(buf);
        assertEquals(2, numread);

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
