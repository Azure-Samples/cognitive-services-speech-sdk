//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//

using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Runtime.ConstrainedExecution;
using System.Threading.Tasks;
using Microsoft.CognitiveServices.Speech;

namespace MicrosoftSpeechSDKSamples
{
    public class Settings
    {
        // START OF CONFIGURABLE SETTINGS

        // Embedded speech model license (text).
        // This applies to embedded speech recognition, synthesis and translation.
        // It is presumed that all the customer's embedded speech models use the same license.
        private static readonly string EmbeddedSpeechModelLicense = "YourEmbeddedSpeechModelLicense"; // or set EMBEDDED_SPEECH_MODEL_LICENSE

        // Path to the local embedded speech recognition model(s) on the device file system.
        // This may be a single model folder or a top-level folder for several models.
        // Use an absolute path or a path relative to the application working folder.
        // The path is recursively searched for model files.
        // Files belonging to a specific model must be available as normal individual files in a model folder,
        // not inside an archive, and they must be readable by the application process.
        private static readonly string EmbeddedSpeechRecognitionModelPath = @"YourEmbeddedSpeechRecognitionModelPath"; // or set EMBEDDED_SPEECH_RECOGNITION_MODEL_PATH

        // Name of the embedded speech recognition model to be used for recognition.
        // For example: "en-US" or "Microsoft Speech Recognizer en-US FP Model V8"
        private static readonly string EmbeddedSpeechRecognitionModelName = "YourEmbeddedSpeechRecognitionModelName"; // or set EMBEDDED_SPEECH_RECOGNITION_MODEL_NAME

        // Path to the local embedded speech synthesis voice(s) on the device file system.
        // This may be a single voice folder or a top-level folder for several voices.
        // Use an absolute path or a path relative to the application working folder.
        // The path is recursively searched for voice files.
        // Files belonging to a specific voice must be available as normal individual files in a voice folder,
        // not inside an archive, and they must be readable by the application process.
        private static readonly string EmbeddedSpeechSynthesisVoicePath = @"YourEmbeddedSpeechSynthesisVoicePath"; // or set EMBEDDED_SPEECH_SYNTHESIS_VOICE_PATH

        // Name of the embedded speech synthesis voice to be used for synthesis.
        // For example: "en-US-JennyNeural" or "Microsoft Server Speech Text to Speech Voice (en-US, JennyNeural)"
        private static readonly string EmbeddedSpeechSynthesisVoiceName = "YourEmbeddedSpeechSynthesisVoiceName"; // or set EMBEDDED_SPEECH_SYNTHESIS_VOICE_NAME

        // Path to the local embedded speech translation model(s) on the device file system.
        // This may be a single model folder or a top-level folder for several models.
        // Use an absolute path or a path relative to the application working folder.
        // The path is recursively searched for model files.
        // Files belonging to a specific model must be available as normal individual files in a model folder,
        // not inside an archive, and they must be readable by the application process.
        private static readonly string EmbeddedSpeechTranslationModelPath = "YourEmbeddedSpeechTranslationModelPath"; // or set EMBEDDED_SPEECH_TRANSLATION_MODEL_PATH

        // Name of the embedded speech translation model to be used for translation.
        // For example: "Microsoft Speech Translator Many-to-English Model V2"
        private static readonly string EmbeddedSpeechTranslationModelName = "YourEmbeddedSpeechTranslationModelName"; // or set EMBEDDED_SPEECH_TRANSLATION_MODEL_NAME

        // Cloud speech service subscription and language settings.
        // These are needed with hybrid (cloud & embedded) speech configuration.
        // The language must be specified in BCP-47 format, case-sensitive.
        private static readonly string CloudSpeechSubscriptionKey = "YourCloudSpeechSubscriptionKey"; // or set CLOUD_SPEECH_SUBSCRIPTION_KEY
        private static readonly string CloudSpeechServiceEndpoint = "YourCloudSpeechServiceEndpoint"; // or set CLOUD_SPEECH_SERVICE_ENDPOINT
        private static readonly string CloudSpeechRecognitionLanguage = "en-US"; // or set CLOUD_SPEECH_RECOGNITION_LANGUAGE
        private static readonly string CloudSpeechSynthesisLanguage = "en-US"; // or set CLOUD_SPEECH_SYNTHESIS_LANGUAGE

        // END OF CONFIGURABLE SETTINGS


