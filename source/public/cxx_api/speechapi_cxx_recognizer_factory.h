//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_cxx_recognizer_factory.h: Public API declarations for RecognizerFactory C++ static class methods
//

#pragma once
#include <memory>
#include <vector>
#include <speechapi_cxx_common.h>
#include <speechapi_cxx_speech_recognizer.h>
#include <speechapi_cxx_intent_recognizer.h>
#include <speechapi_cxx_translation_recognizer.h>
#include <speechapi_cxx_recognizer_factory_parameter.h>
#include <speechapi_cxx_audioinputstream.h>
#include <speechapi_c_common.h>
#include <speechapi_c_recognizer_factory.h>


namespace CARBON_NAMESPACE_ROOT {
namespace Recognition {

/// <summary>
/// IRecognizerFactory interface, defines a number of factory methods for creating various recognizers.
/// </summary>
class IRecognizerFactory
{
public:

    /// <summary>
    /// Creates a new factory instance parametrized with an instance of RecognizerFactoryParameterCollection.
    /// </summary>
    IRecognizerFactory(RecognizerFactoryParameterCollection& parameters) : Parameters(parameters) { }

    /// <summary>
    /// Virtual destructor.
    /// </summary>
    virtual ~IRecognizerFactory() { }

    /// <summary>
    /// Creates a SpeechRecognizer, using the default microphone as input.
    /// </summary>
    /// <returns>A shared pointer to SpeechRecognizer</returns>
    virtual std::shared_ptr<Speech::SpeechRecognizer> CreateSpeechRecognizer() = 0;

    /// <summary>
    /// Creates a SpeechRecognizer, using the specified audio file as input.
    /// </summary>
    /// <param name="fileName">Specifies the input audio file.</param>
    /// <returns>A shared pointer to SpeechRecognizer</returns>
    virtual std::shared_ptr<Speech::SpeechRecognizer> CreateSpeechRecognizerWithFileInput(const std::wstring& fileName) = 0;

    /// <summary>
    /// Creates an IntentRecognizer, using the default microphone as input.
    /// </summary>
    /// <returns>A shared pointer to IntentRecognizer</returns>
    virtual std::shared_ptr<Intent::IntentRecognizer> CreateIntentRecognizer() = 0;

    /// <summary>
    /// Creates an IntentRecognizer, using the specified audio file as input.
    /// </summary>
    /// <param name="fileName">Specifies the input audio file.</param>
    /// <returns>A shared pointer to IntentRecognizer</returns>
    virtual std::shared_ptr<Intent::IntentRecognizer> CreateIntentRecognizerWithFileInput(const std::wstring& fileName) = 0;

    /// <summary>
    /// Creates a TranslationRecognizer, using the specified pair of source/target languages and the default microphone as input.
    /// </summary>
    /// <param name="sourceLanguage">Source language in BCP-47 format (used in the input audio).</param>
    /// <param name="targetLanguages">Vector of target translation languages in BCP-47 format.</param>
    /// <returns>A shared pointer to TranslationRecognizer</returns>
    virtual std::shared_ptr<Translation::TranslationRecognizer> CreateTranslationRecognizer(const std::wstring& sourceLanguage, const std::vector<std::wstring>& targetLanguages) = 0;

    /// <summary>
    /// Creates a TranslationRecognizer, using the specified audio file as input and the pair of source/target languages.
    /// </summary>
    /// <param name="fileName">Specifies the input audio file.</param>
    /// <param name="sourceLanguage">Source language in BCP-47 format (used in the input audio).</param>
    /// <param name="targetLanguages">Vector of target translation languages in BCP-47 format.</param>
    /// <returns>A shared pointer to TranslationRecognizer</returns>
    virtual std::shared_ptr<Translation::TranslationRecognizer> CreateTranslationRecognizerWithFileInput(const std::wstring& fileName, const std::wstring& sourceLanguage, const std::vector<std::wstring>& targetLanguages) = 0;

    /// <summary>
    /// Sets a subscription key that will be used for all recognizers created by the factory.
    /// </summary>
    /// <param name="value">A subscription key.</param>
    virtual void SetSubscriptionKey(const std::wstring& value) = 0;

    /// <summary>
    /// Sets a speech service endpoint that will be used for all recognizers created by the factory.
    /// </summary>
    /// <param name="value">A speech service endpoint (url).</param>
    virtual void SetSpeechEndpoint(const std::wstring& value) = 0;

