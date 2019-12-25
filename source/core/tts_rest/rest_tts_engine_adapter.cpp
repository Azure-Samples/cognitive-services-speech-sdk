//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// rest_tts_engine_adapter.cpp: Implementation definitions for CSpxRestTtsEngineAdapter C++ class
//

#include "stdafx.h"
#include "synthesis_helper.h"
#include "rest_tts_engine_adapter.h"
#include "create_object_helpers.h"
#include "guid_utils.h"
#include "http_utils.h"
#include "endpoint_utils.h"
#include "service_helpers.h"
#include "shared_ptr_helpers.h"
#include "property_bag_impl.h"
#include "property_id_2_name_map.h"
#include "usp.h"
#include "azure_c_shared_utility_platform_wrapper.h"
#include "azure_c_shared_utility/shared_util_options.h"

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
    Term();
}

void CSpxRestTtsEngineAdapter::Init()
{
    SPX_DBG_TRACE_VERBOSE_IF(SPX_DBG_TRACE_REST_TTS, __FUNCTION__);

    // Get proxy setting
    GetProxySetting();

    // Initialize websocket platform
    Microsoft::CognitiveServices::Speech::USP::PlatformInit(m_proxyHost.data(), m_proxyPort, m_proxyUsername.data(), m_proxyPassword.data());

    // Initialize azure-c-shared HTTP API
    HTTPAPI_Init();

    // Initialize authentication related information

    std::string endpointUrl = ISpxPropertyBagImpl::GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_Endpoint), "");
    std::string hostUrl = ISpxPropertyBagImpl::GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_Host), "");
    std::string endpoint;
    std::string region = ISpxPropertyBagImpl::GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_Region), "");
    std::string subscriptionKey = ISpxPropertyBagImpl::GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_Key), "");

    // Use custom endpoint if available and valid
    if (!endpointUrl.empty())
    {
        // Check for invalid use of auth token service endpoint

        bool isTokenServiceEndpoint = false;
        std::string endpointRegion;

        std::tie(isTokenServiceEndpoint, endpointRegion) =
            EndpointUtils::IsTokenServiceEndpoint(endpointUrl);

        if (isTokenServiceEndpoint)
        {
            // Ignore the custom endpoint and use defaults except for region
            if (!endpointRegion.empty())
            {
                region = endpointRegion;
            }
        }
        else
        {
            endpoint = endpointUrl;
        }
    }

    // Use custom host if available and there was no custom endpoint
    if (endpoint.empty() && !hostUrl.empty())
    {
        Url url = HttpUtils::ParseUrl(hostUrl);
        
        // Check FromHost restrictions
        if (!url.path.empty())
        {
            ThrowInvalidArgumentException("Resource path is not allowed in the host URI.");
        }
        if (!url.query.empty())
        {
            ThrowInvalidArgumentException("Query parameters are not allowed in the host URI.");
        }

        // Construct the full endpoint address

        std::ostringstream oss;

        oss << (url.secure ? HTTPS_PROTOCOL : HTTP_PROTOCOL)
            << url.host << ':' << url.port
            << TTS_COGNITIVE_SERVICE_URL_PATH;

        endpoint = oss.str();
    }

    if (!endpoint.empty() && !CSpxSynthesisHelper::IsCustomVoiceEndpoint(endpoint) && !CSpxSynthesisHelper::IsStandardVoiceEndpoint(endpoint))
    {
        // Scenario 1, custom endpoint (e.g. on prem), no need authentication
        m_endpoint = endpoint;
    }
    else if (!endpoint.empty() && CSpxSynthesisHelper::IsCustomVoiceEndpoint(endpoint))
    {
        // Scenario 2, custom voice, need authentication (and therefore need initialize m_authenticator)
        m_endpoint = endpoint;
        region = CSpxSynthesisHelper::ParseRegionFromCognitiveServiceEndpoint(endpoint);

        // Construct cognitive service token issue URL based on region
        auto issueTokenUrl = std::string(HTTPS_URL_PREFIX) + region + ISSUE_TOKEN_HOST_SUFFIX + ISSUE_TOKEN_URL_PATH;

        m_authenticator = std::make_shared<CSpxRestTtsAuthenticator>(issueTokenUrl, subscriptionKey, m_proxyHost, m_proxyPort, m_proxyUsername, m_proxyPassword);
    }
    else if ((endpoint.empty() && !region.empty()) || (!endpoint.empty() && CSpxSynthesisHelper::IsStandardVoiceEndpoint(endpoint)))
    {
        // Scenario 3, standard voice, need issue token (and therefore need initialize m_authenticator)
        if (endpoint.empty() && !region.empty())
        {
            // Construct standard voice endpoint based on region
            m_endpoint = std::string(HTTPS_URL_PREFIX) + region + TTS_COGNITIVE_SERVICE_HOST_SUFFIX + TTS_COGNITIVE_SERVICE_URL_PATH;
        }

        if (!endpoint.empty() && CSpxSynthesisHelper::IsStandardVoiceEndpoint(endpoint))
        {
            m_endpoint = endpoint;
            region = CSpxSynthesisHelper::ParseRegionFromCognitiveServiceEndpoint(endpoint);
        }

        // Construct cognitive service token issue URL based on region
        auto issueTokenUrl = std::string(HTTPS_URL_PREFIX) + region + ISSUE_TOKEN_HOST_SUFFIX + ISSUE_TOKEN_URL_PATH;

        m_authenticator = std::make_shared<CSpxRestTtsAuthenticator>(issueTokenUrl, subscriptionKey, m_proxyHost, m_proxyPort, m_proxyUsername, m_proxyPassword);
    }
    else
    {
        ThrowRuntimeError("Invalid combination of endpoint, region and(or) subscription key.");
    }
}

