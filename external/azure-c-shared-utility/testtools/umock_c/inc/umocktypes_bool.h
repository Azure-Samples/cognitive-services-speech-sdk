// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef UMOCKTYPES_BOOL_H
#define UMOCKTYPES_BOOL_H

#include "azure_c_shared_utility/macro_utils.h"

#ifdef __cplusplus
extern "C" {
#include <cstdbool>
#else
#include <stdbool.h>
#endif

    extern int umocktypes_bool_register_types(void);

    extern char* umocktypes_stringify_bool(const bool* value); \
    extern int umocktypes_are_equal_bool(const bool* left, const bool* right); \
    extern int umocktypes_copy_bool(bool* destination, const bool* source); \
    extern void umocktypes_free_bool(bool* value);

#ifdef __cplusplus
}
#endif

#endif /* UMOCKTYPES_BOOL_H */
