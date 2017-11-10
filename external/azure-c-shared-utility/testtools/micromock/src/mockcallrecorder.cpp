// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <cstdlib>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "stdafx.h"
#include "mockcallrecorder.h"
#include "mockvaluebase.h"
#include "micromocktestrunnerhooks.h"
#include "mockcallcomparer.h"
#include "micromockexception.h"

CMockCallRecorder::CMockCallRecorder(AUTOMATIC_CALL_COMPARISON performAutomaticCallComparison, CMockCallComparer* mockCallComparer) :
    m_PerformAutomaticCallComparison(performAutomaticCallComparison),
    m_MockCallComparer(mockCallComparer)
{
    MicroMockInitializeCriticalSection(&m_MockCallRecorderCS);
}

CMockCallRecorder::~CMockCallRecorder(void)
{
    if (m_PerformAutomaticCallComparison)
    {
        AssertActualAndExpectedCalls();
    }

    ResetAllCalls();

    MicroMockDeleteCriticalSection(&m_MockCallRecorderCS);
}

void CMockCallRecorder::AssertActualAndExpectedCalls(void)
{
#if defined CPP_UNITTEST || defined USE_CTEST
    if ((GetMissingCalls().length()>0) || (GetUnexpectedCalls().length()>0))
    {
        std::tstring result = std::tstring(_T("Expected: ")) + GetMissingCalls() + std::tstring(_T(" Actual: ")) + GetUnexpectedCalls();
        MOCK_FAIL(result.c_str());
    }
#else /*so it is TAEF*/
    WEX::Common::String missingCalls(GetMissingCalls().c_str());
    WEX::Common::String unexpectedCalls(GetUnexpectedCalls().c_str());
    MOCK_ASSERT(missingCalls, unexpectedCalls, _T("Missing calls do not match expected calls"));
#endif

    m_PerformAutomaticCallComparison = AUTOMATIC_CALL_COMPARISON_OFF;
}

void CMockCallRecorder::RecordExpectedCall(CMockMethodCallBase* mockMethodCall)
{
    Lock();
    m_ExpectedCalls.push_back(mockMethodCall);
    Unlock();
}

CMockValueBase* CMockCallRecorder::RecordActualCall(CMockMethodCallBase* mockMethodCall, bool* failed)
{
    CMockValueBase* result = NULL;

    MicroMockEnterCriticalSection(&m_MockCallRecorderCS);

    m_ActualCalls.push_back(mockMethodCall);
    result = MatchActualCall(mockMethodCall, failed);
    MicroMockLeaveCriticalSection(&m_MockCallRecorderCS);

    return result;
}

CMockValueBase* CMockCallRecorder::MatchActualCall(CMockMethodCallBase* mockMethodCall, bool* failed)
{
    CMockValueBase* result = NULL;

    MicroMockEnterCriticalSection(&m_MockCallRecorderCS);

    if (mockMethodCall->HasMatch())
    {
        mockMethodCall->m_MatchedCall->RollbackMatch();
        mockMethodCall->RollbackMatch();
    }

    CMockMethodCallBase* matchedCall = NULL;
    if (NULL != m_MockCallComparer)
    {
        matchedCall = m_MockCallComparer->MatchCall(m_ExpectedCalls, mockMethodCall);
    }
    else
    {
        MOCK_THROW(CMicroMockException(MICROMOCK_EXCEPTION_INTERNAL_ERROR,
            _T("Invalid Mock Call Comparer")));
    }

    if (NULL != matchedCall)
    {
        mockMethodCall->CopyOutArgumentBuffers(matchedCall);
        CMockValueBase* failValue = matchedCall->GetFailReturnValue();
        if (failValue != NULL)
        {
            result = failValue;
            *failed = true;
        }
        else
        {
            result = matchedCall->GetReturnValue();
            *failed = false;
        }
    }
    else
    {
        *failed = false;
    }

    MicroMockLeaveCriticalSection(&m_MockCallRecorderCS);

    return result;
}

