// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include "azure_c_shared_utility/macro_utils.h"
#include "umocktypes.h"
#include "umocktypes_c.h"
#include "umockalloc.h"
#include "umock_log.h"

#define IMPLEMENT_STRINGIFY(type, function_postfix, printf_specifier) \
    char* C2(umocktypes_stringify_,function_postfix)(type* value) \
    { \
        char* result; \
        if (value == NULL) \
        { \
            UMOCK_LOG(TOSTRING(C2(umocktypes_stringify_,function_postfix)) ": NULL value."); \
            result = NULL; \
        } \
        else \
        { \
            char temp_buffer[32]; \
            size_t length = sprintf(temp_buffer, printf_specifier, *value); \
            result = (char*)umockalloc_malloc(length + 1); \
            if (result == NULL) \
            { \
                UMOCK_LOG(TOSTRING(C2(umocktypes_stringify_,function_postfix)) ": Cannot allocate memory for result string."); \
            } \
            else \
            { \
                (void)memcpy(result, temp_buffer, length + 1); \
            } \
        } \
        return result; \
    }

#define IMPLEMENT_ARE_EQUAL(type, function_postfix) \
    int C2(umocktypes_are_equal_,function_postfix)(type* left, type* right) \
    { \
        int result; \
        if ((left == NULL) || (right == NULL)) \
        { \
            UMOCK_LOG(TOSTRING(C2(umocktypes_are_equal_,function_postfix)) ": Bad arguments: left = %p, right = %p", left, right); \
            result = -1; \
        } \
        else \
        { \
            result = ((*left) == (*right)) ? 1 : 0; \
        } \
        return result; \
    }

#define IMPLEMENT_COPY(type, function_postfix) \
    int C2(umocktypes_copy_,function_postfix)(type* destination, type* source) \
    { \
        int result; \
        if ((destination == NULL) || \
            (source == NULL)) \
        { \
            UMOCK_LOG(TOSTRING(C2(umocktypes_are_equal_,function_postfix)) ": Bad arguments: destination = %p, source = %p", destination, source); \
            result = __LINE__; \
        } \
        else \
        { \
            *destination = *source; \
            result = 0; \
        } \
        return result; \
    }

#define IMPLEMENT_FREE(type, function_postfix) \
    void C2(umocktypes_free_,function_postfix)(type* value) \
    { \
        (void)value; \
    }

#define IMPLEMENT_TYPE_HANDLERS(type, function_postfix, printf_specifier) \
    IMPLEMENT_STRINGIFY(type, function_postfix, printf_specifier) \
    IMPLEMENT_ARE_EQUAL(type, function_postfix) \
    IMPLEMENT_COPY(type, function_postfix) \
    IMPLEMENT_FREE(type, function_postfix)

/* Codes_SRS_UMOCKTYPES_C_01_002: [ umocktypes_stringify_char shall return the string representation of value. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_003: [ If value is NULL, umocktypes_stringify_char shall return NULL. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_005: [ If any other error occurs when creating the string representation, umocktypes_stringify_char shall return NULL. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_004: [ If allocating a new string to hold the string representation fails, umocktypes_stringify_char shall return NULL. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_006: [ umocktypes_are_equal_char shall compare the 2 chars pointed to by left and right. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_007: [ If any of the arguments is NULL, umocktypes_are_equal_char shall return -1. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_008: [ If the values pointed to by left and right are equal, umocktypes_are_equal_char shall return 1. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_009: [ If the values pointed to by left and right are different, umocktypes_are_equal_char shall return 0. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_010: [ umocktypes_copy_char shall copy the char value from source to destination. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_012: [ If source or destination are NULL, umocktypes_copy_char shall return a non-zero value. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_011: [ On success umocktypes_copy_char shall return 0. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_013: [ umocktypes_free_char shall do nothing. ]*/
IMPLEMENT_TYPE_HANDLERS(char, char, "%d")

