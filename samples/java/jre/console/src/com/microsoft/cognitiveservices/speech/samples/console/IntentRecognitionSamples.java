package com.microsoft.cognitiveservices.speech.samples.console;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import java.io.IOException;
import java.util.ArrayList;
import java.util.Dictionary;
import java.util.List;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.Semaphore;

// <toplevel>
import com.microsoft.cognitiveservices.speech.*;
import com.microsoft.cognitiveservices.speech.audio.*;
import com.microsoft.cognitiveservices.speech.intent.*;
// </toplevel>

@SuppressWarnings("resource") // scanner
public class IntentRecognitionSamples {

    // Intent recognition using microphone.
    public static void intentRecognitionWithMicrophone() throws InterruptedException, ExecutionException
    {
        // <IntentRecognitionWithMicrophone>
        // Creates an instance of a speech config with specified
        // subscription key (called 'endpoint key' by the Language Understanding service)
        // and service region. Replace with your own subscription (endpoint) key
        // and service region (e.g., "westus2").
        // The default language is "en-us".
        SpeechConfig config = SpeechConfig.fromSubscription("YourLanguageUnderstandingSubscriptionKey", "YourLanguageUnderstandingServiceRegion");

        // Creates an intent recognizer using microphone as audio input.
        IntentRecognizer recognizer = new IntentRecognizer(config);

        // Creates a language understanding model using the app id, and adds specific intents from your model
        LanguageUnderstandingModel model = LanguageUnderstandingModel.fromAppId("YourLanguageUnderstandingAppId");
        recognizer.addIntent(model, "YourLanguageUnderstandingIntentName1", "id1");
        recognizer.addIntent(model, "YourLanguageUnderstandingIntentName2", "id2");
        recognizer.addIntent(model, "YourLanguageUnderstandingIntentName3", "any-IntentId-here");

        System.out.println("Say something...");

        // Starts recognition. It returns when the first utterance has been recognized.
        IntentRecognitionResult result = recognizer.recognizeOnceAsync().get();

        // Checks result.
        if (result.getReason() == ResultReason.RecognizedIntent) {
            System.out.println("RECOGNIZED: Text=" + result.getText());
            System.out.println("    Intent Id: " + result.getIntentId());
            System.out.println("    Intent Service JSON: " + result.getProperties().getProperty(PropertyId.LanguageUnderstandingServiceResponse_JsonResult));
        }
        else if (result.getReason() == ResultReason.RecognizedSpeech) {
            System.out.println("RECOGNIZED: Text=" + result.getText());
            System.out.println("    Intent not recognized.");
        }
        else if (result.getReason() == ResultReason.NoMatch) {
            System.out.println("NOMATCH: Speech could not be recognized.");
        }
        else if (result.getReason() == ResultReason.Canceled) {
            CancellationDetails cancellation = CancellationDetails.fromResult(result);
            System.out.println("CANCELED: Reason=" + cancellation.getReason());

            if (cancellation.getReason() == CancellationReason.Error) {
                System.out.println("CANCELED: ErrorCode=" + cancellation.getErrorCode());
                System.out.println("CANCELED: ErrorDetails=" + cancellation.getErrorDetails());
                System.out.println("CANCELED: Did you update the subscription info?");
            }
        }
        // </IntentRecognitionWithMicrophone>
    }

