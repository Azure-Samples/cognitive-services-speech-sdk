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


class RecognizerFactory
{
public:

    class Parameters
    {
    public:

        static bool IsString(const wchar_t* name) { return RecognizerFactoryParameter(name).IsString(); }
        static void SetString(const wchar_t* name, const wchar_t* value) { RecognizerFactoryParameter(name).SetString(value); }
        static std::wstring GetString(const wchar_t* name, const wchar_t* defaultValue = L"") { return RecognizerFactoryParameter(name).GetString(defaultValue); }

        static bool IsNumber(const wchar_t* name) { return RecognizerFactoryParameter(name).IsNumber(); }
        static void SetNumber(const wchar_t* name, int32_t value) { RecognizerFactoryParameter(name).SetNumber(value); }
        static int32_t GetNumber(const wchar_t* name, int32_t defaultValue = 0) { return RecognizerFactoryParameter(name).GetNumber(defaultValue); }

        static bool IsBool(const wchar_t* name) { return RecognizerFactoryParameter(name).IsBool(); }
        static void SetBool(const wchar_t* name, bool value) { RecognizerFactoryParameter(name).SetBool(value); }
        static bool GetBool(const wchar_t* name, bool defaultValue = false) { return RecognizerFactoryParameter(name).GetBool(defaultValue); }
    };

    static std::shared_ptr<Speech::SpeechRecognizer> CreateSpeechRecognizer()
    {
        SPXRECOHANDLE hreco = SPXHANDLE_INVALID;
        SPX_THROW_ON_FAIL(::RecognizerFactory_CreateSpeechRecognizer_With_Defaults(&hreco));
        return std::make_shared<Speech::SpeechRecognizer>(hreco); 
    }

    static std::shared_ptr<Speech::SpeechRecognizer> CreateSpeechRecognizer(const std::wstring& language) { UNUSED(language); throw nullptr; };

    static std::shared_ptr<Speech::SpeechRecognizer> CreateSpeechRecognizerWithFileInput(const std::wstring& fileName)
    {
        SPXRECOHANDLE hreco = SPXHANDLE_INVALID;
        SPX_THROW_ON_FAIL(::RecognizerFactory_CreateSpeechRecognizer_With_FileInput(&hreco, fileName.c_str()));
        return std::make_shared<Speech::SpeechRecognizer>(hreco);
    };

    static std::shared_ptr<Speech::SpeechRecognizer> CreateSpeechRecognizerWithFileInput(const std::wstring& fileName, const std::wstring& language) { UNUSED(fileName); UNUSED(language); throw nullptr; };

    static std::shared_ptr<Translation::TranslationRecognizer> CreateTranslationRecognizer(const std::wstring& sourceLanguage, const std::wstring& targetLanguage)
    {
        return std::make_shared<Translation::TranslationRecognizer>(sourceLanguage, targetLanguage);
    }

    static std::shared_ptr<Translation::TranslationRecognizer> CreateTranslationRecognizerWithFileInput(const std::wstring& fileName, const std::wstring& sourceLanguage, const std::wstring& targetLanguage)
    {
        UNUSED(fileName);
        UNUSED(sourceLanguage);
        UNUSED(targetLanguage);
        throw nullptr;
    }

    static std::shared_ptr<Intent::IntentRecognizer> CreateIntentRecognizer()
    {
        SPXRECOHANDLE hreco = SPXHANDLE_INVALID;
        SPX_THROW_ON_FAIL(::RecognizerFactory_CreateIntentRecognizer_With_Defaults(&hreco));
        return std::make_shared<Intent::IntentRecognizer>(hreco); 
    }

    static std::shared_ptr<Intent::IntentRecognizer> CreateIntentRecognizer(const std::wstring& language) { UNUSED(language); throw nullptr; };

    static std::shared_ptr<Intent::IntentRecognizer> CreateIntentRecognizerWithFileInput(const std::wstring& fileName)
    {
        SPXRECOHANDLE hreco = SPXHANDLE_INVALID;
        SPX_THROW_ON_FAIL(::RecognizerFactory_CreateIntentRecognizer_With_FileInput(&hreco, fileName.c_str()));
        return std::make_shared<Intent::IntentRecognizer>(hreco);
    }

    static std::shared_ptr<Intent::IntentRecognizer> CreateIntentRecognizerWithFileInput(const std::wstring& fileName, const std::wstring& language) { UNUSED(fileName); UNUSED(language); throw nullptr; };


private:

    RecognizerFactory() = delete;
    RecognizerFactory(RecognizerFactory&&) = delete;
    RecognizerFactory(const RecognizerFactory&) = delete;
    RecognizerFactory& operator=(RecognizerFactory&&) = delete;
    RecognizerFactory& operator=(const RecognizerFactory&) = delete;
};


} } // CARBON_NAMESPACE_ROOT :: Recognition
