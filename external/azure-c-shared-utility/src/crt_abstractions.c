// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#define __STDC_WANT_LIB_EXT1__ 1

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include "azure_c_shared_utility/gballoc.h"

#include "azure_c_shared_utility/crt_abstractions.h"
#include "errno.h"
#include <stddef.h>
#include <limits.h>

#ifdef _MSC_VER
#else

#include <stdarg.h>

/*Codes_SRS_CRT_ABSTRACTIONS_99_008: [strcat_s shall append the src to dst and terminates the resulting string with a null character.]*/
int strcat_s(char* dst, size_t dstSizeInBytes, const char* src)
{
    int result;
    /*Codes_SRS_CRT_ABSTRACTIONS_99_004: [If dst is NULL or unterminated, the error code returned shall be EINVAL & dst shall not be modified.]*/
    if (dst == NULL)
    {
        result = EINVAL;
    }
    /*Codes_SRS_CRT_ABSTRACTIONS_99_005: [If src is NULL, the error code returned shall be EINVAL and dst[0] shall be set to 0.]*/
    else if (src == NULL)
    {
        dst[0] = '\0';
        result = EINVAL;
    }
    else
    {
        /*Codes_SRS_CRT_ABSTRACTIONS_99_006: [If the dstSizeInBytes is 0 or smaller than the required size for dst & src, the error code returned shall be ERANGE & dst[0] set to 0.]*/
        if (dstSizeInBytes == 0)
        {
            result = ERANGE;
            dst[0] = '\0';
        }
        else
        {
            size_t dstStrLen = 0;
#ifdef __STDC_LIB_EXT1__
            dstStrLen = strnlen_s(dst, dstSizeInBytes);
#else
            size_t i;
            for(i=0; (i < dstSizeInBytes) && (dst[i]!= '\0'); i++)
            {
            }
            dstStrLen = i;
#endif
            /*Codes_SRS_CRT_ABSTRACTIONS_99_004: [If dst is NULL or unterminated, the error code returned shall be EINVAL & dst shall not be modified.]*/
            if (dstSizeInBytes == dstStrLen) /* this means the dst string is not terminated*/
            {
                result = EINVAL;
            }
            else
            {
                /*Codes_SRS_CRT_ABSTRACTIONS_99_009: [The initial character of src shall overwrite the terminating null character of dst.]*/
                (void)strncpy(&dst[dstStrLen], src, dstSizeInBytes - dstStrLen);
                /*Codes_SRS_CRT_ABSTRACTIONS_99_006: [If the dstSizeInBytes is 0 or smaller than the required size for dst & src, the error code returned shall be ERANGE & dst[0] set to 0.]*/
                if (dst[dstSizeInBytes-1] != '\0')
                {
                    dst[0] = '\0';
                    result = ERANGE;
                }
                else
                {
                    /*Codes_SRS_CRT_ABSTRACTIONS_99_003: [strcat_s shall return Zero upon success.]*/
                    result = 0;
                }
            }
        }
    }

    return result;
}

