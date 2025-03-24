//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//

#include "stdafx.h"

#include <algorithm>
#include <deque>
#include <iostream>
#include <map>
#include <sstream>
#include <regex>
#include <vector>

#include "es_integer_parser.h"
#include "string_utils.h"

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

// Beginning of Parser section
const static std::map<const char*, const Token, comparator> NUMBERS
{
    // 0b1 mask
    {"cero", Token(0 , 0x1)},
        {"uno", Token(1, 0x1)}, {"un", Token(1, 0x1)}, {"una", Token(1, 0x1)},
        {"dos", Token(2, 0b1)}, {"tres", Token(3, 0b1)}, {"cuatro", Token(4, 0b1)}, {"cinco", Token(5, 0b1)},
        {"seis", Token(6, 0b1)}, {"siete", Token(7, 0b1)}, {"ocho", Token(8, 0b1)}, {"nueve", Token(9, 0b1)},
    // 0b11 mask
    {"diez", Token(10, 0b11)}, {"once", Token(11, 0b11)}, {"doce", Token(12, 0b11)}, {"trece", Token(13, 0b11)},
        {"catorce", Token(14, 0b11)}, {"quince", Token(15, 0b11)}, {"dieciseis", Token(16, 0b11)}, {"diecisiete", Token(17, 0b11)},
        {"dieciocho", Token(18, 0b11)}, {"diecinueve", Token(19, 0b11)},
        {"veinte", Token(20, 0b11)}, {"veintiuno", Token(21, 0b11)}, {"veintidos", Token(22, 0b11)}, {"veintitres", Token(23, 0b11)},
        {"veinticuatro", Token(24, 0b11)}, {"veinticinco", Token(25, 0b11)}, {"veintiseis", Token(26, 0b11)}, {"veintisiete", Token(27, 0b11)},
        {"veintiocho", Token(28, 0b11)}, {"vientinueve", Token(29, 0b11)},
    // 0b10 mask
    {"treinta", Token(30, 0b10)}, {"cuarenta", Token(40, 0b10)}, {"cincuenta", Token(50, 0b10)}, {"sesenta", Token(60, 0b10)},
        {"setenta", Token(70, 0b10)},{"ochenta", Token(80, 0b10)}, {"noventa", Token(90, 0b10)},
    // 0b111 (single exception) and 0b100
    {"cien", Token(100, 0b111)}, {"ciento", Token(100, 0b100)}, {"doscientos", Token(200, 0b100)}, {"trescientos", Token(300, 0b100)},
        {"cuatrocientos", Token(400, 0b100)}, {"quinientos", Token(500, 0b100)}, {"seiscientos", Token(600, 0b100)},
        {"setecientos", Token(700, 0b100)}, {"ochocientos", Token(800, 0b100)}, {"novecientos", Token(900, 0b100)},
    // 0b1000
    {"mil", Token (1'000, 0b1000)},
    // 0b1000000
    {"millon", Token(1'000'000, 0b1000000)}, {"millones", Token(1'000'000, 0b1000000)},
    // 0b1000000000000
    {"billon", Token(1'000'000'000'000, 0b1000000000000)}, {"billones", Token(1'000'000'000'000, 0b1000000000000)}
};

Maybe<Token> CSpxESIntegerParser::GetNumericValue(std::string& input)
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

inline void NormalizeChar(std::string& input, const std::string& target, const std::string& value)
{
    auto pos = input.find(target);
    if (pos != std::string::npos)
    {
        input.replace(pos, target.size(), value);
    }
}

Maybe<std::vector<Token>> CSpxESIntegerParser::FromString(const std::string& input)
{
    std::vector<Token> output;
    auto lowerCase = PAL::StringUtils::ToLower(input);

    NormalizeChar(lowerCase, "á", "a");
    NormalizeChar(lowerCase, "é", "e");
    NormalizeChar(lowerCase, "í", "i");
    NormalizeChar(lowerCase, "ó", "o");
    NormalizeChar(lowerCase, "ú", "u");

    // this filters out spaces and any punctuation that may appear
    auto delimiters = "(\\w+)";
    std::regex regex(delimiters);

    // We scan from right to left
    std::vector<std::string> vector{};
    std::copy(
        std::sregex_token_iterator(lowerCase.cbegin(), lowerCase.cend(), regex),
        std::sregex_token_iterator(),
        std::back_inserter(vector));

    for (auto& token : vector)
    {
        // if the token is a connector type
        if (token == "y")
        {
            output.push_back(Token{});
            continue;
        }
        // if the token is found in the map of hardocded entities
        auto numericValue = GetNumericValue(token);
        if (numericValue)
        {
            output.push_back(numericValue.Get());
        }
        else
        {
            // if the token can be casted to a number
            try
            {
                uint64_t value = std::stol(token);
                auto explicitToken = Token
                {
                    value,
                    0b1111111111111111,
                    Tag::Explicit
                };
                output.push_back(explicitToken);
            }
            catch (std::invalid_argument&)
            {
                SPX_TRACE_ERROR("Error parsing number in pattern matched", token.c_str());
                return nullptr;
            }
            catch (std::out_of_range&)
            {
                SPX_TRACE_ERROR("Error parsing number in pattern matched", token.c_str());
                return nullptr;
            }
        }
    }

    return output;
}
// End of Parser section

// Beginning of segmentation section
std::vector<std::vector<Token>> CSpxESIntegerParser::Segment(std::vector<Token> input)
{
    // the segmenter needs to read the tokens from right to left
    std::reverse(input.begin(), input.end());

    std::vector<std::vector<Token>> output;
    std::deque<Token> acc;

    // uint16_t cummulative_mask = 0b1;

    for (auto token : input)
    {
        // we always want to push numbers if we are just starting, never start with "y"
        if (token.Type != Tag::Connector && acc.empty())
        {
            acc.push_front(token);
        }
        // The front of "acc" is a connector and the token has a valid mask to follow up numbers between [1 and 9]
        // We verify that there is a connector token afterwards
        else if (token.DigitMask == 0b10
            && token.Type != Tag::Connector
            && acc.front().Type == Tag::Connector)
        {
            acc.push_front(token);
        }
        // is the current token not "y" and has non-overlapping masks with the latest "acc"
        // Numbers with masks higher than 0b10 don't use a connector
        else if (token.Type != Tag::Connector
            && token.DigitMask > 0b10
            && (acc.front().Type != Tag::Connector)
            && (token.DigitMask & acc.front().DigitMask) == 0
            && (token.Value > acc.front().Value))
        {
            acc.push_front(token);
        }
        // We only add connector tokens if they come after numbers with a mask that allows it e.g. [1 - 9]
        else if (token.Type == Tag::Connector
            && acc.front().Value != 0
            && acc.front().DigitMask == 0b1)
        {
            acc.push_front(token);
        }
        else
        {
            // we commit contents of the accumulator to the output
            // while reversing the order as the "parser" expects it
            std::vector<Token> as_vector(
                std::make_move_iterator(acc.begin()),
                std::make_move_iterator(acc.end()));

            output.push_back(as_vector);

            // we clear the accumulator
            acc = std::deque<Token>{};
            acc.push_front(token);
        }
    }

    if (!acc.empty())
    {
        std::vector<Token> as_vector(
            std::make_move_iterator(acc.begin()),
            std::make_move_iterator(acc.end()));

        output.push_back(as_vector);
    }

    return output;
}
// End of segmentation section

// Beginning of aggregation section
inline uint64_t AggregateAcc(std::deque<uint64_t> numbers)
{
    auto accumulator = 0LL;
    while (!numbers.empty())
    {
        auto value = numbers.front();
        numbers.pop_front();
        accumulator += value;
    }

    return accumulator;
}

constexpr bool CSpxESIntegerParser::IsMultiplier(const uint64_t& input)
{
    return input % 1000 == 0;
}

Maybe<uint64_t> CSpxESIntegerParser::AsInteger(const std::vector<Token>& numbers)
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

Maybe<std::string> CSpxESIntegerParser::Aggregate(const std::vector<std::vector<Token>>& numbers)
{
    std::ostringstream output;
    auto it = numbers.rbegin();
    auto end = numbers.rend();

    while (it != end)
    {
        const auto& number = *it;
        auto result = AsInteger(number);
        if (result)
        {
            output << result.Get();
        }
        else
        {
            return nullptr;
        }
        it++;
    }
    return output.str();
}

Maybe<std::string> CSpxESIntegerParser::Aggregate(const std::vector<Token>& number)
{
    std::ostringstream output;
    auto result = AsInteger(number);
    if (result) {
        output << result.Get();
    } else {
        return nullptr;
    }
    return output.str();
}
// End of aggregation section

 // --- ISpxIntegerParser ---
Maybe<std::string> CSpxESIntegerParser::Parse(const std::string& input) const
{
    auto parsed = FromString(input);
    if (parsed)
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
