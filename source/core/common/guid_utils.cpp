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

#ifdef USING_DEFAULT_UUID
#include <random>
#endif

#include "guid.h"

#define UUID_LENGTH 36

namespace PAL
{
    namespace PAL = Microsoft::CognititveServices::Speech::PAL;

    std::wstring CreateGuidWithoutDashes()
    {
        std::string uuidStr = PAL::GenerateGUID();
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
        return PAL::GenerateGUID();
    }

    std::string DeviceUuid()
    {
        // We make this stable per run for now.
        // Only for roobo make this stable for device.
        static std::string uuidStr(UUID_LENGTH, char{ 0 });
        static int uuidStrValid = 0;

        if (!uuidStrValid)
        {
#if defined(ANDROID) || defined(_ANDROID_)
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
                uuidStr = PAL::GenerateGUID();

#if defined(ANDROID) || defined(_ANDROID_)
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
