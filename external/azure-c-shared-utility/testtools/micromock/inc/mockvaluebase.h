// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef MOCKVALUEBASE_H
#define MOCKVALUEBASE_H

#pragma once

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "stdafx.h"

class CMockValueBase
{
public:
    virtual ~CMockValueBase();

public:
    virtual _Check_return_ std::tstring ToString() const = 0;
    virtual _Must_inspect_result_ bool EqualTo(_In_ const CMockValueBase* right) = 0;
};

#endif // MOCKVALUEBASE_H
