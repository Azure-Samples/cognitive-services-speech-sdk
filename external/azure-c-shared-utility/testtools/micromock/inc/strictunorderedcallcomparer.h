// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef STRICTUNORDEREDCALLCOMPARER_H
#define STRICTUNORDEREDCALLCOMPARER_H

#pragma once

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "stdafx.h"
#include "mockcallcomparer.h"
#include "mockmethodcallbase.h"


template<class T>
class CStrictUnorderedCallComparer : virtual public CMockCallComparer, public T
{
public:
    CStrictUnorderedCallComparer(_In_ AUTOMATIC_CALL_COMPARISON performAutomaticCallComparison = AUTOMATIC_CALL_COMPARISON_ON)
    {
        T::SetPerformAutomaticCallComparison(performAutomaticCallComparison);
        T::SetMockCallComparer(this);
        T::CMockCallRecorder::m_MockCallComparer->SetIgnoreUnexpectedCalls(false);
    }

    _Must_inspect_result_
    virtual bool IsUnexpectedCall(_In_ const CMockMethodCallBase* actualCall)
    {
        if (T::CMockCallRecorder::m_MockCallComparer->GetIgnoreUnexpectedCalls())
        {
            return actualCall->m_AlwaysReport;
        }
        else
        {
            return !actualCall->HasMatch();
        }
    }

    _Must_inspect_result_
    virtual bool IsMissingCall(_In_ const CMockMethodCallBase* expectedCall)
    {
        return ((!expectedCall->HasMatch()) &&
                (expectedCall->m_ExpectedTimes > 0) &&
                (!expectedCall->m_IgnoreAllCalls) &&
                (!expectedCall->m_OnlySpecifiesActions));
    }

    _Must_inspect_result_
    virtual CMockMethodCallBase* MatchCall(std::vector<CMockMethodCallBase*>& expectedCalls,
        CMockMethodCallBase* actualCall)
     {
        CMockMethodCallBase* result = NULL;

        // go through all items in the expected call array
        for (size_t i = 0; i < expectedCalls.size(); i++)
        {
            CMockMethodCallBase* expectedCall = expectedCalls[i];

            // if the expected call says all calls of this type should be ignored, 
            // then match
            if ((expectedCall->m_IgnoreAllCalls) &&
                (*expectedCall == *actualCall))
            {
                actualCall->m_MatchedCall = expectedCall;
                expectedCall->m_MatchedCall = actualCall;
                break;
            }
            else
            {
                // if the expected call has not yet been matched
                if ((!expectedCall->HasMatch() || expectedCall->m_ExactExpectedTimes) && 
                    (!expectedCall->m_OnlySpecifiesActions) &&
                    // and it matches the actual call
                    (*expectedCall == *actualCall))
                {
                    // check whether the number of expected calls of this type have been found
                    // in the actual calls array
                    if (expectedCall->m_MatchedTimes < expectedCall->m_ExpectedTimes)
                    {
                        // record that we found yet another actual call matching the expected call
                        expectedCall->m_MatchedTimes++;
                        if (expectedCall->m_MatchedTimes == expectedCall->m_ExpectedTimes)
                        {
                            // mark the expected call as fully matched
                            // (no other actual call will be matched against it)
                            expectedCall->m_MatchedCall = actualCall;
                        }

                        // mark the actual call as having a match
                        actualCall->m_MatchedCall = expectedCall;
                        result = expectedCall;
                    }
                    else
                    {
                        // too many calls, check if exact is specified
                        if (expectedCall->m_ExactExpectedTimes)
                        {
                            actualCall->m_AlwaysReport = true;
                        }
                    }
                    break;
                }
            }
        }

        // have a second loop to see if we need to get a later set return value
        for (int i = (int)expectedCalls.size() - 1; i >= 0; i--)
        {
            if ((expectedCalls[(unsigned int)i]->m_OnlySpecifiesActions) &&
                (*expectedCalls[(unsigned int)i] == *actualCall))
            {
                result = expectedCalls[(unsigned int)i];
                break;
            }
        }

        return result;
    }
};

#endif // STRICTUNORDEREDCALLCOMPARER_H
