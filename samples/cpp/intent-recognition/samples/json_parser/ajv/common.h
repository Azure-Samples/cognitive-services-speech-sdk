//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//
#ifndef __AJV_COMMON_H
#define __AJV_COMMON_H

#include <cstring>

namespace ajv {

    enum class JsonKind
    {
        Error = -1,
        End = 0,
        Object = '{',
        Array = '[',
        String = '\"',
        Number = '1',
        Boolean = 'b',
        Null = 'n',
        Unspecified = '?'
    };

}

#ifndef AJV_FAST
#define AJV_SMALL
#endif

#ifdef AJV_SMALL

#ifdef AVJ_FN_NO_INLINE
#undef AVJ_FN_NO_INLINE
#endif
#ifdef AVJ_FN_NO_INLINE_
#undef AVJ_FN_NO_INLINE_
#endif

#if defined(_MSC_VER)
#define AJV_FN_NO_INLINE inline __declspec(noinline)
#define AJV_FN_NO_INLINE_(type) inline __declspec(noinline) type
#elif defined(__clang__)
#define AJV_FN_NO_INLINE inline __attribute__((noinline))
#define AJV_FN_NO_INLINE_(type) inline type __attribute__((noinline))
#elif defined(__GNUG__)
#define AJV_FN_NO_INLINE inline
#define AJV_FN_NO_INLINE_(type) inline type
#endif

#endif // AJV_SMALL

#ifndef AJV_FN_NO_INLINE
#define AJV_FN_NO_INLINE inline
#endif
#ifndef AJV_FN_NO_INLINE_
#define AJV_FN_NO_INLINE_(type) inline type
#endif

#endif // __AJV_COMMON_H
