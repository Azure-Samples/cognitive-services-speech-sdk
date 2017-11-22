// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef MOCKMETHODCALLBASE_H
#define MOCKMETHODCALLBASE_H

#pragma once

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "stdafx.h"
#include "mockvaluebase.h"
#include "mockcallargumentbase.h"

class CMockMethodCallBase
{
public:
    CMockMethodCallBase();
    virtual ~CMockMethodCallBase();
    CMockMethodCallBase(std::tstring methodName, size_t argCount = 0,
        CMockCallArgumentBase** arguments = NULL);

    std::tstring ToString();
    std::tstring GetArgumentsString();
    bool operator==(const CMockMethodCallBase& right);
    void CopyOutArgumentBuffers(CMockMethodCallBase* sourceMockMethodCall);
    void AddExtraCallArgument(CMockCallArgumentBase* callArgument);

    CMockMethodCallBase* m_MatchedCall;
    bool m_OnlySpecifiesActions;
    bool m_IgnoreAllCalls;
    bool m_AlwaysReport;
    size_t m_ExpectedTimes;
    size_t m_MatchedTimes;
    bool m_ExactExpectedTimes;
    virtual CMockValueBase* GetReturnValue() { return m_ReturnValue; }
    virtual CMockValueBase* GetFailReturnValue() 
    { 
        return m_FailReturnValue; 
    }
    _Must_inspect_result_ bool HasMatch() const { return (NULL != m_MatchedCall); }
    void RollbackMatch();

protected:
    void Init(std::tstring methodName);

    std::vector<CMockCallArgumentBase*> m_MockCallArguments;
    std::tstring m_MethodName;
    CMockValueBase* m_ReturnValue;
    CMockValueBase* m_FailReturnValue;
};

#endif // MOCKMETHODCALLBASE_H
