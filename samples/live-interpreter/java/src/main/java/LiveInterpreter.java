// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license.

import java.io.IOException;
import java.net.URI;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.StandardOpenOption;
import java.util.Map;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.atomic.AtomicReference;

import com.microsoft.cognitiveservices.speech.AutoDetectSourceLanguageConfig;
import com.microsoft.cognitiveservices.speech.CancellationReason;
import com.microsoft.cognitiveservices.speech.PropertyId;
import com.microsoft.cognitiveservices.speech.ResultReason;
import com.microsoft.cognitiveservices.speech.SpeechSynthesisOutputFormat;
import com.microsoft.cognitiveservices.speech.audio.AudioConfig;
import com.microsoft.cognitiveservices.speech.translation.SpeechTranslationConfig;
import com.microsoft.cognitiveservices.speech.translation.TranslationRecognitionResult;
import com.microsoft.cognitiveservices.speech.translation.TranslationRecognizer;

public class LiveInterpreter {
    private static String requiredEnvironment(String name) {
        String value = System.getenv(name);
        if (value == null || value.isEmpty()) {
            throw new IllegalStateException("Set " + name + " before running this sample.");
        }
        return value;
    }

    private static void logResult(String label, TranslationRecognitionResult result) {
        String language = result.getProperties().getProperty(
            PropertyId.SpeechServiceConnection_AutoDetectSourceLanguageResult);
        System.out.printf("%s in '%s': %s%n", label, language, result.getText());
        for (Map.Entry<String, String> translation :
                result.getTranslations().entrySet()) {
            System.out.printf("    TRANSLATED into '%s': %s%n",
                translation.getKey(), translation.getValue());
        }
    }

    public static void main(String[] args) throws Exception {
        String speechKey = requiredEnvironment("SPEECH_KEY");
        URI endpoint = URI.create(requiredEnvironment("SPEECH_ENDPOINT"));
        Path audioFile = Path.of(requiredEnvironment("LIVE_INTERPRETER_AUDIO_FILE"));
        if (!Files.isRegularFile(audioFile)) {
            throw new IOException("WAV input file not found: " + audioFile);
        }

        CountDownLatch stopped = new CountDownLatch(1);
        AtomicInteger audioIndex = new AtomicInteger();
        AtomicReference<Exception> failure = new AtomicReference<>();
        try (SpeechTranslationConfig config =
                SpeechTranslationConfig.fromEndpoint(endpoint, speechKey);
             AutoDetectSourceLanguageConfig autoDetectConfig =
                AutoDetectSourceLanguageConfig.fromOpenRange();
             AudioConfig audioConfig = AudioConfig.fromWavFileInput(audioFile.toString())) {
            config.addTargetLanguage("fr");
            config.setVoiceName("personal-voice");
            config.setSpeechSynthesisOutputFormat(
                SpeechSynthesisOutputFormat.Riff16Khz16BitMonoPcm);
            try (TranslationRecognizer recognizer =
                    new TranslationRecognizer(config, autoDetectConfig, audioConfig)) {
                recognizer.recognizing.addEventListener((sender, event) ->
                    logResult("RECOGNIZING", event.getResult()));
                recognizer.recognized.addEventListener((sender, event) -> {
                    if (event.getResult().getReason() == ResultReason.TranslatedSpeech) {
                        logResult("RECOGNIZED", event.getResult());
                    } else if (event.getResult().getReason() == ResultReason.NoMatch) {
                        System.out.println("NOMATCH: Speech couldn't be recognized.");
                    }
                });
                recognizer.synthesizing.addEventListener((sender, event) -> {
                    byte[] audio = event.getResult().getAudio();
                    if (audio.length == 0) {
                        return;
                    }
                    Path output = Path.of(
                        "translation-" + audioIndex.incrementAndGet() + ".wav");
                    try {
                        Files.write(output, audio, StandardOpenOption.CREATE_NEW,
                            StandardOpenOption.WRITE);
                        System.out.printf("Wrote %s: %d bytes%n", output, audio.length);
                    } catch (IOException error) {
                        failure.compareAndSet(null, error);
                        stopped.countDown();
                    }
                });
                recognizer.canceled.addEventListener((sender, event) -> {
                    System.out.println("CANCELED: Reason=" + event.getReason());
                    if (event.getReason() == CancellationReason.Error) {
                        failure.compareAndSet(null, new IllegalStateException(
                            "ErrorCode=" + event.getErrorCode()
                            + ", ErrorDetails=" + event.getErrorDetails()));
                    }
                    stopped.countDown();
                });
                recognizer.sessionStopped.addEventListener((sender, event) ->
                    stopped.countDown());

                System.out.println("Start translation...");
                recognizer.startContinuousRecognitionAsync().get(30, TimeUnit.SECONDS);
                try {
                    if (!stopped.await(5, TimeUnit.MINUTES)) {
                        throw new IllegalStateException(
                            "Translation timed out after five minutes.");
                    }
                } finally {
                    recognizer.stopContinuousRecognitionAsync().get(30, TimeUnit.SECONDS);
                }
            }
        }
        if (failure.get() != null) {
            throw failure.get();
        }
    }
}
