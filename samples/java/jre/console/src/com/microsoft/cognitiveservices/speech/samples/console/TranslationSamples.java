package com.microsoft.cognitiveservices.speech.samples.console;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import java.io.IOException;
import java.util.Arrays;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.TimeoutException;
import java.util.concurrent.CompletableFuture;
import java.util.Map;
import java.util.Scanner;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.Semaphore;

import javax.sound.sampled.AudioFormat;
import javax.sound.sampled.AudioInputStream;
import javax.sound.sampled.AudioSystem;
import javax.sound.sampled.Clip;
import javax.sound.sampled.DataLine;
import javax.sound.sampled.LineUnavailableException;
import javax.sound.sampled.UnsupportedAudioFileException;

import java.io.ByteArrayInputStream;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.net.URI;
import java.net.URISyntaxException;

import com.azure.core.credential.AzureKeyCredential;
import com.azure.core.credential.TokenCredential;
import com.azure.identity.DefaultAzureCredentialBuilder;
// <toplevel>
import com.microsoft.cognitiveservices.speech.*;
import com.microsoft.cognitiveservices.speech.audio.*;
import com.microsoft.cognitiveservices.speech.translation.*;
// </toplevel>

@SuppressWarnings("resource") // scanner
public class TranslationSamples {
    // Translation with inline commit using a push audio stream.
    public static void translationWithInlineCommitAsync() throws InterruptedException, ExecutionException, IOException, URISyntaxException, TimeoutException
    {
        // <translationWithInlineCommitAsync>
        String endpoint = System.getenv("SPEECH_ENDPOINT");
        String subscriptionKey = System.getenv("SPEECH_RESOURCE_KEY");
        if (endpoint == null || endpoint.trim().isEmpty() || subscriptionKey == null || subscriptionKey.trim().isEmpty()) {
            throw new IllegalArgumentException("Set SPEECH_ENDPOINT and SPEECH_RESOURCE_KEY before running this sample.");
        }

        try (SpeechTranslationConfig config = SpeechTranslationConfig.fromEndpoint(new URI(endpoint), subscriptionKey)) {
            config.setSpeechRecognitionLanguage("en-US");
            config.addTargetLanguage("de");
            config.addTargetLanguage("fr");
            // Enable inline commit on a service deployment that supports the feature.
            config.setServiceProperty("setfeature", "forcecommit", ServicePropertyChannel.UriQueryParameter);

            // Use 16 kHz, 16-bit, mono PCM audio. WavStream skips the WAV header before writing to the push stream.
            try (FileInputStream input = new FileInputStream("whatstheweatherlike.wav")) {
                WavStream reader = new WavStream(input);
                AudioStreamFormat format = reader.getFormat();
                try (PushAudioInputStream pushStream = com.microsoft.cognitiveservices.speech.audio.AudioInputStream.createPushStream(format);
                     AudioConfig audioInput = AudioConfig.fromStreamInput(pushStream);
                     TranslationRecognizer recognizer = new TranslationRecognizer(config, audioInput)) {
                    CompletableFuture<Integer> commitAcknowledged = new CompletableFuture<>();
                    CompletableFuture<String> recognitionStopped = new CompletableFuture<>();

                    recognizer.recognizing.addEventListener((s, e) -> {
                        System.out.println("RECOGNIZING: Text=" + e.getResult().getText());
                    });
                    recognizer.recognized.addEventListener((s, e) -> {
                        int token = e.getResult().getCommitToken();
                        String tokenText = token != 0 ? " commit_token=" + token : "";
                        System.out.println("RECOGNIZED: Reason=" + e.getResult().getReason()
                            + " Text=" + e.getResult().getText() + tokenText);
                        for (Map.Entry<String, String> translation : e.getResult().getTranslations().entrySet()) {
                            System.out.println("  " + translation.getKey() + ": " + translation.getValue());
                        }
                        // NoMatch can acknowledge a commit; naturally segmented results have token 0.
                        if (token != 0) {
                            // Keep the token even if this event arrives before commit() returns.
                            commitAcknowledged.complete(token);
                        }
                    });
                    recognizer.canceled.addEventListener((s, e) -> {
                        System.out.println("CANCELED: Reason=" + e.getReason() + " Details=" + e.getErrorDetails());
                        recognitionStopped.complete(e.getReason() == CancellationReason.Error
                            ? "Recognition canceled: " + e.getErrorCode() + ". " + e.getErrorDetails() : null);
                    });
                    recognizer.sessionStopped.addEventListener((s, e) -> {
                        System.out.println("SESSION STOPPED: SessionId=" + e.getSessionId());
                        recognitionStopped.complete(null);
                    });

                    final int commitAtMilliseconds = 590;
                    final int bytesPerSecond = 16000 * 2;
                    boolean streamClosed = false;
                    try {
                        recognizer.startContinuousRecognitionAsync().get();
                        for (int segment = 0; segment < 2; ++segment) {
                            System.out.println("Writing audio segment " + (segment + 1));
                            int bytesRemaining = bytesPerSecond * commitAtMilliseconds / 1000;
                            while (segment != 0 || bytesRemaining > 0) {
                                if (recognitionStopped.isDone()) {
                                    String error = recognitionStopped.get();
                                    throw new IllegalStateException("Recognition ended before all audio was written. "
                                        + (error == null ? "" : error));
                                }
                                int bytesToRead = segment == 0 ? Math.min(3200, bytesRemaining) : 3200;
                                byte[] buffer = new byte[bytesToRead];
                                int bytesRead = reader.read(buffer);
                                if (bytesRead == 0) {
                                    if (segment == 0) {
                                        throw new IllegalStateException("The WAV file ended before the commit boundary.");
                                    }
                                    break;
                                }
                                pushStream.write(bytesRead == buffer.length ? buffer : Arrays.copyOf(buffer, bytesRead));
                                if (segment == 0) {
                                    bytesRemaining -= bytesRead;
                                }
                                Thread.sleep(bytesRead * 1000L / bytesPerSecond);
                            }

                            // Closing the stream later finalizes the remaining audio without another commit.
                            if (segment != 0) {
                                continue;
                            }
                            // Commit the audio written so far without closing the stream or stopping recognition.
                            int token = pushStream.commit();
                            System.out.println("COMMIT: token=" + token);
                            if (token == 0) {
                                throw new IllegalStateException("The SDK rejected the commit request.");
                            }
                            // A returned token is a local request; wait for a matching service acknowledgment.
                            try {
                                CompletableFuture.anyOf(commitAcknowledged, recognitionStopped).get(15, TimeUnit.SECONDS);
                            } catch (TimeoutException ex) {
                                throw new TimeoutException("No acknowledgment before timeout. Check inline commit support.");
                            }
                            if (!commitAcknowledged.isDone() || commitAcknowledged.get() != token) {
                                String error = recognitionStopped.getNow(null);
                                throw new IllegalStateException("Recognition ended before the audio could be committed. "
                                    + (error == null ? "" : error));
                            }
                            System.out.println("COMMIT ACKNOWLEDGED: token=" + token);
                        }

                        pushStream.close();
                        streamClosed = true;
                        String cancellationError;
                        try {
                            cancellationError = recognitionStopped.get(15, TimeUnit.SECONDS);
                        } catch (TimeoutException ex) {
                            throw new TimeoutException("Timed out waiting for recognition to finish after the end of audio.");
                        }
                        if (cancellationError != null) {
                            throw new IllegalStateException(cancellationError);
                        }
                    } finally {
                        if (!streamClosed) {
                            pushStream.close();
                        }
                        recognizer.stopContinuousRecognitionAsync().get();
                    }
                } finally {
                    format.close();
                }
            }
        }
        // </translationWithInlineCommitAsync>
    }

