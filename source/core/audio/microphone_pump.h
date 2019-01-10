//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
#pragma once

#if defined(_MSC_VER)
   #include "windows/microphone_pump.h"
#elif defined(__linux__)
   #include "linux/microphone_pump.h"
#elif defined(__MACH__)
    #include "osx/microphone_pump.h"
#elif defined(ANDROID) || defined(__ANDROID__)
    #include "android/microphone_pump.h"
#else
    #error undefine platform
#endif
