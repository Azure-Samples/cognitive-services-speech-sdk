//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// uspimpl.c: implementation of the USP library.
//

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif


#include <vector>
#include <set>

#include <assert.h>
#include <inttypes.h>

#include "azure_c_shared_utility_xlogging_wrapper.h"
#include "azure_c_shared_utility_httpheaders_wrapper.h"
#include "azure_c_shared_utility_platform_wrapper.h"
#include "azure_c_shared_utility_urlencode_wrapper.h"

#include "uspcommon.h"
#include "uspinternal.h"

#include "transport.h"
#include "dnscache.h"
#include "metrics.h"

#include "exception.h"

#ifdef __linux__
#include <unistd.h>
#endif

#include "string_utils.h"
#include "guid_utils.h"

#ifdef _MSC_VER
#pragma warning( push )
// disable: (8300,27): error 28020:  : The expression '0&lt;=_Param_(1)&amp;&amp;_Param_(1)&lt;=64-1' is not true at this call.
#pragma warning( disable : 28020 )
#include "json.hpp"
#pragma warning( pop )
#else
#include "json.hpp"
#endif

using namespace std;

uint64_t telemetry_gettime()
{
    auto now = chrono::high_resolution_clock::now();
    auto ms = chrono::duration_cast<chrono::milliseconds>(now.time_since_epoch());
    return ms.count();
}

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace USP {

using namespace std;
using namespace Microsoft::CognitiveServices::Speech::Impl;

template <class T>
static void throw_if_null(const T* ptr, const string& name)
{
    if (ptr == NULL)
    { 
        ThrowInvalidArgumentException("The argument '" + name +"' is null."); \
    }
}

inline bool contains(const string& content, const string& name)
{
    return (content.find(name) != string::npos) ? true : false;
}

// Todo(1126805) url builder + auth interfaces

const vector<string> g_recoModeStrings = { "interactive", "conversation", "dictation" };
const vector<string> g_outFormatStrings = { "simple", "detailed" };


// TODO: remove this as soon as metrics.c is re-written in cpp.
extern "C" {
    const char* g_keywordContentType = headers::contentType;
    const char* g_messagePathSpeechHypothesis = path::speechHypothesis.c_str();
    const char* g_messagePathSpeechPhrase = path::speechPhrase.c_str();
    const char* g_messagePathSpeechFragment = path::speechFragment.c_str();
    const char* g_messagePathTurnStart = path::turnStart.c_str();
    const char* g_messagePathTurnEnd = path::turnEnd.c_str();
    const char* g_messagePathSpeechEndDetected = path::speechEndDetected.c_str();
    const char* g_messagePathSpeechStartDetected = path::speechStartDetected.c_str();
}


// This is called from telemetry_flush, invoked on a worker thread in turn-end. 
void Connection::Impl::OnTelemetryData(const uint8_t* buffer, size_t bytesToWrite, void *context, const char *requestId)
{
    Connection::Impl *connection = (Connection::Impl*)context;
    TransportWriteTelemetry(connection->m_transport.get(), buffer, bytesToWrite, requestId);
}


Connection::Impl::Impl(const Client& config)
    : m_config(config),
    m_connected(false),
    m_haveWork(false),
    m_audioOffset(0),
    m_creationTime(telemetry_gettime())
{
    static once_flag initOnce;

    call_once(initOnce, [] {
        if (platform_init() != 0) {
            ThrowRuntimeError("Failed to initialize platform (azure-c-shared)");
        }
    });

    Validate();
}

uint64_t Connection::Impl::getTimestamp() 
{
    return telemetry_gettime() - m_creationTime;
}

void Connection::Impl::Invoke(std::function<void()> callback)
{
    if (!m_connected)
    {
        return;
    }
    m_mutex.unlock();
    callback();
    m_mutex.lock();
}

void Connection::Impl::WorkThread(weak_ptr<Connection::Impl> ptr)
{
    {
        auto connection = ptr.lock();
        if (connection == nullptr) 
        {
            return;
        }
        connection->SignalConnected();
    }

    while (true)
    {
        auto connection = ptr.lock();
        if (connection == nullptr)
        {
            // connection is destroyed, our work here is done.
            return;
        }

        unique_lock<recursive_mutex> lock(connection->m_mutex);
        if (!connection->m_connected)
        {
            return;
        }

        auto callbacks = connection->m_config.m_callbacks;

        try 
        {
            TransportDoWork(connection->m_transport.get());
        }
        catch (const exception& e)
        {
            connection->Invoke([&] { callbacks->OnError(false, e.what()); });
        }
        catch (...)
        {
            connection->Invoke([&] { callbacks->OnError(false, "Unhandled exception in the USP layer."); });
        }

        connection->m_cv.wait_for(lock, chrono::milliseconds(200), [&] {return connection->m_haveWork || !connection->m_connected; });
        connection->m_haveWork = false;
    }
}

void Connection::Impl::SignalWork()
{
    m_haveWork = true;
    m_cv.notify_one();
}

void Connection::Impl::SignalConnected()
{
    unique_lock<recursive_mutex> lock(m_mutex);
    m_connected = true;
    m_cv.notify_one();
}

void Connection::Impl::Shutdown()
{
    {
        unique_lock<recursive_mutex> lock(m_mutex);
        m_config.m_callbacks = nullptr;

        // This will force the active thread to exit at some point,
        // we do not wait on the thread in order not to block the calling side.
        m_connected = false;
        SignalWork();
    }

    // The thread has its own ref counted copy of callbacks.
    m_worker.detach();
}

void Connection::Impl::Validate()
{
    if (m_config.m_authData.empty())
    {
        ThrowInvalidArgumentException("No valid authentication mechanism was specified.");
    }
}

string Connection::Impl::EncodeParameterString(const string& parameter) const
{
    STRING_HANDLE encodedHandle = URL_EncodeString(parameter.c_str());
    string encodedStr(STRING_c_str(encodedHandle));
    STRING_delete(encodedHandle);
    return encodedStr;
}

string Connection::Impl::ConstructConnectionUrl() const
{
    auto recoMode = static_cast<underlying_type_t<RecognitionMode>>(m_config.m_recoMode);
    ostringstream oss;
    bool customEndpoint = false;

    // Using customized endpoint if it is defined.
    if (!m_config.m_customEndpointUrl.empty())
    {
        oss << m_config.m_customEndpointUrl;
        customEndpoint = true;
    }
    else
    {
        oss << endpoint::protocol;
        switch (m_config.m_endpoint)
        {
        case EndpointType::Speech:
            oss << m_config.m_region
                << endpoint::unifiedspeech::hostnameSuffix
                << endpoint::unifiedspeech::pathPrefix
                << g_recoModeStrings[recoMode]
                << endpoint::unifiedspeech::pathSuffix;
            break;
        case EndpointType::Translation:
            oss << m_config.m_region
                << endpoint::translation::hostnameSuffix
                << endpoint::translation::path;
            break;
        case EndpointType::Intent:
            oss << endpoint::luis::hostname
                << endpoint::luis::pathPrefix1
                << m_config.m_intentRegion
                << endpoint::luis::pathPrefix2
                << g_recoModeStrings[(int)RecognitionMode::Interactive]
                << endpoint::luis::pathSuffix;
            break;
        case EndpointType::CDSDK:
            oss << endpoint::CDSDK::url;
            break;
        default:
            ThrowInvalidArgumentException("Unknown endpoint type.");
        }
    }

    // The first query parameter.
    auto format = static_cast<underlying_type_t<OutputFormat>>(m_config.m_outputFormat);
    if (!customEndpoint || !contains(oss.str(), endpoint::unifiedspeech::outputFormatQueryParam))
    {
        auto delim = (customEndpoint && (oss.str().find('?') != string::npos)) ? '&' : '?';
        oss << delim << endpoint::unifiedspeech::outputFormatQueryParam << g_outFormatStrings[format];
    }

    // Todo: use libcurl or another library to encode the url as whole, instead of each parameter.
    switch(m_config.m_endpoint)
    {
    case EndpointType::Speech:
        if (!m_config.m_modelId.empty())
        {
            if (!customEndpoint || !contains(oss.str(), endpoint::unifiedspeech::deploymentIdQueryParam))
            {
                oss << '&' << endpoint::unifiedspeech::deploymentIdQueryParam << m_config.m_modelId;
            }
        }
        else if (!m_config.m_language.empty())
        {
            if (!customEndpoint || !contains(oss.str(), endpoint::unifiedspeech::langQueryParam))
            {
                oss << '&' << endpoint::unifiedspeech::langQueryParam << m_config.m_language;
            }
        }
        break;
    case EndpointType::Intent:
        if (!m_config.m_language.empty())
        {
            if (!customEndpoint || !contains(oss.str(), endpoint::unifiedspeech::langQueryParam))
            {
                oss << '&' << endpoint::unifiedspeech::langQueryParam << m_config.m_language;
            }
        }
        break;
    case EndpointType::Translation:
        if (!customEndpoint || !contains(oss.str(), endpoint::translation::from))
        {
            oss << '&' << endpoint::translation::from << EncodeParameterString(m_config.m_translationSourceLanguage);
        }
        if (!customEndpoint || !contains(oss.str(), endpoint::translation::to))
        {
            size_t start = 0;
            auto delim = ',';
            size_t end = m_config.m_translationTargetLanguages.find_first_of(delim);
            while (end != string::npos)
            {
                oss << '&' << endpoint::translation::to << EncodeParameterString(m_config.m_translationTargetLanguages.substr(start, end - start));
                start = end + 1;
                end = m_config.m_translationTargetLanguages.find_first_of(delim, start);
            }
            oss << '&' << endpoint::translation::to << EncodeParameterString(m_config.m_translationTargetLanguages.substr(start, end));
        }

        if (!m_config.m_translationVoice.empty())
        {
            if (!customEndpoint || !contains(oss.str(), endpoint::translation::voice))
            {
                oss << '&' << endpoint::translation::features << endpoint::translation::requireVoice;
                oss << '&' << endpoint::translation::voice << EncodeParameterString(m_config.m_translationVoice);
            }
        }
        break;
    case EndpointType::CDSDK:
        // no query parameter needed.
        break;
    }

    return oss.str();
}

void Connection::Impl::Connect()
{
    if (m_transport != nullptr || m_connected)
    {
        ThrowLogicError("USP connection already created.");
    }

    using HeadersPtr = deleted_unique_ptr<remove_pointer<HTTP_HEADERS_HANDLE>::type>;

    HeadersPtr connectionHeaders(HTTPHeaders_Alloc(), HTTPHeaders_Free);

    if (connectionHeaders == nullptr)
    {
        ThrowRuntimeError("Failed to create connection headers.");
    }

    auto headersPtr = connectionHeaders.get();

    if (m_config.m_endpoint == EndpointType::CDSDK)
    {
        // TODO: MSFT: 1135317 Allow for configurable audio format
        HTTPHeaders_ReplaceHeaderNameValuePair(headersPtr, headers::audioResponseFormat, "riff-16khz-16bit-mono-pcm");
        HTTPHeaders_ReplaceHeaderNameValuePair(headersPtr, headers::userAgent, g_userAgent);
    }
    
    assert(!m_config.m_authData.empty());

    switch (m_config.m_authType)
    {
    case AuthenticationType::SubscriptionKey:
        if (HTTPHeaders_ReplaceHeaderNameValuePair(headersPtr, headers::ocpApimSubscriptionKey, m_config.m_authData.c_str()) != 0)
        {
            ThrowRuntimeError("Failed to set authentication using subscription key.");
        }
        break;

    case AuthenticationType::AuthorizationToken:
        {
            ostringstream oss;
            oss << "Bearer " << m_config.m_authData;
            auto token = oss.str();
            if (HTTPHeaders_ReplaceHeaderNameValuePair(headersPtr, headers::authorization, token.c_str()) != 0)
            {
                ThrowRuntimeError("Failed to set authentication using authorization token.");
            }
        }
        break;

    // TODO(1126805): url builder + auth interfaces
    case AuthenticationType::SearchDelegationRPSToken:
        if (HTTPHeaders_ReplaceHeaderNameValuePair(headersPtr, headers::searchDelegationRPSToken, m_config.m_authData.c_str()) != 0)
        {
            ThrowRuntimeError("Failed to set authentication using Search-DelegationRPSToken.");
        }
        break;

    default:
        ThrowRuntimeError("Unsupported authentication type");
    }

    auto connectionUrl = ConstructConnectionUrl();
    LogInfo("connectionUrl=%s", connectionUrl.c_str());

    m_telemetry = TelemetryPtr(telemetry_create(Connection::Impl::OnTelemetryData, this), telemetry_destroy);
    if (m_telemetry == nullptr)
    {
        ThrowRuntimeError("Failed to create telemetry instance.");
    }

    // Log the device uuid
    metrics_device_startup(m_telemetry.get(), PAL::DeviceUuid().c_str());

    std::string connectionId = PAL::ToString(m_config.m_connectionId);
    m_transport = TransportPtr(TransportRequestCreate(connectionUrl.c_str(), this, m_telemetry.get(), headersPtr, connectionId.c_str()), TransportRequestDestroy);
    if (m_transport == nullptr)
    {
        ThrowRuntimeError("Failed to create transport request.");
    }

#ifdef __linux__
    m_dnsCache = DnsCachePtr(DnsCacheCreate(), DnsCacheDestroy);
    if (!m_dnsCache)
    {
        ThrowRuntimeError("Failed to create DNS cache.");
    }
#else
    m_dnsCache = nullptr;
#endif

    TransportSetDnsCache(m_transport.get(), m_dnsCache.get());
    TransportSetCallbacks(m_transport.get(), OnTransportError, OnTransportData);

    m_worker = thread(&Connection::Impl::WorkThread, shared_from_this());
    unique_lock<recursive_mutex> lock(m_mutex);
    m_cv.wait(lock, [&] {return m_connected; });
}

void Connection::Impl::QueueMessage(const string& path, const uint8_t *data, size_t size)
{
    unique_lock<recursive_mutex> lock(m_mutex);

    throw_if_null(data, "data");

    if (path.empty())
    {
        ThrowInvalidArgumentException("The path is null or empty.");
    }

    if (m_connected) 
    {
        (void)TransportMessageWrite(m_transport.get(), path.c_str(), data, size);
    }

    SignalWork();
}

void Connection::Impl::QueueAudioSegment(const uint8_t* data, size_t size)
{
    if (size == 0)
    {
        QueueAudioEnd();
        return;
    }

    unique_lock<recursive_mutex> lock(m_mutex);

    LogInfo("TS:%" PRIu64 ", Write %" PRIu32 " bytes audio data.", getTimestamp(), size);

    throw_if_null(data, "data");

    if (!m_connected)
    {
        return;
    }

    metrics_audiostream_data(size);

    int ret = 0;

    if (m_audioOffset == 0)
    {
        metrics_audiostream_init();
        metrics_audio_start(m_telemetry.get());

        ret = TransportStreamPrepare(m_transport.get(), "/audio");
        if (ret != 0)
        {
            ThrowRuntimeError("TransportStreamPrepare failed. error=" + to_string(ret));
        }
    }

    ret = TransportStreamWrite(m_transport.get(), data, size);
    if (ret != 0)
    {
        ThrowRuntimeError("TransportStreamWrite failed. error=" + to_string(ret));
    }

    m_audioOffset += size;
    SignalWork();
}

void Connection::Impl::QueueAudioEnd()
{
    unique_lock<recursive_mutex> lock(m_mutex);
    LogInfo("TS:%" PRIu64 ", Flush audio buffer.", getTimestamp());

    if (!m_connected || m_audioOffset == 0)
    {
        return;
    }

    auto ret = TransportStreamFlush(m_transport.get());

    m_audioOffset = 0;
    metrics_audiostream_flush();
    metrics_audio_end(m_telemetry.get());

    if (ret != 0)
    {
        ThrowRuntimeError("Returns failure, reason: TransportStreamFlush returned " + to_string(ret));
    }
    SignalWork();
}

// Callback for transport errors
void Connection::Impl::OnTransportError(TransportHandle transportHandle, TransportError reason, void* context)
{
    (void)transportHandle;
    throw_if_null(context, "context");

    Connection::Impl *connection = static_cast<Connection::Impl*>(context);
    LogInfo("TS:%" PRIu64 ", TransportError: connection:0x%x, reason=%d.", connection->getTimestamp(), connection, reason);

    auto callbacks = connection->m_config.m_callbacks;

    switch (reason)
    {
    case TRANSPORT_ERROR_NONE:
        connection->Invoke([&] { callbacks->OnError(true, "Unknown transport error."); });
        break;

    case TRANSPORT_ERROR_HTTP_BADREQUEST:
        connection->Invoke([&] { callbacks->OnError(true, "WebSocket Upgrade failed with a bad request (400). Please check the language name and deployment id, and ensure the deployment id (if used) is correctly associated with the provided subscription key."); });
        break;

    case TRANSPORT_ERROR_HTTP_UNAUTHORIZED:
        connection->Invoke([&] { callbacks->OnError(true, "WebSocket Upgrade failed with an authentication error (401). Please check the subscription key or the authorization token, and the region name."); });
        break;

    case TRANSPORT_ERROR_HTTP_FORBIDDEN:
        connection->Invoke([&] { callbacks->OnError(true, "WebSocket Upgrade failed with an authentication error (403). Please check the subscription key or the authorization token, and the region name."); });
        break;

    case TRANSPORT_ERROR_CONNECTION_FAILURE:
        connection->Invoke([&] { callbacks->OnError(true, "Connection failed (no connection to the remote host). Please check network connection, firewall setting, and the region name used to create speech factory."); });
        break;

    case TRANSPORT_ERROR_DNS_FAILURE:
        connection->Invoke([&] { callbacks->OnError(true, "Connection failed (the remote host did not respond)."); });
        break;

    case TRANSPORT_ERROR_REMOTECLOSED:
        connection->Invoke([&] { callbacks->OnError(true, "Connection was closed by the remote host."); });
        break;

    default:
        connection->Invoke([&] { callbacks->OnError(true, "Communication Error. Error code: " + to_string(reason)); });
        break;
    }

    connection->m_connected = false;
}

static RecognitionStatus ToRecognitionStatus(const string& str)
{
    const static map<string, RecognitionStatus> statusMap = {
        { "Success", RecognitionStatus::Success },
        { "NoMatch", RecognitionStatus::NoMatch },
        { "InitialSilenceTimeout", RecognitionStatus::InitialSilenceTimeout },
        { "BabbleTimeout", RecognitionStatus::InitialBabbleTimeout },
        { "Error", RecognitionStatus::Error },
        { "EndOfDictation", RecognitionStatus::EndOfDictation },
    };

    auto result = statusMap.find(str);

    if (result == statusMap.end())
    {
        PROTOCOL_VIOLATION("Unknown RecognitionStatus: %s", str.c_str());
        return RecognitionStatus::InvalidMessage;
    }
    return result->second;
}

static TranslationStatus ToTranslationStatus(const string& str)
{
    const static map<string, TranslationStatus> statusMap = {
        { "Success", TranslationStatus::Success },
        { "Error", TranslationStatus::Error },
    };

    auto result = statusMap.find(str);

    if (result == statusMap.end())
    {
        PROTOCOL_VIOLATION("Unknown TranslationStatus: %s", str.c_str());
        return TranslationStatus::InvalidMessage;
    }
    return result->second;
}

static SynthesisStatus ToSynthesisStatus(const string& str)
{
    const static map<string, SynthesisStatus> statusMap = {
        { "Success", SynthesisStatus::Success },
        { "Error", SynthesisStatus::Error },
    };

    auto result = statusMap.find(str);

    if (result == statusMap.end())
    {
        PROTOCOL_VIOLATION("Unknown SynthesisStatus: %s", str.c_str());
        return SynthesisStatus::InvalidMessage;
    }
    return result->second;
}

static SpeechHypothesisMsg RetrieveSpeechResult(const nlohmann::json& json)
{
    auto offset = json.at(json_properties::offset).get<OffsetType>();
    auto duration = json.at(json_properties::duration).get<DurationType>();
    auto textObj = json.find(json_properties::text);
    string text;
    if (textObj != json.end())
    {
        text = json.at(json_properties::text).get<string>();
    }
    return SpeechHypothesisMsg(PAL::ToWString(json.dump()), offset, duration, PAL::ToWString(text));
}

static TranslationResult RetrieveTranslationResult(const nlohmann::json& json, bool expectStatus)
{
    auto translation = json[json_properties::translation];

    TranslationResult result;
    if (expectStatus)
    {
        auto status = translation.find(json_properties::translationStatus);
        if (status != translation.end())
        {
            result.translationStatus = ToTranslationStatus(status->get<string>());
        }
        else
        {
            PROTOCOL_VIOLATION("No TranslationStatus is provided. Json: %s", translation.dump().c_str());
            result.translationStatus = TranslationStatus::InvalidMessage;
            result.failureReason = L"Status is missing in the protocol message. Response text:" + PAL::ToWString(json.dump());
        }

        auto failure = translation.find(json_properties::translationFailureReason);
        if (failure != translation.end())
        {
            result.failureReason += PAL::ToWString(failure->get<string>());
        }

        if ((result.translationStatus == TranslationStatus::Success) && (result.translations.size() == 0))
        {
            PROTOCOL_VIOLATION("No Translations text block in the message, but TranslationStatus is success. Response text:", json.dump().c_str());
        }
    }

    if (expectStatus && result.translationStatus != TranslationStatus::Success)
    {
        return result;
    }
    else
    {
        auto translations = translation.at(json_properties::translations);
        for (const auto& object : translations)
        {
            auto lang = object.at(json_properties::lang).get<string>();
            auto txt = object.at(json_properties::text).get<string>();
            if (lang.empty() && txt.empty())
            {
                PROTOCOL_VIOLATION("emtpy language and text field in translations text. lang=%s, text=%s.", lang.c_str(), txt.c_str());
                continue;
            }

            result.translations[PAL::ToWString(lang)] = PAL::ToWString(txt);
        }
        return result;
    }
}

// Callback for data available on tranport
void Connection::Impl::OnTransportData(TransportHandle transportHandle, HTTP_HEADERS_HANDLE responseHeader, const unsigned char* buffer, size_t size, unsigned int errorCode, void* context)
{
    (void)transportHandle;
    throw_if_null(context, "context");

    Connection::Impl *connection = static_cast<Connection::Impl*>(context);

    if (errorCode != 0)
    {
        LogError("Response error %d.", errorCode);
        // TODO: Lower layers need appropriate signals
        return;
    }
    else if (responseHeader == NULL)
    {
        LogError("ResponseHeader is NULL.");
        return;
    }

    auto path = HTTPHeaders_FindHeaderValue(responseHeader, KEYWORD_PATH);
    if (path == NULL)
    {
        PROTOCOL_VIOLATION("response missing '%s' header", KEYWORD_PATH);
        return;
    }

    const char* contentType = NULL;
    if (size != 0)
    {
        contentType = HTTPHeaders_FindHeaderValue(responseHeader, headers::contentType);
        if (contentType == NULL)
        {
            PROTOCOL_VIOLATION("response '%s' contains body with no content-type", path);
            return;
        }
    }

    LogInfo("TS:%" PRIu64 " Response Message: path: %s, content type: %s, size: %zu.", connection->getTimestamp(), path, contentType, size);

    string pathStr(path);
    auto callbacks = connection->m_config.m_callbacks;

    // TODO: pass the frame type (binary/text) so that we can check frame type before calling json::parse.
    if (pathStr == path::translationSynthesis)
    {
        TranslationSynthesisMsg msg;
        msg.audioBuffer = (uint8_t *)buffer;
        msg.audioLength = size;
        connection->Invoke([&] { callbacks->OnTranslationSynthesis(msg); });
        return;
    }

    auto json = (size > 0) ? nlohmann::json::parse(buffer, buffer + size) : nlohmann::json();
    if (pathStr == path::speechStartDetected || path == path::speechEndDetected)
    {
        auto offsetObj = json[json_properties::offset];
        // For whatever reason, offset is sometimes missing on the end detected message.
        auto offset = offsetObj.is_null() ? 0 : offsetObj.get<OffsetType>();

        if (path == path::speechStartDetected)
        {
            connection->Invoke([&] { callbacks->OnSpeechStartDetected({ PAL::ToWString(json.dump()), offset }); });
        }
        else 
        {
            connection->Invoke([&] { callbacks->OnSpeechEndDetected({ PAL::ToWString(json.dump()), offset }); });
        }
    }
    else if (pathStr == path::turnStart)
    {
        auto tag = json[json_properties::context][json_properties::tag].get<string>();
        connection->Invoke([&] { callbacks->OnTurnStart({ PAL::ToWString(json.dump()), tag }); });
    }
    else if (pathStr == path::turnEnd)
    {
        // flush the telemetry before invoking the onTurnEnd callback.
        // TODO: 1164154
        telemetry_flush(connection->m_telemetry.get());
        TransportCreateRequestId(connection->m_transport.get());

        connection->Invoke([&] { callbacks->OnTurnEnd({ }); });
    }
    else if (path == path::speechHypothesis || path == path::speechFragment)
    {
        auto offset = json[json_properties::offset].get<OffsetType>();
        auto duration = json[json_properties::duration].get<DurationType>();
        auto text = json[json_properties::text].get<string>();

        if (path == path::speechHypothesis)
        {
            connection->Invoke([&] { callbacks->OnSpeechHypothesis({
                PAL::ToWString(json.dump()),
                offset,
                duration,
                PAL::ToWString(text)
                }); 
            });
        }
        else
        {
            connection->Invoke([&] { callbacks->OnSpeechFragment({
                PAL::ToWString(json.dump()),
                offset,
                duration,
                PAL::ToWString(text)
                });
            });
        }
    }
    else if (path == path::speechPhrase)
    {
        SpeechPhraseMsg result;
        result.json = PAL::ToWString(json.dump());
        result.offset = json[json_properties::offset].get<OffsetType>();
        result.duration = json[json_properties::duration].get<DurationType>();
        result.recognitionStatus = ToRecognitionStatus(json[json_properties::recoStatus].get<string>());

        switch (result.recognitionStatus)
        {
        case RecognitionStatus::Success:
            if (json.find(json_properties::displayText) != json.end())
            {
                // The DisplayText field will be present only if the RecognitionStatus field has the value Success.
                // and the format output is simple
                result.displayText = PAL::ToWString(json[json_properties::displayText].get<string>());
            }
            else // Detailed
            {
                auto phrases  = json.at(json_properties::nbest);

                double confidence = 0;
                for (const auto& object : phrases)
                {
                    auto currentConfidence = object.at(json_properties::confidence).get<double>();
                    // Picking up the result with the highest confidence.
                    if (currentConfidence > confidence)
                    {
                        confidence = currentConfidence;
                        result.displayText = PAL::ToWString(object.at(json_properties::display).get<string>());
                    }
                }
            }
            connection->Invoke([&] { callbacks->OnSpeechPhrase(result); });
            break;
        case RecognitionStatus::InitialSilenceTimeout:
        case RecognitionStatus::InitialBabbleTimeout:
        case RecognitionStatus::NoMatch:
            connection->Invoke([&] { callbacks->OnSpeechPhrase(result); });
            break;
        case RecognitionStatus::Error:
            {
                auto msg = "The recognition service encountered an internal error and could not continue. Response text:" + json.dump();
                connection->Invoke([&] { callbacks->OnError(false, msg.c_str()); });
                break;
            }
        case RecognitionStatus::EndOfDictation:
            // Currently we do not communicate and of dictation to the user.
            return;
        case RecognitionStatus::InvalidMessage:
        default:
            {
                auto msg = "Responses received is invalid. Response text:" + json.dump();
                connection->Invoke([&] { callbacks->OnError(false, msg.c_str()); });
                break;
            }
        }
    }
    else if (path == path::translationHypothesis)
    {
        auto speechResult = RetrieveSpeechResult(json);
        auto translationResult = RetrieveTranslationResult(json, false);
        // TranslationStatus is always success for translation.hypothesis
        translationResult.translationStatus = TranslationStatus::Success;

        connection->Invoke([&] { callbacks->OnTranslationHypothesis({
            std::move(speechResult.json),
            speechResult.offset,
            speechResult.duration,
            std::move(speechResult.text),
            std::move(translationResult)
            });
        });
    }
    else if (path == path::translationPhrase)
    {
        auto status = ToRecognitionStatus(json.at(json_properties::recoStatus));
        auto speechResult = RetrieveSpeechResult(json);

        TranslationResult translationResult;
        switch (status)
        {
        case RecognitionStatus::EndOfDictation:
            // Currently we do not communicate and of dictation to the user.
            return;
        case RecognitionStatus::Success:
            translationResult = RetrieveTranslationResult(json, true);
            break;
        case RecognitionStatus::InitialSilenceTimeout:
        case RecognitionStatus::InitialBabbleTimeout:
        case RecognitionStatus::NoMatch:
            translationResult.translationStatus = TranslationStatus::Success;
            break;
        case RecognitionStatus::Error:
            translationResult.translationStatus = TranslationStatus::Error;
            translationResult.failureReason = L"The speech recognition service encountered an internal error and could not continue. Response text:" + PAL::ToWString(json.dump());;
            break;
        case RecognitionStatus::InvalidMessage:
        default:
            translationResult.translationStatus = TranslationStatus::InvalidMessage;
            translationResult.failureReason = L"Response received is invalid. Response text:" + PAL::ToWString(json.dump());
            break;
        }

        if (translationResult.translationStatus == TranslationStatus::Success)
        {
            connection->Invoke([&] { callbacks->OnTranslationPhrase({
                std::move(speechResult.json),
                speechResult.offset,
                speechResult.duration,
                std::move(speechResult.text),
                std::move(translationResult),
                status
                });
            });
        }
        else
        {
            connection->Invoke([&] { callbacks->OnError(false, PAL::ToString(translationResult.failureReason).c_str()); });
        }
    }
    else if (path == path::translationSynthesisEnd)
    {
        TranslationSynthesisEndMsg msg;
        std::wstring localReason;

        auto statusHandle = json.find(json_properties::synthesisStatus);
        if (statusHandle != json.end())
        {
            msg.synthesisStatus = ToSynthesisStatus(statusHandle->get<string>());
            if (msg.synthesisStatus == SynthesisStatus::InvalidMessage)
            {
                PROTOCOL_VIOLATION("Invalid synthesis status in synthesis.end message. Json=%s", json.dump().c_str());
                localReason = L"Invalid synthesis status in synthesis.end message.";
            }
        }
        else 
        {
            PROTOCOL_VIOLATION("No synthesis status in synthesis.end message. Json=%s", json.dump().c_str());
            msg.synthesisStatus = SynthesisStatus::InvalidMessage;
            localReason = L"No synthesis status in synthesis.end message.";
        }

        auto failureHandle = json.find(json_properties::translationFailureReason);
        if (failureHandle != json.end())
        {
            if (msg.synthesisStatus == SynthesisStatus::Success)
            {
                PROTOCOL_VIOLATION("FailureReason should be empty if SynthesisStatus is success. Json=%s", json.dump().c_str());
            }
            msg.failureReason = PAL::ToWString(failureHandle->get<string>());
        }

        msg.failureReason = localReason + msg.failureReason;

        if (msg.synthesisStatus == SynthesisStatus::Success)
        {
            connection->Invoke([&] { callbacks->OnTranslationSynthesisEnd(msg); });
        }
        else
        {
            connection->Invoke([&] { callbacks->OnError(false, PAL::ToString(msg.failureReason).c_str()); });
        }
    }
    else
    {
        connection->Invoke([&] { callbacks->OnUserMessage({
            pathStr,
            string(contentType == nullptr ? "" : contentType),
            buffer,
            size
            }); 
        });
    }
}

}}}}
