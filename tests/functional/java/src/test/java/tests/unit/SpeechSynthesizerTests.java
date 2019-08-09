//
//Copyright (c) Microsoft. All rights reserved.
//Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

package tests.unit;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.Future;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.TimeoutException;
import java.util.concurrent.atomic.AtomicInteger;
import java.net.URI;

import java.io.DataInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.UnsupportedEncodingException;

import java.lang.Math;

import static org.junit.Assert.*;

import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.Ignore;


import com.microsoft.cognitiveservices.speech.audio.AudioConfig;
import com.microsoft.cognitiveservices.speech.audio.AudioOutputStream;
import com.microsoft.cognitiveservices.speech.audio.PullAudioOutputStream;
import com.microsoft.cognitiveservices.speech.audio.PushAudioOutputStream;
import com.microsoft.cognitiveservices.speech.audio.PushAudioOutputStreamCallback;
import com.microsoft.cognitiveservices.speech.AudioDataStream;
import com.microsoft.cognitiveservices.speech.CancellationErrorCode;
import com.microsoft.cognitiveservices.speech.CancellationReason;
import com.microsoft.cognitiveservices.speech.ResultReason;
import com.microsoft.cognitiveservices.speech.SpeechConfig;
import com.microsoft.cognitiveservices.speech.SpeechSynthesisResult;
import com.microsoft.cognitiveservices.speech.SpeechSynthesisWordBoundaryEventArgs;
import com.microsoft.cognitiveservices.speech.SpeechSynthesizer;
import com.microsoft.cognitiveservices.speech.StreamStatus;
import com.microsoft.cognitiveservices.speech.SpeechSynthesisCancellationDetails;
import com.microsoft.cognitiveservices.speech.SpeechSynthesisOutputFormat;
import com.microsoft.cognitiveservices.speech.PropertyId;
import com.microsoft.cognitiveservices.speech.util.EventHandler;

import tests.Settings;
import tests.TestHelper;

public class SpeechSynthesizerTests {
    private final Integer EMPTY_WAVE_FILE_SIZE = 46;

    private final Integer GUID_LENGTH = 32;

    private final Integer BINARY_COMPARE_TOLERANCE = 0;

    private final Integer MOCK_AUDIO_SIZE = 32000;

    private final Integer MOCK_AUDIO_CHUNK_SIZE = 3200;

    private final String SSML_TEMPLATE = "<speak version='1.0' xmlns='http://www.w3.org/2001/10/synthesis' xmlns:mstts='http://www.w3.org/2001/mstts' xmlns:emo='http://www.w3.org/2009/10/emotionml' xml:lang='%s'><voice name='%s'>%s</voice></speak>";

    private final String DEFAULT_LANGUAGE = "en-US";

    private final String DEFAULT_VOICE = "Microsoft Server Speech Text to Speech Voice (en-US, JessaRUS)";

    private static ExecutorService s_executorService;

    @BeforeClass
    static public void setUpBeforeClass() throws Exception {
        // Override inputs, if necessary
        Settings.LoadSettings();

        // Start a thread pool for asynchronous action
        s_executorService = Executors.newCachedThreadPool();
    }

    // -----------------------------------------------------------------------
    // ---
    // -----------------------------------------------------------------------

    @Test
    public void testDefaults() throws InterruptedException, ExecutionException {
        SpeechConfig speechConfig = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(speechConfig);

        SpeechSynthesizer synthesizer = new SpeechSynthesizer(speechConfig);
        assertNotNull(synthesizer);

        SpeechSynthesisResult result1 = synthesizer.SpeakTextAsync("{{{text1}}}").get(); // "{{{text1}}}" has now completed rendering to default speakers
        SpeechSynthesisResult result2 = synthesizer.SpeakTextAsync("{{{text2}}}").get(); // "{{{text2}}}" has now completed rendering to default speakers

        result1.close();
        result2.close();
        synthesizer.close();
        speechConfig.close();
    }

    @Test
    public void testExplicitlyUseDefaultSpeakers() throws InterruptedException, ExecutionException {
        SpeechConfig speechConfig = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(speechConfig);

        AudioConfig deviceConfig = AudioConfig.fromDefaultSpeakerOutput();
        assertNotNull(deviceConfig);

        SpeechSynthesizer synthesizer = new SpeechSynthesizer(speechConfig, deviceConfig);
        assertNotNull(synthesizer);

        SpeechSynthesisResult result1 = synthesizer.SpeakTextAsync("{{{text1}}}").get(); // "{{{text1}}}" has now completed rendering to default speakers
        SpeechSynthesisResult result2 = synthesizer.SpeakTextAsync("{{{text2}}}").get(); // "{{{text2}}}" has now completed rendering to default speakers

        result1.close();
        result2.close();
        synthesizer.close();
        deviceConfig.close();
        speechConfig.close();
    }

    @Test
    public void testPickLanguage() throws InterruptedException, ExecutionException {
        SpeechConfig speechConfig = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(speechConfig);
        speechConfig.setSpeechSynthesisLanguage("en-GB");

        SpeechSynthesizer synthesizer = new SpeechSynthesizer(speechConfig);
        assertNotNull(synthesizer);

        SpeechSynthesisResult result1 = synthesizer.SpeakTextAsync("{{{text1}}}").get(); // "{{{text1}}}" has now completed rendering to default speakers
        SpeechSynthesisResult result2 = synthesizer.SpeakTextAsync("{{{text2}}}").get(); // "{{{text2}}}" has now completed rendering to default speakers

        result1.close();
        result2.close();
        synthesizer.close();
        speechConfig.close();
    }

    @Test
    public void testPickVoice() throws InterruptedException, ExecutionException {
        SpeechConfig speechConfig = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(speechConfig);
        speechConfig.setSpeechSynthesisVoiceName("Microsoft Server Speech Text to Speech Voice (en-GB, HazelRUS)");

        SpeechSynthesizer synthesizer = new SpeechSynthesizer(speechConfig);
        assertNotNull(synthesizer);

        SpeechSynthesisResult result1 = synthesizer.SpeakTextAsync("{{{text1}}}").get(); // "{{{text1}}}" has now completed rendering to default speakers
        SpeechSynthesisResult result2 = synthesizer.SpeakTextAsync("{{{text2}}}").get(); // "{{{text2}}}" has now completed rendering to default speakers

        result1.close();
        result2.close();
        synthesizer.close();
        speechConfig.close();
    }

    @Test
    public void testSynthesizerOutputToFile() throws InterruptedException, ExecutionException {
        SpeechConfig speechConfig = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(speechConfig);

        AudioConfig fileConfig = AudioConfig.fromWavFileOutput("wavefile.wav");
        assertNotNull(fileConfig);

        SpeechSynthesizer synthesizer = new SpeechSynthesizer(speechConfig, fileConfig);
        assertNotNull(synthesizer);
        SpeechSynthesisResult result1 = synthesizer.SpeakTextAsync("{{{text1}}}").get(); // "{{{wavefile.wav}}}" now contains synthesized audio for "{{{text1}}}""
        result1.close();
        synthesizer.close(); // "{{{wavefile.wav}}}" is now closed

        long waveSize1 = (new File("wavefile.wav")).length();
        assertTrue(waveSize1 > EMPTY_WAVE_FILE_SIZE);

        synthesizer = new SpeechSynthesizer(speechConfig, fileConfig);
        assertNotNull(synthesizer);
        result1 = synthesizer.SpeakTextAsync("{{{text1}}}").get(); // "{{{wavefile.wav}}}" now contains synthesized audio for "{{{text1}}}""
        result1.close();
        SpeechSynthesisResult result2 = synthesizer.SpeakTextAsync("{{{text2}}}").get(); // "{{{wavefile.wav}}}" now contains synthesized audio for both "{{{text1}}}"" and "{{{text2}}}"
        result2.close();
        synthesizer.close(); // "{{{wavefile.wav}}}" is now closed

        long waveSize2 = (new File("wavefile.wav")).length();
        assertTrue(waveSize2 > waveSize1);

        fileConfig.close();
        speechConfig.close();
    }

    @Test
    public void testSynthesizerOutputToPushStream() throws InterruptedException, ExecutionException {
        SpeechConfig speechConfig = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(speechConfig);

        PushAudioOutputStreamTestCallback callback = new PushAudioOutputStreamTestCallback();
        PushAudioOutputStream stream = AudioOutputStream.createPushStream(callback);
        assertNotNull(stream);

        AudioConfig streamConfig = AudioConfig.fromStreamOutput(stream);
        assertNotNull(streamConfig);

        SpeechSynthesizer synthesizer = new SpeechSynthesizer(speechConfig, streamConfig);
        assertNotNull(synthesizer);

        SpeechSynthesisResult result1 = synthesizer.SpeakTextAsync("{{{text1}}}").get(); // "{{{text1}}}" has completed rendering to pullstream
        SpeechSynthesisResult result2 = synthesizer.SpeakTextAsync("{{{text2}}}").get(); // "{{{text2}}}" has completed rendering to pullstream

        result1.close();
        result2.close();
        synthesizer.close();

        assertTrue(callback.getAudioSize() > 0);
        assertTrue(callback.isClosed());
    }

