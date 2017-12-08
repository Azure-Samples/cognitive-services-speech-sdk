//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// guid_utils.cpp: Utility classes/functions dealing with GUIDs
//

#include "stdafx.h"
#include "guid_utils.h"
#include <windows.h>


std::wstring PAL_CreateGuid()
{
    UUID uuid;
    auto status = UuidCreate(&uuid);
    SPX_IFTRUE_THROW_HR(status != RPC_S_OK, SPXERR_UUID_CREATE_FAILED);

    RPC_WSTR rpcStr;
    status = UuidToStringW(&uuid, &rpcStr);
    SPX_IFTRUE_THROW_HR(status != RPC_S_OK, SPXERR_UUID_CREATE_FAILED);

    auto str = reinterpret_cast<const wchar_t*>(rpcStr);

    std::wstring uuidStr;
    for (int i = 0; i < wcslen(str); i++)
    {
        if (rpcStr[i] != '-')
        {
            uuidStr.push_back(rpcStr[i]);
        }
    }

    return uuidStr;
}
