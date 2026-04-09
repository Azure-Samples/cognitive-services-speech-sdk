package com.microsoft.cognitiveservices.speech.samples.ttstextstream;

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
import java.util.concurrent.Future;

public class SpeakTextStream {

    private static final int SAMPLE_RATE = 16000;
    private static final int CHANNEL_COUNT = 1;
    private static final int BITS_PER_SAMPLE = 16;
    private static final String DEFAULT_VOICE = "en-us-Ava:DragonHDLatestNeural";
    private static final String DEFAULT_OUTPUT_FILE = "target/streaming_output.wav";
    private static final String DEFAULT_DOMAIN = "tts.speech.microsoft.com";
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

    public static void main(String[] args) throws Exception {
        String speechKey = readRequiredEnv("SPEECH_KEY");
        String speechRegion = readRequiredEnv("SPEECH_REGION");
        String speechVoice = readEnv("SPEECH_VOICE", DEFAULT_VOICE);
        File outputFile = new File(readEnv("OUTPUT_FILE", DEFAULT_OUTPUT_FILE)).getAbsoluteFile();
        File logFile = new File(System.getProperty("java.io.tmpdir"), "tts-text-stream.log").getAbsoluteFile();
        String endpoint = buildEndpoint(speechRegion);

        System.out.println("Preparing text streaming...");
        System.out.println("Using endpoint: " + endpoint);
        System.out.println("Voice: " + speechVoice);
        System.out.println("Output file: " + outputFile.getAbsolutePath());
        System.out.println("SDK log file: " + logFile.getAbsolutePath());

        runTextStreamingDemo(speechKey, speechVoice, endpoint, logFile, outputFile);
    }

