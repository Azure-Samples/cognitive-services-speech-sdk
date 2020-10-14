//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// http_recog_engine_adapter.cpp: Private implementation declarations for CSpxHttpRecoEngineAdapter
//

#include "stdafx.h"
#include <sstream>
#include <type_traits>
#include <usp.h>
#include "http_recog_engine_adapter.h"
#include "http_recog_engine_response_helpers.h"
#include "property_id_2_name_map.h"
#include "http_client.h"
#include "service_helpers.h"
#include "create_object_helpers.h"
#include "error_info.h"
#include <json.h>
#include <http_request.h>
#include <http_response.h>
#include <http_headers.h>
#include <http_utils.h>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

using namespace std;
using json = nlohmann::json;

#define FROM_SECOND_TO_HNS 10000000 // 7 zeros

namespace  voiceProfileHelper
{
    void GetEnrollmentInfo(const nlohmann::json& json, ISpxNamedProperties * properties);

    template <typename FieldType>
    bool GetValue(const nlohmann::json& json, const char* key, FieldType& value)
    {
        bool hasValue = false;
        if (json.find(key) != json.end())
        {
            auto rawValue = json[key];
            if (!rawValue.is_null())
            {
                value = json[key].get<FieldType>();
                hasValue = true;
            }
        }
        return hasValue;
    }

    std::string CreateKey(const char* prefix, const char* key)
    {
        std::string keyName{ prefix };
        if (!keyName.empty())
        {
            keyName += ".";
        }
        return keyName.append(key);
    }

    template <typename T, typename std::enable_if_t<std::is_same<T, std::string>::value>* = nullptr>
    void FromJsonToPropertyBag(const nlohmann::json& json, const char* key, ISpxNamedProperties* properties, const char* prefix = "")
    {
        std::string value{ "" };
        if (GetValue<std::string>(json, key, value))
        {
            std::string keyName = CreateKey(prefix, key);
            properties->SetStringValue(keyName.c_str(), value.c_str());
        }
    }

    template <typename T, typename std::enable_if_t<std::is_floating_point<T>::value>* = nullptr>
    void FromJsonToPropertyBag(const nlohmann::json& json, const char* key, ISpxNamedProperties* properties, const char* prefix = "")
    {
        float valueInSeconds = 0.0;
        if (GetValue<float>(json, key, valueInSeconds))
        {
            std::string keyName = CreateKey(prefix, key);
            uint64_t valueInHNS = static_cast<uint64_t>(valueInSeconds * FROM_SECOND_TO_HNS);
            properties->SetStringValue(keyName.c_str(), std::to_string(valueInHNS).c_str());
        }
    }

    template <typename Integer, std::enable_if_t<std::is_integral<Integer>::value, int> =0>
    void FromJsonToPropertyBag(const nlohmann::json& json, const char* key, ISpxNamedProperties* properties, const char* prefix = "")
    {
        int value = 0;
        if (GetValue<int>(json, key, value))
        {
            std::string keyName = CreateKey(prefix, key);
            properties->SetStringValue(keyName.c_str(), std::to_string(value).c_str());
        }
    }

    template <typename T, typename Fields>
    void FromJsonToPropertyBag(const nlohmann::json& json, Fields fields, ISpxNamedProperties* properties, const char* prefix = "")
    {
        for (auto& field : fields)
        {
            FromJsonToPropertyBag<T>(json, field, properties, prefix);
        }
    }
}

void voiceProfileHelper::GetEnrollmentInfo(const nlohmann::json& json, ISpxNamedProperties * properties)
{
    if (properties == nullptr)
    {
        return;
    }

    vector<const char*> intFields{ "enrollmentsCount", "remainingEnrollmentsCount"};
    voiceProfileHelper::FromJsonToPropertyBag<int>(json, intFields, properties, "enrollment");

    // todo: "HTTP Get Profile" from service returns "enrollmentSpeechLength" missing s after enrollmentsSpeechLength.
    vector<const char*> floatFields{ "enrollmentsSpeechLength", "enrollmentSpeechLength", "remainingEnrollmentsSpeechLength"};
    voiceProfileHelper::FromJsonToPropertyBag<float>(json, floatFields, properties, "enrollment");
}