    @Test
    public void testSynthesizerOutputToPullStreamUseAfterSynthesisCompleted() throws InterruptedException, ExecutionException {
        SpeechConfig speechConfig = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(speechConfig);

        PullAudioOutputStream stream = AudioOutputStream.createPullStream();
        assertNotNull(stream);

        AudioConfig streamConfig = AudioConfig.fromStreamOutput(stream);
        assertNotNull(streamConfig);

        SpeechSynthesizer synthesizer = new SpeechSynthesizer(speechConfig, streamConfig);
        assertNotNull(synthesizer);

        SpeechSynthesisResult result1 = synthesizer.SpeakTextAsync("{{{text1}}}").get(); // "{{{text1}}}" has completed rendering to pullstream
        SpeechSynthesisResult result2 = synthesizer.SpeakTextAsync("{{{text2}}}").get(); // "{{{text2}}}" has completed rendering to pullstream

        result1.close();
        result2.close();
        synthesizer.close();
        streamConfig.close();
        speechConfig.close();

        DoSomethingWithAudioInPullStream(stream);

        stream.close();
    }

    @Test
    public void testSynthesizerOutputToPullStreamStartUsingBeforeDoneSynthesizing() throws InterruptedException, ExecutionException {
        SpeechConfig speechConfig = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(speechConfig);

        PullAudioOutputStream stream = AudioOutputStream.createPullStream();
        assertNotNull(stream);

        Future<Void> future = DoSomethingWithAudioInPullStreamInBackground(stream);

        AudioConfig streamConfig = AudioConfig.fromStreamOutput(stream);
        assertNotNull(streamConfig);

        SpeechSynthesizer synthesizer = new SpeechSynthesizer(speechConfig, streamConfig);
        assertNotNull(synthesizer);

        SpeechSynthesisResult result1 = synthesizer.SpeakTextAsync("{{{text1}}}").get(); // "{{{text1}}}" has completed rendering to pullstream
        SpeechSynthesisResult result2 = synthesizer.SpeakTextAsync("{{{text2}}}").get(); // "{{{text2}}}" has completed rendering to pullstream

        result1.close();
        result2.close();
        synthesizer.close();
        streamConfig.close();
        speechConfig.close();

        future.get();

        stream.close();
    }

    @Test
    public void testSpeakOutInResults() throws InterruptedException, ExecutionException {
        SpeechConfig speechConfig = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(speechConfig);

        SpeechSynthesizer synthesizer = new SpeechSynthesizer(speechConfig, null); // null indicates to do nothing with synthesizer audio by default
        assertNotNull(synthesizer);

        SpeechSynthesisResult result1 = synthesizer.SpeakTextAsync("{{{text1}}}").get(); // "{{{text1}}}" has completed rendering, and available in result1
        assertEquals(GUID_LENGTH, (Integer)result1.getResultId().length());
        assertEquals(ResultReason.SynthesizingAudioCompleted, result1.getReason());
        assertTrue(result1.getAudioLength() > 0);

        byte[] audioData1 = result1.getAudioData();
        DoSomethingWithAudioInVector(audioData1, result1.getAudioLength());

        SpeechSynthesisResult result2 = synthesizer.SpeakTextAsync("{{{text2}}}").get(); // "{{{text2}}}" has completed rendering, and available in result2
        assertEquals(GUID_LENGTH, (Integer)result2.getResultId().length());
        assertEquals(ResultReason.SynthesizingAudioCompleted, result2.getReason());
        assertTrue(result2.getAudioLength() > 0);

        byte[] audioData2 = result2.getAudioData();
        DoSomethingWithAudioInVector(audioData2, result2.getAudioLength());

        result1.close();
        result2.close();
        synthesizer.close();
        speechConfig.close();
    }

    @Test
    public void testSpeakOutputInChunksInEventSynthesizing() throws InterruptedException, ExecutionException {
        SpeechConfig speechConfig = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(speechConfig);

        SpeechSynthesizer synthesizer = new SpeechSynthesizer(speechConfig, null); // null indicates to do nothing with synthesizer audio by default
        assertNotNull(synthesizer);

        synthesizer.Synthesizing.addEventListener((o, e) -> {
            ResultReason resultReason = e.getResult().getReason();
            assertEquals(ResultReason.SynthesizingAudio, resultReason);

            long audioLength = e.getResult().getAudioLength();
            assertTrue(audioLength > 0);

            byte[] audioData = e.getResult().getAudioData();
            DoSomethingWithAudioInVector(audioData, e.getResult().getAudioLength());
        });

        SpeechSynthesisResult result1 = synthesizer.SpeakTextAsync("{{{text1}}}").get(); // "{{{text1}}}" has completed rendering
        SpeechSynthesisResult result2 = synthesizer.SpeakTextAsync("{{{text2}}}").get(); // "{{{text2}}}" has completed rendering

        result1.close();
        result2.close();
        synthesizer.close();
        speechConfig.close();
    }

    @Test
    public void testSpeakOutputInStreams() throws InterruptedException, ExecutionException {
        SpeechConfig speechConfig = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(speechConfig);

        SpeechSynthesizer synthesizer = new SpeechSynthesizer(speechConfig, null); // null indicates to do nothing with synthesizer audio by default
        assertNotNull(synthesizer);

        SpeechSynthesisResult result1 = synthesizer.SpeakTextAsync("{{{text1}}}").get(); // "{{{text1}}}" has completed rendering, and available in result1
        assertEquals(ResultReason.SynthesizingAudioCompleted, result1.getReason());
        assertTrue(result1.getAudioLength() > 0);

        AudioDataStream stream1 = AudioDataStream.fromResult(result1);
        DoSomethingWithAudioInDataStream(stream1, true);

        SpeechSynthesisResult result2 = synthesizer.SpeakTextAsync("{{{text2}}}").get(); // "{{{text2}}}" has completed rendering, and available in result2
        assertEquals(ResultReason.SynthesizingAudioCompleted, result2.getReason());
        assertTrue(result2.getAudioLength() > 0);

        AudioDataStream stream2 = AudioDataStream.fromResult(result2);
        DoSomethingWithAudioInDataStream(stream2, true);

        result1.close();
        result2.close();
        stream1.close();
        stream2.close();
        synthesizer.close();
        speechConfig.close();
    }

    @Test
    public void testSpeakOutputInStreamsBeforeDoneFromEventSynthesisStarted() throws InterruptedException, ExecutionException {
        SpeechConfig speechConfig = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(speechConfig);

        SpeechSynthesizer synthesizer = new SpeechSynthesizer(speechConfig, null); // null indicates to do nothing with synthesizer audio by default
        assertNotNull(synthesizer);

        final ArrayList<Future<Void>> backgroundThreads = new ArrayList<>();
        final ArrayList<AudioDataStream> streams = new ArrayList<>();
        synthesizer.SynthesisStarted.addEventListener((o, e) -> {
            ResultReason resultReason = e.getResult().getReason();
            assertEquals(ResultReason.SynthesizingAudioStarted, resultReason);

            long audioLength = e.getResult().getAudioLength();
            assertEquals(0, audioLength);

            AudioDataStream stream = AudioDataStream.fromResult(e.getResult());
            streams.add(stream);
            backgroundThreads.add(DoSomethingWithAudioInDataStreamInBackground(stream, false));
        });

        SpeechSynthesisResult result1 = synthesizer.SpeakTextAsync("{{{text1}}}").get(); // "{{{text1}}}" has completed rendering
        SpeechSynthesisResult result2 = synthesizer.SpeakTextAsync("{{{text2}}}").get(); // "{{{text2}}}" has completed rendering

        Future<SpeechSynthesisResult> future3 = synthesizer.SpeakTextAsync("{{{text3}}}"); // "{{{text3}}}" synthesis might have started
        SpeechSynthesisResult result3 = future3.get(); // "{{{text3}}}" synthesis has completed

        for (int i = 0; i < backgroundThreads.size(); ++i)
        {
            backgroundThreads.get(i).get();
        }

        for (int i = 0; i < streams.size(); ++i)
        {
            streams.get(i).close();
        }

        result1.close();
        result2.close();
        result3.close();
        synthesizer.close();
        speechConfig.close();
    }

