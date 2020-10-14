//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
#pragma once

#include <ispxinterfaces.h>
#include <interface_helpers.h>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

template <class T>
class ISpxObjectWithSiteInitImpl : public ISpxObjectWithSite, public ISpxObjectInit
{
public:
    // --- ISpxObjectWithSite
    void SetSite(std::weak_ptr<ISpxGenericSite> site) override
    {
        auto shared = site.lock();
        auto ptr = SpxQueryInterface<T>(shared);
        SPX_IFFALSE_THROW_HR((bool)ptr == (bool)shared, SPXERR_INVALID_ARG);

        if (m_hasSite)
        {
            Term();
            m_site.reset();
            m_hasSite = false;
        }

        m_site = ptr;
        m_hasSite = ptr.get() != nullptr;

        if (m_hasSite)
        {
            Init();
        }
    }

    // --- ISpxObjectInit
    void Init() override
    {
    }

    void Term() override
    {
    }

protected:
    ISpxObjectWithSiteInitImpl() : m_hasSite(false) {}

    std::shared_ptr<T> GetSite() const
    {
        return m_site.lock();
    }

    template<class F>
    void InvokeOnSite(F f) const
    {
        auto site = GetSite();
        if (site != nullptr)
        {
            f(site);
        }
    }

private:
    bool m_hasSite;
    mutable std::weak_ptr<T> m_site;
};


} } } }