void CSpxHttpRecoEngineAdapter::Init()
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    Microsoft::CognitiveServices::Speech::USP::PlatformInit(nullptr, 0, nullptr, nullptr);

    m_speakerIdPaths[VoiceProfileType::TextIndependentIdentification] = "/speaker/identification/v2.0/text-independent/profiles";
    m_speakerIdPaths[VoiceProfileType::TextIndependentVerification] = "/speaker/verification/v2.0/text-independent/profiles";
    m_speakerIdPaths[VoiceProfileType::TextDependentVerification] = "/speaker/verification/v2.0/text-dependent/profiles";

    auto endpointUrl = GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_Endpoint), "");
    auto hostUrl = GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_Host), "");

    auto endpoint = !endpointUrl.empty()
        ? endpointUrl
        : !hostUrl.empty()
        ? hostUrl
        : GetDefaultEndpoint();

    auto url = HttpUtils::ParseUrl(endpoint);
    m_uriScheme = url.scheme;
    m_speakerRecognitionHostName = url.host;
    m_devEndpoint = (m_speakerRecognitionHostName.find("dev.spr-frontend") != string::npos) ? true : false;
    m_audioFlushed = false;
}

void CSpxHttpRecoEngineAdapter::Term()
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    m_httpData = nullptr;
    m_response = nullptr;
}

std::string CSpxHttpRecoEngineAdapter::GetDefaultEndpoint()
{
    return "https://westus.api.cognitive.microsoft.com";
}

const std::string CSpxHttpRecoEngineAdapter::GetSubscriptionKey() const
{
    auto subscriptionKey = GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_Key), "");

    if (subscriptionKey.empty())
    {
        SPX_THROW_HR(SPXERR_RUNTIME_ERROR);
    }
    return subscriptionKey;
}

VoiceProfilePtr CSpxHttpRecoEngineAdapter::CreateVoiceProfile(VoiceProfileType type, string&& locale) const
{
    HttpEndpointInfo end_point = CreateEndpoint(type);
    string voice_profile_id{ "" };

    ostringstream oss;
    oss << R"({"locale": ")" << locale << R"("})";
    auto culture = oss.str();

    auto response = SendRequest(end_point, HTTPAPI_REQUEST_POST, culture.c_str(), culture.length());
    SPX_IFTRUE_RETURN_X(response == nullptr, nullptr);

    auto json = json::parse(response->ReadContentAsString());

    if( json.find("profileId") != json.end())
    {
        voice_profile_id = json["profileId"].get<string>();
    }

    if (!voice_profile_id.empty())
    {
        SPX_TRACE_INFO("Successfully created a profile id as %s", voice_profile_id.c_str());
    }

    auto voiceProfile = SpxCreateObjectWithSite<ISpxVoiceProfile>("CSpxVoiceProfile", SpxGetRootSite());
    SPX_IFTRUE_RETURN_X(voiceProfile == nullptr, nullptr);

    voiceProfile->SetProfileId(move(voice_profile_id));
    voiceProfile->SetType(type);

    return voiceProfile;
}

unique_ptr<HttpResponse> CSpxHttpRecoEngineAdapter::SendRequest(const HttpEndpointInfo& endPoint, HTTPAPI_REQUEST_TYPE requestType, const void *content, size_t contentSize) const
{
    if (!endPoint.IsValid())
    {
        SPX_TRACE_ERROR("Invalid end point for %s.", endPoint.EndpointUrl().c_str());
        SPX_THROW_HR(SPXERR_RUNTIME_ERROR);
    }
    HttpRequest request(endPoint);
    request.SetRequestHeader(m_devEndpoint?USP::headers::apimSubscriptionKey:USP::headers::ocpApimSubscriptionKey, GetSubscriptionKey().c_str());
    request.SetRequestHeader(USP::headers::contentType, "application/json");
    request.SetPath(endPoint.Path());

    SPX_DBG_TRACE_INFO("The url http request is: %s", endPoint.EndpointUrl().c_str());

    std::unique_ptr<HttpResponse> response;
    try
    {
        response = content != nullptr
            ? request.SendRequest(requestType, content, contentSize)
            : request.SendRequest(requestType);
    }
    catch (HttpException& e)
    {
        std::string message = "Error in sending a http request." + std::to_string(e.statusCode());
        SPX_DBG_TRACE_ERROR("%s", message.c_str());
    }
    catch (...)
    {
    }

    if (response == nullptr)
    {
        return response;
    }

    SPX_TRACE_ERROR_IF(!response->IsSuccess(), "SendRequest failed with HTTP %u [%s] url:'%s'",
        response->GetStatusCode(),
        response->ReadContentAsString().c_str(),
        endPoint.EndpointUrl().c_str());

    return response;
}

