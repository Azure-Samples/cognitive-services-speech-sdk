// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef GLOBALMOCK_H
#define GLOBALMOCK_H

#pragma once

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "stdafx.h"
#include "mock.h"
#include "mockcallrecorder.h"
#include "micromockexception.h"

template<class C>
class CGlobalMock : public CMock<C>
{
public:
    CGlobalMock(_In_ AUTOMATIC_CALL_COMPARISON performAutomaticCallComparison = AUTOMATIC_CALL_COMPARISON_ON) :
        CMock<C>(performAutomaticCallComparison)
    {
        m_GlobalMockInstance = this;
    }

    virtual ~CGlobalMock()
    {
        m_GlobalMockInstance = NULL;
    }

    _Check_return_
    _Post_satisfies_(return != NULL)
    static CGlobalMock<C>* GetSingleton()
    {
        if (NULL == m_GlobalMockInstance)
        {
            MOCK_THROW(CMicroMockException(MICROMOCK_EXCEPTION_INTERNAL_ERROR,
                _T("Error retrieving singleton")));
        }

        return m_GlobalMockInstance;
    }

protected:
    static CGlobalMock<C>* m_GlobalMockInstance;
};

template<class C>
CGlobalMock<C>* CGlobalMock<C>::m_GlobalMockInstance;

#endif // GLOBALMOCK_H
