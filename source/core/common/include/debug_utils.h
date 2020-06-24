//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
//

#pragma once
#include <string>

namespace Debug
{
    std::string GetCallStack(size_t skipLevels = 0);

    void HookSignalHandlers();

#if defined(UNDER_OSX_ONLY)
    int carbon_backtrace(void** buffer, int size);
    char** carbon_backtrace_symbols(void* const* buffer, int size);
#endif
}
