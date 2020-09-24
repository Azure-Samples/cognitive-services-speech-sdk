//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "stdafx.h"
#include <map>

#include "test_utils.h"
#include "file_utils.h"
#include "recognizer_utils.h"

SPXTEST_CASE_BEGIN("SourceLanguageConfig::Verify invalid SourceLanguageConfig construction", "[api][cxx]")
{
    SPX_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    REQUIRE_THROWS(SourceLanguageConfig::FromLanguage(""));
    REQUIRE_THROWS(SourceLanguageConfig::FromLanguage("en-US", ""));
    REQUIRE_THROWS(SpeechRecognizer::FromConfig(CurrentSpeechConfig(SpxGetTestTrafficType(__FILE__, __LINE__)), (std::shared_ptr<SourceLanguageConfig>)nullptr, nullptr));
} SPXTEST_CASE_END()

SPXTEST_CASE_BEGIN("SourceLanguageConfig::Verify invalid AutoDetectSourceLanguageConfig construction", "[api][cxx]")
{
    SPX_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    REQUIRE_THROWS(AutoDetectSourceLanguageConfig::FromLanguages({}));
    std::vector<std::shared_ptr<SourceLanguageConfig>> sourceLanguageConfigs;
    REQUIRE_THROWS(AutoDetectSourceLanguageConfig::FromSourceLanguageConfigs({}));
    REQUIRE_THROWS(AutoDetectSourceLanguageConfig::FromSourceLanguageConfigs(sourceLanguageConfigs));
    sourceLanguageConfigs.push_back(nullptr);
    REQUIRE_THROWS(AutoDetectSourceLanguageConfig::FromSourceLanguageConfigs(sourceLanguageConfigs));
    REQUIRE_THROWS(
        SpeechRecognizer::FromConfig(CurrentSpeechConfig(SpxGetTestTrafficType(__FILE__, __LINE__)), (std::shared_ptr<AutoDetectSourceLanguageConfig>)nullptr, nullptr));
} SPXTEST_CASE_END()
