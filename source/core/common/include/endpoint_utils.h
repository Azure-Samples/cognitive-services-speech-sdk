//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// endpoint_utils.h: Utility class for endpoint handling
//

#pragma once

#include <algorithm>
#include <string>
#include <utility>
#include <regex>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class EndpointUtils
{
public:
    static std::pair<bool, std::string> IsTokenServiceEndpoint(const std::string& endpointStr)
    {
        const std::regex tokenServiceEndpoint(R"(https://([^\.]+)\..+/issuetoken)"); // "https://<region>.<whatever>/issuetoken"
        std::smatch matches;
        bool isTokenServiceEndpoint = false;
        auto lowercaseText = endpointStr;
        std::string endpointRegion;

        transform(lowercaseText.begin(), lowercaseText.end(), lowercaseText.begin(),
            [](unsigned char c) ->char { return (char)::tolower(c); });

        if (std::regex_match(lowercaseText, matches, tokenServiceEndpoint))
        {
            isTokenServiceEndpoint = true;
            // The first match is the whole string; the second
            // is the first parenthesized expression in regex.
            endpointRegion = matches[1].str();
        }

        return std::make_pair(isTokenServiceEndpoint, endpointRegion);
    };
};

} } } } // Microsoft::CognitiveServices::Speech::Impl
