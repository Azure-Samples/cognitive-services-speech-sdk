// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef MOCKCALLARGUMENT_H
#define MOCKCALLARGUMENT_H

#pragma once

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "stdafx.h"
#include "mockvalue.h"
#include "mockcallargumentbase.h"
#include "micromockexception.h"
#include "stddef.h"

template<typename T>
class CMockCallArgument :
    public CMockValue<T>, public CMockCallArgumentBase
{
public:
    CMockCallArgument(_In_ T argValue, _In_ bool IsIgnoreable = true) :
        CMockValue<T>(argValue),
        m_Ignored(false)
    {
        m_IsIgnoreable = IsIgnoreable;
    }

    virtual ~CMockCallArgument()
    {
        for (size_t i = 0; i < m_BufferValidations.size(); i++)
        {
            void* buffer = m_BufferValidations[i].m_Buffer;
            if (buffer != NULL)
            {
                free(buffer);
            }
        }

        for (size_t i = 0; i < m_CopyOutArgumentBuffers.size(); i++)
        {
            void* buffer = m_CopyOutArgumentBuffers[i].m_Buffer;
            if (buffer != NULL)
            {
                free(buffer);
            }
        }
    }

    // TODO: this should be fixed to be const
    _Must_inspect_result_
        bool operator==(_In_ CMockCallArgument<T>& rhs)
    {
        bool result = true;

        if (m_BufferValidations.size() > 0 &&
            rhs.m_BufferValidations.size() == 0)
        {
            return rhs == *this;
        }

        if (rhs.m_BufferValidations.size() > 0)
        {
            UINT8* argBufferValidationMask = rhs.CreateBufferValidationMask();

            if (NULL != argBufferValidationMask)
            {
                FormatArgumentBufferString(rhs.m_BufferValidations, *((UINT8**)&this->CMockValue<T>::m_Value));
                rhs.FormatArgumentBufferString(rhs.m_BufferValidations, argBufferValidationMask);
                free(argBufferValidationMask);
            }
            else
            {
                MOCK_THROW(CMicroMockException(MICROMOCK_EXCEPTION_ALLOCATION_FAILURE,
                    _T("Failed allocating buffer validation mask")));
            }

            result = result && ValidateArgumentBuffer(rhs.m_BufferValidations);
        }

        if (result &&
            !m_Ignored &&
            !rhs.m_Ignored)
        {
            // simply invoke the CMockValue == operator to take advantage of
            // any specializations defined in tests
            result = *((CMockValue<T>*)this) == (*(CMockValue<T>*)&rhs);
        }

        return result;
    }

    _Must_inspect_result_
        bool ValidateArgumentBuffer(_In_ const std::vector<BUFFER_ARGUMENT_DATA>& bufferValidations) const
    {
        bool result = true;

        for (size_t i = 0; i < bufferValidations.size(); i++)
        {
            UINT8* argBuffer = *((UINT8**)&this->CMockValue<T>::m_Value);
            if (NULL == argBuffer)
            {
                result = false;
                break;
            }

            result &= (memcmp(argBuffer + bufferValidations[i].m_Offset, bufferValidations[i].m_Buffer,
                bufferValidations[i].m_ByteCount) == 0);
        }

        return result;
    }

    virtual _Check_return_ std::tstring ToString() const
    {
        if (m_ArgumentAsString.length() > 0)
        {
            return m_ArgumentAsString;
        }
        else
        {
            return this->CMockValue<T>::ToString();
        }
    }

    virtual void CopyOutArgumentDataFrom(_In_ const CMockCallArgumentBase* sourceMockCallArgument)
    {
        const CMockCallArgument<T>* argument = (const CMockCallArgument<T>*)(sourceMockCallArgument);
        if (NULL != argument)
        {
            for (size_t i = 0; i < argument->m_CopyOutArgumentBuffers.size(); i++)
            {
                UINT8* argBuffer = *((UINT8**)&this->m_OriginalValue);
                if (NULL != argBuffer)
                {
                    memmove(argBuffer + argument->m_CopyOutArgumentBuffers[i].m_Offset,
                        argument->m_CopyOutArgumentBuffers[i].m_Buffer,
                        argument->m_CopyOutArgumentBuffers[i].m_ByteCount);
                }
            }
        }
    }

    _Must_inspect_result_
        virtual bool EqualTo(_In_ const CMockCallArgumentBase* right)
    {
        return (*this == *(CMockCallArgument<T>*)(const CMockCallArgument<T>*)(right));
    }

    void AddBufferValidation(_In_reads_bytes_(bytesToValidate) const void* expectedBuffer, _In_ size_t bytesToValidate, _In_ size_t byteOffset = 0)
    {
        if ((NULL != expectedBuffer) &&
            (bytesToValidate > 0))
        {
            for (size_t i = byteOffset; i < byteOffset + bytesToValidate; i++)
            {
                UINT8 argBufferValidateByte = 0;
                if ((GetArgBufferValidationExpectedByte(i, &argBufferValidateByte) == true) &&
                    (argBufferValidateByte != ((UINT8*)expectedBuffer)[i - byteOffset]))
                {
                    MOCK_THROW(CMicroMockException(MICROMOCK_EXCEPTION_INVALID_VALIDATE_BUFFERS,
                        _T("Validate argument buffer specified 2 times for the same byte")));
                }
            }

            BUFFER_ARGUMENT_DATA argumentValidationData;

            argumentValidationData.m_Buffer = malloc(bytesToValidate);
            if (NULL != argumentValidationData.m_Buffer)
            {
                memcpy(argumentValidationData.m_Buffer, expectedBuffer, bytesToValidate);
            }

            argumentValidationData.m_ByteCount = bytesToValidate;
            argumentValidationData.m_Offset = byteOffset;

            m_BufferValidations.push_back(argumentValidationData);

            // ignore the argument
            SetIgnored(true);
        }
        else
        {
            MOCK_THROW(CMicroMockException(MICROMOCK_EXCEPTION_INVALID_VALIDATE_BUFFERS,
                _T("Invalid arguments for AddBufferValidation.")));
        }
    }

    virtual void AddCopyOutArgumentBuffer(_In_reads_bytes_(bytesToCopy) const void* injectedBuffer, _In_ size_t bytesToCopy, _In_ size_t byteOffset = 0)
    {
        BUFFER_ARGUMENT_DATA outArgumentCopyData;

        outArgumentCopyData.m_Buffer = malloc(bytesToCopy);
        if (NULL != outArgumentCopyData.m_Buffer)
        {
            memcpy(outArgumentCopyData.m_Buffer, injectedBuffer, bytesToCopy);
        }
        outArgumentCopyData.m_ByteCount = bytesToCopy;
        outArgumentCopyData.m_Offset = byteOffset;

        m_CopyOutArgumentBuffers.push_back(outArgumentCopyData);

        // ignore the argument
        SetIgnored(true);
    }

    virtual void SetIgnored(_In_ bool ignored)
    {
        if (m_IsIgnoreable == true)
        {
            m_Ignored = ignored;
        }
    }

private:
    void FormatArgumentBufferString(_In_ std::vector<BUFFER_ARGUMENT_DATA>& bufferValidations, _In_ const UINT8* buffer)
    {
        size_t pos = 0;
        std::tostringstream strStream;

        strStream << std::hex << std::uppercase;

        if (NULL != buffer)
        {
            sort(bufferValidations.begin(), bufferValidations.end());

            strStream << _T("[");

            for (size_t i = 0; i < bufferValidations.size(); i++)
            {
                // fill with ".."
                while (pos < bufferValidations[i].m_Offset)
                {
                    if (pos > 0)
                    {
                        strStream << _T(" ");
                    }

                    strStream << _T("..");
                    pos++;
                }

                // put the actual bytes in
                while (pos < bufferValidations[i].m_ByteCount + bufferValidations[i].m_Offset)
                {
                    if (pos > 0)
                    {
                        strStream << _T(" ");
                    }

                    strStream << std::setfill(_T('0')) << std::setw(2) << (unsigned int)buffer[pos];
                    pos++;
                }
            }

            strStream << _T("]");

            m_ArgumentAsString = strStream.str();
        }
        else
        {
            MOCK_THROW(CMicroMockException(MICROMOCK_EXCEPTION_INVALID_ARGUMENT,
                _T("NULL buffer argument attempted to be used for argument validation.")));
        }
    }

    _Must_inspect_result_
        _Success_(return == true)
        bool GetArgBufferValidationExpectedByte(_In_ size_t pos, _Out_writes_(1) UINT8* buffer) const
    {
        bool result = false;

        for (size_t i = 0; i < m_BufferValidations.size(); i++)
        {
            if ((m_BufferValidations[i].m_Offset <= pos) &&
                (pos < m_BufferValidations[i].m_Offset + m_BufferValidations[i].m_ByteCount))
            {
                *buffer = ((UINT8*)m_BufferValidations[i].m_Buffer)[pos - m_BufferValidations[i].m_Offset];
                result = true;
            }
        }

        return result;
    }

    _Must_inspect_result_
        _Success_(return != NULL)
        UINT8* CreateBufferValidationMask()
    {
        UINT8* result = NULL;

        if (m_BufferValidations.size() > 0)
        {
            size_t bufferSize;

            sort(m_BufferValidations.begin(), m_BufferValidations.end());
            bufferSize = m_BufferValidations[m_BufferValidations.size() - 1].m_Offset + m_BufferValidations[m_BufferValidations.size() - 1].m_ByteCount;

            result = (UINT8*)malloc(bufferSize);
            if (NULL != result)
            {
                for (size_t i = 0; i < m_BufferValidations.size(); i++)
                {
                    memcpy(result + m_BufferValidations[i].m_Offset,
                        m_BufferValidations[i].m_Buffer, m_BufferValidations[i].m_ByteCount);
                }
            }
        }

        return result;
    }

    std::vector<BUFFER_ARGUMENT_DATA> m_BufferValidations;
    std::vector<BUFFER_ARGUMENT_DATA> m_CopyOutArgumentBuffers;
    std::tstring m_ArgumentAsString;
    bool m_Ignored;
    bool m_IsIgnoreable; /*used for "time is never ignoreable"*/
};

#endif // MOCKCALLARGUMENT_H
