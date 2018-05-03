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
#include <speechapi_cxx_speech_recognizer.h>
#include <speechapi_cxx_intent_recognizer.h>
#include <speechapi_cxx_translation_recognizer.h>
#include <speechapi_cxx_factory_parameter.h>
#include <speechapi_cxx_audioinputstream.h>
#include <speechapi_c_common.h>
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
    explicit ISpeechFactory(FactoryParameterCollection& parameters) : Parameters(parameters) { }

    /// <summary>
    /// Virtual destructor.
    /// </summary>
    virtual ~ISpeechFactory() { }

    /// <summary>
    /// Collection of parameters used for all recognizers created by the factory.
    /// </summary>
    FactoryParameterCollection& Parameters;

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
    /// Creates a new factory instance parametrized with an instance of FactoryParameterCollection.
    /// </summary>
    explicit ICognitiveServicesSpeechFactory(FactoryParameterCollection& parameters) : ISpeechFactory(parameters) { }

    /// <summary>
    /// Creates a SpeechRecognizer, using the default microphone as input.
    /// </summary>
    /// <returns>A shared pointer to SpeechRecognizer</returns>
    virtual std::shared_ptr<Speech::SpeechRecognizer> CreateSpeechRecognizer() = 0;

    /// <summary>
    /// Creates a SpeechRecognizer for the specified spoken language, using the default microphone as input.
    /// </summary>
    /// <param name="language">Specifies the name of spoken language to be recognized in BCP-47 format.</param>
    /// <returns>A shared pointer to SpeechRecognizer</returns>
    virtual std::shared_ptr<Speech::SpeechRecognizer> CreateSpeechRecognizer(const std::wstring& language) = 0;

    /// <summary>
    /// Creates a SpeechRecognizer, using the specified audio file as input.
    /// </summary>
    /// <param name="fileName">Specifies the input audio file. Currently, only WAV / PCM with 16-bit samples, 16 KHz sample rate, and a single channel (Mono) is supported.</param>
    /// <returns>A shared pointer to SpeechRecognizer</returns>
    virtual std::shared_ptr<Speech::SpeechRecognizer> CreateSpeechRecognizerWithFileInput(const std::wstring& fileName) = 0;

    /// <summary>
    /// Creates a SpeechRecognizer for the specified spoken language, using the specified audio file as input.
    /// </summary>
    /// <param name="fileName">Specifies the input audio file. Currently, only WAV / PCM with 16-bit samples, 16 KHz sample rate, and a single channel (Mono) is supported.</param>
    /// <param name="language">Specifies the name of spoken language to be recognized in BCP-47 format.</param>
    /// <returns>A shared pointer to SpeechRecognizer</returns>
    virtual std::shared_ptr<Speech::SpeechRecognizer> CreateSpeechRecognizerWithFileInput(const std::wstring& fileName, const std::wstring& language) = 0;

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
    virtual std::shared_ptr<Intent::IntentRecognizer> CreateIntentRecognizer(const std::wstring& language) = 0;

    /// <summary>
    /// Creates an IntentRecognizer, using the specified audio file as input.
    /// </summary>
    /// <param name="fileName">Specifies the input audio file.</param>
    /// <returns>A shared pointer to IntentRecognizer</returns>
    virtual std::shared_ptr<Intent::IntentRecognizer> CreateIntentRecognizerWithFileInput(const std::wstring& fileName) = 0;

    /// <summary>
    /// Creates an IntentRecognizer for the specified spoken language, using the specified audio file as input.
    /// </summary>
    /// <param name="fileName">Specifies the input audio file.</param>
    /// <param name="language">Specifies the name of spoken language to be recognized in BCP-47 format.</param>
    /// <returns>A shared pointer to IntentRecognizer</returns>
    virtual std::shared_ptr<Intent::IntentRecognizer> CreateIntentRecognizerWithFileInput(const std::wstring& fileName, const std::wstring& language) = 0;

    /// <summary>
    /// Creates a TranslationRecognizer, using the specified pair of source/target languages and the default microphone as input.
    /// </summary>
    /// <param name="sourceLanguage">The spoken language of the audio input in BCP-47 format.</param>
    /// <param name="targetLanguages">A list of target languages of translation in BCP-47 format.</param>
    /// <param name="voice">Optional. Specifies the name of voice tag if a synthesized audio output is desired.</param>
    /// <returns>A shared pointer to TranslationRecognizer</returns>
    virtual std::shared_ptr<Translation::TranslationRecognizer> CreateTranslationRecognizer(const std::wstring& sourceLanguage, const std::vector<std::wstring>& targetLanguages, const std::wstring& voice = L"") = 0;

    /// <summary>
    /// Creates a TranslationRecognizer, using the specified audio stream as input and the pair of source/target languages.
    /// </summary>
    /// <param name="fileName">Specifies the input audio file.</param>
    /// <param name="sourceLanguage">The spoken language of the audio input in BCP-47 format.</param>
    /// <param name="targetLanguages">A list of target languages of translation in BCP-47 format.</param>
    /// <param name="voice">Optional. Specifies the name of voice tag if a synthesized audio output is desired.</param>
    /// <returns>A shared pointer to TranslationRecognizer</returns>
    virtual std::shared_ptr<Translation::TranslationRecognizer> CreateTranslationRecognizerWithFileInput(const std::wstring& fileName, const std::wstring& sourceLanguage, const std::vector<std::wstring>& targetLanguages, const std::wstring& voice = L"") = 0;

    /// <summary>
    /// Sets an authorization token that will be used for all recognizers created by the factory when connecting to the service.
    /// The caller needs to ensure that the token is valid and sets a new token if the current one is expired.
    /// </summary>
    /// <param name="value">An authorization token.</param>
    virtual void SetAuthorizationToken(const std::wstring& value) = 0;

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
    /// <param name="region">The region name.</param>
    static std::shared_ptr<ICognitiveServicesSpeechFactory> FromSubscription(const std::wstring& subscription, const std::wstring& region)
    {
        SPXFACTORYHANDLE hfactory;
        SpeechFactory_FromSubscription(subscription.c_str(), region.c_str(), &hfactory);

        auto ptr = new InternalCognitiveServicesSpeechFactory(hfactory);
        auto factory = std::shared_ptr<ICognitiveServicesSpeechFactory>(ptr);
        return factory;
    }

    /// <summary>
    /// Creates an instance of the speech factory with specified authorization token and region.
    /// </summary>
    /// <param name="authorizationToken">The authorization token.</param>
    /// <param name="region">The region name.</param>
    static std::shared_ptr<ICognitiveServicesSpeechFactory> FromAuthorizationToken(const std::wstring& authToken, const std::wstring& region)
    {
        SPXFACTORYHANDLE hfactory;
        SpeechFactory_FromAuthorizationToken(authToken.c_str(), region.c_str(), &hfactory);

        auto ptr = new InternalCognitiveServicesSpeechFactory(hfactory);
        auto factory = std::shared_ptr<ICognitiveServicesSpeechFactory>(ptr);
        return factory;
    }

    /// <summary>
    /// Creates an instance of the speech factory with specified endpoint and subscription.
    /// </summary>
    /// <param name="endpoint">The service endpoint to connect to.</param>
    /// <param name="subscriptionKey">The subscription key.</param>
    static std::shared_ptr<ICognitiveServicesSpeechFactory> FromEndpoint(const std::wstring& endpoint, const std::wstring& subscription)
    {
        SPXFACTORYHANDLE hfactory;
        SpeechFactory_FromEndpoint(endpoint.c_str(), subscription.c_str(), &hfactory);

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
            m_parameters(hfactory), 
            m_hfactory(hfactory)
        {
        }

        // --- ICognitiveServicesSpeechFactory overrides ---

        virtual std::shared_ptr<Speech::SpeechRecognizer> CreateSpeechRecognizer() override
        {
            SPXRECOHANDLE hreco = SPXHANDLE_INVALID;
            SPX_THROW_ON_FAIL(::SpeechFactory_CreateSpeechRecognizer_With_Defaults(m_hfactory, &hreco));
            return std::make_shared<Speech::SpeechRecognizer>(hreco);
        }

        virtual std::shared_ptr<Speech::SpeechRecognizer> CreateSpeechRecognizer(const std::wstring& language) override
        {
            SPXRECOHANDLE hreco = SPXHANDLE_INVALID;
            SPX_THROW_ON_FAIL(::SpeechFactory_CreateSpeechRecognizer_With_Language(m_hfactory, &hreco, language.c_str()));
            return std::make_shared<Speech::SpeechRecognizer>(hreco);
        }

        virtual std::shared_ptr<Speech::SpeechRecognizer> CreateSpeechRecognizerWithFileInput(const std::wstring& fileName) override
        {
            SPXRECOHANDLE hreco = SPXHANDLE_INVALID;
            SPX_THROW_ON_FAIL(::SpeechFactory_CreateSpeechRecognizer_With_FileInput(m_hfactory, &hreco, fileName.c_str()));
            return std::make_shared<Speech::SpeechRecognizer>(hreco);
        }

        virtual std::shared_ptr<Speech::SpeechRecognizer> CreateSpeechRecognizerWithFileInput(const std::wstring& fileName, const std::wstring& language) override
        {
            SPXRECOHANDLE hreco = SPXHANDLE_INVALID;
            SPX_THROW_ON_FAIL(::SpeechFactory_CreateSpeechRecognizer_With_FileInputAndLanguage(m_hfactory, &hreco, language.c_str(), fileName.c_str()));
            return std::make_shared<Speech::SpeechRecognizer>(hreco);
        }

        typedef struct _SpeechApi_AudioInputStreamAdapter
        {
            // must be first;
            ::SpeechApi_AudioInputStream functionParams;
            AudioInputStream * m_audioInputStream;
        } SpeechApi_AudioInputStreamAdapter;

        static void InitSpeechApi_AudioInputStreamAdapterFromAudioInputStream(SpeechApi_AudioInputStreamAdapter* adapter, AudioInputStream* audioInputStream)
        {
            adapter->m_audioInputStream = audioInputStream;
            adapter->functionParams.Close = Wrap_Close;
            adapter->functionParams.GetFormat = Wrap_GetFormat;
            adapter->functionParams.Read = Wrap_Read;
        }

        static unsigned short Wrap_GetFormat(::SpeechApi_AudioInputStream* context, ::AudioInputStreamFormat* pformat, unsigned short cbFormat)
        {
            unsigned short retValue = 0;

            if (pformat)
            {
                AudioInputStreamFormat pFormat2;
                retValue = ((SpeechApi_AudioInputStreamAdapter*)context)->m_audioInputStream->GetFormat(&pFormat2, cbFormat);

                pformat->cbSize = pFormat2.cbSize;
                pformat->nAvgBytesPerSec = pFormat2.nAvgBytesPerSec;
                pformat->nBlockAlign = pFormat2.nBlockAlign;
                pformat->nChannels = pFormat2.nChannels;
                pformat->nSamplesPerSec = pFormat2.nSamplesPerSec;
                pformat->wBitsPerSample = pFormat2.wBitsPerSample;
                pformat->wFormatTag = pFormat2.wFormatTag;
            }
            else
            {
                retValue = ((SpeechApi_AudioInputStreamAdapter*)context)->m_audioInputStream->GetFormat(nullptr, cbFormat);
            }

            return retValue;
        }

        static unsigned int Wrap_Read(::SpeechApi_AudioInputStream* context, unsigned char* pbuffer, unsigned int cbBuffer)
        {
            return ((SpeechApi_AudioInputStreamAdapter*)context)->m_audioInputStream->Read((char*)pbuffer, cbBuffer);
        }

        static void Wrap_Close(::SpeechApi_AudioInputStream* context)
        {
            ((SpeechApi_AudioInputStreamAdapter*)context)->m_audioInputStream->Close();
        }

        virtual std::shared_ptr<Intent::IntentRecognizer> CreateIntentRecognizer() override
        {
            SPXRECOHANDLE hreco = SPXHANDLE_INVALID;
            SPX_THROW_ON_FAIL(::SpeechFactory_CreateIntentRecognizer_With_Defaults(m_hfactory, &hreco));
            return std::make_shared<Intent::IntentRecognizer>(hreco);
        }

        virtual std::shared_ptr<Intent::IntentRecognizer> CreateIntentRecognizer(const std::wstring& language) override
        {
            SPXRECOHANDLE hreco = SPXHANDLE_INVALID;
            SPX_THROW_ON_FAIL(::SpeechFactory_CreateIntentRecognizer_With_Language(m_hfactory, &hreco, language.c_str()));
            return std::make_shared<Intent::IntentRecognizer>(hreco);
        }

        virtual std::shared_ptr<Intent::IntentRecognizer> CreateIntentRecognizerWithFileInput(const std::wstring& fileName) override
        {
            SPXRECOHANDLE hreco = SPXHANDLE_INVALID;
            SPX_THROW_ON_FAIL(::SpeechFactory_CreateIntentRecognizer_With_FileInput(m_hfactory, &hreco, fileName.c_str()));
            return std::make_shared<Intent::IntentRecognizer>(hreco);
        }

        virtual std::shared_ptr<Intent::IntentRecognizer> CreateIntentRecognizerWithFileInput(const std::wstring& fileName, const std::wstring& language) override
        {
            SPXRECOHANDLE hreco = SPXHANDLE_INVALID;
            SPX_THROW_ON_FAIL(::SpeechFactory_CreateIntentRecognizer_With_FileInputAndLanguage(m_hfactory, &hreco, language.c_str(), fileName.c_str()));
            return std::make_shared<Intent::IntentRecognizer>(hreco);
        }

        virtual std::shared_ptr<Translation::TranslationRecognizer> CreateTranslationRecognizer(const std::wstring& sourceLanguage, const std::vector<std::wstring>& targetLanguages, const std::wstring& voice = L"") override
        {
            SPXRECOHANDLE hreco = SPXHANDLE_INVALID;
            auto targetLangBuffer = std::make_unique<const wchar_t*[]>(targetLanguages.size());
            size_t num = 0;
            for (const auto& lang : targetLanguages)
            {
                targetLangBuffer[num++] = lang.c_str();
            }

            SPX_THROW_ON_FAIL(::SpeechFactory_CreateTranslationRecognizer(m_hfactory, &hreco, sourceLanguage.c_str(), targetLangBuffer.get(), num, voice.c_str()));
            return std::make_shared<Translation::TranslationRecognizer>(hreco);
        }


        virtual std::shared_ptr<Translation::TranslationRecognizer> CreateTranslationRecognizerWithFileInput(const std::wstring& fileName, const std::wstring& sourceLanguage, const std::vector<std::wstring>& targetLanguages, const std::wstring& voice = L"") override
        {
            SPXRECOHANDLE hreco = SPXHANDLE_INVALID;

            auto targetLangBuffer = std::make_unique<const wchar_t*[]>(targetLanguages.size());
            size_t num = 0;
            for (const auto& lang : targetLanguages)
            {
                targetLangBuffer[num++] = lang.c_str();
            }
            SPX_THROW_ON_FAIL(::SpeechFactory_CreateTranslationRecognizer_With_FileInput(m_hfactory, &hreco, sourceLanguage.c_str(), targetLangBuffer.get(), num, voice.c_str(), fileName.c_str()));
            return std::make_shared<Translation::TranslationRecognizer>(hreco);
        }

        virtual void SetAuthorizationToken(const std::wstring& value) override
        {
            m_parameters[FactoryParameter::AuthorizationToken] = value;
        }

    private:

        DISABLE_COPY_AND_MOVE(InternalCognitiveServicesSpeechFactory);

        FactoryParameterCollection m_parameters;

        SPXFACTORYHANDLE m_hfactory;
    };

    DISABLE_COPY_AND_MOVE(SpeechFactory);
};


} } } // Microsoft::CognitiveServices::Speech