void CSpxRestTtsEngineAdapter::Term()
{
    if (m_httpConnect != nullptr)
    {
        HTTPAPI_CloseConnection(m_httpConnect);
        m_httpConnect = nullptr;
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
        ssml = CSpxSynthesisHelper::BuildSsml(text, language, voice);
    }

    SPX_DBG_TRACE_VERBOSE("SSML sent to TTS cognitive service: %s", ssml.data());

    bool hasHeader = false;
    auto outputFormat = GetOutputFormat(m_audioOutput, &hasHeader);
    auto outputFormatString = GetOutputFormatString(m_audioOutput);

    std::shared_ptr<ISpxSynthesisResult> result;

    InvokeOnSite([this, requestId, ssml, outputFormatString, outputFormat, hasHeader, &result](const SitePtr& p) {
        result = SpxCreateObjectWithSite<ISpxSynthesisResult>("CSpxSynthesisResult", p->QueryInterface<ISpxGenericSite>());
        auto resultInit = SpxQueryInterface<ISpxSynthesisResultInit>(result);

        EnsureHttpConnection();

        std::string token;
        if (m_authenticator != nullptr)
        {
            token = m_authenticator->GetAccessToken();
        }

        if (token.empty())
        {
            token = ISpxPropertyBagImpl::GetStringValue(GetPropertyName(PropertyId::SpeechServiceAuthorization_Token), "");
        }

        RestTtsRequest request;
        request.requestId = requestId;
        request.endpoint = m_endpoint;
        request.postContent = ssml;
        request.isSsml = true;
        request.accessToken = token;
        request.outputFormatString = outputFormatString;
        request.outputFormat = outputFormat;
        request.outputHasHeader = hasHeader;
        request.adapter = this;
        request.site = p;

        if (m_httpConnect != nullptr)
        {
            PostTtsRequest(m_httpConnect, request, resultInit);
            if (result->GetReason() == ResultReason::Canceled)
            {
                // The http connection could be disconnected, release it to make it re-created for next request
                HTTPAPI_CloseConnection(m_httpConnect);
                m_httpConnect = nullptr;

                if (result->GetAudioData()->empty() && request.response.body.empty())
                {
                    // Re-connect and re-send the request if disconnection happened and no data was received
                    EnsureHttpConnection();
                    resultInit->Reset();
                    SpxQueryInterface<ISpxNamedProperties>(resultInit)->SetStringValue(GetPropertyName(PropertyId::CancellationDetails_ReasonDetailedText), "");
                    PostTtsRequest(m_httpConnect, request, resultInit);
                }
            }
        }
        else
        {
            resultInit->InitSynthesisResult(request.requestId, ResultReason::Canceled, CancellationReason::Error,
                CancellationErrorCode::ConnectionFailure, nullptr, 0, request.outputFormat.get(), request.outputHasHeader);
        }});

    return result;
}

