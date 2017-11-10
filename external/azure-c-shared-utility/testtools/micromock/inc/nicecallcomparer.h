// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef NICECALLCOMPARER_H
#define NICECALLCOMPARER_H

#pragma once

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "stdafx.h"
#include "strictunorderedcallcomparer.h"
#include "mockmethodcallbase.h"

template<class T>
class CNiceCallComparer :
    public CStrictUnorderedCallComparer<T>
{
public:
    CNiceCallComparer(_In_ AUTOMATIC_CALL_COMPARISON performAutomaticCallComparison = AUTOMATIC_CALL_COMPARISON_ON) :
        CStrictUnorderedCallComparer<T>(performAutomaticCallComparison)
    {
        T::CMockCallRecorder::m_MockCallComparer->SetIgnoreUnexpectedCalls(true);
    }
};

#endif // NICECALLCOMPARER_H
