//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

package tests.endtoend;

import java.util.Random;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.Rule;
import org.junit.Ignore;
import static org.junit.Assert.*;

import tests.AudioUtterancesKeys;
import tests.BaseJUnit;
import tests.Settings;
import tests.Retry;
import tests.unit.WavFileAudioInputStream;

@SuppressWarnings("unused")
public class WavFileAudioInputStreamTest extends BaseJUnit {
    WavFileAudioInputStream waveStream;

    @Rule
    public Retry retry = new Retry(Settings.TestRetryCount);

    @BeforeClass
    static public void setUpBeforeClass() throws Exception {
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
        new WavFileAudioInputStream(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath));
    }

    @Test
    public void testTestClose1() {
        WavFileAudioInputStream s = new WavFileAudioInputStream(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath));
        assertNotNull(s);

        s.close();
    }

    @Test
    public void testTestClose2() {
        WavFileAudioInputStream s = new WavFileAudioInputStream(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath));
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
        WavFileAudioInputStream s = new WavFileAudioInputStream(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath));
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
        WavFileAudioInputStream s = new WavFileAudioInputStream(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath));
        assertNotNull(s);

        byte[]buf = new byte[0];
        int numread = s.read(buf);
        assertEquals(0, numread);
    }

    @Test
    public void testTestRead3() {
        WavFileAudioInputStream s = new WavFileAudioInputStream(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath));
        assertNotNull(s);

        byte[]buf = new byte[16];
        int numread = s.read(buf);
        assertEquals(16, numread);
    }

    @Test
    public void testTestRead4() {
        WavFileAudioInputStream s = new WavFileAudioInputStream(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath));
        assertNotNull(s);

        byte[]buf = new byte[15];
        int numread = s.read(buf);
        assertTrue(numread > 0); // must read at least one sample since file is not empty
        assertTrue(numread <= buf.length); // might not read full number of bytes!
    }

    @Test
    public void testTestRead5() {
        WavFileAudioInputStream s = new WavFileAudioInputStream(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath));
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
