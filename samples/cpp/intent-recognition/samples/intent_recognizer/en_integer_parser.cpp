//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//

#include "stdafx.h"

#include <regex>
#include <stack>
#include <cmath>
#include <cstring>
#include <sstream>
#include <stdlib.h>

#include "en_integer_parser.h"

namespace Microsoft {
namespace SpeechSDK {
namespace Standalone {
namespace Intent {
namespace Impl {

// Must use this so the maps will compare the values and not the pointers.
struct cmp_str
{
    bool operator()(char const* a, char const* b) const
    {
        return std::strcmp(a, b) < 0;
    }
};

const std::map<const char*, int64_t, cmp_str> CardinalNumberMap
{
    { "a", 1L },
    { "zero", 0L },
    { "oh", 0L },
    { "an", 1L },
    { "one", 1L },
    { "two", 2L },
    { "too", 2L },
    { "to", 2L },
    { "three", 3L },
    { "four", 4L },
    { "for", 4L },
    { "fore", 4L },
    { "five", 5L },
    { "six", 6L },
    { "seven", 7L },
    { "eight", 8L },
    { "nine", 9L },
    { "ten", 10L },
    { "eleven", 11L },
    { "twelve", 12L },
    { "dozen", 12L },
    { "dozens", 12L },
    { "thirteen", 13L },
    { "fourteen", 14L },
    { "fifteen", 15L },
    { "sixteen", 16L },
    { "seventeen", 17L },
    { "eighteen", 18L },
    { "nineteen", 19L },
    { "twenty", 20L },
    { "thirty", 30L },
    { "forty", 40L },
    { "fifty", 50L },
    { "sixty", 60L },
    { "seventy", 70L },
    { "eighty", 80L },
    { "ninety", 90L },
    { "hundred", 100L },
    { "thousand", 1000L },
    { "million", 1000000L },
    { "mln", 1000000L },
    { "billion", 1000000000L },
    { "bln", 1000000000L },
    { "trillion", 1000000000000L },
    { "tln", 1000000000000L },
    { "lakh", 100000L },
    { "crore", 10000000L },
    { "score", 20L }
};

const std::map<const char*, int64_t, cmp_str> RoundNumberMap
{
    { "hundred", 100L },
    { "thousand", 1000L },
    { "million", 1000000L },
    { "mln", 1000000L },
    { "billion", 1000000000L },
    { "bln", 1000000000L },
    { "trillion", 1000000000000L },
    { "tln", 1000000000000L },
    { "lakh", 100000L },
    { "crore", 10000000L },
    { "hundredth", 100L },
    { "thousandth", 1000L },
    { "millionth", 1000000L },
    { "billionth", 1000000000L },
    { "trillionth", 1000000000000L },
    { "hundredths", 100L },
    { "thousandths", 1000L },
    { "millionths", 1000000L },
    { "billionths", 1000000000L },
    { "trillionths", 1000000000000L },
    { "dozen", 12L },
    { "dozens", 12L },
    { "k", 1000L },
    { "m", 1000000L },
    { "mm", 1000000L },
    { "mil", 1000000L },
    { "g", 1000000000L },
    { "b", 1000000000L },
    { "t", 1000000000000L }
};

const std::map<const char*, int64_t, cmp_str> OrdinalNumberMap
{
    { "first", 1L },
    { "second", 2L },
    { "secondary", 2L },
    { "half", 2L },
    { "third", 3L },
    { "fourth", 4L },
    { "quarter", 4L },
    { "fifth", 5L },
    { "sixth", 6L },
    { "seventh", 7L },
    { "eighth", 8L },
    { "ninth", 9L },
    { "nineth", 9L },
    { "tenth", 10L },
    { "eleventh", 11L },
    { "twelfth", 12L },
    { "thirteenth", 13L },
    { "fourteenth", 14L },
    { "fifteenth", 15L },
    { "sixteenth", 16L },
    { "seventeenth", 17L },
    { "eighteenth", 18L },
    { "nineteenth", 19L },
    { "twentieth", 20L },
    { "thirtieth", 30L },
    { "fortieth", 40L },
    { "fiftieth", 50L },
    { "sixtieth", 60L },
    { "seventieth", 70L },
    { "eightieth", 80L },
    { "ninetieth", 90L },
    { "hundredth", 100L },
    { "thousandth", 1000L },
    { "millionth", 1000000L },
    { "billionth", 1000000000L },
    { "trillionth", 1000000000000L }
    // We don't support fractions but may in the future.
    //{ "firsts", 1L },
    //{ "halves", 2L },
    //{ "thirds", 3L },
    //{ "fourths", 4L },
    //{ "quarters", 4L },
    //{ "fifths", 5L },
    //{ "sixths", 6L },
    //{ "sevenths", 7L },
    //{ "eighths", 8L },
    //{ "ninths", 9L },
    //{ "nineths", 9L },
    //{ "tenths", 10L },
    //{ "elevenths", 11L },
    //{ "twelfths", 12L },
    //{ "thirteenths", 13L },
    //{ "fourteenths", 14L },
    //{ "fifteenths", 15L },
    //{ "sixteenths", 16L },
    //{ "seventeenths", 17L },
    //{ "eighteenths", 18L },
    //{ "nineteenths", 19L },
    //{ "twentieths", 20L },
    //{ "thirtieths", 30L },
    //{ "fortieths", 40L },
    //{ "fiftieths", 50L },
    //{ "sixtieths", 60L },
    //{ "seventieths", 70L },
    //{ "eightieths", 80L },
    //{ "ninetieths", 90L },
    //{ "hundredths", 100L },
    //{ "thousandths", 1000L },
    //{ "millionths", 1000000L },
    //{ "billionths", 1000000000L },
    //{ "trillionths", 1000000000000L }
};

const char* NegativeRegex = "(-|minus|negative).*";
const char* NumberPatternRegex = R"(\b(\d+|\d+(rd|st|th|nd)|and| -|negative|minus|seventeen|eighteen|thousand|trillion|nineteen|thirteen|fourteen|sixteen|seventy|fifteen|hundred|billion|million|twenty|ninety|twelve|dozens|thirty|eighty|eleven|forty|seven|crore|sixty|eight|three|fifty|dozen|lakh|nine|zero|five|four|bln|two|mln|one|tln|ten|six|an|a|seventeenths|seventieths|nineteenths|thousandths|seventeenth|trillionths|thirteenths|fourteenths|eighteenths|eightieths|seventieth|sixteenths|trillionth|thousandth|fourteenth|fifteenths|thirtieths|twentieths|millionths|eighteenth|nineteenth|hundredths|ninetieths|billionths|thirteenth|elevenths|thirtieth|sixteenth|twentieth|fiftieths|eightieth|ninetieth|fortieths|fifteenth|secondary|billionth|millionth|hundredth|sixtieths|fortieth|sixtieth|twelfths|eleventh|fiftieth|sevenths|quarters|eighths|twelfth|seventh|quarter|fourths|nineths|tenths|fourth|thirds|sixths|halves|ninths|firsts|second|nineth|fifths|eighth|third|ninth|first|tenth|fifth|sixth|half|oh|for|too|to|fore)\b)";

// Note: The number parsing from this section is based on the open source project and converted from the C# code there.
// https://github.com/microsoft/Recognizers-Text
Maybe<std::string> CSpxENIntegerParser::Parse(const std::string& input) const
{
    try
    {
        std::regex numberPatternRegex(NumberPatternRegex, std::regex_constants::icase);
        std::smatch sm;
        std::string stringToProcess = input;
        int64_t result = 0;
        bool found = false;

        // Remove locale number punctuation.
        stringToProcess.erase(std::remove(stringToProcess.begin(), stringToProcess.end(), ','), stringToProcess.end());

        auto matchBegin = std::sregex_iterator(stringToProcess.begin(), stringToProcess.end(), numberPatternRegex);
        auto matchEnd = std::sregex_iterator();
        std::vector<std::string> matches;

        for (std::sregex_iterator iter = matchBegin; iter != matchEnd; ++iter)
        {
            auto match = *iter;

            // If there is ever a prefix or suffix not containing a number, then there are words captured that are not numbers.
            // In this case we should empty matches and break. This means we are returning no number.

            //trim the strings of spaces and '-'s
            auto prefix = match.prefix().str();
            prefix.erase(0, prefix.find_first_not_of(' '));
            prefix.erase(0, prefix.find_first_not_of('-'));
            prefix.erase(prefix.find_last_not_of(' ') + 1);
            prefix.erase(prefix.find_last_not_of('-') + 1);
            auto suffix = match.suffix().str();
            suffix.erase(0, suffix.find_first_not_of(' '));
            suffix.erase(0, suffix.find_first_not_of('-'));
            suffix.erase(suffix.find_last_not_of(' ') + 1);
            suffix.erase(suffix.find_last_not_of('-') + 1);

            if ((!prefix.empty() && !std::regex_search(prefix, sm, numberPatternRegex))
                || (!suffix.empty() && !std::regex_search(suffix, sm, numberPatternRegex)))
            {
                matches.clear();
                break;
            }

            // If we matched, we want the group not the whole string or the beginning word boundary group so we get match[1].
            matches.push_back(match[1].str());
        }

        if (matches.size() != 0)
        {
            bool isNegative = false;
            // Check for negatives
            std::regex negativeRegex(NegativeRegex);
            std::cmatch cm;
            if (std::regex_match(input.c_str(), cm, negativeRegex))
            {
                isNegative = true;
            }

            auto nums = ConvertMatchesToNumVector(matches);

            if (!nums.empty())
            {
                auto strResult = ConvertVectorToNum(nums);
                char* endptr = NULL;
                // errno can be set to any non-zero value by a library function call
                // regardless of whether there was an error, so it needs to be cleared
                // in order to check the error set by strtol
                errno = 0;
                auto numResult = strtol(strResult.c_str(), &endptr, 10);
                if (endptr != strResult.c_str() && errno == 0)
                {
                    isNegative ?
                        result -= numResult :
                        result += numResult;
                    found = true;
                }
            }
        }

        return found ? Maybe<std::string>(std::to_string(result)) : Maybe<std::string>();;
    }
    catch (const std::regex_error& ex)
    {
        SPX_TRACE_ERROR("REGEX: Error: %s", ex.what());
        UNUSED(ex);
        return Maybe<std::string>();
    }
}

std::vector<uint64_t> CSpxENIntegerParser::ConvertMatchesToNumVector(const std::vector<std::string>& matches) const
{
    std::vector<uint64_t> results;

    for (auto& match : matches)
    {
        auto cardinal = CardinalNumberMap.find(match.c_str());
        if (cardinal != CardinalNumberMap.end())
        {
            results.push_back(cardinal->second);
            continue;
        }
        auto ordinal = OrdinalNumberMap.find(match.c_str());
        if (ordinal != OrdinalNumberMap.end())
        {
            results.push_back(ordinal->second);
            continue;
        }

        auto pullOutDigitNum = [&](auto& regex, const auto& str)
        {
            std::cmatch cm;
            if (std::regex_match(str.c_str(), cm, regex))
            {
                // errno can be set to any non-zero value by a library function call
                // regardless of whether there was an error, so it needs to be cleared
                // in order to check the error set by strtol
                errno = 0;
                char* endptr = NULL;
                auto digitNum = strtol(str.c_str(), &endptr, 10);
                if (endptr != str.c_str() && errno == 0)
                {
                    results.push_back(digitNum);
                }
            }
        };

        try
        {
            std::regex digitOrdinalRegex("\\d+(nd|st|rd|th)");
            pullOutDigitNum(digitOrdinalRegex, match);

            std::regex digitRegex("\\d+");
            pullOutDigitNum(digitRegex, match);
        }
        catch (const std::regex_error& ex)
        {
            SPX_TRACE_ERROR("REGEX: Error: %s", ex.what());
            UNUSED(ex);
        }
    }

    return results;
}

struct ValueStruct
{
    uint64_t Value;
    // Digit mask is a bit mask to show what digits are figured. "1040" = 1010
    uint32_t DigitMask;
    // Cover mask is a convenience mask to show how big the number is in digits. "1040" = 1111
    uint32_t CoverMask;

