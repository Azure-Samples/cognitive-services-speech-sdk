package com.microsoft.cognitiveservices.speech.samples.ttstextstream;

import com.microsoft.cognitiveservices.speech.*;
import org.json.JSONArray;
import org.json.JSONObject;

import javax.sound.sampled.*;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.net.HttpURLConnection;
import java.net.URI;
import java.net.URL;
import java.nio.charset.StandardCharsets;
import java.util.Scanner;
import java.util.concurrent.Future;

public class Main {
    // Please set your environment variables or replace these values directly
    private static String SPEECH_SUBSCRIPTION_KEY = System.getenv("SPEECH_KEY");
    private static String SPEECH_REGION = System.getenv("SPEECH_REGION");
    
    // OPENAI / OLLAMA Configuration
    // Read configuration from environment variables to avoid hardcoding.
    private static final String OPENAI_ENDPOINT = System.getenv("OPENAI_ENDPOINT");
    private static final String OPENAI_KEY = System.getenv("OPENAI_KEY");
    private static final String OPENAI_DEPLOYMENT_NAME = System.getenv("OPENAI_DEPLOYMENT_NAME");

    private static SpeechSynthesizer synthesizer;
    private static Connection connection;
    private static SourceDataLine audioLine;
    private static final int SAMPLE_RATE = 16000;
    private static boolean isStopped = false;

    private static final MarkdownStreamFilter markdownFilter = new MarkdownStreamFilter();

    public static void main(String[] args) {
        if (SPEECH_SUBSCRIPTION_KEY == null || SPEECH_REGION == null) {
            System.out.println("Please set SPEECH_KEY and SPEECH_REGION environment variables.");
            // For testing convenience, you can uncomment and set keys here:
            // SPEECH_SUBSCRIPTION_KEY = "YourSubscriptionKey";
            // SPEECH_REGION = "YourServiceRegion";
            if (SPEECH_SUBSCRIPTION_KEY == null) return;
        }

        try {
            initializeAudioTrack();
            initializeSpeechSynthesizer();

            System.out.println("Press Enter to start streaming from LLM...");
            new Scanner(System.in).nextLine();

            runStreamingProcess();

            System.out.println("Press Enter to exit...");
            new Scanner(System.in).nextLine();

        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            cleanupResources();
        }
    }

    private static void initializeAudioTrack() throws LineUnavailableException {
        AudioFormat format = new AudioFormat(SAMPLE_RATE, 16, 1, true, false);
        DataLine.Info info = new DataLine.Info(SourceDataLine.class, format);

        if (!AudioSystem.isLineSupported(info)) {
            System.err.println("Audio line not supported");
            System.exit(0);
        }

        audioLine = (SourceDataLine) AudioSystem.getLine(info);
        audioLine.open(format);
        audioLine.start();
        System.out.println("Audio initialized.");
    }

    private static void initializeSpeechSynthesizer() {
        String endpoint = String.format("wss://%s.tts.speech.microsoft.com/cognitiveservices/websocket/v2", SPEECH_REGION);
        SpeechConfig speechConfig = SpeechConfig.fromEndpoint(URI.create(endpoint), SPEECH_SUBSCRIPTION_KEY);
        
        String voiceName = "zh-CN-XiaoxiaoMultilingualNeural";
        speechConfig.setSpeechSynthesisVoiceName(voiceName);
        speechConfig.setSpeechSynthesisLanguage("zh-CN");
        speechConfig.setSpeechSynthesisOutputFormat(SpeechSynthesisOutputFormat.Raw16Khz16BitMonoPcm);
        
        speechConfig.setProperty(PropertyId.SpeechServiceConnection_SynthVoice, voiceName);
        speechConfig.setProperty(PropertyId.SpeechSynthesis_FrameTimeoutInterval, "120000"); 
        speechConfig.setProperty("SpeechSynthesis_KeepConnectionAfterStopping", "true");

        synthesizer = new SpeechSynthesizer(speechConfig, null);
        connection = Connection.fromSpeechSynthesizer(synthesizer);

        connection.connected.addEventListener((o, e) -> System.out.println("Speech Service Connected."));
        connection.disconnected.addEventListener((o, e) -> System.out.println("Speech Service Disconnected."));
        
        synthesizer.Synthesizing.addEventListener((o, e) -> {
            byte[] data = e.getResult().getAudioData();
            int len = data != null ? data.length : 0;
            // System.out.println("Received " + len + " bytes audio.");
            
            if (len > 0) {
                if (audioLine != null) {
                    audioLine.write(data, 0, len);
                }
            }
            e.close();
        });

        synthesizer.SynthesisCanceled.addEventListener((o, e) -> {
            String detail = SpeechSynthesisCancellationDetails.fromResult(e.getResult()).toString();
            System.out.println("Synthesis Canceled: " + detail);
        });
    }

