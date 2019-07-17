#pragma once

#include <unordered_map>
#include <string>
#include <rapidjson/encodings.h>
#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>
#include "Exception.h"

namespace Microsoft
{
	namespace speechlib
	{
        
		class ConfigManager
		{
            public:
            ConfigManager() = delete;

            /**
             * \brief Get config value
             * \param section: config section
             * \param key: config value
             * \return config value, a string
             */
            static std::string GetValue(const std::string& section, const std::string& key);
            static std::string GetStrValue(const std::string& section, const std::string& key);
            static bool GetBoolValue(const std::string& section, const std::string& key);
            static int GetIntValue(const std::string& section, const std::string& key);
            static rapidjson::Document init();


            private:
            static const rapidjson::GenericValue<rapidjson::UTF8<>>&
            get_value(const std::string& section, const std::string& key);
		};
        inline rapidjson::Document ConfigManager::init()
        {
            FILE* fp = fopen("../conf/config.json", "rb");
            if(fp == nullptr)
            {
                throw Exception("Failed to open file config.json");
            }

            char read_buf[65536];
            rapidjson::FileReadStream is(fp, read_buf, sizeof(read_buf));

            rapidjson::Document doc;
            doc.ParseStream(is);

            fclose(fp);

            assert(doc.IsObject());
            return doc;
        }
        
	}
}
