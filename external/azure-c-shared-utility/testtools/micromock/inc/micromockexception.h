// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef MICROMOCKEXCEPTION_H
#define MICROMOCKEXCEPTION_H

#pragma once

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "stdafx.h"
#include "azure_c_shared_utility/macro_utils.h"

#define MICROMOCK_EXCEPTION_VALUES                        \
MICROMOCK_EXCEPTION_INVALID_VALIDATE_BUFFERS,             \
MICROMOCK_EXCEPTION_ALLOCATION_FAILURE,                   \
MICROMOCK_EXCEPTION_INVALID_ARGUMENT,                     \
MICROMOCK_EXCEPTION_INVALID_CALL_MODIFIER_COMBINATION,    \
MICROMOCK_EXCEPTION_MOCK_NOT_FOUND,                       \
MICROMOCK_EXCEPTION_SET_TIME_BEFORE_CALL,                 \
MICROMOCK_EXCEPTION_SET_ARRAY_SIZE_BEFORE_CALL,           \
MICROMOCK_EXCEPTION_INTERNAL_ERROR                        \


DEFINE_ENUM(MICROMOCK_EXCEPTION, MICROMOCK_EXCEPTION_VALUES)

extern const TCHAR* MicroMockExceptionToString(_In_ MICROMOCK_EXCEPTION exceptionCode);

class CMicroMockException
{
public:
    CMicroMockException(_In_ MICROMOCK_EXCEPTION exceptionCode, _In_ std::tstring exceptionText) :
        m_ExceptionCode(exceptionCode),
        m_ExceptionString(exceptionText)
    {
    }
    ~CMicroMockException()
    {
    }

    MICROMOCK_EXCEPTION GetMicroMockExceptionCode() const { return m_ExceptionCode; }
    std::tstring GetExceptionString() const { return m_ExceptionString; }

protected:
    MICROMOCK_EXCEPTION m_ExceptionCode;
    std::tstring m_ExceptionString;
};

#ifdef MOCK_TOSTRING
MOCK_TOSTRING<MICROMOCK_EXCEPTION>(const MICROMOCK_EXCEPTION& t) { return  MicroMockExceptionToString(t);}
#endif

#endif // MICROMOCKEXCEPTION_H
