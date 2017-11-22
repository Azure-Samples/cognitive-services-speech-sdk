// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include <stdio.h>
#include <string.h>
#include "umocktypes.h"
#include "azure_c_shared_utility/macro_utils.h"
#include "umocktypes_charptr.h"
#include "umockalloc.h"
#include "umock_log.h"

char* umocktypes_stringify_charptr(const char** value)
{
    char* result;

    if (value == NULL)
    {
        /* Codes_SRS_UMOCKTYPES_CHARPTR_01_004: [ If value is NULL, umocktypes_stringify_charptr shall return NULL. ]*/
        UMOCK_LOG("umocktypes_stringify_charptr: NULL value.");
        result = NULL;
    }
    else
    {
        if (*value == NULL)
        {
            result = (char*)umockalloc_malloc(sizeof("NULL") + 1);
            if (result != NULL)
            {
                (void)memcpy(result, "NULL", sizeof("NULL") + 1);
            }
        }
        else
        {
            /* Codes_SRS_UMOCKTYPES_CHARPTR_01_002: [ umocktypes_stringify_charptr shall return a string containing the string representation of value, enclosed by quotes ("value"). ] */
            size_t length = strlen(*value);
            result = (char*)umockalloc_malloc(length + 3);
            /* Codes_SRS_UMOCKTYPES_CHARPTR_01_003: [ If allocating a new string to hold the string representation fails, umocktypes_stringify_charptr shall return NULL. ]*/
            if (result == NULL)
            {
                UMOCK_LOG("umocktypes_stringify_charptr: Cannot allocate memory for result.");
            }
            else
            {
                result[0] = '\"';
                (void)memcpy(result + 1, *value, length);
                result[length + 1] = '\"';
                result[length + 2] = '\0';
            }
        }
    }

    return result;
}

/* Codes_SRS_UMOCKTYPES_CHARPTR_01_005: [ umocktypes_are_equal_charptr shall compare the 2 strings pointed to by left and right. ] */
int umocktypes_are_equal_charptr(const char** left, const char** right)
{
    int result;

    /* Codes_SRS_UMOCKTYPES_CHARPTR_01_007: [ If left and right are equal, umocktypes_are_equal_charptr shall return 1. ]*/
    if (left == right)
    {
        result = 1;
    }
    else if ((left == NULL) || (right == NULL))
    {
        /* Codes_SRS_UMOCKTYPES_CHARPTR_01_008: [ If only one of the left and right argument is NULL, umocktypes_are_equal_charptr shall return 0. ] */
        result = 0;
    }
    else
    {
        /* Codes_SRS_UMOCKTYPES_CHARPTR_01_009: [ If the string pointed to by left is equal to the string pointed to by right, umocktypes_are_equal_charptr shall return 1. ]*/
        /* Codes_SRS_UMOCKTYPES_CHARPTR_01_010: [ If the string pointed to by left is different than the string pointed to by right, umocktypes_are_equal_charptr shall return 0. ]*/
        /* Codes_SRS_UMOCKTYPES_CHARPTR_01_006: [ The comparison shall be case sensitive. ]*/
        result = (strcmp(*left, *right) == 0) ? 1 : 0;
    }

    return result;
}