        // Embedded speech recognition default input audio format settings.
        // In addition, little-endian signed integer samples are required.
        public static uint GetEmbeddedSpeechSamplesPerSecond() { return 16000; }  // or 8000
        public static byte GetEmbeddedSpeechBitsPerSample() { return 16; }        // DO NOT MODIFY; no other format supported
        public static byte GetEmbeddedSpeechChannels() { return 1; }              // DO NOT MODIFY; no other format supported

        // Get names and other properties of example files included with the sample project.
        public static string GetSpeechRawAudioFileName() { return "data/speech_test.raw"; }
        public static string GetSpeechWavAudioFileName() { return "data/speech_test.wav"; }
        public static string GetPerfTestAudioFileName() { return "data/performance_test.wav"; }

        // For more information about keyword recognition and models, see
        // https://docs.microsoft.com/azure/cognitive-services/speech-service/keyword-recognition-overview
        public static string GetKeywordModelFileName() { return "data/keyword_computer.table"; }
        public static string GetKeywordPhrase() { return "Computer"; }

        // Get a setting value from environment or defaults.
        private static string GetSetting(string environmentVariableName, string defaultValue)
        {
            var value = System.Environment.GetEnvironmentVariable(environmentVariableName);
            return !string.IsNullOrEmpty(value) ? value : defaultValue;
        }


        // These are set in VerifySettingsAsync() after some basic verification.
        private static string SpeechModelLicense;
        private static string SpeechRecognitionModelPath;
        private static string SpeechRecognitionModelName;
        private static string SpeechSynthesisVoicePath;
        private static string SpeechSynthesisVoiceName;
        private static string SpeechTranslationModelPath;
        private static string SpeechTranslationModelName;

        // Utility functions for main menu.
        public static bool HasSpeechRecognitionModel()
        {
            if (string.IsNullOrEmpty(SpeechRecognitionModelPath) || string.IsNullOrEmpty(SpeechRecognitionModelName))
            {
                Console.Error.WriteLine("## ERROR: No speech recognition model specified.");
                return false;
            }
            return true;
        }

        public static bool HasSpeechSynthesisVoice()
        {
            if (string.IsNullOrEmpty(SpeechSynthesisVoicePath) || string.IsNullOrEmpty(SpeechSynthesisVoiceName))
            {
                Console.Error.WriteLine("## ERROR: No speech synthesis voice specified.");
                return false;
            }
            return true;
        }

        public static bool HasSpeechTranslationModel()
        {
            if (string.IsNullOrEmpty(SpeechTranslationModelPath) || string.IsNullOrEmpty(SpeechTranslationModelName))
            {
                Console.Error.WriteLine("## ERROR: No speech translation model specified.");
                return false;
            }
            return true;
        }

        // Creates an instance of an embedded speech config.
        public static EmbeddedSpeechConfig CreateEmbeddedSpeechConfig()
        {
            List<string> paths = new List<string>();

            // Add paths for offline data.
            if (!string.IsNullOrEmpty(SpeechRecognitionModelPath))
            {
                paths.Add(SpeechRecognitionModelPath);
            }
            if (!string.IsNullOrEmpty(SpeechSynthesisVoicePath))
            {
                paths.Add(SpeechSynthesisVoicePath);
            }
            if (!string.IsNullOrEmpty(SpeechTranslationModelPath))
            {
                paths.Add(SpeechTranslationModelPath);
            }

            if (paths.Count == 0)
            {
                Console.Error.WriteLine("## ERROR: No model path(s) specified.");
                return null;
            }

            // Note, if there is only one path then you can also use EmbeddedSpeechConfig.FromPath(string).
            // All paths must be valid directory paths on the file system, otherwise e.g. initialization of
            // embedded speech synthesis will fail.
            var config = EmbeddedSpeechConfig.FromPaths(paths.ToArray());

            // Enable Speech SDK logging. If you want to report an issue, include this log with the report.
            // If no path is specified, the log file will be created in the program default working folder.
            // If a path is specified, make sure that it is writable by the application process.
            /*
            config.SetProperty(PropertyId.Speech_LogFilename, @"SpeechSDK.log");
            */

            if (!string.IsNullOrEmpty(SpeechRecognitionModelName))
            {
                // Mandatory configuration for embedded speech (and intent) recognition.
                config.SetSpeechRecognitionModel(SpeechRecognitionModelName, SpeechModelLicense);
            }

            if (!string.IsNullOrEmpty(SpeechSynthesisVoiceName))
            {
                // Mandatory configuration for embedded speech synthesis.
                config.SetSpeechSynthesisVoice(SpeechSynthesisVoiceName, SpeechModelLicense);
                if (SpeechSynthesisVoiceName.Contains("Neural"))
                {
                    // Embedded neural voices only support 24kHz sample rate.
                    config.SetSpeechSynthesisOutputFormat(SpeechSynthesisOutputFormat.Riff24Khz16BitMonoPcm);
                }
            }

            if (!string.IsNullOrEmpty(SpeechTranslationModelName))
            {
                // Mandatory configuration for embedded speech translation.
                config.SetSpeechTranslationModel(SpeechTranslationModelName, SpeechModelLicense);
            }

            // Disable profanity masking.
            /*
            config.SetProfanity(ProfanityOption.Raw);
            */

            return config;
        }


