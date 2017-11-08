#pragma once
#include <memory>
#include <speechapi_cxx_common.h>
#include <speechapi_cxx_speech_recognizer.h>
#include <speechapi_cxx_todo_intent.h>


using namespace CARBON_NAMESPACE_ROOT::Recognition::Speech;
using namespace CARBON_NAMESPACE_ROOT::Recognition::Intent;

namespace CARBON_NAMESPACE_ROOT {
namespace Recognition {


class RecognizerFactory
{
public:

    static std::shared_ptr<SpeechRecognizer> CreateSpeechRecognizer() { return std::make_shared<SpeechRecognizer>(); }
    static std::shared_ptr<SpeechRecognizer> CreateSpeechRecognizer(bool passiveListeningEnaled) { throw nullptr; }
    static std::shared_ptr<SpeechRecognizer> CreateSpeechRecognizer(const std::wstring& language) { throw nullptr; };
    static std::shared_ptr<SpeechRecognizer> CreateSpeechRecognizerWithFileInput(const std::wstring& filename) { throw nullptr; };
    static std::shared_ptr<SpeechRecognizer> CreateSpeechRecognizerWithFileInput(const std::wstring& filename, const std::wstring& language) { throw nullptr; };

    static std::shared_ptr<SpeechRecognizer> CreateDictationRecognizer() { return std::make_shared<SpeechRecognizer>(); }
    static std::shared_ptr<SpeechRecognizer> CreateDictationRecognizer(bool passiveListeningEnaled) { throw nullptr; }
    static std::shared_ptr<SpeechRecognizer> CreateDictationRecognizer(const std::wstring& language) { throw nullptr; };
    static std::shared_ptr<SpeechRecognizer> CreateDictationRecognizerWithFileInput(const std::wstring& filename) { throw nullptr; };
    static std::shared_ptr<SpeechRecognizer> CreateDictationRecognizerWithFileInput(const std::wstring& filename, const std::wstring& language) { throw nullptr; };

    static std::shared_ptr<IntentRecognizer> CreateIntentRecognizer() { return std::make_shared<IntentRecognizer>(); }
    static std::shared_ptr<IntentRecognizer> CreateIntentRecognizer(bool passiveListeningEnaled) { throw nullptr; }
    static std::shared_ptr<IntentRecognizer> CreateIntentRecognizer(const std::wstring& language) { throw nullptr; };
    static std::shared_ptr<IntentRecognizer> CreateIntentRecognizerWithFileInput(const std::wstring& filename) { throw nullptr; };
    static std::shared_ptr<IntentRecognizer> CreateIntentRecognizerWithFileInput(const std::wstring& filename, const std::wstring& language) { throw nullptr; };


private:

    RecognizerFactory() = delete;
    RecognizerFactory(const RecognizerFactory&) = delete;
    RecognizerFactory(const RecognizerFactory&&) = delete;
};


} }; // CARBON_NAMESPACE_ROOT :: Recognition
