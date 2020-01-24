//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// extension_site_helpers.cpp: Implementation definitions for SpxGetExtensionRootSite helper methods
//

#include "stdafx.h"
#include "site_helpers.h"
#include "extension_resource_manager.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

/* SpxGetExtensionRootSite creates a singleton of CSpxExtensionResourceManager. Any extension can call this function as a site
   so that an extension can create any core objects that are normally created by PrimaryCarbon_CreateModuleObject.
   An extension can static link with common to get this functionality.
*/
std::shared_ptr<ISpxGenericSite> SpxGetNonCoreRootSite()
{
    return SpxQueryInterface<ISpxGenericSite>(CSpxExtensionResourceManager::GetObjectFactory());
}

} } } } // Microsoft::CognitiveServices::Speech::Impl
