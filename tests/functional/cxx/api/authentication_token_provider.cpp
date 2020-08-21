//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "authentication_token_provider.h"
#include <http_request.h>
#include <http_response.h>
#include <http_endpoint_info.h>

using namespace std;
using namespace std::chrono;
using namespace std::chrono_literals;
using namespace Microsoft::CognitiveServices::Speech::Impl;

static auto constexpr MIN_AUTH_TOKEN_VALIDITY = 5min;
static auto constexpr REQUESTED_AUTH_TOKEN_VALIDITY = 30min;

static string GenerateKey(const string& subscriptionKey, const string& region)
{
    return subscriptionKey + " | " + region;
}

string AuthenticationTokenProvider::GetOrCreateToken(const string& subscriptionKey, const string& region)
{
    if (subscriptionKey.empty() || region.empty())
    {
        throw invalid_argument("The subscription key and/or region cannot be empty");
    }

    auto& instance = Instance();
    lock_guard<recursive_mutex> lock(instance.m_lock);
    {
        string key = GenerateKey(subscriptionKey, region);
        auto found = instance.m_entries.find(key);

        string token;
        if (found != instance.m_entries.end())
        {
            auto minimumValidity = steady_clock::now() + MIN_AUTH_TOKEN_VALIDITY;
            if (minimumValidity <= found->second.validUntil)
            {
                token = found->second.authenticationToken;
            }
        }

        if (token.empty())
        {
            token = CreateToken(subscriptionKey, region, REQUESTED_AUTH_TOKEN_VALIDITY);
        }

        return token;
    }
}

string AuthenticationTokenProvider::CreateToken(const string& subscriptionKey, const string& region, chrono::seconds validity)
{
    if (subscriptionKey.empty())
    {
        throw invalid_argument("You must specify a subscription key");
    }
    else if (region.empty())
    {
        throw invalid_argument("You must specify a region");
    }
    else if (validity <= 0s)
    {
        throw invalid_argument("You must specify a validity greater than 0 seconds");
    }

    Entry entry;
    entry.subscriptionKey = subscriptionKey;
    entry.region = region;
    entry.validUntil = steady_clock::now() + validity;

    HttpEndpointInfo authTokenEndpoint;
    authTokenEndpoint
        .Scheme(UriScheme::HTTPS)
        .Host(region + ".api.cognitive.microsoft.com")
        .Path("/sts/v1.0/issueToken")
        .AddQueryParameter("expiredTime", to_string(validity.count()))
        .SetHeader("Ocp-Apim-Subscription-Key", subscriptionKey);

    HttpRequest request(authTokenEndpoint);
    auto response = request.SendRequest(HTTPAPI_REQUEST_POST);
    response->EnsureSuccess();

    string token = entry.authenticationToken = response->ReadContentAsString();

    auto& instance = Instance();
    lock_guard<recursive_mutex> lock(instance.m_lock);
    {
        string key = GenerateKey(subscriptionKey, region);
        instance.m_entries[key] = move(entry);

        return token;
    }
}

AuthenticationTokenProvider& AuthenticationTokenProvider::Instance()
{
    static AuthenticationTokenProvider instance;
    return instance;
}
