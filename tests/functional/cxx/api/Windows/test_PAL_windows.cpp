//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "../stdafx.h"

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <windef.h>
#include <winhttp.h>

#include <cstdio>
#include <iostream>
#include <stdexcept>
#include <array>
#include <regex>
#include <locale>
#include <codecvt>

#include <azure_c_shared_utility/platform.h>
#include <azure_c_shared_utility_httpapi_wrapper.h>
#include "test_PAL.h"


bool TEST_PAL_HTTP_Init()
{
    if (platform_init()) {
        return false;
    }

    // now initialise the Azure IoT code
    auto result = HTTPAPI_Init();
    if (result != HTTPAPI_OK)
    {
        return false;
    }

    return true;
}

void TEST_PAL_HTTP_Teardown()
{
    HTTPAPI_Deinit();
    platform_deinit();
}

static void free_ie_proxy_config(WINHTTP_CURRENT_USER_IE_PROXY_CONFIG& config)
{
    if (config.lpszProxy != NULL)
    {
        GlobalFree(config.lpszProxy);
        config.lpszProxy = NULL;
    }

    if (config.lpszAutoConfigUrl != NULL) {
        GlobalFree(config.lpszAutoConfigUrl);
        config.lpszAutoConfigUrl = NULL;
    }

    if (config.lpszProxyBypass != NULL) {
        GlobalFree(config.lpszProxyBypass);
        config.lpszProxyBypass = NULL;
    }
}

bool TEST_PAL_Get_Default_Proxy(TEST_PAL_Proxy_Info& info)
{
    WINHTTP_CURRENT_USER_IE_PROXY_CONFIG proxyInfo;

    try
    {
        // Retrieve the default proxy configuration.
        if (WinHttpGetIEProxyConfigForCurrentUser(&proxyInfo) == FALSE)
        {
            return false;
        }

        if (proxyInfo.fAutoDetect || proxyInfo.lpszProxy == NULL) {
            free_ie_proxy_config(proxyInfo);
            return false;
        }

        std::string proxyList;
        {
            std::wstring proxyListW = proxyInfo.lpszProxy;
            std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
            proxyList = converter.to_bytes(proxyListW);
        }

        // proxy list contains a semicolon or whitespace separated list of proxies
        // e.g. http=http://localhost:8888;https=https://localhost:8888/
        // We want the https one
        std::regex parser("https=(https://)?([^:]+)(:([0-9]+))?");
        std::smatch match;
        if (std::regex_search(proxyList, match, parser) && match.size() > 0)
        {
            info.host = match.str(2);
            info.port = 443;

            if (match.size() >= 5)
            {
                info.port = std::stoi(match.str(4));
            }
        }

        free_ie_proxy_config(proxyInfo);
        return true;
    }
    catch (...)
    {
        free_ie_proxy_config(proxyInfo);
        return false;
    }
}

/*
This is a hack that uses an external executable built in C# to show the sign in dialogue and
return the OpenID token
*/
std::string TEST_PAL_AzureADV2_SignIn(const std::string& clientId, const std::string& redirectUrl)
{
    std::string cmd("\\\\skynet-z400\\shared\\MsaSignIn.exe ");
    cmd += clientId;
    cmd += " ";
    cmd += redirectUrl;

    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(cmd.c_str(), "r"), _pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }

    while (fgets(buffer.data(), (int)buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }

    if (result.length() > 2 && result[result.length() - 1] == '\n')
    {
        // remove trailing \n as that breaks things
        result = result.substr(0, result.length() - 1);
    }

    return result;
}