    explicit ValueStruct(uint64_t value)
    {
        Value = value;
        auto c{ 0 };
        uint32_t mask{ 0 };
        uint32_t coverMask{ 0 };
        coverMask = ~coverMask;
        do
        {
            mask |= ((value % 10) != 0) << c++;
            coverMask <<= 1;
        } while ((value /= 10) != 0);
        CoverMask = ~coverMask;
        DigitMask = mask;
    }

    ValueStruct& operator<<(unsigned int digits)
    {
        auto order = static_cast<unsigned int>(std::pow(10, digits));
        Value *= order;
        DigitMask <<= digits;
        auto updatedCoverMask = ~CoverMask;
        updatedCoverMask <<= digits;
        CoverMask = ~updatedCoverMask;
        return *this;
    }

    ValueStruct operator+(ValueStruct& other)
    {
        return ValueStruct{ Value + other.Value };
    }

    ValueStruct operator*(ValueStruct& other)
    {
        return ValueStruct{ Value * other.Value };
    }

    ValueStruct operator*(uint64_t other)
    {
        return ValueStruct{ Value * other };
    }
};

std::string CSpxENIntegerParser::ConvertVectorToNum(const std::vector<uint64_t>& nums) const
{
    std::vector<ValueStruct> valueStructs{};

    for (auto num : nums)
    {
        valueStructs.push_back(ValueStruct(num));
    }

    ValueStruct incompleteResult{ 0 };

    std::deque<ValueStruct> resultStack{};

    bool previousZero = false;
    for (auto& currentStruct : valueStructs)
    {
        if (resultStack.size() == 0)
        {
            resultStack.push_back(currentStruct);
        }
        // Someone has said "2 0 4"
        else if (currentStruct.Value == 0 || previousZero)
        {
            incompleteResult = resultStack.back();
            resultStack.pop_back();
            incompleteResult = incompleteResult * 10;
            resultStack.push_back(incompleteResult);
            if (previousZero && currentStruct.Value != 0)
            {
                previousZero = false;
                resultStack.push_back(currentStruct);
            }
            else
            {
                previousZero = true;
            }
        }
        // Current struct is separate and covers some of the same digits. Might be a different number.
        // Could also be a common expression such as 19 80 5
        else if ((currentStruct.DigitMask & resultStack.back().DigitMask) != 0)
        {
            resultStack.push_back(currentStruct);
        }
        // This means the new one 'currentStruct' is lower order of magnitude "100 20 5"
        // currentStruct does not cover same digits as result
        else if ((currentStruct.DigitMask < resultStack.back().DigitMask) && ((currentStruct.CoverMask & resultStack.back().DigitMask) == 0))
        {
            resultStack.push_back(currentStruct);
        }
        // Someone has said "0-99 hundred" and lets shift mask by 2
        else if ((currentStruct.Value == 100) && (resultStack.back().DigitMask < 4))
        {
            incompleteResult = resultStack.back();
            resultStack.pop_back();
            resultStack.push_back(incompleteResult * currentStruct);
        }
        // handle "round" numbers that are 100 or greater.
        else if ((currentStruct.Value > 99) && (currentStruct.Value % 10 == 0))
        {
            // we are the first element and we should just push
            if (resultStack.size() == 0)
            {
                resultStack.push_back(currentStruct);
            }
            // the last number in the stack is larger than this one so don't combine
            else if (resultStack.back().CoverMask > currentStruct.CoverMask)
            {
                resultStack.push_back(currentStruct);
            }
            // they are the same and should be multiplied. 1 100 100
            else if (resultStack.back().CoverMask == currentStruct.CoverMask)
            {
                auto last = resultStack.back();
                resultStack.pop_back();
                last = last * currentStruct;

            }
            // The resultStack number is smaller and they should be combined until this isn't true, then multiplied.
            // Example: 2 100 30 5 1000 | s = 1000, resultStack = {200, 35}
            else
            {
                while ((resultStack.size() != 0) && (resultStack.back().CoverMask < currentStruct.CoverMask))
                {
                    incompleteResult = resultStack.back();
                    resultStack.pop_back();
                    // This was the last one
                    if (resultStack.size() == 0 || resultStack.back().CoverMask >= currentStruct.CoverMask)
                    {
                        incompleteResult = incompleteResult * currentStruct;

                    }
                    // There are more results and we should keep adding them up.
                    else
                    {
                        auto stackTop = resultStack.back();
                        resultStack.pop_back();
                        resultStack.push_back(incompleteResult + stackTop);
                    }
                }

                // Now that we have collapsed and multiplied, figure out if we should add to the result.
                // 1 million 234 thousand.
                if (resultStack.size() != 0 && incompleteResult.CoverMask < resultStack.back().CoverMask)
                {
                    auto stackTop = resultStack.back();
                    resultStack.pop_back();
                    incompleteResult = incompleteResult + stackTop;
                }
                resultStack.push_back(incompleteResult);
            }
        }
        else
        {
            resultStack.push_back(currentStruct);
        }
    }
    std::vector<uint64_t> numbers{};
    incompleteResult = ValueStruct{ 0 };
    // Collapse any numbers possible 1234000 500 60 7 can be collapsed and added.
    // Also handle common expressions such as 15 hundred
    for (auto& currentStruct : resultStack)
    {
        if (incompleteResult.DigitMask == 0)
        {
            incompleteResult = currentStruct;
        }
        // Result is less than 100 and currentStruct is 10'currentStruct
        else if ((incompleteResult.DigitMask <= 3) && (currentStruct.DigitMask & 2))
        {
            incompleteResult.Value = incompleteResult.Value * 100 + currentStruct.Value;
            incompleteResult.DigitMask = (incompleteResult.DigitMask << 2) + currentStruct.DigitMask;
        }
        // CurrentStruct is smaller without conflicting digits and therefore we can add.
        else if (((incompleteResult.DigitMask & currentStruct.DigitMask) == 0) && (incompleteResult.Value > currentStruct.Value))
        {
            incompleteResult.Value = incompleteResult.Value + currentStruct.Value;
            incompleteResult.DigitMask |= currentStruct.DigitMask;
        }
        else
        {
            numbers.push_back(incompleteResult.Value);
            incompleteResult = currentStruct;
        }
    }

    numbers.push_back(incompleteResult.Value);

    std::ostringstream resultStringStream;
    for (auto& num : numbers)
    {
        resultStringStream << num;
    }
    return resultStringStream.str();
}

}}}}}
