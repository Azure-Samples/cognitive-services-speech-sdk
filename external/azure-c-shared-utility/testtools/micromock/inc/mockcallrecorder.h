// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef MOCKCALLRECORDER_H
#define MOCKCALLRECORDER_H

#pragma once

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "stdafx.h"
#include "mockmethodcallbase.h"
#include "micromocktestrunnerhooks.h"
#include "mockcallcomparer.h"

typedef enum AUTOMATIC_CALL_COMPARISON_TAG
{
    AUTOMATIC_CALL_COMPARISON_OFF,
    AUTOMATIC_CALL_COMPARISON_ON
} AUTOMATIC_CALL_COMPARISON;

class CMockCallRecorder
{
public:
    CMockCallRecorder(AUTOMATIC_CALL_COMPARISON performAutomaticCallComparison = AUTOMATIC_CALL_COMPARISON_ON,
        CMockCallComparer* mockCallComparer = NULL);
    virtual ~CMockCallRecorder(void);

public:
    void RecordExpectedCall(CMockMethodCallBase* mockMethodCall);
    CMockValueBase * RecordActualCall(CMockMethodCallBase * mockMethodCall, bool* failed);
    CMockValueBase* MatchActualCall(CMockMethodCallBase* mockMethodCall, bool* failed);
    void AssertActualAndExpectedCalls(void);
    std::tstring CompareActualAndExpectedCalls(void);
    std::tstring GetUnexpectedCalls(std::tstring unexpectedCallPrefix = _T(""));
    std::tstring GetMissingCalls(std::tstring missingCallPrefix = _T(""));
    void ResetExpectedCalls();
    void ResetActualCalls();
    void ResetAllCalls();
    SAL_Acquires_lock_(m_MockCallRecorderCS) void Lock();
    SAL_Releases_lock_(m_MockCallRecorderCS) void Unlock();
    void SetPerformAutomaticCallComparison(AUTOMATIC_CALL_COMPARISON performAutomaticCallComparison);
    void SetMockCallComparer(CMockCallComparer* mockCallComparer) { m_MockCallComparer = mockCallComparer; };

protected:
    std::vector<CMockMethodCallBase*> m_ExpectedCalls;
    std::vector<CMockMethodCallBase*> m_ActualCalls;

protected:
    MICROMOCK_CRITICAL_SECTION m_MockCallRecorderCS;
    AUTOMATIC_CALL_COMPARISON m_PerformAutomaticCallComparison;
    CMockCallComparer* m_MockCallComparer;
};

#endif // MOCKCALLRECORDER_H
