//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_cxx_speech_config.cpp: Definitions for  speech config related C++ methods
//
#pragma once

#include <string>

#include "speechapi_cxx_properties.h"
#include "speechapi_c_common.h"
#include "speechapi_c_speech_config.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {

class SpeechConfig
{
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
    static std::shared_ptr<SpeechConfig> FromSubscription(const std::string& subscription, const std::string& region)
    {
        SPXSPEECHCONFIGHANDLE hconfig = SPXHANDLE_INVALID;
        SPX_THROW_ON_FAIL(speech_config_from_subscription(&hconfig, subscription.c_str(), region.c_str()));

        auto ptr = new SpeechConfig(hconfig);
        return std::shared_ptr<SpeechConfig>(ptr);
    }

    /// <summary>
    /// Creates an instance of the speech config with specified authorization token and region.
    /// </summary>
    /// <param name="authToken">The authorization token.</param>
    /// <param name="region">The region name (see the <a href="https://aka.ms/csspeech/region">region page</a>).</param>
    static std::shared_ptr<SpeechConfig> FromAuthorizationToken(const std::string& authToken, const std::string& region)
    {
        SPXSPEECHCONFIGHANDLE hconfig = SPXHANDLE_INVALID;
        SPX_THROW_ON_FAIL(speech_config_from_authorization_token(&hconfig, authToken.c_str(), region.c_str()));

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
    /// <param name="subscriptionKey">The subscription key.</param>
    static std::shared_ptr<SpeechConfig> FromEndpoint(const std::string& endpoint, const std::string& subscription)
    {
        SPXSPEECHCONFIGHANDLE hconfig = SPXHANDLE_INVALID;
        SPX_THROW_ON_FAIL(speech_config_from_endpoint(&hconfig, endpoint.c_str(), subscription.c_str()));

        auto ptr = new SpeechConfig(hconfig);
        return std::shared_ptr<SpeechConfig>(ptr);
    }

    /// <summary>
    /// Set the input language to the speech recognizer.
    /// </summary>
    /// <param name="lang">Specifies the name of spoken language to be recognized in BCP-47 format.</param>
    void SetSpeechRecognitionLanguage(const std::string & lang)
    {
        property_bag_set_string(m_propertybag, static_cast<int>(PropertyId::SpeechServiceConnection_RecoLanguage), nullptr, lang.c_str());
        property_bag_set_string(m_propertybag, static_cast<int>(PropertyId::SpeechServiceConnection_TranslationFromLanguage), nullptr, lang.c_str());
        property_bag_set_string(m_propertybag, static_cast<int>(PropertyId::SpeechServiceConnection_IntentSourceLanguage), nullptr, lang.c_str());
    }

    /// <summary>
    /// Gets the input language to the speech recognition.
    /// The language is specified in BCP-47 format.
    /// </summary>
    std::string GetSpeechRecognitionLanguage() const
    {
        return GetProperty(PropertyId::SpeechServiceConnection_RecoLanguage);
    }

    /// <summary>
    /// Sets the endpoint ID.
    /// </summary>
    void SetEndpointId(const std::string & endpointId)
    {
        property_bag_set_string(m_propertybag, static_cast<int>(PropertyId::SpeechServiceConnection_EndpointId), nullptr, endpointId.c_str());
    }

    /// <summary>
    /// Gets the endpoint ID.
    /// </summary>
    std::string GetEndpointId() const
    {
        return GetProperty(PropertyId::SpeechServiceConnection_EndpointId);
    }

    /// <summary>
    /// Sets the Authorization Token to connect to the service.
    /// </summary>
    void SetAuthorizationToken(const std::string& token)
    {
        property_bag_set_string(m_propertybag, static_cast<int>(PropertyId::SpeechServiceAuthorization_Token), nullptr, token.c_str());
    }

    /// <summary>
    /// Gets the Authorization token that is used to create a recognizer.
    /// </summary>
    std::string GetAuthorizationToken() const
    {
        return GetProperty(PropertyId::SpeechServiceAuthorization_Token);
    }

    /// <summary>
    /// Gets the SubscriptionKey key that used to create Speech Recognizer or Intent Recognizer or Translation Recognizer.
    /// </summary>
    std::string GetSubscriptionKey() const
    {
        return GetProperty(PropertyId::SpeechServiceConnection_Key);
    }

    /// <summary>
    /// Gets the region key that used to create Speech Recognizer or Intent Recognizer or Translation Recognizer.
    /// </summary>
    std::string GetRegion() const
    {
        return GetProperty(PropertyId::SpeechServiceConnection_Region);
    }

    /// <summary>
    /// Gets output format.
    /// </summary>
    OutputFormat GetOutputFormat() const
    {
        auto result = GetProperty(PropertyId::SpeechServiceResponse_RequestDetailedResultTrueFalse);
        return result == "true" ? OutputFormat::Detailed : OutputFormat::Simple;
    }

    /// <summary>
    /// Sets output format.
    /// </summary>
    /// <param name="format">Output format</param>
    void SetOutputFormat(OutputFormat format)
    {
        property_bag_set_string(m_propertybag, static_cast<int>(PropertyId::SpeechServiceResponse_RequestDetailedResultTrueFalse), nullptr,
            format == OutputFormat::Detailed ? "true" : "false");
    }

    /// <summary>
    /// Sets a property value by name.
    /// </summary>
    /// <param name="name">The property name.</param>
    /// <param name="value">The property value.</param>
    void SetProperty(const std::string& name, const std::string& value)
    {
        property_bag_set_string(m_propertybag, -1, name.c_str(), value.c_str());
    }

    /// <summary>
    /// Gets a property value by name.
    /// </summary>
    /// <param name="name">The parameter name.</param>
    std::string GetProperty(const std::string& name) const
    {
        const char* value = property_bag_get_string(m_propertybag, -1, name.c_str(), "");
        return CopyAndFreePropertyString(value);
    }

    /// <summary>
    /// Gets a property value by ID.
    /// </summary>
    /// <param name="id">The parameter id.</param>
    std::string GetProperty(PropertyId id) const
    {
        const char* value = property_bag_get_string(m_propertybag, static_cast<int>(id), nullptr, "");
        return CopyAndFreePropertyString(value);
    }

    /// <summary>
    /// Sets a property value by ID.
    /// </summary>
    /// <param name="id">The property id.</param>
    /// <param name="value">The property value.</param>
    void SetProperty(PropertyId id, const std::string& value)
    {
        property_bag_set_string(m_propertybag, static_cast<int>(id), nullptr, value.c_str());
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