        // Creates an instance of a hybrid (cloud & embedded) speech config.
        public static HybridSpeechConfig CreateHybridSpeechConfig()
        {
            var cloudSpeechConfig = SpeechConfig.FromEndpoint(
                new Uri(GetSetting("CLOUD_SPEECH_SERVICE_ENDPOINT", CloudSpeechServiceEndpoint)),
                GetSetting("CLOUD_SPEECH_SUBSCRIPTION_KEY", CloudSpeechSubscriptionKey)
                );
            // Optional language configuration for cloud speech services.
            // The internal default is en-US.
            // Also see
            // https://github.com/Azure-Samples/cognitive-services-speech-sdk/tree/master/samples/csharp/dotnetcore/console
            // for complete Speech SDK samples using cloud speech services.
            cloudSpeechConfig.SpeechRecognitionLanguage = GetSetting("CLOUD_SPEECH_RECOGNITION_LANGUAGE", CloudSpeechRecognitionLanguage);
            cloudSpeechConfig.SpeechSynthesisLanguage = GetSetting("CLOUD_SPEECH_SYNTHESIS_LANGUAGE", CloudSpeechSynthesisLanguage);

            var embeddedSpeechConfig = CreateEmbeddedSpeechConfig();

            var config = HybridSpeechConfig.FromConfigs(cloudSpeechConfig, embeddedSpeechConfig);

            return config;
        }


