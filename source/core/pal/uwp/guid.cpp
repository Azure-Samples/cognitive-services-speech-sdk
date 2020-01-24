//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
#include "guid.h"

#include <sstream>
#include <iomanip>

#include <combaseapi.h>

namespace PAL = Microsoft::CognititveServices::Speech::PAL;

std::string PAL::GenerateGUID()
{
    GUID guid;
    auto hr = CoCreateGuid(&guid);
    if (hr != S_OK)
    {
        throw std::runtime_error{ "GUID creation failed." };
    }
    std::stringstream ss{};
    auto print_as_hex = [&ss](auto v)
    {
        constexpr auto w = (2 * sizeof(v)) / sizeof(char);
        ss << std::uppercase << std::setfill('0') << std::setw(w) << std::hex << static_cast<uint32_t>(v);
    };
    print_as_hex(guid.Data1);
    ss << "-";
    print_as_hex(guid.Data2);
    ss << "-";
    print_as_hex(guid.Data3);
    ss << "-";
    print_as_hex(guid.Data4[0]);
    print_as_hex(guid.Data4[1]);
    ss << "-";
    print_as_hex(guid.Data4[2]);
    print_as_hex(guid.Data4[3]);
    print_as_hex(guid.Data4[4]);
    print_as_hex(guid.Data4[5]);
    print_as_hex(guid.Data4[6]);
    print_as_hex(guid.Data4[7]);
    return ss.str();
}