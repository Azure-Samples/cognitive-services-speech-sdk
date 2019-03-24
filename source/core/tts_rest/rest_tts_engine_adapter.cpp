//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// rest_tts_engine_adapter.cpp: Implementation definitions for CSpxRestTtsEngineAdapter C++ class
//

#include "stdafx.h"
#include "httpapi.h"
#include "rest_tts_helper.h"
#include "rest_tts_engine_adapter.h"
#include "create_object_helpers.h"
#include "guid_utils.h"
#include "handle_table.h"
#include "service_helpers.h"
#include "shared_ptr_helpers.h"
#include "property_bag_impl.h"
#include "property_id_2_name_map.h"
#include "azure_c_shared_utility/platform.h"

#define SPX_DBG_TRACE_REST_TTS 0


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


CSpxRestTtsEngineAdapter::CSpxRestTtsEngineAdapter()
{
    SPX_DBG_TRACE_VERBOSE_IF(SPX_DBG_TRACE_REST_TTS, __FUNCTION__);
}

CSpxRestTtsEngineAdapter::~CSpxRestTtsEngineAdapter()
{
    SPX_DBG_TRACE_VERBOSE_IF(SPX_DBG_TRACE_REST_TTS, __FUNCTION__);
}

void CSpxRestTtsEngineAdapter::Init()
{
    SPX_DBG_TRACE_VERBOSE_IF(SPX_DBG_TRACE_REST_TTS, __FUNCTION__);

    // Initialize websocket platform
    platform_init();

    // Initialize azure-c-shared HTTP API
    HTTPAPI_Init();

    // Initialize authentication related information

    std::string endpoint = ISpxPropertyBagImpl::GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_Endpoint), "");
    std::string region = ISpxPropertyBagImpl::GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_Region), "");
    std::string subscriptionKey = ISpxPropertyBagImpl::GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_Key), "");

    if (!endpoint.empty() && !IsCustomVoiceEndpoint(endpoint) && !IsStandardVoiceEndpoint(endpoint))
    {
        // Scenario 1, custom endpoint (e.g. on prem), no need issue token
        m_endpoint = endpoint;
    }
    else if (!endpoint.empty() && IsCustomVoiceEndpoint(endpoint))
    {
        // Scenario 2, custom voice, need issue token (and therefore need initialize m_authenticator)
        m_endpoint = endpoint;
        region = ParseRegionFromCognitiveServiceEndpoint(endpoint);

        // Construct cognitive service token issue URL based on region
        auto issueTokenUrl = std::string(HTTPS_URL_PREFIX) + region + ISSUE_TOKEN_HOST_SUFFIX + ISSUE_TOKEN_URL_PATH;

        if (subscriptionKey.empty())
        {
            ThrowRuntimeError("Subscription key is required for cognitive service TTS custom voice request.");
        }

        m_authenticator = std::make_shared<CSpxRestTtsAuthenticator>(issueTokenUrl, subscriptionKey);
    }
    else if ((endpoint.empty() && !region.empty()) || (!endpoint.empty() && IsStandardVoiceEndpoint(endpoint)))
    {
        // Scenario 3, standard voice, need issue token (and therefore need initialize m_authenticator)
        if (endpoint.empty() && !region.empty())
        {
            // Construct standard voice endpoint based on region
            m_endpoint = std::string(HTTPS_URL_PREFIX) + region + TTS_COGNITIVE_SERVICE_HOST_SUFFIX + TTS_COGNITIVE_SERVICE_URL_PATH;
        }

        if (!endpoint.empty() && IsStandardVoiceEndpoint(endpoint))
        {
            m_endpoint = endpoint;
            region = ParseRegionFromCognitiveServiceEndpoint(endpoint);
        }

        // Construct cognitive service token issue URL based on region
        auto issueTokenUrl = std::string(HTTPS_URL_PREFIX) + region + ISSUE_TOKEN_HOST_SUFFIX + ISSUE_TOKEN_URL_PATH;

        if (subscriptionKey.empty())
        {
            ThrowRuntimeError("Subscription key is required for cognitive service TTS standard voice request.");
        }

        m_authenticator = std::make_shared<CSpxRestTtsAuthenticator>(issueTokenUrl, subscriptionKey);
    }
    else
    {
        ThrowRuntimeError("Invalid combination of endpoint, region and(or) subscription key.");
    }
}

