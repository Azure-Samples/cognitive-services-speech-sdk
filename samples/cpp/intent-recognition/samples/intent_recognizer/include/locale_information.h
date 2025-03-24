//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//

#pragma once
#include <string>

#include "intent_interfaces.h"

namespace Microsoft {
namespace SpeechSDK {
namespace Standalone {
namespace Intent {
namespace Impl {
namespace Locales {

    /// <summary>
    /// Gets the default (en) orthography
    /// </summary>
    const OrthographyInformation& default_orthography();

    /// <summary>
    /// Finds the orthography for the specified language
    /// </summary>
    /// <param name="name">The BCP 47 language code (e.g. en, fr, zh)</param>
    /// <returns>The orthography information for that language, or a nullptr if there isn't one specified</returns>
    const OrthographyInformation* find_orthography(const std::string& language);

    namespace Utils
    {
        // French is special because they use non-breaking spaces before and after punctuation.
        // This function looks for punctuation in French that should have a space in front and removes it.
        // This is used for the pattern matching engine to compare without counting the space.
        std::string RemoveLeadingPunctuationSpaceFR(const std::string& input);

        // French is special because they use non-breaking spaces before and after punctuation.
        // This function looks for punctuation in French that should have a space in front and adds it.
        std::string AddLeadingPunctuationSpaceFR(const std::string& input);
    }

}}}}}}
