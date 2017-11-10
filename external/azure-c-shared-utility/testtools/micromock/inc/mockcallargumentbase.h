// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef MOCKCALLARGUMENTBASE_H
#define MOCKCALLARGUMENTBASE_H

#pragma once

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "stdafx.h"
#include "mockvaluebase.h"


typedef struct BUFFER_ARGUMENT_DATA_TAG
{
    void*   m_Buffer;
    size_t  m_ByteCount;
    size_t  m_Offset;

    _Must_inspect_result_
    bool operator<(_In_ const BUFFER_ARGUMENT_DATA_TAG& rhs) const
    {
        if (m_Offset < rhs.m_Offset)
        {
            return true;
        }

        if (m_Offset > rhs.m_Offset)
        {
            return false;
        }

        if (m_ByteCount < rhs.m_ByteCount)
        {
            return true;
        }

        if (m_ByteCount > rhs.m_ByteCount)
        {
            return false;
        }

        return m_Buffer < rhs.m_Buffer;
    }
} BUFFER_ARGUMENT_DATA;

class CMockCallArgumentBase
{
public:
    virtual ~CMockCallArgumentBase() {};

    virtual void SetIgnored(_In_ bool ignored) = 0;
    virtual _Check_return_ std::tstring ToString() const = 0;
    virtual bool EqualTo(_In_ const CMockCallArgumentBase* right) = 0;
    virtual void AddCopyOutArgumentBuffer(_In_reads_bytes_(bytesToCopy) const void* injectedBuffer, _In_ size_t bytesToCopy, _In_ size_t byteOffset = 0) = 0;
    virtual void AddBufferValidation(_In_reads_bytes_(bytesToValidate) const void* expectedBuffer, _In_ size_t bytesToValidate, _In_ size_t byteOffset = 0) = 0;
    virtual void CopyOutArgumentDataFrom(_In_ const CMockCallArgumentBase* sourceMockCallArgument) = 0;
};

#endif // MOCKCALLARGUMENTBASE_H
