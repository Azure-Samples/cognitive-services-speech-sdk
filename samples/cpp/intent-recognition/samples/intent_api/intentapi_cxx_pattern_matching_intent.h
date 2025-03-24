//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//
// intentapi_cxx_pattern_matching_intent.h: Intent recognition API declarations for PatternMatchingIntent C++ struct
//
#pragma once

#include <string>
#include <vector>

namespace Microsoft {
namespace SpeechSDK {
namespace Standalone {
namespace Intent {

/// <summary>
/// Represents a pattern matching intent used for intent recognition.
/// </summary>
struct PatternMatchingIntent
{
    /// <summary>
    /// Phrases and patterns that will trigger this intent. At least one phrase must exist to be able to
    /// apply this intent to an IntentRecognizer.
    /// </summary> 
    std::vector<std::string> Phrases;

    /// <summary>
    /// An Id used to define this Intent if it is matched. If no Id is specified, then the first phrase in Phrases
    /// will be used.
    /// </summary>
    std::string Id;
};

}}}}
