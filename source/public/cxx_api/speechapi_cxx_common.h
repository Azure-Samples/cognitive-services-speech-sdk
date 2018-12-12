//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
// speechapi_cxx_common.h: Public API declarations for global C++ APIs/namespaces
//

#pragma once

#include <functional>
#include <string>
#include <stdexcept>
#include <speechapi_c_common.h>
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
    auto error = error_get_error_code(handle);
    if (error == SPX_NOERROR)
    {
        throw hr;
    }

    auto callstack = error_get_call_stack(handle);
    auto what = error_get_message(handle);

    std::runtime_error er("");
    try
    {
        er = std::runtime_error(
            (what == nullptr ? "Exception with error code: " + std::to_string(error) : std::string(what)) +
            (callstack == nullptr ? "" : std::string(callstack)));
    }
    catch (...)
    {
        error_release(handle);
        throw hr;
    }

    error_release(handle);
    throw er;
}

#ifndef __SPX_THROW_HR_IMPL
#define __SPX_THROW_HR_IMPL(hr) __spx_rethrow(hr)
#endif

#if defined(__GNUG__) && defined(__linux__) && !defined(ANDROID) && !defined(__ANDROID__)
#include <cxxabi.h>
#define SHOULD_HANDLE_FORCED_UNWIND 1
#endif
