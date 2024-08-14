//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//
package com.microsoft.cognitiveservices.speech.samples.embedded;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ExecutionException;

import com.microsoft.cognitiveservices.speech.*;


public class Settings
{
    // START OF CONFIGURABLE SETTINGS

    // Embedded speech model license (text).
    // This applies to embedded speech recognition, synthesis and translation.
    // It is presumed that all the customer's embedded speech models use the same license.
    private static final String EmbeddedSpeechModelLicense = "YourEmbeddedSpeechModelLicense"; // or set EMBEDDED_SPEECH_MODEL_LICENSE

    // Path to the local embedded speech recognition model(s) on the device file system.
    // This may be a single model folder or a top-level folder for several models.
    // Use an absolute path or a path relative to the application working folder.
    // The path is recursively searched for model files.
    // Files belonging to a specific model must be available as normal individual files in a model folder,
    // not inside an archive, and they must be readable by the application process.
    private static final String EmbeddedSpeechRecognitionModelPath = "YourEmbeddedSpeechRecognitionModelPath"; // or set EMBEDDED_SPEECH_RECOGNITION_MODEL_PATH

    // Name of the embedded speech recognition model to be used for recognition.
    // For example: "en-US" or "Microsoft Speech Recognizer en-US FP Model V8"
    private static final String EmbeddedSpeechRecognitionModelName = "YourEmbeddedSpeechRecognitionModelName"; // or set EMBEDDED_SPEECH_RECOGNITION_MODEL_NAME

    // Path to the local embedded speech synthesis voice(s) on the device file system.
    // This may be a single voice folder or a top-level folder for several voices.
    // Use an absolute path or a path relative to the application working folder.
    // The path is recursively searched for voice files.
    // Files belonging to a specific voice must be available as normal individual files in a voice folder,
    // not inside an archive, and they must be readable by the application process.
    private static final String EmbeddedSpeechSynthesisVoicePath = "YourEmbeddedSpeechSynthesisVoicePath"; // or set EMBEDDED_SPEECH_SYNTHESIS_VOICE_PATH

    // Name of the embedded speech synthesis voice to be used for synthesis.
    // For example: "en-US-JennyNeural" or "Microsoft Server Speech Text to Speech Voice (en-US, JennyNeural)"
    private static final String EmbeddedSpeechSynthesisVoiceName = "YourEmbeddedSpeechSynthesisVoiceName"; // or set EMBEDDED_SPEECH_SYNTHESIS_VOICE_NAME

    // Path to the local embedded speech translation model(s) on the device file system.
    // This may be a single model folder or a top-level folder for several models.
    // Use an absolute path or a path relative to the application working folder.
    // The path is recursively searched for model files.
    // Files belonging to a specific model must be available as normal individual files in a model folder,
    // not inside an archive, and they must be readable by the application process.
    private static final String EmbeddedSpeechTranslationModelPath = "YourEmbeddedSpeechTranslationModelPath"; // or set EMBEDDED_SPEECH_TRANSLATION_MODEL_PATH

    // Name of the embedded speech translation model to be used for translation.
    // For example: "Microsoft Speech Translator Many-to-English Model V2"
    private static final String EmbeddedSpeechTranslationModelName = "YourEmbeddedSpeechTranslationModelName"; // or set EMBEDDED_SPEECH_TRANSLATION_MODEL_NAME

    // Cloud speech service subscription and language settings.
    // These are needed with hybrid (cloud & embedded) speech configuration.
    // The language must be specified in BCP-47 format, case-sensitive.
    private static final String CloudSpeechSubscriptionKey = "YourCloudSpeechSubscriptionKey"; // or set CLOUD_SPEECH_SUBSCRIPTION_KEY
    private static final String CloudSpeechServiceRegion = "YourCloudSpeechServiceRegion"; // or set CLOUD_SPEECH_SERVICE_REGION
    private static final String CloudSpeechRecognitionLanguage = "en-US"; // or set CLOUD_SPEECH_RECOGNITION_LANGUAGE
    private static final String CloudSpeechSynthesisLanguage = "en-US"; // or set CLOUD_SPEECH_SYNTHESIS_LANGUAGE

    // END OF CONFIGURABLE SETTINGS


