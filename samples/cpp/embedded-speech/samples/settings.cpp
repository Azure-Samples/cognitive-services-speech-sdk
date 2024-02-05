//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//

#include <iostream>
#include <speechapi_cxx.h>

using namespace std;
using namespace Microsoft::CognitiveServices::Speech;

#include <stdio.h>  /* for FILENAME_MAX */
#ifdef _MSC_VER
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif


// START OF CONFIGURABLE SETTINGS

// Locale to be used in speech recognition, cloud and embedded. In BCP-47 format, case-sensitive.
// If EmbeddedSpeechRecognitionModelName is changed from the default, it will override this for embedded.
const string SpeechRecognitionLocale = "en-US"; // or set SPEECH_RECOGNITION_LOCALE

// Locale to be used in speech synthesis (text-to-speech), cloud and embedded. In BCP-47 format, case-sensitive.
// If EmbeddedSpeechSynthesisVoiceName is changed from the default, it will override this for embedded.
const string SpeechSynthesisLocale = "en-US"; // or set SPEECH_SYNTHESIS_LOCALE

// Path to the local embedded speech recognition model(s) on the device file system.
// This may be a single model folder or a top-level folder for several models.
// Use an absolute path or a path relative to the application working folder.
// The path is recursively searched for model files.
// Files belonging to a specific model must be available as normal individual files in a model folder,
// not inside an archive, and they must be readable by the application process.
const string EmbeddedSpeechRecognitionModelPath = "YourEmbeddedSpeechRecognitionModelPath"; // or set EMBEDDED_SPEECH_RECOGNITION_MODEL_PATH

// Name of the embedded speech recognition model to be used for recognition.
// If changed from the default, this will override SpeechRecognitionLocale.
// For example: "en-US" or "Microsoft Speech Recognizer en-US FP Model V8"
const string EmbeddedSpeechRecognitionModelName = "YourEmbeddedSpeechRecognitionModelName"; // or set EMBEDDED_SPEECH_RECOGNITION_MODEL_NAME

// Decryption key of the (encrypted) embedded speech recognition model.
// WARNING: The key may be visible in the program binary if hard-coded as a plain string.
const string EmbeddedSpeechRecognitionModelKey = "YourEmbeddedSpeechRecognitionModelKey"; // or set EMBEDDED_SPEECH_RECOGNITION_MODEL_KEY

// Path to the local embedded speech synthesis voice(s) on the device file system.
// This may be a single voice folder or a top-level folder for several voices.
// Use an absolute path or a path relative to the application working folder.
// The path is recursively searched for voice files.
// Files belonging to a specific voice must be available as normal individual files in a voice folder,
// not inside an archive, and they must be readable by the application process.
const string EmbeddedSpeechSynthesisVoicePath = "YourEmbeddedSpeechSynthesisVoicePath"; // or set EMBEDDED_SPEECH_SYNTHESIS_VOICE_PATH

// Name of the embedded speech synthesis voice to be used for synthesis.
// If changed from the default, this will override SpeechSynthesisLocale.
// For example: "en-US-JennyNeural" or "Microsoft Server Speech Text to Speech Voice (en-US, JennyNeural)"
const string EmbeddedSpeechSynthesisVoiceName = "YourEmbeddedSpeechSynthesisVoiceName"; // or set EMBEDDED_SPEECH_SYNTHESIS_VOICE_NAME

// Decryption key of the (encrypted) embedded speech synthesis voice.
// WARNING: The key may be visible in the program binary if hard-coded as a plain string.
const string EmbeddedSpeechSynthesisVoiceKey = "YourEmbeddedSpeechSynthesisVoiceKey"; // or set EMBEDDED_SPEECH_SYNTHESIS_VOICE_KEY

// Path to the local embedded speech translation model(s) on the device file system.
// This may be a single model folder or a top-level folder for several models.
// Use an absolute path or a path relative to the application working folder.
// The path is recursively searched for model files.
// Files belonging to a specific model must be available as normal individual files in a model folder,
// not inside an archive, and they must be readable by the application process.
const string EmbeddedSpeechTranslationModelPath = "YourEmbeddedSpeechTranslationModelPath"; // or set EMBEDDED_SPEECH_TRANSLATION_MODEL_PATH

