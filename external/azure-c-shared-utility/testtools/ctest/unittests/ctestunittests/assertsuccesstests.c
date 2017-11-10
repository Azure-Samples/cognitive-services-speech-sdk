// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "ctest.h"

CTEST_BEGIN_TEST_SUITE(AssertSuccessTests)

typedef struct mystruct_tag
{
    unsigned char x;
} mystruct;

CTEST_COMPARE(mystruct_ptr, mystruct*)
{
    return (left->x != right->x);
}

CTEST_TO_STRING(mystruct_ptr, mystruct*, string, bufferSize, value)
{
    (void)snprintf(string, bufferSize, "{ %d }", (int)value->x);
}

/* CTEST_ASSERT_ARE_EQUAL */

CTEST_FUNCTION(Assert_Are_Equal_2_Ints_Succeeds)
{
    CTEST_ASSERT_ARE_EQUAL(int, 1, 1);
}

CTEST_FUNCTION(Assert_Are_Equal_2_Chars_Succeeds)
{
    CTEST_ASSERT_ARE_EQUAL(char, (char)1, (char)1);
}

CTEST_FUNCTION(Assert_Are_Equal_2_Shorts_Succeeds)
{
    CTEST_ASSERT_ARE_EQUAL(short, (short)1, (short)1);
}

CTEST_FUNCTION(Assert_Are_Equal_2_Longs_Succeeds)
{
    CTEST_ASSERT_ARE_EQUAL(long, (long)1, (long)1);
}

CTEST_FUNCTION(Assert_Are_Equal_2_Unsigned_Longs_Succeeds)
{
    CTEST_ASSERT_ARE_EQUAL(unsigned_long, 1UL, 1UL);
}

CTEST_FUNCTION(Assert_Are_Equal_2_uint8_t_Succeeds)
{
    CTEST_ASSERT_ARE_EQUAL(uint8_t, (uint8_t)1, (uint8_t)1);
}

CTEST_FUNCTION(Assert_Are_Equal_2_int8_t_Succeeds)
{
    CTEST_ASSERT_ARE_EQUAL(int8_t, (int8_t)1, (int8_t)1);
}

CTEST_FUNCTION(Assert_Are_Equal_2_uint16_t_Succeeds)
{
    CTEST_ASSERT_ARE_EQUAL(uint16_t, (uint16_t)1, (uint16_t)1);
}

CTEST_FUNCTION(Assert_Are_Equal_2_int16_t_Succeeds)
{
    CTEST_ASSERT_ARE_EQUAL(int16_t, (int16_t)1, (int16_t)1);
}

CTEST_FUNCTION(Assert_Are_Equal_2_uint32_t_Succeeds)
{
    CTEST_ASSERT_ARE_EQUAL(uint32_t, (uint32_t)1, (uint32_t)1);
}

CTEST_FUNCTION(Assert_Are_Equal_2_int32_t_Succeeds)
{
    CTEST_ASSERT_ARE_EQUAL(int32_t, (int32_t)1, (int32_t)1);
}

CTEST_FUNCTION(Assert_Are_Equal_2_uint64_t_Succeeds)
{
    CTEST_ASSERT_ARE_EQUAL(uint64_t, (uint64_t)1, (uint64_t)1);
}

CTEST_FUNCTION(Assert_Are_Equal_2_int64_t_Succeeds)
{
    CTEST_ASSERT_ARE_EQUAL(int64_t, (int64_t)1, (int64_t)1);
}

CTEST_FUNCTION(Assert_Are_Equal_2_strings_With_To_String_Type_Succeeds)
{
    CTEST_ASSERT_ARE_EQUAL(char_ptr, "dan", "dan");
}

CTEST_FUNCTION(Assert_Are_Equal_2_strings_With_Same_Address_Succeeds)
{
    char c1;
    CTEST_ASSERT_ARE_EQUAL(void_ptr, &c1, &c1);
}


CTEST_FUNCTION(Assert_Are_Equal_2_Structures_Succeeds)
{
    mystruct a = { 1 };
    mystruct b = { 1 };

    CTEST_ASSERT_ARE_EQUAL(mystruct_ptr, &a, &b);
}

/* CTEST_ASSERT_ARE_EQUAL_WITH_MSG */

CTEST_FUNCTION(Assert_Are_Equal_With_Msg_2_Ints_Succeeds)
{
    CTEST_ASSERT_ARE_EQUAL(int, 1, 1, "Test message");
}