    // Intent recognition in the specified language, using microphone.
    public static void intentRecognitionWithLanguage() throws InterruptedException, ExecutionException
    {
        // <IntentRecognitionWithLanguage>
        // Creates an instance of a speech config with specified
        // subscription key (called 'endpoint key' by the Language Understanding service)
        // and service region. Replace with your own subscription (endpoint) key
        // and service region (e.g., "westus2").
        SpeechConfig config = SpeechConfig.fromSubscription("YourLanguageUnderstandingSubscriptionKey", "YourLanguageUnderstandingServiceRegion");

        // Creates an intent recognizer in the specified language using microphone as audio input.
        String lang = "de-de";
        config.setSpeechRecognitionLanguage(lang);
        IntentRecognizer recognizer = new IntentRecognizer(config);

        // Creates a language understanding model using the app id, and adds specific intents from your model
        LanguageUnderstandingModel model = LanguageUnderstandingModel.fromAppId("YourLanguageUnderstandingAppId");
        recognizer.addIntent(model, "YourLanguageUnderstandingIntentName1", "id1");
        recognizer.addIntent(model, "YourLanguageUnderstandingIntentName2", "id2");
        recognizer.addIntent(model, "YourLanguageUnderstandingIntentName3", "any-IntentId-here");

        System.out.println("Say something...");

        // Starts recognition. It returns when the first utterance has been recognized.
        IntentRecognitionResult result = recognizer.recognizeOnceAsync().get();

        // Checks result.
        if (result.getReason() == ResultReason.RecognizedIntent) {
            System.out.println("RECOGNIZED: Text=" + result.getText());
            System.out.println("    Intent Id: " + result.getIntentId());
            System.out.println("    Intent Service JSON: " + result.getProperties().getProperty(PropertyId.LanguageUnderstandingServiceResponse_JsonResult));
        }
        else if (result.getReason() == ResultReason.RecognizedSpeech) {
            System.out.println("RECOGNIZED: Text=" + result.getText());
            System.out.println("    Intent not recognized.");
        }
        else if (result.getReason() == ResultReason.NoMatch) {
            System.out.println("NOMATCH: Speech could not be recognized.");
        }
        else if (result.getReason() == ResultReason.Canceled) {
            CancellationDetails cancellation = CancellationDetails.fromResult(result);
            System.out.println("CANCELED: Reason=" + cancellation.getReason());

            if (cancellation.getReason() == CancellationReason.Error) {
                System.out.println("CANCELED: ErrorCode=" + cancellation.getErrorCode());
                System.out.println("CANCELED: ErrorDetails=" + cancellation.getErrorDetails());
                System.out.println("CANCELED: Did you update the subscription info?");
            }
        }
        // </IntentRecognitionWithLanguage>
    }

