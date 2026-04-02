package com.microsoft.cognitiveservices.speech.samples.ttstextstream;

import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.ScrollView;
import android.widget.TextView;
import android.media.AudioTrack;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import android.media.AudioAttributes;
import android.media.AudioFormat;

import com.microsoft.cognitiveservices.speech.SpeechConfig;
import com.microsoft.cognitiveservices.speech.SpeechSynthesisOutputFormat;
import com.microsoft.cognitiveservices.speech.SpeechSynthesizer;
import com.microsoft.cognitiveservices.speech.SpeechSynthesisRequest;
import com.microsoft.cognitiveservices.speech.SpeechSynthesisResult;
import com.microsoft.cognitiveservices.speech.ResultReason;
import com.microsoft.cognitiveservices.speech.CancellationReason;
import com.microsoft.cognitiveservices.speech.AudioDataStream;
import com.microsoft.cognitiveservices.speech.PropertyId;
import com.microsoft.cognitiveservices.speech.Connection;
import com.microsoft.cognitiveservices.speech.SpeechSynthesisCancellationDetails;
import com.microsoft.cognitiveservices.speech.samples.ttstextstream.MarkdownStreamFilter;

import java.util.concurrent.Executors;
import java.nio.charset.StandardCharsets;
import java.io.ByteArrayOutputStream;
import java.io.FileOutputStream;
import java.net.URI;

import static android.Manifest.permission.INTERNET;

public class MainActivity extends AppCompatActivity {

    private static final String TAG = "TTSStreamSample";

    private static final String SPEECH_SUBSCRIPTION_KEY = "YourSubscriptionKey";
    private static final String SPEECH_REGION = "YourServiceRegion";

    // OPENAI / OLLAMA Configuration
    // Ensure your local LLM server is reachable from the Android Emulator/Device.
    private static final String OPENAI_ENDPOINT = "http://Your-LLM-Server-IP:11434/v1";
    private static final String OPENAI_KEY = "ollama"; // API Key if required
    private static final String OPENAI_DEPLOYMENT_NAME = "qwen2.5"; // Model name
    // -------------------------------------------------------------------------
    private SpeechConfig speechConfig;
    private TextView outputMessage;
    private TextView contentTextView;
    private Button startButton;
    private Button stopButton;
    private SpeechSynthesizer synthesizer;
    private Connection connection;
    private AudioTrack audioTrack;
    // Buffer to capture full audio for WAV saving
    private ByteArrayOutputStream fullAudioBuffer;
    
    private volatile boolean isStopped = false;
    private final int SAMPLE_RATE = 16000;

    // Add filter as a field
    private MarkdownStreamFilter markdownFilter = new MarkdownStreamFilter();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Request Internet Permission
        ActivityCompat.requestPermissions(MainActivity.this, new String[]{INTERNET}, 5);

        // UI Initialization
        outputMessage = findViewById(R.id.outputMessage);
        contentTextView = findViewById(R.id.tv_content);
        startButton = findViewById(R.id.button_start_stream);
        stopButton = findViewById(R.id.button_stop_stream);

