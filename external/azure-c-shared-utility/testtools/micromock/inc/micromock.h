// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef MICROMOCK_H
#define MICROMOCK_H

#pragma once

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "mock.h"
#include "globalmock.h"
#include "threadsafeglobalmock.h"
#include "mockmethodcallbase.h"
#include "mockmethodcall.h"
#include "mockcallrecorder.h"
#include "mockcallargument.h"
#include "mockvalue.h"
#include "mockvaluebase.h"
#include "mockresultvalue.h"
#include "strictunorderedcallcomparer.h"
#include "strictorderedcallcomparer.h"
#include "nicecallcomparer.h"
#include "micromockexception.h"
#include "micromocktestmutex.h"
#include "micromockenumtostring.h"

// Defines for ignored arguments
#define IGNORED_PTR_ARG (NULL)
#define IGNORED_NUM_ARG (17)
#ifndef _MSC_VER
#define IGNORE (0)
#endif

#define TYPED_MOCK_CLASS(className, mockType)       \
class className : public CStrictUnorderedCallComparer< mockType<className> >

#define MICROMOCK_POINTER_TYPE_EQUAL(T)                             \
template<>                                                          \
static bool operator==<T*>(_In_ const CMockValue<T*>& lhs,          \
    _In_ const CMockValue<T*>& rhs)                                 \
{                                                                   \
    bool result = false;                                            \
    if (lhs.GetValue() == rhs.GetValue())                           \
    {                                                               \
        result = true;                                              \
    } else                                                          \
    if ((NULL != lhs.GetValue()) &&                                 \
        (NULL != rhs.GetValue()) &&                                 \
        (*lhs.GetValue() == *rhs.GetValue()))                       \
    {                                                               \
        result = true;                                              \
    }                                                               \
    return result;                                                  \
}

// A strict expected call implies that all arguments are checked
#define STRICT_EXPECTED_CALL(ClassName,  ...)        \
    C2(ClassName.Expected_, __VA_ARGS__)

// By using the below macro, none of the arguments are checked by default
// To specify checking the argument values, use the ValidateArgument 
// expected call modifier method
#define EXPECTED_CALL(ClassName, ...)               \
STRICT_EXPECTED_CALL(ClassName, __VA_ARGS__)        \
    .IgnoreAllArguments()

// When called only specifies actions for the specific call instance
// he actions specified this way are not included in the comparison
// of expected and actual calls
#define WHEN_CALLED(ClassName, ...)          \
EXPECTED_CALL(ClassName, __VA_ARGS__)        \
    .OnlySpecifiesActions()

// This is the strict version of WHEN_CALLED
#define WHEN_CALLED_STRICT(ClassName, ...)   \
STRICT_EXPECTED_CALL(ClassName, __VA_ARGS__) \
    .OnlySpecifiesActions()

#define RECORD_ACTUAL_MOCK_CALL(mockMethodCall, fail)         \
    RecordActualCall(mockMethodCall, fail)

#define RECORD_EXPECTED_MOCK_CALL(mockMethodCall)       \
    RecordExpectedCall(mockMethodCall)

#define REMATCH_ACTUAL_MOCK_CALL(mockMethodCall)        \
    MatchActualCall(mockMethodCall)

#define RECORD_ACTUAL_STATIC_MOCK_CALL(mockMethodCall, fail)  \
    GetSingleton()->RecordActualCall(mockMethodCall, fail)

#define RECORD_EXPECTED_STATIC_MOCK_CALL(mockMethodCall)\
    GetSingleton()->RecordExpectedCall(mockMethodCall)

#define REMATCH_ACTUAL_STATIC_MOCK_CALL(mockMethodCall) \
    GetSingleton()->MatchActualCall(mockMethodCall)

#define STATIC_MOCK_LOCK(name)                          \
void MockLock_##name()                                  \
{                                                       \
    GetSingleton()->Lock();                             \
}

#define STATIC_MOCK_UNLOCK()                            \
    GetSingleton()->Unlock()

#define MOCK_METHOD_END(resultType, value)                                                  \
    return (NULL != result) ? dynamic_cast<CMockValue<resultType>*>(result)->GetValue() : (value); \
}

#define MOCK_VOID_METHOD_END()                                                              \
    result = result;                                                                        \
}

#include "micromockcallmacros.h"

#endif // MICROMOCK_H