std::shared_ptr<ISpxNamedProperties> CSpxRestTtsEngineAdapter::GetParentProperties() const
{
    return SpxQueryService<ISpxNamedProperties>(GetSite());
}

void CSpxRestTtsEngineAdapter::GetProxySetting()
{
    m_proxyHost = ISpxPropertyBagImpl::GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_ProxyHostName), "");
    m_proxyPort = std::stoi(ISpxPropertyBagImpl::GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_ProxyPort), "0"));
    if (m_proxyPort < 0)
    {
        ThrowInvalidArgumentException("Invalid proxy port: %d", m_proxyPort);
    }

    m_proxyUsername = ISpxPropertyBagImpl::GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_ProxyUserName), "");
    m_proxyPassword = ISpxPropertyBagImpl::GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_ProxyPassword), "");
}

void CSpxRestTtsEngineAdapter::EnsureHttpConnection()
{
    // If the connection is already setup, skip this
    if (m_httpConnect != nullptr)
    {
        return;
    }

    // Create the connection
    auto url = HttpUtils::ParseUrl(m_endpoint);
    m_httpConnect = HTTPAPI_CreateConnection_Advanced(url.host.data(), url.port, url.secure,
        m_proxyHost.data(), m_proxyPort, m_proxyUsername.data(), m_proxyPassword.data());
    if (!m_httpConnect)
    {
        SPX_TRACE_ERROR("Could not create HTTP connection");
    }

#ifdef SPEECHSDK_USE_OPENSSL
    int tls_version = OPTION_TLS_VERSION_1_2;
    if (HTTPAPI_SetOption(m_httpConnect, OPTION_TLS_VERSION, &tls_version) != HTTPAPI_OK)
    {
        HTTPAPI_CloseConnection(m_httpConnect);
        m_httpConnect = nullptr;
        SPX_TRACE_ERROR("Could not set TLS 1.2 option");
    }
#endif
}

SpxWAVEFORMATEX_Type CSpxRestTtsEngineAdapter::GetOutputFormat(std::shared_ptr<ISpxAudioOutput> output, bool* hasHeader)
{
    auto audioStream = SpxQueryInterface<ISpxAudioStream>(output);
    auto requiredFormatSize = audioStream->GetFormat(nullptr, 0);
    auto format = SpxAllocWAVEFORMATEX(requiredFormatSize);
    audioStream->GetFormat(format.get(), requiredFormatSize);

    if (hasHeader != nullptr)
    {
        *hasHeader = SpxQueryInterface<ISpxAudioOutputFormat>(output)->HasHeader();
    }

    return format;
}

std::string CSpxRestTtsEngineAdapter::GetOutputFormatString(std::shared_ptr<ISpxAudioOutput> output)
{
    auto outputFormat = SpxQueryInterface<ISpxAudioOutputFormat>(output);
    auto formatString = outputFormat->GetFormatString();
    if (outputFormat->HasHeader())
    {
        formatString = outputFormat->GetRawFormatString();
    }

    return formatString;
}

