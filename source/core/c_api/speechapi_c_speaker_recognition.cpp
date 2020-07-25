//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_c_speaker_recognition.cpp: Public API definitions for Speaker Recognition related C methods

#include "stdafx.h"
#include <speechapi_c_common.h>
#include "speechapi_c_utils.h"
#include "create_object_helpers.h"
#include "handle_helpers.h"
#include "handle_table.h"
#include "platform.h"
#include "site_helpers.h"
#include "string_utils.h"
#include "spxdebug.h"
#include <assert.h>

using namespace Microsoft::CognitiveServices::Speech;
using namespace Microsoft::CognitiveServices::Speech::Impl;
using namespace std;

SPXAPI create_voice_profile_from_id(SPXVOICEPROFILEHANDLE * phVoiceProfile, const char* id)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, id == nullptr || !(*id));
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, phVoiceProfile == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        *phVoiceProfile = SPXHANDLE_INVALID;

        auto voiceProfile = SpxCreateObjectWithSite<ISpxVoiceProfile>("CSpxVoiceProfile", SpxGetRootSite());

        voiceProfile->SetProfileId(id);

        auto voiceProfileHandles = CSpxSharedPtrHandleTableManager::Get<ISpxVoiceProfile, SPXPARTICIPANTHANDLE>();
        *phVoiceProfile = voiceProfileHandles->TrackHandle(voiceProfile);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI voice_profile_client_release_handle(SPXVOICEPROFILECLIENTHANDLE hvoiceclient)
{
    if (hvoiceclient == SPXHANDLE_INVALID)
    {
        return SPX_NOERROR;
    }
    else if (Handle_IsValid<SPXVOICEPROFILECLIENTHANDLE, ISpxVoiceProfileClient>(hvoiceclient))
    {
        return Handle_Close<SPXVOICEPROFILECLIENTHANDLE, ISpxVoiceProfileClient>(hvoiceclient);
    }
    else
    {
        return SPXERR_INVALID_HANDLE;
    }
}

SPXAPI voice_profile_release_handle(SPXVOICEPROFILEHANDLE hvoiceprofile)
{
    if (hvoiceprofile == SPXHANDLE_INVALID)
    {
        return SPX_NOERROR;
    }
    else if (Handle_IsValid<SPXVOICEPROFILEHANDLE, ISpxVoiceProfile>(hvoiceprofile))
    {
        return Handle_Close<SPXVOICEPROFILEHANDLE, ISpxVoiceProfile>(hvoiceprofile);
    }
    else
    {
        return SPXERR_INVALID_HANDLE;
    }
}

SPXAPI speaker_recognizer_release_handle(SPXSPEAKERIDHANDLE phspeakerid)
{
    if (phspeakerid == SPXHANDLE_INVALID)
    {
        return SPX_NOERROR;
    }
    else if (Handle_IsValid<SPXSPEAKERIDHANDLE, ISpxVoiceProfileClient>(phspeakerid))
    {
        return Handle_Close<SPXSPEAKERIDHANDLE, ISpxVoiceProfileClient>(phspeakerid);
    }
    else
    {
        return SPXERR_INVALID_HANDLE;
    }
}

SPXAPI voice_profile_get_id(SPXVOICEPROFILEHANDLE hvoiceprofile, char * psz, uint32_t * pcch)
{
    return RetrieveStringValue(hvoiceprofile, psz, pcch, &ISpxVoiceProfile::GetProfileId);
}

