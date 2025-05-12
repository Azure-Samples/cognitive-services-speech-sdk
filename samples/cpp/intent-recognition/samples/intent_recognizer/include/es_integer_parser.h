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

// This represents the type of tokens that may come in our input
enum class Tag
{
    // "Number" represents a token found in our internal map of expected values
    Number = 0,
    // "Connector" is placed whenever "y" is found in a number. This "concatenates" numbers in Spanish
    Connector,
    // "Explicit" is used when a numerical value is provided directly as an input
    Explicit,
};

struct Token
{
    // The type of the token as described above
    Tag Type;
    // Its numerical integer value
    uint64_t Value;
    // A mask that represents the digits this number affects when used. For example 1000 would have a mask of 0b1000
    uint16_t DigitMask;

    Token(uint64_t value, uint16_t digitMask, Tag type)
    {
        this->Value = value;
        this->DigitMask = digitMask;
        this->Type = type;
    }

    Token(uint64_t value, uint16_t digit_mask) : Type{ Tag::Number }
    {
        this->Value = value;
        this->DigitMask = digit_mask;
    }

    Token() : Type{ Tag::Connector }, Value{ 0 }, DigitMask{ 0 }
    {}

    bool operator==(const Token& other) const
    {
        return this->Type == other.Type
            && this->Value == other.Value
            && this->DigitMask == other.DigitMask;
    }
};

class CSpxESIntegerParser : public ISpxIntegerParser
{
public:

    // --- ISpxIntegerParser ---
    Maybe<std::string> Parse(const std::string& input) const override;

private:
    // Parser layer
    // Methods that take the input string and tokenized it into processable entities
    static Maybe<std::vector<Token>> FromString(const std::string& input);
    static Maybe<Token> GetNumericValue(std::string& input);

    // Segmentation layer
    // Once an input is parsed, we proceed to check if a group of tokens represent a single number
    static std::vector<std::vector<Token>> Segment(std::vector<Token> input);

    // Aggregation layer
    // After segmentation determines, how many numbers we have, we aggregate the tokens to generate the final result
    static Maybe<std::string> Aggregate(const std::vector<std::vector<Token>>& numbers);
    static Maybe<std::string> Aggregate(const std::vector<Token>& number);
    static Maybe<uint64_t> AsInteger(const std::vector<Token>& input);
    constexpr static bool IsMultiplier(const uint64_t& input);
};

}}}}}
