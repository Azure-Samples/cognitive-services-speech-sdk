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

        // Locale to be used in speech recognition, cloud and embedded. In BCP-47 format, case-sensitive.
        // If EmbeddedSpeechRecognitionModelName is changed from the default, it will override this for embedded.
        private static readonly string SpeechRecognitionLocale = "en-US"; // or set SPEECH_RECOGNITION_LOCALE

        // Locale to be used in speech synthesis (text-to-speech), cloud and embedded. In BCP-47 format, case-sensitive.
        // If EmbeddedSpeechSynthesisVoiceName is changed from the default, it will override this for embedded.
        private static readonly string SpeechSynthesisLocale = "en-US"; // or set SPEECH_SYNTHESIS_LOCALE

        // Path to the local embedded speech recognition model(s) on the device file system.
        // This may be a single model folder or a top-level folder for several models.
        // Use an absolute path or a path relative to the application working folder.
        // The path is recursively searched for model files.
        // Files belonging to a specific model must be available as normal individual files in a model folder,
        // not inside an archive, and they must be readable by the application process.
        private static readonly string EmbeddedSpeechRecognitionModelPath = @"YourEmbeddedSpeechRecognitionModelPath"; // or set EMBEDDED_SPEECH_RECOGNITION_MODEL_PATH

        // Name of the embedded speech recognition model to be used for recognition.
        // If changed from the default, this will override SpeechRecognitionLocale.
        // For example: "en-US" or "Microsoft Speech Recognizer en-US FP Model V8"
        private static readonly string EmbeddedSpeechRecognitionModelName = "YourEmbeddedSpeechRecognitionModelName"; // or set EMBEDDED_SPEECH_RECOGNITION_MODEL_NAME

        // Decryption key of the (encrypted) embedded speech recognition model.
        // WARNING: The key may be visible in the program binary if hard-coded as a plain string.
        private static readonly string EmbeddedSpeechRecognitionModelKey = "YourEmbeddedSpeechRecognitionModelKey"; // or set EMBEDDED_SPEECH_RECOGNITION_MODEL_KEY

        // Path to the local embedded speech synthesis voice(s) on the device file system.
        // This may be a single voice folder or a top-level folder for several voices.
        // Use an absolute path or a path relative to the application working folder.
        // The path is recursively searched for voice files.
        // Files belonging to a specific voice must be available as normal individual files in a voice folder,
        // not inside an archive, and they must be readable by the application process.
        private static readonly string EmbeddedSpeechSynthesisVoicePath = @"YourEmbeddedSpeechSynthesisVoicePath"; // or set EMBEDDED_SPEECH_SYNTHESIS_VOICE_PATH

        // Name of the embedded speech synthesis voice to be used for synthesis.
        // If changed from the default, this will override SpeechSynthesisLocale.
        // For example: "en-US-JennyNeural" or "Microsoft Server Speech Text to Speech Voice (en-US, JennyNeural)"
        private static readonly string EmbeddedSpeechSynthesisVoiceName = "YourEmbeddedSpeechSynthesisVoiceName"; // or set EMBEDDED_SPEECH_SYNTHESIS_VOICE_NAME

        // Decryption key of the (encrypted) embedded speech synthesis voice.
        // WARNING: The key may be visible in the program binary if hard-coded as a plain string.
        private static readonly string EmbeddedSpeechSynthesisVoiceKey = "YourEmbeddedSpeechSynthesisVoiceKey"; // or set EMBEDDED_SPEECH_SYNTHESIS_VOICE_KEY

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

        // Decryption key of the (encrypted) embedded speech translation model.
        // WARNING: The key may be visible in the program binary if hard-coded as a plain string.
        private static readonly string EmbeddedSpeechTranslationModelKey = "YourEmbeddedSpeechTranslationModelKey"; // or set EMBEDDED_SPEECH_TRANSLATION_MODEL_KEY

        // Cloud speech service subscription information.
        // This is needed with hybrid (cloud & embedded) speech configuration.
        private static readonly string CloudSpeechSubscriptionKey = "YourCloudSpeechSubscriptionKey"; // or set CLOUD_SPEECH_SUBSCRIPTION_KEY
        private static readonly string CloudSpeechServiceRegion = "YourCloudSpeechServiceRegion"; // or set CLOUD_SPEECH_SERVICE_REGION

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
        private static string SpeechRecognitionModelName;
        private static string SpeechRecognitionModelKey;
        private static string SpeechSynthesisVoiceName;
        private static string SpeechSynthesisVoiceKey;
        private static string SpeechTranslationModelName;
        private static string SpeechTranslationModelKey;

        // Utility functions for main menu.
        public static bool HasSpeechRecognitionModel()
        {
            if (string.IsNullOrEmpty(SpeechRecognitionModelName))
            {
                Console.Error.WriteLine("## ERROR: No speech recognition model specified.");
                return false;
            }
            return true;
        }

        public static bool HasSpeechSynthesisVoice()
        {
            if (string.IsNullOrEmpty(SpeechSynthesisVoiceName))
            {
                Console.Error.WriteLine("## ERROR: No speech synthesis voice specified.");
                return false;
            }
            return true;
        }

        public static bool HasSpeechTranslationModel()
        {
            if (string.IsNullOrEmpty(SpeechTranslationModelName))
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
            var recognitionModelPath = GetSetting("EMBEDDED_SPEECH_RECOGNITION_MODEL_PATH", EmbeddedSpeechRecognitionModelPath);
            if (!string.IsNullOrEmpty(recognitionModelPath) && !recognitionModelPath.Equals("YourEmbeddedSpeechRecognitionModelPath"))
            {
                paths.Add(recognitionModelPath);
            }
            var synthesisVoicePath = GetSetting("EMBEDDED_SPEECH_SYNTHESIS_VOICE_PATH", EmbeddedSpeechSynthesisVoicePath);
            if (!string.IsNullOrEmpty(synthesisVoicePath) && !synthesisVoicePath.Equals("YourEmbeddedSpeechSynthesisVoicePath"))
            {
                paths.Add(synthesisVoicePath);
            }
            var translationModelPath = GetSetting("EMBEDDED_SPEECH_TRANSLATION_MODEL_PATH", EmbeddedSpeechTranslationModelPath);
            if (!string.IsNullOrEmpty(translationModelPath) && !translationModelPath.Equals("YourEmbeddedSpeechTranslationModelPath"))
            {
                paths.Add(translationModelPath);
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
                // Mandatory configuration for embedded speech recognition.
                config.SetSpeechRecognitionModel(SpeechRecognitionModelName, SpeechRecognitionModelKey);
            }

            if (!string.IsNullOrEmpty(SpeechSynthesisVoiceName))
            {
                // Mandatory configuration for embedded speech synthesis.
                config.SetSpeechSynthesisVoice(SpeechSynthesisVoiceName, SpeechSynthesisVoiceKey);
                if (SpeechSynthesisVoiceName.Contains("Neural"))
                {
                    // Embedded neural voices only support 24kHz sample rate.
                    config.SetSpeechSynthesisOutputFormat(SpeechSynthesisOutputFormat.Riff24Khz16BitMonoPcm);
                }
            }

            if (!string.IsNullOrEmpty(SpeechTranslationModelName))
            {
                // Mandatory configuration for embedded speech translation.
                config.SetSpeechTranslationModel(SpeechTranslationModelName, SpeechTranslationModelKey);
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
            var cloudSpeechConfig = SpeechConfig.FromSubscription(
                GetSetting("CLOUD_SPEECH_SUBSCRIPTION_KEY", CloudSpeechSubscriptionKey),
                GetSetting("CLOUD_SPEECH_SERVICE_REGION", CloudSpeechServiceRegion)
                );
            // Optional language configuration for cloud speech services.
            // The internal default is en-US.
            // Also see
            // https://github.com/Azure-Samples/cognitive-services-speech-sdk/tree/master/samples/csharp/dotnetcore/console
            // for complete Speech SDK samples using cloud speech services.
            cloudSpeechConfig.SpeechRecognitionLanguage = GetSetting("SPEECH_RECOGNITION_LOCALE", SpeechRecognitionLocale);
            cloudSpeechConfig.SpeechSynthesisLanguage = GetSetting("SPEECH_SYNTHESIS_LOCALE", SpeechSynthesisLocale);

            var embeddedSpeechConfig = CreateEmbeddedSpeechConfig();

            var config = HybridSpeechConfig.FromConfigs(cloudSpeechConfig, embeddedSpeechConfig);

            return config;
        }


        // Do some basic verification of embedded speech settings.
        public static async Task<bool> VerifySettingsAsync()
        {
            string cwd = System.IO.Directory.GetCurrentDirectory();
            Console.WriteLine($"Current working directory: {cwd}");

            var recognitionModelPath = GetSetting("EMBEDDED_SPEECH_RECOGNITION_MODEL_PATH", EmbeddedSpeechRecognitionModelPath);
            if (string.IsNullOrEmpty(recognitionModelPath) || recognitionModelPath.Equals("YourEmbeddedSpeechRecognitionModelPath"))
            {
                recognitionModelPath = null;
            }

            var synthesisVoicePath = GetSetting("EMBEDDED_SPEECH_SYNTHESIS_VOICE_PATH", EmbeddedSpeechSynthesisVoicePath);
            if (string.IsNullOrEmpty(synthesisVoicePath) || synthesisVoicePath.Equals("YourEmbeddedSpeechSynthesisVoicePath"))
            {
                synthesisVoicePath = null;
            }

            var translationModelPath = GetSetting("EMBEDDED_SPEECH_TRANSLATION_MODEL_PATH", EmbeddedSpeechTranslationModelPath);
            if (string.IsNullOrEmpty(translationModelPath) || translationModelPath.Equals("YourEmbeddedSpeechTranslationModelPath"))
            {
                translationModelPath = null;
            }

            // Find an embedded speech recognition model based on the name or locale.

            SpeechRecognitionModelName = null;

            if (recognitionModelPath != null)
            {
                var modelName = GetSetting("EMBEDDED_SPEECH_RECOGNITION_MODEL_NAME", EmbeddedSpeechRecognitionModelName);
                var modelLocale = GetSetting("SPEECH_RECOGNITION_LOCALE", SpeechRecognitionLocale);

                if (string.IsNullOrEmpty(modelName) || modelName.Equals("YourEmbeddedSpeechRecognitionModelName"))
                {
                    modelName = null; // no name given -> search by locale
                }

                var config = EmbeddedSpeechConfig.FromPath(recognitionModelPath);
                var models = config.GetSpeechRecognitionModels();

                var result = models.FirstOrDefault(model =>
                    (modelName == null && model.Locales[0].Equals(modelLocale)) ||
                    (modelName != null && (model.Name.Equals(modelName) || model.Locales[0].Equals(modelName))));

                if (result != null)
                {
                    SpeechRecognitionModelName = result.Name;
                }

                if (string.IsNullOrEmpty(SpeechRecognitionModelName))
                {
                    Console.Write("## WARNING: Cannot locate an embedded speech recognition model by ");
                    if (modelName == null)
                    {
                        Console.Write($"locale \"{modelLocale}\". ");
                    }
                    else
                    {
                        Console.Write($"name \"{modelName}\". ");
                    }
                    Console.WriteLine($"Current recognition model search path: {recognitionModelPath}");
                }
                else
                {
                    SpeechRecognitionModelKey = GetSetting("EMBEDDED_SPEECH_RECOGNITION_MODEL_KEY", EmbeddedSpeechRecognitionModelKey);
                    if (string.IsNullOrEmpty(SpeechRecognitionModelKey) || SpeechRecognitionModelKey.Equals("YourEmbeddedSpeechRecognitionModelKey"))
                    {
                        SpeechRecognitionModelKey = null;
                        Console.WriteLine($"## WARNING: The key for \"{SpeechRecognitionModelName}\" is not set.");
                    }
                }
            }

            // Find an embedded speech synthesis voice based on the name or locale.

            SpeechSynthesisVoiceName = null;

            if (synthesisVoicePath != null)
            {
                var voiceName = GetSetting("EMBEDDED_SPEECH_SYNTHESIS_VOICE_NAME", EmbeddedSpeechSynthesisVoiceName);
                var voiceLocale = GetSetting("SPEECH_SYNTHESIS_LOCALE", SpeechSynthesisLocale);

                if (string.IsNullOrEmpty(voiceName) || voiceName.Equals("YourEmbeddedSpeechSynthesisVoiceName"))
                {
                    voiceName = null; // no name given -> search by locale
                }

                var config = EmbeddedSpeechConfig.FromPath(synthesisVoicePath);

                using var synthesizer = new SpeechSynthesizer(config, null);
                using var voicesList = await synthesizer.GetVoicesAsync("");

                if (voicesList.Reason == ResultReason.VoicesListRetrieved)
                {
                    var result = voicesList.Voices.FirstOrDefault(voice =>
                        (voiceName == null && voice.Locale.Equals(voiceLocale)) ||
                        (voiceName != null && (voice.Name.Equals(voiceName) || voice.ShortName.Equals(voiceName))));

                    if (result != null)
                    {
                        SpeechSynthesisVoiceName = result.Name;
                    }
                }

                if (string.IsNullOrEmpty(SpeechSynthesisVoiceName))
                {
                    Console.Write("## WARNING: Cannot locate an embedded speech synthesis voice by ");
                    if (voiceName == null)
                    {
                        Console.Write($"locale \"{voiceLocale}\". ");
                    }
                    else
                    {
                        Console.Write($"name \"{voiceName}\". ");
                    }
                    Console.WriteLine($"Current synthesis voice search path: {synthesisVoicePath}");
                }
                else
                {
                    SpeechSynthesisVoiceKey = GetSetting("EMBEDDED_SPEECH_SYNTHESIS_VOICE_KEY", EmbeddedSpeechSynthesisVoiceKey);
                    if (string.IsNullOrEmpty(SpeechSynthesisVoiceKey) || SpeechSynthesisVoiceKey.Equals("YourEmbeddedSpeechSynthesisVoiceKey"))
                    {
                        SpeechSynthesisVoiceKey = null;
                        Console.WriteLine($"## WARNING: The key for \"{SpeechSynthesisVoiceName}\" is not set.");
                    }
                }
            }

            // Find an embedded speech translation model based on the name.

            SpeechTranslationModelName = null;

            if (translationModelPath != null)
            {
                var modelName = GetSetting("EMBEDDED_SPEECH_TRANSLATION_MODEL_NAME", EmbeddedSpeechTranslationModelName);

                var config = EmbeddedSpeechConfig.FromPath(translationModelPath);
                var models = config.GetSpeechTranslationModels();

                var result = models.FirstOrDefault(model =>
                    (modelName != null && model.Name.Equals(modelName)));

                if (result != null)
                {
                    SpeechTranslationModelName = result.Name;
                }

                if (string.IsNullOrEmpty(SpeechTranslationModelName))
                {
                    Console.Write("## WARNING: Cannot locate an embedded speech translation model by ");
                    Console.Write($"name \"{modelName}\". ");
                    Console.WriteLine($"Current translation model search path: {translationModelPath}");
                }
                else
                {
                    SpeechTranslationModelKey = GetSetting("EMBEDDED_SPEECH_TRANSLATION_MODEL_KEY", EmbeddedSpeechTranslationModelKey);
                    if (string.IsNullOrEmpty(SpeechTranslationModelKey) || SpeechTranslationModelKey.Equals("YourEmbeddedSpeechTranslationModelKey"))
                    {
                        SpeechTranslationModelKey = null;
                        Console.WriteLine($"## WARNING: The key for \"{SpeechTranslationModelName}\" is not set.");
                    }
                }
            }

            Func<string, string> maskValue = (string value) =>
            {
                // Mask the string value, leave only the last 3 chars visible
                int visibleLength = value.Length > 3 ? 3 : 0;
                string masked = new string('*', value.Length - visibleLength) + value.Substring(value.Length - visibleLength);
                return masked;
            };

            Console.WriteLine($"Embedded speech recognition\n  model search path: {(recognitionModelPath == null ? "(not set)" : recognitionModelPath)}");
            if (recognitionModelPath != null)
            {
                Console.WriteLine($"  model name:        {(string.IsNullOrEmpty(SpeechRecognitionModelName) ? "(not found)" : SpeechRecognitionModelName)}");
                if (!string.IsNullOrEmpty(SpeechRecognitionModelName))
                {
                    Console.WriteLine($"  model key:         {(string.IsNullOrEmpty(SpeechRecognitionModelKey) ? "(not set)" : maskValue(SpeechRecognitionModelKey))}");
                }
            }
            Console.WriteLine($"Embedded speech synthesis\n  voice search path: {(synthesisVoicePath == null ? "(not set)" : synthesisVoicePath)}");
            if (synthesisVoicePath != null)
            {
                Console.WriteLine($"  voice name:        {(string.IsNullOrEmpty(SpeechSynthesisVoiceName) ? "(not found)" : SpeechSynthesisVoiceName)}");
                if (!string.IsNullOrEmpty(SpeechSynthesisVoiceName))
                {
                    Console.WriteLine($"  voice key:         {(string.IsNullOrEmpty(SpeechSynthesisVoiceKey) ? "(not set)" : maskValue(SpeechSynthesisVoiceKey))}");
                }
            }
            Console.WriteLine($"Embedded speech translation\n  model search path: {(translationModelPath == null ? "(not set)" : translationModelPath)}");
            if (translationModelPath != null)
            {
                Console.WriteLine($"  model name:        {(string.IsNullOrEmpty(SpeechTranslationModelName) ? "(not found)" : SpeechTranslationModelName)}");
                if (!string.IsNullOrEmpty(SpeechTranslationModelName))
                {
                    Console.WriteLine($"  model key:         {(string.IsNullOrEmpty(SpeechTranslationModelKey) ? "(not set)" : maskValue(SpeechTranslationModelKey))}");
                }
            }

            return true;
        }
    }
}
