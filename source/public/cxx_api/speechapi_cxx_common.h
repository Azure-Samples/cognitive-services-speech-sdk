//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_cxx_common.h: Public API declarations for global C++ APIs/namespaces
//

#pragma once

#include <string>
#include <stdexcept>
#include <speechapi_c_error.h>

#ifdef SWIG
#define final
#endif

#define DISABLE_COPY_AND_MOVE(T)     \
    /** \brief Disable copy constructor */ \
    T(const T&) = delete;            \
    /** \brief Disable copy assignment */ \
    T& operator=(const T&) = delete; \
    /** \brief Disable move constructor */ \
    T(T&&) = delete;                 \
    /** \brief Disable move assignment */ \
    T& operator=(T&&) = delete

#define DISABLE_DEFAULT_CTORS(T)     \
    /** \brief Disable default constructor */ \
    T() = delete;                    \
    DISABLE_COPY_AND_MOVE(T)

inline void __spx_rethrow(SPXHR hr)
{
    auto handle = reinterpret_cast<SPXERRORHANDLE>(hr);
    auto error = Error_GetCode(handle);
    if (error == SPX_NOERROR) 
    {
        throw hr;
    }
    auto callstack = Error_GetCallStack(handle);
    auto what = Error_GetMessage(handle);
    throw std::runtime_error(
        (what == nullptr ? "Exception with error code: " + std::to_string(error) : std::string(what)) +
        (callstack == nullptr ? "" : std::string(callstack)));
}

#ifndef __SPX_THROW_HR_IMPL
#define __SPX_THROW_HR_IMPL(hr) __spx_rethrow(hr)
#endif
