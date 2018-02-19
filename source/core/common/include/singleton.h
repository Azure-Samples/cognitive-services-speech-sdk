//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// singleton.h: Implementation declarations for CSpxSingleton C++ template class
//

#pragma once
#include <mutex>
#include "spxcore_common.h"


namespace CARBON_IMPL_NAMESPACE() {


template <class T, class I>
class CSpxSingleton
{
public:

    static std::shared_ptr<T> GetObject()
    {
        static std::once_flag m_initOnce;

        std::call_once(m_initOnce, InitSingleton);
        return m_sharedPtr;
    };

    static std::shared_ptr<I> GetInterface()
    {
        return GetObject();
    }


private:

    static void InitSingleton()
    {
        auto ptr = new T();
        m_sharedPtr = std::shared_ptr<T>(ptr);
    };


    static std::shared_ptr<T> m_sharedPtr;
};


template <class T, class I>
std::shared_ptr<T> CSpxSingleton<T, I>::m_sharedPtr;


} // CARBON_IMPL_NAMESPACE
