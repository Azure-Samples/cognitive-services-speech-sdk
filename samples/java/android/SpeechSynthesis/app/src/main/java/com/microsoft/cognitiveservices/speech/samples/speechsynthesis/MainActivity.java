//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.samples.speechsynthesis;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;

import android.graphics.Color;
import android.media.AudioAttributes;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.os.Bundle;
import android.text.Spannable;
import android.text.method.ScrollingMovementMethod;
import android.text.style.ForegroundColorSpan;
import android.util.Log;
import android.view.View;
import android.widget.EditText;
import android.widget.TextView;

import com.microsoft.cognitiveservices.speech.AudioDataStream;
import com.microsoft.cognitiveservices.speech.Connection;
import com.microsoft.cognitiveservices.speech.PropertyId;
import com.microsoft.cognitiveservices.speech.SpeechConfig;
import com.microsoft.cognitiveservices.speech.SpeechSynthesisCancellationDetails;
import com.microsoft.cognitiveservices.speech.SpeechSynthesisOutputFormat;
import com.microsoft.cognitiveservices.speech.SpeechSynthesisResult;
import com.microsoft.cognitiveservices.speech.SpeechSynthesizer;

import java.util.Locale;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import static android.Manifest.permission.INTERNET;

public class MainActivity extends AppCompatActivity {

    // Replace below with your own subscription key
    private static String speechSubscriptionKey = "YourSubscriptionKey";
    // Replace below with your own service region (e.g., "westus").
    private static String serviceRegion = "YourServiceRegion";

    private SpeechConfig speechConfig;
    private SpeechSynthesizer synthesizer;
    private Connection connection;
    private AudioTrack audioTrack;

    private TextView outputMessage;

    private SpeakingRunnable speakingRunnable;
    private ExecutorService singleThreadExecutor;
    private final Object synchronizedObj = new Object();
    private boolean stopped = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Note: we need to request the permissions
        int requestCode = 5; // Unique code for the permission request
        ActivityCompat.requestPermissions(MainActivity.this, new String[]{INTERNET}, requestCode);

        singleThreadExecutor = Executors.newSingleThreadExecutor();
        speakingRunnable = new SpeakingRunnable();

        outputMessage = this.findViewById(R.id.outputMessage);
        outputMessage.setMovementMethod(new ScrollingMovementMethod());

        audioTrack = new AudioTrack(
                new AudioAttributes.Builder()
                        .setUsage(AudioAttributes.USAGE_MEDIA)
                        .setContentType(AudioAttributes.CONTENT_TYPE_SPEECH)
                        .build(),
                new AudioFormat.Builder()
                        .setEncoding(AudioFormat.ENCODING_PCM_16BIT)
                        .setSampleRate(24000)
                        .setChannelMask(AudioFormat.CHANNEL_OUT_MONO)
                        .build(),
                AudioTrack.getMinBufferSize(
                        24000,
                        AudioFormat.CHANNEL_OUT_MONO,
                        AudioFormat.ENCODING_PCM_16BIT) * 2,
                AudioTrack.MODE_STREAM,
                AudioManager.AUDIO_SESSION_ID_GENERATE);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();

        // Release speech synthesizer and its dependencies
        if (synthesizer != null) {
            synthesizer.close();
            connection.close();
        }
        if (speechConfig != null) {
            speechConfig.close();
        }

