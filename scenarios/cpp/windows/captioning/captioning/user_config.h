//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
#pragma once

#include <speechapi_cxx.h>
#include <optional>
#include <string>
#include <vector>

using namespace Microsoft::CognitiveServices::Speech;
using namespace Microsoft::CognitiveServices::Speech::Audio;

enum CaptioningMode
{
    Offline,
    RealTime
};

class UserConfig
{
public:
    const static int defaultMaxLineLengthSBCS = 37;
    const static int defaultMaxLineLengthMBCS = 30;

    const bool useCompressedAudio = false;
    const AudioStreamContainerFormat compressedAudioFormat = AudioStreamContainerFormat::ANY;
    const ProfanityOption profanityOption = ProfanityOption::Masked;
    const std::string language;
    const std::optional<std::string> inputFile = std::nullopt;
    const std::optional<std::string> outputFile = std::nullopt;
    const std::optional<std::string> phraseList;
    const bool suppressConsoleOutput = false;
    const CaptioningMode captioningMode;
    const int remainTime;
    const int delay;
    const bool useSubRipTextCaptionFormat = false;
    const int maxLineLength;
    const int lines;
    const std::optional<std::string> stablePartialResultThreshold = std::nullopt;
    const std::string subscriptionKey;
    const std::string region;
    
    UserConfig(
        bool useCompressedAudio,
        AudioStreamContainerFormat compressedAudioFormat,
        ProfanityOption profanityOption,
        std::string language,
        std::optional<std::string> inputFile,
        std::optional<std::string> outputFile,
        std::optional<std::string> phraseList,
        bool suppressConsoleOutput,
        CaptioningMode captioningMode,
        int remainTime,
        int delay,
        bool useSubRipTextCaptionFormat,
        int maxLineLength,
        int lines,
        std::optional<std::string> stablePartialResultThreshold,
        std::string subscriptionKey,
        std::string region
        ) :
        useCompressedAudio(useCompressedAudio),
        compressedAudioFormat(compressedAudioFormat),
        profanityOption(profanityOption),
        language(language),
        inputFile(inputFile),
        outputFile(outputFile),
        phraseList(phraseList),
        suppressConsoleOutput(suppressConsoleOutput),
        captioningMode(captioningMode),
        remainTime(remainTime),
        delay(delay),
        useSubRipTextCaptionFormat(useSubRipTextCaptionFormat),
        maxLineLength(maxLineLength),
        lines(lines),
        stablePartialResultThreshold(stablePartialResultThreshold),
        subscriptionKey(subscriptionKey),
        region(region)
        {}
};

bool CommandLineOptionExists(char** begin, char** end, const std::string& option);
std::string getEnvironmentVariable(const char* name);
std::shared_ptr<UserConfig> UserConfigFromArgs(int argc, char* argv[], std::string usage);
