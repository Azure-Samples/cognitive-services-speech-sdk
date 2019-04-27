//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// property_id_2_name_map.h: internal maping function from id to its name
//

#pragma once

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

const char* GetPropertyName(const PropertyId& id);

constexpr const char* g_recoModeInteractive = "INTERACTIVE";
constexpr const char* g_recoModeDictation = "DICTATION";
constexpr const char* g_recoModeConversation = "CONVERSATION";

constexpr const char* g_audioSourceMicrophone = "Microphones";
constexpr const char* g_audioSourceStream = "Stream";
constexpr const char* g_audioSourceFile = "File";

constexpr const char* KeywordConfig_EnableKeywordVerification = "KeywordConfig_EnableKeywordVerification";
}}}}
