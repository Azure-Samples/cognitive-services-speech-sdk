//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "../stdafx.h"
#include <azure_c_shared_utility_platform_wrapper.h>
#include "test_PAL.h"

#ifndef UNUSED
#define UNUSED(_p) (void)_p
#endif

bool TEST_PAL_HTTP_Init()
{
    return platform_init() == 0;
}

void TEST_PAL_HTTP_Teardown()
{
    platform_deinit();
}

bool TEST_PAL_Get_Default_Proxy(TEST_PAL_Proxy_Info& info)
{
    UNUSED(info);
    return false;
}

std::string TEST_PAL_AzureADV2_SignIn(const std::string& clientId, const std::string& redirectUrl)
{
    UNUSED(clientId);
    UNUSED(redirectUrl);
    return std::string();
}
