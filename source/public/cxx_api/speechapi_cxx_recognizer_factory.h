//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_cxx_recognizer_factory.h: Public API declarations for RecognizerFactory C++ static class methods
//

#pragma once
#include <memory>
#include <speechapi_cxx_common.h>
#include <speechapi_cxx_speech_recognizer.h>
#include <speechapi_cxx_todo_intent.h>
#include <speechapi_c_recognizer_factory.h>


using namespace CARBON_NAMESPACE_ROOT::Recognition::Speech;
using namespace CARBON_NAMESPACE_ROOT::Recognition::Intent;

namespace CARBON_NAMESPACE_ROOT {
namespace Recognition {


class RecognizerFactory
{
public:

    static std::shared_ptr<SpeechRecognizer> CreateSpeechRecognizer()
    {
        SPX_INIT_HR(hr);

        SPXRECOHANDLE hreco = SPXHANDLE_INVALID;
        if (SPX_SUCCEEDED(hr))
        {
            hr = ::RecognizerFactory_CreateSpeechRecognzier_With_Defaults(&hreco);
        }

        SPX_THROW_ON_FAIL(hr);

        return std::make_shared<SpeechRecognizer>(hreco); 
    }

    static std::shared_ptr<SpeechRecognizer> CreateSpeechRecognizer(bool passiveListeningEnaled) { throw nullptr; }
    static std::shared_ptr<SpeechRecognizer> CreateSpeechRecognizer(const std::wstring& language) { throw nullptr; };

    static std::shared_ptr<SpeechRecognizer> CreateSpeechRecognizerWithFileInput(const std::wstring& fileName)
    {
        SPX_INIT_HR(hr);

        SPXRECOHANDLE hreco = SPXHANDLE_INVALID;
        if (SPX_SUCCEEDED(hr))
        {
            hr = ::RecognizerFactory_CreateSpeechRecognzier_With_FileInput(&hreco, fileName.c_str());
        }

        SPX_THROW_ON_FAIL(hr);

        return std::make_shared<SpeechRecognizer>(hreco);
    };

    static std::shared_ptr<SpeechRecognizer> CreateSpeechRecognizerWithFileInput(const std::wstring& fileName, const std::wstring& language) { throw nullptr; };

    static std::shared_ptr<SpeechRecognizer> CreateDictationRecognizer() { return std::make_shared<SpeechRecognizer>(); }
    static std::shared_ptr<SpeechRecognizer> CreateDictationRecognizer(bool passiveListeningEnaled) { throw nullptr; }
    static std::shared_ptr<SpeechRecognizer> CreateDictationRecognizer(const std::wstring& language) { throw nullptr; };
    static std::shared_ptr<SpeechRecognizer> CreateDictationRecognizerWithFileInput(const std::wstring& fileName) { throw nullptr; };
    static std::shared_ptr<SpeechRecognizer> CreateDictationRecognizerWithFileInput(const std::wstring& fileName, const std::wstring& language) { throw nullptr; };

    static std::shared_ptr<IntentRecognizer> CreateIntentRecognizer() { return std::make_shared<IntentRecognizer>(); }
    static std::shared_ptr<IntentRecognizer> CreateIntentRecognizer(bool passiveListeningEnaled) { throw nullptr; }
    static std::shared_ptr<IntentRecognizer> CreateIntentRecognizer(const std::wstring& language) { throw nullptr; };
    static std::shared_ptr<IntentRecognizer> CreateIntentRecognizerWithFileInput(const std::wstring& fileName) { throw nullptr; };
    static std::shared_ptr<IntentRecognizer> CreateIntentRecognizerWithFileInput(const std::wstring& fileName, const std::wstring& language) { throw nullptr; };


private:

    RecognizerFactory() = delete;
    RecognizerFactory(const RecognizerFactory&) = delete;
    RecognizerFactory(const RecognizerFactory&&) = delete;
};


} }; // CARBON_NAMESPACE_ROOT :: Recognition