/*Codes_SRS_CRT_ABSTRACTIONS_99_025: [strncpy_s shall copy the first N characters of src to dst, where N is the lesser of MaxCount and the length of src.]*/
int strncpy_s(char* dst, size_t dstSizeInBytes, const char* src, size_t maxCount)
{
    int result;
    int truncationFlag = 0;
    /*Codes_SRS_CRT_ABSTRACTIONS_99_020: [If dst is NULL, the error code returned shall be EINVAL and dst shall not be modified.]*/
    if (dst == NULL)
    {
        result = EINVAL;
    }
    /*Codes_SRS_CRT_ABSTRACTIONS_99_021: [If src is NULL, the error code returned shall be EINVAL and dst[0] shall be set to 0.]*/
    else if (src == NULL)
    {
        dst[0] = '\0';
        result = EINVAL;
    }
    /*Codes_SRS_CRT_ABSTRACTIONS_99_022: [If the dstSizeInBytes is 0, the error code returned shall be EINVAL and dst shall not be modified.]*/
    else if (dstSizeInBytes == 0)
    {
        result = EINVAL;
    }
    else 
    {
        size_t srcLength = strlen(src);
        if (maxCount != _TRUNCATE)
        {
            /*Codes_SRS_CRT_ABSTRACTIONS_99_041: [If those N characters will fit within dst (whose size is given as dstSizeInBytes) and still leave room for a null terminator, then those characters shall be copied and a terminating null is appended; otherwise, strDest[0] is set to the null character and ERANGE error code returned.]*/
            if (srcLength > maxCount)
            {
                srcLength = maxCount;
            }

            /*Codes_SRS_CRT_ABSTRACTIONS_99_023: [If dst is not NULL & dstSizeInBytes is smaller than the required size for the src string, the error code returned shall be ERANGE and dst[0] shall be set to 0.]*/
            if (srcLength + 1 > dstSizeInBytes)
            {
                dst[0] = '\0';
                result = ERANGE;
            }
            else
            {
                (void)strncpy(dst, src, srcLength);
                dst[srcLength] = '\0';
                /*Codes_SRS_CRT_ABSTRACTIONS_99_018: [strncpy_s shall return Zero upon success]*/
                result = 0;
            }
        }
        /*Codes_SRS_CRT_ABSTRACTIONS_99_026: [If MaxCount is _TRUNCATE (defined as -1), then as much of src as will fit into dst shall be copied while still leaving room for the terminating null to be appended.]*/
        else
        {
            if (srcLength + 1 > dstSizeInBytes )
            {
                srcLength = dstSizeInBytes - 1;
                truncationFlag = 1;
            }
            (void)strncpy(dst, src, srcLength);
            dst[srcLength] = '\0';
            result = 0;
        }
    }

    /*Codes_SRS_CRT_ABSTRACTIONS_99_019: [If truncation occurred as a result of the copy, the error code returned shall be STRUNCATE.]*/
    if (truncationFlag == 1)
    {
        result = STRUNCATE;
    }

    return result;
}

/* Codes_SRS_CRT_ABSTRACTIONS_99_016: [strcpy_s shall copy the contents in the address of src, including the terminating null character, to the location that's specified by dst.]*/
int strcpy_s(char* dst, size_t dstSizeInBytes, const char* src)
{
    int result;

    /* Codes_SRS_CRT_ABSTRACTIONS_99_012: [If dst is NULL, the error code returned shall be EINVAL & dst shall not be modified.]*/
    if (dst == NULL)
    {
        result = EINVAL;
    }
    /* Codes_SRS_CRT_ABSTRACTIONS_99_013: [If src is NULL, the error code returned shall be EINVAL and dst[0] shall be set to 0.]*/
    else if (src == NULL)
    {
        dst[0] = '\0';
        result = EINVAL;
    }
    /* Codes_SRS_CRT_ABSTRACTIONS_99_014: [If the dstSizeInBytes is 0 or smaller than the required size for the src string, the error code returned shall be ERANGE & dst[0] set to 0.]*/
    else if (dstSizeInBytes == 0)
    {
        dst[0] = '\0';
        result = ERANGE;
    }
    else
    {
        size_t neededBuffer = strlen(src);
        /* Codes_SRS_CRT_ABSTRACTIONS_99_014: [If the dstSizeInBytes is 0 or smaller than the required size for the src string, the error code returned shall be ERANGE & dst[0] set to 0.]*/
        if (neededBuffer + 1 > dstSizeInBytes)
        {
            dst[0] = '\0';
            result = ERANGE;
        }
        else
        {
            memcpy(dst, src, neededBuffer + 1);
            /*Codes_SRS_CRT_ABSTRACTIONS_99_011: [strcpy_s shall return Zero upon success]*/
            result = 0;
        }
    }

    return result;
}