CTEST_FUNCTION(Assert_Are_Equal_With_Msg_2_Chars_Succeeds)
{
    CTEST_ASSERT_ARE_EQUAL(char, (char)1, (char)1, "Test message");
}

CTEST_FUNCTION(Assert_Are_Equal_With_Msg_2_Shorts_Succeeds)
{
    CTEST_ASSERT_ARE_EQUAL(short, (short)1, (short)1, "Test message");
}

CTEST_FUNCTION(Assert_Are_Equal_With_Msg_2_Longs_Succeeds)
{
    CTEST_ASSERT_ARE_EQUAL(long, (long)1, (long)1, "Test message");
}

CTEST_FUNCTION(Assert_Are_Equal_With_Msg_2_Unsigned_Longs_Succeeds)
{
    CTEST_ASSERT_ARE_EQUAL(unsigned_long, 1UL, 1UL, "Test message");
}

CTEST_FUNCTION(Assert_Are_Equal_With_Msg_2_uint8_t_Succeeds)
{
    CTEST_ASSERT_ARE_EQUAL(uint8_t, (uint8_t)1, (uint8_t)1, "Test message");
}

CTEST_FUNCTION(Assert_Are_Equal_With_Msg_2_int8_t_Succeeds)
{
    CTEST_ASSERT_ARE_EQUAL(int8_t, (int8_t)1, (int8_t)1, "Test message");
}

CTEST_FUNCTION(Assert_Are_Equal_With_Msg_2_uint16_t_Succeeds)
{
    CTEST_ASSERT_ARE_EQUAL(uint16_t, (uint16_t)1, (uint16_t)1, "Test message");
}

CTEST_FUNCTION(Assert_Are_Equal_With_Msg_2_int16_t_Succeeds)
{
    CTEST_ASSERT_ARE_EQUAL(int16_t, (int16_t)1, (int16_t)1, "Test message");
}

CTEST_FUNCTION(Assert_Are_Equal_With_Msg_2_uint32_t_Succeeds)
{
    CTEST_ASSERT_ARE_EQUAL(uint32_t, (uint32_t)1, (uint32_t)1, "Test message");
}

CTEST_FUNCTION(Assert_Are_Equal_With_Msg_2_int32_t_Succeeds)
{
    CTEST_ASSERT_ARE_EQUAL(int32_t, (int32_t)1, (int32_t)1, "Test message");
}

CTEST_FUNCTION(Assert_Are_Equal_With_Msg_2_uint64_t_Succeeds)
{
    CTEST_ASSERT_ARE_EQUAL(uint64_t, (uint64_t)1, (uint64_t)1, "Test message");
}

CTEST_FUNCTION(Assert_Are_Equal_With_Msg_2_int64_t_Succeeds)
{
    CTEST_ASSERT_ARE_EQUAL(int64_t, (int64_t)1, (int64_t)1, "Test message");
}

CTEST_FUNCTION(Assert_Are_Equal_With_Msg_2_strings_With_To_String_Type_Succeeds)
{
    CTEST_ASSERT_ARE_EQUAL(char_ptr, "dan", "dan", "Test message");
}

CTEST_FUNCTION(Assert_Are_Equal_With_Msg_2_void_ptr_same_address_Succeeds)
{
    char c1;
    CTEST_ASSERT_ARE_EQUAL(void_ptr, &c1, &c1, "Test message");
}

CTEST_FUNCTION(Assert_Are_Equal_With_Msg_2_Structures_Succeeds)
{
    mystruct a = { 1 };
    mystruct b = { 1 };

    CTEST_ASSERT_ARE_EQUAL(mystruct_ptr, &a, &b, "Test message");
}

/* CTEST_ASSERT_ARE_NOT_EQUAL */

CTEST_FUNCTION(Assert_Are_Not_Equal_2_Ints_Succeeds)
{
    CTEST_ASSERT_ARE_NOT_EQUAL(int, 0, 1);
}

CTEST_FUNCTION(Assert_Are_Not_Equal_2_Chars_Succeeds)
{
    CTEST_ASSERT_ARE_NOT_EQUAL(char, (char)0, (char)1);
}

CTEST_FUNCTION(Assert_Are_Not_Equal_2_Shorts_Succeeds)
{
    CTEST_ASSERT_ARE_NOT_EQUAL(short, (short)0, (short)1);
}