    // Embedded speech recognition default input audio format settings.
    // In addition, little-endian signed integer samples are required.
    public static long getEmbeddedSpeechSamplesPerSecond() { return 16000; };  // or 8000
    public static short getEmbeddedSpeechBitsPerSample() { return 16; };       // DO NOT MODIFY; no other format supported
    public static short getEmbeddedSpeechChannels() { return 1; };             // DO NOT MODIFY; no other format supported

    // Get names and other properties of example files included with the sample project.
    public static String getSpeechRawAudioFileName() { return "data/speech_test.raw"; }
    public static String getSpeechWavAudioFileName() { return "data/speech_test.wav"; }
    public static String getPerfTestAudioFileName() { return "data/performance_test.wav"; }

    // For more information about keyword recognition and models, see
    // https://docs.microsoft.com/azure/cognitive-services/speech-service/keyword-recognition-overview
    public static String getKeywordModelFileName() { return "data/keyword_computer.table"; }
    public static String getKeywordPhrase() { return "Computer"; }

    // Get a setting value from environment or defaults.
    private static String getSetting(String environmentVariableName, String defaultValue)
    {
        String value = System.getenv(environmentVariableName);
        return (value != null && !value.isEmpty()) ? value : defaultValue;
    }


    // These are set in verifySettings() after some basic verification.
    private static String SpeechModelLicense;
    private static String SpeechRecognitionModelPath;
    private static String SpeechRecognitionModelName;
    private static String SpeechSynthesisVoicePath;
    private static String SpeechSynthesisVoiceName;
    private static String SpeechTranslationModelPath;
    private static String SpeechTranslationModelName;

    // Utility functions for main menu.
    public static boolean hasSpeechRecognitionModel()
    {
        if (SpeechRecognitionModelPath.isEmpty() || SpeechRecognitionModelName.isEmpty())
        {
            System.err.println("## ERROR: No speech recognition model specified.");
            return false;
        }
        return true;
    }

    public static boolean hasSpeechSynthesisVoice()
    {
        if (SpeechSynthesisVoicePath.isEmpty() || SpeechSynthesisVoiceName.isEmpty())
        {
            System.err.println("## ERROR: No speech synthesis voice specified.");
            return false;
        }
        return true;
    }

    public static boolean hasSpeechTranslationModel()
    {
        if (SpeechTranslationModelPath.isEmpty() || SpeechTranslationModelName.isEmpty())
        {
            System.err.println("## ERROR: No speech translation model specified.");
            return false;
        }
        return true;
    }

    // Creates an instance of an embedded speech config.
    public static EmbeddedSpeechConfig createEmbeddedSpeechConfig() throws InterruptedException, ExecutionException
    {
        List<String> paths = new ArrayList<>();

        // Add paths for offline data.
        if (!SpeechRecognitionModelPath.isEmpty())
        {
            paths.add(SpeechRecognitionModelPath);
        }
        if (!SpeechSynthesisVoicePath.isEmpty())
        {
            paths.add(SpeechSynthesisVoicePath);
        }
        if (!SpeechTranslationModelPath.isEmpty())
        {
            paths.add(SpeechTranslationModelPath);
        }

        if (paths.size() == 0)
        {
            System.err.println("## ERROR: No model path(s) specified.");
            return null;
        }

        // Note, if there is only one path then you can also use EmbeddedSpeechConfig.fromPath(String).
        // All paths must be valid directory paths on the file system, otherwise e.g. initialization of
        // embedded speech synthesis will fail.
        EmbeddedSpeechConfig config = EmbeddedSpeechConfig.fromPaths(paths);

        // Enable Speech SDK logging. If you want to report an issue, include this log with the report.
        // If no path is specified, the log file will be created in the program default working folder.
        // If a path is specified, make sure that it is writable by the application process.
        /*
        config.setProperty(PropertyId.Speech_LogFilename, "SpeechSDK.log");
        */

        if (!SpeechRecognitionModelName.isEmpty())
        {
            // Mandatory configuration for embedded speech (and intent) recognition.
            config.setSpeechRecognitionModel(SpeechRecognitionModelName, SpeechModelLicense);
        }

        if (!SpeechSynthesisVoiceName.isEmpty())
        {
            // Mandatory configuration for embedded speech synthesis.
            config.setSpeechSynthesisVoice(SpeechSynthesisVoiceName, SpeechModelLicense);
            if (SpeechSynthesisVoiceName.contains("Neural"))
            {
                // Embedded neural voices only support 24kHz sample rate.
                config.setSpeechSynthesisOutputFormat(SpeechSynthesisOutputFormat.Riff24Khz16BitMonoPcm);
            }
        }

        if (!SpeechTranslationModelName.isEmpty())
        {
            // Mandatory configuration for embedded speech translation.
            config.setSpeechTranslationModel(SpeechTranslationModelName, SpeechModelLicense);
        }

        // Disable profanity masking.
        /*
        config.setProfanity(ProfanityOption.Raw);
        */

        return config;
    }


