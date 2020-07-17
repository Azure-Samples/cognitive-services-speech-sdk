//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
#pragma once

#include <chrono>
#include <future>
#include <type_traits>

#include <ispxinterfaces.h>

namespace Carbon = Microsoft::CognitiveServices::Speech::Impl;
namespace SDK = Microsoft::CognitiveServices::Speech;

inline auto ConstructDefaultFormat()
{
    Carbon::SpxWaveFormatEx format{ sizeof(Carbon::SPXWAVEFORMATEX) };
    format->wFormatTag = Carbon::WAVE_FORMAT_PCM;
    format->nChannels = 1;
    format->nSamplesPerSec = 16000;
    format->nAvgBytesPerSec = 16000;
    format->nBlockAlign = 1;
    format->wBitsPerSample = 8;
    format->cbSize = 0;
    return format;
}

template<typename F, typename... Args>
void CallIfNotEmpty(F fn, Args&&... args)
{
    if (fn)
    {
        fn(std::forward<Args>(args)...);
    }
}

/* When we move to C++17 replace with std::invoke_result */
template<typename F, typename... Args>
struct InvokeResult
{
    using Type = decltype(std::declval<F>()(std::declval<Args>()...));
};

template<typename F, typename... Args>
using InvokeResultT = typename InvokeResult<F, Args...>::Type;

template<typename F, typename... Args>
auto CallIfNotEmptyR(F fn, InvokeResultT<F, Args...> defaultValue, Args&&... args) -> InvokeResultT<F, Args...>
{
    if (fn)
    {
        return fn(std::forward<Args>(args)...);
    }
    return defaultValue;
}

template<typename... Ts>
struct Environment
{
    Environment(std::shared_ptr<Ts>... items) : m_items{ std::make_tuple(items...) }
    {
    }

    template<typename T>
    std::shared_ptr<T>& Get()
    {
        return std::get<std::shared_ptr<T>>(m_items);
    }
private:
    std::tuple<std::shared_ptr<Ts>...> m_items;
};

template<typename... Ts>
Environment<Ts...> MakeEnvironment(std::shared_ptr<Ts>... items)
{
    return Environment<Ts...>{ items... };
}

template<typename E, typename F>
bool CheckThrow(F fn)
{
    try
    {
        fn();
    }
    catch (const E&)
    {
        return true;
    }
    return false;
}

template<typename F, typename B, class Rep, class Period> 
std::tuple<bool, bool> CheckBlocks(F testFn, B releaseFn, const std::chrono::duration<Rep, Period>& timeout)
{
    std::future<void> future = std::async(std::launch::async, testFn);
    auto status = future.wait_for(timeout);
    releaseFn();
    auto statusAfterRelease = future.wait_for(timeout);
    return { status == std::future_status::timeout, statusAfterRelease == std::future_status::ready };
}

template<typename I, typename T, std::enable_if_t<std::is_base_of<Carbon::ISpxInterfaceBase, T>::value, int> = 0>
std::shared_ptr<I> GetSharedPointer(T* ptr)
{
    auto i = static_cast<Carbon::ISpxInterfaceBase*>(ptr);
    return i->QueryInterface<I>();
}

