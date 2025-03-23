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

// Embedded speech model license (text).
// This applies to embedded speech recognition, synthesis and translation.
// It is presumed that all the customer's embedded speech models use the same license.
const string EmbeddedSpeechModelLicense = "YourEmbeddedSpeechModelLicense"; // or set EMBEDDED_SPEECH_MODEL_LICENSE

// Path to the local embedded speech recognition model(s) on the device file system.
// This may be a single model folder or a top-level folder for several models.
// Use an absolute path or a path relative to the application working folder.
// The path is recursively searched for model files.
// Files belonging to a specific model must be available as normal individual files in a model folder,
// not inside an archive, and they must be readable by the application process.
const string EmbeddedSpeechRecognitionModelPath = "YourEmbeddedSpeechRecognitionModelPath"; // or set EMBEDDED_SPEECH_RECOGNITION_MODEL_PATH

// Name of the embedded speech recognition model to be used for recognition.
// For example: "en-US" or "Microsoft Speech Recognizer en-US FP Model V8"
const string EmbeddedSpeechRecognitionModelName = "YourEmbeddedSpeechRecognitionModelName"; // or set EMBEDDED_SPEECH_RECOGNITION_MODEL_NAME

// Path to the local embedded speech synthesis voice(s) on the device file system.
// This may be a single voice folder or a top-level folder for several voices.
// Use an absolute path or a path relative to the application working folder.
// The path is recursively searched for voice files.
// Files belonging to a specific voice must be available as normal individual files in a voice folder,
// not inside an archive, and they must be readable by the application process.
const string EmbeddedSpeechSynthesisVoicePath = "YourEmbeddedSpeechSynthesisVoicePath"; // or set EMBEDDED_SPEECH_SYNTHESIS_VOICE_PATH

// Name of the embedded speech synthesis voice to be used for synthesis.
// For example: "en-US-JennyNeural" or "Microsoft Server Speech Text to Speech Voice (en-US, JennyNeural)"
const string EmbeddedSpeechSynthesisVoiceName = "YourEmbeddedSpeechSynthesisVoiceName"; // or set EMBEDDED_SPEECH_SYNTHESIS_VOICE_NAME

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

// Cloud speech service subscription and language settings.
// These are needed with hybrid (cloud & embedded) speech configuration.
// The language must be specified in BCP-47 format, case-sensitive.
const string CloudSpeechSubscriptionKey = "YourCloudSpeechSubscriptionKey"; // or set CLOUD_SPEECH_SUBSCRIPTION_KEY
const string CloudSpeechServiceEndpoint = "YourCloudSpeechServiceEndpoint"; // or set CLOUD_SPEECH_SERVICE_ENDPOINT
const string CloudSpeechRecognitionLanguage = "en-US"; // or set CLOUD_SPEECH_RECOGNITION_LANGUAGE
const string CloudSpeechSynthesisLanguage = "en-US"; // or set CLOUD_SPEECH_SYNTHESIS_LANGUAGE

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
string SpeechModelLicense;
string SpeechRecognitionModelPath;
string SpeechRecognitionModelName;
string SpeechSynthesisVoicePath;
string SpeechSynthesisVoiceName;
string SpeechTranslationModelPath;
string SpeechTranslationModelName;

// Utility functions for main menu.
bool HasSpeechRecognitionModel()
{
    if (SpeechRecognitionModelPath.empty() || SpeechRecognitionModelName.empty())
    {
        cerr << "## ERROR: No speech recognition model specified.\n";
        return false;
    }
    return true;
}

bool HasSpeechSynthesisVoice()
{
    if (SpeechSynthesisVoicePath.empty() || SpeechSynthesisVoiceName.empty())
    {
        cerr << "## ERROR: No speech synthesis voice specified.\n";
        return false;
    }
    return true;
}