    /// <summary>
    /// Collection of parameters used for all recognizers created by the factory.
    /// </summary>
    RecognizerFactoryParameterCollection& Parameters;

private:

    DISABLE_COPY_AND_MOVE(IRecognizerFactory);
};

/// <summary>
/// IDefaultRecognizerFactory interface.
/// </summary>
class IDefaultRecognizerFactory : public IRecognizerFactory
{
public:

    /// <summary>
    /// Creates a new factory instance parametrized with an instance of RecognizerFactoryParameterCollection.
    /// </summary>
    IDefaultRecognizerFactory(RecognizerFactoryParameterCollection& parameters) : IRecognizerFactory(parameters) { }

    virtual std::shared_ptr<Speech::SpeechRecognizer> CreateSpeechRecognizer() = 0;
    virtual std::shared_ptr<Speech::SpeechRecognizer> CreateSpeechRecognizer(const std::wstring& language) = 0;
    virtual std::shared_ptr<Speech::SpeechRecognizer> CreateSpeechRecognizerWithFileInput(const std::wstring& fileName) = 0;
    virtual std::shared_ptr<Speech::SpeechRecognizer> CreateSpeechRecognizerWithFileInput(const std::wstring& fileName, const std::wstring& language) = 0;
    virtual std::shared_ptr<Speech::SpeechRecognizer> CreateSpeechRecognizerWithStream(AudioInputStream* audioInputStream) = 0;

    virtual std::shared_ptr<Intent::IntentRecognizer> CreateIntentRecognizer() = 0;
    virtual std::shared_ptr<Intent::IntentRecognizer> CreateIntentRecognizer(const std::wstring& language) = 0;
    virtual std::shared_ptr<Intent::IntentRecognizer> CreateIntentRecognizerWithFileInput(const std::wstring& fileName) = 0;
    virtual std::shared_ptr<Intent::IntentRecognizer> CreateIntentRecognizerWithFileInput(const std::wstring& fileName, const std::wstring& language) = 0;

    virtual std::shared_ptr<Translation::TranslationRecognizer> CreateTranslationRecognizer(const std::wstring& sourceLanguage, const std::vector<std::wstring>& targetLanguages) = 0;
    virtual std::shared_ptr<Translation::TranslationRecognizer> CreateTranslationRecognizerWithFileInput(const std::wstring& fileName, const std::wstring& sourceLanguage, const std::vector<std::wstring>& targetLanguages) = 0;
    // Todo: AudioInputStream* or const AudioInputStream&??
    virtual std::shared_ptr<Translation::TranslationRecognizer> CreateTranslationRecognizerWithStream(AudioInputStream* audioInputStream, const std::wstring& sourceLanguage, const std::vector<std::wstring>& targetLanguages) = 0;

    virtual void SetSubscriptionKey(const std::wstring& value) = 0;
    virtual void SetSpeechEndpoint(const std::wstring& value) = 0;


private:

    DISABLE_COPY_AND_MOVE(IDefaultRecognizerFactory);
};


/// <summary>
/// Utility class providing a static method to get an instance of the default recognizer factory.
/// </summary>
class RecognizerFactory
{
public:

    /// <summary>
    /// Returns a shared pointer to IDefaultRecognizerFactory.
    /// </summary>
    static std::shared_ptr<IDefaultRecognizerFactory> GetDefault()
    {
        static std::mutex mutex;
        static std::shared_ptr<IDefaultRecognizerFactory> factory;

        if (factory == nullptr)
        {
            std::unique_lock<std::mutex> lock(mutex);
            if (factory == nullptr)
            {
                auto ptr = new InternalDefaultRecognizerFactory();
                factory = std::shared_ptr<IDefaultRecognizerFactory>(ptr);
            }
        }

        return factory;
    }


private:

    class InternalDefaultRecognizerFactory : public IDefaultRecognizerFactory
    {
    public:

        InternalDefaultRecognizerFactory() : IDefaultRecognizerFactory(m_parameters) { }


        // --- IRecognizerFactory overrides ---

