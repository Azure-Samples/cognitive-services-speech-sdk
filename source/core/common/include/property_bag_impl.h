//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// property_bag_impl.h: Implementation definitions for ISpxNamedPropertiesImpl C++ class
//

#pragma once
#include <map>
#include "ispxinterfaces.h"
#include "property_id_2_name_map.h"

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
            LogPropertyAndValue(name, item->second);
            return item->second;
        }

        lock.unlock();
        auto parentProperties = GetParentProperties();
        if (parentProperties != nullptr)
        {
            return parentProperties->GetStringValue(name, defaultValue);
        }

        LogPropertyAndValue(name, "");
        return std::string(defaultValue);
    }

    void SetStringValue(const char* name, const char* value) override
    {
        std::unique_lock<std::mutex> lock(m_mutexProperties);
        SPX_IFTRUE_THROW_HR(name == nullptr, SPXERR_INVALID_ARG);
        SPX_IFTRUE_THROW_HR(value == nullptr, SPXERR_INVALID_ARG);

        m_stringPropertyMap[std::string(name)] = value;
        LogPropertyAndValue(name, value);
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

    const std::string m_subKeyName = GetPropertyName(PropertyId::SpeechServiceConnection_Key);
    const std::string m_authTokenName = GetPropertyName(PropertyId::SpeechServiceAuthorization_Token);
    const std::string m_proxyPassName = GetPropertyName(PropertyId::SpeechServiceConnection_ProxyPassword);
    const std::string m_proxyUserName = GetPropertyName(PropertyId::SpeechServiceConnection_ProxyUserName);
    const std::string m_conversationSecretKey = GetPropertyName(PropertyId::Conversation_Secret_Key);

    void LogPropertyAndValue(std::string name, std::string value) const
    {
        // hide property value for subscription key, authorization token, conversation secret key, proxy password and proxy username
        if (name == m_subKeyName || name == m_authTokenName || name == m_conversationSecretKey)
        {
            int l = value.length() > 2 ? 2 : 0;
            value.replace(value.begin(), value.end()-l, value.length()-l, '*');
        }
        if ((name == m_proxyPassName || name == m_proxyUserName) && !value.empty())
        {
            value = std::string("set to non-empty string");
        }
        SPX_DBG_TRACE_VERBOSE("%s: this=0x%8x; name='%s'; value='%s'", __FUNCTION__, this, name.c_str(), value.c_str());
    }
};


} } } } // Microsoft::CognitiveServices::Speech::Impl
