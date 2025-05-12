//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//
// intentapi_cxx_enums.h: Intent recognition API declarations for C++ enumerations
//
#pragma once

namespace Microsoft {
namespace SpeechSDK {
namespace Standalone {
namespace Intent {

/// <summary>
/// Used to define the type of entity used for intent recognition.
/// </summary>
enum class EntityType
{
    /// <summary>
    /// This will match any text that fills the slot.
    /// </summary>
    Any = 0,
    /// <summary>
    /// This will match text that is contained within the list or any text if the mode is set to "fuzzy".
    /// </summary>
    List = 1,
    /// <summary>
    /// This will match cardinal and ordinal integers.
    /// </summary>
    PrebuiltInteger = 2
};

/// <summary>
/// Used to define the type of entity used for intent recognition.
/// </summary>
enum class EntityMatchMode
{
    /// <summary>
    /// This is the basic or default mode of matching based on the EntityType
    /// </summary>
    Basic = 0,
    /// <summary>
    /// This will match only exact matches within the entities phrases.
    /// </summary>
    Strict = 1,
    /// <summary>
    /// This will match text within the slot the entity is in, but not require anything from that text.
    /// </summary>
    Fuzzy = 2
};

/// <summary>
/// Used to define the greediness of the entity.
/// </summary>
enum class EntityGreed
{
    /// <summary>
    /// Lazy will match as little as possible.
    /// </summary>
    Lazy = 0,
    /// <summary>
    /// Greedy will match as much as possible.
    /// </summary>
    Greedy = 1,
};

}}}}
