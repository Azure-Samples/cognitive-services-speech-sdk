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
#include "http_utils.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class EndpointUtils
{
public:
    static std::pair<bool, std::string> IsTokenServiceEndpoint(const std::string& endpointStr)
    {
        bool isTokenServiceEndpoint = false;
        auto lowercaseText = endpointStr;
        std::string endpointRegion;

        transform(lowercaseText.begin(), lowercaseText.end(), lowercaseText.begin(),
            [](unsigned char c) ->char { return (char)::tolower(c); });

        if (std::string::npos !=
            lowercaseText.find(".api.cognitive.microsoft.com/sts/v1.0/issuetoken"))
        {
            isTokenServiceEndpoint = true;

            // Read the region from the endpoint string
            auto url = HttpUtils::ParseUrl(lowercaseText);
            size_t pos = url.host.find(".");

            if (std::string::npos != pos && pos > 0)
            {
                endpointRegion = url.host.substr(0, pos);
            }
        }

        return std::make_pair(isTokenServiceEndpoint, endpointRegion);
    };
};

} } } } // Microsoft::CognitiveServices::Speech::Impl
