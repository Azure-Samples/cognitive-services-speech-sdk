//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#pragma once

#include <spxdebug.h>
#include "conversation_translator.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {
namespace ConversationTranslation {

    /// <summary>
    /// Converts the conversation state into a string
    /// </summary>
    const char * ConversationStateString(const CSpxConversationTranslator::ConversationState state);



// The Android GCC compiler expects a void * argument when the %p format specifier is used
#if defined(ANDROID) || defined(__ANDROID__)
#define P_FORMAT_POINTER(T) ((void *)T)
#else
#define P_FORMAT_POINTER(T) T
#endif



#ifndef _MSC_VER
    // macros in this header generate a bunch of
    // "ISO C++11 requires at least one argument for the "..." in a variadic macro" errors.
    // system_header pragma is the only mechanism that helps to suppress them.
    // https://stackoverflow.com/questions/35587137/how-to-suppress-gcc-variadic-macro-argument-warning-for-zero-arguments-for-a-par
#pragma GCC system_header
#endif



#define CT_LOG_INFO(msg, ...) \
    __SPX_TRACE_INFO("[CONV_TRANS][INFO]: ", __FILE__, __LINE__, msg, ##__VA_ARGS__)

#define CT_LOG_WARNING(msg, ...) \
    __SPX_TRACE_WARNING("[CONV_TRANS][WARNING]: ", __FILE__, __LINE__,  msg, ##__VA_ARGS__)

#define CT_LOG_ERROR(msg, ...) \
    __SPX_TRACE_ERROR("[CONV_TRANS][ERROR]: ", __FILE__, __LINE__, msg, ##__VA_ARGS__)



#define CT_I_LOG_INFO(msg, ...) \
    __SPX_TRACE_INFO("[CONV_TRANS][INFO]: ", __FILE__, __LINE__, "[0x%p] " msg, P_FORMAT_POINTER(this), ##__VA_ARGS__)

#define CT_I_LOG_WARNING(msg, ...) \
    __SPX_TRACE_WARNING("[CONV_TRANS][WARNING]: ", __FILE__, __LINE__, "[0x%p] " msg, P_FORMAT_POINTER(this), ##__VA_ARGS__)

#define CT_I_LOG_ERROR(msg, ...) \
    __SPX_TRACE_ERROR("[CONV_TRANS][ERROR]: ", __FILE__, __LINE__, "[0x%p] " msg, P_FORMAT_POINTER(this), ##__VA_ARGS__)



#ifdef SPX_CONFIG_DBG_TRACE_SCOPE
#define CT_DBG_TRACE_SCOPE(x, y) __SPX_TRACE_SCOPE("[CONV_TRANS][VERBOSE_SCOPE_ENTER]: ", __FILE__, __LINE__, "[CONV_TRANS][VERBOSE_SCOPE_EXIT]: ", x, y)
#else
#define CT_DBG_TRACE_SCOPE(x, y)
#endif

#ifdef SPX_CONFIG_DBG_TRACE_FUNCTION
#define CT_DBG_TRACE_FUNCTION(...) __SPX_TRACE_VERBOSE("[CONV_TRANS][VERBOSE_FNCTN]: ", __FILE__, __LINE__, __FUNCTION__)
#else
#define CT_DBG_TRACE_FUNCTION(...)
#endif



#define CT_THROW_HR_IF(cond, hr) \
    do { \
        int fCond = !!(cond); \
        if (fCond) { \
            SPXHR x = hr; \
            CT_LOG_ERROR("(THROW_HR_IF) Throwing " __SPX_EXPR_AS_STRING(hr) " = 0x%0" PRIxPTR ". Failed condition = '%s'", x, #cond); \
            __SPX_THROW_HR(x); \
    } } while (0)

#define CT_THROW_HR(hr) \
    do { \
        SPXHR x = hr; \
        CT_LOG_ERROR("(THROW_HR) Throwing " __SPX_EXPR_AS_STRING(hr) " = 0x%0" PRIxPTR, x); \
        __SPX_THROW_HR(x); \
    } while (0)



#define CT_I_THROW_HR_IF(cond, hr) \
    do { \
        int fCond = !!(cond); \
        if (fCond) { \
            SPXHR x = hr; \
            CT_I_LOG_ERROR("(THROW_HR_IF) Throwing " __SPX_EXPR_AS_STRING(hr) " = 0x%0" PRIxPTR ". Failed condition = '%s'", x, #cond); \
            __SPX_THROW_HR(x); \
    } } while (0)

#define CT_I_THROW_HR(hr) \
    do { \
        SPXHR x = hr; \
        CT_I_LOG_ERROR("(THROW_HR) Throwing " __SPX_EXPR_AS_STRING(hr) " = 0x%0" PRIxPTR, x); \
        __SPX_THROW_HR(x); \
    } while (0)



#define CT_GET_AND_LOG_STATE(msg, ...) \
    const auto state = GetState(); \
    CT_I_LOG_INFO("(%s) " msg, ConversationStateString(state), ##__VA_ARGS__)

#define CT_HANDLE_UNSUPPORTED_STATE() \
    CT_I_LOG_ERROR("(%s) Unsupported", ConversationStateString(state)); \
    CT_I_THROW_HR(SPXERR_NOT_IMPL)

}}}}} // Microsoft::CognitiveServices::Speech::Impl::Conversation