void CSpxHttpRecoEngineAdapter::SetFormat(const SPXWAVEFORMATEX* pformat, VoiceProfileType type, vector<string>&& profileIds, bool enroll)
{
    UNUSED(pformat);
    string fullPath;
    string profileId;

    SPX_THROW_HR_IF(SPXERR_INVALID_ARG, type == VOICE_PROFILE_TYPE_NONE);

    if (enroll)
    {
        SPX_DBG_ASSERT(profileIds.size() == 1);
        profileId = profileIds[0];
        fullPath = m_speakerIdPaths.at(type) + "/" + profileId + "/enrollments";
    }
    else
    {
        if (type == VoiceProfileType::TextDependentVerification || type == VoiceProfileType::TextIndependentVerification)
        {
            SPX_DBG_ASSERT(profileIds.size() == 1);
            profileId = profileIds[0];
            fullPath = m_speakerIdPaths.at(type) + "/" + profileId + "/verify?ignoreMinLength=True";
        }
        else if (type == VoiceProfileType::TextIndependentIdentification)
        {
            SPX_DBG_ASSERT(profileIds.size() >= 1);
            ostringstream oss;
            for (const auto& id : profileIds)
            {
                oss << id << ",";
            }
            // remove the last , There is at least one id in the profileIds.
            oss.seekp(-1, std::ios_base::end);
            oss << '&';
            fullPath = m_speakerIdPaths.at(type) + "/identifySingleSpeaker?profileIds=" + oss.str();
            fullPath += "ignoreMinLength=True";
        }
    }
    SPX_DBG_ASSERT(!fullPath.empty());

    auto endPoint = CreateEndpoint(move(fullPath));

    if (!endPoint.IsValid())
    {
        SPX_TRACE_ERROR("Invalid end point for %s.", fullPath.c_str());
        SPX_THROW_HR(SPXERR_RUNTIME_ERROR);
    }

    m_httpData = make_unique<CSpxHttpClient>(endPoint, GetSubscriptionKey(), m_devEndpoint ? USP::headers::apimSubscriptionKey : USP::headers::ocpApimSubscriptionKey);
    m_enroll = enroll;
    m_voiceProfileType = type;
    m_profileIdForVerification = profileId;
    m_audioFlushed = false;
}

void CSpxHttpRecoEngineAdapter::ProcessAudio(const DataChunkPtr& audioChunk)
{
    SPX_DBG_ASSERT(m_httpData != nullptr);
    if (m_httpData && !m_audioFlushed)
    {
        m_httpData->ProcessAudio(audioChunk);
    }
}

void CSpxHttpRecoEngineAdapter::FlushAudio()
{
    SPX_DBG_ASSERT(m_httpData != nullptr);
    if (m_httpData)
    {
        m_response = m_httpData->FlushAudio();
    }
    m_audioFlushed = true;
}

