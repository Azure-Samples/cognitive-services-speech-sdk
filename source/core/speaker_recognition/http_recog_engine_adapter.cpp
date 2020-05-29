//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// http_recog_engine_adapter.cpp: Private implementation declarations for CSpxHttpRecoEngineAdapter
//

#include "stdafx.h"
#include <sstream>
#include <usp.h>
#include "http_recog_engine_adapter.h"
#include "property_id_2_name_map.h"
#include "http_client.h"
#include "service_helpers.h"

#include <json.h>
#include <http_request.h>
#include <http_response.h>
#include <http_headers.h>
#include <http_utils.h>

using json = nlohmann::json;
using namespace std;

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

#define FROM_SECOND_TO_HNS 10000000 // 7 zeros

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

string CSpxHttpRecoEngineAdapter::CreateVoiceProfile(VoiceProfileType type, string&& locale) const
{
    HttpEndpointInfo end_point = CreateEndpoint(type);
    string voice_profile_id{ "" };

    ostringstream oss;
    oss << R"({"locale": ")" << locale << R"("})";
    auto culture = oss.str();

    auto response = SendRequest(end_point, HTTPAPI_REQUEST_POST, culture.c_str(), culture.length());
    SPX_IFTRUE_RETURN_X(response == nullptr, voice_profile_id);

    auto json = json::parse(response->ReadContentAsString());

    if( json.find("profileId") != json.end())
    {
        voice_profile_id = json["profileId"].get<string>();
    }

    if (!voice_profile_id.empty())
    {
        SPX_TRACE_INFO("Successfully created a profile id as %s", voice_profile_id.c_str());
    }
    return voice_profile_id;
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

    SPX_TRACE_ERROR_IF(!response->IsSuccess(), "ModifyVoiceProfile failed with HTTP %u [%s] url:'%s'",
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

RecognitionResultPtr CSpxHttpRecoEngineAdapter::ModifyVoiceProfile(bool reset, VoiceProfileType type, std::string&& id) const
{
    SPX_IFTRUE_RETURN_X(type == VOICE_PROFILE_TYPE_NONE, CreateErrorResult("error in delete/reset a voice profile. Must set a voicetype."));

    auto fullPath = m_speakerIdPaths.at(type) + "/" + id;
    if (reset)
    {
        fullPath += "/reset";
    }

    auto endPoint = CreateEndpoint(move(fullPath));

    auto response = SendRequest(endPoint, reset ? HTTPAPI_REQUEST_POST : HTTPAPI_REQUEST_DELETE);
    if (response == nullptr)
    {
        return CreateErrorResult("Error in delete/reset a voice profile");
    }
    SPX_DBG_TRACE_INFO("Successfully %s voice profile %s", reset?"reset":"delete", id.c_str());
    auto factory = make_unique<DeleteOrResetResultFactory>(move(response), reset);

    return factory->CreateResult([=](ResultReason reason, CancellationReason canReason, NoMatchReason noMatchReason, CancellationErrorCode errorCode, const wchar_t* text) {
        return CreateRecoResult(reason, canReason, noMatchReason, errorCode, text);
        });
}

RecognitionResultPtr DeleteOrResetResultFactory::CreateResult(CreateFinalResultFuncPtr func)
{
    if (m_response == nullptr || func == nullptr)
    {
        return CreateErrorResult(func, "Error in Delete/Reset a voice profile");
    }

    ResultReason reason{ ResultReason::Canceled };
    if (m_response->IsSuccess())
    {
        reason = m_reset ? ResultReason::ResetVoiceProfile : ResultReason::DeletedVoiceProfile;
    }

    auto cancellationReason = GetCancellationReason(m_response->IsSuccess());
    auto noMatchReason = GetNoMatchReason();
    auto errorCode = GetCancellationErrorCode(m_response->GetStatusCode());
    string displayText;
    if (!m_response->IsSuccess())
    {
        displayText = GetErrorMessageInJson(m_response->ReadContentAsString(), m_response->GetStatusCode());
    }

    auto result = func(reason, cancellationReason, noMatchReason, errorCode, PAL::ToWString(displayText).c_str());
    PopulateJsonResult(result, m_response->ReadContentAsString());
    return result;
}

CancellationReason ISpxSpeakerResultFactory::GetCancellationReason(bool isSuccess)
{
    return isSuccess ? REASON_CANCELED_NONE : CancellationReason::Error;
}

NoMatchReason ISpxSpeakerResultFactory::GetNoMatchReason()
{
    return NO_MATCH_REASON_NONE;
}

CancellationErrorCode ISpxSpeakerResultFactory::GetCancellationErrorCode(int httpStatusCode)
{
    return HttpStatusCodeToCancellationErrorCode(httpStatusCode);
}

RecognitionResultPtr ISpxSpeakerResultFactory::CreateErrorResult(CreateFinalResultFuncPtr func, const std::string& error)
{
    return func(ResultReason::Canceled, CancellationReason::Error, NO_MATCH_REASON_NONE, CancellationErrorCode::RuntimeError, PAL::ToWString(error).c_str());
}

void ISpxSpeakerResultFactory::PopulateJsonResult(RecognitionResultPtr& result, const std::string& jsonString)
{
    auto namedProperties = SpxQueryInterface<ISpxNamedProperties>(result);
    if (jsonString.empty())
    {
        return;
    }
    // save the origin response as JsonResult.
    namedProperties->SetStringValue(GetPropertyName(PropertyId::SpeechServiceResponse_JsonResult), jsonString.c_str());
}

RecognitionResultPtr IdentifyResultFactory::CreateResult(CreateFinalResultFuncPtr func)
{
    if (m_response == nullptr || func == nullptr)
    {
        return CreateErrorResult(func, "Error in identifying a voice profile.");
    }

    ResultReason reason{ ResultReason::Canceled };
    json json;
    float score= 0.0f;
    string profileId;
    if (m_response->IsSuccess())
    {
        json = json::parse(m_response->ReadContentAsString());
        profileId = json["identifiedProfile"]["profileId"].get<string>();
        score = json["identifiedProfile"]["score"].get<float>();
        if (!profileId.empty())
        {
            reason = ResultReason::RecognizedSpeakers;
        }
    }

    auto errorCode = GetCancellationErrorCode(m_response->GetStatusCode());
    string errorMessage;
    if (!m_response->IsSuccess())
    {
        errorMessage = GetErrorMessageInJson(m_response->ReadContentAsString(), m_response->GetStatusCode());
    }

    auto result = func(reason,
                       GetCancellationReason(m_response->IsSuccess()),
                       NO_MATCH_REASON_NONE,
                       errorCode,
                       PAL::ToWString(errorMessage).c_str());
    PopulateJsonResult(result, m_response->ReadContentAsString());
    if (!m_response->IsSuccess())
    {
        return result;
    }

    auto namedProperties = SpxQueryInterface<ISpxNamedProperties>(result);
    if (namedProperties == nullptr)
    {
        SPX_TRACE_ERROR("verification result does not surpport ISpxNamedProperties.");
        SPX_THROW_HR(SPXERR_RUNTIME_ERROR);
    }
    namedProperties->SetStringValue("speakerrecognition.score", to_string(score).c_str());
    namedProperties->SetStringValue("speakerrecognition.profileid", profileId.c_str());

    return result;
}

ResultReason VerifyResultFactory::GetResultReason()
{
    ResultReason resultReason = ResultReason::Canceled;
    if (m_response == nullptr)
    {
        return resultReason;
    }

    if (m_response->IsSuccess())
    {
        auto json = json::parse(m_response->ReadContentAsString());
        auto recoResult = json["recognitionResult"].get<string>();
        if (PAL::StringUtils::ToLower(recoResult) == "accept")
        {
            resultReason = ResultReason::RecognizedSpeaker;
        }

        if (PAL::StringUtils::ToLower(recoResult) == "reject")
        {
            resultReason = ResultReason::NoMatch;
        }
    }
    return resultReason;
}

string VerifyResultFactory::GetErrorMesssage(ResultReason resultReason)
{
    if (m_response == nullptr)
    {
        return string{};
    }
    bool rejected = resultReason == ResultReason::NoMatch ? true: false;
    string errorMessage;
    if (!m_response->IsSuccess() && !rejected)
    {
        errorMessage = GetErrorMessageInJson(m_response->ReadContentAsString(), m_response->GetStatusCode());
    }

    return errorMessage;
}

RecognitionResultPtr VerifyResultFactory::CreateResult(CreateFinalResultFuncPtr func)
{
    if (m_response == nullptr || func == nullptr)
    {
        return CreateErrorResult(func, "Error in verifying a voice profile.");
    }

    auto resultReason = GetResultReason();
    auto errorMessage = GetErrorMesssage(resultReason);
    auto result = func(resultReason,
                       GetCancellationReason(m_response->IsSuccess()),
                       GetNoMatchReason(),
                       GetCancellationErrorCode(m_response->GetStatusCode()),
                       PAL::ToWString(errorMessage).c_str());
    PopulateJsonResult(result, m_response->ReadContentAsString());
    if (!m_response->IsSuccess())
    {
        return result;
    }

    auto namedProperties = SpxQueryInterface<ISpxNamedProperties>(result);
    try
    {
        auto json = json::parse(m_response->ReadContentAsString());
        auto score = json["score"].get<float>();
        namedProperties->SetStringValue("speakerrecognition.score", to_string(score).c_str());
        namedProperties->SetStringValue("speakerrecognition.profileid", m_profileId.c_str());
    }
    catch (const exception& e)
    {
        string error{ "exception in parsing verification result." };
        error += e.what();
        SPX_DBG_TRACE_ERROR("%s", error.c_str());
        return CreateErrorResult(func, error);
    }
    return result;
}

RecognitionResultPtr EnrollmentResultFactory::PopulateEnrollmentDurations(CreateFinalResultFuncPtr func, RecognitionResultPtr result, const json& json)
{
    try
    {
        auto namedProperties = SpxQueryInterface<ISpxNamedProperties>(result);

        auto enrollmentsCount = json["enrollmentsCount"].get<int>();
        namedProperties->SetStringValue("enrollment.enrollmentsCount", to_string(enrollmentsCount).c_str());

        auto enrollmentsLength = json["enrollmentsLength"].get<float>();
        SetInPropertyBag(namedProperties.get(), "enrollment.enrollmentsLength", enrollmentsLength);

        auto enrollmentsSpeechLength = json["enrollmentsSpeechLength"].get<float>();
        SetInPropertyBag(namedProperties.get(), "enrollment.enrollmentsSpeechLength", enrollmentsSpeechLength);

        if (json.find("remainingEnrollmentsCount") != json.end())
        {
            auto remainingEnrollmentsCount = json["remainingEnrollmentsCount"].get<int>();
            namedProperties->SetStringValue("enrollment.remainingEnrollmentsCount", to_string(remainingEnrollmentsCount).c_str());
        }

        if (json.find("remainingEnrollmentsSpeechLength") != json.end())
        {
            auto remainingEnrollmentsSpeechLength = json["remainingEnrollmentsSpeechLength"].get<float>();
            SetInPropertyBag(namedProperties.get(), "enrollment.remainingEnrollmentsSpeechLength", remainingEnrollmentsSpeechLength);
        }

        auto audioLength = json["audioLength"].get<float>();
        SetInPropertyBag(namedProperties.get(), "enrollment.audioLength", audioLength);

        auto audioSpeechLength = json["audioSpeechLength"].get<float>();
        SetInPropertyBag(namedProperties.get(), "enrollment.audioSpeechLength", audioSpeechLength);

        auto profileId = json["profileId"].get<string>();
        namedProperties->SetStringValue("enrollment.profileId", profileId.c_str());
    }
    catch (const exception& e)
    {
        string error{ "exception in parsing enrollment result." };
        error += e.what();
        SPX_DBG_TRACE_ERROR("%s", error.c_str());
        return CreateErrorResult(func, error);
    }
    return result;
}

RecognitionResultPtr EnrollmentResultFactory::CreateResult(CreateFinalResultFuncPtr func)
{
    if (m_response == nullptr)
    {
        return CreateErrorResult(func, "HTTP response is null in enrolling a voice profile. This might due to data receive timeout.");
    }

    ResultReason reason{ ResultReason::Canceled };
    auto json = json::parse(m_response->ReadContentAsString());

    string enrollmentStatus{};

    if (!m_response->IsSuccess())
    {
        reason = ResultReason::Canceled;
    }
    else
    {
        enrollmentStatus = json["enrollmentStatus"].get<string>();
        if (PAL::StringUtils::ToLower(enrollmentStatus) == "enrolling" || PAL::StringUtils::ToLower(enrollmentStatus) == "training")
        {
            reason = ResultReason::EnrollingVoiceProfile;
        }
        else if (PAL::StringUtils::ToLower(enrollmentStatus) == "enrolled")
        {
            reason = ResultReason::EnrolledVoiceProfile;
        }
        else
        {
            string error{ "Unexpected enrollment status %s when converting to ResultReason. enrollmentStatus = " };
            error += enrollmentStatus;
            SPX_TRACE_ERROR("%s", error.c_str());
            return CreateErrorResult(func, error);
        }
    }

    auto cancellationReason = GetCancellationReason(m_response->IsSuccess());
    auto noMatchReason = GetNoMatchReason();
    auto errorCode = GetCancellationErrorCode(m_response->GetStatusCode());

    auto errorMessage = GetErrorMessageInJson(m_response->ReadContentAsString(), m_response->GetStatusCode());
    string passPhrase;
    if (json.find("passPhrase") != json.end())
    {
        passPhrase = json["passPhrase"].get<string>();
    }

    auto result = func(reason, cancellationReason, noMatchReason, errorCode, m_response->IsSuccess()?PAL::ToWString(passPhrase).c_str():PAL::ToWString(errorMessage).c_str());

    PopulateJsonResult(result, m_response->ReadContentAsString());
    if (!m_response->IsSuccess())
    {
        return result;
    }

    return PopulateEnrollmentDurations(func, result, json);
}

void ISpxSpeakerResultFactory::SetInPropertyBag(ISpxNamedProperties* properties, string&& name, float valueInSeconds)
{
    if (properties)
    {
        uint64_t enrollmentsLengthInHNS = static_cast<uint64_t>(valueInSeconds * FROM_SECOND_TO_HNS);
        properties->SetStringValue(name.c_str(), to_string(enrollmentsLengthInHNS).c_str());
    }
}

std::string ISpxSpeakerResultFactory::GetErrorMessageInJson(const std::string& str, int statusCode)
{
    std::string error;
    if (str.empty())
    {
        error = "Error of " + std::to_string(statusCode);
    }
    else
    {
        auto json = nlohmann::json::parse(str);
        if (json.find("error") != json.end())
        {
            error = json["error"]["message"].get<std::string>();
        }
    }
    return error;
}

RecognitionResultPtr  CSpxHttpRecoEngineAdapter::GetResult()
{
    unique_ptr<ISpxSpeakerResultFactory> factory;

    if (m_enroll)
    {
        factory = make_unique<EnrollmentResultFactory>(move(m_response));
    }
    else
    {
        if (m_voiceProfileType == VoiceProfileType::TextIndependentIdentification)
        {
            factory = make_unique<IdentifyResultFactory>(move(m_response));
        }
        else
        {
            factory = make_unique<VerifyResultFactory>(move(m_response), move(m_profileIdForVerification));
        }
    }

    SPX_DBG_ASSERT(factory != nullptr);

    return factory->CreateResult([=](ResultReason reason, CancellationReason canReason, NoMatchReason noMatchReason, CancellationErrorCode errorCode, const wchar_t* text) {
            return CreateRecoResult(reason, canReason, noMatchReason, errorCode, text);
     });
}

RecognitionResultPtr CSpxHttpRecoEngineAdapter::CreateRecoResult(ResultReason reason, CancellationReason cancellationReason, NoMatchReason noMatchReason, CancellationErrorCode errorCode, const wchar_t* text) const
{
    auto site = GetSite();
    auto factory = SpxQueryService<ISpxRecoResultFactory>(site);
    return  factory->CreateFinalResult(nullptr, reason, noMatchReason, cancellationReason, errorCode, text, 0, 0);
}

RecognitionResultPtr CSpxHttpRecoEngineAdapter::CreateErrorResult(const std::string& error) const
{
    return CreateRecoResult(ResultReason::Canceled, CancellationReason::Error, NO_MATCH_REASON_NONE, CancellationErrorCode::RuntimeError, PAL::ToWString(error).c_str());
}
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

}}}}
