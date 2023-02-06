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

// Locale to be used in speech recognition (SR), cloud and embedded. In BCP-47 format, case-sensitive.
// If EmbeddedSpeechRecognitionModelName is changed from the default, it will override this for embedded.
const string SpeechRecognitionLocale = "en-US"; // or set SPEECH_RECOGNITION_LOCALE

// Locale to be used in speech synthesis (text-to-speech, TTS), cloud and embedded. In BCP-47 format, case-sensitive.
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

// Cloud speech service subscription information.
// This is needed with hybrid (cloud & embedded) speech configuration.
const string CloudSpeechSubscriptionKey = "YourCloudSpeechSubscriptionKey"; // or set CLOUD_SPEECH_SUBSCRIPTION_KEY
const string CloudSpeechServiceRegion = "YourCloudSpeechServiceRegion"; // or set CLOUD_SPEECH_SERVICE_REGION

// END OF CONFIGURABLE SETTINGS


// Embedded speech recognition input audio format settings.
// In addition, little-endian signed integer samples are required.
// THIS IS THE ONLY SUPPORTED AUDIO FORMAT AT THE MOMENT. DO NOT MODIFY.
// These settings are just included here to make them explicit and visible.
uint32_t GetEmbeddedSpeechSamplesPerSecond() { return 16000; }
uint8_t GetEmbeddedSpeechBitsPerSample() { return 16; }
uint8_t GetEmbeddedSpeechChannels() { return 1; }

// Get names and other properties of example files included with the sample project.
const string GetSpeechRawAudioFileName() { return "data/speech_test.raw"; }
const string GetSpeechWavAudioFileName() { return "data/speech_test.wav"; }
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

