// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

/*defines*/
#include <cstdlib>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "stdafx.h"
#include "micromockexception.h"
#include "micromocktestrunnerhooks.h"
#define _MACROSTR(a) _T(#a)
/*types*/

/*static variables*/
/*static functions*/
/*variable exports*/

const TCHAR* MicroMockExceptionToString(_In_ MICROMOCK_EXCEPTION exceptionCode)
{
    switch (exceptionCode)
    {
    default:
        return _T("Invalid exception code");

    case MICROMOCK_EXCEPTION_INVALID_VALIDATE_BUFFERS:
        return _MACROSTR(MICROMOCK_EXCEPTION_INVALID_VALIDATE_BUFFERS);
    case MICROMOCK_EXCEPTION_ALLOCATION_FAILURE:
        return _MACROSTR(MICROMOCK_EXCEPTION_ALLOCATION_FAILURE);
    case MICROMOCK_EXCEPTION_INVALID_ARGUMENT:
        return _MACROSTR(MICROMOCK_EXCEPTION_INVALID_ARGUMENT);
    case MICROMOCK_EXCEPTION_INVALID_CALL_MODIFIER_COMBINATION:
        return _MACROSTR(MICROMOCK_EXCEPTION_INVALID_CALL_MODIFIER_COMBINATION);
    case MICROMOCK_EXCEPTION_MOCK_NOT_FOUND:
        return _MACROSTR(MICROMOCK_EXCEPTION_MOCK_NOT_FOUND);
    case MICROMOCK_EXCEPTION_SET_TIME_BEFORE_CALL:
        return _MACROSTR(MICROMOCK_EXCEPTION_SET_TIME_BEFORE_CALL);
    case MICROMOCK_EXCEPTION_SET_ARRAY_SIZE_BEFORE_CALL:
        return _MACROSTR(MICROMOCK_EXCEPTION_SET_ARRAY_SIZE_BEFORE_CALL);
    case MICROMOCK_EXCEPTION_INTERNAL_ERROR:
        return _MACROSTR(MICROMOCK_EXCEPTION_INTERNAL_ERROR);
    }
};

/*function exports*/

