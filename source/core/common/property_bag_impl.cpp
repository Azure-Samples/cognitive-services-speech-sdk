//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// property_bag_impl.cpp: Implementation definitions for ISpxNamedPropertiesImpl C++ class
//

#include "stdafx.h"
#include "property_bag_impl.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

void ISpxPropertyBagImpl::Copy(ISpxNamedProperties* from)
{
    std::unique_lock<std::mutex> lock(m_mutexProperties);

    auto p = dynamic_cast<ISpxPropertyBagImpl*>(from);
    SPX_IFTRUE_THROW_HR(p == nullptr, SPXERR_INVALID_ARG);

    for (const auto& it : p->m_stringPropertyMap)
    {
        m_stringPropertyMap.insert(it);
    }
}

std::string ISpxPropertyBagImpl::GetStringValue(const char* name, const char* defaultValue) const
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

void ISpxPropertyBagImpl::SetStringValue(const char* name, const char* value)
{
    std::unique_lock<std::mutex> lock(m_mutexProperties);
    SPX_IFTRUE_THROW_HR(name == nullptr, SPXERR_INVALID_ARG);
    SPX_IFTRUE_THROW_HR(value == nullptr, SPXERR_INVALID_ARG);

    m_stringPropertyMap[std::string(name)] = value;
    LogPropertyAndValue(name, value);
}

bool ISpxPropertyBagImpl::HasStringValue(const char* name) const
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

CSpxStringMap ISpxPropertyBagImpl::FindPrefix(const char* prefix_) const
{
    std::unique_lock<std::mutex> lock(m_mutexProperties);

    std::string prefix{ prefix_ };
    CSpxStringMap wanted;
    for (const auto& v : m_stringPropertyMap)
    {
        if (std::search(v.first.begin(), v.first.end(), prefix.begin(), prefix.end()) != v.first.end())
        {
            auto keyWithoutPrefix = v.first;
            auto seperator = keyWithoutPrefix.find(g_propertyNameSeperator);
            if (seperator != std::string::npos)
            {
                // +1 to move over the seperator
                keyWithoutPrefix.erase(0, seperator + 1);
            }
            wanted[keyWithoutPrefix] = v.second;
        }
    }

    if (!wanted.empty())
    {
        return wanted;
    }

    lock.unlock();
    auto parentProperties = GetParentProperties();
    if (parentProperties != nullptr)
    {
        return parentProperties->FindPrefix(prefix_);
    }
    return wanted;
}

void ISpxPropertyBagImpl::LogPropertyAndValue(std::string name, std::string value) const
{
    constexpr auto subKeyName = GetPropertyName(PropertyId::SpeechServiceConnection_Key);
    constexpr auto authTokenName = GetPropertyName(PropertyId::SpeechServiceAuthorization_Token);
    constexpr auto proxyPassName = GetPropertyName(PropertyId::SpeechServiceConnection_ProxyPassword);
    constexpr auto proxyUserName = GetPropertyName(PropertyId::SpeechServiceConnection_ProxyUserName);
    constexpr auto applicationId = GetPropertyName(PropertyId::Conversation_ApplicationId);

    // hide property value for: subscription key, authorization token, conversation secret key/dialog id, proxy password and proxy username
    if (name == subKeyName || name == authTokenName || name == applicationId)
    {
        int l = value.length() > 2 ? 2 : 0;
        value.replace(value.begin(), value.end() - l, value.length() - l, '*');
    }
    if ((name == proxyPassName || name == proxyUserName) && !value.empty())
    {
        value = std::string("set to non-empty string");
    }
    SPX_DBG_TRACE_VERBOSE("%s: this=0x%p; name='%s'; value='%s'", __FUNCTION__, (void*)this, name.c_str(), value.c_str());
}

} } } } // Microsoft::CognitiveServices::Speech::Impl
