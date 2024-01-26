//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//
package com.microsoft.cognitiveservices.speech.samples.embedded;

import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.io.StringReader;
import java.util.Arrays;
import java.util.List;
import java.util.Map;
import java.util.Scanner;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.Semaphore;

import com.microsoft.cognitiveservices.speech.*;
import com.microsoft.cognitiveservices.speech.audio.*;
import com.microsoft.cognitiveservices.speech.translation.*;


public class SpeechTranslationSamples
{
    // List available embedded speech translation models.
    public static void listEmbeddedModels() throws InterruptedException, ExecutionException
    {
        // Creates an instance of an embedded speech config.
        EmbeddedSpeechConfig speechConfig = Settings.createEmbeddedSpeechConfig();
        if (speechConfig == null)
        {
            return;
        }

        // Gets a list of models.
        List<SpeechTranslationModel> models = speechConfig.getSpeechTranslationModels();

        if (!models.isEmpty())
        {
            System.out.println("Models found [" + models.size() + "]:");
            for (SpeechTranslationModel model : models)
            {
                System.out.println();
                System.out.println(model.getName());
                System.out.print(" Source language(s) [" + model.getSourceLanguages().size() + "]: ");
                for (String sourceLang : model.getSourceLanguages())
                {
                    System.out.print(sourceLang + " ");
                }
                System.out.println();
                System.out.print(" Target language(s) [" + model.getTargetLanguages().size() + "]: ");
                for (String targetLang : model.getTargetLanguages())
                {
                    System.out.print(targetLang + " ");
                }
                System.out.println();
                System.out.println(" Path: " + model.getPath());
            }
        }
        else
        {
            System.err.println("No models found. Either the path is not valid or the format of model(s) is unknown.");
        }
    }


    private static void translateSpeechAsync(TranslationRecognizer recognizer) throws InterruptedException, ExecutionException
    {
        // Subscribes to events.
        recognizer.recognizing.addEventListener((s, e) ->
        {
            // Intermediate result (hypothesis).
            // Note that embedded "many-to-1" translation models support only one
            // target language (the model native output language). For example, a
            // "Many-to-English" model generates only output in English.
            // At the moment embedded translation cannot provide transcription of
            // the source language.
            if (e.getResult().getReason() == ResultReason.TranslatingSpeech)
            {
                // Source (input) language identification is enabled when TranslationRecognizer
                // is created with an AutoDetectSourceLanguageConfig argument.
                // In case the model does not support this functionality or the language cannot
                // be identified, the result Language is "Unknown".
                AutoDetectSourceLanguageResult sourceLangResult = AutoDetectSourceLanguageResult.fromResult(e.getResult());
                String sourceLang = sourceLangResult.getLanguage();

                for (Map.Entry<String, String> translation : e.getResult().getTranslations().entrySet())
                {
                    String targetLang = translation.getKey();
                    String outputText = translation.getValue();
                    System.out.println("Translating [" + sourceLang + " -> " + targetLang + "]: " + outputText);
                }
            }
        });

        recognizer.recognized.addEventListener((s, e) ->
        {
            // Final result. May differ from the last intermediate result.
            if (e.getResult().getReason() == ResultReason.TranslatedSpeech)
            {
                AutoDetectSourceLanguageResult sourceLangResult = AutoDetectSourceLanguageResult.fromResult(e.getResult());
                String sourceLang = sourceLangResult.getLanguage();

                for (Map.Entry<String, String> translation : e.getResult().getTranslations().entrySet())
                {
                    String targetLang = translation.getKey();
                    String outputText = translation.getValue();
                    System.out.println("TRANSLATED [" + sourceLang + " -> " + targetLang + "]: " + outputText);
                }
            }
            else if (e.getResult().getReason() == ResultReason.NoMatch)
            {
                // NoMatch occurs when no speech was recognized.
                NoMatchReason reason = NoMatchDetails.fromResult(e.getResult()).getReason();
                System.out.println("NO MATCH: Reason=" + reason);
            }
        });

        recognizer.canceled.addEventListener((s, e) ->
        {
            System.out.println("CANCELED: Reason=" + e.getReason());

            if (e.getReason() == CancellationReason.Error)
            {
                // NOTE: In case of an error, do not use the same recognizer for recognition anymore.
                System.err.println("CANCELED: ErrorCode=" + e.getErrorCode());
                System.err.println("CANCELED: ErrorDetails=\"" + e.getErrorDetails() + "\"");
            }
        });

        recognizer.sessionStarted.addEventListener((s, e) ->
        {
            System.out.println("Session started.");
        });

        recognizer.sessionStopped.addEventListener((s, e) ->
        {
            System.out.println("Session stopped.");
        });

        // The following lines run continuous recognition that listens for speech
        // in input audio and generates results until stopped. To run recognition
        // only once, replace this code block with
        //
        // SpeechRecognitionResult result = recognizer.recognizeOnceAsync().get();
        //
        // and optionally check this returned result for the outcome instead of
        // doing it in event handlers.

        // Starts continuous recognition.
        recognizer.startContinuousRecognitionAsync().get();

        System.out.println("Say something (press Enter to stop)...");
        @SuppressWarnings({ "unused", "resource" })
        String input = new Scanner(System.in).nextLine();

        // Stops recognition.
        recognizer.stopContinuousRecognitionAsync().get();
    }


    // Translates speech using embedded speech config and the system default microphone device.
    public static void embeddedTranslationFromMicrophone() throws InterruptedException, ExecutionException
    {
        EmbeddedSpeechConfig speechConfig = Settings.createEmbeddedSpeechConfig();
        AutoDetectSourceLanguageConfig sourceLangConfig = AutoDetectSourceLanguageConfig.fromOpenRange(); // optional, for input language identification
        AudioConfig audioConfig = AudioConfig.fromDefaultMicrophoneInput();

        TranslationRecognizer recognizer = new TranslationRecognizer(speechConfig, sourceLangConfig, audioConfig);
        translateSpeechAsync(recognizer);

        recognizer.close();
        audioConfig.close();
        speechConfig.close();
    }
}