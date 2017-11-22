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

#include "azure_c_shared_utility/buffer_.h"
#include "azure_c_shared_utility/xlogging.h"

typedef struct BUFFER_TAG
{
    unsigned char* buffer;
    size_t size;
}BUFFER;

/* Codes_SRS_BUFFER_07_001: [BUFFER_new shall allocate a BUFFER_HANDLE that will contain a NULL unsigned char*.] */
BUFFER_HANDLE BUFFER_new(void)
{
    BUFFER* temp = (BUFFER*)malloc(sizeof(BUFFER));
    /* Codes_SRS_BUFFER_07_002: [BUFFER_new shall return NULL on any error that occurs.] */
    if (temp != NULL)
    {
        temp->buffer = NULL;
        temp->size = 0;
    }
    return (BUFFER_HANDLE)temp;
}

BUFFER_HANDLE BUFFER_create(const unsigned char* source, size_t size)
{
    BUFFER* result;
    /*Codes_SRS_BUFFER_02_001: [If source is NULL then BUFFER_create shall return NULL.]*/
    if (source == NULL)
    {
        result = NULL;
    }
    else
    {
        /*Codes_SRS_BUFFER_02_002: [Otherwise, BUFFER_create shall allocate memory to hold size bytes and shall copy from source size bytes into the newly allocated memory.] */
        result = (BUFFER*)malloc(sizeof(BUFFER));
        if (result == NULL)
        {
            /*Codes_SRS_BUFFER_02_003: [If allocating memory fails, then BUFFER_create shall return NULL.] */
            /*fallthrough*/
        }
        else
        {
            result->buffer = (unsigned char*)malloc(size);
            if (result->buffer == NULL)
            {
                /*Codes_SRS_BUFFER_02_003: [If allocating memory fails, then BUFFER_create shall return NULL.]*/
                free(result);
                result = NULL;
            }
            else
            {
                /*Codes_SRS_BUFFER_02_004: [Otherwise, BUFFER_create shall return a non-NULL handle.] */
                memcpy(result->buffer, source, size);
                result->size = size;
            }
        }
    }
    return (BUFFER_HANDLE)result;
}

/* Codes_SRS_BUFFER_07_003: [BUFFER_delete shall delete the data associated with the BUFFER_HANDLE along with the Buffer.] */
void BUFFER_delete(BUFFER_HANDLE handle)
{
    /* Codes_SRS_BUFFER_07_004: [BUFFER_delete shall not delete any BUFFER_HANDLE that is NULL.] */
    if (handle != NULL)
    {
        BUFFER* b = (BUFFER*)handle;
        if (b->buffer != NULL)
        {
            /* Codes_SRS_BUFFER_07_003: [BUFFER_delete shall delete the data associated with the BUFFER_HANDLE along with the Buffer.] */
            free(b->buffer);
        }
        free(b);
    }
}

/*return 0 if the buffer was copied*/
/*else return different than zero*/
/* Codes_SRS_BUFFER_07_008: [BUFFER_build allocates size_t bytes, copies the unsigned char* into the buffer and returns zero on success.] */
int BUFFER_build(BUFFER_HANDLE handle, const unsigned char* source, size_t size)
{
    int result;
    if (handle == NULL)
    {
        /* Codes_SRS_BUFFER_07_009: [BUFFER_build shall return nonzero if handle is NULL ] */
        result = __LINE__;
    }
    /* Codes_SRS_BUFFER_01_002: [The size argument can be zero, in which case the underlying buffer held by the buffer instance shall be freed.] */
    else if (size == 0)
    {
        /* Codes_SRS_BUFFER_01_003: [If size is zero, source can be NULL.] */
        BUFFER* b = (BUFFER*)handle;
        free(b->buffer);
        b->buffer = NULL;
        b->size = 0;

        result = 0;
    }
    else
    {
        if (source == NULL)
        {
            /* Codes_SRS_BUFFER_01_001: [If size is positive and source is NULL, BUFFER_build shall return nonzero] */
            result = __LINE__;
        }
        else
        {
            BUFFER* b = (BUFFER*)handle;
            /* Codes_SRS_BUFFER_07_011: [BUFFER_build shall overwrite previous contents if the buffer has been previously allocated.] */
            unsigned char* newBuffer = (unsigned char*)realloc(b->buffer, size);
            if (newBuffer == NULL)
            {
                /* Codes_SRS_BUFFER_07_010: [BUFFER_build shall return nonzero if any error is encountered.] */
                result = __LINE__;
            }
            else
            {
                b->buffer = newBuffer;
                b->size = size;
                /* Codes_SRS_BUFFER_01_002: [The size argument can be zero, in which case nothing shall be copied from source.] */
                (void)memcpy(b->buffer, source, size);

                result = 0;
            }
        }
    }

    return result;
}

