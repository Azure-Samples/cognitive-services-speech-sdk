//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//
// intentapi_cxx_language_understanding_model.h: Intent recognition API declarations for LanguageUnderstandingModel C++ class
//
#pragma once

namespace Microsoft {
namespace SpeechSDK {
namespace Standalone {
namespace Intent {

/// <summary>
/// Represents language understanding model used for intent recognition.
/// </summary>
class LanguageUnderstandingModel 
{
public:

    enum class LanguageUnderstandingModelType
    {
        PatternMatchingModel
        // No other model types are supported.
    };

    /// <summary>
    /// Gets the model type.
    /// </summary>
    /// <returns>An enum representing the type of the model.</returns>
    LanguageUnderstandingModelType GetModelType() const { return m_type; }

protected:

    /// <summary>
    /// Protected constructor for base classes to set type.
    /// </summary>
    /// <param name="type">Language understanding model type.</param>
    LanguageUnderstandingModel(LanguageUnderstandingModelType type) : m_type(type){}

    LanguageUnderstandingModelType m_type;
};

}}}}
