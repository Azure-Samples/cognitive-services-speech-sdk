//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "stdafx.h"
#include "speech_config.h"
#include "property_id_2_name_map.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


void CSpxSpeechConfig::InitAuthorizationToken(const char * authToken, const char * region)
{
    SPX_IFTRUE_THROW_HR(m_init, SPXERR_ALREADY_INITIALIZED);
    m_init = true;

    SetStringValue(GetPropertyName(SpeechPropertyId::SpeechServiceAuthorization_Token), authToken);
    SetStringValue(GetPropertyName(SpeechPropertyId::SpeechServiceConnection_Region), region);
}

void CSpxSpeechConfig::InitFromEndpoint(const char * endpoint, const char* subscription)
{
    SPX_IFTRUE_THROW_HR(m_init, SPXERR_ALREADY_INITIALIZED);
    m_init = true;

    SetStringValue(GetPropertyName(SpeechPropertyId::SpeechServiceConnection_Endpoint), endpoint);
    SetStringValue(GetPropertyName(SpeechPropertyId::SpeechServiceConnection_Key), subscription);
}


void CSpxSpeechConfig::InitFromSubscription(const char * subscription, const char* region)
{
    SPX_IFTRUE_THROW_HR(m_init, SPXERR_ALREADY_INITIALIZED);
    m_init = true;

    SetStringValue(GetPropertyName(SpeechPropertyId::SpeechServiceConnection_Key), subscription);
    SetStringValue(GetPropertyName(SpeechPropertyId::SpeechServiceConnection_Region), region);
}

} } } } // Microsoft::CognitiveServices::Speech::Impl
