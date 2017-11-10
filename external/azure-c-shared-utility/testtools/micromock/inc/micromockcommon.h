// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef MICROMOCKCOMMON_H
#define MICROMOCKCOMMON_H

#pragma once

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "tchar.h"
#include "string"
#include "sstream"

#define COUNT_OF(a)     (sizeof(a) / sizeof((a)[0]))

namespace std
{
    typedef std::basic_string<TCHAR> tstring;
    typedef std::basic_ostringstream<TCHAR> tostringstream;
}

#endif // MICROMOCKCOMMON_H
