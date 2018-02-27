//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// create_module_object.cpp: Implementation definitions for *CreateModuleObject* methods
//

#include "stdafx.h"

#include "audio_pump.h"
#include "audio_stream_session.h"
#include "default_recognizer_factory.h"
#include "factory_helpers.h"
#include "recognizer.h"
#include "recognition_result.h"
#include "recognition_event_args.h"
#include "session_event_args.h"
#include "wav_file_reader.h"
#include "usp_reco_engine_adapter.h"


namespace CARBON_IMPL_NAMESPACE() {


SPX_EXTERN_C void* PrimaryCarbon_CreateModuleObject(const char* className, const char* interfaceName)
{
    SPX_FACTORY_MAP_BEGIN();
    SPX_FACTORY_MAP_ENTRY(CSpxDefaultRecognizerFactory, ISpxRecognizerFactory);
    SPX_FACTORY_MAP_ENTRY(CSpxAudioPump, ISpxAudioPumpReaderInit);
    SPX_FACTORY_MAP_ENTRY(CSpxAudioStreamSession, ISpxSession);
    SPX_FACTORY_MAP_ENTRY(CSpxRecognitionEventArgs, ISpxRecognitionEventArgs);
    SPX_FACTORY_MAP_ENTRY(CSpxRecognitionResult, ISpxRecognitionResult);
    SPX_FACTORY_MAP_ENTRY(CSpxRecognizer, ISpxRecognizer);
    SPX_FACTORY_MAP_ENTRY(CSpxSessionEventArgs, ISpxSessionEventArgs);
    SPX_FACTORY_MAP_ENTRY(CSpxWavFileReader, ISpxAudioFile);
    SPX_FACTORY_MAP_ENTRY(CSpxUspRecoEngineAdapter, ISpxRecoEngineAdapter);
    SPX_FACTORY_MAP_END();
}

SPX_EXTERN_C SPXDLL_EXPORT void* CreateModuleObject(const char* className, const char* interfaceName)
{
    return PrimaryCarbon_CreateModuleObject(className, interfaceName);
}


} // CARBON_IMPL_NAMESPACE
