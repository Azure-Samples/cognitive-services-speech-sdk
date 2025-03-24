//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <initializer_list>
#include <map>
#include <limits>

namespace Microsoft {
namespace SpeechSDK {
namespace Standalone {
namespace Intent {
namespace Impl {

    /// <summary>
    /// Base substring matcher that uses a trie internally to match one or more substrings. It will always return the longest
    /// match it can find. This class is templated so that you can specify the type of the value that is returned when a match
    /// if found
    /// </summary>
    template<typename TValue>
    class SubstringsMatcherBase
    {
    public:
        /// <summary>
        /// The value returned when there was no match
        /// </summary>
        static constexpr size_t NO_MATCH{ std::numeric_limits<size_t>::max() };

    protected:
        using ValueCreator = TValue(*)(bool isMatch, const std::string& subString, const TValue& val);
        using ValueUpdater = void(*)(bool isMatch, const std::string& subString, const TValue& val, bool existingIsMatch, TValue& existing);

        struct SearchNode
        {
            SearchNode() = default;
            SearchNode(const std::string& substring) : substring{ substring }
            {}

            std::string substring;
            bool isMatch { false };
            std::map<char, SearchNode> next;
            TValue value{};
        };

        SearchNode m_root;
        size_t m_maxMatchLen{ 0 };

    protected:
        SubstringsMatcherBase() = default;

    public:
        /// <summary>
        /// Gets the maximum length a substring could be based on the inputs provided
        /// </summary>
        /// <returns>The maximum length</returns>
        size_t MaxMatchLen() const { return m_maxMatchLen; }

        /// <summary>
        /// Finds the next substring match in the specified input. The longest match will always be
        /// returned
        /// </summary>
        /// <param name="input">The input to search in</param>
        /// <param name="value">(Optional) The value for the match that was found</param>
        /// <returns>The index of the match, or <see cref="NO_MATCH"/></returns>
        size_t Find(const std::string& input, TValue* value = nullptr) const
        {
            return Find(input, 0, input.length(), value);
        }

        /// <summary>
        /// Finds the substring match in the specified input. It will start searching from the specified
        /// offset. The longest match will always be returned
        /// </summary>
        /// <param name="input">The input to search in</param>
        /// <param name="offset">The offset to start searching in the input from</param>
        /// <param name="value">(Optional) The value for the match that was found</param>
        /// <returns>The index of the match, or <see cref="NO_MATCH"/></returns>
        size_t Find(const std::string& input, size_t offset, TValue* value = nullptr) const
        {
            return Find(input, offset, input.length() - offset, value);
        }

        /// <summary>
        /// Finds the substring match in the specified input. It will start searching from the specified
        /// offset and only inspect the next specified number of characters. Some matches may be missed
        /// if you specify a count less than the max substring length you specified. The longest match
        /// will always be returned
        /// </summary>
        /// <param name="input">The input to search in</param>
        /// <param name="offset">The offset to start searching in the input from</param>
        /// <param name="count">The maximum number of characters to search from the offset</param>
        /// <param name="value">(Optional) The value for the match that was found</param>
        /// <returns>The index of the match, or NO_MATCH</returns>
        size_t Find(const std::string& input, size_t offset, size_t count, TValue* value = nullptr) const
        {
            size_t stopAt = std::min(offset + count, input.length());
            for (size_t i = offset; i < stopAt;)
            {
                const SearchNode* match = Match(input, i, stopAt, &m_root);
                if (match->isMatch)
                {
                    if (value != nullptr)
                    {
                        *value = match->value;
                    }
                    return i;
                }
                else
                {
                    i += match->substring.length() + 1;
                }
            }

            return NO_MATCH;
        }

    protected:
        static void UpdateSearchTree(
            SearchNode& root,
            const std::string& substring,
            const TValue& value,
            ValueCreator creator,
            ValueUpdater updater)
        {
            if (substring.empty())
            {
                return;
            }

            SearchNode* current = &root;
            for (size_t i = 0; i < substring.length(); i++)
            {
                const auto& pair = current->next.insert(std::make_pair(
                    substring[i],
                    SubstringsMatcherBase::SearchNode(substring.substr(0, i + 1))
                ));

                current = &(pair.first->second);
                bool isMatch = current->substring.length() == substring.length();
                if (pair.second)
                {
                    // newly added node
                    current->value = creator(isMatch, current->substring, value);
                }
                else
                {
                    if (updater) 
                    {
                        updater(isMatch, current->substring, value, current->isMatch, current->value);
                    }

                    current->isMatch |= isMatch;
                }
            }

            current->isMatch = true;
        }

        static const SearchNode* Match(const std::string& input, size_t offset, size_t stopAt, const SearchNode* root)
        {
            const SearchNode* lastMatch = nullptr;
            const SearchNode* current = root;

            for (size_t i = offset; i < stopAt; i++)
            {
                const SearchNode* next;
                auto iter = current->next.find(input[i]);
                if (iter != current->next.end())
                {
                    next = &(iter->second);
                    if (next->isMatch)
                    {
                        lastMatch = next;
                    }
                    
                    current = next;
                }
                else
                {
                    break;
                }
            }

            return lastMatch != nullptr
                ? lastMatch
                : current;
        }
    };

    // *sigh* provide "storage" for this value so GCC can link. As of C++ 17, this is no longer required
    template<typename TVal>
    constexpr size_t SubstringsMatcherBase<TVal>::NO_MATCH;

    /// <summary>
    /// Uses a trie internally to match one or more substrings. It will always return the longest
    /// match it can find
    /// </summary>
    class SubstringsMatcher : public SubstringsMatcherBase<std::string>
    {
    public:
        /// <summary>
        /// Creates an empty matcher
        /// </summary>
        SubstringsMatcher() = default;

        /// <summary>
        /// Creates a substring matcher that will look for all the specified strings
        /// </summary>
        /// <param name="substrings">The substrings to look for</param>
        SubstringsMatcher(const std::vector<std::string>& substrings);

        /// <summary>
        /// Creates a substring matcher that will look for all the specified strings
        /// </summary>
        /// <param name="substrings">The substrings to look for</param>
        SubstringsMatcher(const std::initializer_list<std::string>& substrings);
    };

}}}}}