/* Codes_SRS_UMOCKTYPES_C_01_014: [ umocktypes_stringify_unsignedchar shall return the string representation of value. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_015: [ If value is NULL, umocktypes_stringify_unsignedchar shall return NULL. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_017: [ If any other error occurs when creating the string representation, umocktypes_stringify_unsignedchar shall return NULL. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_016: [ If allocating a new string to hold the string representation fails, umocktypes_stringify_unsignedchar shall return NULL. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_018: [ umocktypes_are_equal_unsignedchar shall compare the 2 unsigned chars pointed to by left and right. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_019: [ If any of the arguments is NULL, umocktypes_are_equal_unsignedchar shall return -1. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_020: [ If the values pointed to by left and right are equal, umocktypes_are_equal_unsignedchar shall return 1. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_021: [ If the values pointed to by left and right are different, umocktypes_are_equal_unsignedchar shall return 0. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_022: [ umocktypes_copy_unsignedchar shall copy the unsigned char value from source to destination. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_024: [ If source or destination are NULL, umocktypes_copy_unsignedchar shall return a non-zero value. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_023: [ On success umocktypes_copy_unsignedchar shall return 0. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_025: [ umocktypes_free_unsignedchar shall do nothing. ]*/
IMPLEMENT_TYPE_HANDLERS(unsigned char, unsignedchar, "%u")

/* Codes_SRS_UMOCKTYPES_C_01_026: [ umocktypes_stringify_short shall return the string representation of value. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_027: [ If value is NULL, umocktypes_stringify_short shall return NULL. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_029: [ If any other error occurs when creating the string representation, umocktypes_stringify_short shall return NULL. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_028: [ If allocating a new string to hold the string representation fails, umocktypes_stringify_short shall return NULL. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_030: [ umocktypes_are_equal_short shall compare the 2 shorts pointed to by left and right. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_031: [ If any of the arguments is NULL, umocktypes_are_equal_short shall return -1. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_032: [ If the values pointed to by left and right are equal, umocktypes_are_equal_short shall return 1. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_033: [ If the values pointed to by left and right are different, umocktypes_are_equal_short shall return 0. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_034: [ umocktypes_copy_short shall copy the short value from source to destination. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_036: [ If source or destination are NULL, umocktypes_copy_short shall return a non-zero value. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_035: [ On success umocktypes_copy_short shall return 0. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_037: [ umocktypes_free_short shall do nothing. ]*/
IMPLEMENT_TYPE_HANDLERS(short, short, "%hd")

/* Codes_SRS_UMOCKTYPES_C_01_038: [ umocktypes_stringify_unsignedshort shall return the string representation of value. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_039: [ If value is NULL, umocktypes_stringify_unsignedshort shall return NULL. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_041: [ If any other error occurs when creating the string representation, umocktypes_stringify_unsignedshort shall return NULL. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_040: [ If allocating a new string to hold the string representation fails, umocktypes_stringify_unsignedshort shall return NULL. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_042: [ umocktypes_are_equal_unsignedshort shall compare the 2 unsigned shorts pointed to by left and right. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_043: [ If any of the arguments is NULL, umocktypes_are_equal_unsignedshort shall return -1. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_044: [ If the values pointed to by left and right are equal, umocktypes_are_equal_unsignedshort shall return 1. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_045: [ If the values pointed to by left and right are different, umocktypes_are_equal_unsignedshort shall return 0. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_046: [ umocktypes_copy_unsignedshort shall copy the unsigned short value from source to destination. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_048: [ If source or destination are NULL, umocktypes_copy_unsignedshort shall return a non-zero value. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_047: [ On success umocktypes_copy_unsignedshort shall return 0. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_049: [ umocktypes_free_unsignedshort shall do nothing. ]*/
IMPLEMENT_TYPE_HANDLERS(unsigned short, unsignedshort, "%hu")

/* Codes_SRS_UMOCKTYPES_C_01_050: [ umocktypes_stringify_int shall return the string representation of value. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_051: [ If value is NULL, umocktypes_stringify_int shall return NULL. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_053: [ If any other error occurs when creating the string representation, umocktypes_stringify_int shall return NULL. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_052: [ If allocating a new string to hold the string representation fails, umocktypes_stringify_int shall return NULL. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_054: [ umocktypes_are_equal_int shall compare the 2 ints pointed to by left and right. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_055: [ If any of the arguments is NULL, umocktypes_are_equal_int shall return -1. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_056: [ If the values pointed to by left and right are equal, umocktypes_are_equal_int shall return 1. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_057: [ If the values pointed to by left and right are different, umocktypes_are_equal_int shall return 0. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_058: [ umocktypes_copy_int shall copy the int value from source to destination. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_060: [ If source or destination are NULL, umocktypes_copy_int shall return a non-zero value. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_059: [ On success umocktypes_copy_int shall return 0. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_061: [ umocktypes_free_int shall do nothing. ]*/
IMPLEMENT_TYPE_HANDLERS(int, int, "%d")

