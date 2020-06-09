//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "translator_languages.h"
#include <string_utils.h>
#include <sstream>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace IntegrationTests {

    using StringUtils = PAL::StringUtils;

    static std::string ToFullCode(const std::string& lang, const std::string& script = "", const std::string& region = "")
    {
        std::ostringstream oss;
        oss << lang;

        if (!script.empty())
        {
            oss << "-";
            oss << script;
        }

        if (!region.empty())
        {
            oss << "-";
            oss << region;
        }

        return oss.str();
    }

    TranslatorLanguageBase::TranslatorLanguageBase(const std::string& lang, const std::string& fullCode)
        : fullCode(fullCode), lang(lang)
    {
        if (lang.empty())
        {
            throw std::invalid_argument("Language code cannot be empty");
        }
    }

    const std::string& TranslatorLanguageBase::Code() const
    {
        return fullCode;
    }

    const std::string& TranslatorLanguageBase::Lang() const
    {
        return lang;
    }

    bool TranslatorLanguageBase::TryParseLanguageCode(const std::string& rawLangCode, std::string& lang, std::string& script, std::string& region)
    {
        lang = std::string{};
        script = std::string{};
        region = std::string{};

        if (rawLangCode.empty())
        {
            return false;
        }

        auto parts = StringUtils::Tokenize(rawLangCode, "-");
        if (parts.size() == 0)
        {
            throw std::invalid_argument("The raw language code is invalid");
        }

        // the first part should always be the language
        lang = StringUtils::ToLower(parts[0]);
        if (lang.empty())
        {
            return false;
        }

        // now extract the region codes and scripts. Please note that this uses
        // VERY simplified parsing that may not be accurate in all cases but is
        // sufficient for our needs
        for (size_t i = 1; i < parts.size() && script.empty() && region.empty(); i++)
        {
            if (parts[i].size() == 2)
            {
                // this is the region code
                region = StringUtils::ToUpper(parts[i]);

                if (lang == "zh" && region == "CN")
                {
                    script = "Hans";
                }
                else if (lang == "zh" && region == "TW")
                {
                    script = "Hant";
                }
            }
            else if (parts[i].size() == 3 && region.empty())
            {
                // special case for us to handle zh-CHS and zh-CHT which are non-standard.
                // Script should always come before the region code
                if (StringUtils::ToUpper(parts[i]) == "CHS")
                {
                    script = "Hans";
                }
                else if (StringUtils::ToUpper(parts[i]) == "CHT")
                {
                    script = "Hant";
                }
            }
            else if (parts[i].size() == 4 && region.empty())
            {
                // We found the script. This should always come before the region code
                script = parts[i];
                script[0] = static_cast<char>(toupper(script[0]));
            }
        }

        // Adjust language codes are returned for Norwegian and Bosnian
        if (lang == "no")
        {
            lang = "nb";
        }
        else if (lang == "bs" && script == "Latn")
        {
            lang = "bs";
            script = std::string{};
        }

        return true;
    }

    TranslatorTextLanguage::TranslatorTextLanguage(const std::string& lang, const std::string& script)
        : TranslatorLanguageBase(lang, ToFullCode(lang, script)), script(script)
    {
    }

    TranslatorTextLanguage TranslatorTextLanguage::Parse(const std::string& rawLang)
    {
        std::string lang, script, region;
        if (TryParseLanguageCode(rawLang, lang, script, region))
        {
            return TranslatorTextLanguage(lang, script);
        }

        throw std::invalid_argument("Invalid language code");
    }

    const std::string& TranslatorTextLanguage::Script() const
    {
        return script;
    }

    bool TranslatorTextLanguage::Equals(const TranslatorLanguageBase& other) const
    {
        return Code() == other.Code();
    }

    TranslatorSpeechLanguage::TranslatorSpeechLanguage(const std::string& lang, const std::string& region, const TranslatorTextLanguage& textLanguage)
        : TranslatorLanguageBase(lang, ToFullCode(lang, {}, region)), textLang(textLanguage)
    {
    }

    TranslatorSpeechLanguage TranslatorSpeechLanguage::Parse(const std::string& rawLang)
    {
        std::string lang, script, region;
        if (TryParseLanguageCode(rawLang, lang, script, region))
        {
            return TranslatorSpeechLanguage(lang, region, TranslatorTextLanguage::Parse(ToFullCode(lang, script)));
        }

        throw std::invalid_argument("Invalid language code");
    }

    const std::string& TranslatorSpeechLanguage::Region() const
    {
        return region;
    }

    const TranslatorTextLanguage& TranslatorSpeechLanguage::TextLanguage() const
    {
        return textLang;
    }

    bool TranslatorSpeechLanguage::Equals(const TranslatorLanguageBase& other) const
    {
        return Code() == other.Code();
    }

    bool TranslatorSpeechLanguage::Equals(const TranslatorTextLanguage& other) const
    {
        return textLang.Equals(other);
    }
}

}}}
