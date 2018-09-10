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
#include "mock_audio_reader.h"
#include "mock_interactive_microphone.h"
#include "mock_kws_engine_adapter.h"
#include "mock_lu_engine_adapter.h"
#include "mock_reco_engine_adapter.h"
#include "mock_wav_file_reader.h"
#include "mock_wav_file_pump.h"
#include "mock_controller.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


bool ShouldMock(const wchar_t* psz)
{
    return SpxGetMockParameterBool(PAL::ToString(psz).c_str(), false);
}

SPX_EXTERN_C void* Mock_CreateModuleObject(const char* className, const char* interfaceName)
{
    SPX_DBG_TRACE_VERBOSE("%s trying to create %s, %s.", __FUNCTION__, className, interfaceName);

    SPX_FACTORY_MAP_BEGIN();
    SPX_FACTORY_MAP_ENTRY(CSpxMockAudioReader, ISpxAudioStreamReader);
    SPX_FACTORY_MAP_ENTRY(CSpxMockInteractiveMicrophone, ISpxAudioPump);
    SPX_FACTORY_MAP_ENTRY(CSpxMockKwsEngineAdapter, ISpxKwsEngineAdapter);
    SPX_FACTORY_MAP_ENTRY(CSpxMockRecoEngineAdapter, ISpxRecoEngineAdapter);
    SPX_FACTORY_MAP_ENTRY(CSpxMockWavFileReader, ISpxAudioFile);
    SPX_FACTORY_MAP_ENTRY(CSpxMockWavFilePump, ISpxAudioFile);
    SPX_FACTORY_MAP_ENTRY(CSpxMockLuEngineAdapter, ISpxLuEngineAdapter);
    SPX_FACTORY_MAP_ENTRY_IF(ShouldMock(L"CARBON-INTERNAL-MOCK-SdkKwsEngine"), CSpxSdkKwsEngineAdapter, ISpxKwsEngineAdapter, CSpxMockKwsEngineAdapter);
    SPX_FACTORY_MAP_ENTRY_IF(ShouldMock(L"CARBON-INTERNAL-MOCK-UspRecoEngine"), CSpxUspRecoEngineAdapter, ISpxRecoEngineAdapter, CSpxMockRecoEngineAdapter);
    SPX_FACTORY_MAP_ENTRY_IF(ShouldMock(L"CARBON-INTERNAL-MOCK-UnidecRecoEngine"), CSpxUnidecRecoEngineAdapter, ISpxRecoEngineAdapter, CSpxMockRecoEngineAdapter);
    SPX_FACTORY_MAP_ENTRY_IF(ShouldMock(L"CARBON-INTERNAL-MOCK-Microphone"), CSpxInteractiveMicrophone, ISpxAudioPump, CSpxMockInteractiveMicrophone);
    SPX_FACTORY_MAP_ENTRY_IF(ShouldMock(L"CARBON-INTERNAL-MOCK-WavFileReader"), CSpxWavFileReader, ISpxAudioFile, CSpxMockWavFileReader);
    SPX_FACTORY_MAP_ENTRY_IF(ShouldMock(L"CARBON-INTERNAL-MOCK-WavFilePump"), CSpxWavFilePump, ISpxAudioFile, CSpxMockWavFilePump);
    SPX_FACTORY_MAP_ENTRY_IF(ShouldMock(L"CARBON-INTERNAL-MOCK-LuisDirectEngine"), CSpxLuisDirectEngineAdapter, ISpxLuEngineAdapter, CSpxMockLuEngineAdapter);
    SPX_FACTORY_MAP_END();
}

SPX_EXTERN_C SPXDLL_EXPORT void* CreateModuleObject(const char* className, const char* interfaceName)
{
    SPX_FACTORY_MAP_BEGIN();
    SPX_FACTORY_MAP_ENTRY_FUNC(Mock_CreateModuleObject);
    SPX_FACTORY_MAP_END();
}


} } } } // Microsoft::CognitiveServices::Speech::Impl
