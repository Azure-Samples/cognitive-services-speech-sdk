//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
//

#pragma once
#include <string>

namespace Debug
{
    std::string GetCallStack(size_t skipLevels = 0, bool makeFunctionNamesStandOut = false);

    void HookSignalHandlers();
}