        virtual std::shared_ptr<Speech::SpeechRecognizer> CreateSpeechRecognizer() override
        {
            SPXRECOHANDLE hreco = SPXHANDLE_INVALID;
            SPX_THROW_ON_FAIL(::RecognizerFactory_CreateSpeechRecognizer_With_Defaults(SPXHANDLE_DEFAULT, &hreco));
            return std::make_shared<Speech::SpeechRecognizer>(hreco);
        }

        virtual std::shared_ptr<Speech::SpeechRecognizer> CreateSpeechRecognizer(const std::wstring& language) override
        {
            SPXRECOHANDLE hreco = SPXHANDLE_INVALID;
            SPX_THROW_ON_FAIL(::RecognizerFactory_CreateSpeechRecognizer_With_Language(SPXHANDLE_DEFAULT, &hreco, language.c_str()));
            return std::make_shared<Speech::SpeechRecognizer>(hreco);
        }

        virtual std::shared_ptr<Intent::IntentRecognizer> CreateIntentRecognizer() override
        {
            SPXRECOHANDLE hreco = SPXHANDLE_INVALID;
            SPX_THROW_ON_FAIL(::RecognizerFactory_CreateIntentRecognizer_With_Defaults(SPXHANDLE_DEFAULT, &hreco));
            return std::make_shared<Intent::IntentRecognizer>(hreco);
        }


        // --- IDefaultRecognizerFactory overrides ---

        virtual std::shared_ptr<Speech::SpeechRecognizer> CreateSpeechRecognizerWithFileInput(const std::wstring& fileName)
        {
            SPXRECOHANDLE hreco = SPXHANDLE_INVALID;
            SPX_THROW_ON_FAIL(::RecognizerFactory_CreateSpeechRecognizer_With_FileInput(SPXHANDLE_DEFAULT, &hreco, fileName.c_str()));
            return std::make_shared<Speech::SpeechRecognizer>(hreco);
        }

        virtual std::shared_ptr<Speech::SpeechRecognizer> CreateSpeechRecognizerWithFileInput(const std::wstring& fileName, const std::wstring& language)
        {
            SPXRECOHANDLE hreco = SPXHANDLE_INVALID;
            SPX_THROW_ON_FAIL(::RecognizerFactory_CreateSpeechRecognizer_With_FileInputAndLanguage(SPXHANDLE_DEFAULT, &hreco, language.c_str(), fileName.c_str()));
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

        virtual std::shared_ptr<Speech::SpeechRecognizer> CreateSpeechRecognizerWithStream(AudioInputStream* audioInputStream)
        {
            SPXRECOHANDLE hreco = SPXHANDLE_INVALID;
            SpeechApi_AudioInputStreamAdapter *audioInputStream2 = new SpeechApi_AudioInputStreamAdapter();
            InitSpeechApi_AudioInputStreamAdapterFromAudioInputStream(audioInputStream2, audioInputStream);

            SPX_THROW_ON_FAIL(::RecognizerFactory_CreateSpeechRecognizer_With_Stream(SPXHANDLE_DEFAULT, &hreco, (::SpeechApi_AudioInputStream*)audioInputStream2));
            return std::make_shared<Speech::SpeechRecognizer>(hreco);
        };

        virtual std::shared_ptr<Intent::IntentRecognizer> CreateIntentRecognizer(const std::wstring& language)
        {
            UNUSED(language);
            throw SPXERR_NOT_IMPL;
        }

        virtual std::shared_ptr<Intent::IntentRecognizer> CreateIntentRecognizerWithFileInput(const std::wstring& fileName)
        {
            SPXRECOHANDLE hreco = SPXHANDLE_INVALID;
            SPX_THROW_ON_FAIL(::RecognizerFactory_CreateIntentRecognizer_With_FileInput(SPXHANDLE_DEFAULT, &hreco, fileName.c_str()));
            return std::make_shared<Intent::IntentRecognizer>(hreco);
        }

        virtual std::shared_ptr<Intent::IntentRecognizer> CreateIntentRecognizerWithFileInput(const std::wstring& fileName, const std::wstring& language)
        {
            UNUSED(language);
            UNUSED(fileName);
            throw SPXERR_NOT_IMPL;
        }

        virtual std::shared_ptr<Translation::TranslationRecognizer> CreateTranslationRecognizer(const std::wstring& sourceLanguage, const std::vector<std::wstring>& targetLanguages)
        {
            SPXRECOHANDLE hreco = SPXHANDLE_INVALID;
            // Hack use only one target language.
            auto lang = targetLanguages.at(0);
            SPX_THROW_ON_FAIL(::RecognizerFactory_CreateTranslationRecognizer(SPXHANDLE_DEFAULT, &hreco, sourceLanguage.c_str(), lang.c_str()));
            return std::make_shared<Translation::TranslationRecognizer>(hreco);
        }

        virtual std::shared_ptr<Translation::TranslationRecognizer> CreateTranslationRecognizerWithFileInput(const std::wstring& fileName, const std::wstring& sourceLanguage, const std::vector<std::wstring>& targetLanguages)
        {
            SPXRECOHANDLE hreco = SPXHANDLE_INVALID;
            // Hack use only one target language.
            auto lang = targetLanguages.at(0);
            SPX_THROW_ON_FAIL(::RecognizerFactory_CreateTranslationRecognizer_With_FileInput(SPXHANDLE_DEFAULT, &hreco, sourceLanguage.c_str(), lang.c_str(), fileName.c_str()));
            return std::make_shared<Translation::TranslationRecognizer>(hreco);
        }

        virtual std::shared_ptr<Translation::TranslationRecognizer> CreateTranslationRecognizerWithStream(AudioInputStream* audioInputStream, const std::wstring& sourceLanguage, const std::vector<std::wstring>& targetLanguages)
        {
            SPXRECOHANDLE hreco = SPXHANDLE_INVALID;
            SpeechApi_AudioInputStreamAdapter *audioInputStreamAdapter = new SpeechApi_AudioInputStreamAdapter();
            InitSpeechApi_AudioInputStreamAdapterFromAudioInputStream(audioInputStreamAdapter, audioInputStream);

            auto lang = targetLanguages.at(0);
            // Hack use only one target language.
            SPX_THROW_ON_FAIL(::RecognizerFactory_CreateTranslationRecognizer_With_Stream(SPXHANDLE_DEFAULT, &hreco, sourceLanguage.c_str(), lang.c_str(), (::SpeechApi_AudioInputStream*)audioInputStreamAdapter));
            return std::make_shared<Translation::TranslationRecognizer>(hreco);
        };

        virtual void SetSubscriptionKey(const std::wstring& value)
        {
            Parameters[FactoryParameter::SpeechSubscriptionKey] = value.c_str();
        }

        virtual void SetSpeechEndpoint(const std::wstring& value)
        {
            Parameters[FactoryParameter::SpeechEndpoint] = value.c_str();
        }

    private:

        DISABLE_COPY_AND_MOVE(InternalDefaultRecognizerFactory);

        RecognizerFactoryParameterCollection m_parameters;
    };

