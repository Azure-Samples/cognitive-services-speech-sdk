//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#pragma once

#include <string>

/**
* @brief Initialises the HTTP stack for your operating system
*
* @return true on success, false otherwise
*/
extern bool TEST_PAL_HTTP_Init();

/**
* @brief Tears down the HTTP stack for your operating system
*
* @return 0 on success, non-zero in the case of errors
*/
extern void TEST_PAL_HTTP_Teardown();

typedef struct {
    std::string host;
    int port;
} TEST_PAL_Proxy_Info;

/**
* @brief Gets the system default proxy (if one is set)
*
* @param info Reference to the proxy information structure to populate
*
* @return true on success, false otherwise
*/
extern bool TEST_PAL_Get_Default_Proxy(TEST_PAL_Proxy_Info& info);

/**
* @brief Starts the Azure ADV2 sign in process that supports both personal and work accounts
*
* @param clientId The identifier for your client
* @param redirectUrl Where to redirect to after you sign in or cancel
*
* @return The Open ID token for the user you signed in as, or an empty string otherwise
*/
extern std::string TEST_PAL_AzureADV2_SignIn(const std::string& clientId, const std::string& redirectUrl);
