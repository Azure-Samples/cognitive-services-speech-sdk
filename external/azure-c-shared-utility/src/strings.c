// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

//
// PUT NO INCLUDES BEFORE HERE
//
#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include "azure_c_shared_utility/gballoc.h"

#include <stddef.h>
#include <string.h>
//
// PUT NO CLIENT LIBRARY INCLUDES BEFORE HERE
//

#include "azure_c_shared_utility/strings.h"
#include "azure_c_shared_utility/xlogging.h"

static const char hexToASCII[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

typedef struct STRING_TAG
{
    char* s;
}STRING;

/*this function will allocate a new string with just '\0' in it*/
/*return NULL if it fails*/
/* Codes_SRS_STRING_07_001: [STRING_new shall allocate a new STRING_HANDLE pointing to an empty string.] */
STRING_HANDLE STRING_new(void)
{
    STRING* result;
    if ((result = (STRING*)malloc(sizeof(STRING))) != NULL)
    {
        if ((result->s = (char*)malloc(1)) != NULL)
        {
            result->s[0] = '\0';
        }
        else
        {
            /* Codes_SRS_STRING_07_002: [STRING_new shall return an NULL STRING_HANDLE on any error that is encountered.] */
            free(result);
            result = NULL;
        }
    }
    return (STRING_HANDLE)result;
}

/*Codes_SRS_STRING_02_001: [STRING_clone shall produce a new string having the same content as the handle string.*/
STRING_HANDLE STRING_clone(STRING_HANDLE handle)
{
    STRING* result;
    /*Codes_SRS_STRING_02_002: [If parameter handle is NULL then STRING_clone shall return NULL.]*/
    if (handle == NULL)
    {
        result = NULL;
    }
    else
    {
        /*Codes_SRS_STRING_02_003: [If STRING_clone fails for any reason, it shall return NULL.] */
        if ((result = (STRING*)malloc(sizeof(STRING))) != NULL)
        {
            STRING* source = (STRING*)handle;
            /*Codes_SRS_STRING_02_003: [If STRING_clone fails for any reason, it shall return NULL.] */
            size_t sourceLen = strlen(source->s);
            if ((result->s = (char*)malloc(sourceLen + 1)) == NULL)
            {
                free(result);
                result = NULL;
            }
            else
            {
                memcpy(result->s, source->s, sourceLen + 1);
            }
        }
        else
        {
            /*not much to do, result is NULL from malloc*/
        }
    }
    return (STRING_HANDLE)result;
}

/* Codes_SRS_STRING_07_003: [STRING_construct shall allocate a new string with the value of the specified const char*.] */
STRING_HANDLE STRING_construct(const char* psz)
{
    STRING_HANDLE result;
    if (psz == NULL)
    {
        /* Codes_SRS_STRING_07_005: [If the supplied const char* is NULL STRING_construct shall return a NULL value.] */
        result = NULL;
    }
    else
    {
        STRING* str;
        if ((str = (STRING*)malloc(sizeof(STRING))) != NULL)
        {
            size_t nLen = strlen(psz) + 1;
            if ((str->s = (char*)malloc(nLen)) != NULL)
            {
                memcpy(str->s, psz, nLen);
                result = (STRING_HANDLE)str;
            }
            /* Codes_SRS_STRING_07_032: [STRING_construct encounters any error it shall return a NULL value.] */
            else
            {
                free(str);
                result = NULL;
            }
        }
        else
        {
            /* Codes_SRS_STRING_07_032: [STRING_construct encounters any error it shall return a NULL value.] */
            result = NULL;
        }
    }
    return result;
}

/*this function will return a new STRING with the memory for the actual string passed in as a parameter.*/
/*return NULL if it fails.*/
/* The supplied memory must have been allocated with malloc! */
/* Codes_SRS_STRING_07_006: [STRING_new_with_memory shall return a STRING_HANDLE by using the supplied char* memory.] */
STRING_HANDLE STRING_new_with_memory(const char* memory)
{
    STRING* result;
    if (memory == NULL)
    {
        /* Codes_SRS_STRING_07_007: [STRING_new_with_memory shall return a NULL STRING_HANDLE if the supplied char* is NULL.] */
        result = NULL;
    }
    else
    {
        if ((result = (STRING*)malloc(sizeof(STRING))) != NULL)
        {
            result->s = (char*)memory;
        }
    }
    return (STRING_HANDLE)result;
}

/* Codes_SRS_STRING_07_008: [STRING_new_quoted shall return a valid STRING_HANDLE Copying the supplied const char* value surrounded by quotes.] */
STRING_HANDLE STRING_new_quoted(const char* source)
{
    STRING* result;
    if (source == NULL)
    {
        /* Codes_SRS_STRING_07_009: [STRING_new_quoted shall return a NULL STRING_HANDLE if the supplied const char* is NULL.] */
        result = NULL;
    }
    else if ((result = (STRING*)malloc(sizeof(STRING))) != NULL)
    {
        size_t sourceLength = strlen(source);
        if ((result->s = (char*)malloc(sourceLength + 3)) != NULL)
        {
            result->s[0] = '"';
            memcpy(result->s + 1, source, sourceLength);
            result->s[sourceLength + 1] = '"';
            result->s[sourceLength + 2] = '\0';
        }
        else
        {
            /* Codes_SRS_STRING_07_031: [STRING_new_quoted shall return a NULL STRING_HANDLE if any error is encountered.] */
            free(result);
            result = NULL;
        }
    }
    return (STRING_HANDLE)result;
}

/*this function takes a regular const char* and turns in into "this is a\"JSON\" strings\u0008" (starting and ending quote included)*/
/*the newly created handle needs to be disposed of with STRING_delete*/
/*returns NULL if there are errors*/
STRING_HANDLE STRING_new_JSON(const char* source)
{
    STRING* result;
    if (source == NULL)
    {
        /*Codes_SRS_STRING_02_011: [If source is NULL then STRING_new_JSON shall return NULL.] */
        result = NULL;
        LogError("invalid arg (NULL)");
    }
    else
    {
        size_t i;
        size_t nControlCharacters = 0; /*counts how many characters are to be expanded from 1 character to \uxxxx (6 characters)*/
        size_t nEscapeCharacters = 0;
        size_t vlen = strlen(source);

        for (i = 0; i < vlen; i++)
        {
            /*Codes_SRS_STRING_02_014: [If any character has the value outside [1...127] then STRING_new_JSON shall fail and return NULL.] */
            if ((unsigned char)source[i] >= 128) /*this be a UNICODE character begin*/
            {
                break;
            }
            else
            {
                if (source[i] <= 0x1F)
                {
                    nControlCharacters++;
                }
                else if (
                    (source[i] == '"') ||
                    (source[i] == '\\') ||
                    (source[i] == '/')
                    )
                {
                    nEscapeCharacters++;
                }
            }
        }

        if (i < vlen)
        {
            result = NULL;
            LogError("invalid character in input string");
        }
        else
        {
            if ((result = (STRING*)malloc(sizeof(STRING))) == NULL)
            {
                /*Codes_SRS_STRING_02_021: [If the complete JSON representation cannot be produced, then STRING_new_JSON shall fail and return NULL.] */
                LogError("malloc failure");
            }
            else if ((result->s = (char*)malloc(vlen + 5 * nControlCharacters + nEscapeCharacters + 3)) == NULL)
            {
                /*Codes_SRS_STRING_02_021: [If the complete JSON representation cannot be produced, then STRING_new_JSON shall fail and return NULL.] */
                free(result);
                result = NULL;
                LogError("malloc failed");
            }
            else
            {
                size_t pos = 0;
                /*Codes_SRS_STRING_02_012: [The string shall begin with the quote character.] */
                result->s[pos++] = '"';
                for (i = 0; i < vlen; i++)
                {
                    if (source[i] <= 0x1F)
                    {
                        /*Codes_SRS_STRING_02_019: [If the character code is less than 0x20 then it shall be represented as \u00xx, where xx is the hex representation of the character code.]*/
                        result->s[pos++] = '\\';
                        result->s[pos++] = 'u';
                        result->s[pos++] = '0';
                        result->s[pos++] = '0';
                        result->s[pos++] = hexToASCII[(source[i] & 0xF0) >> 4]; /*high nibble*/
                        result->s[pos++] = hexToASCII[source[i] & 0x0F]; /*low nibble*/
                    }
                    else if (source[i] == '"')
                    {
                        /*Codes_SRS_STRING_02_016: [If the character is " (quote) then it shall be repsented as \".] */
                        result->s[pos++] = '\\';
                        result->s[pos++] = '"';
                    }
                    else if (source[i] == '\\')
                    {
                        /*Codes_SRS_STRING_02_017: [If the character is \ (backslash) then it shall represented as \\.] */
                        result->s[pos++] = '\\';
                        result->s[pos++] = '\\';
                    }
                    else if (source[i] == '/')
                    {
                        /*Codes_SRS_STRING_02_018: [If the character is / (slash) then it shall be represented as \/.] */
                        result->s[pos++] = '\\';
                        result->s[pos++] = '/';
                    }
                    else
                    {
                        /*Codes_SRS_STRING_02_013: [The string shall copy the characters of source "as they are" (until the '\0' character) with the following exceptions:] */
                        result->s[pos++] = source[i];
                    }
                }
                /*Codes_SRS_STRING_02_020: [The string shall end with " (quote).] */
                result->s[pos++] = '"';
                /*zero terminating it*/
                result->s[pos] = '\0';
            }
        }

    }
    return (STRING_HANDLE)result;
}

/*this function will concatenate to the string s1 the string s2, resulting in s1+s2*/
/*returns 0 if success*/
/*any other error code is failure*/
/* Codes_SRS_STRING_07_012: [STRING_concat shall concatenate the given STRING_HANDLE and the const char* value and place the value in the handle.] */
int STRING_concat(STRING_HANDLE handle, const char* s2)
{
    int result;
    if ((handle == NULL) || (s2 == NULL))
    {
        /* Codes_SRS_STRING_07_013: [STRING_concat shall return a nonzero number if an error is encountered.] */
        result = __LINE__;
    }
    else
    {
        STRING* s1 = (STRING*)handle;
        size_t s1Length = strlen(s1->s);
        size_t s2Length = strlen(s2);
        char* temp = (char*)realloc(s1->s, s1Length + s2Length + 1);
        if (temp == NULL)
        {
            /* Codes_SRS_STRING_07_013: [STRING_concat shall return a nonzero number if an error is encountered.] */
            result = __LINE__;
        }
        else
        {
            s1->s = temp;
            memcpy(s1->s + s1Length, s2, s2Length + 1);
            result = 0;
        }
    }
    return result;
}

/*this function will concatenate to the string s1 the string s2, resulting in s1+s2*/
/*returns 0 if success*/
/*any other error code is failure*/
/* Codes_SRS_STRING_07_034: [String_Concat_with_STRING shall concatenate a given STRING_HANDLE variable with a source STRING_HANDLE.] */
int STRING_concat_with_STRING(STRING_HANDLE s1, STRING_HANDLE s2)
{
    int result;
    if ((s1 == NULL) || (s2 == NULL))
    {
        /* Codes_SRS_STRING_07_035: [String_Concat_with_STRING shall return a nonzero number if an error is encountered.] */
        result = __LINE__;
    }
    else
    {
        STRING* dest = (STRING*)s1;
        STRING* src = (STRING*)s2;

        size_t s1Length = strlen(dest->s);
        size_t s2Length = strlen(src->s);
        char* temp = (char*)realloc(dest->s, s1Length + s2Length + 1);
        if (temp == NULL)
        {
            /* Codes_SRS_STRING_07_035: [String_Concat_with_STRING shall return a nonzero number if an error is encountered.] */
            result = __LINE__;
        }
        else
        {
            dest->s = temp;
            /* Codes_SRS_STRING_07_034: [String_Concat_with_STRING shall concatenate a given STRING_HANDLE variable with a source STRING_HANDLE.] */
            memcpy(dest->s + s1Length, src->s, s2Length + 1);
            result = 0;
        }
    }
    return result;
}

/*this function will copy the string from s2 to s1*/
/*returns 0 if success*/
/*any other error code is failure*/
/* Codes_SRS_STRING_07_016: [STRING_copy shall copy the const char* into the supplied STRING_HANDLE.] */
int STRING_copy(STRING_HANDLE handle, const char* s2)
{
    int result;
    if ((handle == NULL) || (s2 == NULL))
    {
        /* Codes_SRS_STRING_07_017: [STRING_copy shall return a nonzero value if any of the supplied parameters are NULL.] */
        result = __LINE__;
    }
    else
    {
        STRING* s1 = (STRING*)handle;
        /* Codes_SRS_STRING_07_026: [If the underlying char* refered to by s1 handle is equal to char* s2 than STRING_copy shall be a noop and return 0.] */
        if (s1->s != s2)
        {
            size_t s2Length = strlen(s2);
            char* temp = (char*)realloc(s1->s, s2Length + 1);
            if (temp == NULL)
            {
                /* Codes_SRS_STRING_07_027: [STRING_copy shall return a nonzero value if any error is encountered.] */
                result = __LINE__;
            }
            else
            {
                s1->s = temp;
                memmove(s1->s, s2, s2Length + 1);
                result = 0;
            }
        }
        else
        {
            /* Codes_SRS_STRING_07_033: [If overlapping pointer address is given to STRING_copy the behavior is undefined.] */
            result = 0;
        }
    }
    return result;
}

/*this function will copy n chars from s2 to the string s1, resulting in n chars only from s2 being stored in s1.*/
/*returns 0 if success*/
/*any other error code is failure*/
/* Codes_SRS_STRING_07_018: [STRING_copy_n shall copy the number of characters in const char* or the size_t whichever is lesser.] */
int STRING_copy_n(STRING_HANDLE handle, const char* s2, size_t n)
{
    int result;
    if ((handle == NULL) || (s2 == NULL))
    {
        /* Codes_SRS_STRING_07_019: [STRING_copy_n shall return a nonzero value if STRING_HANDLE or const char* is NULL.] */
        result = __LINE__;
    }
    else
    {
        STRING* s1 = (STRING*)handle;
        size_t s2Length = strlen(s2);
        char* temp;
        if (s2Length > n)
        {
            s2Length = n;
        }

        temp = (char*)realloc(s1->s, s2Length + 1);
        if (temp == NULL)
        {
            /* Codes_SRS_STRING_07_028: [STRING_copy_n shall return a nonzero value if any error is encountered.] */
            result = __LINE__;
        }
        else
        {
            s1->s = temp;
            memcpy(s1->s, s2, s2Length);
            s1->s[s2Length] = 0;
            result = 0;
        }

    }
    return result;
}

/*this function will quote the string passed as argument string =>"string"*/
/*returns 0 if success*/ /*doesn't change the string otherwise*/
/*any other error code is failure*/
/* Codes_SRS_STRING_07_014: [STRING_quote shall "quote" the supplied STRING_HANDLE and return 0 on success.] */
int STRING_quote(STRING_HANDLE handle)
{
    int result;
    if (handle == NULL)
    {
        /* Codes_SRS_STRING_07_015: [STRING_quote shall return a nonzero value if any of the supplied parameters are NULL.] */
        result = __LINE__;
    }
    else
    {
        STRING* s1 = (STRING*)handle;
        size_t s1Length = strlen(s1->s);
        char* temp = (char*)realloc(s1->s, s1Length + 2 + 1);/*2 because 2 quotes, 1 because '\0'*/
        if (temp == NULL)
        {
            /* Codes_SRS_STRING_07_029: [STRING_quote shall return a nonzero value if any error is encountered.] */
            result = __LINE__;
        }
        else
        {
            s1->s = temp;
            memmove(s1->s + 1, s1->s, s1Length);
            s1->s[0] = '"';
            s1->s[s1Length + 1] = '"';
            s1->s[s1Length + 2] = '\0';
            result = 0;
        }
    }
    return result;
}
/*this function will revert a string to an empty state*/
/*Returns 0 if the revert was succesful*/
/* Codes_SRS_STRING_07_022: [STRING_empty shall revert the STRING_HANDLE to an empty state.] */
int STRING_empty(STRING_HANDLE handle)
{
    int result;
    if (handle == NULL)
    {
        /* Codes_SRS_STRING_07_023: [STRING_empty shall return a nonzero value if the STRING_HANDLE is NULL.] */
        result = __LINE__;
    }
    else
    {
        STRING* s1 = (STRING*)handle;
        char* temp = (char*)realloc(s1->s, 1);
        if (temp == NULL)
        {
            /* Codes_SRS_STRING_07_030: [STRING_empty shall return a nonzero value if the STRING_HANDLE is NULL.] */
            result = __LINE__;
        }
        else
        {
            s1->s = temp;
            s1->s[0] = '\0';
            result = 0;
        }
    }
    return result;
}

/*this function will deallocate a string constructed by str_new*/
/* Codes_SRS_STRING_07_010: [STRING_delete will free the memory allocated by the STRING_HANDLE.] */
void STRING_delete(STRING_HANDLE handle)
{
    /* Codes_SRS_STRING_07_011: [STRING_delete will not attempt to free anything with a NULL STRING_HANDLE.] */
    if (handle != NULL)
    {
        STRING* value = (STRING*)handle;
        free(value->s);
        value->s = NULL;
        free(value);
    }
}

/* Codes_SRS_STRING_07_020: [STRING_c_str shall return the const char* associated with the given STRING_HANDLE.] */
const char* STRING_c_str(STRING_HANDLE handle)
{
    const char* result;
    if (handle != NULL)
    {
        result = ((STRING*)handle)->s;
    }
    else
    {
        /* Codes_SRS_STRING_07_021: [STRING_c_str shall return NULL if the STRING_HANDLE is NULL.] */
        result = NULL;
    }
    return result;
}

/* Codes_SRS_STRING_07_024: [STRING_length shall return the length of the underlying char* for the given handle] */
size_t STRING_length(STRING_HANDLE handle)
{
    size_t result = 0;
    /* Codes_SRS_STRING_07_025: [STRING_length shall return zero if the given handle is NULL.] */
    if (handle != NULL)
    {
        STRING* value = (STRING*)handle;
        result = strlen(value->s);
    }
    return result;
}

/*Codes_SRS_STRING_02_007: [STRING_construct_n shall construct a STRING_HANDLE from first "n" characters of the string pointed to by psz parameter.]*/
STRING_HANDLE STRING_construct_n(const char* psz, size_t n)
{
    STRING_HANDLE result;
    /*Codes_SRS_STRING_02_008: [If psz is NULL then STRING_construct_n shall return NULL.] */
    if (psz == NULL)
    {
        result = NULL;
        LogError("invalid arg (NULL)");
    }
    else
    {
        size_t len = strlen(psz);
        /*Codes_SRS_STRING_02_009: [If n is bigger than the size of the string psz, then STRING_construct_n shall return NULL.] */
        if (n > len)
        {
            result = NULL;
            LogError("invalig arg (n is bigger than the size of the string)");
        }
        else
        {
            STRING* str;
            if ((str = (STRING*)malloc(sizeof(STRING))) != NULL)
            {
                if ((str->s = (char*)malloc(len + 1)) != NULL)
                {
                    memcpy(str->s, psz, n);
                    str->s[n] = '\0';
                    result = (STRING_HANDLE)str;
                }
                /* Codes_SRS_STRING_02_010: [In all other error cases, STRING_construct_n shall return NULL.]  */
                else
                {
                    free(str);
                    result = NULL;
                }
            }
            else
            {
                /* Codes_SRS_STRING_02_010: [In all other error cases, STRING_construct_n shall return NULL.]  */
                result = NULL;
            }
        }
    }
    return result;
}

/* Codes_SRS_STRING_07_034: [STRING_compare returns an integer greater than, equal to, or less than zero, accordingly as the string pointed to by s1 is greater than, equal to, or less than the string s2.] */
int STRING_compare(STRING_HANDLE s1, STRING_HANDLE s2)
{
    int result;
    if (s1 == NULL && s2 == NULL)
    {
        /* Codes_SRS_STRING_07_035: [If h1 and h2 are both NULL then STRING_compare shall return 0.]*/
        result = 0;
    }
    else if (s1 == NULL)
    {
        /* Codes_SRS_STRING_07_036: [If h1 is NULL and h2 is nonNULL then STRING_compare shall return 1.]*/
        result = 1;
    }
    else if (s2 == NULL)
    {
        /* Codes_SRS_STRING_07_037: [If h2 is NULL and h1 is nonNULL then STRING_compare shall return -1.] */
        result = -1;
    }
    else
    {
        /* Codes_SRS_STRING_07_038: [STRING_compare shall compare the char s variable using the strcmp function.] */
        STRING* value1 = (STRING*)s1;
        STRING* value2 = (STRING*)s2;
        result = strcmp(value1->s, value2->s);
    }
    return result;
}

STRING_HANDLE STRING_from_byte_array(const unsigned char* source, size_t size)
{
    STRING* result;
    /*Codes_SRS_STRING_02_022: [ If source is NULL and size > 0 then STRING_from_BUFFER shall fail and return NULL. ]*/
    if ((source == NULL) && (size > 0))
    {
        LogError("invalid parameter (NULL)");
        result = NULL;
    }
    else
    {
        /*Codes_SRS_STRING_02_023: [ Otherwise, STRING_from_BUFFER shall build a string that has the same content (byte-by-byte) as source and return a non-NULL handle. ]*/
        result = (STRING*)malloc(sizeof(STRING));
        if (result == NULL)
        {
            /*Codes_SRS_STRING_02_024: [ If building the string fails, then STRING_from_BUFFER shall fail and return NULL. ]*/
            LogError("oom - unable to malloc");
            /*return as is*/
        }
        else
        {
            /*Codes_SRS_STRING_02_023: [ Otherwise, STRING_from_BUFFER shall build a string that has the same content (byte-by-byte) as source and return a non-NULL handle. ]*/
            result->s = (char*)malloc(size + 1);
            if (result->s == NULL)
            {
                /*Codes_SRS_STRING_02_024: [ If building the string fails, then STRING_from_BUFFER shall fail and return NULL. ]*/
                LogError("oom - unable to malloc");
                free(result);
                result = NULL;
            }
            else
            {
                memcpy(result->s, source, size);
                result->s[size] = '\0'; /*all is fine*/
            }
        }
    }
    return (STRING_HANDLE)result;
}
