//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
// create_module_object.cpp: Implementation definitions for *CreateModuleObject* methods
//

#include "stdafx.h"
#include "create_module_object.h"
#include "create_object_helpers.h"
#include "factory_helpers.h"
#include "kws_engine_adapter.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

SPX_EXTERN_C void* SDKKWS_CreateModuleObject(const char* className, const char* interfaceName)
{
    SPX_FACTORY_MAP_BEGIN();
        SPX_FACTORY_MAP_ENTRY(CSpxSdkKwsEngineAdapter, ISpxKwsEngineAdapter);
    SPX_FACTORY_MAP_END();
}

#ifndef STATIC_KWS_EXTENSION
SPX_EXTERN_C SPXDLL_EXPORT void* CreateModuleObject(const char* className, const char* interfaceName)
{
    SPX_FACTORY_MAP_BEGIN();
        SPX_FACTORY_MAP_ENTRY_FUNC(SDKKWS_CreateModuleObject);
    SPX_FACTORY_MAP_END();
}
#endif

}}}}