    DISABLE_COPY_AND_MOVE(RecognizerFactory);
};

/// <summary>
/// Utility class providing a number of static method for creating recognizers.
/// </summary>
class DefaultRecognizerFactory
{
public:

    // --- 'SpeechRecognizer' RecognizerFactory methods ---

    /// <summary>
    /// Creates a SpeechRecognizer, using the default microphone as input.
    /// </summary>
    /// <returns>A shared pointer to SpeechRecognizer</returns>
    static std::shared_ptr<Speech::SpeechRecognizer> CreateSpeechRecognizer()
    {
        auto factory = RecognizerFactory::GetDefault();
        return factory->CreateSpeechRecognizer();
    }

    /// <summary>
    /// Creates a SpeechRecognizer, using the specified source audio language and the default microphone as input.
    /// </summary>
    /// <param name="language">Source audio language in BCP-47 format.</param>
    /// <returns>A shared pointer to SpeechRecognizer</returns>
    static std::shared_ptr<Speech::SpeechRecognizer> CreateSpeechRecognizer(const std::wstring& language)
    {
        auto factory = RecognizerFactory::GetDefault();
        return factory->CreateSpeechRecognizer(language);
    }

    /// <summary>
    /// Creates a SpeechRecognizer, using the specified audio file as input.
    /// </summary>
    /// <param name="fileName">Specifies the input audio file.</param>
    /// <returns>A shared pointer to SpeechRecognizer</returns>
    static std::shared_ptr<Speech::SpeechRecognizer> CreateSpeechRecognizerWithFileInput(const std::wstring& fileName)
    {
        auto factory = RecognizerFactory::GetDefault();
        return factory->CreateSpeechRecognizerWithFileInput(fileName);
    }

