// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef UMOCKTYPES_H
#define UMOCKTYPES_H

#ifdef __cplusplus
#include <cstddef>
extern "C" {
#else
#include <stddef.h>
#endif

    typedef char*(*UMOCKTYPE_STRINGIFY_FUNC)(const void* value);
    typedef int(*UMOCKTYPE_COPY_FUNC)(void* destination, const void* source);
    typedef void(*UMOCKTYPE_FREE_FUNC)(void* value);
    typedef int(*UMOCKTYPE_ARE_EQUAL_FUNC)(const void* left, const void* right);

    extern int umocktypes_init(void);
    extern void umocktypes_deinit(void);
    extern int umocktypes_register_type(const char* type, UMOCKTYPE_STRINGIFY_FUNC stringify_func, UMOCKTYPE_ARE_EQUAL_FUNC are_equal_func, UMOCKTYPE_COPY_FUNC copy_func, UMOCKTYPE_FREE_FUNC free_func);
    extern int umocktypes_register_alias_type(const char* type, const char* alias_type);

    extern char* umocktypes_stringify(const char* type, const void* value);
    extern int umocktypes_are_equal(const char* type, const void* left, const void* right);
    extern int umocktypes_copy(const char* type, void* destination, const void* source);
    extern void umocktypes_free(const char* type, void* value);

    /* This is a convenience macro that allows registering a type by simply specifying the name and a function_postfix*/
#define REGISTER_TYPE(type, function_postfix) \
        umocktypes_register_type(TOSTRING(type), (UMOCKTYPE_STRINGIFY_FUNC)C2(umocktypes_stringify_, function_postfix), \
            (UMOCKTYPE_ARE_EQUAL_FUNC)C2(umocktypes_are_equal_,function_postfix), \
            (UMOCKTYPE_COPY_FUNC)C2(umocktypes_copy_,function_postfix), \
            (UMOCKTYPE_FREE_FUNC)C2(umocktypes_free_,function_postfix))

/* Codes_SRS_UMOCK_C_LIB_01_181: [ If a value that is not part of the enum is used, it shall be treated as an int value. ]*/
#define IMPLEMENT_UMOCK_C_ENUM_STRINGIFY(type, ...) \
    char* C2(umocktypes_stringify_,type)(const type* value) \
    { \
        char* result; \
        static const char *C2(enum_name,_strings)[]= \
        { \
            FOR_EACH_1(DEFINE_ENUMERATION_CONSTANT_AS_STRING, __VA_ARGS__) \
        }; \
        if (value == NULL) \
        { \
            result = NULL; \
        } \
        else \
        { \
            if (*value < sizeof(C2(enum_name,_strings)) / sizeof(C2(enum_name,_strings)[0])) \
            { \
                size_t length = strlen(C2(enum_name_, strings)[*value]); \
                if (length < 0) \
                { \
                    result = NULL; \
                } \
                else \
                { \
                    result = (char*)malloc(length + 1); \
                    if (result != NULL) \
                    { \
                        (void)memcpy(result, C2(enum_name_, strings)[*value], length + 1); \
                    } \
                } \
            } \
            else \
            { \
                result = (char*)malloc(64); \
                if (result != NULL) \
                { \
                    (void)sprintf(result, "%d", (int)*value); \
                } \
            } \
        } \
        return result; \
    }

#define IMPLEMENT_UMOCK_C_ENUM_ARE_EQUAL(type) \
    int C2(umocktypes_are_equal_,type)(const type* left, const type* right) \
    { \
        int result; \
        if ((left == NULL) || (right == NULL)) \
        { \
            result = -1; \
        } \
        else \
        { \
            result = ((*left) == (*right)) ? 1 : 0; \
        } \
        return result; \
    }

#define IMPLEMENT_UMOCK_C_ENUM_COPY(type) \
    int C2(umocktypes_copy_,type)(type* destination, const type* source) \
    { \
        int result; \
        if ((destination == NULL) || \
            (source == NULL)) \
        { \
            result = __LINE__; \
        } \
        else \
        { \
            *destination = *source; \
            result = 0; \
        } \
        return result; \
    }

#define IMPLEMENT_UMOCK_C_ENUM_FREE(type) \
    void C2(umocktypes_free_,type)(type* value) \
    { \
    }

/* Codes_SRS_UMOCK_C_LIB_01_179: [ IMPLEMENT_UMOCK_C_ENUM_TYPE shall implement umock_c handlers for an enum type. ]*/
/* Codes_SRS_UMOCK_C_LIB_01_180: [ The variable arguments are a list making up the enum values. ]*/
#define IMPLEMENT_UMOCK_C_ENUM_TYPE(type, ...) \
    IMPLEMENT_UMOCK_C_ENUM_STRINGIFY(type, __VA_ARGS__) \
    IMPLEMENT_UMOCK_C_ENUM_ARE_EQUAL(type) \
    IMPLEMENT_UMOCK_C_ENUM_COPY(type) \
    IMPLEMENT_UMOCK_C_ENUM_FREE(type)

#ifdef __cplusplus
}
#endif

#endif /* UMOCKTYPES_H */