/*Codes_SRS_CRT_ABSTRACTIONS_99_029: [The sprintf_s function shall format and store series of characters and values in dst.  Each argument (if any) is converted and output according to the corresponding Format Specification in the format variable.]*/
/*Codes_SRS_CRT_ABSTRACTIONS_99_031: [A null character is appended after the last character written.]*/
int sprintf_s(char* dst, size_t dstSizeInBytes, const char* format, ...)
{
    int result;
    /*Codes_SRS_CRT_ABSTRACTIONS_99_028: [If dst or format is a null pointer, sprintf_s shall return -1 and set errno to EINVAL]*/
    if ((dst == NULL) ||
        (format == NULL))
    {
        errno = EINVAL;
        result = -1;
    }
    else
    {
        /*Codes_SRS_CRT_ABSTRACTIONS_99_033: [sprintf_s shall check the format string for valid formatting characters.  If the check fails, the function returns -1.]*/

#if defined _MSC_VER 
#error crt_abstractions is not provided for Microsoft Compilers
#else
        /*not Microsoft compiler... */
#if defined (__STDC_VERSION__) || (__cplusplus)
#if ( \
        ((__STDC_VERSION__  == 199901L) || (__STDC_VERSION__ == 201000L) || (__STDC_VERSION__ == 201112L)) || \
        (defined __cplusplus) \
    )
        /*C99 compiler*/
        va_list args;
        va_start(args, format);
        /*Codes_SRS_CRT_ABSTRACTIONS_99_027: [sprintf_s shall return the number of characters stored in dst upon success.  This number shall not include the terminating null character.]*/
        result = vsnprintf(dst, dstSizeInBytes, format, args);
        va_end(args);

        /*C99: Thus, the null-terminated output has been completely written if and only if the returned value is nonnegative and less than n*/
        if (result < 0)
        {
            result = -1;
        }
        else if ((size_t)result >= dstSizeInBytes)
        {
            /*Codes_SRS_CRT_ABSTRACTIONS_99_034: [If the dst buffer is too small for the text being printed, then dst is set to an empty string and the function shall return -1.]*/
            dst[0] = '\0';
            result = -1;
        }
        else
        {
            /*do nothing, all is fine*/
        }
#else
#error STDC_VERSION defined, but of unknown value; unable to sprinf_s, or provide own implementation
#endif
#else
#error for STDC_VERSION undefined (assumed C89), provide own implementation of sprintf_s
#endif
#endif
    }
    return result;
}
#endif /* _MSC_VER */

/*Codes_SRS_CRT_ABSTRACTIONS_99_038: [mallocAndstrcpy_s shall allocate memory for destination buffer to fit the string in the source parameter.]*/
int mallocAndStrcpy_s(char** destination, const char* source)
{
    int result;
    /*Codes_SRS_CRT_ABSTRACTIONS_99_036: [destination parameter or source parameter is NULL, the error code returned shall be EINVAL and destination shall not be modified.]*/
    if ((destination == NULL) || (source == NULL))
    {
        /*If strDestination or strSource is a null pointer[...]these functions return EINVAL */
        result = EINVAL;
    }
    else
    {
        size_t l = strlen(source);
        *destination = (char*)malloc(l + 1);
        /*Codes_SRS_CRT_ABSTRACTIONS_99_037: [Upon failure to allocate memory for the destination, the function will return ENOMEM.]*/
        if (*destination == NULL)
        {
            result = ENOMEM;
        }
        else
        {
            /*Codes_SRS_CRT_ABSTRACTIONS_99_039: [mallocAndstrcpy_s shall copy the contents in the address source, including the terminating null character into location specified by the destination pointer after the memory allocation.]*/
            int temp = strcpy_s(*destination, l + 1, source);
            if (temp < 0) /*strcpy_s error*/
            {
                free(*destination);
                *destination = NULL;
                result = temp;
            }
            else
            {
                /*Codes_SRS_CRT_ABSTRACTIONS_99_035: [mallocAndstrcpy_s shall return Zero upon success]*/
                result = 0;
            }
        }
    }
    return result;
}

