// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef UMOCK_C_NEGATIVE_TESTS_H
#define UMOCK_C_NEGATIVE_TESTS_H

#ifdef __cplusplus
#include <cstddef>
extern "C" {
#else
#include <stddef.h>
#endif

    extern int umock_c_negative_tests_init(void);
    extern void umock_c_negative_tests_deinit(void);
    extern void umock_c_negative_tests_snapshot(void);
    extern void umock_c_negative_tests_reset(void);
    extern void umock_c_negative_tests_fail_call(size_t index);
    extern size_t umock_c_negative_tests_call_count(void);

#ifdef __cplusplus
}
#endif

#endif /* UMOCK_C_NEGATIVE_TESTS_H */
