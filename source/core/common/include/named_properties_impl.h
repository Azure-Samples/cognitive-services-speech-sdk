//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// named_properties_impl.h: Implementation definitions for ISpxNamedPropertiesImpl C++ class
//

#pragma once
#include <map>
#include "ispxinterfaces.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class ISpxNamedPropertiesImpl : public ISpxNamedProperties
{
public:

    // --- ISpxNamedProperties

    std::wstring GetStringValue(const wchar_t* name, const wchar_t* defaultValue) const override
    {
        std::unique_lock<std::mutex> lock(m_mutexProperties);
        auto item = m_stringPropertyMap.find(std::wstring(name));
        if (item != m_stringPropertyMap.end())
        {
            SPX_DBG_TRACE_VERBOSE_IF(1, "%s: this=0x%8x; name='%ls'; value='%ls'", __FUNCTION__, this, name, item->second.c_str());
            return item->second;
        }

        lock.unlock();
        auto parentProperties = GetParentProperties();
        if (parentProperties != nullptr)
        {
            return parentProperties->GetStringValue(name, defaultValue);
        }

        return std::wstring(defaultValue);
    }

    void SetStringValue(const wchar_t* name, const wchar_t* value) override
    {
        std::unique_lock<std::mutex> lock(m_mutexProperties);
        m_stringPropertyMap[std::wstring(name)] = value;
        SPX_DBG_TRACE_VERBOSE_IF(1, "%s: this=0x%8x; name='%ls'; value='%ls'", __FUNCTION__, this, name, value);
    }

    bool HasStringValue(const wchar_t* name) const override
    {
        std::unique_lock<std::mutex> lock(m_mutexProperties);
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

    double GetNumberValue(const wchar_t* name, double defaultValue) const override
    {
        std::unique_lock<std::mutex> lock(m_mutexProperties);
        auto item = m_numberPropertyMap.find(std::wstring(name));
        if (item != m_numberPropertyMap.end())
        {
            return item->second;
        }

        lock.unlock();
        auto parentProperties = GetParentProperties();
        if (parentProperties != nullptr)
        {
            return parentProperties->GetNumberValue(name, defaultValue);
        }

        return defaultValue;
    }

    void SetNumberValue(const wchar_t* name, double value) override
    {
        std::unique_lock<std::mutex> lock(m_mutexProperties);
        m_numberPropertyMap[std::wstring(name)] = value;
    }

    bool HasNumberValue(const wchar_t* name) const override
    {
        std::unique_lock<std::mutex> lock(m_mutexProperties);
        if (m_numberPropertyMap.find(name) != m_numberPropertyMap.end())
        {
            return true;
        }

        lock.unlock();
        auto parentProperties = GetParentProperties();
        if (parentProperties != nullptr)
        {
            return parentProperties->HasNumberValue(name);
        }

        return false;
    }

    bool GetBooleanValue(const wchar_t* name, bool defaultValue) const override
    {
        std::unique_lock<std::mutex> lock(m_mutexProperties);
        auto item = m_boolPropertyMap.find(std::wstring(name));
        if (item != m_boolPropertyMap.end())
        {
            return item->second;
        }

        lock.unlock();

        auto parentProperties = GetParentProperties();
        if (parentProperties != nullptr)
        {
            return parentProperties->GetBooleanValue(name, defaultValue);
        }

        return defaultValue;
    }

    void SetBooleanValue(const wchar_t* name, bool value) override
    {
        std::unique_lock<std::mutex> lock(m_mutexProperties);
        m_boolPropertyMap[std::wstring(name)] = value;
    }

    bool HasBooleanValue(const wchar_t* name) const override
    {
        std::unique_lock<std::mutex> lock(m_mutexProperties);
        if (m_boolPropertyMap.find(name) != m_boolPropertyMap.end())
        {
            return true;
        }

        lock.unlock();

        auto parentProperties = GetParentProperties();
        if (parentProperties != nullptr)
        {
            return parentProperties->HasBooleanValue(name);
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

    std::map<std::wstring, std::wstring> m_stringPropertyMap;
    std::map<std::wstring, double> m_numberPropertyMap;
    std::map<std::wstring, bool> m_boolPropertyMap;
};


} } } } // Microsoft::CognitiveServices::Speech::Impl
