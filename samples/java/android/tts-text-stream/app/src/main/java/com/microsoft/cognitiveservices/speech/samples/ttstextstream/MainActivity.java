//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.samples.ttstextstream;

import static android.Manifest.permission.INTERNET;

import android.media.AudioAttributes;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.ScrollView;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;

import com.microsoft.cognitiveservices.speech.AudioDataStream;
import com.microsoft.cognitiveservices.speech.Connection;
import com.microsoft.cognitiveservices.speech.PropertyId;
import com.microsoft.cognitiveservices.speech.ResultReason;
import com.microsoft.cognitiveservices.speech.SpeechConfig;
import com.microsoft.cognitiveservices.speech.SpeechSynthesisCancellationDetails;
import com.microsoft.cognitiveservices.speech.SpeechSynthesisOutputFormat;
import com.microsoft.cognitiveservices.speech.SpeechSynthesisRequest;
import com.microsoft.cognitiveservices.speech.SpeechSynthesisRequest.SpeechSynthesisRequestInputType;
import com.microsoft.cognitiveservices.speech.SpeechSynthesisResult;
import com.microsoft.cognitiveservices.speech.SpeechSynthesizer;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.net.URI;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;

public class MainActivity extends AppCompatActivity {

    private static final String SPEECH_KEY = "YourSubscriptionKey";
    private static final String SPEECH_REGION = "YourServiceRegion";
    private static final String SPEECH_VOICE = "en-us-Ava:DragonHDLatestNeural";
    private static final int SAMPLE_RATE = 16000;
    private static final int CHANNEL_COUNT = 1;
    private static final int BITS_PER_SAMPLE = 16;
    private static final String[] TEXT_CHUNKS = {
        "Hello! ",
        "Welcome to the Azure ",
        "Speech SDK text stream sample. ",
        "This text is sent ",
        "in small chunks, ",
        "so speech starts ",
        "before the whole text ",
        "is available."
    };

    private TextView outputMessage;
    private TextView contentTextView;
    private Button startButton;
    private Button stopButton;

    private final ExecutorService executorService = Executors.newSingleThreadExecutor();
    private volatile Future<?> currentTask;
    private volatile boolean stopRequested;

    private volatile StreamingSession activeSession;

    private static final class StreamingSession {
        final SpeechSynthesisRequest request;
        final SpeechSynthesizer synthesizer;
        final AudioTrack audioTrack;

        StreamingSession(SpeechSynthesisRequest request, SpeechSynthesizer synthesizer, AudioTrack audioTrack) {
            this.request = request;
            this.synthesizer = synthesizer;
            this.audioTrack = audioTrack;
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        if (getSupportActionBar() != null) {
            getSupportActionBar().hide();
        }

        ActivityCompat.requestPermissions(this, new String[] { INTERNET }, 5);

        outputMessage = findViewById(R.id.outputMessage);
        contentTextView = findViewById(R.id.tv_content);
        startButton = findViewById(R.id.button_start_stream);
        stopButton = findViewById(R.id.button_stop_stream);

        startButton.setOnClickListener(v -> startDemo());
        stopButton.setOnClickListener(v -> stopDemo());
    }

    @Override
    protected void onDestroy() {
        stopDemo();
        executorService.shutdownNow();
        super.onDestroy();
    }

    private void startDemo() {
        clearOutput();
        if (SPEECH_KEY.startsWith("Your") || SPEECH_REGION.startsWith("Your")) {
            appendLog("Update SPEECH_KEY and SPEECH_REGION in MainActivity before running the sample.");
            return;
        }

        stopRequested = false;
        setRunningState(true);
        currentTask = executorService.submit(() -> {
            try {
                runTextStreamingDemo();
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
                appendLog("Streaming stopped.");
            } catch (Exception e) {
                appendLog("Error: " + e.getMessage());
            } finally {
                runOnUiThread(() -> setRunningState(false));
            }
        });
    }

    private void stopDemo() {
        stopRequested = true;

        StreamingSession session = activeSession;
        if (session != null) {
            session.request.getInputStream().close();
            session.synthesizer.StopSpeakingAsync();
            session.audioTrack.pause();
            session.audioTrack.flush();
        }

        if (currentTask != null) {
            currentTask.cancel(true);
        }

        runOnUiThread(() -> setRunningState(false));
    }