/*return 0 if the buffer was pre-build(that is, had its space allocated)*/
/*else return different than zero*/
/* Codes_SRS_BUFFER_07_005: [BUFFER_pre_build allocates size_t bytes of BUFFER_HANDLE and returns zero on success.] */
int BUFFER_pre_build(BUFFER_HANDLE handle, size_t size)
{
    int result;
    if (handle == NULL)
    {
        /* Codes_SRS_BUFFER_07_006: [If handle is NULL or size is 0 then BUFFER_pre_build shall return a nonzero value.] */
        result = __LINE__;
    }
    else if (size == 0)
    {
        /* Codes_SRS_BUFFER_07_006: [If handle is NULL or size is 0 then BUFFER_pre_build shall return a nonzero value.] */
        result = __LINE__;
    }
    else
    {
        BUFFER* b = (BUFFER*)handle;
        if (b->buffer != NULL)
        {
            /* Codes_SRS_BUFFER_07_007: [BUFFER_pre_build shall return nonzero if the buffer has been previously allocated and is not NULL.] */
            result = __LINE__;
        }
        else
        {
            if ((b->buffer = (unsigned char*)malloc(size)) == NULL)
            {
                /* Codes_SRS_BUFFER_07_013: [BUFFER_pre_build shall return nonzero if any error is encountered.] */
                result = __LINE__;
            }
            else
            {
                b->size = size;
                result = 0;
            }
        }
    }
    return result;
}

/* Codes_SRS_BUFFER_07_019: [BUFFER_content shall return the data contained within the BUFFER_HANDLE.] */
int BUFFER_content(BUFFER_HANDLE handle, const unsigned char** content)
{
    int result;
    if ((handle == NULL) || (content == NULL))
    {
        /* Codes_SRS_BUFFER_07_020: [If the handle and/or content*is NULL BUFFER_content shall return nonzero.] */
        result = __LINE__;
    }
    else
    {
        BUFFER* b = (BUFFER*)handle;
        *content = b->buffer;
        result = 0;
    }
    return result;
}

/*return 0 if everything went ok and whatever was built in the buffer was unbuilt*/
/* Codes_SRS_BUFFER_07_012: [BUFFER_unbuild shall clear the underlying unsigned char* data associated with the BUFFER_HANDLE this will return zero on success.] */
extern int BUFFER_unbuild(BUFFER_HANDLE handle)
{
    int result;
    if (handle == NULL)
    {
        /* Codes_SRS_BUFFER_07_014: [BUFFER_unbuild shall return a nonzero value if BUFFER_HANDLE is NULL.] */
        result = __LINE__;
    }
    else
    {
        BUFFER* b = (BUFFER*)handle;
        if (b->buffer != NULL)
        {
            free(b->buffer);
            b->buffer = NULL;
            b->size = 0;
            result = 0;
        }
        else
        {
            /* Codes_SRS_BUFFER_07_015: [BUFFER_unbuild shall return a nonzero value if the unsigned char* referenced by BUFFER_HANDLE is NULL.] */
            result = __LINE__;
        }
    }
    return result;
}

/* Codes_SRS_BUFFER_07_016: [BUFFER_enlarge shall increase the size of the unsigned char* referenced by BUFFER_HANDLE.] */
int BUFFER_enlarge(BUFFER_HANDLE handle, size_t enlargeSize)
{
    int result;
    if (handle == NULL)
    {
        /* Codes_SRS_BUFFER_07_017: [BUFFER_enlarge shall return a nonzero result if any parameters are NULL or zero.] */
        result = __LINE__;
    }
    else if (enlargeSize == 0)
    {
        /* Codes_SRS_BUFFER_07_017: [BUFFER_enlarge shall return a nonzero result if any parameters are NULL or zero.] */
        result = __LINE__;
    }
    else
    {
        BUFFER* b = (BUFFER*)handle;
        unsigned char* temp = (unsigned char*)realloc(b->buffer, b->size + enlargeSize);
        if (temp == NULL)
        {
            /* Codes_SRS_BUFFER_07_018: [BUFFER_enlarge shall return a nonzero result if any error is encountered.] */
            result = __LINE__;
        }
        else
        {
            b->buffer = temp;
            b->size += enlargeSize;
            result = 0;
        }
    }
    return result;
}

/* Codes_SRS_BUFFER_07_021: [BUFFER_size shall place the size of the associated buffer in the size variable and return zero on success.] */
int BUFFER_size(BUFFER_HANDLE handle, size_t* size)
{
    int result;
    if ((handle == NULL) || (size == NULL))
    {
        /* Codes_SRS_BUFFER_07_022: [BUFFER_size shall return a nonzero value for any error that is encountered.] */
        result = __LINE__;
    }
    else
    {
        BUFFER* b = (BUFFER*)handle;
        *size = b->size;
        result = 0;
    }
    return result;
}

