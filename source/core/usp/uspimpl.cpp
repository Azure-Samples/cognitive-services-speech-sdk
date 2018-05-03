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

// Todo(1126805) url builder + auth interfaces

const vector<string> g_recoModeStrings = { "interactive", "conversation", "dictation" };
const vector<string> g_outFormatStrings = { "format=simple", "format=detailed" };


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
    m_inCallback(false),
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
    m_inCallback = true;
    m_mutex.unlock();
    callback();
    m_mutex.lock();
    m_inCallback = false;
    m_cv.notify_all();
}

void Connection::Impl::WorkThread(weak_ptr<Connection::Impl> ptr)
{
    auto connection = ptr.lock();
    connection->SignalConnected();
    connection.reset();

    while (true) 
    {
        connection = ptr.lock();
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

        auto& callbacks = connection->m_config.m_callbacks;

        try 
        {
            TransportDoWork(connection->m_transport.get());
        }
        catch (const exception& e)
        {
            connection->Invoke([&] { callbacks.OnError(e.what()); });
        }
        catch (...)
        {
            connection->Invoke([&] { callbacks.OnError("Unhandled exception in the USP layer."); });
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
    unique_lock<recursive_mutex> lock(m_mutex);
    m_connected = false;
    SignalWork();
    m_cv.wait(lock, [&] {return !m_inCallback; });
}

void Connection::Impl::Validate()
{
    if (m_config.m_endpoint == EndpointType::Cris && !m_config.m_language.empty())
    {
        // TODO: make this a proper warning.
        LogInfo("WARNING: Language option for CRIS service is not yet supported and will probably be ignored.");
    }

    if (m_config.m_endpoint != EndpointType::Cris && !m_config.m_modelId.empty())
    {
        // TODO: make this a proper warning.
        LogInfo("WARNING: Modeld id option can only used in combination with a CRIS endpoint and will be ignored.");
    }

    if (m_config.m_endpoint == EndpointType::Custom && m_config.m_endpointUrl.empty())
    {
        ThrowInvalidArgumentException("No valid endpoint was specified.");
    }

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

    oss << endpoint::protocol;
    switch (m_config.m_endpoint) 
    {
    case EndpointType::BingSpeech:
        oss << endpoint::hostname::bingSpeech
            << endpoint::pathPrefix
            << g_recoModeStrings[recoMode]
            << endpoint::pathSuffix;
        break;
    case EndpointType::Cris:
        oss << m_config.m_modelId
            << endpoint::hostname::CRIS
            << endpoint::pathPrefix
            << g_recoModeStrings[recoMode]
            << endpoint::pathSuffix;
        break;
    case EndpointType::Translation:
        oss << endpoint::hostname::Translation
            << endpoint::translation::path;
        break;
    case EndpointType::CDSDK:
        oss << endpoint::hostname::CDSDK;
        break;
    case EndpointType::Custom:
        // Just returns what user passes.
        return m_config.m_endpointUrl;
        break;
    default:
        ThrowInvalidArgumentException("Unknown endpoint type.");
    }

    // The first query parameter does not require '&'.
    auto format = static_cast<underlying_type_t<OutputFormat>>(m_config.m_outputFormat);
    oss << g_outFormatStrings[format];

    // Todo: use libcurl or another library to encode the url as whole, instead of each parameter.
    if (m_config.m_endpoint == EndpointType::Translation)
    {
        oss << '&' << endpoint::translation::from << EncodeParameterString(m_config.m_translationSourceLanguage);
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

        if (!m_config.m_translationVoice.empty())
        {
            oss << '&' << endpoint::translation::features << endpoint::translation::requireVoice;
            oss << '&' << endpoint::translation::voice << EncodeParameterString(m_config.m_translationVoice);
        }
        // Need to provide cid for now.
        oss << '&' << "cid=" << m_config.m_modelId;
    }
    else if (!m_config.m_language.empty())
    {
        // Set language for non-translation recognizer.
        oss << '&' << endpoint::langQueryParam << EncodeParameterString(m_config.m_language);
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

    m_transport = TransportPtr(TransportRequestCreate(connectionUrl.c_str(), this, m_telemetry.get(), headersPtr), TransportRequestDestroy);
    if (m_transport == nullptr)
    {
        ThrowRuntimeError("Failed to create transport request.");
    }

    m_dnsCache = DnsCachePtr(DnsCacheCreate(), DnsCacheDestroy);
    if (!m_dnsCache)
    {
        ThrowRuntimeError("Failed to create DNS cache.");
    }

    TransportSetDnsCache(m_transport.get(), m_dnsCache.get());
    TransportSetCallbacks(m_transport.get(), OnTransportError, OnTransportData);

    thread worker(&Connection::Impl::WorkThread, shared_from_this());
    unique_lock<recursive_mutex> lock(m_mutex);
    m_cv.wait(lock, [&] {return m_connected; });
    worker.detach();
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

    if (!connection->m_connected)
    {
        return;
    }

    auto& callbacks = connection->m_config.m_callbacks;

    switch (reason)
    {
    case TRANSPORT_ERROR_NONE:
        connection->Invoke([&] { callbacks.OnError("Unknown transport error."); });
        break;

    case TRANSPORT_ERROR_HTTP_UNAUTHORIZED:
        connection->Invoke([&] { callbacks.OnError("WebSocket Upgrade failed with an authentication error (401)."); });
        break;

    case TRANSPORT_ERROR_HTTP_FORBIDDEN:
        connection->Invoke([&] { callbacks.OnError("WebSocket Upgrade failed with an authentication error (403)."); });
        break;

    case TRANSPORT_ERROR_CONNECTION_FAILURE:
        connection->Invoke([&] { callbacks.OnError("Connection failed (no connection to the remote host)."); });
        break;

    case TRANSPORT_ERROR_DNS_FAILURE:
        connection->Invoke([&] { callbacks.OnError("Connection failed (the remote host did not respond)."); });
        break;

    case TRANSPORT_ERROR_REMOTECLOSED:
        connection->Invoke([&] { callbacks.OnError("Connection was closed by the remote host."); });
        break;

    default:
        connection->Invoke([&] { callbacks.OnError("Communication Error. Error code: " + to_string(reason)); });
        break;
    }
}

static RecognitionStatus ToRecognitionStatus(const string& str)
{
    const static map<string, RecognitionStatus> statusMap = {
        { "Success", RecognitionStatus::Success },
        { "NoMatch", RecognitionStatus::NoMatch },
        { "InitialSilenceTimeout", RecognitionStatus::InitialSilenceTimeout },
        { "BabbleTimeout", RecognitionStatus::BabbleTimeout },
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
            result.failureReason = L"Status is missing in the protocol message.";
        }

        auto failure = translation.find(json_properties::translationFailureReason);
        if (failure != translation.end())
        {
            result.failureReason += PAL::ToWString(failure->get<string>());
        }

        if ((result.translationStatus == TranslationStatus::Success) && (result.translations.size() == 0))
        {
            PROTOCOL_VIOLATION("No Translations text block in the message, but TranslationStatus is succcess. Json:", translation.dump().c_str());
        }
    }

    if (result.translationStatus == TranslationStatus::Success)
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
    }

    return result;
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
    auto& callbacks = connection->m_config.m_callbacks;

    // TODO: pass the frame type (binary/text) so that we can check frame type before calling json::parse.
    if (pathStr == path::translationSynthesis)
    {
        TranslationSynthesisMsg msg;
        msg.audioBuffer = (uint8_t *)buffer;
        msg.audioLength = size;
        connection->Invoke([&] { callbacks.OnTranslationSynthesis(msg); });
        return;
    }

    auto json = (size > 0) ? nlohmann::json::parse(buffer, buffer + size) : nlohmann::json();
    if (pathStr == path::speechStartDetected || path == path::speechEndDetected)
    {
        auto offsetObj = json[json_properties::offset];
        // For whatever reason, offset is sometimes missing on the end detected message.
        auto offset = offsetObj.is_null() ? 0 : offsetObj.get<OffsetType>();

        if (path == path::speechStartDetected) {
            connection->Invoke([&] { callbacks.OnSpeechStartDetected({ PAL::ToWString(json.dump()), offset }); });
        }
        else 
        {
            connection->Invoke([&] { callbacks.OnSpeechEndDetected({ PAL::ToWString(json.dump()), offset }); });
        }
    }
    else if (pathStr == path::turnStart)
    {
        auto tag = json[json_properties::context][json_properties::tag].get<string>();
        connection->Invoke([&] { callbacks.OnTurnStart({ PAL::ToWString(json.dump()), tag }); });
    }
    else if (pathStr == path::turnEnd)
    {
        // flush the telemetry before invoking the onTurnEnd callback.
        // TODO: 1164154
        telemetry_flush(connection->m_telemetry.get());
        TransportCreateRequestId(connection->m_transport.get());
        
        connection->Invoke([&] { callbacks.OnTurnEnd({ }); });
    }
    else if (path == path::speechHypothesis || path == path::speechFragment)
    {
        auto offset = json[json_properties::offset].get<OffsetType>();
        auto duration = json[json_properties::duration].get<DurationType>();
        auto text = json[json_properties::text].get<string>();

        if (path == path::speechHypothesis)
        {
            connection->Invoke([&] { callbacks.OnSpeechHypothesis({
                PAL::ToWString(json.dump()),
                offset,
                duration,
                PAL::ToWString(text)
                }); 
            });
        }
        else
        {
            connection->Invoke([&] { callbacks.OnSpeechFragment({
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
        auto offset = json[json_properties::offset].get<OffsetType>();
        auto duration = json[json_properties::duration].get<DurationType>();
        auto status = ToRecognitionStatus(json[json_properties::recoStatus].get<string>());
        
        string text;
        if (status == RecognitionStatus::Success)
        {
            // The DisplayText field will be present only if the RecognitionStatus field has the value Success.
            text = json[json_properties::displayText].get<string>();
        }

        connection->Invoke([&] { callbacks.OnSpeechPhrase({
            PAL::ToWString(json.dump()),
            offset,
            duration,
            status,
            PAL::ToWString(text)
            });
        });
    }
    else if (path == path::translationHypothesis)
    {
        auto speechResult = RetrieveSpeechResult(json);
        auto translationResult = RetrieveTranslationResult(json, false);
        // TranslationStatus is always success for translation.hypothesis
        translationResult.translationStatus = TranslationStatus::Success;
        connection->Invoke([&] { callbacks.OnTranslationHypothesis({
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
        std::wstring localReason;

        auto status = ToRecognitionStatus(json.at(json_properties::recoStatus));

        if (status == RecognitionStatus::EndOfDictation)
        {
            // Currently we do not communicate and of dictation to the user.
            return;
        }

        auto speechResult = RetrieveSpeechResult(json);

        TranslationResult translationResult;
        if (status == RecognitionStatus::Success)
        {
            translationResult = RetrieveTranslationResult(json, true);
        }

        connection->Invoke([&] { callbacks.OnTranslationPhrase({
            std::move(speechResult.json),
            speechResult.offset,
            speechResult.duration,
            std::move(speechResult.text),
            std::move(translationResult),
            status
            });
        });
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

        connection->Invoke([&] { callbacks.OnTranslationSynthesisEnd(msg); });
    }
    else
    {
        connection->Invoke([&] { callbacks.OnUserMessage({
            pathStr,
            string(contentType == nullptr ? "" : contentType),
            buffer,
            size
            }); 
        });
    }
}

}}}}