bool HasSpeechTranslationModel()
{
    if (SpeechTranslationModelPath.empty() || SpeechTranslationModelName.empty())
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
    if (!SpeechRecognitionModelPath.empty())
    {
        paths.push_back(SpeechRecognitionModelPath);
    }
    if (!SpeechSynthesisVoicePath.empty())
    {
        paths.push_back(SpeechSynthesisVoicePath);
    }
    if (!SpeechTranslationModelPath.empty())
    {
        paths.push_back(SpeechTranslationModelPath);
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
        config->SetSpeechRecognitionModel(SpeechRecognitionModelName, SpeechModelLicense);
    }

    if (!SpeechSynthesisVoiceName.empty())
    {
        // Mandatory configuration for embedded speech synthesis.
        config->SetSpeechSynthesisVoice(SpeechSynthesisVoiceName, SpeechModelLicense);
        if (SpeechSynthesisVoiceName.find("Neural") != string::npos)
        {
            // Embedded neural voices only support 24kHz sample rate.
            config->SetSpeechSynthesisOutputFormat(SpeechSynthesisOutputFormat::Riff24Khz16BitMonoPcm);
        }
    }

    if (!SpeechTranslationModelName.empty())
    {
        // Mandatory configuration for embedded speech translation.
        config->SetSpeechTranslationModel(SpeechTranslationModelName, SpeechModelLicense);
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
    auto cloudSpeechConfig = SpeechConfig::FromEndpoint(
        GetSetting("CLOUD_SPEECH_SERVICE_ENDPOINT", CloudSpeechServiceEndpoint),
        GetSetting("CLOUD_SPEECH_SUBSCRIPTION_KEY", CloudSpeechSubscriptionKey)
    );
    // Optional language configuration for cloud speech services.
    // The internal default is en-US.
    // Also see
    // https://github.com/Azure-Samples/cognitive-services-speech-sdk/tree/master/samples/cpp/windows/console
    // for complete Speech SDK samples using cloud speech services.
    cloudSpeechConfig->SetSpeechRecognitionLanguage(GetSetting("CLOUD_SPEECH_RECOGNITION_LANGUAGE", CloudSpeechRecognitionLanguage));
    cloudSpeechConfig->SetSpeechSynthesisLanguage(GetSetting("CLOUD_SPEECH_SYNTHESIS_LANGUAGE", CloudSpeechSynthesisLanguage));

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

    SpeechModelLicense = GetSetting("EMBEDDED_SPEECH_MODEL_LICENSE", EmbeddedSpeechModelLicense);
    if (SpeechModelLicense.empty() || SpeechModelLicense.compare("YourEmbeddedSpeechModelLicense") == 0)
    {
        cerr << "## ERROR: The embedded speech model license is not set.\n";
        return false;
    }

    SpeechRecognitionModelPath = GetSetting("EMBEDDED_SPEECH_RECOGNITION_MODEL_PATH", EmbeddedSpeechRecognitionModelPath);
    if (SpeechRecognitionModelPath.compare("YourEmbeddedSpeechRecognitionModelPath") == 0)
    {
        SpeechRecognitionModelPath.clear();
    }
    SpeechRecognitionModelName = GetSetting("EMBEDDED_SPEECH_RECOGNITION_MODEL_NAME", EmbeddedSpeechRecognitionModelName);
    if (SpeechRecognitionModelName.compare("YourEmbeddedSpeechRecognitionModelName") == 0)
    {
        SpeechRecognitionModelName.clear();
    }

    SpeechSynthesisVoicePath = GetSetting("EMBEDDED_SPEECH_SYNTHESIS_VOICE_PATH", EmbeddedSpeechSynthesisVoicePath);
    if (SpeechSynthesisVoicePath.compare("YourEmbeddedSpeechSynthesisVoicePath") == 0)
    {
        SpeechSynthesisVoicePath.clear();
    }
    SpeechSynthesisVoiceName = GetSetting("EMBEDDED_SPEECH_SYNTHESIS_VOICE_NAME", EmbeddedSpeechSynthesisVoiceName);
    if (SpeechSynthesisVoiceName.compare("YourEmbeddedSpeechSynthesisVoiceName") == 0)
    {
        SpeechSynthesisVoiceName.clear();
    }

    SpeechTranslationModelPath = GetSetting("EMBEDDED_SPEECH_TRANSLATION_MODEL_PATH", EmbeddedSpeechTranslationModelPath);
    if (SpeechTranslationModelPath.compare("YourEmbeddedSpeechTranslationModelPath") == 0)
    {
        SpeechTranslationModelPath.clear();
    }
    SpeechTranslationModelName = GetSetting("EMBEDDED_SPEECH_TRANSLATION_MODEL_NAME", EmbeddedSpeechTranslationModelName);
    if (SpeechTranslationModelName.compare("YourEmbeddedSpeechTranslationModelName") == 0)
    {
        SpeechTranslationModelName.clear();
    }

    // Find an embedded speech recognition model based on the name.
    if (!SpeechRecognitionModelPath.empty() && !SpeechRecognitionModelName.empty())
    {
        auto config = EmbeddedSpeechConfig::FromPath(SpeechRecognitionModelPath);
        auto models = config->GetSpeechRecognitionModels();

        auto result =
            find_if(models.begin(), models.end(), [&](shared_ptr<SpeechRecognitionModel> model)
                {
                    return model->Name.compare(SpeechRecognitionModelName) == 0 || model->Locales[0].compare(SpeechRecognitionModelName) == 0;
                });

        if (result == models.end())
        {
            cout << "## WARNING: Cannot locate an embedded speech recognition model \"" << SpeechRecognitionModelName << "\"\n";
        }
    }

    // Find an embedded speech synthesis voice based on the name.
    if (!SpeechSynthesisVoicePath.empty() && !SpeechSynthesisVoiceName.empty())
    {
        auto config = EmbeddedSpeechConfig::FromPath(SpeechSynthesisVoicePath);
        auto synthesizer = SpeechSynthesizer::FromConfig(config, nullptr);

        bool found = false;
        const auto voicesList = synthesizer->GetVoicesAsync("").get();

        if (voicesList->Reason == ResultReason::VoicesListRetrieved)
        {
            const auto& voices = voicesList->Voices;
            auto result =
                find_if(voices.begin(), voices.end(), [&](shared_ptr<VoiceInfo> voice)
                    {
                        return voice->Name.compare(SpeechSynthesisVoiceName) == 0 || voice->ShortName.compare(SpeechSynthesisVoiceName) == 0;
                    });

            if (result != voices.end())
            {
                found = true;
            }
        }

        if (!found)
        {
            cout << "## WARNING: Cannot locate an embedded speech synthesis voice \"" << SpeechSynthesisVoiceName << "\"\n";
        }
    }

    // Find an embedded speech translation model based on the name.
    if (!SpeechTranslationModelPath.empty() && !SpeechTranslationModelName.empty())
    {
        auto config = EmbeddedSpeechConfig::FromPath(SpeechTranslationModelPath);
        auto models = config->GetSpeechTranslationModels();

        auto result =
            find_if(models.begin(), models.end(), [&](shared_ptr<SpeechTranslationModel> model)
                {
                    return model->Name.compare(SpeechTranslationModelName) == 0;
                });

        if (result == models.end())
        {
            cout << "## WARNING: Cannot locate an embedded speech translation model \"" << SpeechTranslationModelName << "\"\n";
        }
    }

    cout << "Embedded speech recognition\n";
    cout << "  model search path: " << (SpeechRecognitionModelPath.empty() ? "(not set)" : SpeechRecognitionModelPath) << endl;
    cout << "  model name:        " << (SpeechRecognitionModelName.empty() ? "(not set)" : SpeechRecognitionModelName) << endl;
    cout << "Embedded speech synthesis\n";
    cout << "  voice search path: " << (SpeechSynthesisVoicePath.empty() ? "(not set)" : SpeechSynthesisVoicePath) << endl;
    cout << "  voice name:        " << (SpeechSynthesisVoiceName.empty() ? "(not set)" : SpeechSynthesisVoiceName) << endl;
    cout << "Embedded speech translation\n";
    cout << "  model search path: " << (SpeechTranslationModelPath.empty() ? "(not set)" : SpeechTranslationModelPath) << endl;
    cout << "  model name:        " << (SpeechTranslationModelName.empty() ? "(not set)" : SpeechTranslationModelName) << endl;

    return true;
}