    @Test
    public void testSpeakOutputInStreamsBeforeDoneFromMethodStartSpeakingTextAsync() throws InterruptedException, ExecutionException {
        SpeechConfig speechConfig = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(speechConfig);

        SpeechSynthesizer synthesizer = new SpeechSynthesizer(speechConfig, null); // null indicates to do nothing with synthesizer audio by default
        assertNotNull(synthesizer);

        SpeechSynthesisResult result1 = synthesizer.StartSpeakingTextAsync("{{{text1}}}").get(); // "{{{text1}}}" synthesis has started, likely not finished
        assertEquals(ResultReason.SynthesizingAudioStarted, result1.getReason());
        assertEquals(0, result1.getAudioLength());

        AudioDataStream stream1 = AudioDataStream.fromResult(result1); // of type AudioDataStream, does not block
        Future<Void> future1 = DoSomethingWithAudioInDataStreamInBackground(stream1, false); // does not block, just spins a thread up

        SpeechSynthesisResult result2 = synthesizer.StartSpeakingTextAsync("{{{text2}}}").get(); // "{{{text2}}}" synthesis has started, likely not finished
        assertEquals(ResultReason.SynthesizingAudioStarted, result2.getReason());
        assertEquals(0, result2.getAudioLength());

        AudioDataStream stream2 = AudioDataStream.fromResult(result2); // of type AudioDataStream, does not block
        Future<Void> future2 = DoSomethingWithAudioInDataStreamInBackground(stream2, false); // does not block, just spins a thread up

        future1.get();
        future2.get();

        result1.close();
        result2.close();

        stream1.close();
        stream2.close();

        synthesizer.close();
        speechConfig.close();
    }

    @Test
    public void testSpeakOutputInStreamsBeforeDoneQueued() throws InterruptedException, ExecutionException {
        SpeechConfig speechConfig = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(speechConfig);

        SpeechSynthesizer synthesizer = new SpeechSynthesizer(speechConfig, null); // null indicates to do nothing with synthesizer audio by default
        assertNotNull(synthesizer);

        final int[] startedRequests = new int[1];
        startedRequests[0] = 0;
        synthesizer.SynthesisStarted.addEventListener((o, e) -> {
            startedRequests[0]++;
        });

        final int[] completedRequests = new int[1];
        completedRequests[0] = 0;
        synthesizer.SynthesisCompleted.addEventListener((o, e) -> {
            completedRequests[0]++;
            if (completedRequests[0] == 1)
            {
                // This is to check the requests is queued
                // When one request is already completed, the other one is still not started
                assertEquals(1, startedRequests[0]);
            }
        });

        Future<SpeechSynthesisResult> futureResult1 = synthesizer.StartSpeakingTextAsync("{{{text1}}}"); // "{{{text1}}}" synthesis might have started, likely not finished
        Future<SpeechSynthesisResult> futureResult2 = synthesizer.StartSpeakingTextAsync("{{{text2}}}"); // "{{{text2}}}" synthesis might have started (very unlikely)

        Future<Void> future1 = DoSomethingWithAudioInResultInBackground(futureResult1, false); // does not block, just spins a thread up
        Future<Void> future2 = DoSomethingWithAudioInResultInBackground(futureResult2, false); // does not block, just spins a thread up

        future1.get();
        future2.get();

        synthesizer.close();
        speechConfig.close();
    }

    @Test
    public void testSpeakOutputInStreamsWithAllDataGetOnSynthesisStartedResult() throws InterruptedException, ExecutionException {
        SpeechConfig speechConfig = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(speechConfig);

        SpeechSynthesizer synthesizer = new SpeechSynthesizer(speechConfig, null); // null indicates to do nothing with synthesizer audio by default
        assertNotNull(synthesizer);

        SpeechSynthesisResult result = synthesizer.StartSpeakingTextAsync("{{{text1}}}").get(); // "{{{text1}}}" synthesis has started, likely not finished
        assertEquals(ResultReason.SynthesizingAudioStarted, result.getReason());
        assertEquals(0, result.getAudioLength());

        final boolean[] SynthesisCompleted = new boolean[1];
        SynthesisCompleted[0] = false;

        synthesizer.SynthesisCompleted.addEventListener((o, e) -> {
            SynthesisCompleted[0] = true;
        });

        while (!SynthesisCompleted[0]) {
            Thread.sleep(100); // wait for the synthesis to be done
        }

        AudioDataStream stream = AudioDataStream.fromResult(result); // now get the stream from result
        DoSomethingWithAudioInDataStream(stream, true);

        stream.close();
        result.close();
        synthesizer.close();
        speechConfig.close();
    }

    @Test
    public void testSpeakOutputWithWordBoundaryEvents() throws InterruptedException, ExecutionException {
        String endpoint = String.format("wss://%s.tts.speech.microsoft.com/cognitiveservices/websocket/v1", Settings.SpeechRegion);
        SpeechConfig speechConfig = SpeechConfig.fromEndpoint(URI.create(endpoint), Settings.SpeechSubscriptionKey);
        assertNotNull(speechConfig);

        speechConfig.setSpeechSynthesisVoiceName("Microsoft Server Speech Text to Speech Voice (zh-CN, HuihuiRUS)");

        SpeechSynthesizer synthesizer = new SpeechSynthesizer(speechConfig, null); // null indicates to do nothing with synthesizer audio by default
        assertNotNull(synthesizer);

        String plainText = "您好，我是来自Microsoft的中文声音。";
        String ssml = "<speak version='1.0' xmlns='http://www.w3.org/2001/10/synthesis' xmlns:mstts='http://www.w3.org/2001/mstts' xmlns:emo='http://www.w3.org/2009/10/emotionml' xml:lang='zh-CN'><voice name='Microsoft Server Speech Text to Speech Voice (zh-CN, HuihuiRUS)'>您好，<break time='50ms'/>我是来自Microsoft的中文声音。</voice></speak>";

        int expectedWordCount = 8; // Both above plain text and SSML are expected to be separated to 8 words
        long[] expectedAudioOffsets = { 500000, 7443750, 9175625, 10585000, 15096875, 22924375, 25213125, 28760000 };
        long[] expectedTextOffsets = { 0, 3, 4, 5, 7, 16, 17, 19 };
        long[] expectedSsmlOffsets = { 251, 274, 275, 276, 278, 287, 288, 290 };
        long[] expectedWordLengths = { 2, 1, 1, 2, 9, 1, 2, 2 };

        final int[] order = new int[1];
        order[0] = 0;
        EventHandler<SpeechSynthesisWordBoundaryEventArgs> wordBoundaryEventHandler = new EventHandler<SpeechSynthesisWordBoundaryEventArgs>() {
            @Override
            public void onEvent(Object o, SpeechSynthesisWordBoundaryEventArgs e) {
                assertEquals(expectedAudioOffsets[order[0]], e.getAudioOffset());
                assertEquals(expectedTextOffsets[order[0]], e.getTextOffset());
                assertEquals(expectedWordLengths[order[0]], e.getWordLength());
                order[0]++;
            }
        };

        synthesizer.WordBoundary.addEventListener(wordBoundaryEventHandler);
        SpeechSynthesisResult result = synthesizer.SpeakTextAsync(plainText).get();
        result.close();

        assertEquals(expectedWordCount, order[0]);

        synthesizer.WordBoundary.removeEventListener(wordBoundaryEventHandler);

        order[0] = 0;
        wordBoundaryEventHandler = new EventHandler<SpeechSynthesisWordBoundaryEventArgs>() {
            @Override
            public void onEvent(Object o, SpeechSynthesisWordBoundaryEventArgs e) {
                assertTrue(e.getAudioOffset() > 0);
                assertEquals(expectedSsmlOffsets[order[0]], e.getTextOffset());
                assertEquals(expectedWordLengths[order[0]], e.getWordLength());
                order[0]++;
            }
        };

        synthesizer.WordBoundary.addEventListener(wordBoundaryEventHandler);
        result = synthesizer.SpeakSsmlAsync(ssml).get();
        result.close();

        assertEquals(expectedWordCount, order[0]);

        synthesizer.close();
        speechConfig.close();
    }

