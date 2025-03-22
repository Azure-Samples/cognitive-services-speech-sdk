//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//

#pragma once

#include "intent_interfaces.h"

namespace Microsoft {
namespace SpeechSDK {
namespace Standalone {
namespace Intent {
namespace Impl {

class CSpxZHIntegerParser : public ISpxIntegerParser
{
public:
    // --- ISpxIntegerParser ---
    Maybe<std::string> Parse(const std::string& input) const override;

private:
    std::vector<uint64_t> ConvertMatchesToNumVector(const std::vector<std::string>& matched) const;
    std::string ConvertVectorToNum(const std::vector<uint64_t>& nums) const;
    std::vector<uint64_t> ConvertToMultiplicativeAdditive(const std::vector<uint64_t> matches) const;
    std::vector<uint64_t> NormalizeNumbers(std::vector<uint64_t> matches) const;
    bool IsPowerOfTen(const uint64_t number) const;
    bool IsPowerOfTenThousand(const uint64_t number) const;
    bool IsContinuousNum(const std::vector<uint64_t> matches) const;
};

}}}}}
