//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
// spxdebug.h: Public API definitions for global C Trace/Debug methods and related #defines
//

#pragma once
#include <inttypes.h>
#include <spxerror.h>
#include <algorithm>

#ifndef _MSC_VER
// macros in this header generate a bunch of
// "ISO C++11 requires at least one argument for the "..." in a variadic macro" errors.
// system_header pragma is the only mechanism that helps to suppress them.
// https://stackoverflow.com/questions/35587137/how-to-suppress-gcc-variadic-macro-argument-warning-for-zero-arguments-for-a-par
// TODO: try to make macros standard-compliant.
#pragma GCC system_header
#endif

#ifndef __cplusplus
#define static_assert _Static_assert
#endif

#define UNUSED(x) (void)(x)

//-------------------------------------------------------
//  Re-enabled ability to compile out all macros...
//  However, currently still need to keep all macros until
//  final review of all macros is complete.
//-------------------------------------------------------

#define SPX_CONFIG_TRACE_INCLUDE_DBG_WITH_ALL       1

#ifdef SPX_CONFIG_TRACE_INCLUDE_DBG_WITH_ALL
#if defined(SPX_CONFIG_TRACE_ALL) && !defined(SPX_CONFIG_DBG_TRACE_ALL) && (!defined(DEBUG) || !defined(_DEBUG))
#define SPX_CONFIG_DBG_TRACE_ALL                    1
#endif
#endif

//-------------------------------------------------------
//  SPX_ and SPX_DBG_ macro configuration
//-------------------------------------------------------

#ifdef SPX_CONFIG_DBG_TRACE_ALL
#define SPX_CONFIG_DBG_TRACE_VERBOSE                1
#define SPX_CONFIG_DBG_TRACE_INFO                   1
#define SPX_CONFIG_DBG_TRACE_WARNING                1
#define SPX_CONFIG_DBG_TRACE_ERROR                  1
#define SPX_CONFIG_DBG_TRACE_FUNCTION               1
#define SPX_CONFIG_DBG_TRACE_SCOPE                  1
#define SPX_CONFIG_DBG_TRACE_ASSERT                 1
#define SPX_CONFIG_DBG_TRACE_VERIFY                 1
#ifndef SPX_CONFIG_TRACE_ALL
#define SPX_CONFIG_TRACE_ALL                        1
#endif
#endif

#ifdef SPX_CONFIG_TRACE_ALL
#define SPX_CONFIG_TRACE_VERBOSE                    1
#define SPX_CONFIG_TRACE_INFO                       1
#define SPX_CONFIG_TRACE_WARNING                    1
#define SPX_CONFIG_TRACE_ERROR                      1
#define SPX_CONFIG_TRACE_FUNCTION                   1
#define SPX_CONFIG_TRACE_SCOPE                      1
#define SPX_CONFIG_TRACE_THROW_ON_FAIL              1
#define SPX_CONFIG_TRACE_REPORT_ON_FAIL             1
#define SPX_CONFIG_TRACE_RETURN_ON_FAIL             1
#define SPX_CONFIG_TRACE_EXITFN_ON_FAIL             1
#endif

//-----------------------------------------------------------
//  SPX_TRACE macro common implementations
//-----------------------------------------------------------

#define __SPX_TRACE_LEVEL_INFO        0x08 // Trace_Info
#define __SPX_TRACE_LEVEL_WARNING     0x04 // Trace_Warning
#define __SPX_TRACE_LEVEL_ERROR       0x02 // Trace_Error
#define __SPX_TRACE_LEVEL_VERBOSE     0x10 // Trace_Verbose

#ifndef __SPX_DO_TRACE_IMPL
#ifdef __cplusplus
#include <stdio.h>
#include <stdarg.h>
#include <string>
inline void __spx_do_trace_message(int level, const char* pszTitle, const char* fileName, const int lineNumber, const char* pszFormat, ...) throw()
{
    UNUSED(level);

    bool logToConsole = false;
#if defined(DEBUG) || defined(_DEBUG)
    logToConsole = true;
#endif

    if (!logToConsole)
    {
        return;
    }

    try
    {
        va_list argptr;
        va_start(argptr, pszFormat);

        std::string format;
        while (*pszFormat == '\n' || *pszFormat == '\r')
        {
            if (*pszFormat == '\r')
            {
                pszTitle = nullptr;
            }

            format += *pszFormat++;
        }

        if (pszTitle != nullptr)
        {
            format += pszTitle;
        }

        std::string fileNameOnly(fileName);
        std::replace(fileNameOnly.begin(), fileNameOnly.end(), '\\', '/');

        std::string fileNameLineNumber = " " + fileNameOnly.substr(fileNameOnly.find_last_of('/', std::string::npos) + 1) + ":" + std::to_string(lineNumber) + " ";

        format += fileNameLineNumber;

        format += pszFormat;

        if (format.length() < 1 || format[format.length() - 1] != '\n')
        {
            format += "\n";
        }

        vfprintf(stderr, format.c_str(), argptr);

        va_end(argptr);
    }
    catch(...)
    {
    }
}
#define __SPX_DO_TRACE_IMPL __spx_do_trace_message
#else
#define __SPX_DO_TRACE_IMPL
#endif
#endif

