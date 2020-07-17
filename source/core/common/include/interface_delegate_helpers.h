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


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

template <class T>
class CSpxDelegateToSharedPtrHelper
{
public:

    CSpxDelegateToSharedPtrHelper() = default;
    virtual ~CSpxDelegateToSharedPtrHelper() = default;

    bool IsZombie() const { return m_zombie; }
    void Zombie(bool zombie = true) { m_zombie = zombie; }

    bool IsClear() const { return !m_ptr; }
    void Clear() { m_ptr = nullptr; }

    bool IsReady() { return !IsZombie() && !IsClear(); }

protected:

    using Ptr_Type = std::shared_ptr<T>;
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

        return m_zombie ? nullptr : m_ptr;
    }

    Const_Delegate_Type GetConstDelegate() const
    {
        return m_zombie ? nullptr : m_ptr;
    }

    virtual void InitDelegatePtr(Ptr_Type& ptr) { UNUSED(ptr);  }

private:

    Ptr_Type m_ptr;
    bool m_zombie { false };
};


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

#define SPX_DELEGATE_ACCESSORS(Name, Helper, I)                                             \
    using Delegate_Type = std::shared_ptr< I >;                                     \
    using Const_Delegate_Type = std::shared_ptr<I>;                                 \
    inline Delegate_Type Get ## Name ## Delegate () { return Helper::GetDelegate(); }       \
    inline Const_Delegate_Type Get ## Name ## ConstDelegate () const { return Helper::GetConstDelegate(); } \
    inline void Set ## Name ## Delegate (Delegate_Type ptr) { Helper::SetDelegate(ptr); }   \
    inline bool Is ## Name ## DelegateZombie() { return Helper::IsZombie(); }               \
    inline void Zombie ## Name ## Delegate(bool zombie = true) { Helper::Zombie(zombie); }  \
    inline bool Is ## Name ## DelegateClear() { return Helper::IsClear(); }                 \
    inline void Clear ## Name ## Delegate() { Helper::Clear(); }                            \
    inline bool Is ## Name ## DelegateReady() { return Helper::IsReady(); }

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