    /// <summary>
    /// Creates a SpeechRecognizer, using the specified audio stream as input.
    /// </summary>
    /// <param name="audioInputStream">A pointer to the input audio stream.</param>
    /// <returns>A shared pointer to SpeechRecognizer</returns>
    static std::shared_ptr<Speech::SpeechRecognizer> CreateSpeechRecognizerWithStream(AudioInputStream* audioInputStream)
    {
        auto factory = RecognizerFactory::GetDefault();
        return factory->CreateSpeechRecognizerWithStream(audioInputStream);
    };

    /// <summary>
    /// Creates a SpeechRecognizer, using the specified audio file as input and source audio language.
    /// </summary>
    /// <param name="fileName">Specifies the input audio file.</param>
    /// <param name="language">Source audio language in BCP-47 format.</param>
    /// <returns>A shared pointer to SpeechRecognizer</returns>
    static std::shared_ptr<Speech::SpeechRecognizer> CreateSpeechRecognizerWithFileInput(const std::wstring& fileName, const std::wstring& language)
    {
        auto factory = RecognizerFactory::GetDefault();
        return factory->CreateSpeechRecognizerWithFileInput(fileName, language);
    }


    // --- 'IntentRecognizer' RecognizerFactory methods ---

    /// <summary>
    /// Creates an IntentRecognizer, using the default microphone as input.
    /// </summary>
    /// <returns>A shared pointer to IntentRecognizer</returns>
    static std::shared_ptr<Intent::IntentRecognizer> CreateIntentRecognizer()
    {
        auto factory = RecognizerFactory::GetDefault();
        return factory->CreateIntentRecognizer();
    }

    /// <summary>
    /// Creates an IntentRecognizer, using the specified source audio language and the default microphone as input.
    /// </summary>
    /// <param name="language">Source audio language in BCP-47 format.</param>
    /// <returns>A shared pointer to IntentRecognizer</returns>
    static std::shared_ptr<Intent::IntentRecognizer> CreateIntentRecognizer(const std::wstring& language)
    {
        auto factory = RecognizerFactory::GetDefault();
        return factory->CreateIntentRecognizer(language);
    }

    /// <summary>
    /// Creates an IntentRecognizer, using the specified audio file as input.
    /// </summary>
    /// <param name="fileName">Specifies the input audio file.</param>
    /// <returns>A shared pointer to IntentRecognizer</returns>
    static std::shared_ptr<Intent::IntentRecognizer> CreateIntentRecognizerWithFileInput(const std::wstring& fileName)
    {
        auto factory = RecognizerFactory::GetDefault();
        return factory->CreateIntentRecognizerWithFileInput(fileName);
    }

    /// <summary>
    /// Creates an IntentRecognizer, using the specified audio file as input and source audio language.
    /// </summary>
    /// <param name="fileName">Specifies the input audio file.</param>
    /// <param name="language">Source audio language in BCP-47 format.</param>
    /// <returns>A shared pointer to IntentRecognizer</returns>
    static std::shared_ptr<Intent::IntentRecognizer> CreateIntentRecognizerWithFileInput(const std::wstring& fileName, const std::wstring& language)
    {
        auto factory = RecognizerFactory::GetDefault();
        return factory->CreateIntentRecognizerWithFileInput(fileName, language);
    };


    // --- 'Translation' RecognizerFactory methods ---

    /// <summary>
    /// Creates a TranslationRecognizer, using the specified pair of source/target languages and the default microphone as input.
    /// </summary>
    /// <param name="sourceLanguage">Source language in BCP-47 format (used in the input audio).</param>
    /// <param name="targetLanguages">Vector of target translation languages in BCP-47 format.</param>
    /// <returns>A shared pointer to TranslationRecognizer</returns>
    static std::shared_ptr<Translation::TranslationRecognizer> CreateTranslationRecognizer(const std::wstring& sourceLanguage, const std::vector<std::wstring>& targetLanguages)
    {
        auto factory = RecognizerFactory::GetDefault();
        return factory->CreateTranslationRecognizer(sourceLanguage, targetLanguages);
    }

