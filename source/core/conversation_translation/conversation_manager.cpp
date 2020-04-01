//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// conversation_manager.cpp: Implementation for CSpxConversationManager C++ class
//

#include <ctime>
#include <azure_c_shared_utility_urlencode_wrapper.h>
#include <azure_c_shared_utility_xlogging_wrapper.h>
#include <json.h>
#include <http_request.h>
#include <http_response.h>
#include <http_headers.h>
#include "common.h"
#include "conversation_manager.h"
#include "conversation_utils.h"
#include "conversation_translator_logging.h"

#define CT_DBG_TRACE_CONVERSATION_ROOM_MANAGER 0

#define ADD_API_VERSION(request) request.AddQueryParameter(ConversationQueryParameters::ApiVersion, ConversationConstants::ApiVersion)

using json = nlohmann::json;

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {
namespace ConversationTranslation {

static std::string GetUtcTimeString()
{
    struct tm utcTime;
    char buffer[30];
    std::time_t time = std::time(nullptr);

#ifdef _MSC_VER
    // convert to UTC the "safe" Windows way
    auto error = gmtime_s(&utcTime, &time);
    if (error)
    {
        return std::string();
    }
#else
    // convert to UTC the "safe" POSIX way
    if (gmtime_r(&time, &utcTime) == nullptr)
    {
        return std::string();
    }
#endif

    size_t written = std::strftime(buffer, sizeof(buffer) / sizeof(buffer[0]), "%F %TZ", &utcTime);
    if (written == 0)
    {
        return std::string();
    }

    return std::string(buffer, written);
}

ConversationManager::ConversationManager(const HttpEndpointInfo& endpointInfo) :
    m_endpointInfo(endpointInfo)
{
    CT_LOG_VERBOSE_IF(CT_DBG_TRACE_CONVERSATION_ROOM_MANAGER, __FUNCTION__);

    if (!m_endpointInfo.IsValid())
    {
        throw std::invalid_argument("The endpoint info is not valid for the conversation manager");
    }
}

ConversationManager::~ConversationManager()
{
    CT_LOG_VERBOSE_IF(CT_DBG_TRACE_CONVERSATION_ROOM_MANAGER, __FUNCTION__);
}

ConversationArgs ConversationManager::CreateOrJoin(const CreateConversationArgs& args, const std::string& roomCode)
{
    CT_LOG_VERBOSE_IF(CT_DBG_TRACE_CONVERSATION_ROOM_MANAGER, __FUNCTION__);

    HttpRequest request(m_endpointInfo);

    ADD_API_VERSION(request);
    request.AddQueryParameter("language", args.LanguageCode);
    
    if (!args.Nickname.empty())
    {
        request.AddQueryParameter("nickname", args.Nickname);
    }

    for (auto const& to : args.TranslateTo)
    {
        request.AddQueryParameter("translateTo", to);
    }

    if (!roomCode.empty())
    {
        request.AddQueryParameter("roomId", roomCode);
    }

    if (!args.HostCode.empty())
    {
        request.AddQueryParameter("hostCode", args.HostCode);
    }

    if (!args.ParticipantCode.empty())
    {
        request.AddQueryParameter("participantCode", args.ParticipantCode);
    }

    if (!args.TtsVoiceCode.empty())
    {
        request.AddQueryParameter("voice", args.TtsVoiceCode);

        const char* formatString;
        switch (args.TtsFormat)
        {
            default:
            case TextToSpeechFormat::Wav:
                formatString = "audio/wav";
                break;

            case TextToSpeechFormat::Siren:
                formatString = "audio/wav-siren";
                break;
        }

        request.AddQueryParameter("format", formatString);
    }

    if (args.CognitiveSpeechRegion.empty())
    {
        if (roomCode.empty())
        {
            ThrowInvalidArgumentException("You must specify the cognitive speech region to use");
        }
    }
    else
    {
        request.SetRequestHeader(USP::headers::region, args.CognitiveSpeechRegion);
    }

    if (!args.CognitiveSpeechSubscriptionKey.empty())
    {
        request.SetRequestHeader(USP::headers::ocpApimSubscriptionKey, args.CognitiveSpeechSubscriptionKey);
    }
    else if (!args.CognitiveSpeechAuthenticationToken.empty())
    {
        request.SetRequestHeader(USP::headers::authorization, "Bearer " + args.CognitiveSpeechAuthenticationToken);
    }
    else if (roomCode.empty())
    {
        ThrowInvalidArgumentException("You must specify either an authentication token to use, or a Cognitive Speech subscription key");
    }

    if (!args.ClientAppId.empty())
    {
        request.SetRequestHeader(ConversationConstants::ClientAppIdHeader, args.ClientAppId);
    }

    CT_LOG_INFO("Sending create/join room request to '%s'", m_endpointInfo.EndpointUrl().c_str());
    auto response = request.SendRequest(HTTPAPI_REQUEST_POST);

    if (!response->IsSuccess())
    {
        CT_LOG_ERROR(
            "Creating/Joining room failed with HTTP %u [%s] at '%s'",
            response->GetStatusCode(),
            response->GetHeader(ConversationConstants::RequestIdHeader).c_str(),
            GetUtcTimeString().c_str());
    }

    // throws an HttpException on non success status codes
    response->EnsureSuccess();

    auto json = json::parse(response->ReadContentAsString());

    ConversationArgs roomArgs;

    try
    {
        roomArgs.Avatar = json["avatar"].get<std::string>();
        roomArgs.CognitiveSpeechAuthToken = json["cognitiveSpeechAuthToken"].get<std::string>();
        roomArgs.CognitiveSpeechRegion = json["cognitiveSpeechRegion"].get<std::string>();
        roomArgs.CorrelationId = json["correlationId"].get<std::string>();
        roomArgs.IsApproved = json["isApproved"];
        roomArgs.IsMuted = json["isMuted"];
        roomArgs.RoomCode = json["roomId"].get<std::string>();
        roomArgs.SessionToken = json["token"].get<std::string>();
        roomArgs.ParticipantId = json["participantId"].get<std::string>();

        roomArgs.Nickname = args.Nickname;
        roomArgs.RequestId = response->GetHeader(ConversationConstants::RequestIdHeader);
    }
    catch (exception& ex)
    {
        std::string error("Server response to create/join conversation is malformed. RequestId: ");
        error += response->GetHeader(ConversationConstants::RequestIdHeader);
        error += ". Details: ";
        error += ex.what();

        CT_LOG_ERROR(error.c_str());
        ThrowRuntimeError(error);
    }

    CT_LOG_INFO(
        "Successfully created conversation '%s' [%s] at '%s'",
        roomArgs.RoomCode.c_str(),
        roomArgs.RequestId.c_str(),
        GetUtcTimeString().c_str());

    return roomArgs;
}

void ConversationManager::Leave(const std::string& sessionToken)
{
    CT_LOG_VERBOSE_IF(CT_DBG_TRACE_CONVERSATION_ROOM_MANAGER, __FUNCTION__);

    HttpRequest request(m_endpointInfo);

    ADD_API_VERSION(request);
    request.AddQueryParameter(ConversationQueryParameters::AuthToken, sessionToken);

    auto response = request.SendRequest(HTTPAPI_REQUEST_DELETE);
    response->EnsureSuccess();
}

}}}}} // Microsoft::CognitiveServices::Speech::Impl::Conversation