SPXAPI enroll_voice_profile(SPXVOICEPROFILECLIENTHANDLE hVoiceProfileClient, SPXVOICEPROFILEHANDLE hVoiceProfile, SPXAUDIOCONFIGHANDLE hAudioInput, SPXRESULTHANDLE * phProfileResult)
{
    SPX_IFTRUE_THROW_HR(hVoiceProfileClient == nullptr, SPXERR_INVALID_ARG);
    SPX_IFTRUE_THROW_HR(hVoiceProfile == nullptr, SPXERR_INVALID_ARG);

    SPX_IFTRUE_THROW_HR(phProfileResult == nullptr, SPXERR_INVALID_ARG);

    SPXAPI_INIT_HR_TRY(hr)
    {
        *phProfileResult = SPXHANDLE_INVALID;

        auto client = GetInstance<ISpxVoiceProfileClient>(hVoiceProfileClient);

        auto factory = SpxQueryService<ISpxSpeechApiFactory>(client);
        SPX_IFTRUE_THROW_HR(factory == nullptr, SPXERR_RUNTIME_ERROR);

        auto voice_profile = GetInstance<ISpxVoiceProfile>(hVoiceProfile);
        auto audio_input = TryGetInstance<ISpxAudioConfig>(hAudioInput); // hAudioInput could be nullptr, that is OK.

        auto session = SpxQueryService<ISpxHttpAudioStreamSession>(client);
        auto audio_init = SpxQueryInterface<ISpxAudioStreamSessionInit>(session);
        factory->InitSessionFromAudioInputConfig(audio_init, audio_input);

        auto type = voice_profile->GetType();
        auto id = voice_profile->GetProfileId();
        auto result = client->ProcessProfileAction(ISpxVoiceProfileClient::Action::Enroll, type, std::move(id));

        auto result_handles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognitionResult, SPXRESULTHANDLE>();
        *phProfileResult = result_handles->TrackHandle(result);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI create_voice_profile(SPXVOICEPROFILECLIENTHANDLE hclient, int id, const char* locale, SPXVOICEPROFILEHANDLE *pProfile)
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, hclient == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, pProfile == nullptr);
    // locale is a must have input
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, locale == nullptr || !(*locale));

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto client = GetInstance<ISpxVoiceProfileClient>(hclient);

        auto voice_profile = client->Create(static_cast<VoiceProfileType>(id), locale);

        auto voice_profile_handles = CSpxSharedPtrHandleTableManager::Get<ISpxVoiceProfile, SPXVOICEPROFILEHANDLE>();
        *pProfile = voice_profile_handles->TrackHandle(voice_profile);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI voice_profile_client_get_property_bag(SPXVOICEPROFILECLIENTHANDLE hclient, SPXPROPERTYBAGHANDLE *hpropbag)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        *hpropbag = SPXHANDLE_INVALID;
        auto client = GetInstance<ISpxVoiceProfileClient>(hclient);
        auto namedProperties = SpxQueryInterface<ISpxNamedProperties>(client);

        *hpropbag = CSpxSharedPtrHandleTableManager::TrackHandle<ISpxNamedProperties, SPXVOICEPROFILECLIENTHANDLE>(namedProperties);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI delete_voice_profile(SPXVOICEPROFILECLIENTHANDLE hclient, SPXVOICEPROFILEHANDLE hProfileHandle, SPXRESULTHANDLE *phresult)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        *phresult = SPXHANDLE_INVALID;
        auto client = GetInstance<ISpxVoiceProfileClient>(hclient);
        auto profile = GetInstance<ISpxVoiceProfile>(hProfileHandle);

        auto result = client->ProcessProfileAction(ISpxVoiceProfileClient::Action::Delete, profile->GetType(), profile->GetProfileId());
        auto result_handles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognitionResult, SPXRESULTHANDLE>();
        *phresult = result_handles->TrackHandle(result);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI reset_voice_profile(SPXVOICEPROFILECLIENTHANDLE hclient, SPXVOICEPROFILEHANDLE hProfileHandle, SPXRESULTHANDLE *phresult)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        *phresult = SPXHANDLE_INVALID;
        auto client = GetInstance<ISpxVoiceProfileClient>(hclient);
        auto profile = GetInstance<ISpxVoiceProfile>(hProfileHandle);

        auto result = client->ProcessProfileAction(ISpxVoiceProfileClient::Action::Reset, profile->GetType(), profile->GetProfileId());
        auto result_handles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognitionResult, SPXRESULTHANDLE>();
        *phresult = result_handles->TrackHandle(result);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI speaker_recognizer_get_property_bag(SPXSPEAKERIDHANDLE phspeakerid, SPXPROPERTYBAGHANDLE *hpropbag)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        *hpropbag = SPXHANDLE_INVALID;
        auto client = GetInstance<ISpxVoiceProfileClient>(phspeakerid);
        auto namedProperties = SpxQueryInterface<ISpxNamedProperties>(client);

        *hpropbag = CSpxSharedPtrHandleTableManager::TrackHandle<ISpxNamedProperties, SPXVOICEPROFILECLIENTHANDLE>(namedProperties);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI speaker_identification_model_create(SPXSIMODELHANDLE* psimodel)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, psimodel == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {

        *psimodel = SPXHANDLE_INVALID;
        auto model = SpxCreateObjectWithSite<ISpxSIModel>("CSpxSpeakerIdentificationModel", SpxGetRootSite());

        auto handles = CSpxSharedPtrHandleTableManager::Get<ISpxSIModel, SPXSIMODELHANDLE>();
        *psimodel = handles->TrackHandle(model);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI speaker_identification_model_add_profile(SPXSIMODELHANDLE hsimodel, SPXVOICEPROFILEHANDLE hprofile)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto profile = GetInstance<ISpxVoiceProfile>(hprofile);
        auto model = GetInstance<ISpxSIModel>(hsimodel);
        model->AddProfile(profile);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI speaker_identification_model_release_handle(SPXSIMODELHANDLE hmodel)
{
    if (hmodel == SPXHANDLE_INVALID)
    {
        return SPX_NOERROR;
    }
    else if (Handle_IsValid<SPXVOICEPROFILEHANDLE, ISpxSIModel>(hmodel))
    {
        return Handle_Close<SPXVOICEPROFILEHANDLE, ISpxSIModel>(hmodel);
    }
    else
    {
        return SPXERR_INVALID_HANDLE;
    }
}

SPXAPI speaker_verification_model_create(SPXSVMODELHANDLE* psvmodel, SPXVOICEPROFILEHANDLE hprofile)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, psvmodel == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        *psvmodel = SPXHANDLE_INVALID;
        auto model = SpxCreateObjectWithSite<ISpxSVModel>("CSpxSpeakerVerificationModel", SpxGetRootSite());
        auto profile = GetInstance<ISpxVoiceProfile>(hprofile);
        model->InitModel(profile);

        auto handles = CSpxSharedPtrHandleTableManager::Get<ISpxSVModel, SPXSVMODELHANDLE>();
        *psvmodel = handles->TrackHandle(model);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI speaker_verification_model_release_handle(SPXSVMODELHANDLE hsvmodel)
{
    if (hsvmodel == SPXHANDLE_INVALID)
    {
        return SPX_NOERROR;
    }
    else if (Handle_IsValid<SPXSVMODELHANDLE, ISpxSVModel>(hsvmodel))
    {
        return Handle_Close<SPXSVMODELHANDLE, ISpxSVModel>(hsvmodel);
    }
    else
    {
        return SPXERR_INVALID_HANDLE;
    }
}

SPXAPI speaker_recognizer_identify(SPXSPEAKERIDHANDLE phspeakerid, SPXSIMODELHANDLE hsimodel, SPXRESULTHANDLE *phresult)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        *phresult = SPXHANDLE_INVALID;

        auto recognizer = GetInstance<ISpxVoiceProfileClient>(phspeakerid);
        auto model = GetInstance<ISpxSIModel>(hsimodel);
        auto result = recognizer->Identify(model->GetProfiles());

        auto result_handles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognitionResult, SPXRESULTHANDLE>();
        *phresult = result_handles->TrackHandle(result);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI speaker_recognizer_verify(SPXSPEAKERIDHANDLE phspeakerid, SPXSVMODELHANDLE hsvmodel, SPXRESULTHANDLE *phresult)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        *phresult = SPXHANDLE_INVALID;
        auto recognizer = GetInstance<ISpxVoiceProfileClient>(phspeakerid);
        auto model = GetInstance<ISpxSVModel>(hsvmodel);
        auto profile = model->GetProfile();
        if (profile != nullptr)
        {
            auto result = recognizer->ProcessProfileAction(ISpxVoiceProfileClient::Action::Verify, profile->GetType(), profile->GetProfileId());
            auto result_handles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognitionResult, SPXRESULTHANDLE>();
            *phresult = result_handles->TrackHandle(result);
        }
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}
