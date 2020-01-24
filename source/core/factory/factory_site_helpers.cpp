//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// factory_site_helpers.cpp: Implementation definitions for Spx*Site helper methods
//

#include "stdafx.h"
#include "site_helpers.h"
#include "resource_manager.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

std::shared_ptr<ISpxGenericSite> SpxGetCoreRootSite()
{
    // Inside our Carbon implementation, the Resource Manager is a central figure, and is the root of
    // several things, most notably object factories... Also, it's the root "site" object ...
    // For a description of what a "site" is, and how it works within the service provider design pattern
    // see the comments inside resource_manager.h.

    return SpxQueryInterface<ISpxGenericSite>(CSpxResourceManager::GetObjectFactory());
}

} } } } // Microsoft::CognitiveServices::Speech::Impl