RecognitionResultPtr CSpxHttpRecoEngineAdapter::ModifyVoiceProfile(ModifyOperation operation, VoiceProfileType type, std::string&& id)
{
    if (type == VOICE_PROFILE_TYPE_NONE)
    {
        constexpr auto message = "Internal error encountered while deleting or resetting a voice profile. "
            "A voice profile type must be specified.";
        const auto error = ErrorInfo::FromRuntimeMessage(message);
        return CreateErrorResult(error);
    }

    auto fullPath = m_speakerIdPaths.at(type) + "/" + id;
    bool reset = operation == ModifyOperation::Reset;
    if (reset)
    {
        fullPath += "/reset";
    }

    auto endPoint = CreateEndpoint(move(fullPath));

    m_response = SendRequest(endPoint, reset ? HTTPAPI_REQUEST_POST : HTTPAPI_REQUEST_DELETE);
    auto operationType = reset ? SpeakerRecognitionOperationType::ResetProfile : SpeakerRecognitionOperationType::DeleteProfile;
    auto result = GetResult(operationType);

    return result;
}

RecognitionResultPtr CSpxHttpRecoEngineAdapter::GetResult(SpeakerRecognitionOperationType operationType)
{
    if (m_response == nullptr)
    {
        constexpr auto message = "Internal error retrieving a result: no response was present.";
        auto error = ErrorInfo::FromRuntimeMessage(message);
        return CreateErrorResult(error);
    }

    auto errorFromResponse = ErrorFromResponse(m_response);
    if (errorFromResponse != nullptr)
    {
        return CreateErrorResult(errorFromResponse);
    }

    auto resultReason = GetResultReasonForResponse(operationType);
    if (resultReason == ResultReason::Canceled)
    {
        constexpr auto message = "Internal error retrieving a result: unexpected response contents.";
        auto error = ErrorInfo::FromRuntimeMessage(message);
        return CreateErrorResult(error);
    }

    std::string resultText;
    if (operationType == SpeakerRecognitionOperationType::EnrollProfile)
    {
        auto json = json::parse(m_response->ReadContentAsString());
        if (json.find("passPhrase") != json.end())
        {
            resultText = json["passPhrase"].get<string>();
        }
    }

    auto result = CreateFinalResult(resultReason, NO_MATCH_REASON_NONE, PAL::ToWString(resultText).c_str(), 0, 0);
    auto populateError = TryPopulateResultFromResponse(result);

    return populateError != nullptr ? CreateErrorResult(populateError) : result;
}

RecognitionResultPtr CSpxHttpRecoEngineAdapter::GetResult() { return GetResult(GetCurrentOperationType()); }

shared_ptr<ISpxNamedProperties> CSpxHttpRecoEngineAdapter::GetParentProperties() const
{
    return SpxQueryInterface<ISpxNamedProperties>(GetSite());
}

HttpEndpointInfo CSpxHttpRecoEngineAdapter::CreateEndpoint(VoiceProfileType type) const
{
    HttpEndpointInfo endpoint;
    SPX_DBG_ASSERT(!m_speakerRecognitionHostName.empty());
    SPX_DBG_ASSERT(m_uriScheme == UriScheme::HTTP || m_uriScheme == UriScheme::HTTPS);
    return endpoint
        .Scheme(m_uriScheme)
        .Host(m_speakerRecognitionHostName)
        .Path(m_speakerIdPaths.at(type));
}

HttpEndpointInfo CSpxHttpRecoEngineAdapter::CreateEndpoint(string&& fullPath) const
{
    HttpEndpointInfo endpoint;
    SPX_DBG_ASSERT(!m_speakerRecognitionHostName.empty());
    SPX_DBG_ASSERT(m_uriScheme == UriScheme::HTTP || m_uriScheme == UriScheme::HTTPS);
    return endpoint
        .Scheme(m_uriScheme)
        .Host(m_speakerRecognitionHostName)
        .Path(fullPath);
}

shared_ptr<ISpxRecognitionResult> CSpxHttpRecoEngineAdapter::CreateIntermediateResult(const wchar_t*, uint64_t, uint64_t)
{
    SPX_DBG_ASSERT(false);
    return nullptr;
}

std::shared_ptr<ISpxRecognitionResult> CSpxHttpRecoEngineAdapter::CreateFinalResult(ResultReason reason, NoMatchReason noMatchReason, const wchar_t* text, uint64_t offset, uint64_t duration, const wchar_t*)
{
    auto result = SpxCreateObjectWithSite<ISpxRecognitionResult>("CSpxRecognitionResult", this);
    auto initResult = SpxQueryInterface<ISpxRecognitionResultInit>(result);
    initResult->InitFinalResult(reason, noMatchReason, text, offset, duration);

    return result;
}

