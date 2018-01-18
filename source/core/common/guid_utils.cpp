//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// guid_utils.cpp: Utility classes/functions dealing with GUIDs
//

#include "stdafx.h"
#include "guid_utils.h"
#include "azure_c_shared_utility/uniqueid.h"

#define UUID_LENGTH 36

namespace PAL {
    // TODO: drop prefix, use a namespace instead?
    std::wstring CreateGuid()
    {
        std::string uuidStr(UUID_LENGTH, char{ 0 });
        auto result = UniqueId_Generate(&uuidStr[0], UUID_LENGTH + 1);

        SPX_IFTRUE_THROW_HR(result != UNIQUEID_OK, SPXERR_UUID_CREATE_FAILED);

        std::wstring uuidWStr;
        for (int i = 0; i < UUID_LENGTH; i++)
        {
            if (uuidStr[i] != '-')
            {
                uuidWStr.push_back(uuidStr[i]);
            }
        }

        return uuidWStr;
    }
}; // PAL