CTEST_FUNCTION(Assert_Are_Not_Equal_2_Longs_Succeeds)
{
    CTEST_ASSERT_ARE_NOT_EQUAL(long, (long)0, (long)1);
}

#if defined CTEST_USE_STDINT

CTEST_FUNCTION(Assert_Are_Not_Equal_2_uint8_t_Succeeds)
{
    CTEST_ASSERT_ARE_NOT_EQUAL(uint8_t, (uint8_t)0, (uint8_t)1);
}

CTEST_FUNCTION(Assert_Are_Not_Equal_2_int8_t_Succeeds)
{
    CTEST_ASSERT_ARE_NOT_EQUAL(int8_t, (int8_t)0, (int8_t)1);
}

CTEST_FUNCTION(Assert_Are_Not_Equal_2_uint16_t_Succeeds)
{
    CTEST_ASSERT_ARE_NOT_EQUAL(uint16_t, (uint16_t)0, (uint16_t)1);
}

CTEST_FUNCTION(Assert_Are_Not_Equal_2_int16_t_Succeeds)
{
    CTEST_ASSERT_ARE_NOT_EQUAL(int16_t, (int16_t)0, (int16_t)1);
}

CTEST_FUNCTION(Assert_Are_Not_Equal_2_uint32_t_Succeeds)
{
    CTEST_ASSERT_ARE_NOT_EQUAL(uint32_t, (uint32_t)0, (uint32_t)1);
}

CTEST_FUNCTION(Assert_Are_Not_Equal_2_int32_t_Succeeds)
{
    CTEST_ASSERT_ARE_NOT_EQUAL(int32_t, (int32_t)0, (int32_t)1);
}

CTEST_FUNCTION(Assert_Are_Not_Equal_2_uint64_t_Succeeds)
{
    CTEST_ASSERT_ARE_NOT_EQUAL(uint64_t, (uint64_t)0, (uint64_t)1);
}

CTEST_FUNCTION(Assert_Are_Not_Equal_2_int64_t_Succeeds)
{
    CTEST_ASSERT_ARE_NOT_EQUAL(int64_t, (int64_t)0, (int64_t)1);
}

#endif

CTEST_FUNCTION(Assert_Are_Not_Equal_2_strings_With_To_String_Type_Succeeds)
{
    CTEST_ASSERT_ARE_NOT_EQUAL(char_ptr, "test", "dan");
}

CTEST_FUNCTION(Assert_Are_Not_Equal_2_void_ptr_different_address_Succeeds)
{
    int c1, c2;
    CTEST_ASSERT_ARE_NOT_EQUAL(void_ptr, &c1, &c2);
}

CTEST_FUNCTION(Assert_Are_Not_Equal_2_Structures_Succeeds)
{
    mystruct a = { 1 };
    mystruct b = { 42 };

    CTEST_ASSERT_ARE_NOT_EQUAL(mystruct_ptr, &a, &b);
}

/* CTEST_ASSERT_ARE_NOT_EQUAL_WITH_MSG */

CTEST_FUNCTION(Assert_Are_Not_Equal_With_Msg_2_Ints_Succeeds)
{
    CTEST_ASSERT_ARE_NOT_EQUAL(int, 0, 1, "Test message");
}

CTEST_FUNCTION(Assert_Are_Not_Equal_With_Msg_2_Chars_Succeeds)
{
    CTEST_ASSERT_ARE_NOT_EQUAL(char, (char)0, (char)1, "Test message");
}

CTEST_FUNCTION(Assert_Are_Not_Equal_With_Msg_2_Shorts_Succeeds)
{
    CTEST_ASSERT_ARE_NOT_EQUAL(short, (short)0, (short)1, "Test message");
}

CTEST_FUNCTION(Assert_Are_Not_Equal_With_Msg_2_Longs_Succeeds)
{
    CTEST_ASSERT_ARE_NOT_EQUAL(long, (long)0, (long)1, "Test message");
}

#if defined CTEST_USE_STDINT

CTEST_FUNCTION(Assert_Are_Not_Equal_With_Msg_2_uint8_t_Succeeds)
{
    CTEST_ASSERT_ARE_NOT_EQUAL(uint8_t, (uint8_t)0, (uint8_t)1, "Test message");
}

CTEST_FUNCTION(Assert_Are_Not_Equal_With_Msg_2_int8_t_Succeeds)
{
    CTEST_ASSERT_ARE_NOT_EQUAL(int8_t, (int8_t)0, (int8_t)1, "Test message");
}

