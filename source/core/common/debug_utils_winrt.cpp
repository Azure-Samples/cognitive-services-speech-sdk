//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// debug_utils_winrt.cpp implements the debug info for winRT
//
#include "stdafx.h"
#include "debug_utils.h"

namespace Debug {

    std::string GetCallStack(size_t skipLevels/* = 0*/, bool makeFunctionNamesStandOut/* = false*/)
    {
        UNUSED(skipLevels);
        UNUSED(makeFunctionNamesStandOut);

        return "not implemented in WinRT\n";
    }
    void HookSignalHandlers()
    {
    }
}