// Creates an instance of an embedded speech config.
shared_ptr<EmbeddedSpeechConfig> CreateEmbeddedSpeechConfig()
{
    vector<string> paths;

    // Add paths for offline data.
    auto modelPath = GetSetting("EMBEDDED_SPEECH_RECOGNITION_MODEL_PATH", EmbeddedSpeechRecognitionModelPath);
    if (!modelPath.empty() && modelPath.compare("YourEmbeddedSpeechRecognitionModelPath") != 0)
    {
        paths.push_back(modelPath);
    }
    auto voicePath = GetSetting("EMBEDDED_SPEECH_SYNTHESIS_VOICE_PATH", EmbeddedSpeechSynthesisVoicePath);
    if (!voicePath.empty() && voicePath.compare("YourEmbeddedSpeechSynthesisVoicePath") != 0)
    {
        paths.push_back(voicePath);
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

    // Enable profanity masking.
    /*
    config->SetProfanity(ProfanityOption::Masked);
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
        cout << "Current working directory:      " << cwd.data() << endl;
    }
    else
    {
        cout << "## WARNING: Cannot get the current working directory, errno=" << errno << endl;
    }

    auto modelPath = GetSetting("EMBEDDED_SPEECH_RECOGNITION_MODEL_PATH", EmbeddedSpeechRecognitionModelPath);
    if (modelPath.empty() || modelPath.compare("YourEmbeddedSpeechRecognitionModelPath") == 0)
    {
        cout << "## WARNING: Embedded SR model search path is not set.\n";
        modelPath.clear();
    }

    auto voicePath = GetSetting("EMBEDDED_SPEECH_SYNTHESIS_VOICE_PATH", EmbeddedSpeechSynthesisVoicePath);
    if (voicePath.empty() || voicePath.compare("YourEmbeddedSpeechSynthesisVoicePath") == 0)
    {
        cout << "## WARNING: Embedded TTS voice search path is not set.\n";
        voicePath.clear();
    }

    if (modelPath.empty() && voicePath.empty())
    {
        cerr << "## ERROR: Cannot run without embedded SR models or TTS voices.\n";
        return false;
    }

    // Find an embedded speech recognition model based on the name or locale.
    if (!modelPath.empty())
    {
        auto modelName = GetSetting("EMBEDDED_SPEECH_RECOGNITION_MODEL_NAME", EmbeddedSpeechRecognitionModelName);
        auto modelLocale = GetSetting("SPEECH_RECOGNITION_LOCALE", SpeechRecognitionLocale);

        if (modelName.empty() || modelName.compare("YourEmbeddedSpeechRecognitionModelName") == 0)
        {
            modelName.clear(); // no name given -> search by locale
        }

        auto config = EmbeddedSpeechConfig::FromPath(modelPath);
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
                        return model->Name.compare(modelName) == 0;
                    }
                });

        if (result != models.end())
        {
            SpeechRecognitionModelName = (*result)->Name;
        }

        if (SpeechRecognitionModelName.empty())
        {
            cout << "## WARNING: Cannot locate an embedded SR model by ";
            if (modelName.empty())
            {
                cout << "locale \"" << modelLocale << "\". ";
            }
            else
            {
                cout << "name \"" << modelName << "\". ";
            }
            cout << "Current model search path: " << modelPath << endl;
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
    if (!voicePath.empty())
    {
        auto voiceName = GetSetting("EMBEDDED_SPEECH_SYNTHESIS_VOICE_NAME", EmbeddedSpeechSynthesisVoiceName);
        auto voiceLocale = GetSetting("SPEECH_SYNTHESIS_LOCALE", SpeechSynthesisLocale);

        if (voiceName.empty() || voiceName.compare("YourEmbeddedSpeechSynthesisVoiceName") == 0)
        {
            voiceName.clear(); // no name given -> search by locale
        }

        auto config = EmbeddedSpeechConfig::FromPath(voicePath);
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
                            return voice->Name.compare(voiceName) == 0;
                        }
                    });

            if (result != voices.end())
            {
                SpeechSynthesisVoiceName = (*result)->Name;
            }
        }

        if (SpeechSynthesisVoiceName.empty())
        {
            cout << "## WARNING: Cannot locate an embedded TTS voice by ";
            if (voiceName.empty())
            {
                cout << "locale \"" << voiceLocale << "\". ";
            }
            else
            {
                cout << "name \"" << voiceName << "\". ";
            }
            cout << "Current voice search path: " << voicePath << endl;
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

    if (SpeechRecognitionModelName.empty() && SpeechSynthesisVoiceName.empty())
    {
        cerr << "## ERROR: Cannot run without embedded SR models or TTS voices.\n";
        return false;
    }

    auto maskValue = [](const string& value)
    {
        // Mask the string value, leave only the last 3 chars visible
        string masked = value;
        int visibleLength = masked.length() > 3 ? 3 : 0;
        masked.replace(masked.begin(), masked.end() - visibleLength, masked.length() - visibleLength, '*');
        return masked;
    };

    cout << "Embedded SR model search path:  " << (modelPath.empty() ? "(not set)" : modelPath) << endl;
    if (!modelPath.empty())
    {
        cout << "Embedded SR model name:         " << (SpeechRecognitionModelName.empty() ? "(not found)" : SpeechRecognitionModelName) << endl;
        if (!SpeechRecognitionModelName.empty())
        {
            cout << "Embedded SR model key:          " << (SpeechRecognitionModelKey.empty() ? "(not set)" : maskValue(SpeechRecognitionModelKey)) << endl;
        }
    }
    cout << "Embedded TTS voice search path: " << (voicePath.empty() ? "(not set)" : voicePath) << endl;
    if (!voicePath.empty())
    {
        cout << "Embedded TTS voice name:        " << (SpeechSynthesisVoiceName.empty() ? "(not found)" : SpeechSynthesisVoiceName) << endl;
        if (!SpeechSynthesisVoiceName.empty())
        {
            cout << "Embedded TTS voice key:         " << (SpeechSynthesisVoiceKey.empty() ? "(not set)" : maskValue(SpeechSynthesisVoiceKey)) << endl;
        }
    }

    return true;
}