    // Translation from microphone.
    public static void translationWithMicrophoneAsync() throws InterruptedException, ExecutionException, IOException, URISyntaxException
    {
        // <TranslationWithMicrophoneAsync>
        // Creates an instance of a speech translation config with specified
        // subscription key and endpoint URL. Replace with your own subscription key
        // and endpoint URL.
        SpeechTranslationConfig config = SpeechTranslationConfig.fromEndpoint(new java.net.URI("YourEndpointUrl"), "YourSubscriptionKey");

        // Sets source and target language(s).
        String fromLanguage = "en-US";
        config.setSpeechRecognitionLanguage(fromLanguage);
        config.addTargetLanguage("de");

        // Sets voice name of synthesis output.
        String GermanVoice = "de-DE-AmalaNeural";
        config.setVoiceName(GermanVoice);

        // Creates a translation recognizer using microphone as audio input.
        TranslationRecognizer recognizer = new TranslationRecognizer(config);
        {
            // Subscribes to events.
            recognizer.recognizing.addEventListener((s, e) -> {
                System.out.println("RECOGNIZING in '" + fromLanguage + "': Text=" + e.getResult().getText());

                Map<String, String> map = e.getResult().getTranslations();
                for(String element : map.keySet()) {
                    System.out.println("    TRANSLATING into '" + element + "'': " + map.get(element));
                }
            });

            recognizer.recognized.addEventListener((s, e) -> {
                if (e.getResult().getReason() == ResultReason.TranslatedSpeech) {
                    System.out.println("RECOGNIZED in '" + fromLanguage + "': Text=" + e.getResult().getText());

                    Map<String, String> map = e.getResult().getTranslations();
                    for(String element : map.keySet()) {
                        System.out.println("    TRANSLATED into '" + element + "'': " + map.get(element));
                    }
                }
                if (e.getResult().getReason() == ResultReason.RecognizedSpeech) {
                    System.out.println("RECOGNIZED: Text=" + e.getResult().getText());
                    System.out.println("    Speech not translated.");
                }
                else if (e.getResult().getReason() == ResultReason.NoMatch) {
                    System.out.println("NOMATCH: Speech could not be recognized.");
                }
            });

            recognizer.synthesizing.addEventListener((s, e) -> {
                byte[] data = e.getResult().getAudio();

                System.out.println("Synthesis result received. Size of audio data: " + data.length);

                // Play the TTS data of we got more than the wav header.
                if (data != null && data.length > 44) {
                    try {
                        ByteArrayInputStream arrayInputStream = new ByteArrayInputStream(data);
                        AudioInputStream audioInputStream = AudioSystem.getAudioInputStream(arrayInputStream);
                        AudioFormat audioFormat = audioInputStream.getFormat();
                        DataLine.Info info = new DataLine.Info(Clip.class, audioFormat);
                        Clip clip = (Clip) AudioSystem.getLine(info);

                        clip.open(audioInputStream);
                        clip.start();
                    } catch (LineUnavailableException e1) {
                        e1.printStackTrace();
                    } catch (UnsupportedAudioFileException e1) {
                        e1.printStackTrace();
                    } catch (IOException e1) {
                        e1.printStackTrace();
                    }
                }
            });

            recognizer.canceled.addEventListener((s, e) -> {
                System.out.println("CANCELED: Reason=" + e.getReason());

                if (e.getReason() == CancellationReason.Error) {
                    System.out.println("CANCELED: ErrorCode=" + e.getErrorCode());
                    System.out.println("CANCELED: ErrorDetails=" + e.getErrorDetails());
                    System.out.println("CANCELED: Did you update the subscription info?");
                }
            });

            recognizer.sessionStarted.addEventListener((s, e) -> {
                System.out.println("\nSession started event.");
            });

            recognizer.sessionStopped.addEventListener((s, e) -> {
                System.out.println("\nSession stopped event.");
            });

            // Starts continuous recognition. Uses StopContinuousRecognitionAsync() to stop recognition.
            System.out.println("Say something...");
            recognizer.startContinuousRecognitionAsync().get();

            System.out.println("Press any key to stop");
            new Scanner(System.in).nextLine();

            recognizer.stopContinuousRecognitionAsync().get();
        }
        // </TranslationWithMicrophoneAsync>
    }

