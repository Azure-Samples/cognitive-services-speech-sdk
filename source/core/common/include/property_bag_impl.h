//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// property_bag_impl.h: Implementation definitions for ISpxNamedPropertiesImpl C++ class
//

#pragma once
#include <map>
#include "ispxinterfaces.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class ISpxPropertyBagImpl : public ISpxNamedProperties
{
public:
    // --- ISpxNamedProperties
    void Copy(ISpxNamedProperties* from) override
    {
        std::unique_lock<std::mutex> lock(m_mutexProperties);

        auto p = dynamic_cast<ISpxPropertyBagImpl*>(from);
        SPX_IFTRUE_THROW_HR(p == nullptr, SPXERR_INVALID_ARG);

        for (const auto& it : p->m_stringPropertyMap)
        {
            m_stringPropertyMap.insert(it);
        }
    }

    std::string GetStringValue(const char* name, const char* defaultValue) const override
    {
        std::unique_lock<std::mutex> lock(m_mutexProperties);

        SPX_IFTRUE_THROW_HR(name == nullptr, SPXERR_INVALID_ARG);
        SPX_IFTRUE_THROW_HR(defaultValue == nullptr, SPXERR_INVALID_ARG);

        auto item = m_stringPropertyMap.find(std::string(name));
        if (item != m_stringPropertyMap.end())
        {
            SPX_DBG_TRACE_VERBOSE_IF(1, "%s: this=0x%8x; name='%s'; value='%s'", __FUNCTION__, this, name, item->second.c_str());
            return item->second;
        }

        lock.unlock();
        auto parentProperties = GetParentProperties();
        if (parentProperties != nullptr)
        {
            return parentProperties->GetStringValue(name, defaultValue);
        }

        return std::string(defaultValue);
    }

    void SetStringValue(const char* name, const char* value) override
    {
        std::unique_lock<std::mutex> lock(m_mutexProperties);
        SPX_IFTRUE_THROW_HR(name == nullptr, SPXERR_INVALID_ARG);
        SPX_IFTRUE_THROW_HR(value == nullptr, SPXERR_INVALID_ARG);

        m_stringPropertyMap[std::string(name)] = value;
        SPX_DBG_TRACE_VERBOSE_IF(1, "%s: this=0x%8x; name='%s'; value='%s'", __FUNCTION__, this, name, value);
    }

    bool HasStringValue(const char* name) const override
    {
        std::unique_lock<std::mutex> lock(m_mutexProperties);

        SPX_IFTRUE_THROW_HR(name == nullptr, SPXERR_INVALID_ARG);

        if (m_stringPropertyMap.find(name) != m_stringPropertyMap.end())
        {
            return true;
        }

        lock.unlock();
        auto parentProperties = GetParentProperties();
        if (parentProperties != nullptr)
        {
            return parentProperties->HasStringValue(name);
        }

        return false;
    }

protected:
    virtual std::shared_ptr<ISpxNamedProperties> GetParentProperties() const
    {
        return nullptr;
    }

private:
    mutable std::mutex m_mutexProperties;
    std::map<std::string, std::string> m_stringPropertyMap;
};


} } } } // Microsoft::CognitiveServices::Speech::Impl