std::tstring CMockCallRecorder::CompareActualAndExpectedCalls()
{
    return GetMissingCalls(_T("Expected:")) + GetUnexpectedCalls(_T("Actual:"));
}

std::tstring CMockCallRecorder::GetUnexpectedCalls(std::tstring unexpectedCallPrefix)
{
    std::tostringstream result;

    if (NULL != m_MockCallComparer)
    {
        for (size_t i = 0; i < m_ActualCalls.size(); i++)
        {
            if (m_MockCallComparer->IsUnexpectedCall(m_ActualCalls[i]))
            {
                result << _T("[");
                result << unexpectedCallPrefix;
                result << m_ActualCalls[i]->ToString();
                result << _T("]");
            }
        }
    }
    else
    {
        MOCK_THROW(CMicroMockException(MICROMOCK_EXCEPTION_INTERNAL_ERROR,
            _T("Invalid Mock Call Comparer")));
    }

    m_PerformAutomaticCallComparison = AUTOMATIC_CALL_COMPARISON_OFF;

    return result.str();
}

std::tstring CMockCallRecorder::GetMissingCalls(std::tstring missingCallPrefix)
{
    std::tostringstream result;

    if (NULL != m_MockCallComparer)
    {
        for (size_t i = 0; i < m_ExpectedCalls.size(); i++)
        {
            if (m_MockCallComparer->IsMissingCall(m_ExpectedCalls[i]))
            {
                result << _T("[");
                result << missingCallPrefix;
                result << m_ExpectedCalls[i]->ToString();
                result << _T("]");

                if (m_ExpectedCalls[i]->m_ExpectedTimes - m_ExpectedCalls[i]->m_MatchedTimes > 1)
                {
                    result << _T("(x");
                    result << m_ExpectedCalls[i]->m_ExpectedTimes - m_ExpectedCalls[i]->m_MatchedTimes;
                    result << _T(")");
                }
            }
        }
    }
    else
    {
        MOCK_THROW(CMicroMockException(MICROMOCK_EXCEPTION_INTERNAL_ERROR,
            _T("Invalid Mock Call Comparer")));
    }

    m_PerformAutomaticCallComparison = AUTOMATIC_CALL_COMPARISON_OFF;

    return result.str();
}

void CMockCallRecorder::ResetExpectedCalls()
{
    MicroMockEnterCriticalSection(&m_MockCallRecorderCS);

    for (size_t i = 0; i < m_ExpectedCalls.size(); i++)
    {
        delete m_ExpectedCalls[i];
    }

    for (size_t i = 0; i < m_ActualCalls.size(); i++)
    {
        m_ActualCalls[i]->m_MatchedCall = NULL;
    }

    m_ExpectedCalls.clear();

    MicroMockLeaveCriticalSection(&m_MockCallRecorderCS);
}

void CMockCallRecorder::ResetActualCalls()
{
    MicroMockEnterCriticalSection(&m_MockCallRecorderCS);

    for (size_t i = 0; i < m_ActualCalls.size(); i++)
    {
        delete m_ActualCalls[i];
    }

    for (size_t i = 0; i < m_ExpectedCalls.size(); i++)
    {
        m_ExpectedCalls[i]->m_MatchedCall = NULL;
    }

    m_ActualCalls.clear();

    MicroMockLeaveCriticalSection(&m_MockCallRecorderCS);
}

void CMockCallRecorder::ResetAllCalls()
{
    ResetActualCalls();
    ResetExpectedCalls();
}

void CMockCallRecorder::SetPerformAutomaticCallComparison(AUTOMATIC_CALL_COMPARISON performAutomaticCallComparison)
{
    m_PerformAutomaticCallComparison = performAutomaticCallComparison;
}

SAL_Acquires_lock_(m_MockCallRecorderCS)
void CMockCallRecorder::Lock()
{
    MicroMockEnterCriticalSection(&m_MockCallRecorderCS);
}

SAL_Releases_lock_(m_MockCallRecorderCS)
void CMockCallRecorder::Unlock()
{
    MicroMockLeaveCriticalSection(&m_MockCallRecorderCS);
}