// Name of the embedded speech translation model to be used for translation.
// For example: "Microsoft Speech Translator Many-to-English Model V2"
const string EmbeddedSpeechTranslationModelName = "YourEmbeddedSpeechTranslationModelName"; // or set EMBEDDED_SPEECH_TRANSLATION_MODEL_NAME

// Decryption key of the (encrypted) embedded speech translation model.
// WARNING: The key may be visible in the program binary if hard-coded as a plain string.
const string EmbeddedSpeechTranslationModelKey = "YourEmbeddedSpeechTranslationModelKey"; // or set EMBEDDED_SPEECH_TRANSLATION_MODEL_KEY

// Cloud speech service subscription information.
// This is needed with hybrid (cloud & embedded) speech configuration.
const string CloudSpeechSubscriptionKey = "YourCloudSpeechSubscriptionKey"; // or set CLOUD_SPEECH_SUBSCRIPTION_KEY
const string CloudSpeechServiceRegion = "YourCloudSpeechServiceRegion"; // or set CLOUD_SPEECH_SERVICE_REGION

// END OF CONFIGURABLE SETTINGS


// Embedded speech recognition default input audio format settings.
// In addition, little-endian signed integer samples are required.
uint32_t GetEmbeddedSpeechSamplesPerSecond() { return 16000; }  // or 8000
uint8_t GetEmbeddedSpeechBitsPerSample() { return 16; }         // DO NOT MODIFY; no other format supported
uint8_t GetEmbeddedSpeechChannels() { return 1; }               // DO NOT MODIFY; no other format supported

// Get names and other properties of example files included with the sample project.
const string GetSpeechRawAudioFileName() { return "data/speech_test.raw"; }
const string GetSpeechWavAudioFileName() { return "data/speech_test.wav"; }
const string GetPerfTestAudioFileName() { return "data/performance_test.wav"; }

// For more information about keyword recognition and models, see
// https://docs.microsoft.com/azure/cognitive-services/speech-service/keyword-recognition-overview
const string GetKeywordModelFileName() { return "data/keyword_computer.table"; }
const string GetKeywordPhrase() { return "Computer"; }

// Get a setting value from environment or defaults.
const string GetSetting(const char* environmentVariableName, const string& defaultValue)
{
#pragma warning(suppress : 4996) // getenv
    auto value = getenv(environmentVariableName);
    return value ? value : defaultValue;
}


// These are set in VerifySettings() after some basic verification.
string SpeechRecognitionModelName;
string SpeechRecognitionModelKey;
string SpeechSynthesisVoiceName;
string SpeechSynthesisVoiceKey;
string SpeechTranslationModelName;
string SpeechTranslationModelKey;

// Utility functions for main menu.
bool HasSpeechRecognitionModel()
{
    if (SpeechRecognitionModelName.empty())
    {
        cerr << "## ERROR: No speech recognition model specified.\n";
        return false;
    }
    return true;
}

bool HasSpeechSynthesisVoice()
{
    if (SpeechSynthesisVoiceName.empty())
    {
        cerr << "## ERROR: No speech synthesis voice specified.\n";
        return false;
    }
    return true;
}

bool HasSpeechTranslationModel()
{
    if (SpeechTranslationModelName.empty())
    {
        cerr << "## ERROR: No speech translation model specified.\n";
        return false;
    }
    return true;
}