    /// <summary>
    /// Creates a TranslationRecognizer, using the specified audio file as input and the pair of source/target languages.
    /// </summary>
    /// <param name="fileName">Specifies the input audio file.</param>
    /// <param name="sourceLanguage">Source language in BCP-47 format (used in the input audio).</param>
    /// <param name="targetLanguages">Vector of target translation languages in BCP-47 format.</param>
    /// <returns>A shared pointer to TranslationRecognizer</returns>
    static std::shared_ptr<Translation::TranslationRecognizer> CreateTranslationRecognizerWithFileInput(const std::wstring& fileName, const std::wstring& sourceLanguage, const std::vector<std::wstring>& targetLanguages)
    {
        auto factory = RecognizerFactory::GetDefault();
        return factory->CreateTranslationRecognizerWithFileInput(fileName, sourceLanguage, targetLanguages);
    }

    /// <summary>
    /// Creates a TranslationRecognizer, using the specified audio stream as input and the pair of source/target languages.
    /// </summary>
    /// <param name="audioInputStream">A pointer to the input audio stream.</param>
    /// <param name="sourceLanguage">Source language in BCP-47 format (used in the input audio).</param>
    /// <param name="targetLanguages">Vector of target translation languages in BCP-47 format.</param>
    /// <returns>A shared pointer to TranslationRecognizer</returns>
    static std::shared_ptr<Translation::TranslationRecognizer> CreateTranslationRecognizerWithStream(AudioInputStream* audioInputStream, const std::wstring& sourceLanguage, const std::vector<std::wstring>& targetLanguages)
    {
        auto factory = RecognizerFactory::GetDefault();
        return factory->CreateTranslationRecognizerWithStream(audioInputStream, sourceLanguage, targetLanguages);
    }

    // --- Static accessors to RecognizerFactoryParameters ---

    /// <summary>
    /// Utility class providing a number of static accessors for global default factory parameter values.
    /// </summary>
    class Parameters
    {
    public:

        /// <summary>
        /// Returns true if default factory contains a global parameter of type string with the corresponding name.
        /// </summary>
        static bool ContainsString(const wchar_t* name) { return RecognizerFactoryParameterValue(SPXHANDLE_DEFAULT, name).IsString(); }

        /// <summary>
        /// Stores the specified string value as a default factory global parameter mapped to the specified name.
        /// </summary>
        static void SetString(const wchar_t* name, const wchar_t* value) { RecognizerFactoryParameterValue(SPXHANDLE_DEFAULT, name).SetString(value); }

        /// <summary>
        /// Returns string value of the default factory global parameter mapped to the specified name.
        /// </summary>
        /// <param name="defaultValue">Default value to return if the mapping does not exist. </param>
        static std::wstring GetString(const wchar_t* name, const wchar_t* defaultValue = L"") { return RecognizerFactoryParameterValue(SPXHANDLE_DEFAULT, name).GetString(defaultValue); }

        /// <summary>
        /// Returns true if default factory contains a global parameter of a number type with the corresponding name.
        /// </summary>
        static bool ContainsNumber(const wchar_t* name) { return RecognizerFactoryParameterValue(SPXHANDLE_DEFAULT, name).IsNumber(); }

        /// <summary>
        /// Stores the specified number value as a default factory global parameter mapped to the specified name.
        /// </summary>
        static void SetNumber(const wchar_t* name, int32_t value) { RecognizerFactoryParameterValue(SPXHANDLE_DEFAULT, name).SetNumber(value); }

        /// <summary>
        /// Returns number value of the default factory global parameter mapped to the specified name.
        /// </summary>
        /// <param name="defaultValue">Default value to return if the mapping does not exist. </param>
        static int32_t GetNumber(const wchar_t* name, int32_t defaultValue = 0) { return RecognizerFactoryParameterValue(SPXHANDLE_DEFAULT, name).GetNumber(defaultValue); }

        /// <summary>
        /// Returns true if default factory contains a global parameter of a boolean type with the corresponding name.
        /// </summary>
        static bool ContainsBool(const wchar_t* name) { return RecognizerFactoryParameterValue(SPXHANDLE_DEFAULT, name).IsBool(); }

        /// <summary>
        /// Stores the specified boolean value as a default factory global parameter mapped to the specified name.
        /// </summary>
        static void SetBool(const wchar_t* name, bool value) { RecognizerFactoryParameterValue(SPXHANDLE_DEFAULT, name).SetBool(value); }
        
