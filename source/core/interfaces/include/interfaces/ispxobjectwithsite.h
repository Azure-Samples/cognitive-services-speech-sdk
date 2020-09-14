//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
#pragma once

#include <memory>

#include "interfaces/base.h"
#include "interfaces/types.h"
#include "interfaces/ispxgenericsite.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class ISpxObjectWithSite : public ISpxInterfaceBaseFor<ISpxObjectWithSite>
{
public:
    virtual void SetSite(std::weak_ptr<ISpxGenericSite> site) = 0;
};

} } } }
