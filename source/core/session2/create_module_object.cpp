//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "stdafx.h"

#include <factory_helpers.h>

#include "session2.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

SPX_EXTERN_C void* Session2_CreateModuleObject(const char* className, const char* interfaceName)
{
    SPX_FACTORY_MAP_BEGIN();
        SPX_FACTORY_MAP_ENTRY(CSpxSession2, ISpxSession2);
    SPX_FACTORY_MAP_END();
}


} } } } // Microsoft::CognitiveServices::Speech::Impl
