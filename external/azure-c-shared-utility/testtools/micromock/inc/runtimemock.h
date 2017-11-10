// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef RUNTIMEMOCK_H
#define RUNTIMEMOCK_H

#pragma once

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#ifdef _MSC_VER
#ifndef DISABLE_DETOURS

#include "stdafx.h"
#include "threadsafeglobalmock.h"
#include "detourshookclass.h"
#include "mockcallcomparer.h"
#include "strictunorderedcallcomparer.h"

template<class C>
class CRuntimeMock : public CThreadSafeGlobalMock<C>,
    public CDetoursHookClass
{
public:
    CRuntimeMock(_In_ AUTOMATIC_CALL_COMPARISON performAutomaticCallComparison = AUTOMATIC_CALL_COMPARISON_ON) :
        CThreadSafeGlobalMock<C>(performAutomaticCallComparison)
    {
    }

    virtual ~CRuntimeMock()
    {
    }

    virtual void RegisterFunction(_In_ const void* functionToRegister, _In_ const void* registerFunctionArgument)
    {
        // functionToRegister is the mock class member,
        // registerFunctionArgument should contain the function that should be hooked (detoured)
        InstallHook(registerFunctionArgument, functionToRegister);
    }

    virtual void UnregisterFunction(_In_ const void* functionToUnregister, _In_ const void* unregisterFunctionArgument)
    {
        // functionToRegister is the mock class member,
        // registerFunctionArgument should contain the function that should have been hooked (detoured)
        UninstallHook(unregisterFunctionArgument, functionToUnregister);
    }
};

#endif // DISABLE_DETOURS
#endif // _MSC_VER

#endif // RUNTIMEMOCK_H
