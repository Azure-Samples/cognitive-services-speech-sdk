//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//
// intentapi_cxx_pattern_matching_entity.h: Intent recognition API declarations for PatternMatchingEntity C++ struct
//
#pragma once

#include <intentapi_cxx_enums.h>

namespace Microsoft {
namespace SpeechSDK {
namespace Standalone {
namespace Intent {

/// <summary>
/// Represents a pattern matching entity used for intent recognition.
/// </summary>
struct PatternMatchingEntity
{
    /// <summary>
    /// An Id used to define this Entity if it is matched. This id must appear in an intent phrase
    /// or it will never be matched.
    /// </summary>
    std::string Id;

    /// <summary>
    /// The Type of this Entity.
    /// </summary>
    EntityType Type;

    /// <summary>
    /// The EntityMatchMode of this Entity.
    /// </summary>
    EntityMatchMode Mode;

    /// <summary>
    /// If the Type is List these phrases will be used as the list.
    /// </summary>
    std::vector<std::string> Phrases;
};

}}}}
