//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
// speechapi_cxx_speech_config.cpp: Definitions for  speech config related C++ methods
//
#pragma once

#include <string>

#include "speechapi_cxx_properties.h"
#include <speechapi_cxx_string_helpers.h>
#include "speechapi_c_common.h"
#include "speechapi_c_speech_config.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {

/// <summary>
/// Class that defines configurations for speech or intent recognition.
/// </summary>
class SpeechConfig
{
friend class PropertyCollection;
public:
    /// <summary>
    /// Internal operator used to get underlying handle value.
    /// </summary>
    explicit operator SPXSPEECHCONFIGHANDLE() const { return m_hconfig; }
    
    /// <summary>
    /// Creates an instance of the speech config with specified subscription key and region.
    /// </summary>
    /// <param name="subscription">The subscription key.</param>
    /// <param name="region">The region name (see the <a href="https://aka.ms/csspeech/region">region page</a>).</param>
    /// <returns>A shared pointer to the new speech config instance.</returns>
    static std::shared_ptr<SpeechConfig> FromSubscription(const SPXSTRING& subscription, const SPXSTRING& region)
    {
        SPXSPEECHCONFIGHANDLE hconfig = SPXHANDLE_INVALID;
        SPX_THROW_ON_FAIL(speech_config_from_subscription(&hconfig, Utils::ToUTF8(subscription).c_str(), Utils::ToUTF8(region).c_str()));

        auto ptr = new SpeechConfig(hconfig);
        return std::shared_ptr<SpeechConfig>(ptr);
    }

    /// <summary>
    /// Creates an instance of the speech config with specified authorization token and region.
    /// </summary>
    /// <param name="authToken">The authorization token.</param>
    /// <param name="region">The region name (see the <a href="https://aka.ms/csspeech/region">region page</a>).</param>
    /// <returns>A shared pointer to the new speech config instance.</returns>
    static std::shared_ptr<SpeechConfig> FromAuthorizationToken(const SPXSTRING& authToken, const SPXSTRING& region)
    {
        SPXSPEECHCONFIGHANDLE hconfig = SPXHANDLE_INVALID;
        SPX_THROW_ON_FAIL(speech_config_from_authorization_token(&hconfig, Utils::ToUTF8(authToken).c_str(), Utils::ToUTF8(region).c_str()));

        auto ptr = new SpeechConfig(hconfig);
        return std::shared_ptr<SpeechConfig>(ptr);
    }

    // <summary>
    /// Creates an instance of the speech config with specified endpoint and subscription.
    /// This method is intended only for users who use a non-standard service endpoint.
    /// Note: The query parameters specified in the endpoint URL are not changed, even if they are set by any other APIs.
    /// For example, if language is defined in uri as query parameter "language=de-DE", and also set by CreateSpeechRecognizer("en-US"),
    /// the language setting in uri takes precedence, and the effective language is "de-DE".
    /// Only the parameters that are not specified in the endpoint URL can be set by other APIs.
    /// </summary>
    /// <param name="endpoint">The service endpoint to connect to.</param>
    /// <param name="subscription">The subscription key.</param>
    /// <returns>A shared pointer to the new speech config instance.</returns>
    static std::shared_ptr<SpeechConfig> FromEndpoint(const SPXSTRING& endpoint, const SPXSTRING& subscription)
    {
        SPXSPEECHCONFIGHANDLE hconfig = SPXHANDLE_INVALID;
        SPX_THROW_ON_FAIL(speech_config_from_endpoint(&hconfig, Utils::ToUTF8(endpoint).c_str(), Utils::ToUTF8(subscription).c_str()));

        auto ptr = new SpeechConfig(hconfig);
        return std::shared_ptr<SpeechConfig>(ptr);
    }

    /// <summary>
    /// Set the input language to the speech recognizer.
    /// </summary>
    /// <param name="lang">Specifies the name of spoken language to be recognized in BCP-47 format.</param>
    void SetSpeechRecognitionLanguage(const SPXSTRING& lang)
    {
        property_bag_set_string(m_propertybag, static_cast<int>(PropertyId::SpeechServiceConnection_RecoLanguage), nullptr, Utils::ToUTF8(lang).c_str());
    }

    /// <summary>
    /// Gets the input language to the speech recognition.
    /// The language is specified in BCP-47 format.
    /// </summary>
    /// <returns>The speech recognition language.</returns>
    SPXSTRING GetSpeechRecognitionLanguage() const
    {
        return GetProperty(PropertyId::SpeechServiceConnection_RecoLanguage);
    }

    /// <summary>
    /// Sets the endpoint ID.
    /// </summary>
    /// <param name="endpointId">Endpoint ID.</param>
    void SetEndpointId(const SPXSTRING& endpointId)
    {
        property_bag_set_string(m_propertybag, static_cast<int>(PropertyId::SpeechServiceConnection_EndpointId), nullptr, Utils::ToUTF8(endpointId).c_str());
    }

    /// <summary>
    /// Gets the endpoint ID.
    /// </summary>
    /// <returns>Endpoint ID.</returns>
    SPXSTRING GetEndpointId() const
    {
        return GetProperty(PropertyId::SpeechServiceConnection_EndpointId);
    }

    /// <summary>
    /// Sets the authorization token to connect to the service.
    /// </summary>
    /// <param name="token">The authorization token.</param>
    void SetAuthorizationToken(const SPXSTRING& token)
    {
        property_bag_set_string(m_propertybag, static_cast<int>(PropertyId::SpeechServiceAuthorization_Token), nullptr, Utils::ToUTF8(token).c_str());
    }

    /// <summary>
    /// Gets the authorization token to connect to the service.
    /// </summary>
    /// <returns>The authorization token.</returns>
    SPXSTRING GetAuthorizationToken() const
    {
        return GetProperty(PropertyId::SpeechServiceAuthorization_Token);
    }

