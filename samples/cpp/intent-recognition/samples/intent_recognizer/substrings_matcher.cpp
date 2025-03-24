//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//

#include <algorithm>
#include <limits>
#include <stdexcept>

#include "substrings_matcher.h"

namespace Microsoft {
namespace SpeechSDK {
namespace Standalone {
namespace Intent {
namespace Impl {

    static std::string GetValue(bool isMatch, const std::string&, const std::string& str)
    {
        return isMatch ? str : std::string{};
    }

    static void UpdateValue(
        bool isMatch,
        const std::string& subString,
        const std::string& value,
        bool existingIsMatch,
        std::string& existing)
    {
        (void)subString;

        if (!existingIsMatch && isMatch)
        {
            existing = value;
        }
        else if (existingIsMatch && isMatch && existing != value)
        {
            // should theoretically never happen
            throw std::invalid_argument("Duplicate substring match detected with different values");
        }
    }

    SubstringsMatcher::SubstringsMatcher(const std::vector<std::string>& substrings)
    {
        std::for_each(substrings.begin(), substrings.end(), [&](const std::string& s)
        {
            m_maxMatchLen = std::max(m_maxMatchLen, s.length());
            SubstringsMatcherBase::UpdateSearchTree(m_root, s, s, GetValue, UpdateValue);
        });
    }

    SubstringsMatcher::SubstringsMatcher(const std::initializer_list<std::string>& substrings)
    {
        std::for_each(substrings.begin(), substrings.end(), [&](const std::string& s)
        {
            m_maxMatchLen = std::max(m_maxMatchLen, s.length());
            SubstringsMatcherBase::UpdateSearchTree(m_root, s, s, GetValue, UpdateValue);
        });
    }

}}}}}
