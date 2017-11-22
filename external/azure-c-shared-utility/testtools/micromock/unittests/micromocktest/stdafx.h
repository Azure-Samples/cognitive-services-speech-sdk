// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include <cstdlib>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

// 4505 is disabled due to unused template specializations declared in CppUnitTest
#pragma warning(disable:4505)

// Headers for CppUnitTest
//#include "testrunnerswitcher.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN
#include "micromock.h"
#include "timediscretemicromock.h"
