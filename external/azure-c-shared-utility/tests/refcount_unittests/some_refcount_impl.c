// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include "azure_c_shared_utility/gballoc.h"

#include "azure_c_shared_utility/refcount.h"
#include "some_refcount_impl.h"

typedef struct pos
{
    int x;
}pos;

DEFINE_REFCOUNT_TYPE(pos);

POS_HANDLE Pos_Create(int x)
{
    pos* result = REFCOUNT_TYPE_CREATE(pos);
    if (result != NULL)
    {
        result->x = x;
    }
    return result;
}

POS_HANDLE Pos_Clone(POS_HANDLE posHandle)
{
    
    if (posHandle != NULL)
    {
        pos* p = posHandle;
        INC_REF(pos, p);
    }
    return posHandle;
}

void Pos_Destroy(POS_HANDLE posHandle)
{
    if (posHandle != NULL)
    {
        pos* p = posHandle;
        if (DEC_REF(pos, p) == DEC_RETURN_ZERO)
        {
            free(p);
        }
    }
}
