//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//
#pragma once

#include <string>
#include <type_traits>
#include <utility>

namespace Microsoft {
namespace SpeechSDK {
namespace Standalone {
namespace Intent {
namespace Impl {

template<typename F, typename... Args>
using InvokeResult = decltype(std::declval<F>()(std::declval<Args>()...));

template<typename F, typename Ret, typename... Args>
struct MatchesSignature: std::integral_constant<bool, std::is_same<Ret, InvokeResult<F, Args...>>::value> {};

template<typename T, template<class...> class Tpl>
struct IsSpecialization: std::integral_constant<bool, false> {};

template<template<class...> class Tpl, typename... Ts>
struct IsSpecialization<Tpl<Ts...>, Tpl>: std::integral_constant<bool, true> {};

template<typename T>
struct HasIterator
{
private:
    template <typename U> static constexpr bool test(typename U::iterator*) { return true; }
    template <typename U> static constexpr bool test(...) { return false; }

public:
    static constexpr bool value = test<T>(nullptr);
};

template<typename T>
struct IsContainer: std::integral_constant<bool, HasIterator<T>::value && !IsSpecialization<T, std::basic_string>::value>
{};

}}}}}
