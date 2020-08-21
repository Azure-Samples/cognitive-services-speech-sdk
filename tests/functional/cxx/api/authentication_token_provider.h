//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#pragma once

#include <unordered_map>
#include <string>
#include <chrono>
#include <mutex>

/// <summary>
/// Helper class to get and cache authentication tokens
/// </summary>
class AuthenticationTokenProvider
{
public:
    /// <summary>
    /// Gets the authentication token for the specified subscription key and region. If there is already
    /// an existing authentication token, that is valid for at least 5 minutes, then it will be returned.
    /// Otherwise a new authentication token will be requested.
    /// </summary>
    /// <param name="subscriptionKey">The Cognitive subscription key</param>
    /// <param name="region">The Cognitive subscription region</param>
    /// <returns>The authentication token to use</returns>
    /// <exception cref="invalid_argument">If one or more of the arguments are invalid</exception>
    /// <exception cref="HttpException">If we failed to create an authentication token</exception>
    static std::string GetOrCreateToken(const std::string& subscriptionKey, const std::string& region);

    /// <summary>
    /// Creates a new authentication token for the specified subscription key and region
    /// </summary>
    /// <param name="subscriptionKey">The subscription key</param>
    /// <param name="region">The subscription</param>
    /// <param name="validity">How long the token should be valid for</param>
    /// <returns>The generated token</returns>
    /// <exception cref="invalid_argument">If one or more of the arguments are invalid</exception>
    /// <exception cref="HttpException">If we failed to create an authentication token</exception
    static std::string CreateToken(const std::string& subscriptionKey, const std::string& region, std::chrono::seconds validity);

private:
    AuthenticationTokenProvider() = default;
    AuthenticationTokenProvider&& operator =(const AuthenticationTokenProvider&&) = delete;

    static AuthenticationTokenProvider& Instance();

    struct Entry
    {
        std::string region;
        std::string subscriptionKey;
        std::string authenticationToken;
        std::chrono::steady_clock::time_point validUntil;
    };

private:
    std::recursive_mutex m_lock;
    std::unordered_map<std::string, Entry> m_entries;
};