        /// <summary>
        /// Returns boolean value of the default factory global parameter mapped to the specified name.
        /// </summary>
        /// <param name="defaultValue">Default value to return if the mapping does not exist. </param>
        static bool GetBool(const wchar_t* name, bool defaultValue = false) { return RecognizerFactoryParameterValue(SPXHANDLE_DEFAULT, name).GetBool(defaultValue); }

        /// <summary>
        /// Returns true if the provided FactoryParameter enum value corresponds to a default factory global parameter of type string.
        /// </summary>
        static bool ContainsString(enum FactoryParameter parameter) { return RecognizerFactoryParameterValue(SPXHANDLE_DEFAULT, parameter).IsString(); }

        /// <summary>
        /// Stores the specified string value as a default factory global parameter corresponding to the provided FactoryParameter enum value.
        /// </summary>
        static void SetString(enum FactoryParameter parameter, const wchar_t* value) { RecognizerFactoryParameterValue(SPXHANDLE_DEFAULT, parameter).SetString(value); }

        /// <summary>
        /// Returns string value of the default factory global parameter corresponding to the provided FactoryParameter enum value.
        /// </summary>
        /// <param name="defaultValue">Default value to return if the corresponding parameter does not exist</param>
        static std::wstring GetString(enum FactoryParameter parameter, const wchar_t* defaultValue = L"") { return RecognizerFactoryParameterValue(SPXHANDLE_DEFAULT, parameter).GetString(defaultValue); }

        /// <summary>
        /// Returns true if the provided FactoryParameter enum value corresponds to a default factory global parameter of a number type.
        /// </summary>
        static bool ContainsNumber(enum FactoryParameter parameter) { return RecognizerFactoryParameterValue(SPXHANDLE_DEFAULT, parameter).IsNumber(); }

        /// <summary>
        /// Stores the specified number value as a default factory global parameter corresponding to the provided FactoryParameter enum value.
        /// </summary>
        static void SetNumber(enum FactoryParameter parameter, int32_t value) { RecognizerFactoryParameterValue(SPXHANDLE_DEFAULT, parameter).SetNumber(value); }

        /// <summary>
        /// Returns number value of the default factory global parameter corresponding to the provided FactoryParameter enum value.
        /// </summary>
        /// <param name="defaultValue">Default value to return if the corresponding parameter does not exist</param>
        static int32_t GetNumber(enum FactoryParameter parameter, int32_t defaultValue = 0) { return RecognizerFactoryParameterValue(SPXHANDLE_DEFAULT, parameter).GetNumber(defaultValue); }

        /// <summary>
        /// Returns true if the provided FactoryParameter enum value corresponds to a default factory global parameter of a boolean type.
        /// </summary>
        static bool ContainsBool(enum FactoryParameter parameter) { return RecognizerFactoryParameterValue(SPXHANDLE_DEFAULT, parameter).IsBool(); }

        /// <summary>
        /// Stores the specified boolean value as a default factory global parameter corresponding to the provided FactoryParameter enum value.
        /// </summary>
        static void SetBool(enum FactoryParameter parameter, bool value) { RecognizerFactoryParameterValue(SPXHANDLE_DEFAULT, parameter).SetBool(value); }

        /// <summary>
        /// Returns boolean value of the default factory global parameter corresponding to the provided FactoryParameter enum value.
        /// </summary>
        /// <param name="defaultValue">Default value to return if the corresponding parameter does not exist</param>
        static bool GetBool(enum FactoryParameter parameter, bool defaultValue = false) { return RecognizerFactoryParameterValue(SPXHANDLE_DEFAULT, parameter).GetBool(defaultValue); }
    };

    /// <summary>
    /// Sets a subscription key that will be used for all recognizers created by the default factory.
    /// </summary>
    /// <param name="value">A subscription key.</param>
    static void SetSubscriptionKey(const std::wstring& value)
    {
        Parameters::SetString(FactoryParameter::SpeechSubscriptionKey, value.c_str());
    }

    /// <summary>
    /// Sets a speech service endpoint that will be used for all recognizers created by the default factory.
    /// </summary>
    /// <param name="value">A speech service endpoint (url).</param>
    static void SetSpeechEndpoint(const std::wstring& value)
    {
        Parameters::SetString(FactoryParameter::SpeechEndpoint, value.c_str());
    }

    DISABLE_DEFAULT_CTORS(DefaultRecognizerFactory);
};


} } // CARBON_NAMESPACE_ROOT :: Recognition
