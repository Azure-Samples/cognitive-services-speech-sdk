// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include "azure_c_shared_utility/platform.h"
#include "EthernetInterface.h"
#include "NTPClient.h"
#include "azure_c_shared_utility/xio.h"
#include "azure_c_shared_utility/tlsio_wolfssl.h"

int setupRealTime(void)
{
    int result;

    if (EthernetInterface::connect())
    {
        result = __LINE__;
    }
    else
    {
        NTPClient ntp;
        if (ntp.setTime("0.pool.ntp.org") != 0)
        {
            result = __LINE__;
        }
        else
        {
            result = 0;
        }
        EthernetInterface::disconnect();
    }

    return result;
}

int platform_init(void)
{
    int result;

    if (EthernetInterface::init())
    {
        result = __LINE__;
    }
    else if (setupRealTime() != 0)
    {
        result = __LINE__;
    } 
    else if (EthernetInterface::connect())
    {
        result = __LINE__;
    }
    else
    {
        result = 0;
    }

    return result;
}

const IO_INTERFACE_DESCRIPTION* platform_get_default_tlsio(void)
{
    return tlsio_wolfssl_get_interface_description();
}

void platform_deinit(void)
{
    EthernetInterface::disconnect();
}
