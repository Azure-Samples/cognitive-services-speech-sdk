//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

package tests.unit;

import static org.junit.Assert.*;

import org.junit.BeforeClass;
import org.junit.Test;

import com.microsoft.cognitiveservices.speech.audio.PullAudioInputStreamCallback;

import tests.AudioUtterancesKeys;
import tests.Settings;

public class AudioInputStreamTests {
    @BeforeClass
    public static void setUpBeforeClass() throws Exception {
        Settings.LoadSettings();
    }

    // -----------------------------------------------------------------------
    // ---
    // -----------------------------------------------------------------------

    @Test
    public void testRead1() {
        WavFileAudioInputStream s = new WavFileAudioInputStream(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath));
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
        WavFileAudioInputStream s = new WavFileAudioInputStream(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath));
        assertNotNull(s);

        byte[] b = new byte[0];

        int n = s.read(b);
        assertEquals(0, n);

        s.close();
    }

    @Test
    public void testRead3() {
        WavFileAudioInputStream s = new WavFileAudioInputStream(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath));
        assertNotNull(s);

        byte[] b = new byte[16];

        // note: might be less, but must be > 0
        int n = s.read(b);
        assertTrue(n > 0);
        assertTrue(n <= b.length);

        s.close();
    }

    @Test
    public void testRead4() {
        WavFileAudioInputStream s = new WavFileAudioInputStream(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath));
        assertNotNull(s);

        byte[] b = new byte[3];

        // note: might be rounded to sample size (which is 2 bytes in our case!)
        int n = s.read(b);
        assertTrue(b.length == n || n == b.length-1);

        s.close();
    }

    @Test
    public void testRead5() {
        WavFileAudioInputStream s = new WavFileAudioInputStream(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath));
        assertNotNull(s);

        // know for sure that file is smaller than 8MB
        byte[] b = new byte[1024*1024*8];

        // must return real size, not just the length of the passed in array.
        int n = s.read(b);
        assertTrue(n < b.length);

        s.close();
    }

    @Test
    public void testRead6() {
        WavFileAudioInputStream s = new WavFileAudioInputStream(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath));
        assertNotNull(s);

        // know for sure that file is smaller than 8MB
        byte[] b = new byte[1024*1024*8];

        // drain the file
        int n = s.read(b);
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
        WavFileAudioInputStream s = new WavFileAudioInputStream(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath));
        assertNotNull(s);

        s.close();
    }

    // -----------------------------------------------------------------------
    // ---
    // -----------------------------------------------------------------------

    @Test
    public void testAudioInputStream() {
        WavFileAudioInputStream s = new WavFileAudioInputStream(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath));
        assertTrue(s instanceof PullAudioInputStreamCallback);

        s.close();
    }

}
