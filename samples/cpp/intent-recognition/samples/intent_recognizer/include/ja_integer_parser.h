//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//

#pragma once

#include "intent_interfaces.h"
#include "es_integer_parser.h"

namespace Microsoft {
namespace SpeechSDK {
namespace Standalone {
namespace Intent {
namespace Impl {

class CSpxJPIntegerParser : public ISpxIntegerParser
{
public:

    // --- ISpxIntegerParser ---
    Maybe<std::string> Parse(const std::string& input) const override;

private:

    // parsing
    static Maybe<std::vector<Token>> FromString(const std::string& input);
    static Maybe<Token> GetNumericValue(std::string& input);

    // segmentation
    static std::vector<std::vector<Token>> Segment(std::vector<Token> input);
    constexpr static bool IsMultiplier(const uint64_t& input);

    // aggregator
    static Maybe<std::string> Aggregate(std::vector<std::vector<Token>> numbers);
    static Maybe<uint64_t> AsInteger(std::vector<Token> input);

};

}}}}}
