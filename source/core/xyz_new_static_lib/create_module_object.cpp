//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "stdafx.h"

#include <factory_helpers.h>

#include "new_class_1.h"
#include "new_class_2.h"
// ...

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

SPX_EXTERN_C void* XyzNewStaticLib_CreateModuleObject(const char* className, const char* interfaceName)
{
    SPX_FACTORY_MAP_BEGIN();
        // SPX_FACTORY_MAP_ENTRY(CSpxNewClass1, ISpxObjectWithSite);
        // SPX_FACTORY_MAP_ENTRY(CSpxNewClass2, ISpxObjectWithSite);
    SPX_FACTORY_MAP_END();
}


} } } } // Microsoft::CognitiveServices::Speech::Impl