    private void runTextStreamingDemo() throws Exception {
        File logFile = new File(System.getProperty("java.io.tmpdir"), "log.log");
        File wavFile = new File(logFile.getParentFile(), "streaming_output.wav");
        String endpoint = "wss://" + SPEECH_REGION + ".tts.speech.microsoft.com/cognitiveservices/websocket/v2";
        ByteArrayOutputStream audioBuffer = new ByteArrayOutputStream();

        appendLog("Preparing text streaming ...");
        appendLog("Text Streaming Demo");
        appendLog("SDK log file: " + logFile.getAbsolutePath());
        appendLog("Using endpoint: " + endpoint);

        SpeechConfig speechConfig = SpeechConfig.fromEndpoint(new URI(endpoint), SPEECH_KEY);
        speechConfig.setSpeechSynthesisOutputFormat(SpeechSynthesisOutputFormat.Raw16Khz16BitMonoPcm);
        speechConfig.setSpeechSynthesisVoiceName(SPEECH_VOICE);
        speechConfig.setProperty(PropertyId.SpeechServiceConnection_SynthVoice, SPEECH_VOICE);
        speechConfig.setProperty(PropertyId.SpeechSynthesis_FrameTimeoutInterval, "10000");
        speechConfig.setProperty(PropertyId.SpeechSynthesis_RtfTimeoutThreshold, "10");
        speechConfig.setProperty(PropertyId.Speech_LogFilename, System.getProperty("java.io.tmpdir") + "/log.log");

        AudioTrack audioTrack = createAudioTrack();
        audioTrack.play();

        SpeechSynthesizer synthesizer = new SpeechSynthesizer(speechConfig, null);
        Connection connection = Connection.fromSpeechSynthesizer(synthesizer);
        SpeechSynthesisRequest request = new SpeechSynthesisRequest(SpeechSynthesisRequestInputType.TextStream);
        activeSession = new StreamingSession(request, synthesizer, audioTrack);
        SpeechSynthesisResult result = null;
        AudioDataStream audioDataStream = null;
        Thread audioThread = null;

        try {
            connection.connected.addEventListener((o, e) -> appendLog("Connection established."));
            connection.disconnected.addEventListener((o, e) -> appendLog("Disconnected."));
            synthesizer.SynthesisStarted.addEventListener((o, e) -> {
                appendLog("Synthesis started. ResultId: " + e.getResult().getResultId());
                e.close();
            });
            synthesizer.SynthesisCompleted.addEventListener((o, e) -> {
                appendLog("Synthesis completed.");
                e.close();
            });
            synthesizer.SynthesisCanceled.addEventListener((o, e) -> {
                appendLog("Synthesis canceled: " + SpeechSynthesisCancellationDetails.fromResult(e.getResult()));
                e.close();
            });
            synthesizer.Synthesizing.addEventListener((o, e) -> {
                byte[] data = e.getResult().getAudioData();
                int len = data != null ? data.length : 0;
                appendLog("Synthesizing event received " + len + " bytes audio data.");
                e.close();
            });

            appendLog("Starting speech synthesis...");
            Future<SpeechSynthesisResult> task = synthesizer.StartSpeakingRequestAsync(request);
            result = task.get();
            audioDataStream = AudioDataStream.fromResult(result);

            AudioDataStream stream = audioDataStream;
            audioThread = new Thread(() -> {
                byte[] buffer = new byte[4096];

                try {
                    while (!stopRequested) {
                        long readBytes = stream.readData(buffer);
                        if (readBytes == 0) {
                            break;
                        }

                        appendLog("[audio: " + readBytes + " bytes]");
                        audioBuffer.write(buffer, 0, (int) readBytes);

                        int offset = 0;
                        while (offset < readBytes && !stopRequested) {
                            int written = audioTrack.write(buffer, offset, (int) readBytes - offset, AudioTrack.WRITE_BLOCKING);
                            if (written <= 0) {
                                appendLog("AudioTrack write returned " + written + ".");
                                return;
                            }
                            offset += written;
                        }
                    }
                } catch (Exception e) {
                    appendLog("Error reading audio stream: " + e.getMessage());
                }
            }, "tts-audio-reader");
            audioThread.start();

            for (String chunk : TEXT_CHUNKS) {
                if (stopRequested) {
                    break;
                }
                appendContent(chunk);
                appendLog("[text: " + chunk.trim() + "]");
                request.getInputStream().write(chunk);
                Thread.sleep(200L);
            }

            request.getInputStream().close();
            appendLog("[TEXT STREAM END]");

            if (audioThread != null) {
                audioThread.join();
            }

            if (result != null && result.getReason() == ResultReason.Canceled) {
                appendLog("Synthesis canceled: " + SpeechSynthesisCancellationDetails.fromResult(result));
            } else if (result != null && result.getReason() == ResultReason.SynthesizingAudioCompleted) {
                appendLog("Synthesis audio stream finished.");
            }

            byte[] pcmData = audioBuffer.toByteArray();
            saveAsWav(wavFile, pcmData);
            appendLog("[AUDIO STREAM END] Total: " + pcmData.length + " bytes");
            appendLog("Audio saved to: " + wavFile.getAbsolutePath());
            appendLog("Total audio bytes: " + pcmData.length);
        } finally {
            activeSession = null;
            currentTask = null;

            try {
                if (audioThread != null && audioThread.isAlive()) {
                    audioThread.interrupt();
                    audioThread.join(1000L);
                }
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
            }
            if (audioDataStream != null) {
                audioDataStream.close();
            }
            if (result != null) {
                result.close();
            }
            connection.close();
            synthesizer.close();
            speechConfig.close();
            audioTrack.stop();
            audioTrack.release();
            audioBuffer.close();
        }
    }

