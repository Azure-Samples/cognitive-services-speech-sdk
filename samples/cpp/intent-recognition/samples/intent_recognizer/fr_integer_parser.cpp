//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//

#include "stdafx.h"

#include <algorithm>
#include <assert.h>
#include <cmath>
#include <limits>
#include <stdexcept>

#include "fr_integer_parser.h"
#include "substrings_matcher.h"

namespace Microsoft {
namespace SpeechSDK {
namespace Standalone {
namespace Intent {
namespace Impl {

enum class NumberType : int8_t
{
    Ignore = std::numeric_limits<int8_t>::min(),
    Separate = -2,
    Whitespace = -1,
    Word = 0,
    Digit,
    Factor
};

static int32_t ten_factor(int64_t num)
{
    if (num == 0) return 0;

    double factor = (double)std::abs(num);
    factor = std::log10(factor);
    factor = std::floor(factor);
    return (int32_t)factor;
}

struct NumberInfo
{
    NumberInfo() :
        str{ },
        value{ 0 },
        type{ NumberType::Ignore },
        factor{ 0 }
    {
    }

    NumberInfo(const char* str, int32_t value) :
        str{ str },
        value{ value },
        type{ NumberType::Word },
        factor{ ten_factor(value) }
    {}

    NumberInfo(const char* str, int32_t value, NumberType type) :
        str{ str },
        value{ value },
        type{ type },
        factor{ ten_factor(value) }
    {}

