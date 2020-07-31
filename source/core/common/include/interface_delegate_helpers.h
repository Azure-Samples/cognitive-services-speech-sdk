//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// interface_delegate_helpers.h: Implementation declarations/definitions for CSpxDelegate* helpers
//

// ROBCH: Introduced in AUDIO.V3

#pragma once
#include "spxcore_common.h"
#include "shared_ptr_helpers.h"
#include "string_utils.h"
#include <interface_helpers.h>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

template <class T>
class CSpxDelegateToSharedPtrHelper
{
public:
    using Ptr_Type = std::shared_ptr<T>;
    using Delegate_Type = std::shared_ptr<T>;
    using Const_Delegate_Type = std::shared_ptr<T>;

    CSpxDelegateToSharedPtrHelper() = default;
    virtual ~CSpxDelegateToSharedPtrHelper() = default;

    bool IsZombie() const
    {
        return m_zombie;
    }

    void Zombie(bool zombie = true)
    {
        m_zombie = zombie;
    }

    bool IsClear() const
    {
        return !m_ptr;
    }

    void Clear()
    {
        m_ptr = nullptr;
    }

    bool IsReady()
    {
        return !IsZombie() && !IsClear();
    }

    void SetDelegate(Delegate_Type ptr)
    {
        m_ptr = ptr;
    }

    Delegate_Type GetDelegate()
    {
        if (!m_zombie && m_ptr == nullptr)
        {
            InitDelegatePtr(m_ptr);
        }

        return m_zombie ? nullptr : m_ptr;
    }

    Const_Delegate_Type GetConstDelegate() const
    {
        return m_zombie ? nullptr : m_ptr;
    }

protected:
    virtual void InitDelegatePtr(Ptr_Type& ptr) { UNUSED(ptr);  }

private:

    Ptr_Type m_ptr;
    bool m_zombie { false };
};

template<typename I>
void SpxTermAndClearDelegate(CSpxDelegateToSharedPtrHelper<I>& delegateHelper)
{
    auto ptr = delegateHelper.GetDelegate();
    delegateHelper.Zombie(true);
    delegateHelper.Clear();
    SpxTermAndClear(ptr);
}

template<typename I, typename U>
std::shared_ptr<I> SpxQueryInterfaceFromDelegate(CSpxDelegateToSharedPtrHelper<U>& delegateHelper)
{
    auto ptr = delegateHelper.GetDelegate();
    return SpxQueryInterface<I>(ptr);
}

template<typename I, typename U>
const std::shared_ptr<I> SpxQueryInterfaceFromDelegate(const CSpxDelegateToSharedPtrHelper<U>& delegateHelper)
{
    auto ptr = delegateHelper.GetConstDelegate();
    return SpxQueryInterface<I>(ptr);
}


template <class T>
class CSpxDelegateToWeakPtrHelper
{
public:

    CSpxDelegateToWeakPtrHelper() = default;
    virtual ~CSpxDelegateToWeakPtrHelper() = default;

    bool IsZombie() const { return m_zombie; }
    void Zombie(bool zombie = true) { m_zombie = zombie; }

    bool IsClear() { return m_ptr.empty(); }
    void Clear() { m_ptr = nullptr; }

    bool IsReady() { return !IsZombie() && !m_ptr.empty(); }

protected:

    using Ptr_Type = std::weak_ptr<T>;
    using Delegate_Type = std::shared_ptr<T>;
    using Const_Delegate_Type = std::shared_ptr<T>;

    void SetDelegate(Delegate_Type ptr)
    {
        m_ptr = ptr;
    }

    Delegate_Type GetDelegate()
    {
        if (!m_zombie && m_ptr == nullptr)
        {
            InitDelegatePtr(m_ptr);
        }

        return (m_zombie || m_ptr == nullptr || m_ptr.expired())
            ? nullptr
            : m_ptr.lock();
    }

    Const_Delegate_Type GetConstDelegate() const
    {
        return (m_zombie || m_ptr == nullptr || m_ptr.expired())
            ? nullptr
            : m_ptr.lock();
    }

    virtual void InitDelegatePtr(Ptr_Type& /* ptr */) { }

private:

    Ptr_Type m_ptr;
    bool m_zombie { false };
};

template <class T, class ObjectWithSiteT>
class CSpxDelegateToSiteWeakPtrHelper : CSpxDelegateToWeakPtrHelper<T>
{
protected:
    using Ptr_Type = std::weak_ptr<T>;
    using Delegate_Type = std::shared_ptr<T>;

    void InitDelegatePtr(Ptr_Type& /* ptr */) override
    {
        SetDelegate(GetDelegatePtrFromSite());
    }

private:

    Delegate_Type GetDelegatePtrFromSite()
    {
        auto site = static_cast<ObjectWithSiteT*>(this)->GetSite();
        return SpxQueryInterface<T>(site);
    }
};

template<typename T, typename F, typename... Ts>
void InvokeOnDelegate(const std::shared_ptr<T>& ptr, F f, Ts&&... args)
{
    SPX_IFTRUE(ptr, ((ptr.get())->*f)(std::forward<Ts>(args)...));
}

template<typename T, typename F, typename... Ts, typename U>
auto InvokeOnDelegateR(const std::shared_ptr<T>& ptr, F f, U default_value, Ts&&... args) -> decltype(((std::declval<T*>())->*f)(std::forward<Ts>(args)...))
{
    using return_type = decltype(((std::declval<T*>())->*f)(std::forward<Ts>(args)...));
    /* When we have c++17 we could collapse these 2 functions */
    SPX_IFTRUE_RETURN_X(ptr, ((ptr.get())->*f)(std::forward<Ts>(args)...));
    return static_cast<return_type>(default_value);
}

} } } } // Microsoft::CognitiveServices::Speech::Impl