void CSpxRestTtsEngineAdapter::SetOutput(std::shared_ptr<ISpxAudioOutput> output)
{
    SPX_DBG_TRACE_VERBOSE_IF(SPX_DBG_TRACE_REST_TTS, __FUNCTION__);
    m_audioOutput = output;
}

std::shared_ptr<ISpxSynthesisResult> CSpxRestTtsEngineAdapter::Speak(const std::string& text, bool isSsml, const std::wstring& requestId)
{
    SPX_DBG_TRACE_VERBOSE_IF(SPX_DBG_TRACE_REST_TTS, __FUNCTION__);

    auto ssml = text;
    if (!isSsml)
    {
        auto language = ISpxPropertyBagImpl::GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_SynthLanguage), "");
        auto voice = ISpxPropertyBagImpl::GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_SynthVoice), "");
        ssml = CSpxRestTtsHelper::BuildSsml(text, language, voice);
    }

    SPX_DBG_TRACE_VERBOSE("SSML sent to TTS cognitive service: %s", ssml.data());

    bool hasHeader = false;
    auto outputFormat = GetOutputFormat(m_audioOutput, &hasHeader);
    auto outputFormatString = GetOutputFormatString(m_audioOutput);

    std::shared_ptr<ISpxSynthesisResult> result;

    InvokeOnSite([this, requestId, ssml, outputFormatString, outputFormat, hasHeader, &result](const SitePtr& p) {
        result = SpxCreateObjectWithSite<ISpxSynthesisResult>("CSpxSynthesisResult", p->QueryInterface<ISpxGenericSite>());
        auto resultInit = SpxQueryInterface<ISpxSynthesisResultInit>(result);
        std::string token = "";
        if (m_authenticator.get() != nullptr)
        {
            token = m_authenticator->GetAccessToken();
        }

        PostTtsRequest(m_endpoint, requestId, ssml, true, token, outputFormatString, outputFormat, hasHeader, p, this, resultInit); });

    return result;
}

std::shared_ptr<ISpxNamedProperties> CSpxRestTtsEngineAdapter::GetParentProperties() const
{
    return SpxQueryService<ISpxNamedProperties>(GetSite());
}

SpxWAVEFORMATEX_Type CSpxRestTtsEngineAdapter::GetOutputFormat(std::shared_ptr<ISpxAudioOutput> output, bool* hasHeader)
{
    auto audioStream = SpxQueryInterface<ISpxAudioStream>(output);
    auto requiredFormatSize = audioStream->GetFormat(nullptr, 0);
    auto format = SpxAllocWAVEFORMATEX(requiredFormatSize);
    audioStream->GetFormat(format.get(), requiredFormatSize);

    if (hasHeader != nullptr)
    {
        *hasHeader = audioStream->HasHeader();
    }

    return format;
}

std::string CSpxRestTtsEngineAdapter::GetOutputFormatString(std::shared_ptr<ISpxAudioOutput> output)
{
    auto audioStream = SpxQueryInterface<ISpxAudioStream>(output);
    auto formatString = audioStream->GetFormatString();
    if (audioStream->HasHeader())
    {
        formatString = audioStream->GetRawFormatString();
    }

    return formatString;
}