/* Codes_SRS_UMOCKTYPES_C_01_062: [ umocktypes_stringify_unsignedint shall return the string representation of value. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_063: [ If value is NULL, umocktypes_stringify_unsignedint shall return NULL. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_065: [ If any other error occurs when creating the string representation, umocktypes_stringify_unsignedint shall return NULL. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_064: [ If allocating a new string to hold the string representation fails, umocktypes_stringify_unsignedint shall return NULL. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_066: [ umocktypes_are_equal_unsignedint shall compare the 2 unsigned ints pointed to by left and right. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_067: [ If any of the arguments is NULL, umocktypes_are_equal_unsignedint shall return -1. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_068: [ If the values pointed to by left and right are equal, umocktypes_are_equal_unsignedint shall return 1. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_069: [ If the values pointed to by left and right are different, umocktypes_are_equal_unsignedint shall return 0. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_070: [ umocktypes_copy_unsignedint shall copy the unsigned int value from source to destination. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_072: [ If source or destination are NULL, umocktypes_copy_unsignedint shall return a non-zero value. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_071: [ On success umocktypes_copy_unsignedint shall return 0. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_073: [ umocktypes_free_unsignedint shall do nothing. ]*/
IMPLEMENT_TYPE_HANDLERS(unsigned int, unsignedint, "%u")

/* Codes_SRS_UMOCKTYPES_C_01_074: [ umocktypes_stringify_long shall return the string representation of value. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_075: [ If value is NULL, umocktypes_stringify_long shall return NULL. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_077: [ If any other error occurs when creating the string representation, umocktypes_stringify_long shall return NULL. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_076: [ If allocating a new string to hold the string representation fails, umocktypes_stringify_long shall return NULL. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_078: [ umocktypes_are_equal_long shall compare the 2 longs pointed to by left and right. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_079: [ If any of the arguments is NULL, umocktypes_are_equal_long shall return -1. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_080: [ If the values pointed to by left and right are equal, umocktypes_are_equal_long shall return 1. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_081: [ If the values pointed to by left and right are different, umocktypes_are_equal_long shall return 0. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_082: [ umocktypes_copy_long shall copy the long value from source to destination. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_084: [ If source or destination are NULL, umocktypes_copy_long shall return a non-zero value. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_083: [ On success umocktypes_copy_long shall return 0. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_085: [ umocktypes_free_long shall do nothing. ]*/
IMPLEMENT_TYPE_HANDLERS(long, long, "%ld")

/* Codes_SRS_UMOCKTYPES_C_01_086: [ umocktypes_stringify_unsignedlong shall return the string representation of value. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_087: [ If value is NULL, umocktypes_stringify_unsignedlong shall return NULL. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_089: [ If any other error occurs when creating the string representation, umocktypes_stringify_unsignedlong shall return NULL. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_088: [ If allocating a new string to hold the string representation fails, umocktypes_stringify_unsignedlong shall return NULL. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_090: [ umocktypes_are_equal_unsignedlong shall compare the 2 unsigned longs pointed to by left and right. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_091: [ If any of the arguments is NULL, umocktypes_are_equal_unsignedlong shall return -1. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_092: [ If the values pointed to by left and right are equal, umocktypes_are_equal_unsignedlong shall return 1. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_093: [ If the values pointed to by left and right are different, umocktypes_are_equal_unsignedlong shall return 0. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_094: [ umocktypes_copy_unsignedlong shall copy the unsigned long value from source to destination. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_096: [ If source or destination are NULL, umocktypes_copy_unsignedlong shall return a non-zero value. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_095: [ On success umocktypes_copy_unsignedlong shall return 0. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_097: [ umocktypes_free_unsignedlong shall do nothing. ]*/
IMPLEMENT_TYPE_HANDLERS(unsigned long, unsignedlong, "%lu")

