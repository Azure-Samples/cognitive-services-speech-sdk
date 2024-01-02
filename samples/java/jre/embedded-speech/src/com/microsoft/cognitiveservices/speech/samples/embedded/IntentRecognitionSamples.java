//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//
package com.microsoft.cognitiveservices.speech.samples.embedded;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Dictionary;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Scanner;
import java.util.concurrent.ExecutionException;

import com.microsoft.cognitiveservices.speech.*;
import com.microsoft.cognitiveservices.speech.audio.*;
import com.microsoft.cognitiveservices.speech.intent.*;


public class IntentRecognitionSamples
{
    // Creates a pattern matching model with some example intents.
    // For more examples on intent patterns, see
    // https://github.com/Azure-Samples/cognitive-services-speech-sdk/blob/master/samples/java/jre/console/src/com/microsoft/cognitiveservices/speech/samples/console/IntentRecognitionSamples.java
    private static PatternMatchingModel createPatternMatchingModel()
    {
        // Creates a pattern matching model. The id is necessary to identify this
        // model from others if there are several models in use at the same time.
        PatternMatchingModel model = new PatternMatchingModel("SomeUniqueIdForThisModel");

        // Make the keyword optional (surround with [ and ] in the pattern string)
        // in case the pattern matching model is used without keyword recognition.
        String patternKeywordOptional = "[" + Settings.getKeywordPhrase() + "]";

        // Specify some intents to add. Example inputs:
        // - "Turn on the radio."
        // - "Switch off the lights in the bedroom."

        // Notes about patterns:
        // - surround entity names with { and }
        // - surround alternatives with ( and ) and separate with |
        // - surround optional parts with [ and ]
        // - default entity {any} matches any text
        String patternTurnOn = "(turn|switch) on";
        String patternTurnOff = "(turn|switch) off";
        String patternTarget = "[{any}] {targetName}";      // using {any} to match any text before {targetName}
        String patternLocation = "[{any}] {locationName}";  // using {any} to match any text before {locationName}

        // Define acceptable values for the entities. By default an entity can have any value.
        // If these are not defined, patterns with {any} before the entity name will not work.
        List<String> targetNames = Arrays.asList( "lamp", "lights", "radio", "TV" );
        model.getEntities().put(PatternMatchingEntity.CreateListEntity(
            "targetName",
            PatternMatchingEntity.EntityMatchMode.Strict,
            targetNames
            ));

        List<String> locationNames = Arrays.asList( "living room", "bedroom", "kitchen" );
        model.getEntities().put(PatternMatchingEntity.CreateListEntity(
            "locationName",
            PatternMatchingEntity.EntityMatchMode.Strict,
            locationNames
            ));

        // Add intents (id, phrase(s)) to the model.
        Map<String, List<String>> intents = new HashMap<>();
        intents.put(
            "HomeAutomation.TurnOn",
            Arrays.asList(
                patternKeywordOptional + " " + patternTurnOn + " " + patternTarget,
                patternKeywordOptional + " " + patternTurnOn + " " + patternTarget + " " + patternLocation
            ));
        intents.put(
            "HomeAutomation.TurnOff",
            Arrays.asList(
                patternKeywordOptional + " " + patternTurnOff + " " + patternTarget,
                patternKeywordOptional + " " + patternTurnOff + " " + patternTarget + " " + patternLocation
            ));

        System.out.println("Added intents");
        intents.forEach((id, phrases) ->
        {
            model.getIntents().put(new PatternMatchingIntent(id, phrases));
            System.out.println("  id=\"" + id + "\"");
            for (String phrase : phrases)
            {
                System.out.println("    phrase=\"" + phrase + "\"");
            }
        });
        System.out.println("where");
        System.out.print("  targetName can be one of ");
        for (String targetName : targetNames)
        {
            System.out.print("\"" + targetName + "\" ");
        }
        System.out.println();
        System.out.print("  locationName can be one of ");
        for (String locationName : locationNames)
        {
            System.out.print("\"" + locationName + "\" ");
        }
        System.out.println("\n");

        return model;
    }