    @Test
    public void testAuthorizationToken() throws InterruptedException, ExecutionException, IOException {
        SpeechConfig speechConfig = SpeechConfig.fromAuthorizationToken("InvalidToken", Settings.SpeechRegion);
        assertNotNull(speechConfig);

        SpeechSynthesizer synthesizer = new SpeechSynthesizer(speechConfig, null); // null indicates to do nothing with synthesizer audio by default
        assertNotNull(synthesizer);
        assertEquals("InvalidToken", synthesizer.getAuthorizationToken());

        String authorizationToken = TestHelper.getAuthorizationToken(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        synthesizer.setAuthorizationToken(authorizationToken);

        SpeechSynthesisResult result1 = synthesizer.SpeakTextAsync("{{{text1}}}").get(); // "{{{text1}}}" has completed rendering, and available in result1
        assertEquals(GUID_LENGTH, (Integer)result1.getResultId().length());
        assertEquals(ResultReason.SynthesizingAudioCompleted, result1.getReason());
        assertTrue(result1.getAudioLength() > 0);

        byte[] audioData1 = result1.getAudioData();
        DoSomethingWithAudioInVector(audioData1, result1.getAudioLength());

        SpeechSynthesisResult result2 = synthesizer.SpeakTextAsync("{{{text2}}}").get(); // "{{{text2}}}" has completed rendering, and available in result2
        assertEquals(GUID_LENGTH, (Integer)result2.getResultId().length());
        assertEquals(ResultReason.SynthesizingAudioCompleted, result2.getReason());
        assertTrue(result2.getAudioLength() > 0);

        byte[] audioData2 = result2.getAudioData();
        DoSomethingWithAudioInVector(audioData2, result2.getAudioLength());

        result1.close();
        result2.close();
        synthesizer.close();
        speechConfig.close();
    }

    @Ignore("TODO not supported by current CI")
    @Test
    public void testDefaultsMock() throws InterruptedException, ExecutionException, UnsupportedEncodingException {
        SpeechConfig speechConfig = SpeechConfig.fromSubscription("None", "None");
        assertNotNull(speechConfig);
        speechConfig.setProperty("CARBON-INTERNAL-UseTtsEngine-Mock", "true");

        SpeechSynthesizer synthesizer = new SpeechSynthesizer(speechConfig);
        assertNotNull(synthesizer);

        SpeechSynthesisResult result1 = synthesizer.SpeakTextAsync("{{{text1}}}").get(); // "{{{text1}}}" has now completed rendering to default speakers
        SpeechSynthesisResult result2 = synthesizer.SpeakTextAsync("{{{text2}}}").get(); // "{{{text2}}}" has now completed rendering to default speakers

        byte[] expectedAudioData1 = buildMockSynthesizedAudio("{{{text1}}}", DEFAULT_LANGUAGE, DEFAULT_VOICE);
        byte[] expectedAudioData2 = buildMockSynthesizedAudio("{{{text2}}}", DEFAULT_LANGUAGE, DEFAULT_VOICE);

        assertTrue(areBinaryEqual(expectedAudioData1, result1.getAudioData()));
        assertTrue(areBinaryEqual(expectedAudioData2, result2.getAudioData()));

        result1.close();
        result2.close();
        synthesizer.close();
        speechConfig.close();
    }

    @Ignore("TODO not supported by current CI")
    @Test
    public void testExplicitlyUseDefaultSpeakersMock() throws InterruptedException, ExecutionException, UnsupportedEncodingException {
        SpeechConfig speechConfig = SpeechConfig.fromSubscription("None", "None");
        assertNotNull(speechConfig);
        speechConfig.setProperty("CARBON-INTERNAL-UseTtsEngine-Mock", "true");

        AudioConfig deviceConfig = AudioConfig.fromDefaultSpeakerOutput();
        assertNotNull(deviceConfig);

        SpeechSynthesizer synthesizer = new SpeechSynthesizer(speechConfig, deviceConfig);
        assertNotNull(synthesizer);

        SpeechSynthesisResult result1 = synthesizer.SpeakTextAsync("{{{text1}}}").get(); // "{{{text1}}}" has now completed rendering to default speakers
        SpeechSynthesisResult result2 = synthesizer.SpeakTextAsync("{{{text2}}}").get(); // "{{{text2}}}" has now completed rendering to default speakers

        byte[] expectedAudioData1 = buildMockSynthesizedAudio("{{{text1}}}", DEFAULT_LANGUAGE, DEFAULT_VOICE);
        byte[] expectedAudioData2 = buildMockSynthesizedAudio("{{{text2}}}", DEFAULT_LANGUAGE, DEFAULT_VOICE);

        assertTrue(areBinaryEqual(expectedAudioData1, result1.getAudioData()));
        assertTrue(areBinaryEqual(expectedAudioData2, result2.getAudioData()));

        result1.close();
        result2.close();
        synthesizer.close();
        deviceConfig.close();
        speechConfig.close();
    }

    @Ignore("TODO not supported by current CI")
    @Test
    public void testPickLanguageMock() throws InterruptedException, ExecutionException, UnsupportedEncodingException {
        SpeechConfig speechConfig = SpeechConfig.fromSubscription("None", "None");
        assertNotNull(speechConfig);
        speechConfig.setProperty("CARBON-INTERNAL-UseTtsEngine-Mock", "true");
        speechConfig.setSpeechSynthesisLanguage("en-GB");

        SpeechSynthesizer synthesizer = new SpeechSynthesizer(speechConfig);
        assertNotNull(synthesizer);

        SpeechSynthesisResult result1 = synthesizer.SpeakTextAsync("{{{text1}}}").get(); // "{{{text1}}}" has now completed rendering to default speakers
        SpeechSynthesisResult result2 = synthesizer.SpeakTextAsync("{{{text2}}}").get(); // "{{{text2}}}" has now completed rendering to default speakers

        byte[] expectedAudioData1 = buildMockSynthesizedAudio("{{{text1}}}", "en-GB", "Microsoft Server Speech Text to Speech Voice (en-GB, HazelRUS)");
        byte[] expectedAudioData2 = buildMockSynthesizedAudio("{{{text2}}}", "en-GB", "Microsoft Server Speech Text to Speech Voice (en-GB, HazelRUS)");

        assertTrue(areBinaryEqual(expectedAudioData1, result1.getAudioData()));
        assertTrue(areBinaryEqual(expectedAudioData2, result2.getAudioData()));

        result1.close();
        result2.close();
        synthesizer.close();
        speechConfig.close();
    }

    @Ignore("TODO not supported by current CI")
    @Test
    public void testPickVoiceMock() throws InterruptedException, ExecutionException, UnsupportedEncodingException {
        SpeechConfig speechConfig = SpeechConfig.fromSubscription("None", "None");
        assertNotNull(speechConfig);
        speechConfig.setProperty("CARBON-INTERNAL-UseTtsEngine-Mock", "true");
        speechConfig.setSpeechSynthesisVoiceName("Microsoft Server Speech Text to Speech Voice (en-GB, HazelRUS)");

        SpeechSynthesizer synthesizer = new SpeechSynthesizer(speechConfig);
        assertNotNull(synthesizer);

        SpeechSynthesisResult result1 = synthesizer.SpeakTextAsync("{{{text1}}}").get(); // "{{{text1}}}" has now completed rendering to default speakers
        SpeechSynthesisResult result2 = synthesizer.SpeakTextAsync("{{{text2}}}").get(); // "{{{text2}}}" has now completed rendering to default speakers

        byte[] expectedAudioData1 = buildMockSynthesizedAudio("{{{text1}}}", DEFAULT_LANGUAGE, "Microsoft Server Speech Text to Speech Voice (en-GB, HazelRUS)");
        byte[] expectedAudioData2 = buildMockSynthesizedAudio("{{{text2}}}", DEFAULT_LANGUAGE, "Microsoft Server Speech Text to Speech Voice (en-GB, HazelRUS)");

        assertTrue(areBinaryEqual(expectedAudioData1, result1.getAudioData()));
        assertTrue(areBinaryEqual(expectedAudioData2, result2.getAudioData()));

        result1.close();
        result2.close();
        synthesizer.close();
        speechConfig.close();
    }

    @Ignore("TODO not supported by current CI")
    @Test
    public void testSynthesizerOutputToFileMock() throws InterruptedException, ExecutionException, UnsupportedEncodingException, IOException {
        SpeechConfig speechConfig = SpeechConfig.fromSubscription("None", "None");
        assertNotNull(speechConfig);
        speechConfig.setProperty("CARBON-INTERNAL-UseTtsEngine-Mock", "true");

        AudioConfig fileConfig = AudioConfig.fromWavFileOutput("wavefile.wav");
        assertNotNull(fileConfig);

        SpeechSynthesizer synthesizer = new SpeechSynthesizer(speechConfig, fileConfig);
        assertNotNull(synthesizer);
        SpeechSynthesisResult result1 = synthesizer.SpeakTextAsync("{{{text1}}}").get(); // "{{{wavefile.wav}}}" now contains synthesized audio for "{{{text1}}}""
        result1.close();
        synthesizer.close(); // "{{{wavefile.wav}}}" is now closed

        byte[] waveData1 = loadWaveFileData("wavefile.wav");
        byte[] expectedAudioData1 = buildMockSynthesizedAudio("{{{text1}}}", DEFAULT_LANGUAGE, DEFAULT_VOICE);
        assertTrue(areBinaryEqual(expectedAudioData1, waveData1));

        synthesizer = new SpeechSynthesizer(speechConfig, fileConfig);
        assertNotNull(synthesizer);
        result1 = synthesizer.SpeakTextAsync("{{{text1}}}").get(); // "{{{wavefile.wav}}}" now contains synthesized audio for "{{{text1}}}""
        result1.close();
        SpeechSynthesisResult result2 = synthesizer.SpeakTextAsync("{{{text2}}}").get(); // "{{{wavefile.wav}}}" now contains synthesized audio for both "{{{text1}}}"" and "{{{text2}}}"
        result2.close();
        synthesizer.close(); // "{{{wavefile.wav}}}" is now closed

        byte[] waveData12 = loadWaveFileData("wavefile.wav");
        byte[] expectedAudioData2 = buildMockSynthesizedAudio("{{{text2}}}", DEFAULT_LANGUAGE, DEFAULT_VOICE);
        byte[] expectedAudioData12 = mergeBinary(expectedAudioData1, expectedAudioData2);
        assertTrue(areBinaryEqual(expectedAudioData12, waveData12));

        fileConfig.close();
        speechConfig.close();
    }

    @Ignore("TODO not supported by current CI")
    @Test
    public void testSynthesizerOutputToPushStreamMock() throws InterruptedException, ExecutionException, UnsupportedEncodingException {
        SpeechConfig speechConfig = SpeechConfig.fromSubscription("None", "None");
        assertNotNull(speechConfig);
        speechConfig.setProperty("CARBON-INTERNAL-UseTtsEngine-Mock", "true");

        PushAudioOutputStreamTestCallback callback = new PushAudioOutputStreamTestCallback();
        PushAudioOutputStream stream = AudioOutputStream.createPushStream(callback);
        assertNotNull(stream);

        AudioConfig streamConfig = AudioConfig.fromStreamOutput(stream);
        assertNotNull(streamConfig);

        SpeechSynthesizer synthesizer = new SpeechSynthesizer(speechConfig, streamConfig);
        assertNotNull(synthesizer);

        byte[] expectedAudioData1 = buildMockSynthesizedAudio("{{{text1}}}", DEFAULT_LANGUAGE, DEFAULT_VOICE);
        byte[] expectedAudioData2 = buildMockSynthesizedAudio("{{{text2}}}", DEFAULT_LANGUAGE, DEFAULT_VOICE);
        byte[] expectedAudioData12 = mergeBinary(expectedAudioData1, expectedAudioData2);

        SpeechSynthesisResult result1 = synthesizer.SpeakTextAsync("{{{text1}}}").get(); // "{{{text1}}}" has completed rendering to pullstream
        assertTrue(areBinaryEqual(expectedAudioData1, callback.getAudioData()));

        SpeechSynthesisResult result2 = synthesizer.SpeakTextAsync("{{{text2}}}").get(); // "{{{text2}}}" has completed rendering to pullstream
        assertTrue(areBinaryEqual(expectedAudioData12, callback.getAudioData()));

        result1.close();
        result2.close();
        synthesizer.close();

        assertTrue(callback.isClosed());
    }

    @Ignore("TODO not supported by current CI")
    @Test
    public void testSynthesizerOutputToPullStreamUseAfterSynthesisCompletedMock() throws InterruptedException, ExecutionException, UnsupportedEncodingException {
        SpeechConfig speechConfig = SpeechConfig.fromSubscription("None", "None");
        assertNotNull(speechConfig);
        speechConfig.setProperty("CARBON-INTERNAL-UseTtsEngine-Mock", "true");

        PullAudioOutputStream stream = AudioOutputStream.createPullStream();
        assertNotNull(stream);

        AudioConfig streamConfig = AudioConfig.fromStreamOutput(stream);
        assertNotNull(streamConfig);

        SpeechSynthesizer synthesizer = new SpeechSynthesizer(speechConfig, streamConfig);
        assertNotNull(synthesizer);

        SpeechSynthesisResult result1 = synthesizer.SpeakTextAsync("{{{text1}}}").get(); // "{{{text1}}}" has completed rendering to pullstream
        SpeechSynthesisResult result2 = synthesizer.SpeakTextAsync("{{{text2}}}").get(); // "{{{text2}}}" has completed rendering to pullstream

        result1.close();
        result2.close();
        synthesizer.close();
        streamConfig.close();
        speechConfig.close();

        byte[] expectedAudioData1 = buildMockSynthesizedAudio("{{{text1}}}", DEFAULT_LANGUAGE, DEFAULT_VOICE);
        byte[] expectedAudioData2 = buildMockSynthesizedAudio("{{{text2}}}", DEFAULT_LANGUAGE, DEFAULT_VOICE);
        byte[] expectedAudioData12 = mergeBinary(expectedAudioData1, expectedAudioData2);

        DoSomethingWithAudioInPullStream(stream, expectedAudioData12);

        stream.close();
    }

    @Ignore("TODO not supported by current CI")
    @Test
    public void testSynthesizerOutputToPullStreamStartUsingBeforeDoneSynthesizingMock() throws InterruptedException, ExecutionException, UnsupportedEncodingException {
        SpeechConfig speechConfig = SpeechConfig.fromSubscription("None", "None");
        assertNotNull(speechConfig);
        speechConfig.setProperty("CARBON-INTERNAL-UseTtsEngine-Mock", "true");

        PullAudioOutputStream stream = AudioOutputStream.createPullStream();
        assertNotNull(stream);

        byte[] expectedAudioData1 = buildMockSynthesizedAudio("{{{text1}}}", DEFAULT_LANGUAGE, DEFAULT_VOICE);
        byte[] expectedAudioData2 = buildMockSynthesizedAudio("{{{text2}}}", DEFAULT_LANGUAGE, DEFAULT_VOICE);
        byte[] expectedAudioData12 = mergeBinary(expectedAudioData1, expectedAudioData2);

        Future<Void> future = DoSomethingWithAudioInPullStreamInBackground(stream, expectedAudioData12);

        AudioConfig streamConfig = AudioConfig.fromStreamOutput(stream);
        assertNotNull(streamConfig);

        SpeechSynthesizer synthesizer = new SpeechSynthesizer(speechConfig, streamConfig);
        assertNotNull(synthesizer);

        SpeechSynthesisResult result1 = synthesizer.SpeakTextAsync("{{{text1}}}").get(); // "{{{text1}}}" has completed rendering to pullstream
        SpeechSynthesisResult result2 = synthesizer.SpeakTextAsync("{{{text2}}}").get(); // "{{{text2}}}" has completed rendering to pullstream

        result1.close();
        result2.close();
        synthesizer.close();
        streamConfig.close();
        speechConfig.close();

        future.get();

        stream.close();
    }

    @Ignore("TODO not supported by current CI")
    @Test
    public void testSpeakOutInResultsMock() throws InterruptedException, ExecutionException, UnsupportedEncodingException {
        SpeechConfig speechConfig = SpeechConfig.fromSubscription("None", "None");
        assertNotNull(speechConfig);
        speechConfig.setProperty("CARBON-INTERNAL-UseTtsEngine-Mock", "true");

        SpeechSynthesizer synthesizer = new SpeechSynthesizer(speechConfig, null); // null indicates to do nothing with synthesizer audio by default
        assertNotNull(synthesizer);

        SpeechSynthesisResult result1 = synthesizer.SpeakTextAsync("{{{text1}}}").get(); // "{{{text1}}}" has completed rendering, and available in result1
        assertEquals(GUID_LENGTH, (Integer)result1.getResultId().length());
        assertEquals(ResultReason.SynthesizingAudioCompleted, result1.getReason());

        byte[] expectedAudioData1 = buildMockSynthesizedAudio("{{{text1}}}", DEFAULT_LANGUAGE, DEFAULT_VOICE);
        assertTrue(areBinaryEqual(expectedAudioData1, result1.getAudioData()));

        SpeechSynthesisResult result2 = synthesizer.SpeakTextAsync("{{{text2}}}").get(); // "{{{text2}}}" has completed rendering, and available in result2
        assertEquals(GUID_LENGTH, (Integer)result2.getResultId().length());
        assertEquals(ResultReason.SynthesizingAudioCompleted, result2.getReason());

        byte[] expectedAudioData2 = buildMockSynthesizedAudio("{{{text2}}}", DEFAULT_LANGUAGE, DEFAULT_VOICE);
        assertTrue(areBinaryEqual(expectedAudioData2, result2.getAudioData()));

        result1.close();
        result2.close();
        synthesizer.close();
        speechConfig.close();
    }

    @Ignore("TODO not supported by current CI")
    @Test
    public void testSpeakOutputInChunksInEventSynthesizingMock() throws InterruptedException, ExecutionException, UnsupportedEncodingException {
        SpeechConfig speechConfig = SpeechConfig.fromSubscription("None", "None");
        assertNotNull(speechConfig);
        speechConfig.setProperty("CARBON-INTERNAL-UseTtsEngine-Mock", "true");

        SpeechSynthesizer synthesizer = new SpeechSynthesizer(speechConfig, null); // null indicates to do nothing with synthesizer audio by default
        assertNotNull(synthesizer);

        byte[] expectedAudioData1 = buildMockSynthesizedAudio("{{{text1}}}", DEFAULT_LANGUAGE, DEFAULT_VOICE);
        byte[] expectedAudioData2 = buildMockSynthesizedAudio("{{{text2}}}", DEFAULT_LANGUAGE, DEFAULT_VOICE);
        final byte[] expectedAudioData12 = mergeBinary(expectedAudioData1, expectedAudioData2);

        final int[] offset = new int[1];
        offset[0] = 0;
        synthesizer.Synthesizing.addEventListener((o, e) -> {
            ResultReason resultReason = e.getResult().getReason();
            assertEquals(ResultReason.SynthesizingAudio, resultReason);

            int audioLength = (int)(e.getResult().getAudioLength());
            assertTrue(audioLength > 0);

            byte[] expectedAudioChunk = new byte[audioLength];
            System.arraycopy(expectedAudioData12, offset[0], expectedAudioChunk, 0, audioLength);
            assertTrue(areBinaryEqual(expectedAudioChunk, e.getResult().getAudioData()));

            offset[0] += audioLength;
        });

        SpeechSynthesisResult result1 = synthesizer.SpeakTextAsync("{{{text1}}}").get(); // "{{{text1}}}" has completed rendering
        SpeechSynthesisResult result2 = synthesizer.SpeakTextAsync("{{{text2}}}").get(); // "{{{text2}}}" has completed rendering

        result1.close();
        result2.close();
        synthesizer.close();
        speechConfig.close();
    }

    @Ignore("TODO not supported by current CI")
    @Test
    public void testSpeakOutputInStreamsMock() throws InterruptedException, ExecutionException, UnsupportedEncodingException {
        SpeechConfig speechConfig = SpeechConfig.fromSubscription("None", "None");
        assertNotNull(speechConfig);
        speechConfig.setProperty("CARBON-INTERNAL-UseTtsEngine-Mock", "true");

        SpeechSynthesizer synthesizer = new SpeechSynthesizer(speechConfig, null); // null indicates to do nothing with synthesizer audio by default
        assertNotNull(synthesizer);

        SpeechSynthesisResult result1 = synthesizer.SpeakTextAsync("{{{text1}}}").get(); // "{{{text1}}}" has completed rendering, and available in result1
        assertEquals(ResultReason.SynthesizingAudioCompleted, result1.getReason());
        assertTrue(result1.getAudioLength() > 0);

        AudioDataStream stream1 = AudioDataStream.fromResult(result1);
        byte[] expectedAudioData1 = buildMockSynthesizedAudio("{{{text1}}}", DEFAULT_LANGUAGE, DEFAULT_VOICE);
        DoSomethingWithAudioInDataStream(stream1, true, expectedAudioData1);

        SpeechSynthesisResult result2 = synthesizer.SpeakTextAsync("{{{text2}}}").get(); // "{{{text2}}}" has completed rendering, and available in result2
        assertEquals(ResultReason.SynthesizingAudioCompleted, result2.getReason());
        assertTrue(result2.getAudioLength() > 0);

        AudioDataStream stream2 = AudioDataStream.fromResult(result2);
        byte[] expectedAudioData2 = buildMockSynthesizedAudio("{{{text2}}}", DEFAULT_LANGUAGE, DEFAULT_VOICE);
        DoSomethingWithAudioInDataStream(stream2, true, expectedAudioData2);

        result1.close();
        result2.close();
        stream1.close();
        stream2.close();
        synthesizer.close();
        speechConfig.close();
    }

    @Ignore("TODO not supported by current CI")
    @Test
    public void testSpeakOutputInStreamsBeforeDoneFromEventSynthesisStartedMock() throws InterruptedException, ExecutionException, UnsupportedEncodingException {
        SpeechConfig speechConfig = SpeechConfig.fromSubscription("None", "None");
        assertNotNull(speechConfig);
        speechConfig.setProperty("CARBON-INTERNAL-UseTtsEngine-Mock", "true");

        SpeechSynthesizer synthesizer = new SpeechSynthesizer(speechConfig, null); // null indicates to do nothing with synthesizer audio by default
        assertNotNull(synthesizer);

        final ArrayList<byte[]> expectedAudioData = new ArrayList<byte[]>();
        expectedAudioData.add(buildMockSynthesizedAudio("{{{text1}}}", DEFAULT_LANGUAGE, DEFAULT_VOICE));
        expectedAudioData.add(buildMockSynthesizedAudio("{{{text2}}}", DEFAULT_LANGUAGE, DEFAULT_VOICE));
        expectedAudioData.add(buildMockSynthesizedAudio("{{{text3}}}", DEFAULT_LANGUAGE, DEFAULT_VOICE));

        final ArrayList<Future<Void>> backgroundThreads = new ArrayList<>();
        final ArrayList<AudioDataStream> streams = new ArrayList<>();
        final int[] requestOrder = new int[1];
        requestOrder[0] = -1;
        synthesizer.SynthesisStarted.addEventListener((o, e) -> {
            ResultReason resultReason = e.getResult().getReason();
            assertEquals(ResultReason.SynthesizingAudioStarted, resultReason);

            long audioLength = e.getResult().getAudioLength();
            assertEquals(0, audioLength);

            requestOrder[0]++;

            AudioDataStream stream = AudioDataStream.fromResult(e.getResult());
            streams.add(stream);
            backgroundThreads.add(DoSomethingWithAudioInDataStreamInBackground(stream, false, expectedAudioData.get(requestOrder[0])));
        });

        SpeechSynthesisResult result1 = synthesizer.SpeakTextAsync("{{{text1}}}").get(); // "{{{text1}}}" has completed rendering
        SpeechSynthesisResult result2 = synthesizer.SpeakTextAsync("{{{text2}}}").get(); // "{{{text2}}}" has completed rendering

        Future<SpeechSynthesisResult> future3 = synthesizer.SpeakTextAsync("{{{text3}}}"); // "{{{text3}}}" synthesis might have started
        SpeechSynthesisResult result3 = future3.get(); // "{{{text3}}}" synthesis has completed

        for (int i = 0; i < backgroundThreads.size(); ++i)
        {
            backgroundThreads.get(i).get();
        }

        for (int i = 0; i < streams.size(); ++i)
        {
            streams.get(i).close();
        }

        result1.close();
        result2.close();
        result3.close();
        synthesizer.close();
        speechConfig.close();
    }

    @Ignore("TODO not supported by current CI")
    @Test
    public void testSpeakOutputInStreamsBeforeDoneFromMethodStartSpeakingTextAsyncMock() throws InterruptedException, ExecutionException, UnsupportedEncodingException {
        SpeechConfig speechConfig = SpeechConfig.fromSubscription("None", "None");
        assertNotNull(speechConfig);
        speechConfig.setProperty("CARBON-INTERNAL-UseTtsEngine-Mock", "true");

        SpeechSynthesizer synthesizer = new SpeechSynthesizer(speechConfig, null); // null indicates to do nothing with synthesizer audio by default
        assertNotNull(synthesizer);

        SpeechSynthesisResult result1 = synthesizer.StartSpeakingTextAsync("{{{text1}}}").get(); // "{{{text1}}}" synthesis has started, likely not finished
        assertEquals(ResultReason.SynthesizingAudioStarted, result1.getReason());
        assertEquals(0, result1.getAudioLength());

        AudioDataStream stream1 = AudioDataStream.fromResult(result1); // of type AudioDataStream, does not block
        byte[] expectedAudioData1 = buildMockSynthesizedAudio("{{{text1}}}", DEFAULT_LANGUAGE, DEFAULT_VOICE);
        Future<Void> future1 = DoSomethingWithAudioInDataStreamInBackground(stream1, false, expectedAudioData1); // does not block, just spins a thread up

        SpeechSynthesisResult result2 = synthesizer.StartSpeakingTextAsync("{{{text2}}}").get(); // "{{{text2}}}" synthesis has started, likely not finished
        assertEquals(ResultReason.SynthesizingAudioStarted, result2.getReason());
        assertEquals(0, result2.getAudioLength());

        AudioDataStream stream2 = AudioDataStream.fromResult(result2); // of type AudioDataStream, does not block
        byte[] expectedAudioData2 = buildMockSynthesizedAudio("{{{text2}}}", DEFAULT_LANGUAGE, DEFAULT_VOICE);
        Future<Void> future2 = DoSomethingWithAudioInDataStreamInBackground(stream2, false, expectedAudioData2); // does not block, just spins a thread up

        future1.get();
        future2.get();

        result1.close();
        result2.close();

        stream1.close();
        stream2.close();

        synthesizer.close();
        speechConfig.close();
    }

    @Ignore("TODO not supported by current CI")
    @Test
    public void testSpeakOutputInStreamsBeforeDoneQueuedMock() throws InterruptedException, ExecutionException, UnsupportedEncodingException {
        SpeechConfig speechConfig = SpeechConfig.fromSubscription("None", "None");
        assertNotNull(speechConfig);
        speechConfig.setProperty("CARBON-INTERNAL-UseTtsEngine-Mock", "true");

        SpeechSynthesizer synthesizer = new SpeechSynthesizer(speechConfig, null); // null indicates to do nothing with synthesizer audio by default
        assertNotNull(synthesizer);

        final int[] startedRequests = new int[1];
        startedRequests[0] = 0;
        synthesizer.SynthesisStarted.addEventListener((o, e) -> {
            startedRequests[0]++;
        });

        final int[] completedRequests = new int[1];
        completedRequests[0] = 0;
        synthesizer.SynthesisCompleted.addEventListener((o, e) -> {
            completedRequests[0]++;
            if (completedRequests[0] == 1)
            {
                // This is to check the requests is queued
                // When one request is already completed, the other one is still not started
                assertEquals(1, startedRequests[0]);
            }
        });

        Future<SpeechSynthesisResult> futureResult1 = synthesizer.StartSpeakingTextAsync("{{{text1}}}"); // "{{{text1}}}" synthesis might have started, likely not finished
        Future<SpeechSynthesisResult> futureResult2 = synthesizer.StartSpeakingTextAsync("{{{text2}}}"); // "{{{text2}}}" synthesis might have started (very unlikely)

        byte[] expectedAudioData1 = buildMockSynthesizedAudio("{{{text1}}}", DEFAULT_LANGUAGE, DEFAULT_VOICE);
        Future<Void> future1 = DoSomethingWithAudioInResultInBackground(futureResult1, false, expectedAudioData1); // does not block, just spins a thread up

        byte[] expectedAudioData2 = buildMockSynthesizedAudio("{{{text2}}}", DEFAULT_LANGUAGE, DEFAULT_VOICE);
        Future<Void> future2 = DoSomethingWithAudioInResultInBackground(futureResult2, false, expectedAudioData2); // does not block, just spins a thread up

        future1.get();
        future2.get();

        synthesizer.close();
        speechConfig.close();
    }

    @Ignore("TODO not supported by current CI")
    @Test
    public void testSpeakOutputInStreamsWithAllDataGetOnSynthesisStartedResultMock() throws InterruptedException, ExecutionException, UnsupportedEncodingException {
        SpeechConfig speechConfig = SpeechConfig.fromSubscription("None", "None");
        assertNotNull(speechConfig);
        speechConfig.setProperty("CARBON-INTERNAL-UseTtsEngine-Mock", "true");

        SpeechSynthesizer synthesizer = new SpeechSynthesizer(speechConfig, null); // null indicates to do nothing with synthesizer audio by default
        assertNotNull(synthesizer);

        SpeechSynthesisResult result = synthesizer.StartSpeakingTextAsync("{{{text1}}}").get(); // "{{{text1}}}" synthesis has started, likely not finished
        assertEquals(ResultReason.SynthesizingAudioStarted, result.getReason());
        assertEquals(0, result.getAudioLength());

        final boolean[] SynthesisCompleted = new boolean[1];
        SynthesisCompleted[0] = false;

        synthesizer.SynthesisCompleted.addEventListener((o, e) -> {
            SynthesisCompleted[0] = true;
        });

        while (!SynthesisCompleted[0]) {
            Thread.sleep(100); // wait for the synthesis to be done
        }

        AudioDataStream stream = AudioDataStream.fromResult(result); // now get the stream from result
        Thread.sleep(500); // wait for more time for synthesis completed event to be sent to stream
        byte[] expectedAudioData = buildMockSynthesizedAudio("{{{text1}}}", DEFAULT_LANGUAGE, DEFAULT_VOICE);
        DoSomethingWithAudioInDataStream(stream, true, expectedAudioData);

        stream.close();
        result.close();
        synthesizer.close();
        speechConfig.close();
    }

    @Ignore("TODO not supported by current CI")
    @Test
    public void testSpeakOutputInStreamsWithAllDataGetSinceSynthesizingResultMock() throws InterruptedException, ExecutionException, UnsupportedEncodingException {
        SpeechConfig speechConfig = SpeechConfig.fromSubscription("None", "None");
        assertNotNull(speechConfig);
        speechConfig.setProperty("CARBON-INTERNAL-UseTtsEngine-Mock", "true");

        SpeechSynthesizer synthesizer = new SpeechSynthesizer(speechConfig, null); // null indicates to do nothing with synthesizer audio by default
        assertNotNull(synthesizer);

        final boolean[] firstChunkSkipped = new boolean[1];
        firstChunkSkipped[0] = false;
        final AudioDataStream[] stream = new AudioDataStream[1];
        stream[0] = null;

        synthesizer.Synthesizing.addEventListener((o, e) -> {
            if (firstChunkSkipped[0] && stream[0] == null)
            {
                stream[0] = AudioDataStream.fromResult(e.getResult());
            }

            firstChunkSkipped[0] = true;
        });

        SpeechSynthesisResult result = synthesizer.SpeakTextAsync("{{{text1}}}").get();
        assertEquals(ResultReason.SynthesizingAudioCompleted, result.getReason());
        assertEquals(StreamStatus.PartialData, stream[0].getStatus());

        byte[] fullAudioData = buildMockSynthesizedAudio("{{{text1}}}", DEFAULT_LANGUAGE, DEFAULT_VOICE);
        byte[] expectedAudioData = new byte[fullAudioData.length - MOCK_AUDIO_CHUNK_SIZE];
        System.arraycopy(fullAudioData, MOCK_AUDIO_CHUNK_SIZE, expectedAudioData, 0, expectedAudioData.length);
        CheckAudioInDataStream(stream[0], expectedAudioData);

        stream[0].close();
        result.close();
        synthesizer.close();
        speechConfig.close();
    }

    private void DoSomethingWithAudioInPullStream(PullAudioOutputStream stream) {
        DoSomethingWithAudioInPullStream(stream, null);
    }

    private void DoSomethingWithAudioInPullStream(PullAudioOutputStream stream, byte[] expectedData) {
        byte[] audioData = new byte[0];
        byte buffer[] = new byte[1024];
        int totalSize = 0;
        int filledSize = 0;

        while ((filledSize = (int)stream.read(buffer)) > 0) { // blocks until atleast 1024 bytes are available
            // do something with buffer
            totalSize += filledSize;

            byte[] audioChunkData = new byte[filledSize];
            System.arraycopy(buffer, 0, audioChunkData, 0, filledSize);
            audioData = mergeBinary(audioData, audioChunkData);
        }

        assertTrue(totalSize > 0);

        if (expectedData != null)
        {
            assertTrue(areBinaryEqual(expectedData, audioData));
        }
    }

    private Future<Void> DoSomethingWithAudioInPullStreamInBackground(PullAudioOutputStream stream) {
        return DoSomethingWithAudioInPullStreamInBackground(stream, null);
    }

    private Future<Void> DoSomethingWithAudioInPullStreamInBackground(PullAudioOutputStream stream, byte[] expectedData) {
        final SpeechSynthesizerTests thisTest = this;
        final PullAudioOutputStream finalStream = stream;
        final byte[] finalExpectedData = expectedData;

        return s_executorService.submit(new java.util.concurrent.Callable<Void>() {
            public Void  call() {
                Runnable runnable = new Runnable() { public void run() { thisTest.DoSomethingWithAudioInPullStream(finalStream, finalExpectedData); }};
                runnable.run();
                return null;
            }});
    }

    private void DoSomethingWithAudioInVector(byte[] audioData, long audioLength) {
        assertTrue(audioData.length > 0);
        assertEquals(audioLength, audioData.length);
    }

    private void DoSomethingWithAudioInDataStream(AudioDataStream stream, boolean afterSynthesisDone)
    {
        DoSomethingWithAudioInDataStream(stream, afterSynthesisDone, null);
    }

    private void DoSomethingWithAudioInDataStream(AudioDataStream stream, boolean afterSynthesisDone, byte[] expectedData)
    {
        if (afterSynthesisDone)
        {
            assertEquals(StreamStatus.AllData, stream.getStatus());
        }

        CheckAudioInDataStream(stream, expectedData);

        if (!afterSynthesisDone)
        {
            assertEquals(StreamStatus.AllData, stream.getStatus());
        }
    }

    private void CheckAudioInDataStream(AudioDataStream stream, byte[] expectedData)
    {
        byte[] audioData = new byte[0];
        byte[] buffer = new byte[1024];
        int totalSize = 0;
        int filledSize = 0;

        while ((filledSize = (int)stream.readData(buffer)) > 0) { // blocks until atleast 1024 bytes are available
            // do something with buffer
            totalSize += (int)filledSize;

            byte[] audioChunkData = new byte[filledSize];
            System.arraycopy(buffer, 0, audioChunkData, 0, filledSize);
            audioData = mergeBinary(audioData, audioChunkData);
        }

        if (expectedData != null)
        {
            assertTrue(areBinaryEqual(expectedData, audioData));
        }

        if (stream.getStatus() == StreamStatus.Canceled) {
            SpeechSynthesisCancellationDetails cancelDetails = SpeechSynthesisCancellationDetails.fromStream(stream);
            // do something with cancellation details
            assertEquals(CancellationReason.Error, cancelDetails.getReason());
            assertEquals(CancellationErrorCode.NoError, cancelDetails.getErrorCode());
        }
        else
        {
            assertTrue(totalSize > 0);
        }
    }

    private Future<Void> DoSomethingWithAudioInDataStreamInBackground(AudioDataStream stream, boolean afterSynthesisDone)
    {
        return DoSomethingWithAudioInDataStreamInBackground(stream, afterSynthesisDone, null);
    }

    private Future<Void> DoSomethingWithAudioInDataStreamInBackground(AudioDataStream stream, boolean afterSynthesisDone, byte[] expectedData)
    {
        final SpeechSynthesizerTests thisTest = this;
        final AudioDataStream finalStream = stream;
        final boolean finalAfterSynthesisDone = afterSynthesisDone;
        final byte[] finalExpectedData = expectedData;

        return s_executorService.submit(new java.util.concurrent.Callable<Void>() {
            public Void  call() {
                Runnable runnable = new Runnable() { public void run() { thisTest.DoSomethingWithAudioInDataStream(finalStream, finalAfterSynthesisDone, finalExpectedData); }};
                runnable.run();
                return null;
            }});
    }

    private Future<Void> DoSomethingWithAudioInResultInBackground(Future<SpeechSynthesisResult> futureResult, boolean afterSynthesisDone)
    {
        return DoSomethingWithAudioInResultInBackground(futureResult, afterSynthesisDone, null);
    }

    private Future<Void> DoSomethingWithAudioInResultInBackground(Future<SpeechSynthesisResult> futureResult, boolean afterSynthesisDone, byte[] expectedData)
    {
        final SpeechSynthesizerTests thisTest = this;
        final Future<SpeechSynthesisResult> finalFutureResult = futureResult;
        final boolean finalAfterSynthesisDone = afterSynthesisDone;
        final byte[] finalExpectedData = expectedData;

        return s_executorService.submit(new java.util.concurrent.Callable<Void>() {
            public Void  call() {
                Runnable runnable = new Runnable() { public void run() {
                    try
                    {
                        SpeechSynthesisResult result = finalFutureResult.get();
                        AudioDataStream stream = AudioDataStream.fromResult(result);
                        thisTest.DoSomethingWithAudioInDataStream(stream, finalAfterSynthesisDone, finalExpectedData);
                        result.close();
                        stream.close();
                    }
                    catch(Exception e)
                    {
                    }
                }};
                runnable.run();
                return null;
            }});
    }

    boolean areBinaryEqual(byte[] expectedData, byte[] actualData)
    {
        if (expectedData == null || actualData == null || expectedData.length != actualData.length)
        {
            return false;
        }

        for (int i = 0; i < expectedData.length; ++i)
        {
            int error = expectedData[i] > actualData[i] ? expectedData[i] - actualData[i] : actualData[i] - expectedData[i];
            if (error > BINARY_COMPARE_TOLERANCE) // Accept some tolerance for some platforms
            {
                return false;
            }
        }

        return true;
    }

    byte[] buildMockSynthesizedAudio(String text, String language, String voice) throws UnsupportedEncodingException
    {
        String ssml = buildSsml(text, language, voice);
        byte[] ssmlBytes = ssml.getBytes("UTF-8");
        byte[] audioData = new byte[MOCK_AUDIO_SIZE + ssml.length()];

        for (int i = 0; i < MOCK_AUDIO_SIZE / 2; ++i)
        {
            double x = (double)i * 3.14159265359 * 2 * 400 / 16000; // 400Hz
            double y = Math.sin(x);
            short sampleValue = (short)(y * 16384);
            int unsignedSampleValue = sampleValue < 0 ? sampleValue + 65536 : sampleValue;
            audioData[i * 2] = (byte)(unsignedSampleValue % 256);
            audioData[i * 2 + 1] = (byte)(unsignedSampleValue / 256);
        }

        for (int i = 0; i < ssmlBytes.length; ++i)
        {
            audioData[MOCK_AUDIO_SIZE + i] = ssmlBytes[i];
        }

        return audioData;
    }

    byte[] mergeBinary(byte[] binary1, byte[] binary2)
    {
        byte[] mergedBinary = new byte[binary1.length + binary2.length];
        System.arraycopy(binary1, 0, mergedBinary, 0, binary1.length);
        System.arraycopy(binary2, 0, mergedBinary, binary1.length, binary2.length);
        return mergedBinary;
    }

    byte[] loadWaveFileData(String waveFile) throws FileNotFoundException, IOException
    {
        File file = new File(waveFile);
        if (file.length() < EMPTY_WAVE_FILE_SIZE)
        {
            return new byte[0];
        }

        FileInputStream fileStream = new FileInputStream(waveFile);
        DataInputStream dataStream = new DataInputStream(fileStream);
        byte[] waveHeader = new byte[EMPTY_WAVE_FILE_SIZE];
        byte[] waveData = new byte[(int)(file.length()) - EMPTY_WAVE_FILE_SIZE];
        dataStream.read(waveHeader);
        dataStream.read(waveData);
        return waveData;
    }

    String buildSsml(String text, String language, String voice)
    {
        return String.format(SSML_TEMPLATE, language, voice, xmlEncode(text));
    }

    String xmlEncode(String text)
    {
        String ssml = new String();
        char[] chars = text.toCharArray();
        for (int i = 0; i < chars.length; ++i)
        {
            if (chars[i] == '&')
            {
                ssml += "&amp;";
            }
            else if (chars[i] == '<')
            {
                ssml += "&lt;";
            }
            else if (chars[i] == '>')
            {
                ssml += "&gt;";
            }
            else if (chars[i] == '\'')
            {
                ssml += "&apos;";
            }
            else if (chars[i] == '"')
            {
                ssml += "&quot;";
            }
            else
            {
                ssml += chars[i];
            }
        }

        return ssml;
    }

    static final class PushAudioOutputStreamTestCallback extends PushAudioOutputStreamCallback
    {
        public PushAudioOutputStreamTestCallback() {
            this._audioData = new byte[0];
            this._closed = false;
        }

        @Override
        public int write(byte[] dataBuffer)
        {
            byte[] updatedAudioData = new byte[_audioData.length + dataBuffer.length];
            System.arraycopy(_audioData, 0, updatedAudioData, 0, _audioData.length);
            System.arraycopy(dataBuffer, 0, updatedAudioData, _audioData.length, dataBuffer.length);

            _audioData = updatedAudioData;
            return dataBuffer.length;
        }

        @Override
        public void close()
        {
            _closed = true;
        }

        public int getAudioSize()
        {
            return _audioData.length;
        }

        public byte[] getAudioData()
        {
            return _audioData;
        }

        public boolean isClosed()
        {
            return _closed;
        }

        private byte[] _audioData;
        boolean _closed;
    }
}