    // Creates an instance of a hybrid (cloud & embedded) speech config.
    public static HybridSpeechConfig createHybridSpeechConfig() throws InterruptedException, ExecutionException
    {
        SpeechConfig cloudSpeechConfig = SpeechConfig.fromSubscription(
                getSetting("CLOUD_SPEECH_SUBSCRIPTION_KEY", CloudSpeechSubscriptionKey),
                getSetting("CLOUD_SPEECH_SERVICE_REGION", CloudSpeechServiceRegion)
                );
        // Optional language configuration for cloud speech services.
        // The internal default is en-US.
        // Also see
        // https://github.com/Azure-Samples/cognitive-services-speech-sdk/tree/master/samples/java/jre/console
        // for complete Speech SDK samples using cloud speech services.
        cloudSpeechConfig.setSpeechRecognitionLanguage(getSetting("CLOUD_SPEECH_RECOGNITION_LANGUAGE", CloudSpeechRecognitionLanguage));
        cloudSpeechConfig.setSpeechSynthesisLanguage(getSetting("CLOUD_SPEECH_SYNTHESIS_LANGUAGE", CloudSpeechSynthesisLanguage));

        EmbeddedSpeechConfig embeddedSpeechConfig = createEmbeddedSpeechConfig();

        HybridSpeechConfig config = HybridSpeechConfig.fromConfigs(cloudSpeechConfig, embeddedSpeechConfig);

        return config;
    }


