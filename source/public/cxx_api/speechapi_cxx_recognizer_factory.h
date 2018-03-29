//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_cxx_recognizer_factory.h: Public API declarations for RecognizerFactory C++ static class methods
//

#pragma once
#include <memory>
#include <speechapi_c_common.h>
#include <speechapi_cxx_common.h>
#include <speechapi_cxx_speech_recognizer.h>
#include <speechapi_cxx_intent_recognizer.h>
#include <speechapi_cxx_translation_recognizer.h>
#include <speechapi_cxx_recognizer_factory_parameter.h>
#include <speechapi_c_recognizer_factory.h>


namespace CARBON_NAMESPACE_ROOT {
namespace Recognition {


class IRecognizerFactory
{
public:

    IRecognizerFactory(RecognizerFactoryParameterCollection& parameters) : Parameters(parameters) { }
    virtual ~IRecognizerFactory() { }

    virtual std::shared_ptr<Speech::SpeechRecognizer> CreateSpeechRecognizer() = 0;
    virtual std::shared_ptr<Intent::IntentRecognizer> CreateIntentRecognizer() = 0;

    RecognizerFactoryParameterCollection& Parameters;


private:

    IRecognizerFactory(IRecognizerFactory&&) = delete;
    IRecognizerFactory(const IRecognizerFactory&) = delete;
    IRecognizerFactory& operator=(IRecognizerFactory&&) = delete;
    IRecognizerFactory& operator=(const IRecognizerFactory&) = delete;
};


class IDefaultRecognizerFactory : public IRecognizerFactory
{
public:

    IDefaultRecognizerFactory(RecognizerFactoryParameterCollection& parameters) : IRecognizerFactory(parameters) { }

    virtual std::shared_ptr<Speech::SpeechRecognizer> CreateSpeechRecognizer() = 0;
    virtual std::shared_ptr<Speech::SpeechRecognizer> CreateSpeechRecognizer(const std::wstring& language) = 0;
    virtual std::shared_ptr<Speech::SpeechRecognizer> CreateSpeechRecognizerWithFileInput(const std::wstring& fileName) = 0;
    virtual std::shared_ptr<Speech::SpeechRecognizer> CreateSpeechRecognizerWithFileInput(const std::wstring& fileName, const std::wstring& language) = 0;

    virtual std::shared_ptr<Intent::IntentRecognizer> CreateIntentRecognizer() = 0;
    virtual std::shared_ptr<Intent::IntentRecognizer> CreateIntentRecognizer(const std::wstring& language) = 0;
    virtual std::shared_ptr<Intent::IntentRecognizer> CreateIntentRecognizerWithFileInput(const std::wstring& fileName) = 0;
    virtual std::shared_ptr<Intent::IntentRecognizer> CreateIntentRecognizerWithFileInput(const std::wstring& fileName, const std::wstring& language) = 0;

    virtual std::shared_ptr<Translation::TranslationRecognizer> CreateTranslationRecognizer(const std::wstring& sourceLanguage, const std::wstring& targetLanguage) = 0;
    virtual std::shared_ptr<Translation::TranslationRecognizer> CreateTranslationRecognizerWithFileInput(const std::wstring& fileName, const std::wstring& sourceLanguage, const std::wstring& targetLanguage) = 0;

    virtual void SetSubscriptionKey(const wchar_t* value) = 0;
    virtual void SetSpeechEndpoint(const wchar_t* value) = 0;


private:

    IDefaultRecognizerFactory(IDefaultRecognizerFactory&&) = delete;
    IDefaultRecognizerFactory(const IDefaultRecognizerFactory&) = delete;
    IDefaultRecognizerFactory& operator=(IDefaultRecognizerFactory&&) = delete;
    IDefaultRecognizerFactory& operator=(const IDefaultRecognizerFactory&) = delete;
};


// TODO: Should we have handles for the RecognizerFactory_C_APIs? 

class RecognizerFactory
{
public:

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

        virtual std::shared_ptr<Intent::IntentRecognizer> CreateIntentRecognizer() override
        {
            SPXRECOHANDLE hreco = SPXHANDLE_INVALID;
            SPX_THROW_ON_FAIL(::RecognizerFactory_CreateIntentRecognizer_With_Defaults(SPXHANDLE_DEFAULT, &hreco));
            return std::make_shared<Intent::IntentRecognizer>(hreco); 
        }


        // --- IDefaultRecognizerFactory overrides ---

        virtual std::shared_ptr<Speech::SpeechRecognizer> CreateSpeechRecognizer(const std::wstring& language)
        {
            UNUSED(language);
            throw SPXERR_NOT_IMPL;
        }

