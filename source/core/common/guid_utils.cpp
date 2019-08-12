//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// guid_utils.cpp: Utility classes/functions dealing with GUIDs
//
#include <cctype>

#define _CRT_SECURE_NO_WARNINGS
#include "stdafx.h"
#include "guid_utils.h"
#include "azure_c_shared_utility_uniqueid_wrapper.h"

#ifdef USING_DEFAULT_UUID
#include <random>
#include <array>
#endif

#define UUID_LENGTH 36

namespace PAL
{

    std::string generate_uuid()
    {
        std::string uuidStr(UUID_LENGTH, char{ 0 });
#ifdef USING_DEFAULT_UUID
        /* HACKHACK: This is to mitigate a problem that exists with the stubbed implementation of guids in azure_c_shared_utilities, work item #1944478 tracks the proper fix */
        std::random_device rd;
        std::uniform_int_distribution<unsigned int> dist(std::numeric_limits<unsigned int>::min(), std::numeric_limits<unsigned int>::max());
        srand(dist(rd));
#endif
        /* In c++17 we have non-const string::data(), maybe we change this in the future */
        auto result = UniqueId_Generate(&uuidStr[0], UUID_LENGTH + 1);
        SPX_IFTRUE_THROW_HR(result != UNIQUEID_OK, SPXERR_UUID_CREATE_FAILED);
        return uuidStr;
    }


    std::wstring CreateGuidWithoutDashes()
    {
        std::string uuidStr = generate_uuid();
        std::wstring uuidWStr;
        auto to_lower = [](unsigned char c)
        {
            return static_cast<unsigned char>(std::tolower(c));
        };
        for (int i = 0; i < UUID_LENGTH; i++)
        {
            if (uuidStr[i] != '-')
            {
                uuidWStr.push_back(to_lower(uuidStr[i]));
            }
        }

        return uuidWStr;
    }

    std::string CreateGuidWithDashesUTF8()
    {
        return generate_uuid();
    }

    std::string DeviceUuid()
    {
        // We make this stable per run for now.
        // Only for roobo make this stable for device.
        static std::string uuidStr(UUID_LENGTH, char{ 0 });
        static int uuidStrValid = 0;

        if (!uuidStrValid)
        {
#if defined(ANDROID) ||defined(_ANDROID_)
            FILE *fp = fopen("/data/sdk-device-uuid.bin", "r");
            if (fp)
            {
                size_t numRead = fread(&uuidStr[0], 1, UUID_LENGTH, fp);
                fclose(fp);

                uuidStrValid = (numRead == UUID_LENGTH);
            }
#endif

            if (!uuidStrValid)
            {
                auto result = UniqueId_Generate(&uuidStr[0], UUID_LENGTH + 1);
                SPX_IFTRUE_THROW_HR(result != UNIQUEID_OK, SPXERR_UUID_CREATE_FAILED);

#if defined(ANDROID) ||defined(_ANDROID_)
                fp = fopen("/data/sdk-device-uuid.bin", "w+");
                if (fp)
                {
                    size_t numWritten = fwrite(&uuidStr[0], 1, UUID_LENGTH, fp);
                    fclose(fp);

                    SPX_IFTRUE_THROW_HR(numWritten == UUID_LENGTH, SPXERR_UUID_CREATE_FAILED);
                    uuidStrValid = (numWritten == UUID_LENGTH);
                }
#endif

                uuidStrValid = 1;
            }
        }

        return uuidStr;
    }

} // PAL