/* Codes_SRS_BUFFER_07_024: [BUFFER_append concatenates b2 onto b1 without modifying b2 and shall return zero on success.] */
int BUFFER_append(BUFFER_HANDLE handle1, BUFFER_HANDLE handle2)
{
    int result;
    if ( (handle1 == NULL) || (handle2 == NULL) || (handle1 == handle2) )
    {
        /* Codes_SRS_BUFFER_07_023: [BUFFER_append shall return a nonzero upon any error that is encountered.] */
        result = __LINE__;
    }
    else
    {
        BUFFER* b1 = (BUFFER*)handle1;
        BUFFER* b2 = (BUFFER*)handle2;
        if (b1->buffer == NULL) 
        {
            /* Codes_SRS_BUFFER_07_023: [BUFFER_append shall return a nonzero upon any error that is encountered.] */
            result = __LINE__;
        }
        else if (b2->buffer == NULL)
        {
            /* Codes_SRS_BUFFER_07_023: [BUFFER_append shall return a nonzero upon any error that is encountered.] */
            result = __LINE__;
        }
        else
        {
            unsigned char* temp = (unsigned char*)realloc(b1->buffer, b1->size + b2->size);
            if (temp == NULL)
            {
                /* Codes_SRS_BUFFER_07_023: [BUFFER_append shall return a nonzero upon any error that is encountered.] */
                result = __LINE__;
            }
            else
            {
                b1->buffer = temp;
                // Append the BUFFER
                memcpy(&b1->buffer[b1->size], b2->buffer, b2->size);
                b1->size += b2->size;
                result = 0;
            }
        }
    }
    return result;
}

int BUFFER_prepend(BUFFER_HANDLE handle1, BUFFER_HANDLE handle2)
{
    int result;
    if ((handle1 == NULL) || (handle2 == NULL) || (handle1 == handle2))
    {
        /* : [BUFFER_append shall return a nonzero upon any error that is encountered.] */
        result = __LINE__;
    }
    else
    {
        BUFFER* b1 = (BUFFER*)handle1;
        BUFFER* b2 = (BUFFER*)handle2;
        if (b1->buffer == NULL)
        {
            /* : [BUFFER_append shall return a nonzero upon any error that is encountered.] */
            result = __LINE__;
        }
        else if (b2->buffer == NULL)
        {
            /* : [BUFFER_append shall return a nonzero upon any error that is encountered.] */
            result = __LINE__;
        }
        else
        {
            unsigned char* temp = (unsigned char*)malloc(b1->size + b2->size);
            if (temp == NULL)
            {
                /* : [BUFFER_append shall return a nonzero upon any error that is encountered.] */
                result = __LINE__;
            }
            else
            {
                // Append the BUFFER
                memcpy(temp, b2->buffer, b2->size);
                memcpy(&temp[b2->size], b1->buffer, b1->size);
                free(b1->buffer);
                b1->buffer = temp;
                b1->size += b2->size;
                result = 0;
            }
        }
    }
    return result;
}


/* Codes_SRS_BUFFER_07_025: [BUFFER_u_char shall return a pointer to the underlying unsigned char*.] */
unsigned char* BUFFER_u_char(BUFFER_HANDLE handle)
{
    unsigned char* result;
    if (handle == NULL)
    {
        /* Codes_SRS_BUFFER_07_026: [BUFFER_u_char shall return NULL for any error that is encountered.] */
        result = NULL;
    }
    else
    {
        BUFFER* b = (BUFFER*)handle;
        result = b->buffer;
    }
    return result;
}

/* Codes_SRS_BUFFER_07_027: [BUFFER_length shall return the size of the underlying buffer.] */
size_t BUFFER_length(BUFFER_HANDLE handle)
{
    size_t result;
    if (handle == NULL)
    {
        /* Codes_SRS_BUFFER_07_028: [BUFFER_length shall return zero for any error that is encountered.] */
        result = 0;
    }
    else
    {
        BUFFER* b = (BUFFER*)handle;
        result = b->size;
    }
    return result;
}

BUFFER_HANDLE BUFFER_clone(BUFFER_HANDLE handle)
{
    BUFFER_HANDLE result;
    if (handle == NULL)
    {
        result = NULL;
    }
    else
    {
        BUFFER* suppliedBuff = (BUFFER*)handle;
        BUFFER* b = (BUFFER*)malloc(sizeof(BUFFER) );
        if (b != NULL)
        {
            if ( (b->buffer = (unsigned char*)malloc(suppliedBuff->size) ) == NULL)
            {
                free(b);
                result = NULL;
            }
            else
            {
                memcpy(b->buffer, suppliedBuff->buffer, suppliedBuff->size);
                b->size = suppliedBuff->size;
                result = (BUFFER_HANDLE)b;
            }
        }
        else
        {
            result = NULL;
        }
    }
    return result;
}