        virtual std::shared_ptr<Speech::SpeechRecognizer> CreateSpeechRecognizerWithFileInput(const std::wstring& fileName)
        {
            SPXRECOHANDLE hreco = SPXHANDLE_INVALID;
            SPX_THROW_ON_FAIL(::RecognizerFactory_CreateSpeechRecognizer_With_FileInput(SPXHANDLE_DEFAULT, &hreco, fileName.c_str()));
            return std::make_shared<Speech::SpeechRecognizer>(hreco);
        }

        virtual std::shared_ptr<Speech::SpeechRecognizer> CreateSpeechRecognizerWithFileInput(const std::wstring& fileName, const std::wstring& language)
        {
            UNUSED(language);
            UNUSED(fileName);
            throw SPXERR_NOT_IMPL;
        }

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

        virtual std::shared_ptr<Translation::TranslationRecognizer> CreateTranslationRecognizer(const std::wstring& sourceLanguage, const std::wstring& targetLanguage)
        {
            UNUSED(sourceLanguage);
            UNUSED(targetLanguage);
            throw SPXERR_NOT_IMPL;
        }

        virtual std::shared_ptr<Translation::TranslationRecognizer> CreateTranslationRecognizerWithFileInput(const std::wstring& fileName, const std::wstring& sourceLanguage, const std::wstring& targetLanguage)
        {
            UNUSED(sourceLanguage);
            UNUSED(targetLanguage);
            UNUSED(fileName);
            throw SPXERR_NOT_IMPL;
        }

        virtual void SetSubscriptionKey(const wchar_t* value)
        {
            Parameters[FactoryParameter::SpeechSubscriptionKey] = value;
        }

        virtual void SetSpeechEndpoint(const wchar_t* value)
        {
            Parameters[FactoryParameter::SpeechEndpoint] = value;
        }


    private:

        InternalDefaultRecognizerFactory(InternalDefaultRecognizerFactory&&) = delete;
        InternalDefaultRecognizerFactory(const InternalDefaultRecognizerFactory&) = delete;
        InternalDefaultRecognizerFactory& operator=(InternalDefaultRecognizerFactory&&) = delete;
        InternalDefaultRecognizerFactory& operator=(const InternalDefaultRecognizerFactory&) = delete;

        RecognizerFactoryParameterCollection m_parameters;
    };

    RecognizerFactory(RecognizerFactory&&) = delete;
    RecognizerFactory(const RecognizerFactory&) = delete;
    RecognizerFactory& operator=(RecognizerFactory&&) = delete;
    RecognizerFactory& operator=(const RecognizerFactory&) = delete;
};


class DefaultRecognizerFactory
{
public:

    // --- 'SpeechRecognizer' RecognizerFactory methods ---

    static std::shared_ptr<Speech::SpeechRecognizer> CreateSpeechRecognizer()
    {
        auto factory = RecognizerFactory::GetDefault();
        return factory->CreateSpeechRecognizer();
    }

    static std::shared_ptr<Speech::SpeechRecognizer> CreateSpeechRecognizer(const std::wstring& language)
    {
        auto factory = RecognizerFactory::GetDefault();
        return factory->CreateSpeechRecognizer(language);
    }

    static std::shared_ptr<Speech::SpeechRecognizer> CreateSpeechRecognizerWithFileInput(const std::wstring& fileName)
    {
        auto factory = RecognizerFactory::GetDefault();
        return factory->CreateSpeechRecognizerWithFileInput(fileName);
    }

    static std::shared_ptr<Speech::SpeechRecognizer> CreateSpeechRecognizerWithFileInput(const std::wstring& fileName, const std::wstring& language)
    {
        auto factory = RecognizerFactory::GetDefault();
        return factory->CreateSpeechRecognizerWithFileInput(fileName, language);
    }


    // --- 'IntentRecognizer' RecognizerFactory methods ---

    static std::shared_ptr<Intent::IntentRecognizer> CreateIntentRecognizer()
    {
        auto factory = RecognizerFactory::GetDefault();
        return factory->CreateIntentRecognizer();
    }

    static std::shared_ptr<Intent::IntentRecognizer> CreateIntentRecognizer(const std::wstring& language)
    {
        auto factory = RecognizerFactory::GetDefault();
        return factory->CreateIntentRecognizer(language);
    }

    static std::shared_ptr<Intent::IntentRecognizer> CreateIntentRecognizerWithFileInput(const std::wstring& fileName)
    {
        auto factory = RecognizerFactory::GetDefault();
        return factory->CreateIntentRecognizerWithFileInput(fileName);
    }

    static std::shared_ptr<Intent::IntentRecognizer> CreateIntentRecognizerWithFileInput(const std::wstring& fileName, const std::wstring& language)
    {
        auto factory = RecognizerFactory::GetDefault();
        return factory->CreateIntentRecognizerWithFileInput(fileName, language);
    };


    // --- 'Translation' RecognizerFactory methods ---