std::string CSpxRestTtsEngineAdapter::ParseRegionFromCognitiveServiceEndpoint(const std::string& endpoint)
{
    bool secure = false;
    const char* host = nullptr;
    size_t host_length = 0;
    int port = 0;
    const char* path = nullptr;
    size_t path_length = 0;
    const char* query = nullptr;
    size_t query_length = 0;
    CSpxRestTtsHelper::ParseHttpUrl(endpoint.data(), &secure, &host, &host_length, &port, &path, &path_length, &query, &query_length);

    std::string host_str = std::string(host, host_length);
    size_t first_dot_pos = host_str.find('.');
    if (first_dot_pos == -1)
    {
        ThrowRuntimeError("The given endpoint is not valid TTS cognitive service endpoint. Expected *.tts.speech.microsoft.com or *.voice.speech.microsoft.com");
    }

    std::string host_name_suffix = host_str.substr(first_dot_pos);
    if (PAL::stricmp(host_name_suffix.data(), TTS_COGNITIVE_SERVICE_HOST_SUFFIX) != 0 &&
        PAL::stricmp(host_name_suffix.data(), TTS_CUSTOM_VOICE_HOST_SUFFIX) != 0)
    {
        ThrowRuntimeError("The given endpoint is not valid TTS cognitive service endpoint. Expected *.tts.speech.microsoft.com or *.voice.speech.microsoft.com");
    }

    return host_str.substr(0, first_dot_pos);
}

