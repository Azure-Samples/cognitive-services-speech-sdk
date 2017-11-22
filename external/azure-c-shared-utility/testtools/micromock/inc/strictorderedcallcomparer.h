// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef STRICTORDEREDCALLCOMPARER_H
#define STRICTORDEREDCALLCOMPARER_H

#pragma once

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "stdafx.h"
#include "mockcallcomparer.h"
#include "mockmethodcallbase.h"

template<class T>
class CStrictOrderedCallComparer : public T
{
public:
    CStrictOrderedCallComparer(_In_ AUTOMATIC_CALL_COMPARISON performAutomaticCallComparison = AUTOMATIC_CALL_COMPARISON_ON) :
        m_ExpectedCallIndex(0)
    {
        T::SetPerformAutomaticCallComparison(performAutomaticCallComparison);
    }

    _Must_inspect_result_
    virtual bool IsUnexpectedCall(_In_ const CMockMethodCallBase* actualCall)
    {
        return !actualCall->HasMatch();
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

        // skip any records that do not enforce expected calls
        while ((m_ExpectedCallIndex < expectedCalls.size()) &&
               (expectedCalls[m_ExpectedCallIndex]->m_OnlySpecifiesActions))
        {
            m_ExpectedCallIndex++;
        }

        // do we still have items in the expected calls array?
        if (m_ExpectedCallIndex < expectedCalls.size())
        {
            CMockMethodCallBase* expectedCall = expectedCalls[m_ExpectedCallIndex];

            // if the expected call says all calls of this type should be ignored, 
            // then match
            if ((expectedCall->m_IgnoreAllCalls) &&
                (*expectedCall == *actualCall))
            {
                actualCall->m_MatchedCall = expectedCall;
                expectedCall->m_MatchedCall = actualCall;
            }
            else
            {
                // if the expected call has not yet been matched
                if (!expectedCall->HasMatch() &&
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
                }
            }
        }

        if ((NULL != result) &&
            (result->m_MatchedTimes == result->m_ExpectedTimes))
        {
            m_ExpectedCallIndex++;
        }
        else
        {
            // have a second loop to see if we need to get a later set return value
            for (int i = (int)expectedCalls.size() - 1; i >= 0; i--)
            {
                if ((expectedCalls[i]->m_OnlySpecifiesActions) &&
                    (*expectedCalls[i] == *actualCall))
                {
                    result = expectedCalls[i];
                    break;
                }
            }
        }

        return result;
    }

protected:
    size_t m_ExpectedCallIndex;
};

#endif // STRICTORDEREDCALLCOMPARER_H