    // Translation using file input.
    // <TranslationWithFileAsync>
    private static Semaphore stopTranslationWithFileSemaphore;

    public static void translationWithFileAsync() throws InterruptedException, ExecutionException, URISyntaxException
    {
        stopTranslationWithFileSemaphore = new Semaphore(0);

        // Creates an instance of a speech translation config with specified
        // subscription key and endpoint URL. Replace with your own subscription key
        // and endpoint URL.
        SpeechTranslationConfig config = SpeechTranslationConfig.fromEndpoint(new java.net.URI("YourEndpointUrl"), "YourSubscriptionKey");

        // Sets source and target languages
        String fromLanguage = "en-US";
        config.setSpeechRecognitionLanguage(fromLanguage);
        config.addTargetLanguage("de");
        config.addTargetLanguage("fr");

        // Creates a translation recognizer using file as audio input.
        // Replace with your own audio file name.
        AudioConfig audioInput = AudioConfig.fromWavFileInput("YourAudioFile.wav");
        TranslationRecognizer recognizer = new TranslationRecognizer(config, audioInput);
        {
            // Subscribes to events.
            recognizer.recognizing.addEventListener((s, e) -> {
                System.out.println("RECOGNIZING in '" + fromLanguage + "': Text=" + e.getResult().getText());

                Map<String, String> map = e.getResult().getTranslations();
                for(String element : map.keySet()) {
                    System.out.println("    TRANSLATING into '" + element + "'': " + map.get(element));
                }
            });

            recognizer.recognized.addEventListener((s, e) -> {
                if (e.getResult().getReason() == ResultReason.TranslatedSpeech) {
                    System.out.println("RECOGNIZED in '" + fromLanguage + "': Text=" + e.getResult().getText());

                    Map<String, String> map = e.getResult().getTranslations();
                    for(String element : map.keySet()) {
                        System.out.println("    TRANSLATED into '" + element + "'': " + map.get(element));
                    }
                }
                if (e.getResult().getReason() == ResultReason.RecognizedSpeech) {
                    System.out.println("RECOGNIZED: Text=" + e.getResult().getText());
                    System.out.println("    Speech not translated.");
                }
                else if (e.getResult().getReason() == ResultReason.NoMatch) {
                    System.out.println("NOMATCH: Speech could not be recognized.");
                }
            });

            recognizer.canceled.addEventListener((s, e) -> {
                System.out.println("CANCELED: Reason=" + e.getReason());

                if (e.getReason() == CancellationReason.Error) {
                    System.out.println("CANCELED: ErrorCode=" + e.getErrorCode());
                    System.out.println("CANCELED: ErrorDetails=" + e.getErrorDetails());
                    System.out.println("CANCELED: Did you update the subscription info?");
                }

                stopTranslationWithFileSemaphore.release();;
            });

            recognizer.sessionStarted.addEventListener((s, e) -> {
                System.out.println("\nSession started event.");
            });

            recognizer.sessionStopped.addEventListener((s, e) -> {
                System.out.println("\nSession stopped event.");

                // Stops translation when session stop is detected.
                System.out.println("\nStop translation.");
                stopTranslationWithFileSemaphore.release();;
            });

            // Starts continuous recognition. Uses StopContinuousRecognitionAsync() to stop recognition.
            System.out.println("Start translation...");
            recognizer.startContinuousRecognitionAsync().get();

            // Waits for completion.
            stopTranslationWithFileSemaphore.acquire();;

            // Stops translation.
            recognizer.stopContinuousRecognitionAsync().get();
        }
    }
    // </TranslationWithFileAsync>