int umocktypes_copy_charptr(char** destination, const char** source)
{
    int result;

    /* Codes_SRS_UMOCKTYPES_CHARPTR_01_013: [ If source or destination are NULL, umocktypes_copy_charptr shall return a non-zero value. ]*/
    if ((destination == NULL) || (source == NULL))
    {
        UMOCK_LOG("umocktypes_copy_charptr: Bad arguments: destination = %p, source = %p.",
            destination, source);
        result = __LINE__;
    }
    else
    {
        if (*source == NULL)
        {
            *destination = NULL;
            result = 0;
        }
        else
        {
            size_t source_length = strlen(*source);
            /* Codes_SRS_UMOCKTYPES_CHARPTR_01_012: [ The number of bytes allocated shall accomodate the string pointed to by source. ]*/
            /* Codes_SRS_UMOCKTYPES_CHARPTR_01_011: [ umocktypes_copy_charptr shall allocate a new sequence of chars by using umockalloc_malloc. ]*/
            /* Codes_SRS_UMOCKTYPES_CHARPTR_01_015: [ The newly allocated string shall be returned in the destination argument. ]*/
            *destination = (char*)umockalloc_malloc(source_length + 1);
            if (*destination == NULL)
            {
                /* Codes_SRS_UMOCKTYPES_CHARPTR_01_036: [ If allocating the memory for the new string fails, umocktypes_copy_charptr shall fail and return a non-zero value. ]*/
                UMOCK_LOG("umocktypes_copy_charptr: Failed allocating memory for the destination string.");
                result = __LINE__;
            }
            else
            {
                /* Codes_SRS_UMOCKTYPES_CHARPTR_01_014: [ umocktypes_copy_charptr shall copy the string pointed to by source to the newly allocated memory. ]*/
                (void)memcpy(*destination, *source, source_length + 1);
                /* Codes_SRS_UMOCKTYPES_CHARPTR_01_016: [ On success umocktypes_copy_charptr shall return 0. ]*/
                result = 0;
            }
        }
    }

    return result;
}

void umocktypes_free_charptr(char** value)
{
    /* Codes_SRS_UMOCKTYPES_CHARPTR_01_018: [ If value is NULL, umocktypes_free_charptr shall do nothing. ] */
    if (value != NULL)
    {
        /* Codes_SRS_UMOCKTYPES_CHARPTR_01_017: [ umocktypes_free_charptr shall free the string pointed to by value. ]*/
        umockalloc_free(*value);
    }
}

char* umocktypes_stringify_const_charptr(const char** value)
{
    char* result;

    if (value == NULL)
    {
        /* Codes_SRS_UMOCKTYPES_CHARPTR_01_020: [ If value is NULL, umocktypes_stringify_const_charptr shall return NULL. ]*/
        UMOCK_LOG("umocktypes_stringify_const_charptr: NULL value.");
        result = NULL;
    }
    else
    {
        if (*value == NULL)
        {
            result = (char*)umockalloc_malloc(sizeof("NULL") + 1);
            if (result == NULL)
            {
                UMOCK_LOG("umocktypes_stringify_const_charptr: Cannot allocate memoryfor result string.");
            }
            else
            {
                (void)memcpy(result, "NULL", sizeof("NULL") + 1);
            }
        }
        else
        {
            /* Codes_SRS_UMOCKTYPES_CHARPTR_01_019: [ umocktypes_stringify_const_charptr shall return a string containing the string representation of value, enclosed by quotes ("value"). ] */
            size_t length = strlen(*value);
            result = (char*)umockalloc_malloc(length + 3);
            /* Codes_SRS_UMOCKTYPES_CHARPTR_01_021: [ If allocating a new string to hold the string representation fails, umocktypes_stringify_const_charptr shall return NULL. ]*/
            if (result != NULL)
            {
                result[0] = '\"';
                (void)memcpy(result + 1, *value, length);
                result[length + 1] = '\"';
                result[length + 2] = '\0';
            }
        }
    }

    return result;
}

/* Codes_SRS_UMOCKTYPES_CHARPTR_01_022: [ umocktypes_are_equal_const_charptr shall compare the 2 strings pointed to by left and right. ] */
int umocktypes_are_equal_const_charptr(const char** left, const char** right)
{
    int result;

    /* Codes_SRS_UMOCKTYPES_CHARPTR_01_024: [ If left and right are equal, umocktypes_are_equal_const_charptr shall return 1. ]*/
    if (left == right)
    {
        result = 1;
    }
    else if ((left == NULL) || (right == NULL))
    {
        /* Codes_SRS_UMOCKTYPES_CHARPTR_01_025: [ If only one of the left and right argument is NULL, umocktypes_are_equal_const_charptr shall return 0. ] */
        result = 0;
    }
    else
    {
        /* Codes_SRS_UMOCKTYPES_CHARPTR_01_026: [ If the string pointed to by left is equal to the string pointed to by right, umocktypes_are_equal_const_charptr shall return 1. ]*/
        /* Codes_SRS_UMOCKTYPES_CHARPTR_01_027: [ If the string pointed to by left is different than the string pointed to by right, umocktypes_are_equal_const_charptr shall return 0. ]*/
        /* Codes_SRS_UMOCKTYPES_CHARPTR_01_023: [ The comparison shall be case sensitive. ]*/
        result = (strcmp(*left, *right) == 0) ? 1 : 0;
    }

    return result;
}

