//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// named_properties_impl.h: Implementation definitions for ISpxNamedPropertiesImpl C++ class
//

#pragma once
#include <map>
#include "ispxinterfaces.h"


namespace CARBON_IMPL_NAMESPACE() {


class ISpxNamedPropertiesImpl : public ISpxNamedProperties
{
public:

    // --- ISpxNamedProperties

    std::wstring GetStringValue(const wchar_t* name, const wchar_t* defaultValue) override { return GetValue(m_stringPropertyMap, name, defaultValue); }
    void SetStringValue(const wchar_t* name, const wchar_t* value) override { SetValue(m_stringPropertyMap, name, std::wstring(value)); }
    bool HasStringValue(const wchar_t* name) override { return HasValue(m_stringPropertyMap, name); }

    double GetNumberValue(const wchar_t* name, double defaultValue) override { return GetValue(m_numberPropertyMap, name, defaultValue); }
    void SetNumberValue(const wchar_t* name, double value) override { SetValue(m_numberPropertyMap, name, value); }
    bool HasNumberValue(const wchar_t* name) override { return HasValue(m_numberPropertyMap, name); }

    bool GetBooleanValue(const wchar_t* name, bool defaultValue) override { return GetValue(m_boolPropertyMap, name, defaultValue); }
    void SetBooleanValue(const wchar_t* name, bool value) override { SetValue(m_boolPropertyMap, name, value); }
    bool HasBooleanValue(const wchar_t* name) override { return HasValue(m_boolPropertyMap, name); }


protected:

    template <class V, class D>
    V GetValue(std::map<std::wstring, V>& map, const wchar_t* name, const D& defaultValue)
    {
        SPX_DBG_TRACE_VERBOSE("Getting Named Property '%S'...", name);
        std::unique_lock<std::mutex> lock(m_mutexProperties);

        auto item = map.find(std::wstring(name));
        if (item != map.end())
        {
            return item->second;
        }

        return V(defaultValue);
    }

    template <class V>
    bool HasValue(std::map<std::wstring, V>& map, const wchar_t* name)
    {
        std::unique_lock<std::mutex> lock(m_mutexProperties);
        return map.find(name) != map.end();
    }

    template <class V>
    void SetValue(std::map<std::wstring, V>& map, const wchar_t* name, const V& value)
    {
        SPX_DBG_TRACE_VERBOSE("Setting Named Property '%S'...", name);
        std::unique_lock<std::mutex> lock(m_mutexProperties);
        map[std::wstring(name)] = value;
    }


private:

    std::mutex m_mutexProperties;

    std::map<std::wstring, std::wstring> m_stringPropertyMap;
    std::map<std::wstring, double> m_numberPropertyMap;
    std::map<std::wstring, bool> m_boolPropertyMap;
};


} // CARBON_IMPL_NAMESPACE