std::shared_ptr<ISpxRecognitionResult> CSpxHttpRecoEngineAdapter::CreateKeywordResult(const double, const uint64_t, const uint64_t, const wchar_t*, ResultReason, std::shared_ptr<ISpxAudioDataStream>)
{
    SPX_DBG_ASSERT(false);
    return nullptr;
}

RecognitionResultPtr CSpxHttpRecoEngineAdapter::CreateErrorResult(const std::shared_ptr<ISpxErrorInformation>& error)
{
    auto result = SpxCreateObjectWithSite<ISpxRecognitionResult>("CSpxRecognitionResult", this);
    auto initResult = SpxQueryInterface<ISpxRecognitionResultInit>(result);
    initResult->InitErrorResult(error);

    // Compatibility: these results previously stored error information on the "result" key (not the "error" key); copy it
    auto resultProps = SpxQueryInterface<ISpxNamedProperties>(result);
    const auto& details = resultProps->GetStringValue(GetPropertyName(PropertyId::SpeechServiceResponse_JsonErrorDetails), "");
    resultProps->SetStringValue(GetPropertyName(PropertyId::SpeechServiceResponse_JsonResult), details.c_str());

    return result;
}

std::shared_ptr<ISpxRecognitionResult> CSpxHttpRecoEngineAdapter::CreateEndOfStreamResult()
{
    auto result = SpxCreateObjectWithSite<ISpxRecognitionResult>("CSpxRecognitionResult", this);
    auto initResult = SpxQueryInterface<ISpxRecognitionResultInit>(result);
    initResult->InitEndOfStreamResult();

    return result;
}

ResultReason CSpxHttpRecoEngineAdapter::GetResultReasonForResponse(SpeakerRecognitionOperationType operationType)
{
    switch (operationType)
    {
    case SpeakerRecognitionOperationType::ResetProfile:
        return ResultReason::ResetVoiceProfile;
    case SpeakerRecognitionOperationType::DeleteProfile:
        return ResultReason::DeletedVoiceProfile;
    case SpeakerRecognitionOperationType::EnrollProfile:
        return EnrollmentResultReasonFromResponse(m_response);
    case SpeakerRecognitionOperationType::Identify:
        return IdentificationResultReasonFromResponse(m_response);
    case SpeakerRecognitionOperationType::Verify:
        return VerificationResultReasonFromResponse(m_response);
    default:
        return ResultReason::Canceled;
    }
}

