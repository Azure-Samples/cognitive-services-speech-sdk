//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// create_module_object.cpp: Implementation definitions for *CreateModuleObject* methods
//

#include "stdafx.h"
#include "create_module_object.h"
#include "create_object_helpers.h"
#include "factory_helpers.h"
#include "unidec_reco_engine_adapter.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


SPX_EXTERN_C void* Unidec_CreateModuleObject(const char* className, const char* interfaceName)
{
    SPX_FACTORY_MAP_BEGIN();
    SPX_FACTORY_MAP_ENTRY(CSpxUnidecRecoEngineAdapter, ISpxRecoEngineAdapter);
    SPX_FACTORY_MAP_END();
}

SPX_EXTERN_C SPXDLL_EXPORT void* CreateModuleObject(const char* className, const char* interfaceName)
{
    SPX_FACTORY_MAP_BEGIN();
    SPX_FACTORY_MAP_ENTRY_FUNC(Unidec_CreateModuleObject);
    SPX_FACTORY_MAP_END();
}


} } } } // Microsoft::CognitiveServices::Speech::Impl
