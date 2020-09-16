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
#include "usp_tts_engine_adapter.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


SPX_EXTERN_C void* TTS_USP_CreateModuleObject(const char* className, const char* interfaceName)
{
    using namespace USP;
    SPX_FACTORY_MAP_BEGIN();
        SPX_FACTORY_MAP_ENTRY(CSpxUspCallbackWrapper, ISpxUspCallbacks);
    SPX_FACTORY_MAP_END();
}


} } } } // Microsoft::CognitiveServices::Speech::Impl
