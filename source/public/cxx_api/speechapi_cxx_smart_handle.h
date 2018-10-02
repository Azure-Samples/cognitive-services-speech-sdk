//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
// speechapi_cxx_smart_handle.h: Public API declarations for SmartHandle class and related typedef
//

#pragma once
#include <speechapi_cxx_common.h>


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {


typedef SPXHR(SPXAPI_CALLTYPE *SmartHandleCloseFunction)(SPXHANDLE);

/// <summary>
/// Smart handle class.
/// </summary>
template <typename T, SmartHandleCloseFunction closeFunction>
class SmartHandle
{
public:

    SmartHandle(T handle) : m_handle(handle) { };
    ~SmartHandle() { reset(); }

    explicit operator T&() const { return m_handle; }

    T get() const { return m_handle; }
    operator T() const { return m_handle; }

    void reset()
    {
        if (m_handle != nullptr)
        {
            closeFunction(m_handle);
            m_handle = SPXHANDLE_INVALID;
        }
    }

private:

    DISABLE_COPY_AND_MOVE(SmartHandle);
    T m_handle;
};


} } } // Microsoft::CognitiveServices::Speech
