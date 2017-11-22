// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef MICROMOCCKTESTRUNNERHOOKS_H
#define MICROMOCCKTESTRUNNERHOOKS_H

#pragma once

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#ifndef MOCK_ASSERT

#include "stdafx.h"
#include "testrunnerswitcher.h"

#define MOCK_ASSERT(expected, actual, assertString)     ASSERT_ARE_EQUAL((expected), (actual), (assertString))
#define MOCK_FAIL(expression)                           ASSERT_FAIL(expression)

#ifndef MBED_BUILD_TIMESTAMP
#define MOCK_THROW(mockException)                       throw(mockException)
#else
#define MOCK_THROW(mockException)
#endif

#else // MOCK_ASSERT
#define MOCK_THROW(...)
#endif // MOCK_ASSERT

#endif // MICROMOCCKTESTRUNNERHOOKS_H
