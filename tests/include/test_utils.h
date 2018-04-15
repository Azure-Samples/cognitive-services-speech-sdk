//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#pragma once

#include <iostream>
#include <random>
#include <signal.h>
#include <fstream>
#include "string_utils.h"
#include "debug_utils.h"


inline bool exists(const std::wstring& name) {
    return std::ifstream(PAL::ToString(name).c_str()).good();
}

inline std::ifstream get_stream(const std::wstring& name) {
    return std::ifstream(PAL::ToString(name).c_str(), std::ifstream::binary);
}

typedef std::linear_congruential_engine<uint_fast32_t, 1664525, 1013904223, UINT_FAST32_MAX> random_engine;

inline void add_signal_handlers()
{
    Debug::HookSignalHandlers();
}
