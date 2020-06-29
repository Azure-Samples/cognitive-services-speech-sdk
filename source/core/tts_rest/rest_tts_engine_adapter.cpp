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
#include <spx_build_information.h>

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

    SPX_IFFALSE_THROW_HR(m_endpoint.empty(), SPXERR_ALREADY_INITIALIZED);

    // Get proxy setting
    GetProxySetting();

    // Initialize websocket platform
    USP::PlatformInit(m_proxyHost.data(), m_proxyPort, m_proxyUsername.data(), m_proxyPassword.data());

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
            m_endpoint = endpointUrl;
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

        oss << HttpUtils::SchemePrefix(url.scheme)
            << url.host << ':' << url.port
            << TTS_COGNITIVE_SERVICE_URL_PATH;

        m_endpoint = oss.str();
    }

    if (m_endpoint.empty() && !region.empty())
    {
        // Construct standard voice endpoint based on region
        m_endpoint = std::string(HttpUtils::SchemePrefix(UriScheme::HTTPS)) + region + TTS_COGNITIVE_SERVICE_HOST_SUFFIX + TTS_COGNITIVE_SERVICE_URL_PATH;
    }

    SPX_IFTRUE_THROW_HR(m_endpoint.empty(), SPXERR_INVALID_ARG);
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

    auto properties = SpxQueryService<ISpxNamedProperties>(GetSite());

    SPX_IFTRUE_THROW_HR(properties == nullptr, SPXERR_UNEXPECTED_TTS_ENGINE_SITE_FAILURE);
    auto subscriptionKey = properties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_Key));
    auto token = properties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceAuthorization_Token));
     
    auto ssml = text;
    if (!isSsml)
    {
        ssml = CSpxSynthesisHelper::BuildSsml(text, properties);
    }

    SPX_DBG_TRACE_VERBOSE("SSML sent to TTS cognitive service: %s", ssml.data());

    bool hasHeader = false;
    auto outputFormat = GetOutputFormat(m_audioOutput, &hasHeader);
    auto outputFormatString = GetOutputFormatString(m_audioOutput);

    std::shared_ptr<ISpxSynthesisResult> result;

    InvokeOnSite([this, properties, requestId, ssml, subscriptionKey, token, outputFormatString, outputFormat, hasHeader, &result](const SitePtr& p) {
        result = SpxCreateObjectWithSite<ISpxSynthesisResult>("CSpxSynthesisResult", p->QueryInterface<ISpxGenericSite>());
        auto resultInit = SpxQueryInterface<ISpxSynthesisResultInit>(result);

        EnsureHttpConnection(properties);

        RestTtsRequest request;
        request.requestId = requestId;
        request.endpoint = m_endpoint;
        request.postContent = ssml;
        request.isSsml = true;
        request.subscriptionKey = subscriptionKey;
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
                    EnsureHttpConnection(properties);
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

void CSpxRestTtsEngineAdapter::EnsureHttpConnection(std::shared_ptr<ISpxNamedProperties> properties)
{
    // If the connection is already setup, skip this
    if (m_httpConnect != nullptr)
    {
        return;
    }

    // Create the connection
    auto url = HttpUtils::ParseUrl(m_endpoint);
    m_httpConnect = HTTPAPI_CreateConnection_Advanced(url.host.c_str(), url.port, url.isSecure(),
        m_proxyHost.c_str(), m_proxyPort, m_proxyUsername.c_str(), m_proxyPassword.c_str());
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

    auto singleTrustedCert = properties->GetStringValue("OPENSSL_SINGLE_TRUSTED_CERT");
    if (!singleTrustedCert.empty())
    {
        bool disableDefaultVerifyPaths = true;
        bool disableCrlCheck = properties->GetStringValue("OPENSSL_SINGLE_TRUSTED_CERT_CRL_CHECK") == "false";
        HTTPAPI_SetOption(m_httpConnect, OPTION_DISABLE_DEFAULT_VERIFY_PATHS, &disableDefaultVerifyPaths);
        HTTPAPI_SetOption(m_httpConnect, OPTION_TRUSTED_CERT, singleTrustedCert.c_str());
        HTTPAPI_SetOption(m_httpConnect, OPTION_DISABLE_CRL_CHECK, &disableCrlCheck);
    }
#else
    UNUSED(properties);
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

        if (HTTPHeaders_AddHeaderNameValuePair(httpRequestHeaders, "User-Agent", ConstructUserAgent().c_str()) != HTTP_HEADERS_OK)
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

        // X-ConnectionId will be logged as clientConnectionId, in guid format with dash
        if (HTTPHeaders_AddHeaderNameValuePair(httpRequestHeaders, "X-ConnectionId", PAL::ToString(request.requestId).c_str()) != HTTP_HEADERS_OK)
        {
            throw std::runtime_error("Could not add HTTP request header: X-ConnectionId");
        }

        if (!request.subscriptionKey.empty())
        {
            if (HTTPHeaders_AddHeaderNameValuePair(httpRequestHeaders, "Ocp-Apim-Subscription-Key", request.subscriptionKey.c_str()) != HTTP_HEADERS_OK)
            {
                throw std::runtime_error("Could not add HTTP request header: Ocp-Apim-Subscription-Key");
            }
        }

        if (!request.accessToken.empty())
        {
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

std::string CSpxRestTtsEngineAdapter::ConstructUserAgent()
{
    std::stringstream ss;

    // Set SDK info
    ss << "SpeechSDK/" << BuildInformation::g_fullVersion << " " << BuildInformation::g_buildPlatform;

    // Set OS info
    const auto osInfo = PAL::getOperatingSystem();
    ss << " (" << osInfo.platform << "; " << osInfo.version << "; " << osInfo.name << ")";

    return ss.str();
}

} } } } // Microsoft::CognitiveServices::Speech::Impl
