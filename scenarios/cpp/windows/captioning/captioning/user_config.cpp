//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include <algorithm>
#include <cctype>
#include <exception>
#include <iomanip>
#include <iostream>
#include <optional>
#include <string>
#include <sstream>
#include <vector>
#include "string_helper.h"
#include "user_config.h"

bool CommandLineOptionExists(char** begin, char** end, const std::string& option)
{
    return std::find(begin, end, option) != end;
}

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

static AudioStreamContainerFormat GetCompressedAudioFormat(char** begin, char** end)
{
    std::optional<std::string> format = GetCommandLineOption(begin, end, "--format");
    if (!format.has_value())
    {
        return AudioStreamContainerFormat::ANY;
    }
    else
    {
        std::string value = StringHelper::ToLower(format.value());
        if ("alaw" == value)
        {
            return AudioStreamContainerFormat::ALAW;
        }
        else if ("flac" == value)
        {
            return AudioStreamContainerFormat::FLAC;
        }
        else if ("mp3" == value)
        {
            return AudioStreamContainerFormat::MP3;
        }
        else if ("mulaw" == value)
        {
            return AudioStreamContainerFormat::MULAW;
        }
        else if ("ogg_opus" == value)
        {
            return AudioStreamContainerFormat::OGG_OPUS;
        }
        else
        {
            return AudioStreamContainerFormat::ANY;
        }
    }
}

std::string GetEnvironmentVariable(const char* name)
{
#if defined(_MSC_VER)
    size_t requiredSize = 0;
    (void)getenv_s(&requiredSize, nullptr, 0, name);
    if (requiredSize == 0)
    {
        return "";
    }
    auto buffer = std::make_unique<char[]>(requiredSize);
    (void)getenv_s(&requiredSize, buffer.get(), requiredSize, name);
    return buffer.get();
#else
    auto value = getenv(name);
    return value ? value : "";
#endif
}

static ProfanityOption GetProfanityOption(char** begin, char** end)
{
    std::optional<std::string> profanity = GetCommandLineOption(begin, end, "--profanity");
    if (!profanity.has_value())
    {
        return ProfanityOption::Masked;
    }
    else
    {
        std::string value = StringHelper::ToLower(profanity.value());
        if ("raw" == value)
        {
            return ProfanityOption::Raw;
        }
        else if ("remove" == value)
        {
            return ProfanityOption::Removed;
        }
        else
        {
            return ProfanityOption::Masked;
        }
    }
}

std::shared_ptr<UserConfig> UserConfigFromArgs(int argc, char* argv[], std::string usage)
{   
    std::optional<std::string> keyOption = GetCommandLineOption(argv, argv + argc, "--key");
    std::string key = keyOption.has_value() ? keyOption.value() : GetEnvironmentVariable("SPEECH_KEY");
    if (0 == size(key))
    {
        throw std::invalid_argument("Please set the SPEECH_KEY environment variable or provide a Speech resource key with the --key option.\n" + usage);
    }

    std::optional<std::string> regionOption = GetCommandLineOption(argv, argv + argc, "--region");
    std::string region = regionOption.has_value() ? regionOption.value() : GetEnvironmentVariable("SPEECH_REGION");
    if (0 == size(region))
    {
        throw std::invalid_argument("Please set the SPEECH_REGION environment variable or provide a Speech resource region with the --region option.\n" + usage);
    }
    
    std::string language = "en-US";
    std::optional<std::string> languageResult = GetCommandLineOption(argv, argv + argc, "--language");
    if (languageResult.has_value())
    {
        language = languageResult.value();
    }

    CaptioningMode captioningMode = CommandLineOptionExists(argv, argv + argc, "--realTime") && !CommandLineOptionExists(argv, argv + argc, "--offline") ? CaptioningMode::RealTime : CaptioningMode::Offline;
    
    std::optional<std::string> strRemainTime = GetCommandLineOption(argv, argv + argc, "--remainTime");
    int remainTime = 1000;
    if (strRemainTime.has_value())
    {
        remainTime = std::stoi(strRemainTime.value());
        if (remainTime < 0)
        {
            remainTime = 1000;
        }
    }
    
    std::optional<std::string> strDelay = GetCommandLineOption(argv, argv + argc, "--delay");
    int delay = 1000;
    if (strDelay.has_value())
    {
        delay = std::stoi(strDelay.value());
        if (delay < 0)
        {
            delay = 1000;
        }
    }
    
    std::optional<std::string> strMaxLineLength = GetCommandLineOption(argv, argv + argc, "--maxLineLength");
    int maxLineLength = UserConfig::defaultMaxLineLengthSBCS;
    if (strMaxLineLength.has_value())
    {
        maxLineLength = std::stoi(strMaxLineLength.value());
        if (maxLineLength < 20)
        {
            maxLineLength = 20;
        }
    }
    
    std::optional<std::string> strLines = GetCommandLineOption(argv, argv + argc, "--lines");
    int lines = 2;
    if (strLines.has_value())
    {
        lines = std::stoi(strLines.value());
        if (lines < 1)
        {
            lines = 2;
        }
    }

    return std::make_shared<UserConfig>(
        CommandLineOptionExists(argv, argv + argc, "--format"),
        GetCompressedAudioFormat(argv, argv + argc),
        GetProfanityOption(argv, argv + argc),
        language,
        GetCommandLineOption(argv, argv + argc, "--input"),
        GetCommandLineOption(argv, argv + argc, "--output"),
        GetCommandLineOption(argv, argv + argc, "--phrases"),
        CommandLineOptionExists(argv, argv + argc, "--quiet"),
        captioningMode,
        remainTime,
        delay,
        CommandLineOptionExists(argv, argv + argc, "--srt"),
        maxLineLength,
        lines,        
        GetCommandLineOption(argv, argv + argc, "--threshold"),
        key,
        region
    );
}
