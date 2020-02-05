//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// conversation_manager.cpp: Implementation for CSpxConversationManager C++ class
//

#include <ctime>
#include <azure_c_shared_utility_urlencode_wrapper.h>
#include <json.h>
#include <http_request.h>
#include <http_response.h>
#include "stdafx.h"
#include "conversation_manager.h"
#include "conversation_utils.h"

#define SPX_DBG_TRACE_CONVERSATION_ROOM_MANAGER 0

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

CSpxConversationManager::CSpxConversationManager(const std::string& host) :
    m_host(host),
    m_port(443),
    m_secure(true),
    m_path(ConversationConstants::RestPath),
    m_autoProxy(true),
    m_proxyHost(),
    m_proxyPort(),
    m_proxyUsername(),
    m_proxyPassword()
{
    SPX_DBG_TRACE_VERBOSE_IF(SPX_DBG_TRACE_CONVERSATION_ROOM_MANAGER, __FUNCTION__);
    Init();
}

CSpxConversationManager::~CSpxConversationManager()
{
    SPX_DBG_TRACE_VERBOSE_IF(SPX_DBG_TRACE_CONVERSATION_ROOM_MANAGER, __FUNCTION__);
    Term();
}

void CSpxConversationManager::Init()
{
    SPX_DBG_TRACE_VERBOSE_IF(SPX_DBG_TRACE_CONVERSATION_ROOM_MANAGER, __FUNCTION__);
}

void CSpxConversationManager::Term()
{
    SPX_DBG_TRACE_VERBOSE_IF(SPX_DBG_TRACE_CONVERSATION_ROOM_MANAGER, __FUNCTION__);
}

void CSpxConversationManager::SetPort(int port)
{
    if (port <= 0)
    {
        Impl::ThrowInvalidArgumentException("Port cannot be 0 or negative");
    }
    else if (port > 65535)
    {
        Impl::ThrowInvalidArgumentException("Port cannot be greater than 65535");
    }

    m_port = port;
}

void CSpxConversationManager::SetSecure(bool secure)
{
    m_secure = secure;
}

void CSpxConversationManager::SetPath(const std::string & path)
{
    if (path.empty() || path[0] != '/')
    {
        m_path = std::string("/") + path;
    }
    else
    {
        m_path = path;
    }
}

void CSpxConversationManager::SetAutomaticProxy(bool autoProxy)
{
    m_autoProxy = autoProxy;
}

void CSpxConversationManager::SetProxy(const std::string & proxy, const int port, const std::string & username, const std::string & password)
{
    m_autoProxy = false;
    m_proxyHost = proxy;
    m_proxyPort = port;
    m_proxyUsername = username;
    m_proxyPassword = password;
}

ConversationArgs CSpxConversationManager::CreateOrJoin(const CreateConversationArgs& args, const std::string& roomCode, const std::string& roomPin)
{
    SPX_DBG_TRACE_VERBOSE_IF(SPX_DBG_TRACE_CONVERSATION_ROOM_MANAGER, __FUNCTION__);

    HttpRequest request(m_host, m_port, m_secure);
    request.SetPath(m_path);
    if (false == m_autoProxy)
    {
        request.SetProxy(m_proxyHost, m_proxyPort, m_proxyUsername, m_proxyPassword);
    }

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

    if (!roomPin.empty())
    {
        request.AddQueryParameter("hostCode", roomPin);
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

    SPX_TRACE_INFO("Sending create/join room request to '%s/%s'", m_host.c_str(), m_path.c_str());
    auto response = request.SendRequest(HTTPAPI_REQUEST_POST);

    SPX_TRACE_ERROR_IF(
        !response->IsSuccess(),
        "Creating/Joining room failed with HTTP %u [%s] at '%s'",
        response->GetStatusCode(),
        response->GetHeader(ConversationConstants::RequestIdHeader).c_str(),
        GetUtcTimeString().c_str());

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
        std::string error("Server response to create/join conversation is malformed. Details: ");
        error += ex.what();
        LogError(error.c_str());
        ThrowRuntimeError(error);
    }

    SPX_TRACE_INFO(
        "Successfully created conversation '%s' [%s] at '%s'",
        roomArgs.RoomCode.c_str(),
        roomArgs.RequestId.c_str(),
        GetUtcTimeString().c_str());

    return roomArgs;
}

void CSpxConversationManager::Leave(const std::string& sessionToken)
{
    SPX_DBG_TRACE_VERBOSE_IF(SPX_DBG_TRACE_CONVERSATION_ROOM_MANAGER, __FUNCTION__);

    HttpRequest request(m_host);

    request.SetPath(ConversationConstants::RestPath);
    ADD_API_VERSION(request);
    request.AddQueryParameter(ConversationQueryParameters::AuthToken, sessionToken);

    if (false == m_autoProxy)
    {
        request.SetProxy(m_proxyHost, m_proxyPort, m_proxyUsername, m_proxyPassword);
    }

    auto response = request.SendRequest(HTTPAPI_REQUEST_DELETE);
    response->EnsureSuccess();
}

}}}}} // Microsoft::CognitiveServices::Speech::Impl::Conversation
