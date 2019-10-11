//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// object_init_impl.h: Implementation declarations for ISpxObjectInitImpl
//

// ROBCH: Introduced in AUDIO.V3

#pragma once
#include "spxcore_common.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class ISpxObjectInitImpl : public ISpxObjectInit
{
    void Init() override { }
    void Term() override { }
};

} } } } // Microsoft::CognitiveServices::Speech::Impl
