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
#include <speechapi_cxx_translation_recognizer.h>
#include <speechapi_cxx_intent_recognizer.h>
#include <speechapi_c_recognizer_factory.h>


namespace CARBON_NAMESPACE_ROOT {
namespace Recognition {

class RecognizerFactory
{
public:

    static std::shared_ptr<Speech::SpeechRecognizer> CreateSpeechRecognizer()
    {
        SPXRECOHANDLE hreco = SPXHANDLE_INVALID;
        SPX_THROW_ON_FAIL(::RecognizerFactory_CreateSpeechRecognizer_With_Defaults(&hreco));
        return std::make_shared<Speech::SpeechRecognizer>(hreco); 
    }

    static std::shared_ptr<Speech::SpeechRecognizer> CreateSpeechRecognizer(bool passiveListeningEnaled) { UNUSED(passiveListeningEnaled); throw nullptr; }
    static std::shared_ptr<Speech::SpeechRecognizer> CreateSpeechRecognizer(const std::wstring& language) { UNUSED(language); throw nullptr; };

    static std::shared_ptr<Speech::SpeechRecognizer> CreateSpeechRecognizerWithFileInput(const std::wstring& fileName)
    {
        SPXRECOHANDLE hreco = SPXHANDLE_INVALID;
        SPX_THROW_ON_FAIL(::RecognizerFactory_CreateSpeechRecognizer_With_FileInput(&hreco, fileName.c_str()));
        return std::make_shared<Speech::SpeechRecognizer>(hreco);
    };

    static std::shared_ptr<Speech::SpeechRecognizer> CreateSpeechRecognizerWithFileInput(const std::wstring& fileName, const std::wstring& language) { UNUSED(fileName); UNUSED(language); throw nullptr; };

    static std::shared_ptr<Speech::SpeechRecognizer> CreateDictationRecognizer() { throw nullptr; return CreateSpeechRecognizer(); }
    static std::shared_ptr<Speech::SpeechRecognizer> CreateDictationRecognizer(bool passiveListeningEnaled) { UNUSED(passiveListeningEnaled); throw nullptr; }
    static std::shared_ptr<Speech::SpeechRecognizer> CreateDictationRecognizer(const std::wstring& language) { UNUSED(language); throw nullptr; };
    static std::shared_ptr<Speech::SpeechRecognizer> CreateDictationRecognizerWithFileInput(const std::wstring& fileName) { UNUSED(fileName); throw nullptr; };
    static std::shared_ptr<Speech::SpeechRecognizer> CreateDictationRecognizerWithFileInput(const std::wstring& fileName, const std::wstring& language) { UNUSED(fileName); UNUSED(language); throw nullptr; };

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

    static std::shared_ptr<Intent::IntentRecognizer> CreateIntentRecognizer(bool passiveListeningEnaled) { UNUSED(passiveListeningEnaled); throw nullptr; }
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
