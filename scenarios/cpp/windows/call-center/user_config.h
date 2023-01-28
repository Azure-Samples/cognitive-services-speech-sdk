//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
#pragma once

#include <iostream>

struct UserConfig
{
    const bool useStereoAudio = false;
    const std::string certificatePath;
    const std::optional<std::string> inputAudioURL;
    const std::optional<std::string> inputFilePath;
    const std::optional<std::string> outputFilePath;
    const std::string language;
    const std::string locale;
    const std::optional<std::string> speechSubscriptionKey;
    const std::optional<std::string> speechEndpoint;
    const std::string languageSubscriptionKey;
    const std::string languageEndpoint;
    
    UserConfig(
        bool useStereoAudio,
        std::string certificatePath,
        std::optional<std::string> inputAudioURL,
        std::optional<std::string> inputFilePath,
        std::optional<std::string> outputFilePath,
        std::string language,
        std::string locale,
        std::optional<std::string> speechSubscriptionKey,
        std::optional<std::string> speechEndpoint,
        std::string languageSubscriptionKey,
        std::string languageEndpoint
        ) :
        useStereoAudio(useStereoAudio),
        certificatePath(certificatePath),
        inputAudioURL(inputAudioURL),
        inputFilePath(inputFilePath),
        outputFilePath(outputFilePath),
        language(language),
        locale(locale),
        speechSubscriptionKey(speechSubscriptionKey),
        speechEndpoint(speechEndpoint),
        languageSubscriptionKey(languageSubscriptionKey),
        languageEndpoint(languageEndpoint)
        {}
};

std::shared_ptr<UserConfig> UserConfigFromArgs(int argc, char* argv[], std::string usage);
bool CommandLineOptionExists(char** begin, char** end, const std::string& option);