    // Translation using audio stream.
    private static Semaphore stopTranslationWithAudioStreamSemaphore;

    public static void translationWithAudioStreamAsync() throws InterruptedException, ExecutionException, FileNotFoundException, URISyntaxException
    {
        stopTranslationWithAudioStreamSemaphore = new Semaphore(0);

        // Creates an instance of a speech translation config with specified
        // subscription key and endpoint URL. Replace with your own subscription key
        // and endpoint URL.
        SpeechTranslationConfig config = SpeechTranslationConfig.fromEndpoint(new java.net.URI("YourEndpointUrl"), "YourSubscriptionKey");

        // Sets source and target languages
        String fromLanguage = "en-US";
        config.setSpeechRecognitionLanguage(fromLanguage);
        config.addTargetLanguage("de");
        config.addTargetLanguage("fr");

        // Create an audio stream from a wav file.
        // Replace with your own audio file name.
        PullAudioInputStreamCallback callback = new WavStream(new FileInputStream("YourAudioFile.wav"));
        AudioConfig audioInput = AudioConfig.fromStreamInput(callback);

        // Creates a translation recognizer using audio stream as input.
        TranslationRecognizer recognizer = new TranslationRecognizer(config, audioInput);
        {
            // Subscribes to events.
            recognizer.recognizing.addEventListener((s, e) -> {
                System.out.println("RECOGNIZING in '" + fromLanguage + "': Text=" + e.getResult().getText());

                Map<String, String> map = e.getResult().getTranslations();
                for(String element : map.keySet()) {
                    System.out.println("    TRANSLATING into '" + element + "'': " + map.get(element));
                }
            });

            recognizer.recognized.addEventListener((s, e) -> {
                if (e.getResult().getReason() == ResultReason.TranslatedSpeech) {
                    System.out.println("RECOGNIZED in '" + fromLanguage + "': Text=" + e.getResult().getText());

                    Map<String, String> map = e.getResult().getTranslations();
                    for(String element : map.keySet()) {
                        System.out.println("    TRANSLATED into '" + element + "'': " + map.get(element));
                    }
                }
                if (e.getResult().getReason() == ResultReason.RecognizedSpeech) {
                    System.out.println("RECOGNIZED: Text=" + e.getResult().getText());
                    System.out.println("    Speech not translated.");
                }
                else if (e.getResult().getReason() == ResultReason.NoMatch) {
                    System.out.println("NOMATCH: Speech could not be recognized.");
                }
            });

            recognizer.canceled.addEventListener((s, e) -> {
                System.out.println("CANCELED: Reason=" + e.getReason());

                if (e.getReason() == CancellationReason.Error) {
                    System.out.println("CANCELED: ErrorCode=" + e.getErrorCode());
                    System.out.println("CANCELED: ErrorDetails=" + e.getErrorDetails());
                    System.out.println("CANCELED: Did you update the subscription info?");
                }

                stopTranslationWithAudioStreamSemaphore.release();
            });

            recognizer.sessionStarted.addEventListener((s, e) -> {
                System.out.println("\nSession started event.");
            });

            recognizer.sessionStopped.addEventListener((s, e) -> {
                System.out.println("\nSession stopped event.");

                // Stops translation when session stop is detected.
                System.out.println("\nStop translation.");
                stopTranslationWithAudioStreamSemaphore.release();
            });

            // Starts continuous recognition. Uses StopContinuousRecognitionAsync() to stop recognition.
            System.out.println("Start translation...");
            recognizer.startContinuousRecognitionAsync().get();

            // Waits for completion.
            stopTranslationWithAudioStreamSemaphore.acquire();

            // Stops translation.
            recognizer.stopContinuousRecognitionAsync().get();
        }
    }