// Creates an instance of an embedded speech config.
shared_ptr<EmbeddedSpeechConfig> CreateEmbeddedSpeechConfig()
{
    vector<string> paths;

    // Add paths for offline data.
    auto recognitionModelPath = GetSetting("EMBEDDED_SPEECH_RECOGNITION_MODEL_PATH", EmbeddedSpeechRecognitionModelPath);
    if (!recognitionModelPath.empty() && recognitionModelPath.compare("YourEmbeddedSpeechRecognitionModelPath") != 0)
    {
        paths.push_back(recognitionModelPath);
    }
    auto synthesisVoicePath = GetSetting("EMBEDDED_SPEECH_SYNTHESIS_VOICE_PATH", EmbeddedSpeechSynthesisVoicePath);
    if (!synthesisVoicePath.empty() && synthesisVoicePath.compare("YourEmbeddedSpeechSynthesisVoicePath") != 0)
    {
        paths.push_back(synthesisVoicePath);
    }
    auto translationModelPath = GetSetting("EMBEDDED_SPEECH_TRANSLATION_MODEL_PATH", EmbeddedSpeechTranslationModelPath);
    if (!translationModelPath.empty() && translationModelPath.compare("YourEmbeddedSpeechTranslationModelPath") != 0)
    {
        paths.push_back(translationModelPath);
    }

    if (paths.size() == 0)
    {
        cerr << "## ERROR: No model path(s) specified.\n";
        return nullptr;
    }

    // Note, if there is only one path then you can also use EmbeddedSpeechConfig::FromPath(string).
    // All paths must be valid directory paths on the file system, otherwise e.g. initialization of
    // embedded speech synthesis will fail.
    auto config = EmbeddedSpeechConfig::FromPaths(paths);

    // Enable Speech SDK logging. If you want to report an issue, include this log with the report.
    // If no path is specified, the log file will be created in the program default working folder.
    // If a path is specified, make sure that it is writable by the application process.
    /*
    config->SetProperty(PropertyId::Speech_LogFilename, "SpeechSDK.log");
    */

    if (!SpeechRecognitionModelName.empty())
    {
        // Mandatory configuration for embedded speech (and intent) recognition.
        config->SetSpeechRecognitionModel(SpeechRecognitionModelName, SpeechRecognitionModelKey);
    }

    if (!SpeechSynthesisVoiceName.empty())
    {
        // Mandatory configuration for embedded speech synthesis.
        config->SetSpeechSynthesisVoice(SpeechSynthesisVoiceName, SpeechSynthesisVoiceKey);
        if (SpeechSynthesisVoiceName.find("Neural") != string::npos)
        {
            // Embedded neural voices only support 24kHz sample rate.
            config->SetSpeechSynthesisOutputFormat(SpeechSynthesisOutputFormat::Riff24Khz16BitMonoPcm);
        }
    }

    if (!SpeechTranslationModelName.empty())
    {
        // Mandatory configuration for embedded speech translation.
        config->SetSpeechTranslationModel(SpeechTranslationModelName, SpeechTranslationModelKey);
    }

    // Disable profanity masking.
    /*
    config->SetProfanity(ProfanityOption::Raw);
    */

    return config;
}


// Creates an instance of a hybrid (cloud & embedded) speech config.
shared_ptr<HybridSpeechConfig> CreateHybridSpeechConfig()
{
    auto cloudSpeechConfig = SpeechConfig::FromSubscription(
        GetSetting("CLOUD_SPEECH_SUBSCRIPTION_KEY", CloudSpeechSubscriptionKey),
        GetSetting("CLOUD_SPEECH_SERVICE_REGION", CloudSpeechServiceRegion)
    );
    // Optional language configuration for cloud speech services.
    // The internal default is en-US.
    // Also see
    // https://github.com/Azure-Samples/cognitive-services-speech-sdk/tree/master/samples/cpp/windows/console
    // for complete Speech SDK samples using cloud speech services.
    cloudSpeechConfig->SetSpeechRecognitionLanguage(GetSetting("SPEECH_RECOGNITION_LOCALE", SpeechRecognitionLocale));
    cloudSpeechConfig->SetSpeechSynthesisLanguage(GetSetting("SPEECH_SYNTHESIS_LOCALE", SpeechSynthesisLocale));

    auto embeddedSpeechConfig = CreateEmbeddedSpeechConfig();

    auto config = HybridSpeechConfig::FromConfigs(cloudSpeechConfig, embeddedSpeechConfig);

    return config;
}


