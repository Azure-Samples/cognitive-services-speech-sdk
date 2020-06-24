//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
//

#include <mutex>
#include <string>

#include <azure_c_shared_utility_platform_wrapper.h>
#include <exception.h>
#include "platform_init.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace USP {

using namespace std;
using namespace Microsoft::CognitiveServices::Speech::Impl;

    void PlatformInit(const char* proxyHost, int proxyPort, const char* proxyUsername, const char* proxyPassword)
    {
        static once_flag initOnce;

        call_once(initOnce, [&]
        {
            if (platform_init() != 0)
            {
                ThrowRuntimeError("Failed to initialize platform (azure-c-shared)");
            }

            // Set proxy if needed.
            if (proxyHost != nullptr && *proxyHost != '\0')
            {
                if (proxyPort <= 0)
                {
                    ThrowRuntimeError("Invalid port of the proxy server.");
                }
                string hostAndPort = proxyHost + string(":") + to_string(proxyPort);
                string userNameAndPassword;
                if (proxyUsername != nullptr)
                {
                    if (proxyPassword == nullptr)
                    {
                        ThrowRuntimeError("Invalid password of the proxy service. It should not be null if user name is specified");
                    }
                    userNameAndPassword = proxyUsername + string(":") + proxyPassword;
                }
                platform_set_http_proxy(hostAndPort.c_str(), userNameAndPassword.empty() ? nullptr : userNameAndPassword.c_str());
            }
        });
    }

}}}}
