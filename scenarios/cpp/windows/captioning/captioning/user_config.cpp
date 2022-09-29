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
    std::optional<std::string> key = GetCommandLineOption(argv, argv + argc, "--key");
    if (!key.has_value())
    {
        throw std::invalid_argument("Missing subscription key.\n" + usage);
    }
    std::optional<std::string> region = GetCommandLineOption(argv, argv + argc, "--region");
    if (!region.has_value())
    {
        throw std::invalid_argument("Missing region.\n" + usage);
    }

    std::optional<std::vector<std::string>> languageIDLanguages = std::nullopt;
    std::optional<std::string> languageIDLanguagesResult = GetCommandLineOption(argv, argv + argc, "--languages");
    if (languageIDLanguagesResult.has_value())
    {
        languageIDLanguages = std::optional<std::vector<std::string>>{ StringHelper::Split(languageIDLanguagesResult.value(), ';') };
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
        languageIDLanguages,
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
        key.value(),
        region.value()
    );
}

std::string V2EndpointFromRegion(std::string region)
{
    // Note: Continuous language identification is supported only with v2 endpoints.
    return std::string("wss://" + region + ".stt.speech.microsoft.com/speech/universal/v2");
}