    static std::shared_ptr<Translation::TranslationRecognizer> CreateTranslationRecognizer(const std::wstring& sourceLanguage, const std::wstring& targetLanguage)
    {
        auto factory = RecognizerFactory::GetDefault();
        return factory->CreateTranslationRecognizer(sourceLanguage, targetLanguage);
    }

    static std::shared_ptr<Translation::TranslationRecognizer> CreateTranslationRecognizerWithFileInput(const std::wstring& fileName, const std::wstring& sourceLanguage, const std::wstring& targetLanguage)
    {
        auto factory = RecognizerFactory::GetDefault();
        return factory->CreateTranslationRecognizerWithFileInput(fileName, sourceLanguage, targetLanguage);
    }


    // --- Static accessors to RecognizerFactoryParameters ---

    class Parameters
    {
    public:

        static bool ContainsString(const wchar_t* name) { return RecognizerFactoryParameterValue(SPXHANDLE_DEFAULT, name).IsString(); }
        static void SetString(const wchar_t* name, const wchar_t* value) { RecognizerFactoryParameterValue(SPXHANDLE_DEFAULT, name).SetString(value); }
        static std::wstring GetString(const wchar_t* name, const wchar_t* defaultValue = L"") { return RecognizerFactoryParameterValue(SPXHANDLE_DEFAULT, name).GetString(defaultValue); }

        static bool ContainsNumber(const wchar_t* name) { return RecognizerFactoryParameterValue(SPXHANDLE_DEFAULT, name).IsNumber(); }
        static void SetNumber(const wchar_t* name, int32_t value) { RecognizerFactoryParameterValue(SPXHANDLE_DEFAULT, name).SetNumber(value); }
        static int32_t GetNumber(const wchar_t* name, int32_t defaultValue = 0) { return RecognizerFactoryParameterValue(SPXHANDLE_DEFAULT, name).GetNumber(defaultValue); }

        static bool ContainsBool(const wchar_t* name) { return RecognizerFactoryParameterValue(SPXHANDLE_DEFAULT, name).IsBool(); }
        static void SetBool(const wchar_t* name, bool value) { RecognizerFactoryParameterValue(SPXHANDLE_DEFAULT, name).SetBool(value); }
        static bool GetBool(const wchar_t* name, bool defaultValue = false) { return RecognizerFactoryParameterValue(SPXHANDLE_DEFAULT, name).GetBool(defaultValue); }

        static bool ContainsString(enum FactoryParameter parameter) { return RecognizerFactoryParameterValue(SPXHANDLE_DEFAULT, parameter).IsString(); }
        static void SetString(enum FactoryParameter parameter, const wchar_t* value) { RecognizerFactoryParameterValue(SPXHANDLE_DEFAULT, parameter).SetString(value); }
        static std::wstring GetString(enum FactoryParameter parameter, const wchar_t* defaultValue = L"") { return RecognizerFactoryParameterValue(SPXHANDLE_DEFAULT, parameter).GetString(defaultValue); }

        static bool ContainsNumber(enum FactoryParameter parameter) { return RecognizerFactoryParameterValue(SPXHANDLE_DEFAULT, parameter).IsNumber(); }
        static void SetNumber(enum FactoryParameter parameter, int32_t value) { RecognizerFactoryParameterValue(SPXHANDLE_DEFAULT, parameter).SetNumber(value); }
        static int32_t GetNumber(enum FactoryParameter parameter, int32_t defaultValue = 0) { return RecognizerFactoryParameterValue(SPXHANDLE_DEFAULT, parameter).GetNumber(defaultValue); }

        static bool ContainsBool(enum FactoryParameter parameter) { return RecognizerFactoryParameterValue(SPXHANDLE_DEFAULT, parameter).IsBool(); }
        static void SetBool(enum FactoryParameter parameter, bool value) { RecognizerFactoryParameterValue(SPXHANDLE_DEFAULT, parameter).SetBool(value); }
        static bool GetBool(enum FactoryParameter parameter, bool defaultValue = false) { return RecognizerFactoryParameterValue(SPXHANDLE_DEFAULT, parameter).GetBool(defaultValue); }
    };

    static void SetSubscriptionKey(const wchar_t* value)
    {
        Parameters::SetString(FactoryParameter::SpeechSubscriptionKey, value);
    }

    static void SetSpeechEndpoint(const wchar_t* value)
    {
        Parameters::SetString(FactoryParameter::SpeechEndpoint, value);
    }

    DefaultRecognizerFactory() = delete;
    DefaultRecognizerFactory(DefaultRecognizerFactory&&) = delete;
    DefaultRecognizerFactory(const DefaultRecognizerFactory&) = delete;
    DefaultRecognizerFactory& operator=(DefaultRecognizerFactory&&) = delete;
    DefaultRecognizerFactory& operator=(const DefaultRecognizerFactory&) = delete;
};


} } // CARBON_NAMESPACE_ROOT :: Recognition
