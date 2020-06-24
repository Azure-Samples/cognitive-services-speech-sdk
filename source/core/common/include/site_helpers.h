//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// site_helpers.h: Implementation declarations/definitions for SpxSite* helper methods
//

#pragma once
#include "spxcore_common.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

template <class T>
std::shared_ptr<ISpxGenericSite> SpxSiteFromThis(T* ptr)
{
    return SpxSharedPtrFromThis<ISpxGenericSite>(ptr);
}

// USE_NON_CORE_ROOT_SITE is defined in the CMakeLists.txt in extensions or unit tests.
#ifdef USE_NON_CORE_ROOT_SITE
std::shared_ptr<ISpxGenericSite> SpxGetNonCoreRootSite();  // this is for extensions and unit tests.
#define SpxGetRootSite SpxGetNonCoreRootSite
#else
std::shared_ptr<ISpxGenericSite> SpxGetCoreRootSite();  // this is for creating any objects inside core.
#define SpxGetRootSite SpxGetCoreRootSite

#endif






} } } } // Microsoft::CognitiveServices::Speech::Impl
