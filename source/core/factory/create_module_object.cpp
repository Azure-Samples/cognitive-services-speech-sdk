//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// create_module_object.cpp: Implementation definitions for *CreateModuleObject* methods
//

#include "stdafx.h"

#include "speech_api_factory.h"
#include "speech_synthesis_api_factory.h"
#include "factory_helpers.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


#ifdef STATIC_CODEC_EXTENSION
SPX_EXTERN_C void* Codec_CreateModuleObject(const char* className, const char* interfaceName);
#endif
#ifdef STATIC_KWS_EXTENSION
SPX_EXTERN_C void* SDKKWS_CreateModuleObject(const char* className, const char* interfaceName);
#endif
SPX_EXTERN_C void* AudioLib_CreateModuleObject(const char* className, const char* interfaceName);
SPX_EXTERN_C void* SRLib_CreateModuleObject(const char* className, const char* interfaceName);
SPX_EXTERN_C void* TTSLib_CreateModuleObject(const char* className, const char* interfaceName);


SPX_EXTERN_C void* PrimaryCarbon_CreateModuleObject(const char* className, const char* interfaceName)
{
    SPX_FACTORY_MAP_BEGIN();
#ifdef STATIC_CODEC_EXTENSION
    SPX_FACTORY_MAP_ENTRY_FUNC(Codec_CreateModuleObject);
#endif
#ifdef STATIC_KWS_EXTENSION
    SPX_FACTORY_MAP_ENTRY_FUNC(SDKKWS_CreateModuleObject);
#endif
    SPX_FACTORY_MAP_ENTRY_FUNC(AudioLib_CreateModuleObject);
    SPX_FACTORY_MAP_ENTRY_FUNC(SRLib_CreateModuleObject);
    SPX_FACTORY_MAP_ENTRY_FUNC(TTSLib_CreateModuleObject);
    SPX_FACTORY_MAP_ENTRY(CSpxSpeechApiFactory, ISpxSpeechApiFactory);
    SPX_FACTORY_MAP_ENTRY(CSpxSpeechSynthesisApiFactory, ISpxSpeechSynthesisApiFactory);
    SPX_FACTORY_MAP_END();
}

SPX_EXTERN_C SPXDLL_EXPORT void* CreateModuleObject(const char* className, const char* interfaceName)
{
    return PrimaryCarbon_CreateModuleObject(className, interfaceName);
}


} } } } // Microsoft::CognitiveServices::Speech::Impl