        startButton.setOnClickListener(v -> onClickStart());
        stopButton.setOnClickListener(v -> onClickStop());
    }

    /**
     * Handles the Stop button click.
     * Signals the streaming loop to stop and cleans up audio resources.
     */
    private void onClickStop() {
        isStopped = true;
        log("Stopping...");
        
        // Stop Speech Synthesizer
        if (synthesizer != null) {
            synthesizer.StopSpeakingAsync();
        }
        
        // Stop Audio Playback
        if (audioTrack != null) {
            try {
                audioTrack.pause();
                audioTrack.flush();
            } catch (Exception e) {
                 Log.w(TAG, "Error pausing/flushing AudioTrack: " + e.getMessage());
            } finally {
                try {
                    audioTrack.release();
                } catch (Exception e) {
                    Log.e(TAG, "Error releasing AudioTrack: " + e.getMessage());
                }
                audioTrack = null;
            }
        }
        
        // Update UI
        runOnUiThread(() -> {
            startButton.setEnabled(true);
            stopButton.setEnabled(false);
        });
    }

    /**
     * Handles the Start button click.
     * Initializes resources and starts the background streaming task.
     */
    private void onClickStart() {
        isStopped = false;
        startButton.setEnabled(false);
        stopButton.setEnabled(true);
        
        if (contentTextView != null) {
            contentTextView.setText("", TextView.BufferType.SPANNABLE);
        }
        outputMessage.setText("Starting...");

        // Clean up any previous session resources
        cleanupResources();

        // Run the main logic in a background thread
        Executors.newSingleThreadExecutor().execute(this::runStreamingProcess);
    }
    
    /**
     * Safely cleans up Speech SDK and Audio resources.
     */
    private void cleanupResources() {
        if (synthesizer != null) {
            try {
                if (connection != null) {
                    connection.close();
                    connection = null;
                }
                synthesizer.close();
                synthesizer = null;
            } catch (Exception e) {
                Log.e(TAG, "Error closing synthesizer: " + e.getMessage());
            }
        }
        if (audioTrack != null) {
            try {
                audioTrack.stop();
                audioTrack.release();
                audioTrack = null;
            } catch (Exception e) {
                Log.e(TAG, "Error releasing AudioTrack: " + e.getMessage());
            }
        }
        if (fullAudioBuffer != null) {
            try {
                fullAudioBuffer.close();
                fullAudioBuffer = null;
            } catch (Exception e) {
                // ignore
            }
        }
    }

    // Main background process:
    private void runStreamingProcess() {
        SpeechSynthesisRequest request = null;
        java.util.concurrent.Future<SpeechSynthesisResult> resultFuture = null;
        String filesDir = System.getProperty("java.io.tmpdir");

        try {
            // 1. Initialize AudioTrack
            initializeAudioTrack();

            // 2. Initialize Speech SDK
            initializeSpeechSynthesizer(filesDir);

            // 3. Create Request for Text Stream
            log("Creating Speech Synthesis Request...");
            request = new SpeechSynthesisRequest(SpeechSynthesisRequest.SpeechSynthesisRequestInputType.TextStream);

            // 4. Start Synthesis (Async)
            log("Starting Synthesis Task...");
            resultFuture = synthesizer.SpeakRequestAsync(request);

            // 5. Connect to LLM and Stream Text
            streamTextFromLLM(request, filesDir);

        } catch (java.net.ConnectException ce) {
            log("Error connecting to LLM: " + ce.getMessage());
            log("Please ensure Ollama/LLM server is running and reachable at " + OPENAI_ENDPOINT);
        } catch (Exception e) {
            log("Error: " + e.getMessage());
            e.printStackTrace();
        } finally {
            // Signal End of Text Stream to TTS to unblock it (in case of error or stop)
            if (request != null) {
                try { request.getInputStream().close(); } catch (Exception ex) { /* fast fail */ }
            }

            // 6. Handle Completion (only if valid future exists and not stopped)
            if (!isStopped && resultFuture != null) {
                try {
                    handleSynthesisCompletion(resultFuture, filesDir);
                } catch (Exception e) {
                   Log.w(TAG, "Error in completion handler: " + e.getMessage());
                }
            }
            
            runOnUiThread(() -> {
                startButton.setEnabled(true);
                stopButton.setEnabled(false);
            });
        }
    }

    private void initializeAudioTrack() {
        int bufferSize = AudioTrack.getMinBufferSize(SAMPLE_RATE, AudioFormat.CHANNEL_OUT_MONO, AudioFormat.ENCODING_PCM_16BIT);
        audioTrack = new AudioTrack.Builder()
                .setAudioAttributes(new AudioAttributes.Builder()
                    .setUsage(AudioAttributes.USAGE_MEDIA)
                    .setContentType(AudioAttributes.CONTENT_TYPE_SPEECH)
                    .build())
                .setAudioFormat(new AudioFormat.Builder()
                    .setEncoding(AudioFormat.ENCODING_PCM_16BIT)
                    .setSampleRate(SAMPLE_RATE)
                    .setChannelMask(AudioFormat.CHANNEL_OUT_MONO)
                    .build())
                .setBufferSizeInBytes(bufferSize)
                .setTransferMode(AudioTrack.MODE_STREAM)
                .build();

        audioTrack.play();
        log("AudioTrack initialized and started.");
    }

    private void initializeSpeechSynthesizer(String logDir) {
        // Use V2 endpoint for text streaming support
        String endpoint = String.format("wss://%s.tts.speech.microsoft.com/cognitiveservices/websocket/v2", SPEECH_REGION);
        speechConfig = SpeechConfig.fromEndpoint(URI.create(endpoint), SPEECH_SUBSCRIPTION_KEY);
        
        // Configure Voice and Output Format
        // Note: Ensure the voice supports the language of your text (e.g., zh-CN for Chinese)
        String voiceName = "zh-CN-XiaoxiaoMultilingualNeural";
        speechConfig.setSpeechSynthesisVoiceName(voiceName);
        speechConfig.setSpeechSynthesisLanguage("zh-CN");
        speechConfig.setSpeechSynthesisOutputFormat(SpeechSynthesisOutputFormat.Raw16Khz16BitMonoPcm);
        
        // Advanced Properties
        speechConfig.setProperty(PropertyId.SpeechServiceConnection_SynthVoice, voiceName);
        speechConfig.setProperty(PropertyId.SpeechSynthesis_FrameTimeoutInterval, "120000"); // 120s timeout
        speechConfig.setProperty(PropertyId.Speech_LogFilename, logDir + "/sdk_log.log");
        speechConfig.setProperty("SpeechSynthesis_KeepConnectionAfterStopping", "true");

        // Create Synthesizer
        // Passing 'null' as AudioConfig because we act as a pull-stream consumer manually handling playback
        synthesizer = new SpeechSynthesizer(speechConfig, null);
        connection = Connection.fromSpeechSynthesizer(synthesizer);

        // Reset buffer
        fullAudioBuffer = new ByteArrayOutputStream();

        // Subscribe to events
        connection.connected.addEventListener((o, e) -> log("Speech Service Connected."));
        connection.disconnected.addEventListener((o, e) -> log("Speech Service Disconnected."));
        
        // Handle Audio Data
        synthesizer.Synthesizing.addEventListener((o, e) -> {
            byte[] data = e.getResult().getAudioData();
            int len = data != null ? data.length : 0;
            log("Received " + len + " bytes audio.");
            
            if (len > 0 && !isStopped) {
                // 1. Play Audio
                if (audioTrack != null) {
                    try {
                        audioTrack.write(data, 0, len);
                    } catch (Exception ex) {
                        Log.e(TAG, "AudioTrack write error: " + ex.getMessage());
                    }
                }
                
                // 2. Save Audio to Buffer
                if (fullAudioBuffer != null) {
                    synchronized(fullAudioBuffer) {
                        fullAudioBuffer.write(data, 0, len);
                    }
                }
            }
            e.close();
        });

        synthesizer.SynthesisCanceled.addEventListener((o, e) -> {
            String detail = SpeechSynthesisCancellationDetails.fromResult(e.getResult()).toString();
            log("Synthesis Canceled: " + detail);
        });
    }

    private void streamTextFromLLM(SpeechSynthesisRequest speechRequest, String logDir) throws Exception {
        log("Connecting to LLM (Ollama)...");
        
        // Prepare Connection
        String endpointUrl = OPENAI_ENDPOINT;
        if (endpointUrl.endsWith("/")) endpointUrl = endpointUrl.substring(0, endpointUrl.length() - 1);
        String urlString = endpointUrl + "/chat/completions";

        java.net.URL url = new java.net.URL(urlString);
        java.net.HttpURLConnection conn = (java.net.HttpURLConnection) url.openConnection();
        conn.setRequestMethod("POST");
        conn.setRequestProperty("Content-Type", "application/json");
        conn.setRequestProperty("Authorization", "Bearer " + OPENAI_KEY);
        conn.setDoOutput(true);
        conn.setConnectTimeout(3000); // Fast fail on connection
        conn.setReadTimeout(10000); // 10s read timeout (keep alive)

        // Build Payload
        org.json.JSONObject jsonBody = new org.json.JSONObject();
        jsonBody.put("model", OPENAI_DEPLOYMENT_NAME);
        jsonBody.put("stream", true);
        jsonBody.put("max_tokens", 800); // Increased for richer content
        jsonBody.put("temperature", 0.7); // More creative responses
        
        org.json.JSONArray messages = new org.json.JSONArray();
        messages.put(new org.json.JSONObject()
                .put("role", "system")
                .put("content", "You are a helpful AI assistant. Response in Chinese (Simplified Chinese)."));
        messages.put(new org.json.JSONObject()
                .put("role", "user")
                .put("content", "Hello! Please allow me to introduce yourself in 100 words."));
        jsonBody.put("messages", messages);

        // Send Request
        log("Sending POST to: " + urlString);
        try(java.io.OutputStream os = conn.getOutputStream()) {
            byte[] input = jsonBody.toString().getBytes(StandardCharsets.UTF_8);
            os.write(input, 0, input.length);
        }

        // Handle Response
        int responseCode = conn.getResponseCode();
        if (responseCode != 200) {
            log("LLM Error: Code " + responseCode + " - " + conn.getResponseMessage());
            speechRequest.getInputStream().close();
            return;
        }

        log("Streaming Text from LLM...");
        StringBuilder fullContentBuffer = new StringBuilder();
            
        try (java.io.BufferedReader br = new java.io.BufferedReader(
                new java.io.InputStreamReader(conn.getInputStream(), StandardCharsets.UTF_8))) {
            
            String line;
            while ((line = br.readLine()) != null) {
                if (isStopped) break; 

                if (line.startsWith("data: ")) {
                    String data = line.substring(6).trim();
                    if ("[DONE]".equals(data)) break;
                    try {
                        org.json.JSONObject chunk = new org.json.JSONObject(data);
                        if (chunk.has("model")) {
                            Log.v(TAG, "Model: " + chunk.getString("model"));
                        }
                        org.json.JSONArray choices = chunk.optJSONArray("choices");
                        if (choices != null && choices.length() > 0) {
                            org.json.JSONObject delta = choices.getJSONObject(0).optJSONObject("delta");
                            if (delta != null) {
                                String content = delta.optString("content", "");
                                if (!content.isEmpty()) {
                                    fullContentBuffer.append(content);
                                    runOnUiThread(() -> {
                                       if (contentTextView != null) {
                                           contentTextView.append(content);
                                           // autoScroll(contentTextView);
                                       }
                                    });
                                    // Use process() for lower latency (character-by-character), 
                                    // or processWithSentenceBoundary() for better prosody (but higher latency).
                                    // consistently lower latency is usually preferred for real-time chat.
                                    
                                    // Filter Markdown for TTS
                                    String filtered = markdownFilter.process(content);
                                    
                                    if (!filtered.isEmpty()) {
                                        log("Filtered chunk: " + (filtered.length() > 50 ? filtered.substring(0, 50) + "..." : filtered));
                                        speechRequest.getInputStream().write(filtered);
                                    } else {
                                        log("Buffered chunk: " + content.length() + " chars (Markdown hidden)");
                                    }
                                }
                            }
                        }
                    } catch (org.json.JSONException je) {
                        Log.e(TAG, "JSON parse error for chunk: " + data + " | " + je.getMessage());
                    }
                }
            }
        }
        log("LLM Stream Finished.");
        
        // Flush any remaining text in the filter
        String remaining = markdownFilter.flush();
        
        if (!remaining.isEmpty()) {
            log("Flushing filter: " + (remaining.length() > 50 ? remaining.substring(0, 50) + "..." : remaining));
            speechRequest.getInputStream().write(remaining);
        }
        
        speechRequest.getInputStream().close(); // Signal End of Text Stream to TTS
        saveTextToFile(logDir + "/response.md", fullContentBuffer.toString());
        log("Original Markdown saved to: " + logDir + "/response.md");
    }

    private void handleSynthesisCompletion(java.util.concurrent.Future<SpeechSynthesisResult> resultFuture, String filesDir) throws Exception {
        if (!isStopped) {
            log("Waiting for TTS completion...");
            SpeechSynthesisResult result = resultFuture.get(); // Blocking wait
            
            if (result.getReason() == ResultReason.SynthesizingAudioCompleted) {
                log("TTS Completed Successfully.");
                
                // Save captured audio buffer to WAV
                if (fullAudioBuffer != null) {
                    synchronized(fullAudioBuffer) {
                        String fileName = filesDir + "/output.wav";
                        saveAsWav(fileName, fullAudioBuffer.toByteArray(), SAMPLE_RATE, 16, 1);
                        log("Audio saved to: " + fileName);
                    }
                }
                
            } else if (result.getReason() == ResultReason.Canceled) {
                var cancellation = SpeechSynthesisCancellationDetails.fromResult(result);
                log("TTS Canceled: " + cancellation.getReason() + " | " + cancellation.getErrorDetails());
            }
        } else {
            log("Process stopped by user.");
        }
    }

    // Helper to save text to file
    private void saveTextToFile(String filename, String content) {
        try (FileOutputStream fos = new FileOutputStream(filename)) {
            fos.write(content.getBytes(StandardCharsets.UTF_8));
        } catch (Exception e) {
            Log.e(TAG, "Error saving file: " + e.getMessage());
        }
    }
    
    private void log(final String message) {
        runOnUiThread(() -> {
            outputMessage.append("\n" + message);
            autoScroll(outputMessage);
        });
        Log.d(TAG, message);
    }
    
    private void autoScroll(TextView view) {
        if (view.getParent() instanceof ScrollView) {
            final ScrollView scroll = (ScrollView) view.getParent();
            scroll.post(() -> scroll.fullScroll(View.FOCUS_DOWN));
        }
    }

    // Saves raw PCM data as a WAV file with proper header
    private void saveAsWav(String filename, byte[] pcmData, int sampleRate, int bitsPerSample, int channels) {
        int byteRate = sampleRate * channels * bitsPerSample / 8;
        int blockAlign = channels * bitsPerSample / 8;
        int dataSize = pcmData.length;
        int chunkSize = 36 + dataSize;
        
        try (FileOutputStream fos = new FileOutputStream(filename)) {
            // RIFF header
            fos.write("RIFF".getBytes());
            fos.write(intToLittleEndian(chunkSize));
            fos.write("WAVE".getBytes());
            
            // fmt subchunk
            fos.write("fmt ".getBytes());
            fos.write(intToLittleEndian(16));              // Subchunk1Size (16 for PCM)
            fos.write(shortToLittleEndian((short) 1));     // AudioFormat (1 = PCM)
            fos.write(shortToLittleEndian((short) channels));
            fos.write(intToLittleEndian(sampleRate));
            fos.write(intToLittleEndian(byteRate));
            fos.write(shortToLittleEndian((short) blockAlign));
            fos.write(shortToLittleEndian((short) bitsPerSample));
            
            // data subchunk
            fos.write("data".getBytes());
            fos.write(intToLittleEndian(dataSize));
            fos.write(pcmData);
        } catch (Exception e) {
             Log.e(TAG, "Error saving WAV file: " + e.getMessage());
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
    
    @Override
    protected void onDestroy() {
        super.onDestroy();
        cleanupResources();
    }
}