bool CSpxRestTtsEngineAdapter::IsCustomVoiceEndpoint(const std::string& endpoint)
{
    bool secure = false;
    const char* host = nullptr;
    size_t host_length = 0;
    int port = 0;
    const char* path = nullptr;
    size_t path_length = 0;
    const char* query = nullptr;
    size_t query_length = 0;
    CSpxRestTtsHelper::ParseHttpUrl(endpoint.data(), &secure, &host, &host_length, &port, &path, &path_length, &query, &query_length);

    std::string host_str = std::string(host, host_length);
    size_t first_dot_pos = host_str.find('.');
    if (first_dot_pos == -1)
    {
        return false;
    }

    std::string host_name_suffix = host_str.substr(first_dot_pos);

    if (PAL::stricmp(host_name_suffix.data(), TTS_CUSTOM_VOICE_HOST_SUFFIX) == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool CSpxRestTtsEngineAdapter::IsStandardVoiceEndpoint(const std::string& endpoint)
{
    bool secure = false;
    const char* host = nullptr;
    size_t host_length = 0;
    int port = 0;
    const char* path = nullptr;
    size_t path_length = 0;
    const char* query = nullptr;
    size_t query_length = 0;
    CSpxRestTtsHelper::ParseHttpUrl(endpoint.data(), &secure, &host, &host_length, &port, &path, &path_length, &query, &query_length);

    std::string host_str = std::string(host, host_length);
    size_t first_dot_pos = host_str.find('.');
    if (first_dot_pos == -1)
    {
        return false;
    }

    std::string host_name_suffix = host_str.substr(first_dot_pos);

    if (PAL::stricmp(host_name_suffix.data(), TTS_COGNITIVE_SERVICE_HOST_SUFFIX) == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void CSpxRestTtsEngineAdapter::PostTtsRequest(const std::string& endpoint, const std::wstring& requestId, const std::string& post_content, bool is_ssml, \
    const std::string& access_token, const std::string& output_format_string, SpxWAVEFORMATEX_Type output_format, bool output_has_header, \
    const SitePtr& site, ISpxTtsEngineAdapter* adapter, std::shared_ptr<ISpxSynthesisResultInit> result_init)
{
    // Parse host name & path from URL
    bool secure = false;
    const char* host = nullptr;
    size_t host_length = 0;
    int port = 0;
    const char* path = nullptr;
    size_t path_length = 0;
    const char* query = nullptr;
    size_t query_length = 0;
    CSpxRestTtsHelper::ParseHttpUrl(endpoint.data(), &secure, &host, &host_length, &port, &path, &path_length, &query, &query_length);

    std::string host_str = std::string(host, host_length);
    std::string path_str = std::string("/") + std::string(path, path_length);
    std::string query_str = std::string("?") + std::string(query, query_length);

    // Allocate resources
    HTTP_HANDLE http_connect = HTTPAPI_CreateConnection(host_str.data());
    HTTP_HEADERS_HANDLE httpRequestHeaders = HTTPHeaders_Alloc();
    HTTP_HEADERS_HANDLE httpResponseHeaders = HTTPHeaders_Alloc();
    BUFFER_HANDLE buffer = BUFFER_new();

    try
    {
        // Add http headers
        HTTPHeaders_AddHeaderNameValuePair(httpRequestHeaders, "Host", host_str.data());
        HTTPHeaders_AddHeaderNameValuePair(httpRequestHeaders, "User-Agent", USER_AGENT);
        HTTPHeaders_AddHeaderNameValuePair(httpRequestHeaders, "X-Microsoft-OutputFormat", output_format_string.data());
        HTTPHeaders_AddHeaderNameValuePair(httpRequestHeaders, "Content-Length", CSpxRestTtsHelper::itos(post_content.length()).data());
        HTTPHeaders_AddHeaderNameValuePair(httpRequestHeaders, "Content-Type", is_ssml ? "application/ssml+xml" : "text/plain text");
        if (!access_token.empty())
        {
            // This is not always required, e.g. not required for on prem service
            HTTPHeaders_AddHeaderNameValuePair(httpRequestHeaders, "Authorization", (std::string("bearer ") + access_token).data());
        }

        // Execute http request
        unsigned int statusCode = 0;
        HTTPAPI_RESULT result = HTTPAPI_ExecuteRequest(
            http_connect,
            HTTPAPI_REQUEST_POST,
            (path_str + query_str).data(),
            httpRequestHeaders,
            (unsigned char *)post_content.data(),
            post_content.length(),
            &statusCode,
            httpResponseHeaders,
            buffer);

        // Check result
        if (result == HTTPAPI_OK && statusCode >= 200 && statusCode < 300)
        {
            unsigned char* buffer_content = BUFFER_u_char(buffer);
            size_t buffer_length = BUFFER_length(buffer);

            // Write audio to site
            site->Write(adapter, requestId, buffer_content, (uint32_t)buffer_length);

            // Write audio to result
            result_init->InitSynthesisResult(requestId, ResultReason::SynthesizingAudioCompleted, REASON_CANCELED_NONE,
                CancellationErrorCode::NoError, buffer_content, buffer_length, output_format.get(), output_has_header);
        }
        else
        {
            if (result != HTTPAPI_OK)
            {
                result_init->InitSynthesisResult(requestId, ResultReason::Canceled, CancellationReason::Error,
                    CancellationErrorCode::RuntimeError, nullptr, 0, output_format.get(), output_has_header);
            }
            else
            {
                result_init->InitSynthesisResult(requestId, ResultReason::Canceled, CancellationReason::Error,
                    CSpxRestTtsHelper::HttpStatusCodeToCancellationErrorCode(statusCode), nullptr, 0, output_format.get(), output_has_header);
            }

            // Build error message and set it to error detail string
            std::stringstream errorMessage;
            errorMessage << "HTTPAPI result code = " << ENUM_TO_STRING(HTTPAPI_RESULT, result) << ".";
            if (result == HTTPAPI_OK)
            {
                errorMessage << " HTTP status code = " << statusCode << ".";
            }

            auto properties = SpxQueryInterface<ISpxNamedProperties>(result_init);
            properties->SetStringValue(GetPropertyName(PropertyId::CancellationDetails_ReasonDetailedText), errorMessage.str().data());
        }
    }
    catch (...)
    {
        // Release resources
        BUFFER_delete(buffer);
        HTTPHeaders_Free(httpRequestHeaders);
        HTTPHeaders_Free(httpResponseHeaders);
        HTTPAPI_CloseConnection(http_connect);

        throw;
    }

    // Release resources
    BUFFER_delete(buffer);
    HTTPHeaders_Free(httpRequestHeaders);
    HTTPHeaders_Free(httpResponseHeaders);
    HTTPAPI_CloseConnection(http_connect);
}


} } } } // Microsoft::CognitiveServices::Speech::Impl