int umocktypes_copy_const_charptr(const char** destination, const char** source)
{
    int result;

    /* Codes_SRS_UMOCKTYPES_CHARPTR_01_033: [ If source or destination are NULL, umocktypes_copy_const_charptr shall return a non-zero value. ]*/
    if ((destination == NULL) || (source == NULL))
    {
        UMOCK_LOG("umocktypes_copy_const_charptr: Bad arguments: destination = %p, source = %p.",
            destination, source);
        result = __LINE__;
    }
    else
    {
        if (*source == NULL)
        {
            *destination = NULL;
            result = 0;
        }
        else
        {
            size_t source_length = strlen(*source);
            /* Codes_SRS_UMOCKTYPES_CHARPTR_01_029: [ The number of bytes allocated shall accomodate the string pointed to by source. ]*/
            /* Codes_SRS_UMOCKTYPES_CHARPTR_01_028: [ umocktypes_copy_const_charptr shall allocate a new sequence of chars by using umockalloc_malloc. ]*/
            /* Codes_SRS_UMOCKTYPES_CHARPTR_01_031: [ The newly allocated string shall be returned in the destination argument. ]*/
            *destination = (char*)umockalloc_malloc(source_length + 1);
            if (*destination == NULL)
            {
                /* Codes_SRS_UMOCKTYPES_CHARPTR_01_037: [ If allocating the memory for the new string fails, umocktypes_copy_const_charptr shall fail and return a non-zero value. ]*/
                UMOCK_LOG("umocktypes_copy_const_charptr: Cannot allocate memory for destination string.");
                result = __LINE__;
            }
            else
            {
                /* Codes_SRS_UMOCKTYPES_CHARPTR_01_030: [ umocktypes_copy_const_charptr shall copy the string pointed to by source to the newly allocated memory. ]*/
                (void)memcpy((void*)*destination, *source, source_length + 1);
                /* Codes_SRS_UMOCKTYPES_CHARPTR_01_032: [ On success umocktypes_copy_const_charptr shall return 0. ]*/
                result = 0;
            }
        }
    }

    return result;
}

void umocktypes_free_const_charptr(const char** value)
{
    /* Codes_SRS_UMOCKTYPES_CHARPTR_01_035: [ If value is NULL, umocktypes_free_const_charptr shall do nothing. ] */
    if (value != NULL)
    {
        /* Codes_SRS_UMOCKTYPES_CHARPTR_01_034: [ umocktypes_free_const_charptr shall free the string pointed to by value. ]*/
        umockalloc_free((void*)*value);
    }
}

int umocktypes_charptr_register_types(void)
{
    int result;

    /* Codes_SRS_UMOCKTYPES_CHARPTR_01_001: [ umocktypes_charptr_register_types shall register support for the types char\* and const char\* by using the REGISTER_UMOCK_VALUE_TYPE macro provided by umockc. ]*/
    if ((REGISTER_TYPE(char*, charptr) != 0) ||
        (REGISTER_TYPE(const char*, const_charptr) != 0))
    {
        /* Codes_SRS_UMOCKTYPES_CHARPTR_01_039: [ If registering any of the types fails, umocktypes_charptr_register_types shall fail and return a non-zero value. ]*/
        UMOCK_LOG("umocktypes_charptr_register_types: Cannot register types.");
        result = __LINE__;
    }
    else
    {
        /* Codes_SRS_UMOCKTYPES_CHARPTR_01_038: [ On success, umocktypes_charptr_register_types shall return 0. ]*/
        result = 0;
    }

    return result;
}