/*takes "value" and transforms it into a decimal string*/
/*10 => "10"*/
/*return 0 when everything went ok*/
/*Codes_SRS_CRT_ABSTRACTIONS_02_001: [unsignedIntToString shall convert the parameter value to its decimal representation as a string in the buffer indicated by parameter destination having the size indicated by parameter destinationSize.] */
int unsignedIntToString(char* destination, size_t destinationSize, unsigned int value)
{
    int result;
    size_t pos;
    /*the below loop gets the number in reverse order*/
    /*Codes_SRS_CRT_ABSTRACTIONS_02_003: [If destination is NULL then unsignedIntToString shall fail.] */
    /*Codes_SRS_CRT_ABSTRACTIONS_02_002: [If the conversion fails for any reason (for example, insufficient buffer space), a non-zero return value shall be supplied and unsignedIntToString shall fail.] */
    if (
        (destination == NULL) ||
        (destinationSize < 2) /*because the smallest number is '0\0' which requires 2 characters*/
        )
    {
        result = __LINE__;
    }
    else
    {
        pos = 0;
        do
        {
            destination[pos++] = '0' + (value % 10);
            value /= 10;
        } while ((value > 0) && (pos < (destinationSize-1)));

        if (value == 0)
        {
            size_t w;
            destination[pos] = '\0';
            /*all converted and they fit*/
            for (w = 0; w <= (pos-1) >> 1; w++)
            {
                char temp;
                temp = destination[w];
                destination[w] = destination[pos - 1 - w];
                destination[pos -1 - w] = temp;
            }
            /*Codes_SRS_CRT_ABSTRACTIONS_02_004: [If the conversion has been successfull then unsignedIntToString shall return 0.] */
            result = 0;
        }
        else
        {
            /*Codes_SRS_CRT_ABSTRACTIONS_02_002: [If the conversion fails for any reason (for example, insufficient buffer space), a non-zero return value shall be supplied and unsignedIntToString shall fail.] */
            result = __LINE__;
        }
    }
    return result;
}

/*takes "value" and transforms it into a decimal string*/
/*10 => "10"*/
/*return 0 when everything went ok*/
/*Codes_SRS_CRT_ABSTRACTIONS_02_001: [unsignedIntToString shall convert the parameter value to its decimal representation as a string in the buffer indicated by parameter destination having the size indicated by parameter destinationSize.] */
int size_tToString(char* destination, size_t destinationSize, size_t value)
{
    int result;
    size_t pos;
    /*the below loop gets the number in reverse order*/
    /*Codes_SRS_CRT_ABSTRACTIONS_02_003: [If destination is NULL then unsignedIntToString shall fail.] */
    /*Codes_SRS_CRT_ABSTRACTIONS_02_002: [If the conversion fails for any reason (for example, insufficient buffer space), a non-zero return value shall be supplied and unsignedIntToString shall fail.] */
    if (
        (destination == NULL) ||
        (destinationSize < 2) /*because the smallest number is '0\0' which requires 2 characters*/
        )
    {
        result = __LINE__;
    }
    else
    {
        pos = 0;
        do
        {
            destination[pos++] = '0' + (value % 10);
            value /= 10;
        } while ((value > 0) && (pos < (destinationSize - 1)));

        if (value == 0)
        {
            size_t w;
            destination[pos] = '\0';
            /*all converted and they fit*/
            for (w = 0; w <= (pos - 1) >> 1; w++)
            {
                char temp;
                temp = destination[w];
                destination[w] = destination[pos - 1 - w];
                destination[pos - 1 - w] = temp;
            }
            /*Codes_SRS_CRT_ABSTRACTIONS_02_004: [If the conversion has been successfull then unsignedIntToString shall return 0.] */
            result = 0;
        }
        else
        {
            /*Codes_SRS_CRT_ABSTRACTIONS_02_002: [If the conversion fails for any reason (for example, insufficient buffer space), a non-zero return value shall be supplied and unsignedIntToString shall fail.] */
            result = __LINE__;
        }
    }
    return result;
}
