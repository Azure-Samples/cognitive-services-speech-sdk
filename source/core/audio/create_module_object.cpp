//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// create_module_object.cpp: Implementation definitions for *CreateModuleObject* methods
//

#include "stdafx.h"

#include "audio_pump.h"
#include "interactive_microphone.h"
#include "factory_helpers.h"
#include "wav_file_reader.h"
#include "wav_file_pump.h"
#include "stream_pump.h"


namespace CARBON_IMPL_NAMESPACE() {


SPX_EXTERN_C void* AudioLib_CreateModuleObject(const char* className, const char* interfaceName)
{
    SPX_FACTORY_MAP_BEGIN();
    SPX_FACTORY_MAP_ENTRY(CSpxAudioPump, ISpxAudioPumpReaderInit);
    SPX_FACTORY_MAP_ENTRY(CSpxInteractiveMicrophone, ISpxAudioPump);
    SPX_FACTORY_MAP_ENTRY(CSpxWavFileReader, ISpxAudioFile);
    SPX_FACTORY_MAP_ENTRY(CSpxWavFilePump, ISpxAudioFile);
    SPX_FACTORY_MAP_ENTRY(CSpxStreamPump, ISpxStreamPumpReaderInit);
    SPX_FACTORY_MAP_END();
}


} // CARBON_IMPL_NAMESPACE