    private static void runTextStreamingDemo(String speechKey, String speechVoice, String endpoint, File logFile,
            File outputFile) throws Exception {
        ByteArrayOutputStream audioBuffer = new ByteArrayOutputStream();
        SpeechConfig speechConfig = SpeechConfig.fromEndpoint(new URI(endpoint), speechKey);
        speechConfig.setSpeechSynthesisOutputFormat(SpeechSynthesisOutputFormat.Raw16Khz16BitMonoPcm);
        speechConfig.setSpeechSynthesisVoiceName(speechVoice);
        speechConfig.setProperty(PropertyId.SpeechServiceConnection_SynthVoice, speechVoice);
        speechConfig.setProperty(PropertyId.SpeechSynthesis_FrameTimeoutInterval, "10000");
        speechConfig.setProperty(PropertyId.SpeechSynthesis_RtfTimeoutThreshold, "10");
        speechConfig.setProperty(PropertyId.Speech_LogFilename, logFile.getAbsolutePath());

        SpeechSynthesizer synthesizer = new SpeechSynthesizer(speechConfig, null);
        Connection connection = Connection.fromSpeechSynthesizer(synthesizer);
        SpeechSynthesisRequest request = new SpeechSynthesisRequest(SpeechSynthesisRequestInputType.TextStream);

        SpeechSynthesisResult result = null;
        AudioDataStream audioDataStream = null;
        Thread audioThread = null;

        try {
            connection.connected.addEventListener((o, e) -> System.out.println("Connection established."));
            connection.disconnected.addEventListener((o, e) -> System.out.println("Disconnected."));
            synthesizer.SynthesisStarted.addEventListener((o, e) -> {
                System.out.println("Synthesis started. ResultId: " + e.getResult().getResultId());
                e.close();
            });
            synthesizer.SynthesisCompleted.addEventListener((o, e) -> {
                System.out.println("Synthesis completed.");
                e.close();
            });
            synthesizer.SynthesisCanceled.addEventListener((o, e) -> {
                System.out.println("Synthesis canceled: " + SpeechSynthesisCancellationDetails.fromResult(e.getResult()));
                e.close();
            });
            synthesizer.Synthesizing.addEventListener((o, e) -> {
                byte[] data = e.getResult().getAudioData();
                int len = data != null ? data.length : 0;
                System.out.println("Synthesizing event received " + len + " bytes audio data.");
                e.close();
            });

            System.out.println("Starting speech synthesis...");
            Future<SpeechSynthesisResult> task = synthesizer.StartSpeakingRequestAsync(request);
            result = task.get();
            audioDataStream = AudioDataStream.fromResult(result);

            AudioDataStream stream = audioDataStream;
            ByteArrayOutputStream streamBuffer = audioBuffer;
            audioThread = new Thread(() -> readAudioStream(stream, streamBuffer), "tts-audio-reader");
            audioThread.start();

            for (String chunk : TEXT_CHUNKS) {
                System.out.println("[text: " + chunk.trim() + "]");
                request.getInputStream().write(chunk);
                Thread.sleep(200L);
            }

            request.getInputStream().close();
            System.out.println("[TEXT STREAM END]");

            if (audioThread != null) {
                audioThread.join();
            }

            if (result != null && result.getReason() == ResultReason.Canceled) {
                System.out.println("Synthesis canceled: " + SpeechSynthesisCancellationDetails.fromResult(result));
            } else if (result != null && result.getReason() == ResultReason.SynthesizingAudioCompleted) {
                System.out.println("Synthesis audio stream finished.");
            }

            byte[] pcmData = audioBuffer.toByteArray();
            saveAsWav(outputFile, pcmData);
            System.out.println("[AUDIO STREAM END] Total: " + pcmData.length + " bytes");
            System.out.println("Audio saved to: " + outputFile.getAbsolutePath());
        } finally {
            if (audioThread != null && audioThread.isAlive()) {
                audioThread.interrupt();
                audioThread.join(1000L);
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
            audioBuffer.close();
        }
    }

    private static void readAudioStream(AudioDataStream stream, ByteArrayOutputStream audioBuffer) {
        byte[] buffer = new byte[4096];

        try {
            while (true) {
                long readBytes = stream.readData(buffer);
                if (readBytes == 0) {
                    break;
                }

                System.out.println("[audio: " + readBytes + " bytes]");
                audioBuffer.write(buffer, 0, (int) readBytes);
            }
        } catch (Exception e) {
            throw new RuntimeException("Error reading audio stream", e);
        }
    }

    private static String buildEndpoint(String region) {
        String explicitEndpoint = System.getenv("SPEECH_ENDPOINT");
        if (explicitEndpoint != null && !explicitEndpoint.trim().isEmpty()) {
            return explicitEndpoint.trim();
        }

        String domain = readEnv("SPEECH_DOMAIN", DEFAULT_DOMAIN);
        return "wss://" + region + "." + domain + "/cognitiveservices/websocket/v2";
    }

    private static String readRequiredEnv(String name) {
        String value = System.getenv(name);
        if (value == null || value.trim().isEmpty()) {
            throw new IllegalArgumentException("Missing required environment variable: " + name);
        }
        return value.trim();
    }

    private static String readEnv(String name, String defaultValue) {
        String value = System.getenv(name);
        if (value == null || value.trim().isEmpty()) {
            return defaultValue;
        }
        return value.trim();
    }

    private static void saveAsWav(File file, byte[] pcmData) throws Exception {
        int byteRate = SAMPLE_RATE * CHANNEL_COUNT * BITS_PER_SAMPLE / 8;
        int blockAlign = CHANNEL_COUNT * BITS_PER_SAMPLE / 8;
        int dataSize = pcmData.length;
        int chunkSize = 36 + dataSize;

        File parent = file.getParentFile();
        if (parent != null && !parent.exists()) {
            parent.mkdirs();
        }

        try (FileOutputStream output = new FileOutputStream(file)) {
            output.write("RIFF".getBytes("US-ASCII"));
            output.write(intToLittleEndian(chunkSize));
            output.write("WAVE".getBytes("US-ASCII"));
            output.write("fmt ".getBytes("US-ASCII"));
            output.write(intToLittleEndian(16));
            output.write(shortToLittleEndian((short) 1));
            output.write(shortToLittleEndian((short) CHANNEL_COUNT));
            output.write(intToLittleEndian(SAMPLE_RATE));
            output.write(intToLittleEndian(byteRate));
            output.write(shortToLittleEndian((short) blockAlign));
            output.write(shortToLittleEndian((short) BITS_PER_SAMPLE));
            output.write("data".getBytes("US-ASCII"));
            output.write(intToLittleEndian(dataSize));
            output.write(pcmData);
        }
    }

    private static byte[] intToLittleEndian(int value) {
        return new byte[] {
            (byte) (value & 0xFF),
            (byte) ((value >> 8) & 0xFF),
            (byte) ((value >> 16) & 0xFF),
            (byte) ((value >> 24) & 0xFF)
        };
    }

    private static byte[] shortToLittleEndian(short value) {
        return new byte[] {
            (byte) (value & 0xFF),
            (byte) ((value >> 8) & 0xFF)
        };
    }
}