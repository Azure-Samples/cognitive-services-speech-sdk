// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef MOCK_H
#define MOCK_H

#pragma once

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "mockcallrecorder.h"
#include "strictunorderedcallcomparer.h"

#ifndef UNREFERENCED_PARAMETER
#define UNREFERENCED_PARAMETER(param)   param = param;
#endif

template<class C>
class CMock : public CMockCallRecorder
{
public:
    CMock(_In_ AUTOMATIC_CALL_COMPARISON performAutomaticCallComparison = AUTOMATIC_CALL_COMPARISON_ON)
    {
        SetPerformAutomaticCallComparison(performAutomaticCallComparison);
    }
};

#endif // MOCK_H
