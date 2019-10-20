//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
#include "guid.h"
#include "environment.h"

#include <array>

#ifdef USING_DEFAULT_UUID
#include <random>
#endif

constexpr size_t UUID_LENGTH = 36;

namespace PAL = Microsoft::CognititveServices::Speech::PAL;

std::string PAL::GenerateGUID()
{
    std::array<char, UUID_LENGTH + 1> buffer;
#ifdef USING_DEFAULT_UUID
    /* HACKHACK: This is to mitigate a problem that exists with the stubbed implementation of guids in azure_c_shared_utilities, work item #1944478 tracks the proper fix */
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<unsigned int> dist(std::numeric_limits<unsigned int>::min(), std::numeric_limits<unsigned int>::max());
    srand(dist(gen));
#endif
    UniqueId_Generate(buffer.data(), UUID_LENGTH + 1);
    return std::string{ buffer.data() };
}