        if (audioTrack != null) {
            singleThreadExecutor.shutdownNow();
            audioTrack.flush();
            audioTrack.stop();
            audioTrack.release();
        }
    }

    public void onCreateSynthesizerButtonClicked(View v) {
        if (synthesizer != null) {
            speechConfig.close();
            synthesizer.close();
            connection.close();
        }

        // Reuse the synthesizer to lower the latency.
        // I.e. create one synthesizer and speak many times using it.
        clearOutputMessage();
        updateOutputMessage("Initializing synthesizer...\n");

        speechConfig = SpeechConfig.fromSubscription(speechSubscriptionKey, serviceRegion);
        // Use 24k Hz format for higher quality.
        speechConfig.setSpeechSynthesisOutputFormat(SpeechSynthesisOutputFormat.Raw24Khz16BitMonoPcm);
        // Set voice name.
        speechConfig.setSpeechSynthesisVoiceName("en-US-AvaMultilingualNeural");
        synthesizer = new SpeechSynthesizer(speechConfig, null);
        connection = Connection.fromSpeechSynthesizer(synthesizer);

        Locale current = getResources().getConfiguration().locale;

        connection.connected.addEventListener((o, e) -> {
            updateOutputMessage("Connection established.\n");
        });

        connection.disconnected.addEventListener((o, e) -> {
            updateOutputMessage("Disconnected.\n");
        });

        synthesizer.SynthesisStarted.addEventListener((o, e) -> {
            updateOutputMessage(String.format(current,
                "Synthesis started. Result Id: %s.\n",
                e.getResult().getResultId()));
            e.close();
        });

        synthesizer.Synthesizing.addEventListener((o, e) -> {
            updateOutputMessage(String.format(current,
                "Synthesizing. received %d bytes.\n",
                e.getResult().getAudioLength()));
            e.close();
        });

        synthesizer.SynthesisCompleted.addEventListener((o, e) -> {
            updateOutputMessage("Synthesis finished.\n");
            updateOutputMessage("\tFirst byte latency: " + e.getResult().getProperties().getProperty(PropertyId.SpeechServiceResponse_SynthesisFirstByteLatencyMs) + " ms.\n");
            updateOutputMessage("\tFinish latency: " + e.getResult().getProperties().getProperty(PropertyId.SpeechServiceResponse_SynthesisFinishLatencyMs) + " ms.\n");
            e.close();
        });

        synthesizer.SynthesisCanceled.addEventListener((o, e) -> {
            String cancellationDetails =
                    SpeechSynthesisCancellationDetails.fromResult(e.getResult()).toString();
            updateOutputMessage("Error synthesizing. Result ID: " + e.getResult().getResultId() +
                    ". Error detail: " + System.lineSeparator() + cancellationDetails +
                    System.lineSeparator() + "Did you update the subscription info?\n",
                true, true);
            e.close();
        });

        synthesizer.WordBoundary.addEventListener((o, e) -> {
            updateOutputMessage(String.format(current,
                "Word boundary. Text offset %d, length %d; audio offset %d ms.\n",
                e.getTextOffset(),
                e.getWordLength(),
                e.getAudioOffset() / 10000));

        });
    }

    public void onPreConnectButtonClicked(View v) {
        // This method could pre-establish the connection to service to lower the latency
        // This method is useful when you want to synthesize audio in a short time, but the text is
        // not available. E.g. for speech bot, you can warm up the TTS connection when the user is speaking;
        // then call speak() when dialogue utterance is ready.
        if (connection == null) {
            updateOutputMessage("Please initialize the speech synthesizer first\n", true, true);
            return;
        }
        connection.openConnection(true);
        updateOutputMessage("Opening connection.\n");
    }

    public void onSpeechButtonClicked(View v) {
        clearOutputMessage();

        if (synthesizer == null) {
            updateOutputMessage("Please initialize the speech synthesizer first\n", true, true);
            return;
        }

        EditText speakText = this.findViewById(R.id.speakText);

        speakingRunnable.setContent(speakText.getText().toString());
        singleThreadExecutor.execute(speakingRunnable);
    }

    public void onStopButtonClicked(View v) {
        if (synthesizer == null) {
            updateOutputMessage("Please initialize the speech synthesizer first\n", true, true);
            return;
        }

        stopSynthesizing();
    }

    class SpeakingRunnable implements Runnable {
        private String content;

        public void setContent(String content) {
            this.content = content;
        }

        @Override
        public void run() {
            try {
                audioTrack.play();
                synchronized (synchronizedObj) {
                    stopped = false;
                }

                SpeechSynthesisResult result = synthesizer.StartSpeakingTextAsync(content).get();
                AudioDataStream audioDataStream = AudioDataStream.fromResult(result);

                // Set the chunk size to 50 ms. 24000 * 16 * 0.05 / 8 = 2400
                byte[] buffer = new byte[2400];
                while (!stopped) {
                    long len = audioDataStream.readData(buffer);
                    if (len == 0) {
                        break;
                    }
                    audioTrack.write(buffer, 0, (int) len);
                }

                audioDataStream.close();
            } catch (Exception ex) {
                Log.e("Speech Synthesis Demo", "unexpected " + ex.getMessage());
                ex.printStackTrace();
                assert(false);
            }
        }
    }

    private void stopSynthesizing() {
        if (synthesizer != null) {
            synthesizer.StopSpeakingAsync();
        }
        if (audioTrack != null) {
            synchronized (synchronizedObj) {
                stopped = true;
            }
            audioTrack.pause();
            audioTrack.flush();
        }
    }

    private void updateOutputMessage(String text) {
        updateOutputMessage(text, false, true);
    }

    private synchronized void updateOutputMessage(String text, boolean error, boolean append) {
        this.runOnUiThread(() -> {
            if (append) {
                outputMessage.append(text);
            } else {
                outputMessage.setText(text);
            }
            if (error) {
                Spannable spannableText = (Spannable) outputMessage.getText();
                spannableText.setSpan(new ForegroundColorSpan(Color.RED),
                    spannableText.length() - text.length(),
                    spannableText.length(),
                    0);
            }
        });
    }

    private void clearOutputMessage() {
        updateOutputMessage("", false, false);
    }
}
