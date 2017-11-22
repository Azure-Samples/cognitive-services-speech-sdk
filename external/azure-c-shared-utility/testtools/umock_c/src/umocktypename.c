// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include <ctype.h>
#include <string.h>
#include "umocktypename.h"
#include "umockalloc.h"
#include "umock_log.h"

char* umocktypename_normalize(const char* type_name)
{
    char* result;

    if (type_name == NULL)
    {
        /* Codes_SRS_UMOCKTYPENAME_01_005: [ If typename is NULL, then umocktypename_normalize shall fail and return NULL. ]*/
        UMOCK_LOG("umocktypename: NULL type_name.");
        result = NULL;
    }
    else
    {
        size_t length = 0;
        size_t pos = 0;
        size_t last_pos;

        /* we first compute how much space we need for the normalized version ... */
        while (type_name[pos] != '\0')
        {
            last_pos = pos;

            /* find all the spaces first */
            while ((type_name[pos] != '\0') && (isspace(type_name[pos])))
            {
                pos++;
            }

            /* we got a bunch of spaces, figure out whether we need to add any spaces to the normalized type name or not */
            /* Codes_SRS_UMOCKTYPENAME_01_002: [ umocktypename_normalize shall remove all spaces at the beginning of the typename. ]*/
            /* Codes_SRS_UMOCKTYPENAME_01_003: [ umocktypename_normalize shall remove all spaces at the end of the typename. ] */
            /* Codes_SRS_UMOCKTYPENAME_01_006: [ No space shall exist between any other token and a star. ]*/
            if ((last_pos == 0) ||
                (type_name[pos] == '\0') ||
                (type_name[last_pos - 1] == '*') ||
                (type_name[pos] == '*'))
            {
                /* do not add any spaces */
            }
            else
            {
                /* add one space */
                /* Codes_SRS_UMOCKTYPENAME_01_004: [ umocktypename_normalize shall remove all extra spaces (more than 1 space) between elements that are part of the typename. ]*/
                length ++;
            }

            /* found something that is a non-space character, add to the normalized type name */
            while ((type_name[pos] != '\0') && (!isspace(type_name[pos])))
            {
                pos++;
                length++;
            }
        }

        if (length == 0)
        {
            /* Codes_SRS_UMOCKTYPENAME_01_007: [ If the length of the normalized typename is 0, umocktypename_normalize shall return NULL. ]*/
            UMOCK_LOG("umocktypename: 0 length for the normalized type name.");
            result = NULL;
        }
        else
        {
            /* ... then we allocate*/
            /* Codes_SRS_UMOCKTYPENAME_01_001: [ umocktypename_normalize shall return a char\* with a newly allocated string that contains the normalized typename. ]*/
            result = (char*)umockalloc_malloc(length + 1);
            /* Codes_SRS_UMOCKTYPENAME_01_008: [ If allocating memory fails, umocktypename_normalize shall fail and return NULL. ]*/
            if (result != NULL)
            {
                pos = 0;
                length = 0;
                last_pos;

                while (type_name[pos] != '\0')
                {
                    last_pos = pos;

                    /* find all the spaces first */
                    while ((type_name[pos] != '\0') && (isspace(type_name[pos])))
                    {
                        pos++;
                    }

                    /* we got a bunch of spaces, figure out whether we need to add any spaces to the normalized type name or not */
                    /* Codes_SRS_UMOCKTYPENAME_01_002: [ umocktypename_normalize shall remove all spaces at the beginning of the typename. ]*/
                    /* Codes_SRS_UMOCKTYPENAME_01_003: [ umocktypename_normalize shall remove all spaces at the end of the typename. ] */
                    /* Codes_SRS_UMOCKTYPENAME_01_006: [ No space shall exist between any other token and a star. ]*/
                    if ((last_pos == 0) ||
                        (type_name[pos] == '\0') ||
                        (type_name[last_pos - 1] == '*') ||
                        (type_name[pos] == '*'))
                    {
                        /* do not add any spaces */
                    }
                    else
                    {
                        /* add one space */
                        /* Codes_SRS_UMOCKTYPENAME_01_004: [ umocktypename_normalize shall remove all extra spaces (more than 1 space) between elements that are part of the typename. ]*/
                        result[length++] = ' ';
                    }

                    /* found something that is a non-space character, add to the normalized type name */
                    while ((type_name[pos] != '\0') && (!isspace(type_name[pos])))
                    {
                        result[length++] = type_name[pos++];
                    }
                }

                result[length] = '\0';
            }
        }
    }

    return result;
}
