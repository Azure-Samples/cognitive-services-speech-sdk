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

    // Locale to be used in speech recognition (SR), cloud and embedded. In BCP-47 format, case-sensitive.
    // If EmbeddedSpeechRecognitionModelName is changed from the default, it will override this for embedded.
    private static final String SpeechRecognitionLocale = "en-US"; // or set SPEECH_RECOGNITION_LOCALE

    // Locale to be used in speech synthesis (text-to-speech, TTS), cloud and embedded. In BCP-47 format, case-sensitive.
    // If EmbeddedSpeechSynthesisVoiceName is changed from the default, it will override this for embedded.
    private static final String SpeechSynthesisLocale = "en-US"; // or set SPEECH_SYNTHESIS_LOCALE

    // Path to the local embedded speech recognition model(s) on the device file system.
    // This may be a single model folder or a top-level folder for several models.
    // Use an absolute path or a path relative to the application working folder.
    // The path is recursively searched for model files.
    // Files belonging to a specific model must be available as normal individual files in a model folder,
    // not inside an archive, and they must be readable by the application process.
    private static final String EmbeddedSpeechRecognitionModelPath = "YourEmbeddedSpeechRecognitionModelPath"; // or set EMBEDDED_SPEECH_RECOGNITION_MODEL_PATH

    // Name of the embedded speech recognition model to be used for recognition.
    // If changed from the default, this will override SpeechRecognitionLocale.
    // For example: "en-US" or "Microsoft Speech Recognizer en-US FP Model V8"
    private static final String EmbeddedSpeechRecognitionModelName = "YourEmbeddedSpeechRecognitionModelName"; // or set EMBEDDED_SPEECH_RECOGNITION_MODEL_NAME

    // Decryption key of the (encrypted) embedded speech recognition model.
    // WARNING: The key may be visible in the program binary if hard-coded as a plain string.
    private static final String EmbeddedSpeechRecognitionModelKey = "YourEmbeddedSpeechRecognitionModelKey"; // or set EMBEDDED_SPEECH_RECOGNITION_MODEL_KEY

    // Path to the local embedded speech synthesis voice(s) on the device file system.
    // This may be a single voice folder or a top-level folder for several voices.
    // Use an absolute path or a path relative to the application working folder.
    // The path is recursively searched for voice files.
    // Files belonging to a specific voice must be available as normal individual files in a voice folder,
    // not inside an archive, and they must be readable by the application process.
    private static final String EmbeddedSpeechSynthesisVoicePath = "YourEmbeddedSpeechSynthesisVoicePath"; // or set EMBEDDED_SPEECH_SYNTHESIS_VOICE_PATH

    // Name of the embedded speech synthesis voice to be used for synthesis.
    // If changed from the default, this will override SpeechSynthesisLocale.
    // For example: "en-US-JennyNeural" or "Microsoft Server Speech Text to Speech Voice (en-US, JennyNeural)"
    private static final String EmbeddedSpeechSynthesisVoiceName = "YourEmbeddedSpeechSynthesisVoiceName"; // or set EMBEDDED_SPEECH_SYNTHESIS_VOICE_NAME

    // Decryption key of the (encrypted) embedded speech synthesis voice.
    // WARNING: The key may be visible in the program binary if hard-coded as a plain string.
    private static final String EmbeddedSpeechSynthesisVoiceKey = "YourEmbeddedSpeechSynthesisVoiceKey"; // or set EMBEDDED_SPEECH_SYNTHESIS_VOICE_KEY

    // Cloud speech service subscription information.
    // This is needed with hybrid (cloud & embedded) speech configuration.
    private static final String CloudSpeechSubscriptionKey = "YourCloudSpeechSubscriptionKey"; // or set CLOUD_SPEECH_SUBSCRIPTION_KEY
    private static final String CloudSpeechServiceRegion = "YourCloudSpeechServiceRegion"; // or set CLOUD_SPEECH_SERVICE_REGION

    // END OF CONFIGURABLE SETTINGS


    // Embedded speech recognition input audio format settings.
    // In addition, little-endian signed integer samples are required.
    // THIS IS THE ONLY SUPPORTED AUDIO FORMAT AT THE MOMENT. DO NOT MODIFY.
    // These settings are just included here to make them explicit and visible.
    public static long getEmbeddedSpeechSamplesPerSecond() { return 16000; };
    public static short getEmbeddedSpeechBitsPerSample() { return 16; };
    public static short getEmbeddedSpeechChannels() { return 1; };

    // Get names and other properties of example files included with the sample project.
    public static String getSpeechRawAudioFileName() { return "data/speech_test.raw"; }
    public static String getSpeechWavAudioFileName() { return "data/speech_test.wav"; }
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
    private static String SpeechRecognitionModelName;
    private static String SpeechRecognitionModelKey;
    private static String SpeechSynthesisVoiceName;
    private static String SpeechSynthesisVoiceKey;

    // Creates an instance of an embedded speech config.
    public static EmbeddedSpeechConfig createEmbeddedSpeechConfig() throws InterruptedException, ExecutionException
    {
        List<String> paths = new ArrayList<>();

        // Add paths for offline data.
        String modelPath = getSetting("EMBEDDED_SPEECH_RECOGNITION_MODEL_PATH", EmbeddedSpeechRecognitionModelPath);
        if (!modelPath.isEmpty() && !modelPath.equals("YourEmbeddedSpeechRecognitionModelPath"))
        {
            paths.add(modelPath);
        }
        String voicePath = getSetting("EMBEDDED_SPEECH_SYNTHESIS_VOICE_PATH", EmbeddedSpeechSynthesisVoicePath);
        if (!voicePath.isEmpty() && !voicePath.equals("YourEmbeddedSpeechSynthesisVoicePath"))
        {
            paths.add(voicePath);
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
            // Mandatory configuration for embedded speech recognition.
            config.setSpeechRecognitionModel(SpeechRecognitionModelName, SpeechRecognitionModelKey);
        }

        if (!SpeechSynthesisVoiceName.isEmpty())
        {
            // Mandatory configuration for embedded speech synthesis.
            config.setSpeechSynthesisVoice(SpeechSynthesisVoiceName, SpeechSynthesisVoiceKey);
            if (SpeechSynthesisVoiceName.contains("Neural"))
            {
                // Embedded neural voices only support 24kHz sample rate.
                config.setSpeechSynthesisOutputFormat(SpeechSynthesisOutputFormat.Riff24Khz16BitMonoPcm);
            }
        }

        // Enable profanity masking.
        /*
        config.setProfanity(ProfanityOption.Masked);
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
        cloudSpeechConfig.setSpeechRecognitionLanguage(getSetting("SPEECH_RECOGNITION_LOCALE", SpeechRecognitionLocale));
        cloudSpeechConfig.setSpeechSynthesisLanguage(getSetting("SPEECH_SYNTHESIS_LOCALE", SpeechSynthesisLocale));

        EmbeddedSpeechConfig embeddedSpeechConfig = createEmbeddedSpeechConfig();

        HybridSpeechConfig config = HybridSpeechConfig.fromConfigs(cloudSpeechConfig, embeddedSpeechConfig);

        return config;
    }


    // Do some basic verification of embedded speech settings.
    public static boolean verifySettings() throws InterruptedException, ExecutionException
    {
        String cwd = System.getProperty("user.dir");
        System.out.println("Current working directory:      " + cwd);

        String modelPath = getSetting("EMBEDDED_SPEECH_RECOGNITION_MODEL_PATH", EmbeddedSpeechRecognitionModelPath);
        if (modelPath.isEmpty() || modelPath.equals("YourEmbeddedSpeechRecognitionModelPath"))
        {
            System.out.println("## WARNING: Embedded SR model search path is not set.");
            modelPath = "";
        }

        String voicePath = getSetting("EMBEDDED_SPEECH_SYNTHESIS_VOICE_PATH", EmbeddedSpeechSynthesisVoicePath);
        if (voicePath.isEmpty() || voicePath.equals("YourEmbeddedSpeechSynthesisVoicePath"))
        {
            System.out.println("## WARNING: Embedded TTS voice search path is not set.");
            voicePath = "";
        }

        if (modelPath.isEmpty() && voicePath.isEmpty())
        {
            System.err.println("## ERROR: Cannot run without embedded SR models or TTS voices.");
            return false;
        }

        // Find an embedded speech recognition model based on the name or locale.

        SpeechRecognitionModelName = "";

        if (!modelPath.isEmpty())
        {
            String modelName = getSetting("EMBEDDED_SPEECH_RECOGNITION_MODEL_NAME", EmbeddedSpeechRecognitionModelName);
            String modelLocale = getSetting("SPEECH_RECOGNITION_LOCALE", SpeechRecognitionLocale);

            if (modelName.isEmpty() || modelName.equals("YourEmbeddedSpeechRecognitionModelName"))
            {
                modelName = ""; // no name given -> search by locale
            }

            EmbeddedSpeechConfig config = EmbeddedSpeechConfig.fromPath(modelPath);
            List<SpeechRecognitionModel> models = config.getSpeechRecognitionModels();

            final String name = modelName;
            SpeechRecognitionModel result = models.stream()
                .filter(model ->
                    (name.isEmpty() && model.getLocales().get(0).equals(modelLocale)) ||
                    (!name.isEmpty() && model.getName().equals(name)))
                .findAny()
                .orElse(null);

            if (result != null)
            {
                SpeechRecognitionModelName = result.getName();
            }

            if (SpeechRecognitionModelName.isEmpty())
            {
                System.out.print("## WARNING: Cannot locate an embedded SR model by ");
                if (modelName.isEmpty())
                {
                    System.out.print("locale \"" + modelLocale + "\". ");
                }
                else
                {
                    System.out.print("name \"" + modelName + "\". ");
                }
                System.out.println("Current model search path: " + modelPath);
            }
            else
            {
                SpeechRecognitionModelKey = getSetting("EMBEDDED_SPEECH_RECOGNITION_MODEL_KEY", EmbeddedSpeechRecognitionModelKey);
                if (SpeechRecognitionModelKey.isEmpty() || SpeechRecognitionModelKey.equals("YourEmbeddedSpeechRecognitionModelKey"))
                {
                    SpeechRecognitionModelKey = "";
                    System.out.println("## WARNING: The key for \"" + SpeechRecognitionModelName + "\" is not set.");
                }
            }
        }

        // Find an embedded speech synthesis voice based on the name or locale.

        SpeechSynthesisVoiceName = "";

        if (!voicePath.isEmpty())
        {
            String voiceName = getSetting("EMBEDDED_SPEECH_SYNTHESIS_VOICE_NAME", EmbeddedSpeechSynthesisVoiceName);
            String voiceLocale = getSetting("SPEECH_SYNTHESIS_LOCALE", SpeechSynthesisLocale);

            if (voiceName.isEmpty() || voiceName.equals("YourEmbeddedSpeechSynthesisVoiceName"))
            {
                voiceName = ""; // no name given -> search by locale
            }

            EmbeddedSpeechConfig config = EmbeddedSpeechConfig.fromPath(voicePath);
            SpeechSynthesizer synthesizer = new SpeechSynthesizer(config, null);
            SynthesisVoicesResult voicesList = synthesizer.getVoicesAsync("").get();

            if (voicesList.getReason() == ResultReason.VoicesListRetrieved)
            {
                final String name = voiceName;
                VoiceInfo result = voicesList.getVoices().stream()
                    .filter(voice ->
                        (name.isEmpty() && voice.getLocale().equals(voiceLocale)) ||
                        (!name.isEmpty() && voice.getName().equals(name)))
                    .findAny()
                    .orElse(null);

                if (result != null)
                {
                    SpeechSynthesisVoiceName = result.getName();
                }
            }

            voicesList.close();
            synthesizer.close();
            config.close();

            if (SpeechSynthesisVoiceName.isEmpty())
            {
                System.out.print("## WARNING: Cannot locate an embedded TTS voice by ");
                if (voiceName.isEmpty())
                {
                    System.out.print("locale \"" + voiceLocale + "\". ");
                }
                else
                {
                    System.out.println("name \"" + voiceName + "\". ");
                }
                System.out.println("Current voice search path: " + voicePath);
            }
            else
            {
                SpeechSynthesisVoiceKey = getSetting("EMBEDDED_SPEECH_SYNTHESIS_VOICE_KEY", EmbeddedSpeechSynthesisVoiceKey);
                if (SpeechSynthesisVoiceKey.isEmpty() || SpeechSynthesisVoiceKey.equals("YourEmbeddedSpeechSynthesisVoiceKey"))
                {
                    SpeechSynthesisVoiceKey = "";
                    System.out.println("## WARNING: The key for \"" + SpeechSynthesisVoiceName + "\" is not set.");
                }
            }
        }

        if (SpeechRecognitionModelName.isEmpty() && SpeechSynthesisVoiceName.isEmpty())
        {
            System.err.println("## ERROR: Cannot run without embedded SR models or TTS voices.");
            return false;
        }

        System.out.println("Embedded SR model search path:  " + (modelPath.isEmpty() ? "(not set)" : modelPath));
        if (!modelPath.isEmpty())
        {
            System.out.println("Embedded SR model name:         " + (SpeechRecognitionModelName.isEmpty() ? "(not found)" : SpeechRecognitionModelName));
            if (!SpeechRecognitionModelName.isEmpty())
            {
                System.out.println("Embedded SR model key:          " + (SpeechRecognitionModelKey.isEmpty() ? "(not set)" : maskValue(SpeechRecognitionModelKey)));
            }
        }
        System.out.println("Embedded TTS voice search path: " + (voicePath.isEmpty() ? "(not set)" : voicePath));
        if (!voicePath.isEmpty())
        {
            System.out.println("Embedded TTS voice name:        " + (SpeechSynthesisVoiceName.isEmpty() ? "(not found)" : SpeechSynthesisVoiceName));
            if (!SpeechSynthesisVoiceName.isEmpty())
            {
                System.out.println("Embedded TTS voice key:         " + (SpeechSynthesisVoiceKey.isEmpty() ? "(not set)" : maskValue(SpeechSynthesisVoiceKey)));
            }
        }

        return true;
    }

    private static String maskValue(String value)
    {
        // Mask the string value, leave only the last 3 chars visible
        int visibleLength = value.length() > 3 ? 3 : 0;
        String masked = new String(new char[value.length() - visibleLength]).replace('\0', '*') + value.substring(value.length() - visibleLength);
        return masked;
    };
}
