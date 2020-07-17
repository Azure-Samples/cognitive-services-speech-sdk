//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
#pragma once

#include <exception.h>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

inline void ThrowHRIfTrue(bool condition, SPXHR hr)
{
    if (condition)
    {
        ThrowWithCallstack(hr);
    }
}

} } } }