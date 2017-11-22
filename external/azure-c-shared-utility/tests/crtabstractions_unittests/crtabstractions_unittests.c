// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include <limits.h>

#include "testrunnerswitcher.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "errno.h"

#ifdef _MSC_VER
#pragma warning(disable:4505)
#endif

#if defined _MSC_VER
#include "crtdbg.h"
static _invalid_parameter_handler oldInvalidParameterHandler;
static int oldReportType;

static void my_invalid_parameter_handler(
    const wchar_t * expression,
    const wchar_t * function,
    const wchar_t * file,
    unsigned int line,
    uintptr_t pReserved
    )
{
    (void)expression;
    (void)function;
    (void)file;
    (void)line;
    (void)pReserved;
    /*do nothing*/
}
/* The below defines are because on Windows platform, the secure version of the CRT functions will invoke WATSON if no invalid parameter handler provided by the user */
#define HOOK_INVALID_PARAMETER_HANDLER() {oldInvalidParameterHandler = _set_invalid_parameter_handler(my_invalid_parameter_handler);oldReportType=_CrtSetReportMode(_CRT_ASSERT, 0);}
#define UNHOOK_INVALID_PARAMETER_HANDLER() {(void)_CrtSetReportMode(_CRT_ASSERT, oldReportType); (void)_set_invalid_parameter_handler(oldInvalidParameterHandler);}

#else /* _MSC_VER */
#define HOOK_INVALID_PARAMETER_HANDLER() do{}while(0)
#define UNHOOK_INVALID_PARAMETER_HANDLER() do{}while(0)
#endif /* _MSC_VER */

static const unsigned int interestingUnsignedIntNumbersToBeConverted[] =
{
    0,
    1,
    2,
    3,
    4,
    5,
    6,
    7,
    8,
    9,
    10,
    21,
    32,
    43,
    54,
    65,
    76,
    87,
    98,
    123,
    1234,
    12341,
    UINT_MAX / 2,
    UINT_MAX -1, 
    UINT_MAX,
    42,
    0x42
};

#ifndef SIZE_MAX
#define SIZE_MAX ((size_t)~(size_t)0)
#endif

static const size_t interestingSize_tNumbersToBeConverted[] =
{
    0,
    1,
    2,
    3,
    4,
    5,
    6,
    7,
    8,
    9,
    10,
    21,
    32,
    43,
    54,
    65,
    76,
    87,
    98,
    123,
    1234,
    12341,
    SIZE_MAX / 2,
    SIZE_MAX -1, 
    SIZE_MAX,
    42,
    0x42
};

static TEST_MUTEX_HANDLE g_dllByDll;

BEGIN_TEST_SUITE(CRTAbstractions_UnitTests)

TEST_SUITE_INITIALIZE(a)
{
    TEST_INITIALIZE_MEMORY_DEBUG(g_dllByDll);
}

