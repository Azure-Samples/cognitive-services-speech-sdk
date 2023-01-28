//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include <algorithm>
#include <optional>
#include "string_helper.h"
#include "user_config.h"

// The value parameter to std::find must be std::string. Otherwise std::find will compare the pointer values rather than the string contents.
static std::optional<std::string> GetCommandLineOption(char** begin, char** end, const std::string& option)
{
    char** result = std::find(begin, end, option);
    if (result != end)
    {
        // We found the option (for example, "--output"), so advance from that to the value (for example, "filename").
        char** value = ++result;
        // std::string(nullptr) is undefined.        
        if (value != end && nullptr != *value)
        {
            return std::optional<std::string>{ std::string(*value) };
        }
        else
        {
            return std::nullopt;
        }
    }
    else
    {
        return std::nullopt;
    }
}

bool CommandLineOptionExists(char** begin, char** end, const std::string& option)
{
    return std::find(begin, end, option) != end;
}

std::shared_ptr<UserConfig> UserConfigFromArgs(int argc, char* argv[], std::string usage)
{
    // This should not change unless the Speech REST API changes.
    const std::string partialSpeechEndpoint = ".api.cognitive.microsoft.com";
    
    std::optional<std::string> inputAudioURL = GetCommandLineOption(argv, argv + argc, "--input");
    std::optional<std::string> inputFilePath = GetCommandLineOption(argv, argv + argc, "--jsonInput");
    if (!inputAudioURL.has_value() && !inputFilePath.has_value())
    {
        throw std::invalid_argument("Please specify either --input or --jsonInput.\n" + usage);
    }
    
    std::optional<std::string> speechSubscriptionKey = GetCommandLineOption(argv, argv + argc, "--speechKey");
    if (!speechSubscriptionKey.has_value() && !inputFilePath.has_value())
    {
        throw std::invalid_argument("Missing Speech subscription key. Speech subscription key is required unless --jsonInput is present.\n" + usage);
    }
    std::optional<std::string> speechEndpoint = std::nullopt;
    std::optional<std::string> speechRegion = GetCommandLineOption(argv, argv + argc, "--speechRegion");
    if (speechRegion.has_value())
    {
        speechEndpoint = "https://" + speechRegion.value() + partialSpeechEndpoint;
    }
    else if (!inputFilePath.has_value())
    {
        throw std::invalid_argument("Missing Speech region. Speech region is required unless --jsonInput is present.\n" + usage);
    }

    std::optional<std::string> languageSubscriptionKey = GetCommandLineOption(argv, argv + argc, "--languageKey");
    if (!languageSubscriptionKey.has_value())
    {
        throw std::invalid_argument("Missing Language subscription key.\n" + usage);
    }
    std::optional<std::string> languageEndpoint = GetCommandLineOption(argv, argv + argc, "--languageEndpoint");
    if (!languageEndpoint.has_value())
    {
        throw std::invalid_argument("Missing Language endpoint.\n" + usage);
    }
    else if (!StringHelper::StartsWith(languageEndpoint.value(), "https://"))
    {
        languageEndpoint = "https://" + languageEndpoint.value();
    }

    std::optional<std::string> certificatePath = GetCommandLineOption(argv, argv + argc, "--certificate");
    if (!certificatePath.has_value())
    {
        throw std::invalid_argument("Missing certificate path.\n" + usage);
    }
    std::optional<std::string> language = GetCommandLineOption(argv, argv + argc, "--language");
    if (!language.has_value())
    {
        language = std::optional{ "en" };
    }
    std::optional<std::string> locale = GetCommandLineOption(argv, argv + argc, "--locale");
    if (!locale.has_value())
    {
        locale = std::optional{ "en-US" };
    }

    return std::make_shared<UserConfig>(
        CommandLineOptionExists(argv, argv + argc, "--stereo"),
        certificatePath.value(),
        inputAudioURL,
        inputFilePath,
        GetCommandLineOption(argv, argv + argc, "--output"),
        language.value(),
        locale.value(),
        speechSubscriptionKey,
        speechEndpoint,
        languageSubscriptionKey.value(),
        languageEndpoint.value()
    );
}
