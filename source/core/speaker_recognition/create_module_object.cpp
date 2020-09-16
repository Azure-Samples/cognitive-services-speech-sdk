//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// create_module_object.cpp: Implementation definitions for *CreateModuleObject* methods
//

#include "stdafx.h"

#include "factory_helpers.h"
#include "voice_profile.h"
#include "voice_profile_client.h"
#include "http_recog_engine_adapter.h"
#include "http_audio_stream_session.h"
#include "speaker_identification_model.h"
#include "speaker_verification_model.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


SPX_EXTERN_C void* SpeakerRecognition_CreateModuleObject(const char* className, const char* interfaceName)
{
    SPX_FACTORY_MAP_BEGIN();
        SPX_FACTORY_MAP_ENTRY(CSpxVoiceProfile, ISpxVoiceProfile);
        SPX_FACTORY_MAP_ENTRY(CSpxVoiceProfileClient, ISpxVoiceProfileClient);
        SPX_FACTORY_MAP_ENTRY(CSpxHttpRecoEngineAdapter, ISpxHttpRecoEngineAdapter);
        SPX_FACTORY_MAP_ENTRY(CSpxHttpAudioStreamSession, ISpxHttpAudioStreamSession);
        SPX_FACTORY_MAP_ENTRY(CSpxSpeakerIdentificationModel, ISpxSIModel);
        SPX_FACTORY_MAP_ENTRY(CSpxSpeakerVerificationModel, ISpxSVModel);
    SPX_FACTORY_MAP_END();
}


} } } } // Microsoft::CognitiveServices::Speech::Impl
