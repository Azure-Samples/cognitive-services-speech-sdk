//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#pragma once

#include <string>
#include <sstream>

#include "speechapi_cxx_properties.h"
#include <speechapi_cxx_string_helpers.h>
#include <speechapi_cxx_utils.h>
#include "speechapi_c_common.h"
#include "speechapi_c_auto_detect_source_lang_config.h"
#include <stdarg.h>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {

/// <summary>
/// Class that defines auto detection source configuration
/// </summary>
class AutoDetectSourceLanguageConfig
{
public:
    /// <summary>
    /// Internal operator used to get underlying handle value.
    /// </summary>
    /// <returns>A handle.</returns>
    explicit operator SPXAUTODETECTSOURCELANGCONFIGHANDLE() const { return m_hconfig; }

    /// <summary>
    /// Creates an instance of the AutoDetectSourceLanguageConfig with source languages
    /// </summary>
    /// <param name="languages">The list of source languages.</param>
    /// <returns>A shared pointer to the new AutoDetectSourceLanguageConfig instance.</returns>
    static std::shared_ptr<AutoDetectSourceLanguageConfig> FromLanguages(const std::vector<SPXSTRING>& languages)
    {
        SPXAUTODETECTSOURCELANGCONFIGHANDLE hconfig = SPXHANDLE_INVALID;
        std::string languagesStr;
        bool isFirst = true;
        for (const SPXSTRING& language : languages)
        {
            if (!isFirst)
            {
                languagesStr += ",";
            }
            isFirst = false;
            languagesStr += Utils::ToUTF8(language);
        }
        SPX_THROW_ON_FAIL(auto_detect_source_lang_config_from_languages(&hconfig, languagesStr.c_str()));
        auto ptr = new AutoDetectSourceLanguageConfig(hconfig);
        return std::shared_ptr<AutoDetectSourceLanguageConfig>(ptr);
    }

    /// <summary>
    /// Destructs the object.
    /// </summary>
    virtual ~AutoDetectSourceLanguageConfig()
    {
        speech_config_release(m_hconfig);
        property_bag_release(m_propertybag);
    }

private:

    /// <summary>
    /// Internal constructor. Creates a new instance using the provided handle.
    /// </summary>
    explicit AutoDetectSourceLanguageConfig(SPXAUTODETECTSOURCELANGCONFIGHANDLE hconfig)
        :m_hconfig(hconfig)
        {
            SPX_THROW_ON_FAIL(auto_detect_source_lang_config_get_property_bag(hconfig, &m_propertybag));
        }

    /// <summary>
    /// Internal member variable that holds the config
    /// </summary>
    SPXAUTODETECTSOURCELANGCONFIGHANDLE m_hconfig;

    /// <summary>
    /// Internal member variable that holds the properties of the speech config
    /// </summary>
    SPXPROPERTYBAGHANDLE m_propertybag;

    DISABLE_COPY_AND_MOVE(AutoDetectSourceLanguageConfig);
};

}}}

