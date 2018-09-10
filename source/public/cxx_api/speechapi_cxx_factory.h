//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_cxx_factory.h: Public API declarations for SpeechFactory C++ static class methods
//

#pragma once
#include <memory>
#include <vector>
#include <speechapi_cxx_common.h>
#include <speechapi_cxx_enums.h>
#include <speechapi_cxx_audio_config.h>
#include <speechapi_cxx_speech_recognizer.h>
#include <speechapi_cxx_intent_recognizer.h>
#include <speechapi_cxx_translation_recognizer.h>
#include <speechapi_cxx_properties.h>
#include <speechapi_cxx_audio_config.h>
#include <speechapi_c_factory.h>


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {


/// <summary>
/// ISpeechFactory interface, defines a number of factory methods for creating various recognizers.
/// </summary>
class ISpeechFactory
{
public:

    /// <summary>
    /// Creates a new factory instance parametrized with an instance of FactoryParameterCollection.
    /// </summary>
    explicit ISpeechFactory(PropertyCollection<SPXFACTORYHANDLE>& parameters) : Parameters(parameters) { }
    
    /// <summary>
    /// Virtual destructor.
    /// </summary>
    virtual ~ISpeechFactory() { }

    /// <summary>
    /// Collection of parameters used for all recognizers created by the factory.
    /// </summary>
    PropertyCollection<SPXFACTORYHANDLE>& Parameters;


private:

    DISABLE_COPY_AND_MOVE(ISpeechFactory);
};


/// <summary>
/// ICognitiveServicesSpeechFactory interface.
/// </summary>
class ICognitiveServicesSpeechFactory : public ISpeechFactory
{
public:

    /// <summary>
    /// Creates a new factory instance parameterized with an instance of FactoryParameterCollection.
    /// </summary>
    explicit ICognitiveServicesSpeechFactory(PropertyCollection<SPXFACTORYHANDLE>& parameters) : ISpeechFactory(parameters) { }

    /// <summary>
    /// Creates a SpeechRecognizer, using the default microphone as input.
    /// NOTE: Remove this overload once SpeechConfig object exists...
    /// </summary>
    /// <returns>A shared pointer to SpeechRecognizer</returns>
    virtual std::shared_ptr<Speech::SpeechRecognizer> CreateSpeechRecognizer() = 0;

    // TODO: Remove this after Objective-C has proper AudioConfig support
    virtual std::shared_ptr<Speech::SpeechRecognizer> CreateSpeechRecognizerWithFileInputHACKFOROBJECTIVEC(const std::string& fileName) = 0;

    /// <summary>
    /// Creates a SpeechRecognizer for the specified spoken language, using the default microphone as input.
    /// NOTE: Remove this overload once SpeechConfig object exists...
    /// </summary>
    /// <param name="language">Specifies the name of spoken language to be recognized in BCP-47 format.</param>
    /// <returns>A shared pointer to SpeechRecognizer</returns>
    virtual std::shared_ptr<Speech::SpeechRecognizer> CreateSpeechRecognizerFromConfig(const std::string& language) = 0;

    /// <summary>
    /// Creates a SpeechRecognizer, using the specified audio input configuration.
    /// NOTE: Remove this overload once SpeechConfig object exists...
    /// </summary>
    /// <param name="audioInput">The audio input configuration (e.g. microhone, file, or custom audio input stream).</param>
    /// <returns>A shared pointer to SpeechRecognizer</returns>
    virtual std::shared_ptr<Speech::SpeechRecognizer> CreateSpeechRecognizerFromConfig(std::shared_ptr<Audio::AudioConfig> audioInput) = 0;

    /// <summary>
    /// Creates a SpeechRecognizer for the specified spoken language, using the specified audio input configuration.
    /// NOTE: Remove this overload once SpeechConfig object exists...
    /// </summary>
    /// <param name="language">Specifies the name of spoken language to be recognized in BCP-47 format.</param>
    /// <param name="audioInput">The audio input configuration (e.g. microhone, file, or custom audio input stream).</param>
    /// <returns>A shared pointer to SpeechRecognizer</returns>
    virtual std::shared_ptr<Speech::SpeechRecognizer> CreateSpeechRecognizerFromConfig(const std::string& language, std::shared_ptr<Audio::AudioConfig> audioInput) = 0;

    /// <summary>
    /// Creates a SpeechRecognizer for the specified spoken language, and the specified output format.
    /// NOTE: Remove this overload once SpeechConfig object exists...
    /// </summary>
    /// <param name="language">Specifies the name of spoken language to be recognized in BCP-47 format.</param>
    /// <param name="format">Output format.</param>
    /// <returns>A shared pointer to SpeechRecognizer</returns>
    virtual std::shared_ptr<Speech::SpeechRecognizer> CreateSpeechRecognizerFromConfig(const std::string& language, OutputFormat format) = 0;

    /// <summary>
    /// Creates a SpeechRecognizer for the specified spoken language, using the specified audio input configuration.
    /// NOTE: Remove this overload once SpeechConfig object exists...
    /// </summary>
    /// <param name="language">Specifies the name of spoken language to be recognized in BCP-47 format.</param>
    /// <param name="format">Output format.</param>
    /// <param name="audioInput">The audio input configuration (e.g. microhone, file, or custom audio input stream).</param>
    /// <returns>A shared pointer to SpeechRecognizer</returns>
    virtual std::shared_ptr<Speech::SpeechRecognizer> CreateSpeechRecognizerFromConfig(const std::string& language, OutputFormat format, std::shared_ptr<Audio::AudioConfig> audioInput) = 0;

    /// <summary>
    /// Creates an IntentRecognizer, using the default microphone as input.
    /// </summary>
    /// <returns>A shared pointer to IntentRecognizer</returns>
    virtual std::shared_ptr<Intent::IntentRecognizer> CreateIntentRecognizer() = 0;

    /// <summary>
    /// Creates an IntentRecognizer for the specified spoken language, using the default microphone as input.
    /// </summary>
    /// <param name="language">Specifies the name of spoken language to be recognized in BCP-47 format.</param>
    /// <returns>A shared pointer to IntentRecognizer</returns>
    virtual std::shared_ptr<Intent::IntentRecognizer> CreateIntentRecognizerFromConfig(const std::string& language) = 0;

    /// <summary>
    /// Creates an IntentRecognizer, using the specified audio input configuration.
    /// NOTE: Remove this overload once SpeechConfig object exists...
    /// </summary>
    /// <param name="audioInput">The audio input configuration (e.g. microhone, file, or custom audio input stream).</param>
    /// <returns>A shared pointer to IntentRecognizer</returns>
    virtual std::shared_ptr<Intent::IntentRecognizer> CreateIntentRecognizerFromConfig(std::shared_ptr<Audio::AudioConfig> audioInput) = 0;

    /// <summary>
    /// Creates an IntentRecognizer for the specified spoken language, using the specified audio input configuration.
    /// NOTE: Remove this overload once SpeechConfig object exists...
    /// </summary>
    /// <param name="language">Specifies the name of spoken language to be recognized in BCP-47 format.</param>
    /// <param name="audioInput">The audio input configuration (e.g. microhone, file, or custom audio input stream).</param>
    /// <returns>A shared pointer to IntentRecognizer</returns>
    virtual std::shared_ptr<Intent::IntentRecognizer> CreateIntentRecognizerFromConfig(const std::string& language, std::shared_ptr<Audio::AudioConfig> audioInput) = 0;

    /// <summary>
    /// Creates an IntentRecognizer for the specified spoken language, using the specified audio input configuration.
    /// NOTE: Remove this overload once SpeechConfig object exists...
    /// </summary>
    /// <param name="audioInput">The audio input configuration (e.g. microhone, file, or custom audio input stream).</param>
    /// <param name="language">Specifies the name of spoken language to be recognized in BCP-47 format.</param>
    /// <param name="format">Output format.</param>
    /// <returns>A shared pointer to IntentRecognizer</returns>
    virtual std::shared_ptr<Intent::IntentRecognizer> CreateIntentRecognizerFromConfig(const std::string& language, OutputFormat format, std::shared_ptr<Audio::AudioConfig> audioInput) = 0;

    /// <summary>
    /// Creates a TranslationRecognizer, using the specified pair of source/target languages and the default microphone as input.
    /// </summary>
    /// <param name="sourceLanguage">The spoken language of the audio input in BCP-47 format.</param>
    /// <param name="targetLanguages">A list of target languages of translation in BCP-47 format.</param>
    /// <returns>A shared pointer to TranslationRecognizer</returns>
    virtual std::shared_ptr<Translation::TranslationRecognizer> CreateTranslationRecognizerFromConfig(const std::string& sourceLanguage, const std::vector<std::string>& targetLanguages) = 0;

    /// <summary>
    /// Creates a TranslationRecognizer, using the specified pair of source/target languages and the default microphone as input.
    /// </summary>
    /// <param name="sourceLanguage">The spoken language of the audio input in BCP-47 format.</param>
    /// <param name="targetLanguages">A list of target languages of translation in BCP-47 format.</param>
    /// <param name="voice">Specifies the name of voice tag if a synthesized audio output is desired.</param>
    /// <returns>A shared pointer to TranslationRecognizer</returns>
    virtual std::shared_ptr<Translation::TranslationRecognizer> CreateTranslationRecognizerFromConfig(const std::string& sourceLanguage, const std::vector<std::string>& targetLanguages, const std::string& voice) = 0;

    /// <summary>
    /// Creates a TranslationRecognizer, using the specified pair of source/target languages and the specified audio input configuration.
    /// </summary>
    /// <param name="sourceLanguage">The spoken language of the audio input in BCP-47 format.</param>
    /// <param name="targetLanguages">A list of target languages of translation in BCP-47 format.</param>
    /// <param name="audioInput">The audio input configuration (e.g. microhone, file, or custom audio input stream).</param>
    /// <returns>A shared pointer to TranslationRecognizer</returns>
    virtual std::shared_ptr<Translation::TranslationRecognizer> CreateTranslationRecognizerFromConfig(const std::string& sourceLanguage, const std::vector<std::string>& targetLanguages, std::shared_ptr<Audio::AudioConfig> audioInput) = 0;
        
    /// <summary>
    /// Creates a TranslationRecognizer, using the specified pair of source/target languages and the specified audio input configuration.
    /// </summary>
    /// <param name="sourceLanguage">The spoken language of the audio input in BCP-47 format.</param>
    /// <param name="targetLanguages">A list of target languages of translation in BCP-47 format.</param>
    /// <param name="voice">Specifies the name of voice tag if a synthesized audio output is desired.</param>
    /// <param name="audioInput">The audio input configuration (e.g. microhone, file, or custom audio input stream).</param>
    /// <returns>A shared pointer to TranslationRecognizer</returns>
    virtual std::shared_ptr<Translation::TranslationRecognizer> CreateTranslationRecognizerFromConfig(const std::string& sourceLanguage, const std::vector<std::string>& targetLanguages, const std::string& voice, std::shared_ptr<Audio::AudioConfig> audioInput) = 0;

    /// <summary>
    /// Sets an authorization token that will be used for all recognizers created by the factory when connecting to the service.
    /// The caller needs to ensure that the token is valid and sets a new token if the current one is expired.
    /// </summary>
    /// <param name="value">An authorization token.</param>
    virtual void SetAuthorizationToken(const std::string& value) = 0;

private:

    DISABLE_COPY_AND_MOVE(ICognitiveServicesSpeechFactory);
};


/// <summary>
/// Utility class providing a static method to get an instance of the default recognizer factory.
/// </summary>
class SpeechFactory
{
public:

    /// <summary>
    /// Creates an instance of the speech factory with specified subscription key and region.
    /// </summary>
    /// <param name="subscription">The subscription key.</param>
    /// <param name="region">The region name (see the <a href="https://aka.ms/csspeech/region">region page</a>).</param>
    static std::shared_ptr<ICognitiveServicesSpeechFactory> FromSubscription(const std::string& subscription, const std::string& region)
    {
        SPXFACTORYHANDLE hfactory;
        speech_factory_from_subscription(subscription.c_str(), region.c_str(), &hfactory);

        auto ptr = new InternalCognitiveServicesSpeechFactory(hfactory);
        auto factory = std::shared_ptr<ICognitiveServicesSpeechFactory>(ptr);
        return factory;
    }

    /// <summary>
    /// Creates an instance of the speech factory with specified authorization token and region.
    /// </summary>
    /// <param name="authorizationToken">The authorization token.</param>
    /// <param name="region">The region name (see the <a href="https://aka.ms/csspeech/region">region page</a>).</param>
    static std::shared_ptr<ICognitiveServicesSpeechFactory> FromAuthorizationToken(const std::string& authToken, const std::string& region)
    {
        SPXFACTORYHANDLE hfactory;
        speech_factory_from_authorization_token(authToken.c_str(), region.c_str(), &hfactory);

        auto ptr = new InternalCognitiveServicesSpeechFactory(hfactory);
        auto factory = std::shared_ptr<ICognitiveServicesSpeechFactory>(ptr);
        return factory;
    }

    /// <summary>
    /// Creates an instance of the speech factory with specified endpoint and subscription.
    /// This method is intended only for users who use a non-standard service endpoint.
    /// Note: The query parameters specified in the endpoint URL are not changed, even if they are set by any other APIs.
    /// For example, if language is defined in uri as query parameter "language=de-DE", and also set by CreateSpeechRecognizer("en-US"),
    /// the language setting in uri takes precedence, and the effective language is "de-DE".
    /// Only the parameters that are not specified in the endpoint URL can be set by other APIs.
    /// </summary>
    /// <param name="endpoint">The service endpoint to connect to.</param>
    /// <param name="subscriptionKey">The subscription key.</param>
    static std::shared_ptr<ICognitiveServicesSpeechFactory> FromEndpoint(const std::string& endpoint, const std::string& subscription)
    {
        SPXFACTORYHANDLE hfactory;
        speech_factory_from_endpoint(endpoint.c_str(), subscription.c_str(), &hfactory);

        auto ptr = new InternalCognitiveServicesSpeechFactory(hfactory);
        auto factory = std::shared_ptr<ICognitiveServicesSpeechFactory>(ptr);
        return factory;
    }

private:

    class InternalCognitiveServicesSpeechFactory : public ICognitiveServicesSpeechFactory
    {
    public:
        InternalCognitiveServicesSpeechFactory(SPXFACTORYHANDLE hfactory) :
            ICognitiveServicesSpeechFactory(m_parameters),

            m_parameters(hfactory, HandleType::FACTORY),
            m_hfactory(hfactory)
        {
        }

        // --- ICognitiveServicesSpeechFactory overrides ---

        virtual std::shared_ptr<Speech::SpeechRecognizer> CreateSpeechRecognizer() override
        {
            return CreateSpeechRecognizerFromConfigInternal(nullptr, OutputFormat::Simple, nullptr);
        }

        virtual std::shared_ptr<Speech::SpeechRecognizer> CreateSpeechRecognizerWithFileInputHACKFOROBJECTIVEC(const std::string& fileName) override
        {
            auto audioInput = Audio::AudioConfig::FromWavFileInput(fileName);
            return CreateSpeechRecognizerFromConfig(audioInput);
        }

        virtual std::shared_ptr<Speech::SpeechRecognizer> CreateSpeechRecognizerFromConfig(const std::string& language) override
        {
            return CreateSpeechRecognizerFromConfigInternal(language.c_str(), OutputFormat::Simple, nullptr);
        }

        virtual std::shared_ptr<Speech::SpeechRecognizer> CreateSpeechRecognizerFromConfig(std::shared_ptr<Audio::AudioConfig> audioInput) override
        {
            return CreateSpeechRecognizerFromConfigInternal(nullptr, OutputFormat::Simple, audioInput);
        }

        virtual std::shared_ptr<Speech::SpeechRecognizer> CreateSpeechRecognizerFromConfig(const std::string& language, std::shared_ptr<Audio::AudioConfig> audioInput) override
        {
            return CreateSpeechRecognizerFromConfigInternal(language.c_str(), OutputFormat::Simple, audioInput);
        }

        virtual std::shared_ptr<Speech::SpeechRecognizer> CreateSpeechRecognizerFromConfig(const std::string& language, OutputFormat format) override
        {
            return CreateSpeechRecognizerFromConfigInternal(language.c_str(), format, nullptr);
        }

        virtual std::shared_ptr<Speech::SpeechRecognizer> CreateSpeechRecognizerFromConfig(const std::string& language, OutputFormat format, std::shared_ptr<Audio::AudioConfig> audioInput) override
        {
            return CreateSpeechRecognizerFromConfigInternal(language.c_str(), format, audioInput);
        }

        virtual std::shared_ptr<Intent::IntentRecognizer> CreateIntentRecognizer() override
        {
            return CreateIntentRecognizerFromConfigInternal(nullptr, OutputFormat::Simple, nullptr);
        }

        virtual std::shared_ptr<Intent::IntentRecognizer> CreateIntentRecognizerFromConfig(const std::string &language) override
        {
            return CreateIntentRecognizerFromConfigInternal(language.c_str(), OutputFormat::Simple, nullptr);
        }

        virtual std::shared_ptr<Intent::IntentRecognizer> CreateIntentRecognizerFromConfig(std::shared_ptr<Audio::AudioConfig> audioInput) override
        {
            return CreateIntentRecognizerFromConfigInternal(nullptr, OutputFormat::Simple, audioInput);
        }

        virtual std::shared_ptr<Intent::IntentRecognizer> CreateIntentRecognizerFromConfig(const std::string &language, std::shared_ptr<Audio::AudioConfig> audioInput) override
        {
            return CreateIntentRecognizerFromConfigInternal(language.c_str(), OutputFormat::Simple, audioInput);
        }

        virtual std::shared_ptr<Intent::IntentRecognizer> CreateIntentRecognizerFromConfig(const std::string& language, OutputFormat format, std::shared_ptr<Audio::AudioConfig> audioInput) override
        {
            return CreateIntentRecognizerFromConfigInternal(language.c_str(), format, audioInput);
        }

        virtual std::shared_ptr<Translation::TranslationRecognizer> CreateTranslationRecognizerFromConfig(const std::string& sourceLanguage, const std::vector<std::string>& targetLanguages) override
        {
            return CreateTranslationRecognizerFromConfig(sourceLanguage, targetLanguages, "", nullptr);
        }

        virtual std::shared_ptr<Translation::TranslationRecognizer> CreateTranslationRecognizerFromConfig(const std::string& sourceLanguage, const std::vector<std::string>& targetLanguages, const std::string& voice) override
        {
            return CreateTranslationRecognizerFromConfig(sourceLanguage, targetLanguages, voice, nullptr);
        }

        virtual std::shared_ptr<Translation::TranslationRecognizer> CreateTranslationRecognizerFromConfig(const std::string& sourceLanguage, const std::vector<std::string>& targetLanguages, std::shared_ptr<Audio::AudioConfig> audioInput) override
        {
            return CreateTranslationRecognizerFromConfig(sourceLanguage, targetLanguages, "", audioInput);
        }

        virtual std::shared_ptr<Translation::TranslationRecognizer> CreateTranslationRecognizerFromConfig(const std::string& sourceLanguage, const std::vector<std::string>& targetLanguages, const std::string& voice, std::shared_ptr<Audio::AudioConfig> audioInput) override
        {
            SPXRECOHANDLE hreco = SPXHANDLE_INVALID;

            auto targetLangBuffer = std::make_unique<const char*[]>(targetLanguages.size());
            size_t num = 0;
            for (const auto& lang : targetLanguages)
            {
                targetLangBuffer[num++] = lang.c_str();
            }

            SPX_THROW_ON_FAIL(::speech_factory_create_translation_recognizer_from_config(
                m_hfactory,
                &hreco,
                sourceLanguage.c_str(),
                targetLangBuffer.get(),
                num,
                voice.c_str(),
                HandleOrInvalidFromAudioInput(audioInput)));

            return std::make_shared<Translation::TranslationRecognizer>(hreco);
        }

        virtual void SetAuthorizationToken(const std::string& value) override
        {
            m_parameters.SetProperty(SpeechPropertyId::SpeechServiceAuthorization_Token, value);
        }

    protected:

        /// <summary>
        /// Internal helper method to get the audio config handle.
        /// </summary>
        static SPXAUDIOCONFIGHANDLE HandleOrInvalidFromAudioInput(std::shared_ptr<Audio::AudioConfig> audioInput)
        {
            return audioInput == nullptr
                ? (SPXAUDIOCONFIGHANDLE)SPXHANDLE_INVALID
                : (SPXAUDIOCONFIGHANDLE)(*audioInput.get());
        }

        std::shared_ptr<Speech::SpeechRecognizer> CreateSpeechRecognizerFromConfigInternal(const char* language, OutputFormat format, std::shared_ptr<Audio::AudioConfig> audioInput)
        {
            SPXRECOHANDLE hreco = SPXHANDLE_INVALID;
            SPX_THROW_ON_FAIL(::speech_factory_create_speech_recognizer_from_config(
                m_hfactory,
                &hreco,
                language,
                (SpeechOutputFormat)format,
                HandleOrInvalidFromAudioInput(audioInput)));

            return std::make_shared<Speech::SpeechRecognizer>(hreco);
        }

        virtual std::shared_ptr<Intent::IntentRecognizer> CreateIntentRecognizerFromConfigInternal(const char* language, OutputFormat format, std::shared_ptr<Audio::AudioConfig> audioInput)
        {
            SPXRECOHANDLE hreco = SPXHANDLE_INVALID;
            SPX_THROW_ON_FAIL(::speech_factory_create_intent_recognizer_from_config(
                m_hfactory,
                &hreco,
                language,
                (SpeechOutputFormat)format,
                HandleOrInvalidFromAudioInput(audioInput)));

            return std::make_shared<Intent::IntentRecognizer>(hreco);
        }


    private:

        DISABLE_COPY_AND_MOVE(InternalCognitiveServicesSpeechFactory);

        PropertyCollection<SPXFACTORYHANDLE> m_parameters;

        SPXFACTORYHANDLE m_hfactory;
    };

    DISABLE_COPY_AND_MOVE(SpeechFactory);
};


} } } // Microsoft::CognitiveServices::Speech