    private static void runStreamingProcess() {
        SpeechSynthesisRequest request = null;
        Future<SpeechSynthesisResult> resultFuture = null;

        try {
            System.out.println("Creating Speech Synthesis Request...");
            request = new SpeechSynthesisRequest(SpeechSynthesisRequest.SpeechSynthesisRequestInputType.TextStream);

            System.out.println("Starting Synthesis Task...");
            resultFuture = synthesizer.SpeakRequestAsync(request);

            streamTextFromLLM(request);

        } catch (Exception e) {
            System.err.println("Error: " + e.getMessage());
            e.printStackTrace();
        } finally {
            if (request != null) {
                try { request.getInputStream().close(); } catch (Exception ex) { }
            }

            if (resultFuture != null) {
                try {
                    SpeechSynthesisResult result = resultFuture.get();
                    if (result.getReason() == ResultReason.SynthesizingAudioCompleted) {
                        System.out.println("TTS Completed Successfully.");
                    }
                } catch (Exception e) {
                    System.err.println("Error in completion handler: " + e.getMessage());
                }
            }
        }
    }

    private static void streamTextFromLLM(SpeechSynthesisRequest speechRequest) throws Exception {
        System.out.println("Connecting to LLM (Ollama)...");
        
        String endpointUrl = OPENAI_ENDPOINT;
        if (endpointUrl.endsWith("/")) endpointUrl = endpointUrl.substring(0, endpointUrl.length() - 1);
        String urlString = endpointUrl + "/chat/completions";

        URL url = new URL(urlString);
        HttpURLConnection conn = (HttpURLConnection) url.openConnection();
        conn.setRequestMethod("POST");
        conn.setRequestProperty("Content-Type", "application/json");
        conn.setRequestProperty("Authorization", "Bearer " + OPENAI_KEY);
        conn.setDoOutput(true);
        conn.setConnectTimeout(3000); 
        conn.setReadTimeout(10000); 

        JSONObject jsonBody = new JSONObject();
        jsonBody.put("model", OPENAI_DEPLOYMENT_NAME);
        jsonBody.put("stream", true);
        jsonBody.put("max_tokens", 800);
        jsonBody.put("temperature", 0.7);
        
        JSONArray messages = new JSONArray();
        messages.put(new JSONObject()
                .put("role", "system")
                .put("content", "You are a helpful AI assistant. Response in Chinese (Simplified Chinese)."));
        messages.put(new JSONObject()
                .put("role", "user")
                .put("content", "Hello! Please allow me to introduce yourself in 100 words."));
        jsonBody.put("messages", messages);

        System.out.println("Sending POST to: " + urlString);
        try(OutputStream os = conn.getOutputStream()) {
            byte[] input = jsonBody.toString().getBytes(StandardCharsets.UTF_8);
            os.write(input, 0, input.length);
        }

        int responseCode = conn.getResponseCode();
        if (responseCode != 200) {
            System.err.println("LLM Error: Code " + responseCode + " - " + conn.getResponseMessage());
            speechRequest.getInputStream().close();
            try {
                initializeAudioTrack();
                initializeSpeechSynthesizer();

                if (OPENAI_ENDPOINT == null || OPENAI_KEY == null || OPENAI_DEPLOYMENT_NAME == null) {
                    System.out.println("Please set OPENAI_ENDPOINT, OPENAI_KEY and OPENAI_DEPLOYMENT_NAME environment variables before running.");
                    return;
                }

                Scanner scanner = new Scanner(System.in);
                while (true) {
                    System.out.println("Enter your message for LLM (or type 'exit' to quit):");
                    String userInput = scanner.nextLine();
                    if ("exit".equalsIgnoreCase(userInput.trim())) {
                        break;
                    }
                    if (userInput.trim().isEmpty()) {
                        continue;
                    }

                    runStreamingProcess(userInput);
                }

            } catch (Exception e) {
                                String content = delta.optString("content", "");
                                if (!content.isEmpty()) {
                                    System.out.print(content);
                                    
                                    String filtered = markdownFilter.process(content);
                                    
                                    if (!filtered.isEmpty()) {
                                        speechRequest.getInputStream().write(filtered);
                                    }
                                }
                            }
                        }
                    } catch (Exception je) {
                        System.err.println("JSON parse error: " + je.getMessage());
                    }
                }
            }
        }
        System.out.println("\nLLM Stream Finished.");
        
        String remaining = markdownFilter.flush();
        if (!remaining.isEmpty()) {
            speechRequest.getInputStream().write(remaining);
        }
        
        speechRequest.getInputStream().close(); 
    }

    private static void cleanupResources() {
        if (synthesizer != null) {
            synthesizer.close();
            synthesizer = null;
        }
        if (audioLine != null) {
            audioLine.drain();
            audioLine.close();
            audioLine = null;
        }
    }
}