TEST_SUITE_CLEANUP(b)
{
    TEST_DEINITIALIZE_MEMORY_DEBUG(g_dllByDll);
}

        /* strcat_s */

        // Tests_SRS_CRT_ABSTRACTIONS_99_008: [strcat_s shall append the src to dst and terminates the resulting string with a null character.]
        // Tests_SRS_CRT_ABSTRACTIONS_99_009: [The initial character of src shall overwrite the terminating null character of dst.]
        // Tests_SRS_CRT_ABSTRACTIONS_99_003: [strcat_s shall return Zero upon success.]
        TEST_FUNCTION(strcat_s_Appends_Source_To_Destination)
        {
            // arrange
            char dstString[128] = "Destination";
            size_t dstSizeInBytes = sizeof(dstString);
            char srcString[] = "Source";
            int result;

            // act
            result = strcat_s(dstString, dstSizeInBytes, srcString);

            // assert
            ASSERT_ARE_EQUAL(char_ptr, "DestinationSource", dstString);
            ASSERT_ARE_EQUAL(int, 0, result);
        }

        TEST_FUNCTION(strcat_s_Appends_Empty_Source_To_Destination)
        {
            // arrange
            char dstString[128] = "Destination";
            size_t dstSizeInBytes = sizeof(dstString);
            char srcString[] = "";
            int result;

            // act
            result = strcat_s(dstString, dstSizeInBytes, srcString);

            // assert
            ASSERT_ARE_EQUAL(char_ptr, "Destination", dstString);
            ASSERT_ARE_EQUAL(int, 0, result);
        }

        TEST_FUNCTION(strcat_s_Appends_Source_To_Empty_Destination)
        {
            // arrange
            char dstString[128] = "";
            size_t dstSizeInBytes = sizeof(dstString);
            char srcString[] = "Source";
            int result;

            // act
            result = strcat_s(dstString, dstSizeInBytes, srcString);

            // assert
            ASSERT_ARE_EQUAL(char_ptr, "Source", dstString);
            ASSERT_ARE_EQUAL(int, 0, result);
        }

        TEST_FUNCTION(strcat_s_Appends_Empty_Source_To_Empty_Destination)
        {
            // arrange
            char dstString[128] = "";
            size_t dstSizeInBytes = sizeof(dstString);
            char srcString[] = "";
            int result;

            // act
            result = strcat_s(dstString, dstSizeInBytes, srcString);

            // assert
            ASSERT_ARE_EQUAL(char_ptr, "", dstString);
            ASSERT_ARE_EQUAL(int, 0, result);
        }

        // Tests_SRS_CRT_ABSTRACTIONS_99_004: [If dst is NULL or unterminated, the error code returned shall be EINVAL & dst shall not be modified.]
        TEST_FUNCTION(strcat_s_With_NULL_Destination_Fails)
        {
            // arrange
            char* dstString = NULL;
            size_t sizeInBytes = sizeof(dstString);
            char srcString[] = "Source";
            int result;

            // act
            HOOK_INVALID_PARAMETER_HANDLER();
#ifdef _MSC_VER
#pragma warning(suppress: 6387) /* This is test code, explictly calling with NULL argument */
#endif
            result = strcat_s(dstString, sizeInBytes, srcString);
            UNHOOK_INVALID_PARAMETER_HANDLER();

            // assert
            ASSERT_IS_NULL(dstString);
            ASSERT_ARE_EQUAL(int, EINVAL, result);
        }

        TEST_FUNCTION(strcat_s_With_Unterminated_Destination_Fails)
        {
            // arrange
            char dstString[128];
            size_t dstSizeInBytes = sizeof(dstString);
            char srcString[] = "Source";
            int result;
            for (size_t i = 0; i < dstSizeInBytes; i++)
            {
                dstString[i] = 'z';
            }

            // act
            HOOK_INVALID_PARAMETER_HANDLER();
            result = strcat_s(dstString, dstSizeInBytes, srcString);
            UNHOOK_INVALID_PARAMETER_HANDLER();

            // assert
#ifndef _MSC_VER    /* MSDN claims that content of destination buffer is not modified, but that is not true. Filing bug. */
            for (size_t i = 0; i < dstSizeInBytes; i++)
            {
                ASSERT_ARE_EQUAL(char, 'z', dstString[i]);
            }
#endif
            ASSERT_ARE_EQUAL(int, EINVAL, result);
        }
        
        // Tests_SRS_CRT_ABSTRACTIONS_99_005: [If src is NULL, the error code returned shall be EINVAL and dst[0] shall be set to 0.]
        TEST_FUNCTION(strcat_s_With_NULL_Source_Fails)
        {
            // arrange
            char dstString[128] = "Source";
            size_t dstSizeInBytes = sizeof(dstString);
            char* srcString = NULL;
            int result;

            // act
            HOOK_INVALID_PARAMETER_HANDLER();
#ifdef _MSC_VER
#pragma warning(suppress: 6387) /* This is test code, explictly calling with NULL argument */
#endif
            result = strcat_s(dstString, dstSizeInBytes, srcString);
            UNHOOK_INVALID_PARAMETER_HANDLER();

            // assert
            ASSERT_ARE_EQUAL(char,'\0', dstString[0]);
            ASSERT_ARE_EQUAL(int, EINVAL, result);
        }

        // Tests_SRS_CRT_ABSTRACTIONS_99_006: [If the dstSizeInBytes is 0 or smaller than the required size for dst & src, the error code returned shall be ERANGE & dst[0] set to 0.]
        TEST_FUNCTION(strcat_s_With_dstSizeInBytes_Equals_Zero_Fails)
        {
            // arrange
            char dstString[128] = "Destination";
            size_t dstSizeInBytes = sizeof(dstString);
            char srcString[] = "Source";
            int result;

            // act
            dstSizeInBytes = 0;
            HOOK_INVALID_PARAMETER_HANDLER();
            result = strcat_s(dstString, dstSizeInBytes, srcString);
            UNHOOK_INVALID_PARAMETER_HANDLER();

            // assert
#ifdef _MSC_VER /*MSDN Claims that destination buffer would be set to empty & ERANGE is the error, but not the case.  Filing bug.*/
            ASSERT_ARE_EQUAL(char_ptr, "Destination", dstString);
            ASSERT_ARE_EQUAL(int, EINVAL, result);
#else
            ASSERT_ARE_EQUAL(char, '\0', dstString[0]);
            ASSERT_ARE_EQUAL(int, ERANGE, result);
#endif
        }

        TEST_FUNCTION(strcat_s_With_dstSizeInBytes_Smaller_Than_dst_and_src_Fails)
        {
            // arrange
            char dstString[128] = "Destination";
            size_t dstSizeInBytes = sizeof(dstString);
            char srcString[] = "Source";
            int result;

            // act
            HOOK_INVALID_PARAMETER_HANDLER();
            dstSizeInBytes = strlen(dstString) + (strlen(srcString) - 3);
            result = strcat_s(dstString, dstSizeInBytes, srcString);
            UNHOOK_INVALID_PARAMETER_HANDLER();

            // assert
            ASSERT_ARE_EQUAL(char,'\0', dstString[0]);
            ASSERT_ARE_EQUAL(int, ERANGE, result);
        }

        /* strcpy_s */

        // Tests_SRS_CRT_ABSTRACTIONS_99_016: [strcpy_s shall copy the contents in the address of src, including the terminating null character, to the location that's specified by dst.]
        // Tests_SRS_CRT_ABSTRACTIONS_99_011 : [strcpy_s shall return Zero upon success]
        TEST_FUNCTION(strcpy_s_copies_Source_into_Destination)
        {
            // arrange
            char dstString[128] = "Destination";
            size_t dstSizeInBytes = sizeof(dstString);
            char srcString[] = "Source";
            int result = 0;

            // act
            result = strcpy_s(dstString, dstSizeInBytes, srcString);

            // assert
            ASSERT_ARE_EQUAL(char_ptr, "Source", dstString);
            ASSERT_ARE_EQUAL(int, 0, result);
        }

        TEST_FUNCTION(strcpy_s_copies_Empty_Source_into_Destination)
        {
            // arrange
            char dstString[128] = "Destination";
            size_t dstSizeInBytes = sizeof(dstString);
            char srcString[] = "";
            int result = 0;

            // act
            result = strcpy_s(dstString, dstSizeInBytes, srcString);

            // assert
            ASSERT_ARE_EQUAL(char_ptr, "", dstString);
            ASSERT_ARE_EQUAL(int, 0, result);
        }

        TEST_FUNCTION(strcpy_s_copies_Source_into_Empty_Destination)
        {
            // arrange
            char dstString[128] = "";
            size_t dstSizeInBytes = sizeof(dstString);
            char srcString[] = "Source";
            int result = 0;

            // act
            result = strcpy_s(dstString, dstSizeInBytes, srcString);

            // assert
            ASSERT_ARE_EQUAL(char_ptr, "Source", dstString);
            ASSERT_ARE_EQUAL(int, 0, result);
        }

        TEST_FUNCTION(strcpy_s_copies_Empty_Source_into_Empty_Destination)
        {
            // arrange
            char dstString[128] = "";
            size_t dstSizeInBytes = sizeof(dstString);
            char srcString[] = "";
            int result = 0;

            // act
            result = strcpy_s(dstString, dstSizeInBytes, srcString);

            // assert
            ASSERT_ARE_EQUAL(char_ptr, "", dstString);
            ASSERT_ARE_EQUAL(int, 0, result);
        }

        // Tests_SRS_CRT_ABSTRACTIONS_99_012 : [If dst is NULL, the error code returned shall be EINVAL & dst shall not be modified.]
        TEST_FUNCTION(strcpy_s_With_NULL_Destination_Fails)
        {
            // arrange
            char* dstString = NULL;
            size_t dstSizeInBytes = sizeof(dstString);
            char srcString[] = "Source";
            int result = 0;

            // act
            HOOK_INVALID_PARAMETER_HANDLER();
#ifdef _MSC_VER
#pragma warning(suppress: 6387) /* This is test code, explictly calling with NULL argument */
#endif
            result = strcpy_s(dstString, dstSizeInBytes, srcString);
            UNHOOK_INVALID_PARAMETER_HANDLER();

            // assert
            ASSERT_ARE_EQUAL(char_ptr, NULL, dstString);
            ASSERT_ARE_EQUAL(int, EINVAL, result);
        }

        // Tests_SRS_CRT_ABSTRACTIONS_99_013 : [If src is NULL, the error code returned shall be EINVAL and dst[0] shall be set to 0.]
        TEST_FUNCTION(strcpy_s_With_NULL_Source_Fails)
        {
            // arrange
            char dstString[128] = "Destination";
            size_t dstSizeInBytes = sizeof(dstString);
            char* srcString = NULL;
            int result = 0;

            // act
            HOOK_INVALID_PARAMETER_HANDLER();
#ifdef _MSC_VER
#pragma warning(suppress: 6387) /* This is test code, explictly calling with NULL argument */
#endif
            result = strcpy_s(dstString, dstSizeInBytes, srcString);
            UNHOOK_INVALID_PARAMETER_HANDLER();

            // assert
            ASSERT_ARE_EQUAL(char,'\0', dstString[0]);
            ASSERT_ARE_EQUAL(int, EINVAL, result);
        }

        // Tests_SRS_CRT_ABSTRACTIONS_99_014 : [If the dstSizeInBytes is 0 or smaller than the required size for the src string, the error code returned shall be ERANGE & dst[0] set to 0.]
        TEST_FUNCTION(strcpy_s_With_dstSizeInBytes_Equals_Zero_Fails)
        {
            // arrange
            char dstString[128] = "Destination";
            size_t dstSizeInBytes = sizeof(dstString);
            char srcString[] = "Source";
            int result;

            // act
            dstSizeInBytes = 0;
            HOOK_INVALID_PARAMETER_HANDLER();
            result = strcpy_s(dstString, dstSizeInBytes, srcString);
            UNHOOK_INVALID_PARAMETER_HANDLER();

            // assert
#ifdef _MSC_VER /*MSDN Claims that destination buffer would be set to empty & ERANGE is the error, but not the case.  Filing bug.*/
            ASSERT_ARE_EQUAL(char_ptr, "Destination", dstString);
            ASSERT_ARE_EQUAL(int, EINVAL, result);
#else
            ASSERT_ARE_EQUAL(char, '\0', dstString[0]);
            ASSERT_ARE_EQUAL(int, ERANGE, result);
#endif
        }

        TEST_FUNCTION(strcpy_s_With_dstSizeInBytes_Smaller_Than_source_Fails)
        {
            // arrange
            char dstString[128] = "Destination";
            size_t dstSizeInBytes = sizeof(dstString);
            char srcString[] = "Source";
            int result;

            // act
            HOOK_INVALID_PARAMETER_HANDLER();
            dstSizeInBytes = sizeof(srcString) - 2;
            result = strcpy_s(dstString, dstSizeInBytes, srcString);
            UNHOOK_INVALID_PARAMETER_HANDLER();

            // assert
            ASSERT_ARE_EQUAL(char,'\0', dstString[0]);
            ASSERT_ARE_EQUAL(int, ERANGE, result);
        }

        /* strncpy_s */

        // Tests_SRS_CRT_ABSTRACTIONS_99_025 : [strncpy_s shall copy the first N characters of src to dst, where N is the lesser of MaxCount and the length of src.]
        // Tests_SRS_CRT_ABSTRACTIONS_99_041 : [If those N characters will fit within dst(whose size is given as dstSizeInBytes) and still leave room for a null terminator, then those characters shall be copied and a terminating null is appended; otherwise, strDest[0] is set to the null character and ERANGE error code returned per requirement below.]
        // Tests_SRS_CRT_ABSTRACTIONS_99_018: [strncpy_s shall return Zero upon success]
        TEST_FUNCTION(strncpy_s_copies_N_chars_of_source_to_destination_where_maxCount_equals_source_Length)
        {
            // arrange
            char dstString[] = "Destination";
            size_t dstSizeInBytes = sizeof(dstString);
            char srcString[] = "Source";
            size_t maxCount = sizeof(srcString);
            int result;

            // act
            result = strncpy_s(dstString, dstSizeInBytes, srcString, maxCount);

            // assert
            ASSERT_ARE_EQUAL(char_ptr, "Source", dstString);
            ASSERT_ARE_EQUAL(int, 0, result);
        }

        TEST_FUNCTION(strncpy_s_copies_N_chars_of_source_to_destination_where_maxCount_is_larger_than_Source_Length)
        {
            // arrange
            char dstString[] = "Destination";
            size_t dstSizeInBytes = sizeof(dstString);
            char srcString[] = "Source";
            size_t maxCount = sizeof(srcString);
            int result;

            // act
            result = strncpy_s(dstString, dstSizeInBytes, srcString, maxCount+5);

            // assert
            ASSERT_ARE_EQUAL(char_ptr, "Source", dstString);
            ASSERT_ARE_EQUAL(int, 0, result);
        }

        TEST_FUNCTION(strncpy_s_copies_N_chars_of_source_to_destination_where_maxCount_is_less_than_source_length)
        {
            // arrange
            char dstString[] = "Destination";
            size_t dstSizeInBytes = sizeof(dstString);
            char srcString[] = "Source";
            size_t maxCount = sizeof(srcString);
            int result;

            // act
            result = strncpy_s(dstString, dstSizeInBytes, srcString, maxCount - 3);

            // assert
            ASSERT_ARE_EQUAL(char_ptr, "Sour", dstString);
            ASSERT_ARE_EQUAL(int, 0, result);
        }

        // Tests_SRS_CRT_ABSTRACTIONS_99_026 : [If MaxCount is _TRUNCATE(defined as - 1), then as much of src as will fit into dst shall be copied while still leaving room for the terminating null to be appended.]
        TEST_FUNCTION(strncpy_s_with_maxCount_set_to_TRUNCATE_and_destination_fits_source)
        {
            // arrange
            char dstString[] = "Destination";
            size_t dstSizeInBytes = sizeof(dstString);
            char srcString[] = "Source";
            size_t maxCount = sizeof(srcString);
            int result;

            // act
            maxCount = _TRUNCATE;
            result = strncpy_s(dstString, dstSizeInBytes, srcString, maxCount);

            // assert
            ASSERT_ARE_EQUAL(char_ptr, "Source", dstString);
            ASSERT_ARE_EQUAL(int, 0, result);
        }

        // Tests_SRS_CRT_ABSTRACTIONS_99_026 : [If MaxCount is _TRUNCATE(defined as - 1), then as much of src as will fit into dst shall be copied while still leaving room for the terminating null to be appended.]
        // Tests_SRS_CRT_ABSTRACTIONS_99_019: [If truncation occurred as a result of the copy, the error code returned shall be STRUNCATE .]
        TEST_FUNCTION(strncpy_s_with_maxCount_set_to_TRUNCATE_and_destination_is_smaller_than_source)
        {
            // arrange
            char dstString[] = "Dest";
            size_t dstSizeInBytes = sizeof(dstString);
            char srcString[] = "Source";
            size_t maxCount = sizeof(srcString);
            int result;

            // act
            maxCount = _TRUNCATE;
            result = strncpy_s(dstString, dstSizeInBytes, srcString, maxCount);

            // assert
            ASSERT_ARE_EQUAL(char_ptr, "Sour", dstString);
            ASSERT_ARE_EQUAL(int, STRUNCATE, result);
        }

        // Tests_SRS_CRT_ABSTRACTIONS_99_020 : [If dst is NULL, the error code returned shall be EINVAL and dst shall not be modified.]
        TEST_FUNCTION(strncpy_s_fails_with_destination_set_to_NULL)
        {
            // arrange
            char* dstString = NULL;
            size_t dstSizeInBytes = sizeof(dstString);
            char srcString[] = "Source";
            size_t maxCount = sizeof(srcString);
            int result;

            // act
            HOOK_INVALID_PARAMETER_HANDLER();
#ifdef _MSC_VER
#pragma warning(suppress: 6387) /* This is test code, explictly calling with NULL argument */
#endif
            result = strncpy_s(dstString, dstSizeInBytes, srcString, maxCount);
            UNHOOK_INVALID_PARAMETER_HANDLER();

            // assert
            ASSERT_IS_NULL(dstString);
            ASSERT_ARE_EQUAL(int, EINVAL, result);
        }

        // Tests_SRS_CRT_ABSTRACTIONS_99_021: [If src is NULL, the error code returned shall be EINVAL and dst[0] shall be set to 0.]
        TEST_FUNCTION(strncpy_s_fails_with_source_set_to_NULL)
        {
            // arrange
            char dstString[] = "Destination";
            size_t dstSizeInBytes = sizeof(dstString);
            char* srcString = NULL;
            size_t maxCount = sizeof(srcString);
            int result;

            // act
            HOOK_INVALID_PARAMETER_HANDLER();
#ifdef _MSC_VER
#pragma warning(suppress: 6387) /* This is test code, explictly calling with NULL argument */
#endif
            result = strncpy_s(dstString, dstSizeInBytes, srcString, maxCount);
            UNHOOK_INVALID_PARAMETER_HANDLER();

            // assert
            ASSERT_ARE_EQUAL(char,'\0', dstString[0]);
            ASSERT_ARE_EQUAL(int, EINVAL, result);
        }

        // Tests_SRS_CRT_ABSTRACTIONS_99_022: [If the dstSizeInBytes is 0, the error code returned shall be EINVAL and dst shall not be modified.]
        TEST_FUNCTION(strncpy_s_fails_with_dstSizeInBytes_set_to_Zero)
        {
            // arrange
            char dstString[] = "Destination";
            size_t dstSizeInBytes = sizeof(dstString);
            char srcString[] = "Source";
            size_t maxCount = sizeof(srcString);
            int result;

            // act
            HOOK_INVALID_PARAMETER_HANDLER();
            dstSizeInBytes = 0;
            result = strncpy_s(dstString, dstSizeInBytes, srcString, maxCount);
            UNHOOK_INVALID_PARAMETER_HANDLER();

            // assert
            ASSERT_ARE_EQUAL(char_ptr, "Destination", dstString);
            ASSERT_ARE_EQUAL(int, EINVAL, result);
        }

        // Tests_SRS_CRT_ABSTRACTIONS_99_023 : [If dst is not NULL & dstSizeInBytes is smaller than the required size for the src string, the error code returned shall be ERANGE and dst[0] shall be set to 0.]
        TEST_FUNCTION(strncpy_s_dstSizeInBytes_is_smaller_than_the_required_size_for_source)
        {
            // arrange
            char dstString[] = "Dest";
            size_t dstSizeInBytes = sizeof(dstString);
            char srcString[] = "Source";
            size_t maxCount = sizeof(srcString);
            int result;

            // act
            HOOK_INVALID_PARAMETER_HANDLER();
            result = strncpy_s(dstString, dstSizeInBytes, srcString, maxCount);
            UNHOOK_INVALID_PARAMETER_HANDLER();

            // assert
            ASSERT_ARE_EQUAL(char,'\0', dstString[0]);
            ASSERT_ARE_EQUAL(int, ERANGE, result);
        }

        /* sprintf_s */

        // Tests_SRS_CRT_ABSTRACTIONS_99_029: [The sprintf_s function shall format and store series of characters and values in dst.Each argument(if any) is converted and output according to the corresponding Format Specification in the format variable.]
        // Tests_SRS_CRT_ABSTRACTIONS_99_031: [A null character is appended after the last character written.]
        // Tests_SRS_CRT_ABSTRACTIONS_99_027: [sprintf_s shall return the number of characters stored in dst upon success.  This number shall not include the terminating null character.]
        TEST_FUNCTION(sprintf_s_formats_and_stores_chars_and_values_in_destination)
        {
            // arrange
            char dstString[1024];
            size_t dstSizeInBytes = sizeof(dstString);
            char expectedString[] = "sprintf_s: 123, hello, Z, 1.5";
            int expectedStringSize = (int)(sizeof(expectedString));
            int result;

            // act
            result = sprintf_s(dstString, dstSizeInBytes, "sprintf_s: %d, %s, %c, %3.1f", 123, "hello", 'Z', 1.5f);

            // assert
            ASSERT_ARE_EQUAL(char_ptr, expectedString, dstString);
            ASSERT_ARE_EQUAL(int, expectedStringSize-1, result);
        }

        // Tests_SRS_CRT_ABSTRACTIONS_99_028: [If dst or format is a null pointer, sprintf_s shall return -1.]
        TEST_FUNCTION(sprintf_s_fails_with_dst_set_to_null)
        {
            // arrange
            char* dstString = NULL;
            size_t dstSizeInBytes = sizeof(dstString);
            int result;

            // act
            HOOK_INVALID_PARAMETER_HANDLER();
#ifdef _MSC_VER
#pragma warning(suppress: 6387) /* This is test code, explictly calling with NULL argument */
#endif
            result = sprintf_s(dstString, dstSizeInBytes, "sprintf_s: %d, %s, %c, %3.1f", 123, "hello", 'Z', 1.5f);
            UNHOOK_INVALID_PARAMETER_HANDLER();

            // assert
            ASSERT_ARE_EQUAL(int, -1, result);
            ASSERT_ARE_EQUAL(int, EINVAL, errno);
        }

        TEST_FUNCTION(sprintf_s_fails_with_format_set_to_null)
        {
            // arrange
            char dstString[1024];
            size_t dstSizeInBytes = sizeof(dstString);
            int result;

            // act
            HOOK_INVALID_PARAMETER_HANDLER();
#ifdef _MSC_VER
#pragma warning(suppress: 6387) /* This is test code, explictly calling with NULL argument */
#endif
            result = sprintf_s(dstString, dstSizeInBytes, NULL);
            UNHOOK_INVALID_PARAMETER_HANDLER();

            // assert
            ASSERT_ARE_EQUAL(int, -1, result);
            ASSERT_ARE_EQUAL(int, EINVAL, errno);
        }

        // Tests_SRS_CRT_ABSTRACTIONS_99_034 : [If the dst buffer is too small for the text being printed, then dst is set to an empty string and the function shall return -1.]
        TEST_FUNCTION(sprintf_s_fails_with_dst_too_small)
        {
            // arrange
            char dstString[5];
            size_t dstSizeInBytes = sizeof(dstString);
            int result;

            // act
            HOOK_INVALID_PARAMETER_HANDLER();
            result = sprintf_s(dstString, dstSizeInBytes, "sprintf_s: %d, %s, %c, %3.1f", 123, "hello", 'Z', 1.5f);
            UNHOOK_INVALID_PARAMETER_HANDLER();

            // assert
            ASSERT_ARE_EQUAL(char_ptr, "", dstString);
            ASSERT_ARE_EQUAL(int, -1, result);
        }

        TEST_FUNCTION(sprintf_s_fails_with_dst_buffer_size_not_fitting_null_char)
        {
            // arrange
            char dstString[5];
            size_t dstSizeInBytes = sizeof(dstString);
            int result;

            // act
            HOOK_INVALID_PARAMETER_HANDLER();
            result = sprintf_s(dstString, dstSizeInBytes, "12345");
            UNHOOK_INVALID_PARAMETER_HANDLER();

            // assert
            ASSERT_ARE_EQUAL(char_ptr, "", dstString);
            ASSERT_ARE_EQUAL(int, -1, result);
        }

        /* mallocAndStrcpy_s */

        // Tests_SRS_CRT_ABSTRACTIONS_99_038 : [mallocAndstrcpy_s shall allocate memory for destination buffer to fit the string in the source parameter.]
        // Tests_SRS_CRT_ABSTRACTIONS_99_039 : [mallocAndstrcpy_s shall copy the contents in the address source, including the terminating null character into location specified by the destination pointer after the memory allocation.]
        // Tests_SRS_CRT_ABSTRACTIONS_99_035: [mallocAndstrcpy_s shall return Zero upon success]
        TEST_FUNCTION(mallocAndStrcpy_s_copies_source_string_into_allocated_memory)
        {
            // arrange
            char* destString = NULL;
            char srcString[] = "Source";
            int result;

            // act
            result = mallocAndStrcpy_s(&destString, srcString);

            // assert
            ASSERT_ARE_EQUAL(char_ptr, destString, srcString);
            ASSERT_ARE_EQUAL(int, 0, result);

            ///cleanup
            free(destString);
        }

        // Tests_SRS_CRT_ABSTRACTIONS_99_036: [destination parameter or source parameter is NULL, the error code returned shall be EINVAL and destination shall not be modified.]
        TEST_FUNCTION(mallocAndStrcpy_s_fails_with_destination_pointer_set_to_null)
        {
            // arrange
            char** destPointer = NULL;
            char srcString[] = "Source";
            int result;

            // act
            result = mallocAndStrcpy_s(destPointer, srcString);

            // assert
            ASSERT_ARE_EQUAL(int, EINVAL, result);
        }

        TEST_FUNCTION(mallocAndStrcpy_s_fails_with_source_set_to_null)
        {
            // arrange
            char* destString = (char*)("Destination");
            char* srcString = NULL;
            int result;

            // act
            result = mallocAndStrcpy_s(&destString, srcString);

            // assert
            ASSERT_ARE_EQUAL(char_ptr, "Destination", destString);
            ASSERT_ARE_EQUAL(int, EINVAL, result);
        }

        /*http://vstfrd:8080/Azure/RD/_workitems/edit/3216760*/