    // Translation authenticated via aad token crendential..
    public static void translationWithAADTokenCredentialAsync() throws InterruptedException, ExecutionException, IOException, URISyntaxException
    {
        // Use an appropriate instance of token credential. See Azure Core credentials docs for details:
        // https://learn.microsoft.com/java/api/com.azure.core.credential.tokencredential?view=azure-java-stable
        TokenCredential credential = new DefaultAzureCredentialBuilder().build();

        // Creates an instance of a speech config with specified token credential and
        // endpoint URL with custom domain(see: https://learn.microsoft.com/azure/ai-services/speech-service/speech-services-private-link?tabs=portal#create-a-custom-domain-name). 
        // Replace with your own endpoint URL.
        // The default language is "en-us".
        SpeechTranslationConfig config = SpeechTranslationConfig.fromEndpoint(new URI("YourCustomDomainEndpointUrl"), credential);

        // Sets source and target language(s).
        String fromLanguage = "en-US";
        config.setSpeechRecognitionLanguage(fromLanguage);
        config.addTargetLanguage("de");

        // Sets voice name of synthesis output.
        String GermanVoice = "de-DE-AmalaNeural";
        config.setVoiceName(GermanVoice);

        // Creates a translation recognizer using microphone as audio input.
        TranslationRecognizer recognizer = new TranslationRecognizer(config);
        {
            // Subscribes to events.
            recognizer.recognizing.addEventListener((s, e) -> {
                System.out.println("RECOGNIZING in '" + fromLanguage + "': Text=" + e.getResult().getText());

                Map<String, String> map = e.getResult().getTranslations();
                for(String element : map.keySet()) {
                    System.out.println("    TRANSLATING into '" + element + "'': " + map.get(element));
                }
            });

            recognizer.recognized.addEventListener((s, e) -> {
                if (e.getResult().getReason() == ResultReason.TranslatedSpeech) {
                    System.out.println("RECOGNIZED in '" + fromLanguage + "': Text=" + e.getResult().getText());

                    Map<String, String> map = e.getResult().getTranslations();
                    for(String element : map.keySet()) {
                        System.out.println("    TRANSLATED into '" + element + "'': " + map.get(element));
                    }
                }
                if (e.getResult().getReason() == ResultReason.RecognizedSpeech) {
                    System.out.println("RECOGNIZED: Text=" + e.getResult().getText());
                    System.out.println("    Speech not translated.");
                }
                else if (e.getResult().getReason() == ResultReason.NoMatch) {
                    System.out.println("NOMATCH: Speech could not be recognized.");
                }
            });

            recognizer.synthesizing.addEventListener((s, e) -> {
                byte[] data = e.getResult().getAudio();

                System.out.println("Synthesis result received. Size of audio data: " + data.length);

                // Play the TTS data of we got more than the wav header.
                if (data != null && data.length > 44) {
                    try {
                        ByteArrayInputStream arrayInputStream = new ByteArrayInputStream(data);
                        AudioInputStream audioInputStream = AudioSystem.getAudioInputStream(arrayInputStream);
                        AudioFormat audioFormat = audioInputStream.getFormat();
                        DataLine.Info info = new DataLine.Info(Clip.class, audioFormat);
                        Clip clip = (Clip) AudioSystem.getLine(info);

                        clip.open(audioInputStream);
                        clip.start();
                    } catch (LineUnavailableException e1) {
                        e1.printStackTrace();
                    } catch (UnsupportedAudioFileException e1) {
                        e1.printStackTrace();
                    } catch (IOException e1) {
                        e1.printStackTrace();
                    }
                }
            });

            recognizer.canceled.addEventListener((s, e) -> {
                System.out.println("CANCELED: Reason=" + e.getReason());

                if (e.getReason() == CancellationReason.Error) {
                    System.out.println("CANCELED: ErrorCode=" + e.getErrorCode());
                    System.out.println("CANCELED: ErrorDetails=" + e.getErrorDetails());
                    System.out.println("CANCELED: Did you update the subscription info?");
                }
            });

            recognizer.sessionStarted.addEventListener((s, e) -> {
                System.out.println("\nSession started event.");
            });

            recognizer.sessionStopped.addEventListener((s, e) -> {
                System.out.println("\nSession stopped event.");
            });

            // Starts continuous recognition. Uses StopContinuousRecognitionAsync() to stop recognition.
            System.out.println("Say something...");
            recognizer.startContinuousRecognitionAsync().get();

            System.out.println("Press any key to stop");
            new Scanner(System.in).nextLine();

            recognizer.stopContinuousRecognitionAsync().get();
        }
    }

