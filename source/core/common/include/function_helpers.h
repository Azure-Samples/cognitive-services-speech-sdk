//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
#pragma once

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

template<typename... Args, typename I, typename R>
auto resolveOverload(R(I::* member)(Args...)) -> decltype(member)
{
    return member;
}

template<typename I, typename R>
auto resolveOverload(R(I::* member)(void)) -> decltype(member)
{
    return member;
}

} } } }