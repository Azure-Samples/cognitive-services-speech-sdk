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

    bool IsClear() { return !m_ptr; }
    void Clear() { m_ptr = nullptr; }

    bool IsReady() { return !IsZombie() && !IsClear(); }

protected:

    using Ptr_Type = std::shared_ptr<T>;
    using Delegate_Type = std::shared_ptr<T>;

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

} } } } // Microsoft::CognitiveServices::Speech::Impl