    private static void recognizeIntentAsync(boolean useKeyword) throws InterruptedException, ExecutionException
    {
        // Creates an instance of an embedded speech config.
        EmbeddedSpeechConfig speechConfig = Settings.createEmbeddedSpeechConfig();

        // Creates an intent recognizer using microphone as audio input.
        AudioConfig audioConfig = AudioConfig.fromDefaultMicrophoneInput();
        IntentRecognizer recognizer = new IntentRecognizer(speechConfig, audioConfig);

        // Create a pattern matching model.
        PatternMatchingModel patternMatchingModel = createPatternMatchingModel();
        // Add the model to a new language model collection
        List<LanguageUnderstandingModel> modelCollection = new ArrayList<LanguageUnderstandingModel>();
        modelCollection.add(patternMatchingModel);
        // Apply the language model collection to the recognizer.
        // This replaces all previously applied models.
        recognizer.applyLanguageModels(modelCollection);

        // Subscribes to events.
        recognizer.recognizing.addEventListener((s, e) ->
        {
            // Intermediate result (hypothesis).
            if (e.getResult().getReason() == ResultReason.RecognizingSpeech)
            {
                System.out.println("Recognizing:" + e.getResult().getText());
            }
            else if (e.getResult().getReason() == ResultReason.RecognizingKeyword)
            {
                // ignored
            }
        });

        recognizer.recognized.addEventListener((s, e) ->
        {
            if (e.getResult().getReason() == ResultReason.RecognizedKeyword)
            {
                // Keyword detected, speech recognition will start.
                System.out.println("KEYWORD: Text=" + e.getResult().getText());
            }
            else if (e.getResult().getReason() == ResultReason.RecognizedIntent)
            {
                // Final result (intent recognized). May differ from the last intermediate result.
                System.out.println("RECOGNIZED: Text=" + e.getResult().getText());
                System.out.println("  Intent Id: " + e.getResult().getIntentId());

                if (e.getResult().getIntentId().contains("HomeAutomation"))
                {
                    Dictionary<String, String> entities = e.getResult().getEntities();
                    if (entities.get("targetName") != null)
                    {
                        System.out.println("     Target: " + entities.get("targetName"));
                        if (entities.get("locationName") != null)
                        {
                            System.out.println("   Location: " + entities.get("locationName"));
                        }
                    }
                }
            }
            else if (e.getResult().getReason() == ResultReason.RecognizedSpeech)
            {
                // Final result (no intent recognized). May differ from the last intermediate result.
                System.out.println("RECOGNIZED: Text=" + e.getResult().getText());
                System.out.println("  (intent not recognized)");
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

        if (useKeyword)
        {
            // Creates an instance of a keyword recognition model.
            KeywordRecognitionModel keywordModel = KeywordRecognitionModel.fromFile(Settings.getKeywordModelFileName());

            // Starts the following routine:
            // 1. Listen for a keyword in input audio. There is no timeout.
            //    Speech that does not start with the keyword is ignored.
            // 2. If the keyword is spotted, start normal speech recognition.
            // 3. After a recognition result (that always includes at least
            //    the keyword), go back to step 1.
            // Steps 1-3 repeat until stopKeywordRecognitionAsync() is called.
            recognizer.startKeywordRecognitionAsync(keywordModel).get();

            System.out.println("Say something starting with \"" + Settings.getKeywordPhrase() + "\" (press Enter to stop)...");
            @SuppressWarnings({ "resource", "unused" })
            String input = new Scanner(System.in).nextLine();

            // Stops recognition.
            recognizer.stopKeywordRecognitionAsync().get();
        }
        else
        {
            // Starts continuous recognition.
            recognizer.startContinuousRecognitionAsync().get();

            System.out.println("Say something (press Enter to stop)...");
            @SuppressWarnings({ "resource", "unused" })
            String input = new Scanner(System.in).nextLine();

            // Stops recognition.
            recognizer.stopContinuousRecognitionAsync().get();
        }

        recognizer.close();
        audioConfig.close();
        speechConfig.close();
    }


    // Recognizes intents using embedded speech config and the system default microphone device.
    public static void embeddedRecognitionFromMicrophone() throws InterruptedException, ExecutionException
    {
        boolean useKeyword = false;
        recognizeIntentAsync(useKeyword);
    }


    // Recognizes intents using embedded speech config and the system default microphone device.
    // Recognition is triggered with a keyword.
    public static void embeddedRecognitionWithKeywordFromMicrophone() throws InterruptedException, ExecutionException
    {
        boolean useKeyword = true;
        recognizeIntentAsync(useKeyword);
    }
}
