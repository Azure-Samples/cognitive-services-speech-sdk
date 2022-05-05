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
#include <sstream>
#include "helper.h"

static std::vector<std::string> Split(const std::string& s, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while(std::getline(tokenStream, token, delimiter))
    {
        tokens.push_back(token);
    }
    return tokens;
}

bool EndsWith(const std::string& str, const std::string& suffix)
{
    return str.size() >= suffix.size() && 0 == str.compare(str.size()-suffix.size(), suffix.size(), suffix);
}

static std::string ToLower(std::string str)
{
    std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c){ return std::tolower(c); });
    return str;
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

bool CommandLineOptionExists(char** begin, char** end, const std::string& option)
{
    return std::find(begin, end, option) != end;
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
        std::string value = ToLower(format.value());
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
        std::string value = ToLower(profanity.value());
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

std::string V2EndpointFromRegion(std::string region)
{
    // Note: Continuous language identification is supported only with v2 endpoints.
    return std::string("wss://" + region + ".stt.speech.microsoft.com/speech/universal/v2");
}

Timestamp TimestampFromTicks(uint64_t startTicks, uint64_t endTicks)
{
    const float ticksPerSecond = 10000000.0;

    const float startSeconds = startTicks / ticksPerSecond;
    const float endSeconds = endTicks / ticksPerSecond;
    
    const int startMinutes = (int)startSeconds / 60;
    const int endMinutes = (int)endSeconds / 60;    
    
    const int startHours = startMinutes / 60;
    const int endHours = endMinutes / 60;

    return Timestamp(startHours, endHours, startMinutes % 60, endMinutes % 60, fmod(startSeconds, 60.0), fmod(endSeconds, 60.0));
}

std::shared_ptr<UserConfig> UserConfigFromArgs(int argc, char* argv[], std::string usage)
{
    std::optional<std::vector<std::string>> languageIDLanguages = std::nullopt;
    std::optional<std::string> languageIDLanguagesResult = GetCommandLineOption(argv, argv + argc, "--languages");
    if (languageIDLanguagesResult.has_value())
    {
        languageIDLanguages = std::optional<std::vector<std::string>>{ Split(languageIDLanguagesResult.value(), ';') };
    }

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

    return std::make_shared<UserConfig>(
        CommandLineOptionExists(argv, argv + argc, "--format"),
        GetCompressedAudioFormat(argv, argv + argc),
        GetProfanityOption(argv, argv + argc),
        languageIDLanguages,
        GetCommandLineOption(argv, argv + argc, "--input"),
        GetCommandLineOption(argv, argv + argc, "--output"),
        GetCommandLineOption(argv, argv + argc, "--phrases"),
        CommandLineOptionExists(argv, argv + argc, "--quiet"),
        CommandLineOptionExists(argv, argv + argc, "--recognizing"),
        GetCommandLineOption(argv, argv + argc, "--threshold"),
        CommandLineOptionExists(argv, argv + argc, "--srt"),
        key.value(),
        region.value()
    );
}