    // Translation authenticated via key crendential..
    public static void translationWithKeyCredentialAsync() throws InterruptedException, ExecutionException, IOException, URISyntaxException
    {
        AzureKeyCredential azureKeyCredential = new AzureKeyCredential("YourSubscriptionKey");

        // Creates an instance of a speech config with specified key credential and
        // endpoint URL with custom domain(see: https://learn.microsoft.com/azure/ai-services/speech-service/speech-services-private-link?tabs=portal#create-a-custom-domain-name). 
        // Replace with your own endpoint URL.
        // The default language is "en-us".
        SpeechTranslationConfig config = SpeechTranslationConfig.fromEndpoint(new URI("YourCustomDomainEndpointUrl"), azureKeyCredential);

        // Sets source and target language(s).
        String fromLanguage = "en-US";
        config.setSpeechRecognitionLanguage(fromLanguage);
        config.addTargetLanguage("de");

        // Sets voice name of synthesis output.
        String GermanVoice = "de-DE-AmalaNeural";
        config.setVoiceName(GermanVoice);

        // Creates a translation recognizer using microphone as audio input.
        TranslationRecognizer recognizer = new TranslationRecognizer(config);
        {
            // Subscribes to events.
            recognizer.recognizing.addEventListener((s, e) -> {
                System.out.println("RECOGNIZING in '" + fromLanguage + "': Text=" + e.getResult().getText());

                Map<String, String> map = e.getResult().getTranslations();
                for(String element : map.keySet()) {
                    System.out.println("    TRANSLATING into '" + element + "'': " + map.get(element));
                }
            });

            recognizer.recognized.addEventListener((s, e) -> {
                if (e.getResult().getReason() == ResultReason.TranslatedSpeech) {
                    System.out.println("RECOGNIZED in '" + fromLanguage + "': Text=" + e.getResult().getText());

                    Map<String, String> map = e.getResult().getTranslations();
                    for(String element : map.keySet()) {
                        System.out.println("    TRANSLATED into '" + element + "'': " + map.get(element));
                    }
                }
                if (e.getResult().getReason() == ResultReason.RecognizedSpeech) {
                    System.out.println("RECOGNIZED: Text=" + e.getResult().getText());
                    System.out.println("    Speech not translated.");
                }
                else if (e.getResult().getReason() == ResultReason.NoMatch) {
                    System.out.println("NOMATCH: Speech could not be recognized.");
                }
            });

            recognizer.synthesizing.addEventListener((s, e) -> {
                byte[] data = e.getResult().getAudio();

                System.out.println("Synthesis result received. Size of audio data: " + data.length);

                // Play the TTS data of we got more than the wav header.
                if (data != null && data.length > 44) {
                    try {
                        ByteArrayInputStream arrayInputStream = new ByteArrayInputStream(data);
                        AudioInputStream audioInputStream = AudioSystem.getAudioInputStream(arrayInputStream);
                        AudioFormat audioFormat = audioInputStream.getFormat();
                        DataLine.Info info = new DataLine.Info(Clip.class, audioFormat);
                        Clip clip = (Clip) AudioSystem.getLine(info);

                        clip.open(audioInputStream);
                        clip.start();
                    } catch (LineUnavailableException e1) {
                        e1.printStackTrace();
                    } catch (UnsupportedAudioFileException e1) {
                        e1.printStackTrace();
                    } catch (IOException e1) {
                        e1.printStackTrace();
                    }
                }
            });

            recognizer.canceled.addEventListener((s, e) -> {
                System.out.println("CANCELED: Reason=" + e.getReason());

                if (e.getReason() == CancellationReason.Error) {
                    System.out.println("CANCELED: ErrorCode=" + e.getErrorCode());
                    System.out.println("CANCELED: ErrorDetails=" + e.getErrorDetails());
                    System.out.println("CANCELED: Did you update the subscription info?");
                }
            });

            recognizer.sessionStarted.addEventListener((s, e) -> {
                System.out.println("\nSession started event.");
            });

            recognizer.sessionStopped.addEventListener((s, e) -> {
                System.out.println("\nSession stopped event.");
            });

            // Starts continuous recognition. Uses StopContinuousRecognitionAsync() to stop recognition.
            System.out.println("Say something...");
            recognizer.startContinuousRecognitionAsync().get();

            System.out.println("Press any key to stop");
            new Scanner(System.in).nextLine();

            recognizer.stopContinuousRecognitionAsync().get();
        }
    }
}
