// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef MOCKCALLCOMPARER_H
#define MOCKCALLCOMPARER_H

#pragma once

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "stdafx.h"
#include "mockmethodcallbase.h"

class CMockCallComparer
{
public:
    CMockCallComparer() :
        m_IgnoreUnexpectedCalls(false)
    {
    }

    void SetIgnoreUnexpectedCalls(_In_ bool ignoreUnexpectedCalls) { m_IgnoreUnexpectedCalls = ignoreUnexpectedCalls; }
    _Must_inspect_result_ bool GetIgnoreUnexpectedCalls(void) { return m_IgnoreUnexpectedCalls; }

    virtual _Must_inspect_result_ bool IsUnexpectedCall(_In_ const CMockMethodCallBase* actualCall) = 0;
    virtual _Must_inspect_result_ bool IsMissingCall(_In_ const CMockMethodCallBase* actualCall) = 0;
    virtual _Must_inspect_result_ CMockMethodCallBase* MatchCall(std::vector<CMockMethodCallBase*>& expectedCalls,
        CMockMethodCallBase* actualCall) = 0;

protected:
    bool m_IgnoreUnexpectedCalls;
};

#endif // MOCKCALLCOMPARER_H
