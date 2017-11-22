// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <cstdlib>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "stdafx.h"
#include "micromock.h"
#include "micromockcharstararenullterminatedstrings.h"

bool operator==(_In_ const CMockValue<char*>& lhs, _In_ const CMockValue<char*>& rhs)
{
    if (lhs.GetValue() == NULL)
    {
        if (rhs.GetValue() == NULL)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        if (rhs.GetValue() == NULL)
        {
            return false;
        }
        else
        {
            return (strcmp(lhs.GetValue(), rhs.GetValue()) == 0);
        }
    }
}

 bool operator==(_In_ const CMockValue<const char*>& lhs, _In_ const CMockValue<const char*>& rhs)
{
    if (lhs.GetValue() == NULL)
    {
        if (rhs.GetValue() == NULL)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        if (rhs.GetValue() == NULL)
        {
            return false;
        }
        else
        {
            return (strcmp(lhs.GetValue(), rhs.GetValue()) == 0);
        }
    }
}
