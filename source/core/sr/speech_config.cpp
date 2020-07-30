//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "stdafx.h"
#include "speech_config.h"
#include "property_id_2_name_map.h"
#include "usp.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

using namespace std;

void CSpxSpeechConfig::InitAuthorizationToken(const char* authToken, const char* region)
{
    SPX_IFTRUE_THROW_HR(m_init, SPXERR_ALREADY_INITIALIZED);
    m_init = true;

    CheckRegionString(region);
    SetStringValue(GetPropertyName(PropertyId::SpeechServiceAuthorization_Token), authToken);
    SetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_Region), region);
}

void CSpxSpeechConfig::InitFromEndpoint(const char* endpoint, const char* subscription)
{
    SPX_IFTRUE_THROW_HR(m_init, SPXERR_ALREADY_INITIALIZED);
    m_init = true;

    SetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_Endpoint), endpoint);
    if (subscription != nullptr)
    {
        SetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_Key), subscription);
    }
}

void CSpxSpeechConfig::InitFromHost(const char* host, const char* subscription)
{
    SPX_IFTRUE_THROW_HR(m_init, SPXERR_ALREADY_INITIALIZED);
    m_init = true;

    SetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_Host), host);

    if (subscription != nullptr)
    {
        SetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_Key), subscription);
    }
}

void CSpxSpeechConfig::InitFromSubscription(const char* subscription, const char* region)
{
    SPX_IFTRUE_THROW_HR(m_init, SPXERR_ALREADY_INITIALIZED);
    m_init = true;

    CheckRegionString(region);
    SetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_Key), subscription);
    SetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_Region), region);
}

void CSpxSpeechConfig::SetServiceProperty(const string& name, const string& value, ServicePropertyChannel channel)
{
    // parameters have been validated at C-API.
    if (channel == ServicePropertyChannel::UriQueryParameter)
    {
        auto propertyNameQueryParameters = GetPropertyName(PropertyId::SpeechServiceConnection_UserDefinedQueryParameters);
        auto currentQueryParameters = GetStringValue(propertyNameQueryParameters, "");
        if (currentQueryParameters.empty())
        {
            currentQueryParameters = name + "=" + value;
        }
        else
        {
            currentQueryParameters += "&" + name + "=" + value;
        }
        SetStringValue(propertyNameQueryParameters, currentQueryParameters.c_str());
    }
    else if (channel == ServicePropertyChannel::HttpHeader)
    {
        auto propertyName = string{ "HttpHeader" } + g_propertyNameSeperator + name;
        SetStringValue(propertyName.c_str(), value.c_str());
    }
    else
    {
        SPX_TRACE_ERROR("Unsupported channel: %d. Only UriQueryParameter is supported.", (int)channel);
        SPX_THROW_HR(SPXERR_INVALID_ARG);
    }
}

void CSpxSpeechConfig::SetProfanity(ProfanityOption profanity)
{
    string valueStr;
    switch (profanity)
    {
    case ProfanityOption::Masked:
        valueStr = USP::endpoint::profanityMasked;
        break;
    case ProfanityOption::Removed:
        valueStr = USP::endpoint::profanityRemoved;
        break;
    case ProfanityOption::Raw:
        valueStr = USP::endpoint::profanityRaw;
        break;
    default:
        SPX_TRACE_ERROR("Unsupported profanity: %d.", (int)profanity);
        SPX_THROW_HR(SPXERR_INVALID_ARG);
        break;
    }
    SetStringValue(GetPropertyName(PropertyId::SpeechServiceResponse_ProfanityOption), valueStr.c_str());
}

void CSpxSpeechConfig::CheckRegionString(const char *region)
{
    string regionStr(region);
    const auto forbiddenInRegion = {":", "//"};
    for(auto pattern: forbiddenInRegion)
    {
        if (regionStr.find(pattern) != string::npos)
        {
            SPX_TRACE_ERROR("Invalid region: %s.", region);
            SPX_THROW_HR(SPXERR_INVALID_ARG);
        }
    }
}

} } } } // Microsoft::CognitiveServices::Speech::Impl