        // Do some basic verification of embedded speech settings.
        public static async Task<bool> VerifySettingsAsync()
        {
            string cwd = System.IO.Directory.GetCurrentDirectory();
            Console.WriteLine($"Current working directory: {cwd}");

            SpeechModelLicense = GetSetting("EMBEDDED_SPEECH_MODEL_LICENSE", EmbeddedSpeechModelLicense);
            if (string.IsNullOrEmpty(SpeechModelLicense) || SpeechModelLicense.Equals("YourEmbeddedSpeechModelLicense"))
            {
                Console.Error.WriteLine("## ERROR: The embedded speech model license is not set.");
                return false;
            }

            SpeechRecognitionModelPath = GetSetting("EMBEDDED_SPEECH_RECOGNITION_MODEL_PATH", EmbeddedSpeechRecognitionModelPath);
            if (SpeechRecognitionModelPath.Equals("YourEmbeddedSpeechRecognitionModelPath"))
            {
                SpeechRecognitionModelPath = null;
            }
            SpeechRecognitionModelName = GetSetting("EMBEDDED_SPEECH_RECOGNITION_MODEL_NAME", EmbeddedSpeechRecognitionModelName);
            if (SpeechRecognitionModelName.Equals("YourEmbeddedSpeechRecognitionModelName"))
            {
                SpeechRecognitionModelName = null;
            }

            SpeechSynthesisVoicePath = GetSetting("EMBEDDED_SPEECH_SYNTHESIS_VOICE_PATH", EmbeddedSpeechSynthesisVoicePath);
            if (SpeechSynthesisVoicePath.Equals("YourEmbeddedSpeechSynthesisVoicePath"))
            {
                SpeechSynthesisVoicePath = null;
            }
            SpeechSynthesisVoiceName = GetSetting("EMBEDDED_SPEECH_SYNTHESIS_VOICE_NAME", EmbeddedSpeechSynthesisVoiceName);
            if (SpeechSynthesisVoiceName.Equals("YourEmbeddedSpeechSynthesisVoiceName"))
            {
                SpeechSynthesisVoiceName = null;
            }

            SpeechTranslationModelPath = GetSetting("EMBEDDED_SPEECH_TRANSLATION_MODEL_PATH", EmbeddedSpeechTranslationModelPath);
            if (SpeechTranslationModelPath.Equals("YourEmbeddedSpeechTranslationModelPath"))
            {
                SpeechTranslationModelPath = null;
            }
            SpeechTranslationModelName = GetSetting("EMBEDDED_SPEECH_TRANSLATION_MODEL_NAME", EmbeddedSpeechTranslationModelName);
            if (SpeechTranslationModelName.Equals("YourEmbeddedSpeechTranslationModelName"))
            {
                SpeechTranslationModelName = null;
            }

            // Find an embedded speech recognition model based on the name.
            if (!string.IsNullOrEmpty(SpeechRecognitionModelPath) && !string.IsNullOrEmpty(SpeechRecognitionModelName))
            {
                var config = EmbeddedSpeechConfig.FromPath(SpeechRecognitionModelPath);
                var models = config.GetSpeechRecognitionModels();

                var result = models.FirstOrDefault(model =>
                    model.Name.Equals(SpeechRecognitionModelName) || model.Locales[0].Equals(SpeechRecognitionModelName));

                if (result == null)
                {
                    Console.WriteLine("## WARNING: Cannot locate an embedded speech recognition model \"{SpeechRecognitionModelName}\"");
                }
            }

            // Find an embedded speech synthesis voice based on the name.
            if (!string.IsNullOrEmpty(SpeechSynthesisVoicePath) && !string.IsNullOrEmpty(SpeechSynthesisVoiceName))
            {
                var config = EmbeddedSpeechConfig.FromPath(SpeechSynthesisVoicePath);
                using var synthesizer = new SpeechSynthesizer(config, null);

                bool found = false;
                using var voicesList = await synthesizer.GetVoicesAsync("");

                if (voicesList.Reason == ResultReason.VoicesListRetrieved)
                {
                    var result = voicesList.Voices.FirstOrDefault(voice =>
                        voice.Name.Equals(SpeechSynthesisVoiceName) || voice.ShortName.Equals(SpeechSynthesisVoiceName));

                    if (result != null)
                    {
                        found = true;
                    }
                }

                if (!found)
                {
                    Console.WriteLine("## WARNING: Cannot locate an embedded speech synthesis voice \"{SpeechSynthesisVoiceName}\"");
                }
            }

            // Find an embedded speech translation model based on the name.
            if (!string.IsNullOrEmpty(SpeechTranslationModelPath) && !string.IsNullOrEmpty(SpeechTranslationModelName))
            {
                var config = EmbeddedSpeechConfig.FromPath(SpeechTranslationModelPath);
                var models = config.GetSpeechTranslationModels();

                var result = models.FirstOrDefault(model =>
                    model.Name.Equals(SpeechTranslationModelName));

                if (result == null)
                {
                    Console.WriteLine("## WARNING: Cannot locate an embedded speech translation model \"{SpeechTranslationModelName}\". ");
                }
            }

            Console.WriteLine($"Embedded speech recognition");
            Console.WriteLine($"  model search path: {(string.IsNullOrEmpty(SpeechRecognitionModelPath) ? "(not set)" : SpeechRecognitionModelPath)}");
            Console.WriteLine($"  model name:        {(string.IsNullOrEmpty(SpeechRecognitionModelName) ? "(not set)" : SpeechRecognitionModelName)}");
            Console.WriteLine($"Embedded speech synthesis");
            Console.WriteLine($"  voice search path: {(string.IsNullOrEmpty(SpeechSynthesisVoicePath) ? "(not set)" : SpeechSynthesisVoicePath)}");
            Console.WriteLine($"  voice name:        {(string.IsNullOrEmpty(SpeechSynthesisVoiceName) ? "(not set)" : SpeechSynthesisVoiceName)}");
            Console.WriteLine($"Embedded speech translation");
            Console.WriteLine($"  model search path: {(string.IsNullOrEmpty(SpeechTranslationModelPath) ? "(not set)" : SpeechTranslationModelPath)}");
            Console.WriteLine($"  model name:        {(string.IsNullOrEmpty(SpeechTranslationModelName) ? "(not set)" : SpeechTranslationModelName)}");

            return true;
        }
    }
}
