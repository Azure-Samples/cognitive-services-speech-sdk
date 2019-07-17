#define _CRT_SECURE_NO_WARNINGS

#include "config_manager.h"
#include "Exception.h"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include <cassert>
#include <cstdio>
#include <iostream>

namespace Microsoft
{
namespace speechlib
{
    using namespace rapidjson;

    std::string ConfigManager::GetValue(const std::string& section, const std::string& key)
    {
        return GetStrValue(section, key);
    }

    std::string ConfigManager::GetStrValue(const std::string& section, const std::string& key)
    {
        return get_value(section, key).GetString();
    }

    bool ConfigManager::GetBoolValue(const std::string& section, const std::string& key)
    {
        return get_value(section, key).GetBool();
    }

    int ConfigManager::GetIntValue(const std::string& section, const std::string& key)
    {
        return get_value(section, key).GetInt();
    }


    const GenericValue<UTF8<>>&
    ConfigManager::get_value(const std::string& section, const std::string& key)
    {
        auto config_dom = init();
        const auto section_itr = config_dom.FindMember(section.c_str());
        assert(section_itr != config_dom.MemberEnd());
        assert(section_itr->value.IsObject());
        const auto sub_itr = section_itr->value.FindMember(key.c_str());
        assert(sub_itr != section_itr->value.MemberEnd());
        return sub_itr->value;
    }
} // namespace speechlib
} // namespace Microsoft