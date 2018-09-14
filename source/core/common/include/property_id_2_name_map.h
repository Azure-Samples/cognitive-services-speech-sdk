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

constexpr auto g_recoModeInteractive = "INTERACTIVE";
constexpr auto g_recoModeDictation = "DICTATION";
constexpr auto g_recoModeConversation = "CONVERSATION";

}}}}
