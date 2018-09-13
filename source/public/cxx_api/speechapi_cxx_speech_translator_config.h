//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#pragma once

#include <string>
#include <sstream>

#include "speechapi_cxx_properties.h"
#include "speechapi_c_common.h"
#include "speechapi_c_speech_config.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Translation {

class SpeechTranslatorConfig final : public SpeechConfig
{
public:
    /// <summary>
    /// Creates an instance of the speech translator config with specified subscription key and region.
    /// </summary>
    /// <param name="subscription">The subscription key.</param>
    /// <param name="region">The region name (see the <a href="https://aka.ms/csspeech/region">region page</a>).</param>
    static std::shared_ptr<SpeechTranslatorConfig> FromSubscription(const std::string& subscription, const std::string& region)
    {
        SPXSPEECHCONFIGHANDLE hconfig = SPXHANDLE_INVALID;
        SPX_THROW_ON_FAIL(speech_config_from_subscription(&hconfig, subscription.c_str(), region.c_str()));
        return std::shared_ptr<SpeechTranslatorConfig>(new SpeechTranslatorConfig(hconfig));
    }

    /// <summary>
    /// Creates an instance of the speech translator config with specified authorization token and region.
    /// </summary>
    /// <param name="authToken">The authorization token.</param>
    /// <param name="region">The region name (see the <a href="https://aka.ms/csspeech/region">region page</a>).</param>
    static std::shared_ptr<SpeechTranslatorConfig> FromAuthorizationToken(const std::string& authToken, const std::string& region)
    {
        SPXSPEECHCONFIGHANDLE hconfig = SPXHANDLE_INVALID;
        SPX_THROW_ON_FAIL(speech_config_from_authorization_token(&hconfig, authToken.c_str(), region.c_str()));
        return std::shared_ptr<SpeechTranslatorConfig>(new SpeechTranslatorConfig(hconfig));
    }

    // <summary>
    /// Creates an instance of the speech translator config with specified endpoint and subscription.
    /// This method is intended only for users who use a non-standard service endpoint.
    /// Note: The query parameters specified in the endpoint URL are not changed, even if they are set by any other APIs.
    /// For example, if language is defined in uri as query parameter "language=de-DE", and also set by CreateSpeechRecognizer("en-US"),
    /// the language setting in uri takes precedence, and the effective language is "de-DE".
    /// Only the parameters that are not specified in the endpoint URL can be set by other APIs.
    /// </summary>
    /// <param name="endpoint">The service endpoint to connect to.</param>
    /// <param name="subscriptionKey">The subscription key.</param>
    static std::shared_ptr<SpeechTranslatorConfig> FromEndpoint(const std::string& endpoint, const std::string& subscription)
    {
        SPXSPEECHCONFIGHANDLE hconfig = SPXHANDLE_INVALID;
        SPX_THROW_ON_FAIL(speech_config_from_endpoint(&hconfig, endpoint.c_str(), subscription.c_str()));
        return std::shared_ptr<SpeechTranslatorConfig>(new SpeechTranslatorConfig(hconfig));
    }

    /// <summary>
    /// Adds target language for translation.
    /// </summary>
    void AddTargetLanguage(const std::string& language)
    {
        if (!m_targetLanguages.empty())
            m_targetLanguages += ",";
        m_targetLanguages += language;
        property_bag_set_string(m_propertybag, static_cast<int>(SpeechPropertyId::SpeechServiceConnection_TranslationToLanguages), nullptr, m_targetLanguages.c_str());
    }

    /// <summary>
    /// Gets target languages for translation.
    /// </summary>
    std::vector<std::string> GetTargetLanguages() const
    {
        std::vector<std::string> result;
        auto targetLanguges = GetProperty(SpeechPropertyId::SpeechServiceConnection_TranslationToLanguages);
        if (targetLanguges.empty())
            return result;

        // Getting languages one by one.
        std::stringstream langaugeStream(targetLanguges);
        std::string token;
        while (std::getline(langaugeStream, token, ','))
        {
            result.push_back(token);
        }
        return result;
    }

    /// <summary>
    /// Sets output voice name.
    /// </summary>
    void SetVoiceName(const std::string& voice)
    {
        property_bag_set_string(m_propertybag, static_cast<int>(SpeechPropertyId::SpeechServiceConnection_TranslationFeatures), nullptr, "textToSpeech");
        property_bag_set_string(m_propertybag, static_cast<int>(SpeechPropertyId::SpeechServiceConnection_TranslationVoice), nullptr, voice.c_str());
    }

    /// <summary>
    /// Gets output voice name.
    /// </summary>
    std::string GetVoiceName() const
    {
        return GetProperty(SpeechPropertyId::SpeechServiceConnection_TranslationVoice);
    }

private:
    /// <summary>
    /// Internal constructor. Creates a new instance using the provided handle.
    /// </summary>
    explicit SpeechTranslatorConfig(SPXSPEECHCONFIGHANDLE hconfig) : SpeechConfig(hconfig) { }

    DISABLE_COPY_AND_MOVE(SpeechTranslatorConfig);

    std::string m_targetLanguages;
};

}}}}