#define __SPX_DOTRACE(level, title, fileName, lineNumber, ...)                      \
    do {                                                                            \
        __SPX_DO_TRACE_IMPL(level, title, fileName, lineNumber, ##__VA_ARGS__);     \
    } while (0)

#define __SPX_TRACE_INFO(title, fileName, lineNumber, msg, ...) __SPX_DOTRACE(__SPX_TRACE_LEVEL_INFO, title, fileName, lineNumber, msg, ##__VA_ARGS__)
#define __SPX_TRACE_INFO_IF(cond, title, fileName, lineNumber, msg, ...)            \
    do {                                                                            \
        int fCond = !!(cond);                                                       \
        if (fCond) {                                                                \
            __SPX_TRACE_INFO(title, fileName, lineNumber, msg, ##__VA_ARGS__);      \
    } } while (0)

#define __SPX_TRACE_WARNING(title, fileName, lineNumber, msg, ...) __SPX_DOTRACE(__SPX_TRACE_LEVEL_WARNING, title, fileName, lineNumber, msg, ##__VA_ARGS__)
#define __SPX_TRACE_WARNING_IF(cond, title, fileName, lineNumber, msg, ...)         \
    do {                                                                            \
        int fCond = !!(cond);                                                       \
        if (fCond) {                                                                \
            __SPX_TRACE_WARNING(title, fileName, lineNumber, msg, ##__VA_ARGS__);   \
    } } while (0)

#define __SPX_TRACE_ERROR(title, fileName, lineNumber, msg, ...) __SPX_DOTRACE(__SPX_TRACE_LEVEL_ERROR, title, fileName, lineNumber, msg, ##__VA_ARGS__)
#define __SPX_TRACE_ERROR_IF(cond, title, fileName, lineNumber, msg, ...)           \
    do {                                                                            \
        int fCond = !!(cond);                                                       \
        if (fCond) {                                                                \
            __SPX_TRACE_ERROR(title, fileName, lineNumber, msg, ##__VA_ARGS__);     \
    } } while (0)

#define __SPX_TRACE_VERBOSE(title, fileName, lineNumber, msg, ...) __SPX_DOTRACE(__SPX_TRACE_LEVEL_VERBOSE, title, fileName, lineNumber, msg, ##__VA_ARGS__)
#define __SPX_TRACE_VERBOSE_IF(cond, title, fileName, lineNumber, msg, ...)         \
    do {                                                                            \
        int fCond = !!(cond);                                                       \
        if (fCond) {                                                                \
            __SPX_TRACE_VERBOSE(title, fileName, lineNumber, msg, ##__VA_ARGS__);   \
    } } while (0)

#define ___SPX_EXPR_AS_STRING(_String) "" #_String
#define __SPX_EXPR_AS_STRING(_String) ___SPX_EXPR_AS_STRING(_String)

#define __SPX_TRACE_HR(title, fileName, lineNumber, hr, x)             __SPX_TRACE_ERROR(title, fileName, lineNumber, __SPX_EXPR_AS_STRING(hr) " = 0x%0" PRIxPTR, x)
#define __SPX_TRACE_ASSERT(title, fileName, lineNumber, expr)          __SPX_TRACE_ERROR(title, fileName, lineNumber, __SPX_EXPR_AS_STRING(expr) " = false")
#define __SPX_TRACE_ASSERT_MSG(title, fileName, lineNumber, expr, ...) __SPX_TRACE_ERROR(title, fileName, lineNumber, __SPX_EXPR_AS_STRING(expr) " = false; " __VA_ARGS__)


#ifdef __cplusplus
#include <memory>
#define __SPX_TRACE_SCOPE(t1, fileName, lineNumber, t2, x, y)                       \
    __SPX_TRACE_INFO(t1, fileName, lineNumber, "%s", x);                            \
    auto evaluateYInScopeInMacros = y;                                              \
    auto leavingScopePrinterInMacros = [&evaluateYInScopeInMacros](int*) -> void {  \
        __SPX_TRACE_INFO(t2, fileName, lineNumber, "%s", evaluateYInScopeInMacros); \
    }; \
    std::unique_ptr<int, decltype(leavingScopePrinterInMacros)> onExit((int*)1, leavingScopePrinterInMacros)
#endif /* __cplusplus */


//-------------------------------------------------------
//  SPX_ macro definitions
//-------------------------------------------------------

#ifdef __cplusplus
#define SPX_STATIC_ASSERT_IS_BASE_OF(x, y)  static_assert(std::is_base_of<x, y>::value, "std::is_base_of<" # x ", " # y ">::value")
#else
#define SPX_STATIC_ASSERT_IS_BASE_OF(x, y)
#endif

#ifdef SPX_CONFIG_TRACE_VERBOSE
#define SPX_TRACE_VERBOSE(msg, ...)          __SPX_TRACE_VERBOSE("SPX_TRACE_VERBOSE: ", __FILE__, __LINE__, msg, ##__VA_ARGS__)
#define SPX_TRACE_VERBOSE_IF(cond, msg, ...) __SPX_TRACE_VERBOSE_IF(cond, "SPX_TRACE_VERBOSE: ", __FILE__, __LINE__, msg, ##__VA_ARGS__)
#else
#define SPX_TRACE_VERBOSE(...)
#define SPX_TRACE_VERBOSE_IF(...)
#endif

#ifdef SPX_CONFIG_DBG_TRACE_VERBOSE
#define SPX_DBG_TRACE_VERBOSE(msg, ...)          __SPX_TRACE_VERBOSE("SPX_DBG_TRACE_VERBOSE: ", __FILE__, __LINE__, msg, ##__VA_ARGS__)
#define SPX_DBG_TRACE_VERBOSE_IF(cond, msg, ...) __SPX_TRACE_VERBOSE_IF(cond, "SPX_DBG_TRACE_VERBOSE: ", __FILE__, __LINE__, msg, ##__VA_ARGS__)
#else
#define SPX_DBG_TRACE_VERBOSE(...)
#define SPX_DBG_TRACE_VERBOSE_IF(...)
#endif

#ifdef SPX_CONFIG_TRACE_INFO
#define SPX_TRACE_INFO(msg, ...)             __SPX_TRACE_INFO("SPX_TRACE_INFO: ", __FILE__, __LINE__, msg, ##__VA_ARGS__)
#define SPX_TRACE_INFO_IF(cond, msg, ...)    __SPX_TRACE_INFO_IF(cond, "SPX_TRACE_INFO: ", __FILE__, __LINE__, msg, ##__VA_ARGS__)
#else
#define SPX_TRACE_INFO(...)
#define SPX_TRACE_INFO_IF(...)
#endif

#ifdef SPX_CONFIG_DBG_TRACE_INFO
#define SPX_DBG_TRACE_INFO(msg, ...)             __SPX_TRACE_INFO("SPX_DBG_TRACE_INFO: ", __FILE__, __LINE__, msg, ##__VA_ARGS__)
#define SPX_DBG_TRACE_INFO_IF(cond, msg, ...)    __SPX_TRACE_INFO_IF(cond, "SPX_DBG_TRACE_INFO: ", __FILE__, __LINE__, msg, ##__VA_ARGS__)
#else
#define SPX_DBG_TRACE_INFO(...)
#define SPX_DBG_TRACE_INFO_IF(...)
#endif

#ifdef SPX_CONFIG_TRACE_WARNING
#define SPX_TRACE_WARNING(msg, ...)          __SPX_TRACE_WARNING("SPX_TRACE_WARNING:", __FILE__, __LINE__, msg, ##__VA_ARGS__)
#define SPX_TRACE_WARNING_IF(cond, msg, ...) __SPX_TRACE_WARNING_IF(cond, "SPX_TRACE_WARNING:", __FILE__, __LINE__, msg, ##__VA_ARGS__)
#else
#define SPX_TRACE_WARNING(...)
#define SPX_TRACE_WARNING_IF(...)
#endif

#ifdef SPX_CONFIG_DBG_TRACE_WARNING
#define SPX_DBG_TRACE_WARNING(msg, ...)          __SPX_TRACE_WARNING("SPX_DBG_TRACE_WARNING:", __FILE__, __LINE__, msg, ##__VA_ARGS__)
#define SPX_DBG_TRACE_WARNING_IF(cond, msg, ...) __SPX_TRACE_WARNING_IF(cond, "SPX_DBG_TRACE_WARNING:", __FILE__, __LINE__, msg, ##__VA_ARGS__)
#else
#define SPX_DBG_TRACE_WARNING(...)
#define SPX_DBG_TRACE_WARNING_IF(...)
#endif

#ifdef SPX_CONFIG_TRACE_ERROR
#define SPX_TRACE_ERROR(msg, ...)            __SPX_TRACE_ERROR("SPX_TRACE_ERROR: ", __FILE__, __LINE__, msg, ##__VA_ARGS__)
#define SPX_TRACE_ERROR_IF(cond, msg, ...)   __SPX_TRACE_ERROR_IF(cond, "SPX_TRACE_ERROR: ", __FILE__, __LINE__, msg, ##__VA_ARGS__)
#else
#define SPX_TRACE_ERROR(...)
#define SPX_TRACE_ERROR_IF(...)
#endif

#ifdef SPX_CONFIG_DBG_TRACE_ERROR
#define SPX_DBG_TRACE_ERROR(msg, ...)            __SPX_TRACE_ERROR("SPX_DBG_TRACE_ERROR: ", __FILE__, __LINE__, msg, ##__VA_ARGS__)
#define SPX_DBG_TRACE_ERROR_IF(cond, msg, ...)   __SPX_TRACE_ERROR_IF(cond, "SPX_DBG_TRACE_ERROR: ", __FILE__, __LINE__, msg, ##__VA_ARGS__)
#else
#define SPX_DBG_TRACE_ERROR(...)
#define SPX_DBG_TRACE_ERROR_IF(...)
#endif

#ifdef SPX_CONFIG_TRACE_FUNCTION
#define SPX_TRACE_FUNCTION(...) __SPX_TRACE_VERBOSE("SPX_TRACE_FUNCTION: ", __FILE__, __LINE__, __FUNCTION__)
#else
#define SPX_TRACE_FUNCTION(...)
#endif

#ifdef SPX_CONFIG_DBG_TRACE_FUNCTION
#define SPX_DBG_TRACE_FUNCTION(...) __SPX_TRACE_VERBOSE("SPX_DBG_TRACE_FUNCTION: ", __FILE__, __LINE__, __FUNCTION__)
#else
#define SPX_DBG_TRACE_FUNCTION(...)
#endif

#ifdef SPX_CONFIG_TRACE_SCOPE
#define SPX_TRACE_SCOPE(x, y) __SPX_TRACE_SCOPE("SPX_TRACE_SCOPE_ENTER: ", __FILE__, __LINE__, "SPX_TRACE_SCOPE_EXIT: ", x, y)
#else
#define SPX_TRACE_SCOPE(x, y)
#endif

#ifdef SPX_CONFIG_DBG_TRACE_SCOPE
#define SPX_DBG_TRACE_SCOPE(x, y) __SPX_TRACE_SCOPE("SPX_DBG_TRACE_SCOPE_ENTER: ", __FILE__, __LINE__, "SPX_DBG_TRACE_SCOPE_EXIT: ", x, y)
#else
#define SPX_DBG_TRACE_SCOPE(x, y)
#endif

#if defined(SPX_CONFIG_DBG_TRACE_ASSERT) && (defined(DEBUG) || defined(_DEBUG))
#define SPX_DBG_ASSERT(expr)                                                    \
    do {                                                                        \
        int fCond = !!(expr);                                                   \
        if (!fCond) {                                                           \
            __SPX_TRACE_ASSERT("SPX_ASSERT: ", __FILE__, __LINE__, expr);       \
            abort();                                                            \
    } } while (0)
#define SPX_DBG_ASSERT_WITH_MESSAGE(expr, ...)                                                  \
    do {                                                                                        \
        int fCond = !!(expr);                                                                   \
        if (!fCond) {                                                                           \
            __SPX_TRACE_ASSERT_MSG("SPX_ASSERT: ", __FILE__, __LINE__, expr, ##__VA_ARGS__);    \
            abort();                                                                            \
    } } while (0)
#else
#define SPX_DBG_ASSERT(expr)
#define SPX_DBG_ASSERT_WITH_MESSAGE(expr, ...)
#endif

#if defined(SPX_CONFIG_DBG_TRACE_VERIFY) && (defined(DEBUG) || defined(_DEBUG))
#define SPX_DBG_VERIFY(expr)                                                    \
    do {                                                                        \
        int fCond = !!(expr);                                                   \
        if (!fCond) {                                                           \
            __SPX_TRACE_ASSERT("SPX_VERIFY: ", __FILE__, __LINE__, expr);       \
            abort();                                                            \
    } } while (0)
#define SPX_DBG_VERIFY_WITH_MESSAGE(expr, ...)                                                  \
    do {                                                                                        \
        int fCond = !!(expr);                                                                   \
        if (!fCond) {                                                                           \
            __SPX_TRACE_ASSERT_MSG("SPX_VERIFY: ", __FILE__, __LINE__, expr, ##__VA_ARGS__);    \
            abort();                                                                            \
    } } while (0)
#else
#define SPX_DBG_VERIFY(expr)                        (expr)
#define SPX_DBG_VERIFY_WITH_MESSAGE(expr, ...)      (expr)
#endif

#ifdef __cplusplus
#ifndef __SPX_THROW_HR_IMPL
#define __SPX_THROW_HR_IMPL(hr) static_assert(false, "__SPX_THROW_HR_IMPL should be defined elsewhere!")
#endif
#ifndef __SPX_THROW_HR
#define __SPX_THROW_HR(hr) __SPX_THROW_HR_IMPL(hr)
#endif

#ifdef SPX_CONFIG_TRACE_THROW_ON_FAIL
#define SPX_THROW_ON_FAIL(hr)                                                       \
    do {                                                                            \
        SPXHR x = hr;                                                               \
        if (SPX_FAILED(x)) {                                                        \
            __SPX_TRACE_HR("SPX_THROW_ON_FAIL: ", __FILE__, __LINE__, hr, x);       \
            __SPX_THROW_HR(x);                                                      \
    } } while (0)
#define SPX_THROW_ON_FAIL_IF_NOT(hr, hrNot)                                         \
    do {                                                                            \
        SPXHR x = hr;                                                               \
        if (x != hrNot) {                                                           \
            if (SPX_FAILED(x)) {                                                    \
                __SPX_TRACE_HR("SPX_THROW_ON_FAIL: ", __FILE__, __LINE__, hr, x);   \
                __SPX_THROW_HR(x);                                                  \
    } } } while (0)
#define SPX_THROW_HR_IF(hr, cond)                                                   \
    do {                                                                            \
        int fCond = !!(cond);                                                       \
        if (fCond) {                                                                \
            SPXHR x = hr;                                                           \
            __SPX_TRACE_HR("SPX_THROW_HR_IF: ", __FILE__, __LINE__, hr, x);         \
            __SPX_THROW_HR(x);                                                      \
    } } while (0)
#define SPX_THROW_HR(hr)                                                            \
    do {                                                                            \
        SPXHR x = hr;                                                               \
        __SPX_TRACE_HR("SPX_THROW_HR_IF: ", __FILE__, __LINE__, hr, x);             \
        __SPX_THROW_HR(x);                                                          \
    } while (0)
#else
#define SPX_THROW_ON_FAIL(hr)                                   \
    do {                                                        \
        SPXHR x = hr;                                           \
        if (SPX_FAILED(x)) {                                    \
            __SPX_THROW_HR(x);                                  \
    } } while (0)
#define SPX_THROW_ON_FAIL_IF_NOT(hr, hrNot)                     \
    do {                                                        \
        SPXHR x = hr;                                           \
        if (x != hrNot) {                                       \
            if (SPX_FAILED(x)) {                                \
                __SPX_THROW_HR(x);                              \
    } } } while (0)
#define SPX_THROW_HR_IF(hr, cond)                               \
    do {                                                        \
        int fCond = !!(cond);                                   \
        if (fCond) {                                            \
            SPXHR x = hr;                                       \
            __SPX_THROW_HR(x);                                  \
    } } while (0)
#define SPX_THROW_HR(hr)                                        \
    do {                                                        \
        SPXHR x = hr;                                           \
        __SPX_THROW_HR(x);                                      \
    } while (0)
#endif
#else
#define SPX_THROW_ON_FAIL(hr)                   static_assert(false)
#define SPX_THROW_ON_FAIL_IF_NOT(hr, hrNot)     static_assert(false)
#define SPX_THROW_HR_IF(hr, cond)               static_assert(false)
#define SPX_THROW_HR(hr)                        static_assert(false)
#endif

#ifdef SPX_CONFIG_TRACE_REPORT_ON_FAIL
#define SPX_REPORT_ON_FAIL(hr)                                                      \
    do {                                                                            \
        SPXHR x = hr;                                                               \
        if (SPX_FAILED(x)) {                                                        \
            __SPX_TRACE_HR("SPX_REPORT_ON_FAIL: ", __FILE__, __LINE__, hr, x);      \
    } } while (0)
#define SPX_REPORT_ON_FAIL_IFNOT(hr, hrNot)                                         \
    do {                                                                            \
        SPXHR x = hr;                                                               \
        if (x != hrNot) {                                                           \
            if (SPX_FAILED(x)) {                                                    \
                __SPX_TRACE_HR("SPX_REPORT_ON_FAIL: ", __FILE__, __LINE__, hr, x);  \
    } } } while (0)
#else
#define SPX_REPORT_ON_FAIL(hr)                                  \
    UNUSED(hr);
#define SPX_REPORT_ON_FAIL_IFNOT(hr, hrNot)                     \
    UNUSED(hr);                                                 \
    UNUSED(hrNot);
#endif

#ifdef SPX_CONFIG_TRACE_RETURN_ON_FAIL
#define SPX_RETURN_HR(hr)                                                           \
    do {                                                                            \
        SPXHR x = hr;                                                               \
        if (SPX_FAILED(x)) {                                                        \
            __SPX_TRACE_HR("SPX_RETURN_ON_FAIL: ", __FILE__, __LINE__, hr, x);      \
        }                                                                           \
        return x;                                                                   \
    } while (0)
#define SPX_RETURN_HR_IF(hr, cond)                                                  \
    do {                                                                            \
        int fCond = !!(cond);                                                       \
        if (fCond) {                                                                \
            SPXHR x = hr;                                                           \
            if (SPX_FAILED(x)) {                                                    \
                __SPX_TRACE_HR("SPX_RETURN_ON_FAIL: ", __FILE__, __LINE__, hr, x);  \
            }                                                                       \
            return x;                                                               \
    } } while (0)
#define SPX_RETURN_ON_FAIL(hr)                                                      \
    do {                                                                            \
        SPXHR x = hr;                                                               \
        if (SPX_FAILED(x)) {                                                        \
            __SPX_TRACE_HR("SPX_RETURN_ON_FAIL: ", __FILE__, __LINE__, hr, x);      \
            return x;                                                               \
    } } while (0)
#define SPX_RETURN_ON_FAIL_IF_NOT(hr, hrNot)                                        \
    do {                                                                            \
        SPXHR x = hr;                                                               \
        if (x != hrNot) {                                                           \
            if (SPX_FAILED(x)) {                                                    \
                __SPX_TRACE_HR("SPX_RETURN_ON_FAIL: ", __FILE__, __LINE__, hr, x);  \
                return x;                                                           \
    } } } while (0)
#else
#define SPX_RETURN_HR(hr) return hr
#define SPX_RETURN_HR_IF(hr, cond)                              \
    do {                                                        \
        int fCond = !!(cond);                                   \
        if (fCond) {                                            \
            return hr;                                          \
    } } while (0)
#define SPX_RETURN_ON_FAIL(hr)                                  \
    do {                                                        \
        SPXHR x = hr;                                           \
        if (SPX_FAILED(x)) {                                    \
            return x;                                           \
    } } while (0)
#define SPX_RETURN_ON_FAIL_IF_NOT(hr, hrNot)                    \
    do {                                                        \
        SPXHR x = hr;                                           \
        if (x != hrNot) {                                       \
            if (SPX_FAILED(x)) {                                \
                return x;                                       \
    } } } while (0)
#endif

#ifdef SPX_CONFIG_TRACE_EXITFN_ON_FAIL
#define SPX_EXITFN_HR(hr)                                                           \
    do {                                                                            \
        SPXHR x = hr;                                                               \
        if (SPX_FAILED(x)) {                                                        \
            __SPX_TRACE_HR("SPX_RETURN_ON_FAIL: ", __FILE__, __LINE__, hr, x);      \
        }                                                                           \
        goto SPX_EXITFN_CLEANUP;                                                    \
    } while (0)
#define SPX_EXITFN_HR_IF(hr, cond)                                                  \
    do {                                                                            \
        int fCond = !!(cond);                                                       \
        if (fCond) {                                                                \
            SPXHR x = hr;                                                           \
            if (SPX_FAILED(x)) {                                                    \
                __SPX_TRACE_HR("SPX_RETURN_ON_FAIL: ", __FILE__, __LINE__, hr, x);  \
            }                                                                       \
            goto SPX_EXITFN_CLEANUP;                                                \
    } } while (0)
#define SPX_EXITFN_ON_FAIL(hr)                                                      \
    do {                                                                            \
        SPXHR x = hr;                                                               \
        if (SPX_FAILED(x)) {                                                        \
            __SPX_TRACE_HR("SPX_RETURN_ON_FAIL: ", __FILE__, __LINE__, hr, x);      \
            goto SPX_EXITFN_CLEANUP;                                                \
    } } while (0)
#define SPX_EXITFN_ON_FAIL_IF_NOT(hr, hrNot)                                        \
    do {                                                                            \
        SPXHR x = hr;                                                               \
        if (x != hrNot) {                                                           \
            if (SPX_FAILED(x)) {                                                    \
                __SPX_TRACE_HR("SPX_RETURN_ON_FAIL: ", __FILE__, __LINE__, hr, x);  \
                goto SPX_EXITFN_CLEANUP;                                            \
    } } } while (0)
#else
#define SPX_EXITFN_HR(hr)                                       \
    do {                                                        \
        SPXHR x = hr;                                           \
        goto SPX_EXITFN_CLEANUP;                                \
    } while (0)
#define SPX_EXITFN_HR_IF(hr, cond)                              \
    do {                                                        \
        int fCond = !!(cond);                                   \
        if (fCond) {                                            \
            SPXHR x = hr;                                       \
            goto SPX_EXITFN_CLEANUP;                            \
    } } while (0)
#define SPX_EXITFN_ON_FAIL(hr)                                  \
    do {                                                        \
        SPXHR x = hr;                                           \
        if (SPX_FAILED(x)) {                                    \
            goto SPX_EXITFN_CLEANUP;                            \
    } } while (0)
#define SPX_EXITFN_ON_FAIL_IF_NOT(hr, hrNot)                    \
    do {                                                        \
        SPXHR x = hr;                                           \
        if (x != hrNot) {                                       \
            if (SPX_FAILED(x)) {                                \
                goto SPX_EXITFN_CLEANUP;                        \
    } } } while (0)
#endif

#define SPX_IFTRUE_THROW_HR(cond, hr)               SPX_THROW_HR_IF(hr, cond)
#define SPX_IFFALSE_THROW_HR(cond, hr)              SPX_THROW_HR_IF(hr, !(cond))
#define SPX_IFFAILED_THROW_HR(hr)                   SPX_THROW_ON_FAIL(hr)
#define SPX_IFFAILED_THROW_HR_IFNOT(hr, hrNot)      SPX_THROW_ON_FAIL_IF_NOT(hr, hrNot)

#define SPX_IFTRUE_EXITFN_WHR(cond, hr)             SPX_EXITFN_HR_IF(hr, cond)
#define SPX_IFFALSE_EXITFN_WHR(cond, hr)            SPX_EXITFN_HR_IF(hr, !(cond))
#define SPX_IFFAILED_EXITFN_WHR(hr)                 SPX_EXITFN_ON_FAIL(hr)
#define SPX_IFFAILED_EXITFN_WHR_IFNOT(hr, hrNot)    SPX_EXITFN_ON_FAIL_IF_NOT(hr, hrNot)

#define SPX_IFTRUE_RETURN_HR(cond, hr)              SPX_RETURN_HR_IF(hr, cond)
#define SPX_IFFALSE_RETURN_HR(cond, hr)             SPX_RETURN_HR_IF(hr, !(cond))
#define SPX_IFFAILED_RETURN_HR(hr)                  SPX_RETURN_ON_FAIL(hr)
#define SPX_IFFAILED_RETURN_HR_IFNOT(hr, hrNot)     SPX_RETURN_ON_FAIL_IF_NOT(hr, hrNot)

#define SPX_IFTRUE(cond, expr) \
    do {                                                        \
        int fCondT = !!(cond);                                  \
        if (fCondT) {                                           \
            expr;                                               \
    } } while (0)

#define SPX_IFFALSE(cond, expr) \
    do {                                                        \
        int fCondF = !!(cond);                                  \
        if (!fCondF) {                                          \
            expr;                                               \
    } } while (0)

#define SPX_IFTRUE_RETURN_X(cond, expr) \
    do {                                                        \
        int fCondT = !!(cond);                                  \
        if (fCondT) {                                           \
            return expr;                                        \
    } } while (0)

#define SPX_IFFALSE_RETURN_X(cond, expr) \
    do {                                                        \
        int fCondF = !!(cond);                                  \
        if (!fCondF) {                                          \
            return expr;                                        \
    } } while (0)

#define SPX_IFTRUE_RETURN(cond) \
    do {                                                        \
        int fCondT = !!(cond);                                  \
        if (fCondT) {                                           \
            return;                                             \
    } } while (0)

#define SPX_IFFALSE_RETURN(cond) \
    do {                                                        \
        int fCondF = !!(cond);                                  \
        if (!fCondF) {                                          \
            return;                                             \
    } } while (0)

//---------------------------------------------------------------------------

#ifdef __SPXDEBUG_H_EXAMPLES_IN_MAIN

void main()
{
    int x = 4;
    printf("%s = %d\n", __SPX_EXPR_AS_STRING(x + 3), x + 3);

    SPX_TRACE_INFO("hello there");
    SPX_TRACE_ERROR("hello there");
    SPX_TRACE_WARNING("hello there");
    SPX_TRACE_VERBOSE("hello there");

    SPX_TRACE_INFO("hello there %d", 5);
    SPX_TRACE_ERROR("hello there %d", 5);
    SPX_TRACE_WARNING("hello there %d", 5);
    SPX_TRACE_VERBOSE("hello there %d", 5);

    SPX_TRACE_INFO_IF(false, "hello there false");
    SPX_TRACE_ERROR_IF(false, "hello there false");
    SPX_TRACE_WARNING_IF(false, "hello there false");
    SPX_TRACE_VERBOSE_IF(false, "hello there false");

    SPX_TRACE_INFO_IF(false, "hello there false %d", 5);
    SPX_TRACE_ERROR_IF(false, "hello there false %d", 5);
    SPX_TRACE_WARNING_IF(false, "hello there false %d", 5);
    SPX_TRACE_VERBOSE_IF(false, "hello there false %d", 5);

    SPX_TRACE_INFO_IF(true, "hello there true");
    SPX_TRACE_ERROR_IF(true, "hello there true");
    SPX_TRACE_WARNING_IF(true, "hello there true");
    SPX_TRACE_VERBOSE_IF(true, "hello there true");

    SPX_TRACE_INFO_IF(true, "hello there true %d", 5);
    SPX_TRACE_ERROR_IF(true, "hello there true %d", 5);
    SPX_TRACE_WARNING_IF(true, "hello there true %d", 5);
    SPX_TRACE_VERBOSE_IF(true, "hello there true %d", 5);

    SPX_DBG_TRACE_INFO("hello there");
    SPX_DBG_TRACE_ERROR("hello there");
    SPX_DBG_TRACE_WARNING("hello there");
    SPX_DBG_TRACE_VERBOSE("hello there");

    SPX_DBG_TRACE_INFO("hello there %d", 5);
    SPX_DBG_TRACE_ERROR("hello there %d", 5);
    SPX_DBG_TRACE_WARNING("hello there %d", 5);
    SPX_DBG_TRACE_VERBOSE("hello there %d", 5);

    SPX_DBG_TRACE_INFO_IF(false, "hello there false");
    SPX_DBG_TRACE_ERROR_IF(false, "hello there false");
    SPX_DBG_TRACE_WARNING_IF(false, "hello there false");
    SPX_DBG_TRACE_VERBOSE_IF(false, "hello there false");

    SPX_DBG_TRACE_INFO_IF(false, "hello there false %d", 5);
    SPX_DBG_TRACE_ERROR_IF(false, "hello there false %d", 5);
    SPX_DBG_TRACE_WARNING_IF(false, "hello there false %d", 5);
    SPX_DBG_TRACE_VERBOSE_IF(false, "hello there false %d", 5);

    SPX_DBG_TRACE_INFO_IF(true, "hello there true");
    SPX_DBG_TRACE_ERROR_IF(true, "hello there true");
    SPX_DBG_TRACE_WARNING_IF(true, "hello there true");
    SPX_DBG_TRACE_VERBOSE_IF(true, "hello there true");

    SPX_DBG_TRACE_INFO_IF(true, "hello there true %d", 5);
    SPX_DBG_TRACE_ERROR_IF(true, "hello there true %d", 5);
    SPX_DBG_TRACE_WARNING_IF(true, "hello there true %d", 5);
    SPX_DBG_TRACE_VERBOSE_IF(true, "hello there true %d", 5);

    SPX_TRACE_SCOPE("A", "B");

    SPX_TRACE_FUNCTION();
    SPX_DBG_TRACE_FUNCTION();

    SPX_DBG_ASSERT(false);
    SPX_DBG_ASSERT(true);

    SPX_DBG_ASSERT_WITH_MESSAGE(false, "HEY!");
    SPX_DBG_ASSERT_WITH_MESSAGE(true, "HEY!!");

    SPX_DBG_VERIFY(false);
    SPX_DBG_VERIFY(true);

    SPX_DBG_VERIFY_WITH_MESSAGE(false, "HEY!");
    SPX_DBG_VERIFY_WITH_MESSAGE(true, "HEY!!");

    SPXHR hr1 { 0x80001111 };
    SPXHR hr2 { 0x00001111 };

    SPX_TRACE_VERBOSE("Testing out SPX_REPORT_ON_FAIL, should see two failures...");
    SPX_REPORT_ON_FAIL(hr1);
    SPX_REPORT_ON_FAIL_IFNOT(hr1, 0x80001000);
    SPX_TRACE_VERBOSE("Testing out SPX_REPORT_ON_FAIL, should see two failures... Done!");

    SPX_TRACE_VERBOSE("Testing out SPX_REPORT_ON_FAIL, should see zero failures...");
    SPX_REPORT_ON_FAIL(hr2);
    SPX_REPORT_ON_FAIL_IFNOT(hr1, 0x80001111);
    SPX_REPORT_ON_FAIL_IFNOT(hr2, 0x80001111);
    SPX_REPORT_ON_FAIL_IFNOT(hr2, 0x80001000);
    SPX_TRACE_VERBOSE("Testing out SPX_REPORT_ON_FAIL, should see zero failures... Done!");
}

#endif
