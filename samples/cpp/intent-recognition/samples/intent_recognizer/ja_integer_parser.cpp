//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//

#include <codecvt>
#include <deque>
#include <map>
#include <sstream>
#include <stack>
#include <string>
#include <vector>

#include "es_integer_parser.h" // needed for the Token struct
#include "ja_integer_parser.h"

namespace Microsoft {
namespace SpeechSDK {
namespace Standalone {
namespace Intent {
namespace Impl {

struct comparator
{
    bool operator()(const char* lhs, const char* rhs) const
    {
        return std::strcmp(lhs, rhs) < 0;
    }
};

const static std::map<const char*, Token, comparator> NUMBERS
{
    // 0b1 mask
    {"れい", Token(0, 0b1)},
    {"レイ", Token(0, 0b1)},
    {"まる", Token(0, 0b1)},
    {"マル", Token(0, 0b1)},
    {"零", Token(0, 0b1)},
    {"０", Token(0, 0b1)},
    {"0", Token(0, 0b1)},

    {"いち", Token(1, 0b1)},
    {"イチ", Token(1, 0b1)},
    {"一", Token(1, 0b1)},
    {"１", Token(1, 0b1)},
    {"1", Token(1, 0b1)},

    {"に", Token(2, 0b1)},
    {"ニ", Token(2, 0b1)},
    {"二", Token(2, 0b1)},
    {"２", Token(2, 0b1)},
    {"2", Token(2, 0b1)},

    {"さん", Token(3, 0b1)},
    {"サン", Token(3, 0b1)},
    {"三", Token(3, 0b1)},
    {"３", Token(3, 0b1)},
    {"3", Token(3, 0b1)},

    // should I account also for し ?
    {"よん", Token(4, 0b1)},
    {"ヨン", Token(4, 0b1)},
    {"四", Token(4, 0b1)},
    {"し", Token(4, 0b1)},
    {"シ", Token(4, 0b1)},
    {"４", Token(4, 0b1)},
    {"4", Token(4, 0b1)},

    {"ご", Token(5, 0b1)},
    {"ゴ", Token(5, 0b1)},
    {"五", Token(5, 0b1)},
    {"五つ", Token(5, 0b1)},
    {"５", Token(5, 0b1)},
    {"5", Token(5, 0b1)},

    {"ろく", Token(6, 0b1)},
    {"ろっ", Token(6, 0b1)},
    {"ロク", Token(6, 0b1)},
    {"ロッ", Token(6, 0b1)},
    {"六", Token(6, 0b1)},
    {"６", Token(6, 0b1)},
    {"6", Token(6, 0b1)},

    {"なな", Token(7, 0b1)},
    {"ナナ", Token(7, 0b1)},
    {"七", Token(7, 0b1)},
    {"しち", Token(7, 0b1)},
    {"シチ", Token(7, 0b1)},
    {"７", Token(7, 0b1)},
    {"7", Token(7, 0b1)},

    {"はち", Token(8, 0b1)},
    {"はっ", Token(8, 0b1)},
    {"ハチ", Token(8, 0b1)},
    {"ハッ", Token(8, 0b1)},
    {"八", Token(8, 0b1)},
    {"８", Token(8, 0b1)},
    {"8", Token(8, 0b1)},

    {"きゅう", Token(9, 0b1)},
    {"キュウ", Token(9, 0b1)},
    {"九", Token(9, 0b1)},
    {"く", Token(9, 0b1)},
    {"ク", Token(9, 0b1)},
    {"９", Token(9, 0b1)},
    {"9", Token(9, 0b1)},

    // 0b11 mask
    {"じゅう", Token(10, 0b10)},
    {"ジュウ", Token(10, 0b10)},
    {"十", Token(10, 0b10)},

    // 0b111
    {"ひゃく", Token(100, 0b100)},
    {"ヒャク", Token(100, 0b100)},
    {"びゃく", Token(100, 0b100)},
    {"ビャク", Token(100, 0b100)},
    {"ぴゃく", Token(100, 0b100)},
    {"ピャク", Token(100, 0b100)},
    {"百", Token(100, 0b100)}
};

// Section start: Parser
Maybe<Token> CSpxJPIntegerParser::GetNumericValue(std::string& input)
{
    auto result = NUMBERS.find(input.c_str());
    if (result == NUMBERS.end())
    {
        return nullptr;
    }
    else
    {
        return result->second;
    }
}

Maybe<std::vector<Token>> CSpxJPIntegerParser::FromString(const std::string& input)
{
    std::vector<Token> output;

    // Convert the input string to a wide string using a codecvt facet
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    std::wstring winput = converter.from_bytes(input);

    std::vector<wchar_t> accumulator = {};

    // Iterate over the wide string from beginning to end appending characters to the
    // accumulator until we get a match
    for (auto it = winput.begin(); it != winput.end(); ++it)
    {
        // Print each wide character as a UTF-8 encoded string
        accumulator.push_back(*it);
        std::wstring w_accumulated(accumulator.begin(), accumulator.end());
        std::string accumulated = converter.to_bytes(w_accumulated);

        auto token = GetNumericValue(accumulated);
        if (token)
        {
            // std::cout << token.Get().value << std::endl;
            output.push_back(token.Get());
            accumulator = {};
        }
    }
    return output;
}
//Section end: Parser

// Section start: Segmenter
std::vector<std::vector<Token>> CSpxJPIntegerParser::Segment(std::vector<Token> input)
{
    std::vector<std::vector<Token>> output;
    std::deque<Token> acc;

    uint16_t cummulative_mask = 0b0;

    for (auto token : input)
    {
        // update cumulative_mask
        // we adjust the new cummulative mask accoding to whether in new one is a multiplier or not
        uint16_t new_digit_mask = 0b0;
        if (cummulative_mask == 0b0)
        {
            new_digit_mask = token.DigitMask;
        }
        else if (IsMultiplier(token.Value))
        {
            // Our current smallest digit needs to be conflated with the incoming multiplier
            // therefore we take the value, using the mask, to see if present
            // and subtract it from the current cummulative mas
            new_digit_mask = (cummulative_mask & 0b1) * token.DigitMask;
            cummulative_mask -= 1;
            // this means that we have set a multiplier earlier that is greater than the current one
            // therefore we need to restore that information
            if (cummulative_mask > new_digit_mask)
            {
                new_digit_mask += cummulative_mask;
            }
            // TODO for multipliers that would multiply each other we need to add logic here
            // support for 十万 for example (10x10000)
        }
        else
        {
            new_digit_mask = cummulative_mask | token.DigitMask;
        }

        // add token to new group or append to existing group
        if (new_digit_mask > cummulative_mask)
        {
            acc.push_back(token);
            cummulative_mask = new_digit_mask;
        }
        else
        {
            std::vector<Token> as_vector(
                std::make_move_iterator(acc.begin()),
                std::make_move_iterator(acc.end()));

            output.push_back(as_vector);

            // we clear the accumulator
            acc = std::deque<Token>{};
            // We reset the cummulative mask to the carry token added to the accumulator
            // starting the new number
            cummulative_mask = token.DigitMask;
            acc.push_back(token);
        }
    }

    // Residual unaddressed last value in the accumulator
    if (!acc.empty())
    {
        std::vector<Token> as_vector(
            std::make_move_iterator(acc.begin()),
            std::make_move_iterator(acc.end()));

        output.push_back(as_vector);
    }
    return output;
}

constexpr bool CSpxJPIntegerParser::IsMultiplier(const uint64_t& input)
{
    return input == 10 || input == 100;
}
// Section end: Segmenter

// Section start: Aggregator
inline uint64_t AggregateAcc(std::deque<uint64_t> numbers)
{
    uint64_t accumulator = 0;
    while (!numbers.empty())
    {
        auto value = numbers.front();
        numbers.pop_front();
        accumulator += value;
    }

    return accumulator;
}

Maybe<std::string> CSpxJPIntegerParser::Aggregate(std::vector<std::vector<Token>> numbers)
{
    std::ostringstream output;
    auto it = numbers.begin();
    auto end = numbers.end();

    while (it != end) {
        const auto& number = *it;
        auto result = AsInteger(number);
        if (result) {
            output << result.Get();
        } else {
            // TODO do we ignore or abort on failure?
            // return nullptr;
        }
        it++;
    }
    return output.str();
}

Maybe<uint64_t> CSpxJPIntegerParser::AsInteger(std::vector<Token> numbers)
{
    // if the number is only a connector we fail as we don't want to return `0`
    if (numbers.size() == 1 && numbers.front().Type == Tag::Connector)
    {
        return nullptr;
    }

    std::deque<uint64_t> total;

    uint64_t acc = 0LL;
    bool wasLastIterationMultiplication = false;
    for (auto& token : numbers)
    {
        if (token.Type == Tag::Connector)
        {
            continue;
        }
        uint64_t input = token.Value;

        // we only start operating on an accumalator that has been initialised
        if (acc > 0)
        {
            // multipliers are numbers divisible by 1000
            auto isMulti = IsMultiplier(input);

            // accumulator are groups of numbers of length at most xxx
            // namely up to the hundreds digit
            if (isMulti && acc < input)
            {
                wasLastIterationMultiplication = true;
                acc *= input;
            }
            // if did not multiply the accumulator in the previous iteration
            // it means that we are still working in an xxx number group
            // that will be eventually multiplied (unless is the last one)
            else if (!isMulti && acc > input && !wasLastIterationMultiplication)
            {
                wasLastIterationMultiplication = false;
                acc += input;
            }
            // If we have reached a number where that is bigger than the latest
            // accumulator, it means that we commit the result to the accumulator
            // and we start the next group of xxx
            else
            {
                wasLastIterationMultiplication = false;
                total.push_front(acc);
                acc = input;
            }
        }
        // We initialise te accumulator
        else
        {
            acc = input;
        }
    }

    // we break off the for loop before we are able to commit the last bit
    total.push_front(acc);

    // we collapse the deque by adding all its elements
    return AggregateAcc(total);
}
// Section end: Aggregator

 // --- ISpxIntegerParser ---
Maybe<std::string> CSpxJPIntegerParser::Parse(const std::string& input) const
{
    auto parsed = FromString(input);
    if (parsed && !parsed.Get().empty())
    {
        auto segmented = Segment(parsed.Get());
        auto result = Aggregate(segmented);
        return result;
    }
    else
    {
        return nullptr;
    }
}

}}}}}
