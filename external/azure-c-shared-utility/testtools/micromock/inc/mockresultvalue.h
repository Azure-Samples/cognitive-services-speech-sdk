// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef MOCKRESULTVALUE_H
#define MOCKRESULTVALUE_H

#pragma once

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "stdafx.h"
#include "mockvalue.h"

template<typename T>
class CMockResultValue :
    public CMockValue<T>
{
public:
    CMockResultValue(T argValue) :
        CMockValue<T>(argValue)
    {
    }
};

#endif // MOCKRESULTVALUE_H