std::shared_ptr<ISpxErrorInformation> CSpxHttpRecoEngineAdapter::TryPopulateResultFromResponse(RecognitionResultPtr& result)
{
    const auto responseText = m_response->ReadContentAsString();
    if (responseText.empty()) return nullptr;

    auto json = json::parse(responseText);
    auto props = SpxQueryInterface<ISpxNamedProperties>(result);

    props->SetStringValue(GetPropertyName(PropertyId::SpeechServiceResponse_JsonResult), responseText.c_str());

    constexpr auto errorMessageBase{ "Exception encountered while processing " };

    switch (GetCurrentOperationType())
    {
    case SpeakerRecognitionOperationType::EnrollProfile:
        try
        {
            SetJsonProp<int>(json, "enrollmentsCount", props, "enrollment.enrollmentsCount");
            SetJsonHNSProp(json, "enrollmentsLength", props, "enrollment.enrollmentsLength");
            SetJsonHNSProp(json, "enrollmentsSpeechLength", props, "enrollment.enrollmentsSpeechLength");
            SetJsonProp_NoThrow<int>(json, "remainingEnrollmentsCount", props, "enrollment.remainingEnrollmentsCount");
            SetJsonHNSProp_NoThrow(json, "remainingEnrollmentsSpeechLength", props, "enrollment.remainingEnrollmentsSpeechLength");
            SetJsonHNSProp(json, "audioLength", props, "enrollment.audioLength");
            SetJsonHNSProp(json, "audioSpeechLength", props, "enrollment.audioSpeechLength");
            SetJsonProp(json, "profileId", props, "enrollment.profileId");
        }
        catch (const exception& e)
        {
            std::stringstream errorMessageStream;
            errorMessageStream << errorMessageBase << "enrollment result: " << e.what();
            auto errorMessage = errorMessageStream.str();
            SPX_DBG_TRACE_ERROR("%s", errorMessage.c_str());
            auto error = ErrorInfo::FromRuntimeMessage(errorMessage);
            return error;
        }
        break;
    case SpeakerRecognitionOperationType::Identify:
        try
        {
            auto profileId = json["identifiedProfile"]["profileId"].get<string>();
            auto score = json["identifiedProfile"]["score"].get<float>();
            props->SetStringValue("speakerrecognition.score", to_string(score).c_str());
            props->SetStringValue("speakerrecognition.profileid", profileId.c_str());
        }
        catch (const exception& e)
        {
            std::stringstream errorMessageStream;
            errorMessageStream << errorMessageBase << " identification result: " << e.what();
            auto errorMessage = errorMessageStream.str();
            SPX_DBG_TRACE_ERROR("%s", errorMessage.c_str());
            return ErrorInfo::FromRuntimeMessage(errorMessage);
        }
        break;
    case SpeakerRecognitionOperationType::Verify:
        try
        {
            SetJsonProp<float>(json, "score", props, "speakerrecognition.score");
            props->SetStringValue("speakerrecognition.profileid", m_profileIdForVerification.c_str());
        }
        catch (const exception& e)
        {
            std::stringstream messageStream;
            messageStream << "Exception encountered parsing a verification result: " << e.what();
            auto message = messageStream.str();
            SPX_DBG_TRACE_ERROR("%s", message.c_str());
            auto error = ErrorInfo::FromRuntimeMessage(message);
            return error;
        }
        break;
    default:
        break;
    }

    // Population successful; no error
    return nullptr;
}

vector<VoiceProfilePtr> CSpxHttpRecoEngineAdapter::GetVoiceProfiles(VoiceProfileType type) const
{
    // todo: how to conjure a vector VoiceProfilePtr for error case??
    vector<VoiceProfilePtr> result;
    SPX_IFTRUE_RETURN_X(type == VOICE_PROFILE_TYPE_NONE, result);

    return vector<VoiceProfilePtr>{};
}

VoiceProfilePtr CSpxHttpRecoEngineAdapter::GetVoiceProfileStatus(VoiceProfileType type, string&& voiceProfileId) const
{
    // todo: how to conjure a VoiceProfilePtr for error case?
    SPX_IFTRUE_RETURN_X(type == VOICE_PROFILE_TYPE_NONE, nullptr);
    SPX_IFTRUE_RETURN_X(voiceProfileId.empty(), nullptr);

    auto fullPath = m_speakerIdPaths.at(type) + "/" + voiceProfileId;
    auto endPoint = CreateEndpoint(move(fullPath));
    auto response = SendRequest(endPoint, HTTPAPI_REQUEST_GET);
    SPX_IFTRUE_RETURN_X(response == nullptr, nullptr);
    SPX_TRACE_VERBOSE("Successfully get voice profile %s", voiceProfileId.c_str());

    auto voiceProfile = SpxCreateObjectWithSite<ISpxVoiceProfile>("CSpxVoiceProfile", SpxGetRootSite());
    SPX_IFTRUE_RETURN_X(voiceProfile == nullptr, nullptr);

    voiceProfile->SetProfileId(move(voiceProfileId));
    voiceProfile->SetType(type);

    auto json = json::parse(response->ReadContentAsString());
    auto properties = SpxQueryInterface<ISpxNamedProperties>(voiceProfile);
    voiceProfileHelper::GetEnrollmentInfo(json, properties.get());

    vector<const char*> fields{ "locale", "enrollmentStatus", "createdDateTime", "lastUpdatedDateTime", "modelVersion" };
    voiceProfileHelper::FromJsonToPropertyBag<std::string>(json, fields, properties.get());

    return voiceProfile;
}

}}}}
