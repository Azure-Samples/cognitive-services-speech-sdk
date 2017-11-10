#pragma once
#include "recognizer.h"


namespace CARBON_IMPL_NAMESPACE() {


class CSpxRecognizerFactory
{
public:

    static std::shared_ptr<CSpxRecognizer> CreateSpeechRecognizer();
    static std::shared_ptr<CSpxRecognizer> CreateSpeechRecognizer(bool passiveListeningEnaled);
    static std::shared_ptr<CSpxRecognizer> CreateSpeechRecognizer(const std::wstring& language);
    static std::shared_ptr<CSpxRecognizer> CreateSpeechRecognizerWithFileInput(const std::wstring& filename);
    static std::shared_ptr<CSpxRecognizer> CreateSpeechRecognizerWithFileInput(const std::wstring& filename, const std::wstring& language);

    static std::shared_ptr<CSpxRecognizer> CreateDictationRecognizer();
    static std::shared_ptr<CSpxRecognizer> CreateDictationRecognizer(bool passiveListeningEnaled);
    static std::shared_ptr<CSpxRecognizer> CreateDictationRecognizer(const std::wstring& language);
    static std::shared_ptr<CSpxRecognizer> CreateDictationRecognizerWithFileInput(const std::wstring& filename);
    static std::shared_ptr<CSpxRecognizer> CreateDictationRecognizerWithFileInput(const std::wstring& filename, const std::wstring& language);

    static std::shared_ptr<CSpxRecognizer> CreateIntentRecognizer();
    static std::shared_ptr<CSpxRecognizer> CreateIntentRecognizer(bool passiveListeningEnaled);
    static std::shared_ptr<CSpxRecognizer> CreateIntentRecognizer(const std::wstring& language);
    static std::shared_ptr<CSpxRecognizer> CreateIntentRecognizerWithFileInput(const std::wstring& filename);
    static std::shared_ptr<CSpxRecognizer> CreateIntentRecognizerWithFileInput(const std::wstring& filename, const std::wstring& language);


private:

    CSpxRecognizerFactory() = delete;
    CSpxRecognizerFactory(const CSpxRecognizerFactory&) = delete;
    CSpxRecognizerFactory(const CSpxRecognizerFactory&&) = delete;
};


}; // CARBON_IMPL_NAMESPACE()