void CSpxRestTtsEngineAdapter::PostTtsRequest(HTTP_HANDLE http_connect, RestTtsRequest& request, std::shared_ptr<ISpxSynthesisResultInit> result_init)
{
    // Parse URL
    auto url = HttpUtils::ParseUrl(request.endpoint);

    // Allocate resources
    HTTP_HEADERS_HANDLE httpRequestHeaders = HTTPHeaders_Alloc();
    if (!httpRequestHeaders)
    {
        HTTPAPI_CloseConnection(http_connect);
        throw std::runtime_error("Could not allocate HTTP request headers");
    }

    HTTP_HEADERS_HANDLE httpResponseHeaders = HTTPHeaders_Alloc();
    if (!httpResponseHeaders)
    {
        HTTPHeaders_Free(httpRequestHeaders);
        HTTPAPI_CloseConnection(http_connect);
        throw std::runtime_error("Could not allocate HTTP response headers");
    }

#ifdef __MACH__ // There is issue on streaming for IOS/OSX, disable streaming on IOS/OSX for now
    BUFFER_HANDLE buffer = BUFFER_new();
    if (!buffer)
    {
        HTTPHeaders_Free(httpRequestHeaders);
        HTTPHeaders_Free(httpResponseHeaders);
        HTTPAPI_CloseConnection(http_connect);
        throw std::runtime_error("Could not allocate HTTP data buffer");
    }
#endif

    try
    {
        // Add http headers
        if (HTTPHeaders_AddHeaderNameValuePair(httpRequestHeaders, "Host", url.host.data()) != HTTP_HEADERS_OK)
        {
            throw std::runtime_error("Could not add HTTP request header: Host");
        }

        if (HTTPHeaders_AddHeaderNameValuePair(httpRequestHeaders, "User-Agent", USER_AGENT) != HTTP_HEADERS_OK)
        {
            throw std::runtime_error("Could not add HTTP request header: User-Agent");
        }

        if (HTTPHeaders_AddHeaderNameValuePair(httpRequestHeaders, "X-Microsoft-OutputFormat", request.outputFormatString.data()) != HTTP_HEADERS_OK)
        {
            throw std::runtime_error("Could not add HTTP request header: X-Microsoft-OutputFormat");
        }

        if (HTTPHeaders_AddHeaderNameValuePair(httpRequestHeaders, "Content-Length", CSpxSynthesisHelper::itos(request.postContent.length()).data()) != HTTP_HEADERS_OK)
        {
            throw std::runtime_error("Could not add HTTP request header: Content-Length");
        }

        if (HTTPHeaders_AddHeaderNameValuePair(httpRequestHeaders, "Content-Type", request.isSsml ? "application/ssml+xml" : "text/plain text") != HTTP_HEADERS_OK)
        {
            throw std::runtime_error("Could not add HTTP request header: Content-Type");
        }

        if (HTTPHeaders_AddHeaderNameValuePair(httpRequestHeaders, "X-FD-ImpressionGUID", PAL::ToString(request.requestId).c_str()) != HTTP_HEADERS_OK)
        {
            throw std::runtime_error("Could not add HTTP request header: X-FD-ImpressionGUID");
        }

        if (!request.accessToken.empty())
        {
            // This is not always required, e.g. not required for on prem service
            if (HTTPHeaders_AddHeaderNameValuePair(httpRequestHeaders, "Authorization", (std::string("bearer ") + request.accessToken).data()) != HTTP_HEADERS_OK)
            {
                throw std::runtime_error("Could not add HTTP request header: Authorization");
            }
        }

        // Execute http request
        unsigned int statusCode = 0;
        const size_t reasonPhraseSize = 1024;
        char reasonPhrase[reasonPhraseSize];
        reasonPhrase[0] = '\0';
#ifdef __MACH__ // There is issue on streaming for IOS/OSX, disable streaming on IOS/OSX for now
        HTTPAPI_RESULT result = HTTPAPI_ExecuteRequest_With_Reason_Phrase(
            http_connect,
            HTTPAPI_REQUEST_POST,
            (std::string("/") + url.path + std::string("?") + url.query).data(),
            httpRequestHeaders,
            (unsigned char *)request.postContent.data(),
            request.postContent.length(),
            &statusCode,
            reasonPhrase,
            reasonPhraseSize,
            httpResponseHeaders,
            buffer);
#else
        HTTPAPI_RESULT result = HTTPAPI_ExecuteRequest_With_Streaming(
            http_connect,
            HTTPAPI_REQUEST_POST,
            (std::string("/") + url.path + std::string("?") + url.query).data(),
            httpRequestHeaders,
            (unsigned char *)request.postContent.data(),
            request.postContent.length(),
            &statusCode,
            reasonPhrase,
            reasonPhraseSize,
            httpResponseHeaders,
            OnChunkReceived,
            &request);
#endif

        // Check result
        if (result == HTTPAPI_OK && statusCode >= 200 && statusCode < 300)
        {
#ifdef __MACH__ // There is issue on streaming for IOS/OSX, disable streaming on IOS/OSX for now
            // Write audio to site
            unsigned char* buffer_content = BUFFER_u_char(buffer);
            size_t buffer_length = BUFFER_length(buffer);
            request.site->Write(request.adapter, request.requestId, buffer_content, (uint32_t)buffer_length);

            // Write audio to result
            result_init->InitSynthesisResult(request.requestId, ResultReason::SynthesizingAudioCompleted, REASON_CANCELED_NONE,
                CancellationErrorCode::NoError, buffer_content, buffer_length, request.outputFormat.get(), request.outputHasHeader);
#else
            // Write audio to result
            result_init->InitSynthesisResult(request.requestId, ResultReason::SynthesizingAudioCompleted, REASON_CANCELED_NONE,
                CancellationErrorCode::NoError, request.response.body.data(), request.response.body.size(), request.outputFormat.get(), request.outputHasHeader);
#endif
        }
        else
        {
            if (result != HTTPAPI_OK)
            {
                result_init->InitSynthesisResult(request.requestId, ResultReason::Canceled, CancellationReason::Error,
                    CancellationErrorCode::RuntimeError, nullptr, 0, request.outputFormat.get(), request.outputHasHeader);
            }
            else
            {
                result_init->InitSynthesisResult(request.requestId, ResultReason::Canceled, CancellationReason::Error,
                    CSpxSynthesisHelper::HttpStatusCodeToCancellationErrorCode(statusCode), nullptr, 0, request.outputFormat.get(), request.outputHasHeader);
            }

            // Build error message and set it to error detail string
            std::stringstream errorMessage;
            errorMessage << "HTTPAPI result code = " << ENUM_TO_STRING(HTTPAPI_RESULT, result) << ".";
            if (result == HTTPAPI_OK)
            {
                errorMessage << " HTTP status code = " << statusCode << ". Reason: " << reasonPhrase << ".";
            }

            auto properties = SpxQueryInterface<ISpxNamedProperties>(result_init);
            properties->SetStringValue(GetPropertyName(PropertyId::CancellationDetails_ReasonDetailedText), errorMessage.str().data());
        }
    }
    catch (...)
    {
        // Release resources
#ifdef __MACH__ // There is issue on streaming for IOS/OSX, disable streaming on IOS/OSX for now
        BUFFER_delete(buffer);
#endif
        HTTPHeaders_Free(httpRequestHeaders);
        HTTPHeaders_Free(httpResponseHeaders);

        throw;
    }

    // Release resources
#ifdef __MACH__ // There is issue on streaming for IOS/OSX, disable streaming on IOS/OSX for now
    BUFFER_delete(buffer);
#endif
    HTTPHeaders_Free(httpRequestHeaders);
    HTTPHeaders_Free(httpResponseHeaders);
}

void CSpxRestTtsEngineAdapter::OnChunkReceived(void* context, const unsigned char* buffer, size_t size)
{
    auto request = (RestTtsRequest *)context;
    request->site->Write(request->adapter, request->requestId, (uint8_t *)buffer, (uint32_t)size);

    // Append current chunk to total audio data for use of synthesis result
    std::unique_lock<std::mutex> lock(request->response.mutex);
    auto originalSize = request->response.body.size();
    request->response.body.resize(originalSize + size);
    memcpy(request->response.body.data() + originalSize, buffer, size);
}


} } } } // Microsoft::CognitiveServices::Speech::Impl
