// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include <stdbool.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include "azure_c_shared_utility/macro_utils.h"
#include "umocktypes.h"
#include "umock_c.h"
#include "umockalloc.h"
#include "umocktypes_bool.h"

/* Codes_SRS_UMOCKTYPES_BOOL_01_002: [ umocktypes_stringify_bool shall return the string representation of value. ]*/
char* umocktypes_stringify_bool(const bool* value)
{
    char* result;

    /* Codes_SRS_UMOCKTYPES_BOOL_01_003: [ If value is NULL, umocktypes_stringify_bool shall return NULL. ]*/
    if (value == NULL)
    {
        UMOCK_LOG("umocktypes_stringify_bool: NULL value.");
        result = NULL;
    }
    else
    {
        const char* stringified_bool = *value ? "true" : "false";
        size_t length = strlen(stringified_bool);

        /* Codes_SRS_UMOCKTYPES_BOOL_01_004: [ If allocating a new string to hold the string representation fails, umocktypes_stringify_bool shall return NULL. ]*/
        result = (char*)umockalloc_malloc(length + 1);
        if (result == NULL)
        {
            UMOCK_LOG("umocktypes_stringify_bool: Cannot allocate memory for result.");
        }
        else
        {
            (void)memcpy(result, stringified_bool, length + 1);
        }
    }
    return result;
}

/* Codes_SRS_UMOCKTYPES_BOOL_01_006: [ umocktypes_are_equal_bool shall compare the 2 chars pointed to by left and right. ]*/
int umocktypes_are_equal_bool(const bool* left, const bool* right)
{
    int result;

    if ((left == NULL) || (right == NULL))
    {
        /* Codes_SRS_UMOCKTYPES_BOOL_01_007: [ If any of the arguments is NULL, umocktypes_are_equal_bool shall return -1. ]*/
        UMOCK_LOG("umocktypes_are_equal_bool: Bad arguments:left = %p, right = %p.", left, right);
        result = -1;
    }
    else
    {
        /* Codes_SRS_UMOCKTYPES_BOOL_01_009: [ If the values pointed to by left and right are different, umocktypes_are_equal_bool shall return 0. ]*/
        /* Codes_SRS_UMOCKTYPES_BOOL_01_008: [ If the values pointed to by left and right are equal, umocktypes_are_equal_bool shall return 1. ]*/
        result = ((*left) == (*right)) ? 1 : 0;
    }

    return result;
}

/* Codes_SRS_UMOCKTYPES_BOOL_01_010: [ umocktypes_copy_bool shall copy the bool value from source to destination. ]*/
int umocktypes_copy_bool(bool* destination, const bool* source)
{
    int result;

    if ((destination == NULL) ||
        (source == NULL))
    {
        /* Codes_SRS_UMOCKTYPES_BOOL_01_012: [ If source or destination are NULL, umocktypes_copy_bool shall return a non-zero value. ]*/
        UMOCK_LOG("umocktypes_copy_bool: Bad arguments:destination = %p, source = %p.", destination, source);
        result = __LINE__;
    }
    else
    {
        *destination = *source;

        /* Codes_SRS_UMOCKTYPES_BOOL_01_011: [ On success umocktypes_copy_bool shall return 0. ]*/
        result = 0;
    }
    return result;
}

/* Codes_SRS_UMOCKTYPES_BOOL_01_013: [ umocktypes_free_bool shall do nothing. ]*/
void umocktypes_free_bool(bool* value)
{
    (void)value;
}

int umocktypes_bool_register_types(void)
{
    int result;

    /* Codes_SRS_UMOCKTYPES_BOOL_01_001: [ umocktypes_bool_register_types shall register support for all the types in the module. ]*/
    if ((umocktypes_register_type("bool", (UMOCKTYPE_STRINGIFY_FUNC)umocktypes_stringify_bool, (UMOCKTYPE_ARE_EQUAL_FUNC)umocktypes_are_equal_bool, (UMOCKTYPE_COPY_FUNC)umocktypes_copy_bool, (UMOCKTYPE_FREE_FUNC)umocktypes_free_bool) != 0) ||
        (umocktypes_register_type("_Bool", (UMOCKTYPE_STRINGIFY_FUNC)umocktypes_stringify_bool, (UMOCKTYPE_ARE_EQUAL_FUNC)umocktypes_are_equal_bool, (UMOCKTYPE_COPY_FUNC)umocktypes_copy_bool, (UMOCKTYPE_FREE_FUNC)umocktypes_free_bool) != 0))
    {
        /* Codes_SRS_UMOCKTYPES_BOOL_01_015: [ If registering any of the types fails, umocktypes_bool_register_types shall fail and return a non-zero value. ]*/
        UMOCK_LOG("umocktypes_bool_register_types: Cannot register types.");
        result = __LINE__;
    }
    else
    {
        /* Codes_SRS_UMOCKTYPES_BOOL_01_014: [ On success, umocktypes_bool_register_types shall return 0. ]*/
        result = 0;
    }

    return result;
}