    std::string str;
    int32_t value;
    NumberType type;
    int32_t factor;
};

class SubstringToIntMatcher : public SubstringsMatcherBase<NumberInfo>
{
public:
    SubstringToIntMatcher(const std::initializer_list<NumberInfo>& matches)
    {
        std::for_each(matches.begin(), matches.end(), [this](const NumberInfo& entry)
            {
                SubstringsMatcherBase::UpdateSearchTree(
                    m_root,
                    entry.str,
                    entry,
                    [](bool isMatch, const std::string&, const NumberInfo& info) -> NumberInfo
                    {
                        return isMatch
                            ? info
                            : NumberInfo{ "", 0, NumberType::Ignore };
                    },
                    [](bool isMatch, const std::string& subStr, const NumberInfo& value, bool existingIsMatch, NumberInfo& existing) -> void
                    {
                        (void)subStr;

                        if (existingIsMatch && isMatch)
                        {
                            // duplicate match detected. Probably a bug
                            throw std::invalid_argument("Inconsistent value specified");
                        }
                        else if (!existingIsMatch && isMatch)
                        {
                            existing = value;
                        }
                    });
            });
    }
};

static const SubstringToIntMatcher MATCHER(
    {
        { " ", 0, NumberType::Whitespace },
        { "-", 0, NumberType::Whitespace },
        { "et", 0, NumberType::Whitespace }, // this is sometimes used before a one e.g. "vingt et un"
        { ".", 0, NumberType::Ignore }, // thousands separator in Europe e.g. 1.000 <-> 1,000
        { ",", 0, NumberType::Separate },

        { "zéro", 0 },
        { "zero", 0 },
        { "un", 1 },
        { "une", 1 },
        { "deux", 2 },
        { "trois", 3 },
        { "quatre", 4 },
        { "cinq", 5 },
        { "six", 6 },
        { "sept", 7 },
        { "huit", 8 },
        { "neuf", 9 },

        { "premier", 1 },
        { "première", 1 },
        { "premiere", 1 },
        { "1er", 1 },
        { "1 er", 1 },
        { "unième", 1},
        { "unieme", 1},
        { "deuxième", 2 },
        { "deuxieme", 2 },
        { "troisième", 3 },
        { "troisieme", 3 },
        { "quatrième", 4 },
        { "quatrieme", 4 },
        { "cinquième", 5 },
        { "cinquieme", 5 },
        { "sixième", 6 },
        { "sixieme", 6 },
        { "septième", 7 },
        { "septieme", 7 },
        { "huitième", 8 },
        { "huitieme", 8 },
        { "neuvième", 9 },
        { "neuvieme", 9 },

        { "dix", 10 },
        { "onze", 11 },
        { "douze", 12 },
        { "treize", 13 },
        { "quatorze", 14 },
        { "quinze", 15 },
        { "seize", 16 },
        { "dix sept", 17 },
        { "dix-sept", 17 },
        { "dix huit", 18 },
        { "dix-huit", 18 },
        { "dix neuf", 19 },
        { "dix-neuf", 19 },

        { "dixième", 10 },
        { "dixieme", 10 },
        { "onzième", 11 },
        { "onzieme", 11 },
        { "douzième", 12 },
        { "douzieme", 12 },
        { "treizième", 13 },
        { "treizieme", 13 },
        { "quatorzième", 14 },
        { "quatorzieme", 14 },
        { "quinzième", 15 },
        { "quinzieme", 15 },
        { "seizième", 16 },
        { "seizieme", 16 },
        { "dix-septième", 17 },
        { "dix-septieme", 17 },
        { "dix septième", 17 },
        { "dix septieme", 17 },
        { "dix-huitième", 18 },
        { "dix-huitieme", 18 },
        { "dix huitième", 18 },
        { "dix huitieme", 18 },
        { "dix-neuvième", 19 },
        { "dix-neuvieme", 19 },
        { "dix neuvième", 19 },
        { "dix neuvieme", 19 },

        { "vingt", 20 },
        { "trente", 30 },
        { "quarante", 40 },
        { "cinquante", 50 },
        { "soixante", 60 },
        { "soixante dix", 70 },
        { "soixante-dix", 70 },
        { "septante", 70 },
        { "quatre vingt", 80 },
        { "quatre-vingt", 80 },
        { "quatre vingts", 80 },
        { "quatre-vingts", 80 },
        { "octante", 80 },
        { "huitante", 80 },
        { "quatre vingt dix", 90 },
        { "quatre vingt-dix", 90 },
        { "quatre-vingt dix", 90 },
        { "quatre-vingt-dix", 90 },
        { "quatre vingts dix", 90 },
        { "quatre vingts-dix", 90 },
        { "quatre-vingts dix", 90 },
        { "quatre-vingts-dix", 90 },
        { "nonante", 90 },

        { "vingtième", 20 },
        { "vingtieme", 20 },
        { "trentième", 30 },
        { "trentieme", 30 },
        { "quarantième", 40 },
        { "quarantieme", 40 },
        { "cinquantième", 50 },
        { "cinquantieme", 50 },
        { "soixantième", 60 },
        { "soixantieme", 60 },
        { "soixante-dixième", 70 },
        { "soixante-dixieme", 70 },
        { "soixante dixième", 70 },
        { "soixante dixieme", 70 },
        { "quatre-vingtième", 80 },
        { "quatre-vingtieme", 80 },
        { "quatre vingtième", 80 },
        { "quatre vingtieme", 80 },
        { "quatre-vingt-dixième", 90 },
        { "quatre-vingt-dixieme", 90 },
        { "quatre vingt-dixième", 90 },
        { "quatre vingt-dixieme", 90 },
        { "quatre-vingt dixième", 90 },
        { "quatre-vingt dixieme", 90 },
        { "quatre vingt dixième", 90 },
        { "quatre vingt dixieme", 90 },

        { "moins", -1, NumberType::Factor },
        { "négative", -1, NumberType::Factor },
        { "negative", -1, NumberType::Factor },
        { "négatif", -1, NumberType::Factor },
        { "negatif", -1, NumberType::Factor },
        { "douzaine", 12, NumberType::Factor },
        { "douzaines", 12, NumberType::Factor },
        { "cent", 100, NumberType::Factor },
        { "cents", 100, NumberType::Factor },
        { "mille", 1000, NumberType::Factor },
        { "milles", 1000, NumberType::Factor },

        { "centième", 100, NumberType::Factor },
        { "centieme", 100, NumberType::Factor },
        { "millième", 1000, NumberType::Factor },
        { "millieme", 1000, NumberType::Factor },

        { "0", 0, NumberType::Digit },
        { "1", 1, NumberType::Digit },
        { "2", 2, NumberType::Digit },
        { "3", 3, NumberType::Digit },
        { "4", 4, NumberType::Digit },
        { "5", 5, NumberType::Digit },
        { "6", 6, NumberType::Digit },
        { "7", 7, NumberType::Digit },
        { "8", 8, NumberType::Digit },
        { "9", 9, NumberType::Digit },

        // Special cases to always prefer to group 70 and 90 words together
        { "soixante-et-onze",   71 },
        { "soixante et-onze",   71 },
        { "soixante-et onze",   71 },
        { "soixante et onze",   71 },
        { "soixante-onze",      71 },
        { "soixante onze",      71 },
        { "soixante-douze",     72 },
        { "soixante douze",     72 },
        { "soixante-treize",    73 },
        { "soixante treize",    73 },
        { "soixante-quatorze",  74 },
        { "soixante quatorze",  74 },
        { "soixante-quinze",    75 },
        { "soixante quinze",    75 },
        { "soixante-seize",     76 },
        { "soixante seize",     76 },
        { "soixante-dix-sept",  77 },
        { "soixante dix-sept",  77 },
        { "soixante-dix sept",  77 },
        { "soixante dix sept",  77 },
        { "soixante-dix-huit",  78 },
        { "soixante dix-huit",  78 },
        { "soixante-dix huit",  78 },
        { "soixante dix huit",  78 },
        { "soixante-dix-neuf",  79 },
        { "soixante dix-neuf",  79 },
        { "soixante-dix neuf",  79 },
        { "soixante dix neuf",  79 },

        { "quatre-vingt-et-onze",   91 },
        { "quatre-vingt et-onze",   91 },
        { "quatre-vingt-et onze",   91 },
        { "quatre-vingt et onze",   91 },
        { "quatre-vingt-onze",      91 },
        { "quatre-vingt onze",      91 },
        { "quatre vingt-et-onze",   91 },
        { "quatre vingt et-onze",   91 },
        { "quatre vingt-et onze",   91 },
        { "quatre vingt et onze",   91 },
        { "quatre vingt-onze",      91 },
        { "quatre vingt onze",      91 },
        { "quatre-vingt-douze",     92 },
        { "quatre-vingt douze",     92 },
        { "quatre vingt-douze",     92 },
        { "quatre vingt douze",     92 },
        { "quatre-vingt-treize",    93 },
        { "quatre-vingt treize",    93 },
        { "quatre vingt-treize",    93 },
        { "quatre vingt treize",    93 },
        { "quatre-vingt-quatorze",  94 },
        { "quatre-vingt quatorze",  94 },
        { "quatre vingt-quatorze",  94 },
        { "quatre vingt quatorze",  94 },
        { "quatre-vingt-quinze",    95 },
        { "quatre-vingt quinze",    95 },
        { "quatre vingt-quinze",    95 },
        { "quatre vingt quinze",    95 },
        { "quatre-vingt-seize",     96 },
        { "quatre-vingt seize",     96 },
        { "quatre vingt-seize",     96 },
        { "quatre vingt seize",     96 },
        { "quatre-vingt-dix-sept",  97 },
        { "quatre-vingt dix-sept",  97 },
        { "quatre-vingt-dix sept",  97 },
        { "quatre-vingt dix sept",  97 },
        { "quatre vingt-dix-sept",  97 },
        { "quatre vingt dix-sept",  97 },
        { "quatre vingt-dix sept",  97 },
        { "quatre vingt dix sept",  97 },
        { "quatre-vingt-dix-huit",  98 },
        { "quatre-vingt dix-huit",  98 },
        { "quatre-vingt-dix huit",  98 },
        { "quatre-vingt dix huit",  98 },
        { "quatre vingt-dix-huit",  98 },
        { "quatre vingt dix-huit",  98 },
        { "quatre vingt-dix huit",  98 },
        { "quatre vingt dix huit",  98 },
        { "quatre-vingt-dix-neuf",  99 },
        { "quatre-vingt dix-neuf",  99 },
        { "quatre-vingt-dix neuf",  99 },
        { "quatre-vingt dix neuf",  99 },
        { "quatre vingt-dix-neuf",  99 },
        { "quatre vingt dix-neuf",  99 },
        { "quatre vingt-dix neuf",  99 },
        { "quatre vingt dix neuf",  99 },
    });

static size_t parse_numbers(const std::string& str, size_t offset, int64_t& value)
{
    int64_t parsed = 0;
    size_t i = 0;
    for (; i + offset < str.length(); i++)
    {
        char c = str[i + offset];
        if (c >= '0' && c <= '9')
        {
            parsed *= 10;
            parsed += c - (int64_t)'0';
        }
        else if (c == '.')
        {
            // thousands separator in French. Ignore it
        }
        else
        {
            break;
        }
    }

    value += parsed;
    return i;
}

Maybe<std::string> CSpxFRIntegerParser::Parse(const std::string& str) const
{
    std::string output;
    output.reserve(21); // expected maximum length of int64_t value + 1

    NumberInfo info;
    int64_t rolling = 0;
    int64_t interim = 0;
    bool precededBySeparator = true;
    bool isNegative = false;
    bool foundNumber = false;
    int32_t prevFactor = std::numeric_limits<int32_t>::max();

    auto appendAndReset = [&]()
        {
            rolling += interim;
            output.append(std::to_string(rolling));

            foundNumber = false;
            rolling = 0;
            interim = 0;
            prevFactor = std::numeric_limits<int32_t>::max();
        };

    for (size_t i = 0; i < str.length();)
    {
        size_t index = MATCHER.Find(str, i, &info);
        if (index == MATCHER.NO_MATCH)
        {
            // we should find at least one of the number substrings. If we don't then the string contains
            // something that isn't recognized as a number value
            return Maybe<std::string>{};
        }
        else if (index != i)
        {
            // if we jumped forward that means we skipped over some input that was not a valid number
            return Maybe<std::string>{};
        }

        i += info.str.length();

        switch (info.type)
        {
        case NumberType::Ignore:
            continue;

        case NumberType::Whitespace:
            precededBySeparator = true;
            continue;

        case NumberType::Separate:
            precededBySeparator = true;
            appendAndReset();
            continue;

        default:
            // continue
            break;
        }

        // negative special case
        if (info.value == -1)
        {
            if (isNegative)
            {
                // cannot have e.g. "negative four hundred negative five" so treat this as invalid
                return Maybe<std::string>{};
            }

            isNegative = true;
            continue;
        }

        if (!precededBySeparator)
        {
            // we expected some sort of separator here because we don't want to consider e.g.
            // "quarantequatre" as valid (should be "quarante quatre" or "quarante-quatre")
            return Maybe<std::string>{};
        }

        precededBySeparator = false;
        foundNumber = true;

        if (info.type == NumberType::Factor)
        {
            if (interim == 0)
            {
                // assume we mean one of the factor
                interim = 1;
            }

            interim *= info.value;
            rolling += interim;
            interim = 0;
            prevFactor = ten_factor(rolling);
        }
        else
        {
            int64_t value = 0;
            int32_t factor = 0;

            if (info.type == NumberType::Word)
            {
                value = info.value;
                factor = info.factor;
            }
            else if (info.type == NumberType::Digit)
            {
                size_t numParsed = parse_numbers(str, i - 1, value);
                if (numParsed == 0)
                {
                    return Maybe<std::string>{};
                }

                i += numParsed - 1;
            }
            else
            {
                assert(!"should not get here");
            }

            // are we going in descending order of factor?
            if (prevFactor > factor)
            {
                interim += value;
            }
            else
            {
                // we are probably doing something like saying "2 3 4" instead of "234", so we "append"
                // the digits to the end of the existing value
                appendAndReset();
                interim = value;
                foundNumber = true;
            }

            prevFactor = factor;
        }
    }

    if (foundNumber)
    {
        appendAndReset();
    }

    if (isNegative && output != "0") // don't return -0 !
    {
        output = "-" + output;
    }

    return output.length() > 0
        ? output
        : Maybe<std::string>{};
}

}}}}}
