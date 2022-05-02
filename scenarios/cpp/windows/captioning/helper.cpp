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

// See:
// https://stackoverflow.com/a/42844629
bool EndsWith(const std::string& str, const std::string& suffix)
{
    return str.size() >= suffix.size() && 0 == str.compare(str.size()-suffix.size(), suffix.size(), suffix);
}

// See:
// https://stackoverflow.com/a/313990
static std::string ToLower(std::string str)
{
    std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c){ return std::tolower(c); });
    return str;
}

// Simple command line argument parsing functions. See
// https://stackoverflow.com/a/868894
// The value parameter to std::find must be std::string. Otherwise std::find will compare the pointer values rather than the string contents.
static std::optional<std::string> GetCmdOption(char** begin, char** end, const std::string& option)
{
    auto result = std::find(begin, end, option);
    if (result != end)
    {
        // We found the option (for example, "--output"), so advance from that to the value (for example, "filename").
        auto value = ++result;
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

inline bool CmdOptionExists(char** begin, char** end, const std::string& option)
{
    return std::find(begin, end, option) != end;
}

static AudioStreamContainerFormat GetCompressedAudioFormat(char** begin, char** end)
{
    auto format = GetCmdOption(begin, end, "--format");
    if (!format.has_value())
    {
        return AudioStreamContainerFormat::ANY;
    }
    else
    {
        auto value = ToLower(format.value());
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
    auto profanity = GetCmdOption(begin, end, "--profanity");
    if (!profanity.has_value())
    {
        return ProfanityOption::Masked;
    }
    else
    {
        auto value = ToLower(profanity.value());
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
    const auto ticksPerSecond = 10000000.0;

    const auto startSeconds_1 = startTicks / ticksPerSecond;
    const auto endSeconds_1 = endTicks / ticksPerSecond;
    
    const auto startMinutes_1 = startSeconds_1 / 60;
    const auto endMinutes_1 = endSeconds_1 / 60;    
    
    const auto startHours = startMinutes_1 / 60;
    const auto endHours = endMinutes_1 / 60;
    
    const auto startSeconds_2 = fmod(startSeconds_1, 60.0);
    const auto endSeconds_2 = fmod(endSeconds_1, 60.0);
    
    const auto startMinutes_2 = fmod(startMinutes_1, 60.0);
    const auto endMinutes_2 = fmod(endMinutes_1, 60.0);
    
    return Timestamp(startHours, endHours, startMinutes_2, endMinutes_2, startSeconds_2, endSeconds_2);
}

std::shared_ptr<UserConfig> UserConfigFromArgs(int argc, char* argv[], std::string usage)
{
    std::optional<std::vector<std::string>> languageIDLanguages = std::nullopt;
    auto languageIDLanguagesResult = GetCmdOption(argv, argv + argc, "--languages");
    if (languageIDLanguagesResult.has_value())
    {
        languageIDLanguages = std::optional<std::vector<std::string>>{ Split(languageIDLanguagesResult.value(), ',') };
    }

    auto key = GetCmdOption(argv, argv + argc, "--key");
    if (!key.has_value())
    {
        throw std::invalid_argument("Missing subscription key.\n" + usage);
    }
    auto region = GetCmdOption(argv, argv + argc, "--region");
    if (!region.has_value())
    {
        throw std::invalid_argument("Missing region.\n" + usage);
    }

    return std::make_shared<UserConfig>(
        CmdOptionExists(argv, argv + argc, "--format"),
        GetCompressedAudioFormat(argv, argv + argc),
        GetProfanityOption(argv, argv + argc),
        languageIDLanguages,
        GetCmdOption(argv, argv + argc, "--input"),
        GetCmdOption(argv, argv + argc, "--output"),
        GetCmdOption(argv, argv + argc, "--phrases"),
        CmdOptionExists(argv, argv + argc, "--quiet"),
        CmdOptionExists(argv, argv + argc, "--recognizing"),
        GetCmdOption(argv, argv + argc, "--threshold"),
        CmdOptionExists(argv, argv + argc, "--srt"),
        key.value(),
        region.value()
    );
}
