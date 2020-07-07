//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
#pragma once

#include <memory>
#include <chrono>
#include <algorithm>
#include <tuple>

#include <ispxinterfaces.h>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


namespace Details {
    template<uint64_t maxDen>
    std::tuple<uint64_t, uint64_t> GetParametersFromRate(const uint64_t bytesPerSecond)
    {
        uint64_t den{ maxDen };
        uint64_t factor{ 1 };
        for (; (den > 1) && ((bytesPerSecond % den) != 0); den /= 10, factor *= 10);
        return std::make_tuple(bytesPerSecond / den, factor);
    }
}

template<typename Duration>
Duration BytesToDuration(const uint64_t bytes, const uint32_t bytesPerSecond)
{
    constexpr auto durationDen = Duration::period::den;
    const auto params = Details::GetParametersFromRate<durationDen>(bytesPerSecond);
    const auto reducedRate = std::get<0>(params);
    const auto factor = std::get<1>(params);
    auto tickCount = (bytes * factor) / reducedRate;
    return Duration{ tickCount };
}

template<typename Rep, std::intmax_t Den>
uint64_t DurationToBytes(std::chrono::duration<Rep, std::ratio<1, Den>>&& t, const uint32_t bytesPerSecond)
{
    const auto params = Details::GetParametersFromRate<Den>(bytesPerSecond);
    const auto reducedRate = std::get<0>(params);
    const auto factor = std::get<1>(params);
    return (t.count() * reducedRate) / factor;
}

} } } }