    // Continuous intent recognition.
    public static void intentContinuousRecognitionWithFile() throws InterruptedException, ExecutionException, IOException
    {
        // <IntentContinuousRecognitionWithFile>
        stopRecognitionSemaphore = new Semaphore(0);

        // Creates an instance of a speech config with specified
        // subscription key (called 'endpoint key' by the Language Understanding service)
        // and service region. Replace with your own subscription (endpoint) key
        // and service region (e.g., "westus2").
        SpeechConfig config = SpeechConfig.fromSubscription("YourLanguageUnderstandingSubscriptionKey", "YourLanguageUnderstandingServiceRegion");

        // Creates an intent recognizer using file as audio input.
        // Replace with your own audio file name.
        AudioConfig audioInput = AudioConfig.fromWavFileInput("YourAudioFile.wav");
        IntentRecognizer recognizer = new IntentRecognizer(config, audioInput);

        // Creates a language understanding model using the app id, and adds specific intents from your model
        LanguageUnderstandingModel model = LanguageUnderstandingModel.fromAppId("YourLanguageUnderstandingAppId");
        recognizer.addIntent(model, "YourLanguageUnderstandingIntentName1", "id1");
        recognizer.addIntent(model, "YourLanguageUnderstandingIntentName2", "id2");
        recognizer.addIntent(model, "YourLanguageUnderstandingIntentName3", "any-IntentId-here");

        // Subscribes to events.
        recognizer.recognizing.addEventListener((s, e) -> {
            System.out.println("RECOGNIZING: Text=" + e.getResult().getText());
        });

        recognizer.recognized.addEventListener((s, e) -> {
            if (e.getResult().getReason() == ResultReason.RecognizedIntent) {
                System.out.println("RECOGNIZED: Text=" + e.getResult().getText());
                System.out.println("    Intent Id: " + e.getResult().getIntentId());
                System.out.println("    Intent Service JSON: " + e.getResult().getProperties().getProperty(PropertyId.LanguageUnderstandingServiceResponse_JsonResult));
            }
            else if (e.getResult().getReason() == ResultReason.RecognizedSpeech) {
                System.out.println("RECOGNIZED: Text=" + e.getResult().getText());
                System.out.println("    Intent not recognized.");
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

            stopRecognitionSemaphore.release();
        });

        // Starts continuous recognition. Uses StopContinuousRecognitionAsync() to stop recognition.
        recognizer.startContinuousRecognitionAsync().get();

        // Waits for completion.
        stopRecognitionSemaphore.acquire();

        // Stops recognition.
        recognizer.stopContinuousRecognitionAsync().get();
        // </IntentContinuousRecognitionWithFile>
    }

    // Use pattern matching for intent recognition from your default microphone input
    public static void IntentPatternMatchingWithMicrophone() throws InterruptedException, ExecutionException
    {
        // <IntentPatternMatchingWithMicrophone>
        // Creates an instance of a speech config with specified subscription key and service region. Note that in
        // contrast to the other samples this DOES NOT require a LUIS application.
        // The default recognition language is "en-us".
        SpeechConfig config = SpeechConfig.fromSubscription(
            "YourSubscriptionKey",
            "YourServiceRegion");

        // Creates an intent recognizer using microphone as audio input.
        try (IntentRecognizer recognizer = new IntentRecognizer(config))
        {

            // Creates a Pattern Matching model and adds specific intents from your model. The
            // Id is used to identify this model from others in the collection.
            PatternMatchingModel model = new PatternMatchingModel("YourPatternMatchingModelId");

            // Creates a string with a pattern that uses groups of optional words. Optional phrases in square brackets can
            // select one phrase from several choices by separating them inside the brackets with a pipe '|'. Here,
            // "[Go | Take me]" will match either "Go", "Take me", or "". Note the space after the keyword.
            String patternWithOptionalWords = "[Go | Take me] to [floor|level] {floorName}";

            // Creates a string with a pattern that uses an optional entity and group that could be used to tie commands
            // together. Optional patterns in square brackets can also include a reference to an entity. "[{parkingLevel}]"
            // includes a match against the named entity as an optional component in this pattern.
            String patternWithOptionalEntity = "Go to parking [{parkingLevel}]";

            // You can also have multiple entities of the same name in a single pattern by adding appending a unique identifier
            // to distinguish between the instances. For example:
            String patternWithTwoOfTheSameEntity = "Go to floor {floorName:1} [and then go to floor {floorName:2}]";
            // NOTE: Both floorName:1 and floorName:2 are tied to the same list of entries. The identifier can be a string
            //       and is separated from the entity name by a ':'

            // Adds some intents to look for specific patterns.
            model.getIntents().put(new PatternMatchingIntent(
                "ChangeFloors", patternWithOptionalWords, patternWithOptionalEntity, patternWithTwoOfTheSameEntity));
            model.getIntents().put(new PatternMatchingIntent(
                "DoorControl", "{action} the doors", "{action} doors", "{action} the door", "{action} door"));

            // Creates the "floorName" entity and set it to type list.
            // Adds acceptable values. NOTE the default entity type is Any and so we do not need
            // to declare the "action" entity.
            model.getEntities().put(PatternMatchingEntity.CreateListEntity(
                "floorName",
                PatternMatchingEntity.EntityMatchMode.Strict,
                "ground floor", "lobby", "1st", "first", "one", "1", "2nd", "second", "two", "2"));

            // Creates the "parkingLevel" entity as a pre-built integer
            model.getEntities().put(PatternMatchingEntity.CreateIntegerEntity("parkingLevel"));

            // Add the model to a new language model collection
            List<LanguageUnderstandingModel> modelCollection = new ArrayList<LanguageUnderstandingModel>();
            modelCollection.add(model);

            // Apply the language model collection to the recognizer.
            recognizer.applyLanguageModels(modelCollection);

            System.out.println("Say something...");

            // Starts intent recognition, and returns after a single utterance is recognized. The end of a single
            // utterance is determined by listening for silence at the end, or until a maximum of 15 seconds of audio
            // is processed. The task returns the recognition text as result. 
            // Note: Since RecognizeOnceAsync() returns only a single utterance, it is suitable only for single shot
            // recognition like command or query.
            // For long-running multi-utterance recognition, use StartContinuousRecognitionAsync() instead.
            IntentRecognitionResult result = recognizer.recognizeOnceAsync().get();

            // Checks result.
            if (result.getReason() == ResultReason.RecognizedIntent)
            {
                System.out.println("RECOGNIZED: Text= " + result.getText());
                System.out.println(String.format("%17s %s", "Intent Id= ", result.getIntentId() + "."));
                Dictionary<String, String> entities = result.getEntities();
                switch (result.getIntentId())
                {
                    case "ChangeFloors":
                        if (entities.get("floorName") != null)
                        {
                            System.out.println(String.format("%17s %s", "FloorName=", entities.get("floorName")));
                        }

                        if (entities.get("floorName:1") != null)
                        {
                            System.out.println(String.format("%17s %s", "floorName:1=", entities.get("floorName:1")));
                        }

                        if (entities.get("floorName:2") != null)
                        {
                            System.out.println(String.format("%17s %s", "floorName:2=", entities.get("floorName:2")));
                        }

                        if (entities.get("parkingLevel") != null)
                        {
                            System.out.println(String.format("%17s %s", "parkingLevel=", entities.get("parkingLevel")));
                        }
                        break;
                    case "DoorControl":
                        if (entities.get("action") != null)
                        {
                            System.out.println(String.format("%17s %s", "action=", entities.get("action")));
                        }
                        break;
                    default:
                        System.out.println("Unknown intent ID: " + result.getIntentId());
                        break;
                }
            }
            else if (result.getReason() == ResultReason.RecognizedSpeech)
            {
                System.out.println("RECOGNIZED: Text= " + result.getText());
                System.out.println(String.format("%17s", "Intent not recognized."));
            }
            else if (result.getReason() == ResultReason.NoMatch)
            {
                System.out.println("NOMATCH: Speech could not be recognized.");
            }
            else if (result.getReason() == ResultReason.Canceled)
            {
                CancellationDetails cancellation = CancellationDetails.fromResult(result);
                System.out.println("CANCELED: Reason=" + cancellation.getReason());

                if (cancellation.getReason() == CancellationReason.Error)
                {
                    System.out.println("CANCELED: ErrorCode=" + cancellation.getErrorCode());
                    System.out.println("CANCELED: ErrorDetails=" + cancellation.getErrorDetails());
                    System.out.println("CANCELED: Did you update the subscription info?");
                }
            }
        }
        // </IntentPatternMatchingWithMicrophone>
    }

    // The Source to stop recognition.
    private static Semaphore stopRecognitionSemaphore;
    /// Keyword-triggered intent recognition using microphone. This is useful for when you don't have a push-to-talk feature
    /// and want to activate your device with voice only. A keyword model is used for local recognition and activation.
    /// NOTE: It is possible to still call recognize once during a keyword spotting session if you want to have both
    /// push-to-talk and keyword activation.
    /// Example interaction: "Computer turn on the lights".
    public static void IntentPatternMatchingWithMicrophoneAndKeywordSpotting() throws InterruptedException, ExecutionException, IOException
    {
        // <IntentPatternMatchingWithMicrophoneAndKeywordSpotting>
        // Creates an instance of a speech config with specified subscription key and service region. Note that in
        // contrast to the other samples this DOES NOT require a LUIS application.
        // The default recognition language is "en-us".
        SpeechConfig config = SpeechConfig.fromSubscription(
            "YourSubscriptionKey",
            "YourServiceRegion");

        // Creates an instance of a keyword recognition model. Update this to
        // point to the location of your keyword recognition model.
        KeywordRecognitionModel keywordModel = KeywordRecognitionModel.fromFile("YourKeywordRecognitionModelFile.table");

        // The phrase your keyword recognition model triggers on.
        String keyword = "YourKeyword";

        // Creates an intent recognizer using microphone as audio input.
        try (IntentRecognizer recognizer = new IntentRecognizer(config))
        {

            // Create a string containing the keyword with the optional pattern tags on it. This can be useful if you
            // are using push to talk and keyword activation.
            String keywordOptionalPattern = "[" + keyword + "]";

            // Creates a Pattern Matching model and adds specific intents from your model. The Id is used to identify
            // this model from others in the collection.
            PatternMatchingModel patternMatchingModel = new PatternMatchingModel("YourPatternMatchingModelId");

            // Creates the "floorName" entity and set it to type list.
            // Adds acceptable values. NOTE the default entity type is Any and so we do not need
            // to declare the "action" entity.
            patternMatchingModel.getEntities().put(PatternMatchingEntity.CreateListEntity(
                "floorName",
                PatternMatchingEntity.EntityMatchMode.Strict,
                "ground floor", "lobby", "1st", "first", "one", "1", "2nd", "second", "two", "2"));

            // Creates the "parkingLevel" entity as a pre-built integer
            patternMatchingModel.getEntities().put(PatternMatchingEntity.CreateIntegerEntity("parkingLevel"));

            // Creates a string with a pattern that uses groups of optional words. Optional phrases in square brackets can
            // select one phrase from several choices by separating them inside the brackets with a pipe '|'. Here,
            // "[Go | Take me]" will match either "Go", "Take me", or "". Note the space after the keyword.
            String patternWithOptionalWords = keywordOptionalPattern + " " + "[Go | Take me] to [floor|level] {floorName}";

            // Creates a string with a pattern that uses an optional entity and group that could be used to tie commands
            // together. Optional patterns in square brackets can also include a reference to an entity. "[{parkingLevel}]"
            // includes a match against the named entity as an optional component in this pattern.
            String patternWithOptionalEntity = keywordOptionalPattern + " " + "Go to parking [{parkingLevel}]";

            // You can also have multiple entities of the same name in a single pattern by adding appending a unique identifier
            // to distinguish between the instances. For example:
            String patternWithTwoOfTheSameEntity = keywordOptionalPattern + " "
                + "Go to floor {floorName:1} [and then go to floor {floorName:2}]";
            // NOTE: Both floorName:1 and floorName:2 are tied to the same list of entries. The identifier can be a string
            //       and is separated from the entity name by a ':'

            // Adds some intents to look for specific patterns.
            patternMatchingModel.getIntents().put(new PatternMatchingIntent(
                "ChangeFloors", patternWithOptionalWords, patternWithOptionalEntity, patternWithTwoOfTheSameEntity));
            patternMatchingModel.getIntents().put(new PatternMatchingIntent("DoorControl",
                keywordOptionalPattern + " " + "{action} the doors",
                keywordOptionalPattern + " " + "{action} doors",
                keywordOptionalPattern + " " + "{action} the door",
                keywordOptionalPattern + " " + "{action} door"));

            // Add the model to a new language model collection
            List<LanguageUnderstandingModel> modelCollection = new ArrayList<LanguageUnderstandingModel>();
            modelCollection.add(patternMatchingModel);

            // Apply the language model collection to the recognizer.
            recognizer.applyLanguageModels(modelCollection);

            stopRecognitionSemaphore = new Semaphore(0);

            // Subscribes to events.
            recognizer.recognizing.addEventListener((s, e) -> {
                if (e.getResult().getReason() == ResultReason.RecognizingKeyword) {
                    System.out.println("RECOGNIZING KEYWORD: Text= " + e.getResult().getText());
                }
                else if (e.getResult().getReason() == ResultReason.RecognizingSpeech) {
                    System.out.println("RECOGNIZING: Text= " + e.getResult().getText());
                }
            });

            recognizer.recognized.addEventListener((s, e) -> {
                // Checks result.
                IntentRecognitionResult result = e.getResult();
                if (result.getReason() == ResultReason.RecognizedKeyword)
                {
                    System.out.println("RECOGNIZED KEYWORD: Text= " + e.getResult().getText());
                }
                else if (result.getReason() == ResultReason.RecognizedIntent)
                {
                    System.out.println("RECOGNIZED: Text= " + result.getText());
                    System.out.println(String.format("%17s %s", "Intent Id= ", result.getIntentId() + "."));

                    Dictionary<String, String> entities = result.getEntities();
                    switch (result.getIntentId())
                    {
                        case "ChangeFloors":
                        if (entities.get("floorName") != null)
                        {
                            System.out.println(String.format("%17s %s", "FloorName=", entities.get("floorName")));
                        }

                        if (entities.get("floorName:1") != null)
                        {
                            System.out.println(String.format("%17s %s", "floorName:1=", entities.get("floorName:1")));
                        }

                        if (entities.get("floorName:2") != null)
                        {
                            System.out.println(String.format("%17s %s", "floorName:2=", entities.get("floorName:2")));
                        }

                        if (entities.get("parkingLevel") != null)
                        {
                            System.out.println(String.format("%17s %s", "parkingLevel=", entities.get("parkingLevel")));
                        }
                        break;
                    case "DoorControl":
                        if (entities.get("action") != null)
                        {
                            System.out.println(String.format("%17s %s", "action=", entities.get("action")));
                        }
                        break;
                    default:
                        System.out.println("Unknown intent ID: " + result.getIntentId());
                        break;
                    }
                }
                else if (result.getReason() == ResultReason.RecognizedSpeech)
                {
                    System.out.println("RECOGNIZED: Text=" + result.getText());
                    System.out.println(String.format("%17s", "Intent not recognized."));
                }
                else if (result.getReason() == ResultReason.NoMatch)
                {
                    System.out.println("NOMATCH: Speech could not be recognized.");
                }
            });

            recognizer.canceled.addEventListener((s, e) -> {
                System.out.println("CANCELED: Reason=" + e.getReason());

                if (e.getReason() == CancellationReason.Error)
                {
                    System.out.println("CANCELED: ErrorCode=" + e.getErrorCode());
                    System.out.println("CANCELED: ErrorDetails=" + e.getErrorDetails());
                    System.out.println("CANCELED: Did you update the subscription info?");
                }
                stopRecognitionSemaphore.release();
            });

            recognizer.sessionStarted.addEventListener((s, e) -> {
                System.out.println("Session started event.");
            });

            recognizer.sessionStopped.addEventListener((s, e) -> {
                System.out.println("Session stopped event.");

                stopRecognitionSemaphore.release();
            });

            // Starts recognizing.
            System.out.println("Say something starting with the keyword '" + keyword +"' followed by whatever you want...");

            // Starts continuous recognition using the keyword model. Use
            // StopKeywordRecognitionAsync() to stop recognition.
            recognizer.startKeywordRecognitionAsync(keywordModel).get();

            // Waits for a single successful keyword-triggered speech recognition (or error).
            stopRecognitionSemaphore.acquire();

            // Stops recognition.
            recognizer.stopKeywordRecognitionAsync().get();
        }
        // </IntentPatternMatchingWithMicrophoneAndKeywordSpotting>
    }
}
