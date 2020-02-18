//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#pragma once

#include <string>

typedef struct ProxyServerInfo
{
    std::string host;
    int port;
    std::string username;
    std::string password;
} ProxyServerInfo;