// Do some basic verification of embedded speech settings.
bool VerifySettings()
{
    vector<char> cwd;
    cwd.resize(FILENAME_MAX);

    if (GetCurrentDir(cwd.data(), FILENAME_MAX))
    {
        cout << "Current working directory: " << cwd.data() << endl;
    }
    else
    {
        cout << "## WARNING: Cannot get the current working directory, errno=" << errno << endl;
    }

    auto recognitionModelPath = GetSetting("EMBEDDED_SPEECH_RECOGNITION_MODEL_PATH", EmbeddedSpeechRecognitionModelPath);
    if (recognitionModelPath.empty() || recognitionModelPath.compare("YourEmbeddedSpeechRecognitionModelPath") == 0)
    {
        recognitionModelPath.clear();
    }

    auto synthesisVoicePath = GetSetting("EMBEDDED_SPEECH_SYNTHESIS_VOICE_PATH", EmbeddedSpeechSynthesisVoicePath);
    if (synthesisVoicePath.empty() || synthesisVoicePath.compare("YourEmbeddedSpeechSynthesisVoicePath") == 0)
    {
        synthesisVoicePath.clear();
    }

    auto translationModelPath = GetSetting("EMBEDDED_SPEECH_TRANSLATION_MODEL_PATH", EmbeddedSpeechTranslationModelPath);
    if (translationModelPath.empty() || translationModelPath.compare("YourEmbeddedSpeechTranslationModelPath") == 0)
    {
        translationModelPath.clear();
    }

    // Find an embedded speech recognition model based on the name or locale.
    if (!recognitionModelPath.empty())
    {
        auto modelName = GetSetting("EMBEDDED_SPEECH_RECOGNITION_MODEL_NAME", EmbeddedSpeechRecognitionModelName);
        auto modelLocale = GetSetting("SPEECH_RECOGNITION_LOCALE", SpeechRecognitionLocale);

        if (modelName.empty() || modelName.compare("YourEmbeddedSpeechRecognitionModelName") == 0)
        {
            modelName.clear(); // no name given -> search by locale
        }

        auto config = EmbeddedSpeechConfig::FromPath(recognitionModelPath);
        auto models = config->GetSpeechRecognitionModels();

        auto result =
            find_if(models.begin(), models.end(), [&](shared_ptr<SpeechRecognitionModel> model)
                {
                    if (modelName.empty())
                    {
                        return model->Locales[0].compare(modelLocale) == 0;
                    }
                    else
                    {
                        return model->Name.compare(modelName) == 0 || model->Locales[0].compare(modelName) == 0;
                    }
                });

        if (result != models.end())
        {
            SpeechRecognitionModelName = (*result)->Name;
        }

        if (SpeechRecognitionModelName.empty())
        {
            cout << "## WARNING: Cannot locate an embedded speech recognition model by ";
            if (modelName.empty())
            {
                cout << "locale \"" << modelLocale << "\". ";
            }
            else
            {
                cout << "name \"" << modelName << "\". ";
            }
            cout << "Current recognition model search path: " << recognitionModelPath << endl;
        }
        else
        {
            SpeechRecognitionModelKey = GetSetting("EMBEDDED_SPEECH_RECOGNITION_MODEL_KEY", EmbeddedSpeechRecognitionModelKey);
            if (SpeechRecognitionModelKey.empty() || SpeechRecognitionModelKey.compare("YourEmbeddedSpeechRecognitionModelKey") == 0)
            {
                SpeechRecognitionModelKey.clear();
                cout << "## WARNING: The key for \"" << SpeechRecognitionModelName << "\" is not set.\n";
            }
        }
    }

    // Find an embedded speech synthesis voice based on the name or locale.
    if (!synthesisVoicePath.empty())
    {
        auto voiceName = GetSetting("EMBEDDED_SPEECH_SYNTHESIS_VOICE_NAME", EmbeddedSpeechSynthesisVoiceName);
        auto voiceLocale = GetSetting("SPEECH_SYNTHESIS_LOCALE", SpeechSynthesisLocale);

        if (voiceName.empty() || voiceName.compare("YourEmbeddedSpeechSynthesisVoiceName") == 0)
        {
            voiceName.clear(); // no name given -> search by locale
        }

        auto config = EmbeddedSpeechConfig::FromPath(synthesisVoicePath);
        auto synthesizer = SpeechSynthesizer::FromConfig(config, nullptr);
        const auto voicesList = synthesizer->GetVoicesAsync("").get();

        if (voicesList->Reason == ResultReason::VoicesListRetrieved)
        {
            const auto& voices = voicesList->Voices;
            auto result =
                find_if(voices.begin(), voices.end(), [&](shared_ptr<VoiceInfo> voice)
                    {
                        if (voiceName.empty())
                        {
                            return voice->Locale.compare(voiceLocale) == 0;
                        }
                        else
                        {
                            return voice->Name.compare(voiceName) == 0 || voice->ShortName.compare(voiceName) == 0;
                        }
                    });

            if (result != voices.end())
            {
                SpeechSynthesisVoiceName = (*result)->Name;
            }
        }

        if (SpeechSynthesisVoiceName.empty())
        {
            cout << "## WARNING: Cannot locate an embedded speech synthesis voice by ";
            if (voiceName.empty())
            {
                cout << "locale \"" << voiceLocale << "\". ";
            }
            else
            {
                cout << "name \"" << voiceName << "\". ";
            }
            cout << "Current synthesis voice search path: " << synthesisVoicePath << endl;
        }
        else
        {
            SpeechSynthesisVoiceKey = GetSetting("EMBEDDED_SPEECH_SYNTHESIS_VOICE_KEY", EmbeddedSpeechSynthesisVoiceKey);
            if (SpeechSynthesisVoiceKey.empty() || SpeechSynthesisVoiceKey.compare("YourEmbeddedSpeechSynthesisVoiceKey") == 0)
            {
                SpeechSynthesisVoiceKey.clear();
                cout << "## WARNING: The key for \"" << SpeechSynthesisVoiceName << "\" is not set.\n";
            }
        }
    }

    // Find an embedded speech translation model based on the name.
    if (!translationModelPath.empty())
    {
        auto modelName = GetSetting("EMBEDDED_SPEECH_TRANSLATION_MODEL_NAME", EmbeddedSpeechTranslationModelName);

        auto config = EmbeddedSpeechConfig::FromPath(translationModelPath);
        auto models = config->GetSpeechTranslationModels();

        auto result =
            find_if(models.begin(), models.end(), [&](shared_ptr<SpeechTranslationModel> model)
                {
                    return model->Name.compare(modelName) == 0;
                });

        if (result != models.end())
        {
            SpeechTranslationModelName = (*result)->Name;
        }

        if (SpeechTranslationModelName.empty())
        {
            cout << "## WARNING: Cannot locate an embedded speech translation model by ";
            cout << "name \"" << modelName << "\". ";
            cout << "Current translation model search path: " << translationModelPath << endl;
        }
        else
        {
            SpeechTranslationModelKey = GetSetting("EMBEDDED_SPEECH_TRANSLATION_MODEL_KEY", EmbeddedSpeechTranslationModelKey);
            if (SpeechTranslationModelKey.empty() || SpeechTranslationModelKey.compare("YourEmbeddedSpeechTranslationModelKey") == 0)
            {
                SpeechTranslationModelKey.clear();
                cout << "## WARNING: The key for \"" << SpeechTranslationModelName << "\" is not set.\n";
            }
        }
    }

    auto maskValue = [](const string& value)
    {
        // Mask the string value, leave only the last 3 chars visible
        string masked = value;
        int visibleLength = masked.length() > 3 ? 3 : 0;
        masked.replace(masked.begin(), masked.end() - visibleLength, masked.length() - visibleLength, '*');
        return masked;
    };

    cout << "Embedded speech recognition\n  model search path: " << (recognitionModelPath.empty() ? "(not set)" : recognitionModelPath) << endl;
    if (!recognitionModelPath.empty())
    {
        cout << "  model name:        " << (SpeechRecognitionModelName.empty() ? "(not found)" : SpeechRecognitionModelName) << endl;
        if (!SpeechRecognitionModelName.empty())
        {
            cout << "  model key:         " << (SpeechRecognitionModelKey.empty() ? "(not set)" : maskValue(SpeechRecognitionModelKey)) << endl;
        }
    }
    cout << "Embedded speech synthesis\n  voice search path: " << (synthesisVoicePath.empty() ? "(not set)" : synthesisVoicePath) << endl;
    if (!synthesisVoicePath.empty())
    {
        cout << "  voice name:        " << (SpeechSynthesisVoiceName.empty() ? "(not found)" : SpeechSynthesisVoiceName) << endl;
        if (!SpeechSynthesisVoiceName.empty())
        {
            cout << "  voice key:         " << (SpeechSynthesisVoiceKey.empty() ? "(not set)" : maskValue(SpeechSynthesisVoiceKey)) << endl;
        }
    }
    cout << "Embedded speech translation\n  model search path: " << (translationModelPath.empty() ? "(not set)" : translationModelPath) << endl;
    if (!translationModelPath.empty())
    {
        cout << "  model name:        " << (SpeechTranslationModelName.empty() ? "(not found)" : SpeechTranslationModelName) << endl;
        if (!SpeechTranslationModelName.empty())
        {
            cout << "  model key:         " << (SpeechTranslationModelKey.empty() ? "(not set)" : maskValue(SpeechTranslationModelKey)) << endl;
        }
    }

    return true;
}
