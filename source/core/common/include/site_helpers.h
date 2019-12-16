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


std::shared_ptr<ISpxGenericSite> SpxGetRootSite();


} } } } // Microsoft::CognitiveServices::Speech::Impl