/* Codes_SRS_UMOCKTYPES_C_01_098: [ umocktypes_stringify_longlong shall return the string representation of value. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_099: [ If value is NULL, umocktypes_stringify_longlong shall return NULL. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_101: [ If any other error occurs when creating the string representation, umocktypes_stringify_longlong shall return NULL. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_100: [ If allocating a new string to hold the string representation fails, umocktypes_stringify_longlong shall return NULL. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_102: [ umocktypes_are_equal_longlong shall compare the 2 long longs pointed to by left and right. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_103: [ If any of the arguments is NULL, umocktypes_are_equal_longlong shall return -1. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_104: [ If the values pointed to by left and right are equal, umocktypes_are_equal_longlong shall return 1. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_105: [ If the values pointed to by left and right are different, umocktypes_are_equal_longlong shall return 0. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_106: [ umocktypes_copy_longlong shall copy the long long value from source to destination. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_108: [ If source or destination are NULL, umocktypes_copy_longlong shall return a non-zero value. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_107: [ On success umocktypes_copy_longlong shall return 0. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_109: [ umocktypes_free_longlong shall do nothing. ]*/
IMPLEMENT_TYPE_HANDLERS(long long, longlong, "%lld")

/* Codes_SRS_UMOCKTYPES_C_01_110: [ umocktypes_stringify_unsignedlonglong shall return the string representation of value. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_111: [ If value is NULL, umocktypes_stringify_unsignedlonglong shall return NULL. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_113: [ If any other error occurs when creating the string representation, umocktypes_stringify_unsignedlonglong shall return NULL. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_112: [ If allocating a new string to hold the string representation fails, umocktypes_stringify_unsignedlonglong shall return NULL. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_114: [ umocktypes_are_equal_unsignedlonglong shall compare the 2 unsigned long longs pointed to by left and right. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_115: [ If any of the arguments is NULL, umocktypes_are_equal_unsignedlonglong shall return -1. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_116: [ If the values pointed to by left and right are equal, umocktypes_are_equal_unsignedlonglong shall return 1. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_117: [ If the values pointed to by left and right are different, umocktypes_are_equal_unsignedlonglong shall return 0. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_118: [ umocktypes_copy_unsignedlonglong shall copy the unsigned long long value from source to destination. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_120: [ If source or destination are NULL, umocktypes_copy_unsignedlonglong shall return a non-zero value. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_119: [ On success umocktypes_copy_unsignedlonglong shall return 0. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_121: [ umocktypes_free_unsignedlonglong shall do nothing. ]*/
IMPLEMENT_TYPE_HANDLERS(unsigned long long, unsignedlonglong, "%llu")

/* Codes_SRS_UMOCKTYPES_C_01_122: [ umocktypes_stringify_float shall return the string representation of value. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_123: [ If value is NULL, umocktypes_stringify_float shall return NULL. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_125: [ If any other error occurs when creating the string representation, umocktypes_stringify_float shall return NULL. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_124: [ If allocating a new string to hold the string representation fails, umocktypes_stringify_float shall return NULL. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_126: [ umocktypes_are_equal_float shall compare the 2 floats pointed to by left and right. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_127: [ If any of the arguments is NULL, umocktypes_are_equal_float shall return -1. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_128: [ If the values pointed to by left and right are equal, umocktypes_are_equal_float shall return 1. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_129: [ If the values pointed to by left and right are different, umocktypes_are_equal_float shall return 0. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_130: [ umocktypes_copy_float shall copy the float value from source to destination. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_132: [ If source or destination are NULL, umocktypes_copy_float shall return a non-zero value. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_131: [ On success umocktypes_copy_float shall return 0. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_133: [ umocktypes_free_float shall do nothing. ]*/
IMPLEMENT_TYPE_HANDLERS(float, float, "%f")

/* Codes_SRS_UMOCKTYPES_C_01_134: [ umocktypes_stringify_double shall return the string representation of value. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_135: [ If value is NULL, umocktypes_stringify_double shall return NULL. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_137: [ If any other error occurs when creating the string representation, umocktypes_stringify_double shall return NULL. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_136: [ If allocating a new string to hold the string representation fails, umocktypes_stringify_double shall return NULL. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_138: [ umocktypes_are_equal_double shall compare the 2 doubles pointed to by left and right. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_139: [ If any of the arguments is NULL, umocktypes_are_equal_double shall return -1. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_140: [ If the values pointed to by left and right are equal, umocktypes_are_equal_double shall return 1. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_141: [ If the values pointed to by left and right are different, umocktypes_are_equal_double shall return 0. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_142: [ umocktypes_copy_double shall copy the double value from source to destination. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_144: [ If source or destination are NULL, umocktypes_copy_double shall return a non-zero value. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_143: [ On success umocktypes_copy_double shall return 0. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_145: [ umocktypes_free_double shall do nothing. ]*/
IMPLEMENT_TYPE_HANDLERS(double, double, "%f")