CTEST_FUNCTION(Assert_Are_Not_Equal_With_Msg_2_uint16_t_Succeeds)
{
    CTEST_ASSERT_ARE_NOT_EQUAL(uint16_t, (uint16_t)0, (uint16_t)1, "Test message");
}

CTEST_FUNCTION(Assert_Are_Not_Equal_With_Msg_2_int16_t_Succeeds)
{
    CTEST_ASSERT_ARE_NOT_EQUAL(int16_t, (int16_t)0, (int16_t)1, "Test message");
}

CTEST_FUNCTION(Assert_Are_Not_Equal_With_Msg_2_uint32_t_Succeeds)
{
    CTEST_ASSERT_ARE_NOT_EQUAL(uint32_t, (uint32_t)0, (uint32_t)1, "Test message");
}

CTEST_FUNCTION(Assert_Are_Not_Equal_With_Msg_2_int32_t_Succeeds)
{
    CTEST_ASSERT_ARE_NOT_EQUAL(int32_t, (int32_t)0, (int32_t)1, "Test message");
}

CTEST_FUNCTION(Assert_Are_Not_Equal_With_Msg_2_uint64_t_Succeeds)
{
    CTEST_ASSERT_ARE_NOT_EQUAL(uint64_t, (uint64_t)0, (uint64_t)1, "Test message");
}

CTEST_FUNCTION(Assert_Are_Not_Equal_With_Msg_2_int64_t_Succeeds)
{
    CTEST_ASSERT_ARE_NOT_EQUAL(int64_t, (int64_t)0, (int64_t)1, "Test message");
}

#endif

CTEST_FUNCTION(Assert_Are_Not_Equal_With_Msg_2_strings_With_To_String_Type_Succeeds)
{
    CTEST_ASSERT_ARE_NOT_EQUAL(char_ptr, "test", "dan", "Test message");
}

CTEST_FUNCTION(Assert_Are_Not_Equal_With_Msg_2_void_ptr_with_different_addresses_Succeeds)
{
    double b1, b2;
    CTEST_ASSERT_ARE_NOT_EQUAL(void_ptr, &b1, &b2, "Test message");
}

CTEST_FUNCTION(Assert_Are_Not_Equal_With_Msg_2_Structures_Succeeds)
{
    mystruct a = { 1 };
    mystruct b = { 42 };

    CTEST_ASSERT_ARE_NOT_EQUAL(mystruct_ptr, &a, &b, "Test message");
}

/* CTEST_ASSERT_IS_NULL */

CTEST_FUNCTION(Assert_Is_NULL_Succeeds)
{
    CTEST_ASSERT_IS_NULL(NULL);
}

/* CTEST_ASSERT_IS_NULL_WITH_MSG */

CTEST_FUNCTION(Assert_Is_NULL_With_Msg_Succeeds)
{
    CTEST_ASSERT_IS_NULL(NULL, "Test message");
}

/* CTEST_ASSERT_IS_NOT_NULL */

CTEST_FUNCTION(Assert_Is_Not_NULL_Succeeds)
{
    int a;
    CTEST_ASSERT_IS_NOT_NULL(&a);
}

/* CTEST_ASSERT_IS_NOT_NULL_WITH_MSG */

CTEST_FUNCTION(Assert_Is_Not_NULL_With_Msg_Succeeds)
{
    int a;
    CTEST_ASSERT_IS_NOT_NULL(&a, "Test message");
}

/* CTEST_ASSERT_IS_TRUE */

CTEST_FUNCTION(Assert_Is_True_Succeeds)
{
    CTEST_ASSERT_IS_TRUE(1);
}

/* CTEST_ASSERT_IS_TRUE_WITH_MSG */

CTEST_FUNCTION(Assert_Is_True_With_Msg_Succeeds)
{
    CTEST_ASSERT_IS_TRUE(1, "Test message");
}

/* CTEST_ASSERT_IS_FALSE */

CTEST_FUNCTION(Assert_Is_False_Succeeds)
{
    CTEST_ASSERT_IS_FALSE(0);
}

/* CTEST_ASSERT_IS_FALSE_WITH_MSG */

CTEST_FUNCTION(Assert_Is_False_With_Msg_Succeeds)
{
    CTEST_ASSERT_IS_FALSE(0, "Test message");
}

CTEST_END_TEST_SUITE(AssertSuccessTests)