#if 0
        // Tests_SRS_CRT_ABSTRACTIONS_99_037: [Upon failure to allocate memory for the destination, the function will return ENOMEM.]
        TEST_FUNCTION(mallocAndStrcpy_s_fails_upon_failure_to_allocate_memory)
        {
            // arrange
            char* destString = NULL;
            char* srcString = "Source";
            int result;
            mallocSize = 0;

            // act
#ifdef _CRTDBG_MAP_ALLOC
            HOOK_FUNCTION(_malloc_dbg, malloc_null);
            result = mallocAndStrcpy_s(&destString, srcString);
            UNHOOK_FUNCTION(_malloc_dbg, malloc_null);
#else
            HOOK_FUNCTION(malloc, malloc_null);
            result = mallocAndStrcpy_s(&destString, srcString);
            UNHOOK_FUNCTION(malloc, malloc_null);
#endif

            // assert
            ASSERT_ARE_EQUAL(int, ENOMEM, result);
            ASSERT_ARE_EQUAL(size_t,strlen(srcString)+1, mallocSize);
        }
#endif

        /*Tests_SRS_CRT_ABSTRACTIONS_02_003: [If destination is NULL then unsignedIntToString shall fail.] */
        TEST_FUNCTION(unsignedIntToString_fails_when_destination_is_NULL)
        {
            // arrange

            // act
            int result = unsignedIntToString(NULL, 100, 43);

            // assert
            ASSERT_ARE_NOT_EQUAL(int, 0, result);
        }

        /*Tests_SRS_CRT_ABSTRACTIONS_02_002: [If the conversion fails for any reason (for example, insufficient buffer space), a non-zero return value shall be supplied and unsignedIntToString shall fail.] */
        TEST_FUNCTION(unsignedIntToString_fails_when_destination_is_not_sufficient_for_1_digit)
        {
            // arrange
            char destination[1000];
            unsigned int toBeConverted = 1;
            size_t destinationSize = 1;

            ///act
            int result = unsignedIntToString(destination, destinationSize, toBeConverted);

            ///assert
            ASSERT_ARE_NOT_EQUAL(int, 0, result);
            
        }

        /*Tests_SRS_CRT_ABSTRACTIONS_02_002: [If the conversion fails for any reason (for example, insufficient buffer space), a non-zero return value shall be supplied and unsignedIntToString shall fail.] */
        TEST_FUNCTION(unsignedIntToString_fails_when_destination_is_not_sufficient_for_more_than_1_digit)
        {
            // arrange
            char destination[1000];
            unsigned int toBeConverted = 1; /*7 would not be a right starting digit*/
            size_t destinationSize = 1;
            while (toBeConverted <= (UINT_MAX / 10))
            {
                ///arrange
                destinationSize++;
                toBeConverted *= 10;

                ///act
                int result = unsignedIntToString(destination, destinationSize, toBeConverted);

                ///assert
                ASSERT_ARE_NOT_EQUAL(int, 0, result);
            }
        }

        /*Tests_SRS_CRT_ABSTRACTIONS_02_001: [unsignedIntToString shall convert the parameter value to its decimal representation as a string in the buffer indicated by parameter destination having the size indicated by parameter destinationSize.] */
        TEST_FUNCTION(unsignedIntToString_succeeds_1_digit)
        {
            // arrange
            char destination[1000];
            unsigned int toBeConverted = 2;
            size_t destinationSize = 2;

            ///act
            int result = unsignedIntToString(destination, destinationSize, toBeConverted);

            ///assert
            ASSERT_ARE_EQUAL(int, 0, result);
            ASSERT_ARE_EQUAL(char_ptr, "2", destination);

        }

        /*Tests_SRS_CRT_ABSTRACTIONS_02_001: [unsignedIntToString shall convert the parameter value to its decimal representation as a string in the buffer indicated by parameter destination having the size indicated by parameter destinationSize.] */
        /*Tests_SRS_CRT_ABSTRACTIONS_02_004: [If the conversion has been successfull then unsignedIntToString shall return 0.] */
        TEST_FUNCTION(unsignedIntToString_succeeds_for_interesting_numbers)
        {
            // arrange
            char destination[1000];
            size_t i;
            for (i = 0; i<sizeof(interestingUnsignedIntNumbersToBeConverted) / sizeof(interestingUnsignedIntNumbersToBeConverted[0]); i++)
            {
                ///act
                int result = unsignedIntToString(destination, 1000, interestingUnsignedIntNumbersToBeConverted[i]);

                ///assert
                ASSERT_ARE_EQUAL(int, 0, result);
                
                unsigned int valueFromString = 0;
                size_t pos = 0;
                while (destination[pos] != '\0')
                {
                    if (valueFromString > (UINT_MAX / 10))
                    {
                        ASSERT_FAIL("string produced was too big... ");
                    }
                    else
                    {
                        valueFromString *= 10;
                        valueFromString += (destination[pos] - '0');
                    }
                    pos++;
                }
                if (interestingUnsignedIntNumbersToBeConverted[i] != valueFromString)
                {
                    ASSERT_FAIL("unexpected value");
                }
            }
        }

        /*Tests_SRS_CRT_ABSTRACTIONS_02_001: [unsignedIntToString shall convert the parameter value to its decimal representation as a string in the buffer indicated by parameter destination having the size indicated by parameter destinationSize.] */
        /*Tests_SRS_CRT_ABSTRACTIONS_02_004: [If the conversion has been successfull then unsignedIntToString shall return 0.] */
        TEST_FUNCTION(unsignedIntToString_succeeds_for_space_just_about_right)
        {
            // arrange
            char destination[1000];
            unsigned int toBeConverted = 1; /*7 would not be a right starting digit*/
            size_t destinationSize = 2;
            while (toBeConverted <= (UINT_MAX / 10))
            {
                ///arrange
                destinationSize++;
                toBeConverted *= 10;

                ///act
                int result = unsignedIntToString(destination, destinationSize, toBeConverted);

                ///assert
                ASSERT_ARE_EQUAL(int, 0, result);

                unsigned int valueFromString = 0;
                size_t pos = 0;
                while (destination[pos] != '\0')
                {
                    if (valueFromString > (UINT_MAX / 10))
                    {
                        ASSERT_FAIL("string produced was too big... ");
                    }
                    else
                    {
                        valueFromString *= 10;
                        valueFromString += (destination[pos] - '0');
                    }
                    pos++;
                }
                if (toBeConverted != valueFromString)
                {
                    ASSERT_FAIL("unexpected value");
                }
            }
        }


        /*Tests_SRS_CRT_ABSTRACTIONS_02_007: [If destination is NULL then size_tToString shall fail.] */
        TEST_FUNCTION(size_tToString_fails_when_destination_is_NULL)
        {
            // arrange

            // act
            int result = size_tToString(NULL, 100, 43);

            // assert
            ASSERT_ARE_NOT_EQUAL(int, 0, result);
        }

        /*Tests_SRS_CRT_ABSTRACTIONS_02_006: [If the conversion fails for any reason (for example, insufficient buffer space), a non-zero return value shall be supplied and size_tToString shall fail.] */
        TEST_FUNCTION(size_tToString_fails_when_destination_is_not_sufficient_for_1_digit)
        {
            // arrange
            char destination[1000];
            size_t toBeConverted = 1;
            size_t destinationSize = 1;

            ///act
            int result = size_tToString(destination, destinationSize, toBeConverted);

            ///assert
            ASSERT_ARE_NOT_EQUAL(int, 0, result);

        }

        /*Tests_SRS_CRT_ABSTRACTIONS_02_006: [If the conversion fails for any reason (for example, insufficient buffer space), a non-zero return value shall be supplied and size_tToString shall fail.] */
        TEST_FUNCTION(size_tToString_fails_when_destination_is_not_sufficient_for_more_than_1_digit)
        {
            // arrange
            char destination[1000];
            size_t toBeConverted = 1; /*7 would not be a right starting digit*/
            size_t destinationSize = 1;
            while (toBeConverted <= (UINT_MAX / 10))
            {
                ///arrange
                destinationSize++;
                toBeConverted *= 10;

                ///act
                int result = size_tToString(destination, destinationSize, toBeConverted);

                ///assert
                ASSERT_ARE_NOT_EQUAL(int, 0, result);
            }
        }

        /*Tests_SRS_CRT_ABSTRACTIONS_02_001: [size_tToString shall convert the parameter value to its decimal representation as a string in the buffer indicated by parameter destination having the size indicated by parameter destinationSize.] */
        TEST_FUNCTION(size_tToString_succeeds_1_digit)
        {
            // arrange
            char destination[1000];
            size_t toBeConverted = 2;
            size_t destinationSize = 2;

            ///act
            int result = size_tToString(destination, destinationSize, toBeConverted);

            ///assert
            ASSERT_ARE_EQUAL(int, 0, result);
            ASSERT_ARE_EQUAL(char_ptr, "2", destination);

        }

        /*Tests_SRS_CRT_ABSTRACTIONS_02_001: [size_tToString shall convert the parameter value to its decimal representation as a string in the buffer indicated by parameter destination having the size indicated by parameter destinationSize.] */
        /*Tests_SRS_CRT_ABSTRACTIONS_02_004: [If the conversion has been successfull then size_tToString shall return 0.] */
        TEST_FUNCTION(size_tToString_succeeds_for_interesting_numbers)
        {
            // arrange
            char destination[1000];
            size_t i;
            for (i = 0; i<sizeof(interestingSize_tNumbersToBeConverted) / sizeof(interestingSize_tNumbersToBeConverted[0]); i++)
            {
                ///act
                int result = size_tToString(destination, 1000, interestingSize_tNumbersToBeConverted[i]);

                ///assert
                ASSERT_ARE_EQUAL(int, 0, result);

                size_t valueFromString = 0;
                size_t pos = 0;
                while (destination[pos] != '\0')
                {
                    if (valueFromString > (SIZE_MAX / 10))
                    {
                        ASSERT_FAIL("string produced was too big... ");
                    }
                    else
                    {
                        valueFromString *= 10;
                        valueFromString += (destination[pos] - '0');
                    }
                    pos++;
                }
                if (interestingSize_tNumbersToBeConverted[i] != valueFromString)
                {
                    ASSERT_FAIL("unexpected value");
                }
            }
        }

        /*Tests_SRS_CRT_ABSTRACTIONS_02_001: [size_tToString shall convert the parameter value to its decimal representation as a string in the buffer indicated by parameter destination having the size indicated by parameter destinationSize.] */
        /*Tests_SRS_CRT_ABSTRACTIONS_02_004: [If the conversion has been successfull then size_tToString shall return 0.] */
        TEST_FUNCTION(size_tToString_succeeds_for_space_just_about_right)
        {
            // arrange
            char destination[1000];
            size_t toBeConverted = 1; /*7 would not be a right starting digit*/
            size_t destinationSize = 2;
            while (toBeConverted <= (SIZE_MAX / 10))
            {
                ///arrange
                destinationSize++;
                toBeConverted *= 10;

                ///act
                int result = size_tToString(destination, destinationSize, toBeConverted);

                ///assert
                ASSERT_ARE_EQUAL(int, 0, result);

                size_t valueFromString = 0;
                size_t pos = 0;
                while (destination[pos] != '\0')
                {
                    if (valueFromString > (SIZE_MAX / 10))
                    {
                        ASSERT_FAIL("string produced was too big... ");
                    }
                    else
                    {
                        valueFromString *= 10;
                        valueFromString += (destination[pos] - '0');
                    }
                    pos++;
                }
                if (toBeConverted != valueFromString)
                {
                    ASSERT_FAIL("unexpected value");
                }
            }
        }


END_TEST_SUITE(CRTAbstractions_UnitTests)