/* Codes_SRS_UMOCKTYPES_C_01_146: [ umocktypes_stringify_longdouble shall return the string representation of value. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_147: [ If value is NULL, umocktypes_stringify_longdouble shall return NULL. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_149: [ If any other error occurs when creating the string representation, umocktypes_stringify_longdouble shall return NULL. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_148: [ If allocating a new string to hold the string representation fails, umocktypes_stringify_longdouble shall return NULL. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_150: [ umocktypes_are_equal_longdouble shall compare the 2 long doubles pointed to by left and right. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_151: [ If any of the arguments is NULL, umocktypes_are_equal_longdouble shall return -1. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_152: [ If the values pointed to by left and right are equal, umocktypes_are_equal_longdouble shall return 1. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_153: [ If the values pointed to by left and right are different, umocktypes_are_equal_longdouble shall return 0. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_154: [ umocktypes_copy_longdouble shall copy the long double value from source to destination. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_156: [ If source or destination are NULL, umocktypes_copy_longdouble shall return a non-zero value. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_155: [ On success umocktypes_copy_longdouble shall return 0. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_157: [ umocktypes_free_longdouble shall do nothing. ]*/
IMPLEMENT_TYPE_HANDLERS(long double, longdouble, "%Lf")

/* Codes_SRS_UMOCKTYPES_C_01_158: [ umocktypes_stringify_size_t shall return the string representation of value. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_159: [ If value is NULL, umocktypes_stringify_size_t shall return NULL. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_161: [ If any other error occurs when creating the string representation, umocktypes_stringify_size_t shall return NULL. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_160: [ If allocating a new string to hold the string representation fails, umocktypes_stringify_size_t shall return NULL. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_162: [ umocktypes_are_equal_size_t shall compare the 2 size_ts pointed to by left and right. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_163: [ If any of the arguments is NULL, umocktypes_are_equal_size_t shall return -1. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_164: [ If the values pointed to by left and right are equal, umocktypes_are_equal_size_t shall return 1. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_165: [ If the values pointed to by left and right are different, umocktypes_are_equal_size_t shall return 0. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_166: [ umocktypes_copy_size_t shall copy the size_t value from source to destination. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_168: [ If source or destination are NULL, umocktypes_copy_size_t shall return a non-zero value. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_167: [ On success umocktypes_copy_size_t shall return 0. ]*/
/* Codes_SRS_UMOCKTYPES_C_01_169: [ umocktypes_free_size_t shall do nothing. ]*/
IMPLEMENT_TYPE_HANDLERS(size_t, size_t, "%zu")

IMPLEMENT_TYPE_HANDLERS(void*, void_ptr, "%p")

int umocktypes_c_register_types(void)
{
    int result;

    /* Codes_SRS_UMOCKTYPES_C_01_001: [ umocktypes_c_register_types shall register support for all the types in the module. ]*/
    if ((REGISTER_TYPE(char, char) != 0) ||
        (REGISTER_TYPE(unsigned char, unsignedchar) != 0) ||
        (REGISTER_TYPE(short, short) != 0) ||
        (REGISTER_TYPE(unsigned short, unsignedshort) != 0) ||
        (REGISTER_TYPE(int, int) != 0) ||
        (REGISTER_TYPE(unsigned int, unsignedint) != 0) ||
        (REGISTER_TYPE(long, long) != 0) ||
        (REGISTER_TYPE(unsigned long, unsignedlong) != 0) ||
        (REGISTER_TYPE(long long, longlong) != 0) ||
        (REGISTER_TYPE(unsigned long long, unsignedlonglong) != 0) ||
        (REGISTER_TYPE(float, float) != 0) ||
        (REGISTER_TYPE(double, double) != 0) ||
        (REGISTER_TYPE(long double, longdouble) != 0) ||
        (REGISTER_TYPE(size_t, size_t) != 0) ||
        (REGISTER_TYPE(void*, void_ptr) != 0) ||
        (REGISTER_TYPE(const void*, void_ptr) != 0))
    {
        /* Codes_SRS_UMOCKTYPES_C_01_171: [ If registering any of the types fails, umocktypes_c_register_types shall fail and return a non-zero value. ]*/
        UMOCK_LOG("umocktypes_c_register_types: Failed registering types."); \
        result = __LINE__;
    }
    else
    {
        /* Codes_SRS_UMOCKTYPES_C_01_170: [ On success, umocktypes_c_register_types shall return 0. ]*/
        result = 0;
    }

    return result;
}