    // Do some basic verification of embedded speech settings.
    public static boolean verifySettings() throws InterruptedException, ExecutionException
    {
        String cwd = System.getProperty("user.dir");
        System.out.println("Current working directory: " + cwd);

        SpeechModelLicense = getSetting("EMBEDDED_SPEECH_MODEL_LICENSE", EmbeddedSpeechModelLicense);
        if (SpeechModelLicense.isEmpty() || SpeechModelLicense.equals("YourEmbeddedSpeechModelLicense"))
        {
            System.err.println("## ERROR: The embedded speech model license is not set.");
            return false;
        }

        SpeechRecognitionModelPath = getSetting("EMBEDDED_SPEECH_RECOGNITION_MODEL_PATH", EmbeddedSpeechRecognitionModelPath);
        if (SpeechRecognitionModelPath.equals("YourEmbeddedSpeechRecognitionModelPath"))
        {
            SpeechRecognitionModelPath = "";
        }
        SpeechRecognitionModelName = getSetting("EMBEDDED_SPEECH_RECOGNITION_MODEL_NAME", EmbeddedSpeechRecognitionModelName);
        if (SpeechRecognitionModelName.equals("YourEmbeddedSpeechRecognitionModelName"))
        {
            SpeechRecognitionModelName = "";
        }

        SpeechSynthesisVoicePath = getSetting("EMBEDDED_SPEECH_SYNTHESIS_VOICE_PATH", EmbeddedSpeechSynthesisVoicePath);
        if (SpeechSynthesisVoicePath.equals("YourEmbeddedSpeechSynthesisVoicePath"))
        {
            SpeechSynthesisVoicePath = "";
        }
        SpeechSynthesisVoiceName = getSetting("EMBEDDED_SPEECH_SYNTHESIS_VOICE_NAME", EmbeddedSpeechSynthesisVoiceName);
        if (SpeechSynthesisVoiceName.equals("YourEmbeddedSpeechSynthesisVoiceName"))
        {
            SpeechSynthesisVoiceName = "";
        }

        SpeechTranslationModelPath = getSetting("EMBEDDED_SPEECH_TRANSLATION_MODEL_PATH", EmbeddedSpeechTranslationModelPath);
        if (SpeechTranslationModelPath.equals("YourEmbeddedSpeechTranslationModelPath"))
        {
            SpeechTranslationModelPath = "";
        }
        SpeechTranslationModelName = getSetting("EMBEDDED_SPEECH_TRANSLATION_MODEL_NAME", EmbeddedSpeechTranslationModelName);
        if (SpeechTranslationModelName.equals("YourEmbeddedSpeechTranslationModelName"))
        {
            SpeechTranslationModelName = "";
        }

        // Find an embedded speech recognition model based on the name.
        if (!SpeechRecognitionModelPath.isEmpty() && !SpeechRecognitionModelName.isEmpty())
        {
            EmbeddedSpeechConfig config = EmbeddedSpeechConfig.fromPath(SpeechRecognitionModelPath);
            List<SpeechRecognitionModel> models = config.getSpeechRecognitionModels();

            final String name = SpeechRecognitionModelName;
            SpeechRecognitionModel result = models.stream()
                .filter(model ->
                    model.getName().equals(name) || model.getLocales().get(0).equals(name))
                .findAny()
                .orElse(null);

            if (result == null)
            {
                System.out.println("## WARNING: Cannot locate an embedded speech recognition model \"" + SpeechRecognitionModelName + "\"");
            }
        }

        // Find an embedded speech synthesis voice based on the name.
        if (!SpeechSynthesisVoicePath.isEmpty() && !SpeechSynthesisVoiceName.isEmpty())
        {
            EmbeddedSpeechConfig config = EmbeddedSpeechConfig.fromPath(SpeechSynthesisVoicePath);
            SpeechSynthesizer synthesizer = new SpeechSynthesizer(config, null);

            boolean found = false;
            SynthesisVoicesResult voicesList = synthesizer.getVoicesAsync("").get();

            if (voicesList.getReason() == ResultReason.VoicesListRetrieved)
            {
                final String name = SpeechSynthesisVoiceName;
                VoiceInfo result = voicesList.getVoices().stream()
                    .filter(voice ->
                        voice.getName().equals(name) || voice.getShortName().equals(name))
                    .findAny()
                    .orElse(null);

                if (result != null)
                {
                    found = true;
                }
            }

            voicesList.close();
            synthesizer.close();
            config.close();

            if (!found)
            {
                System.out.println("## WARNING: Cannot locate an embedded speech synthesis voice \"" + SpeechSynthesisVoiceName + "\"");
            }
        }

        // Find an embedded speech translation model based on the name.
        if (!SpeechTranslationModelPath.isEmpty() && !SpeechTranslationModelName.isEmpty())
        {
            EmbeddedSpeechConfig config = EmbeddedSpeechConfig.fromPath(SpeechTranslationModelPath);
            List<SpeechTranslationModel> models = config.getSpeechTranslationModels();

            final String name = SpeechTranslationModelName;
            SpeechTranslationModel result = models.stream()
                .filter(model -> model.getName().equals(name))
                .findAny()
                .orElse(null);

            if (result == null)
            {
                System.out.println("## WARNING: Cannot locate an embedded speech translation model \"" + SpeechTranslationModelName + "\"");
            }
        }

        System.out.println("Embedded speech recognition");
        System.out.println("  model search path: " + (SpeechRecognitionModelPath.isEmpty() ? "(not set)" : SpeechRecognitionModelPath));
        System.out.println("  model name:        " + (SpeechRecognitionModelName.isEmpty() ? "(not set)" : SpeechRecognitionModelName));
        System.out.println("Embedded speech synthesis");
        System.out.println("  voice search path: " + (SpeechSynthesisVoicePath.isEmpty() ? "(not set)" : SpeechSynthesisVoicePath));
        System.out.println("  voice name:        " + (SpeechSynthesisVoiceName.isEmpty() ? "(not set)" : SpeechSynthesisVoiceName));
        System.out.println("Embedded speech translation");
        System.out.println("  model search path: " + (SpeechTranslationModelPath.isEmpty() ? "(not set)" : SpeechTranslationModelPath));
        System.out.println("  model name:        " + (SpeechTranslationModelName.isEmpty() ? "(not set)" : SpeechTranslationModelName));

        return true;
    }
}
