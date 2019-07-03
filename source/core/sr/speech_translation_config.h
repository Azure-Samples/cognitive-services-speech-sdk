//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speech_config.h: Implementation declarations for CSpxSpeechConfig C++ class
//

#pragma once

#include "spxcore_common.h"
#include "interface_helpers.h"
#include "service_helpers.h"
#include "property_bag_impl.h"
#include "ispxinterfaces.h"
#include "speech_config.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class CSpxSpeechTranslationConfig : public CSpxSpeechConfig, public ISpxSpeechTranslationConfig
{
public:
    CSpxSpeechTranslationConfig() {}

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectWithSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxServiceProvider)
        SPX_INTERFACE_MAP_ENTRY(ISpxNamedProperties)
        SPX_INTERFACE_MAP_ENTRY(ISpxSpeechConfig)
        SPX_INTERFACE_MAP_ENTRY(ISpxSpeechTranslationConfig)
        SPX_INTERFACE_MAP_ENTRY(ISpxGenericSite)
    SPX_INTERFACE_MAP_END()

    // --- IServiceProvider
    SPX_SERVICE_MAP_BEGIN()
        SPX_SERVICE_MAP_ENTRY(ISpxNamedProperties)
        SPX_SERVICE_MAP_ENTRY_SITE(GetSite())
    SPX_SERVICE_MAP_END()

    // --- ISpxSpeechTranslationConfig
    virtual void AddTargetLanguage(const std::string& lang) override;
    virtual void RemoveTargetLanguage(const std::string& lang) override;

private:
    friend class CSpxTranslationRecognizer;
    static void AddLangToList(const std::string& lang, std::string& languageList);
    static void RemoveLangFromList(const std::string& lang, std::string& languageList);
    DISABLE_COPY_AND_MOVE(CSpxSpeechTranslationConfig);
};

}}}} // Microsoft::CognitiveServices::Speech::Impl