    private void clearOutput() {
        runOnUiThread(() -> {
            contentTextView.setText("");
            outputMessage.setText("");
        });
    }

    private AudioTrack createAudioTrack() {
        return new AudioTrack.Builder()
            .setAudioAttributes(new AudioAttributes.Builder()
                .setUsage(AudioAttributes.USAGE_ASSISTANT)
                .setContentType(AudioAttributes.CONTENT_TYPE_SPEECH)
                .build())
            .setAudioFormat(new AudioFormat.Builder()
                .setEncoding(AudioFormat.ENCODING_PCM_16BIT)
                .setSampleRate(SAMPLE_RATE)
                .setChannelMask(AudioFormat.CHANNEL_OUT_MONO)
                .build())
            .setBufferSizeInBytes(AudioTrack.getMinBufferSize(
                SAMPLE_RATE,
                AudioFormat.CHANNEL_OUT_MONO,
                AudioFormat.ENCODING_PCM_16BIT) * 2)
            .setTransferMode(AudioTrack.MODE_STREAM)
            .build();
    }

    private void saveAsWav(File file, byte[] pcmData) throws Exception {
        int byteRate = SAMPLE_RATE * CHANNEL_COUNT * BITS_PER_SAMPLE / 8;
        int blockAlign = CHANNEL_COUNT * BITS_PER_SAMPLE / 8;
        int dataSize = pcmData.length;
        int chunkSize = 36 + dataSize;

        try (FileOutputStream output = new FileOutputStream(file)) {
            output.write("RIFF".getBytes());
            output.write(intToLittleEndian(chunkSize));
            output.write("WAVE".getBytes());
            output.write("fmt ".getBytes());
            output.write(intToLittleEndian(16));
            output.write(shortToLittleEndian((short) 1));
            output.write(shortToLittleEndian((short) CHANNEL_COUNT));
            output.write(intToLittleEndian(SAMPLE_RATE));
            output.write(intToLittleEndian(byteRate));
            output.write(shortToLittleEndian((short) blockAlign));
            output.write(shortToLittleEndian((short) BITS_PER_SAMPLE));
            output.write("data".getBytes());
            output.write(intToLittleEndian(dataSize));
            output.write(pcmData);
        }
    }

    private byte[] intToLittleEndian(int value) {
        return new byte[] {
            (byte) (value & 0xFF),
            (byte) ((value >> 8) & 0xFF),
            (byte) ((value >> 16) & 0xFF),
            (byte) ((value >> 24) & 0xFF)
        };
    }

    private byte[] shortToLittleEndian(short value) {
        return new byte[] {
            (byte) (value & 0xFF),
            (byte) ((value >> 8) & 0xFF)
        };
    }

    private void appendContent(String text) {
        runOnUiThread(() -> {
            contentTextView.append(text);
            autoScroll(contentTextView);
        });
    }

    private void appendLog(String text) {
        runOnUiThread(() -> {
            if (outputMessage.length() > 0) {
                outputMessage.append("\n");
            }
            outputMessage.append(text);
            autoScroll(outputMessage);
        });
    }

    private void autoScroll(TextView view) {
        if (view.getParent() instanceof ScrollView) {
            ScrollView scrollView = (ScrollView) view.getParent();
            scrollView.post(() -> scrollView.fullScroll(View.FOCUS_DOWN));
        }
    }

    private void setRunningState(boolean running) {
        startButton.setEnabled(!running);
        stopButton.setEnabled(running);
    }
}