    /// <summary>
    /// Gets the subscription key that is used to create Speech Recognizer or Intent Recognizer or Translation Recognizer.
    /// </summary>
    /// <returns>The subscription key.</returns>
    SPXSTRING GetSubscriptionKey() const
    {
        return GetProperty(PropertyId::SpeechServiceConnection_Key);
    }

    /// <summary>
    /// Gets the region key that used to create Speech Recognizer or Intent Recognizer or Translation Recognizer.
    /// </summary>
    /// <returns>Region.</returns>
    SPXSTRING GetRegion() const
    {
        return GetProperty(PropertyId::SpeechServiceConnection_Region);
    }

    /// <summary>
    /// Gets output format.
    /// </summary>
    /// <returns>Output format.</returns>
    OutputFormat GetOutputFormat() const
    {
        auto result = GetProperty(PropertyId::SpeechServiceResponse_RequestDetailedResultTrueFalse);
        return result == Utils::ToSPXString("true") ? OutputFormat::Detailed : OutputFormat::Simple;
    }

    /// <summary>
    /// Sets output format.
    /// </summary>
    /// <param name="format">Output format</param>
    void SetOutputFormat(OutputFormat format)
    {
        property_bag_set_string(m_propertybag, static_cast<int>(PropertyId::SpeechServiceResponse_RequestDetailedResultTrueFalse), nullptr,
            format == OutputFormat::Detailed ? Utils::ToUTF8("true") : Utils::ToUTF8("false"));
    }

    /// <summary>
    /// Sets proxy configuration
    /// Added in version 1.1.0
    /// </summary>
    /// <param name="proxyHostName">The host name of the proxy server</param>
    /// <param name="proxyPort">The port number of the proxy server</param>
    /// <param name="proxyUserName">The user name of the proxy server</param>
    /// <param name="proxyPassword">The password of the proxy server</param>
    void SetProxy(const SPXSTRING& proxyHostName, uint32_t proxyPort, const SPXSTRING& proxyUserName = SPXSTRING(), const SPXSTRING& proxyPassword = SPXSTRING())
    {
        property_bag_set_string(m_propertybag, static_cast<int>(PropertyId::SpeechServiceConnection_ProxyHostName), nullptr,
            Utils::ToUTF8(proxyHostName).c_str());
        property_bag_set_string(m_propertybag, static_cast<int>(PropertyId::SpeechServiceConnection_ProxyPort), nullptr,
            std::to_string(proxyPort).c_str());
        property_bag_set_string(m_propertybag, static_cast<int>(PropertyId::SpeechServiceConnection_ProxyUserName), nullptr,
            Utils::ToUTF8(proxyUserName).c_str());
        property_bag_set_string(m_propertybag, static_cast<int>(PropertyId::SpeechServiceConnection_ProxyPassword), nullptr,
            Utils::ToUTF8(proxyPassword).c_str());
    }

    /// <summary>
    /// Sets a property value by name.
    /// </summary>
    /// <param name="name">The property name.</param>
    /// <param name="value">The property value.</param>
    void SetProperty(const SPXSTRING& name, const SPXSTRING& value)
    {
        property_bag_set_string(m_propertybag, -1, Utils::ToUTF8(name).c_str(), Utils::ToUTF8(value).c_str());
    }

    /// <summary>
    /// Gets a property value by name.
    /// </summary>
    /// <param name="name">The parameter name.</param>
    /// <returns>The property value.</returns>
    SPXSTRING GetProperty(const SPXSTRING& name) const
    {
        const char* value = property_bag_get_string(m_propertybag, -1, Utils::ToUTF8(name).c_str(), "");
        return Utils::ToSPXString(CopyAndFreePropertyString(value));
    }

    /// <summary>
    /// Gets a property value by ID.
    /// </summary>
    /// <param name="id">The parameter id.</param>
    /// <returns>The property value.</returns>
    SPXSTRING GetProperty(PropertyId id) const
    {
        const char* value = property_bag_get_string(m_propertybag, static_cast<int>(id), nullptr, "");
        return Utils::ToSPXString(CopyAndFreePropertyString(value));
    }

    /// <summary>
    /// Sets a property value by ID.
    /// </summary>
    /// <param name="id">The property id.</param>
    /// <param name="value">The property value.</param>
    void SetProperty(PropertyId id, const SPXSTRING& value)
    {
        property_bag_set_string(m_propertybag, static_cast<int>(id), nullptr, Utils::ToUTF8(value).c_str());
    }

    /// <summary>
    /// Destructs the object.
    /// </summary>
    virtual ~SpeechConfig()
    {
        speech_config_release(m_hconfig);
        property_bag_release(m_propertybag);
    }

protected:

    /*! \cond PROTECTED */

    /// <summary>
    /// Internal constructor. Creates a new instance using the provided handle.
    /// </summary>
    explicit SpeechConfig(SPXSPEECHCONFIGHANDLE hconfig)
        :m_hconfig(hconfig)
        {
            SPX_THROW_ON_FAIL(speech_config_get_property_bag(hconfig, &m_propertybag));
        }

    /// <summary>
    /// Internal member variable that holds the speech config
    /// </summary>
    SPXSPEECHCONFIGHANDLE m_hconfig;

    /// <summary>
    /// Internal member variable that holds the properties of the speech config
    /// </summary>
    SPXPROPERTYBAGHANDLE m_propertybag;

    /*! \endcond */

private:
    DISABLE_COPY_AND_MOVE(SpeechConfig);

    inline static std::string CopyAndFreePropertyString(const char* value)
    {
        std::string copy = (value == nullptr) ? "" : value;
        property_bag_free_string(value);
        return copy;
    }
};

}}}
