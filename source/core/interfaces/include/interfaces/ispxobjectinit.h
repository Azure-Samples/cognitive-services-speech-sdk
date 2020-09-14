//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
#pragma once

#include "interfaces/base.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class ISpxObjectInit : public ISpxInterfaceBaseFor<ISpxObjectInit>
{
public:
    virtual void Init() = 0;
    virtual void Term() = 0;
};

} } } }
