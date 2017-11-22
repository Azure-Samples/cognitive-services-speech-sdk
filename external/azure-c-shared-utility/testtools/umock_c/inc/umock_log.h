// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef UMOCK_LOG_H
#define UMOCK_LOG_H

#ifdef __cplusplus
#include <cstdio>
#else
#include <stdio.h>
#endif

#if defined _MSC_VER
#define UMOCK_LOG(format, ...) \
    (void)printf(format "\r\n", __VA_ARGS__);
#else
#define UMOCK_LOG(format, ...) \
    (void)printf(format "\r\n", ##__VA_ARGS__);
#endif

#endif /* UMOCK_LOG_H */
