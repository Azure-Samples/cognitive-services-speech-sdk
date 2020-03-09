//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "stdafx.h"
#include <map>

#include "test_utils.h"
#include "file_utils.h"
#include "recognizer_utils.h"

TEST_CASE("Verify invalid SourceLanguageConfig construction", "[api][cxx]")
{
    SPX_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    REQUIRE_THROWS(SourceLanguageConfig::FromLanguage(""));
    REQUIRE_THROWS(SourceLanguageConfig::FromLanguage("en-US", ""));
    REQUIRE_THROWS(SpeechRecognizer::FromConfig(CurrentSpeechConfig(), (std::shared_ptr<SourceLanguageConfig>)nullptr, nullptr));
}

TEST_CASE("Verify invalid AutoDetectSourceLanguageConfig construction", "[api][cxx]")
{
    SPX_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    REQUIRE_THROWS(AutoDetectSourceLanguageConfig::FromLanguages({}));
    std::vector<std::shared_ptr<SourceLanguageConfig>> sourceLanguageConfigs;
    REQUIRE_THROWS(AutoDetectSourceLanguageConfig::FromSourceLanguageConfigs({}));
    REQUIRE_THROWS(AutoDetectSourceLanguageConfig::FromSourceLanguageConfigs(sourceLanguageConfigs));
    sourceLanguageConfigs.push_back(nullptr);
    REQUIRE_THROWS(AutoDetectSourceLanguageConfig::FromSourceLanguageConfigs(sourceLanguageConfigs));
    REQUIRE_THROWS(
        SpeechRecognizer::FromConfig(CurrentSpeechConfig(), (std::shared_ptr<AutoDetectSourceLanguageConfig>)nullptr, nullptr));
}
