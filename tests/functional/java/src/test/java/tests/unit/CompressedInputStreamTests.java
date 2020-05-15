//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

package tests.unit;

import java.io.FileNotFoundException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.Future;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.TimeoutException;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.atomic.AtomicBoolean;
import java.net.URI;

import static org.junit.Assert.*;

import com.microsoft.cognitiveservices.speech.audio.*;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.Ignore;
import org.junit.Rule;
import org.junit.rules.ExpectedException;

import com.microsoft.cognitiveservices.speech.CancellationReason;
import com.microsoft.cognitiveservices.speech.ResultReason;
import com.microsoft.cognitiveservices.speech.ServicePropertyChannel;
import com.microsoft.cognitiveservices.speech.Recognizer;
import com.microsoft.cognitiveservices.speech.SpeechConfig;
import com.microsoft.cognitiveservices.speech.SpeechRecognitionResult;
import com.microsoft.cognitiveservices.speech.SpeechRecognizer;
import com.microsoft.cognitiveservices.speech.PropertyId;
import com.microsoft.cognitiveservices.speech.OutputFormat;
import com.microsoft.cognitiveservices.speech.Connection;
import com.microsoft.cognitiveservices.speech.ProfanityOption;
import com.microsoft.cognitiveservices.speech.SourceLanguageConfig;
import com.microsoft.cognitiveservices.speech.AutoDetectSourceLanguageConfig;
import com.microsoft.cognitiveservices.speech.AutoDetectSourceLanguageResult;

import tests.AudioUtterancesKeys;
import tests.Settings;
import tests.SubscriptionsRegionsKeys;
import tests.TestHelper;

public class CompressedInputStreamTests {

    @BeforeClass
    static public void setUpBeforeClass() throws Exception {
        String operatingSystem = ("" + System.getProperty("os.name")).toLowerCase();
        System.out.println("Current operation system: " + operatingSystem);
        // "linux" covers Android also because on android the underlined os is linux
        boolean isWindowsOrLinux = operatingSystem.contains("windows") || operatingSystem.contains("linux");
        org.junit.Assume.assumeFalse(!isWindowsOrLinux);
        Settings.LoadSettings();
    }

    // -----------------------------------------------------------------------
    // ---
    // -----------------------------------------------------------------------


    @Test
    public void testSpeechRecognizerCompressedInput() throws InterruptedException, ExecutionException, FileNotFoundException {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Key,
            Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Region);

        assertNotNull(s);

        BinaryAudioStreamReader binStreamReader = new BinaryAudioStreamReader(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_MP3).FilePath));
        assertNotNull(binStreamReader);

        PullAudioInputStream pullAudio = AudioInputStream.createPullStream(binStreamReader,
            AudioStreamFormat.getCompressedFormat(AudioStreamContainerFormat.MP3));

        SpeechRecognizer r = new SpeechRecognizer(s, AudioConfig.fromStreamInput(pullAudio));
        assertNotNull(r);
        assertNotNull(r.getRecoImpl());
        assertTrue(r instanceof Recognizer);

        SpeechRecognitionResult res = r.recognizeOnceAsync().get();
        assertNotNull(res);
        TestHelper.OutputResult(res);
        assertEquals("What's the weather like?", res.getText());

        r.close();
        s.close();
    }

}
