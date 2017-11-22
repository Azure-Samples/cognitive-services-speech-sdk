#pragma once
#include "recognizer.h"


namespace CARBON_IMPL_NAMESPACE() {


class CSpxRecognizerFactory
{
public:

    static std::shared_ptr<ISpxRecognizer> CreateSpeechRecognizer();
    static std::shared_ptr<ISpxRecognizer> CreateSpeechRecognizer(bool passiveListeningEnaled);
    static std::shared_ptr<ISpxRecognizer> CreateSpeechRecognizer(const std::wstring& language);
    static std::shared_ptr<ISpxRecognizer> CreateSpeechRecognizerWithFileInput(const std::wstring& fileName);
    static std::shared_ptr<ISpxRecognizer> CreateSpeechRecognizerWithFileInput(const std::wstring& fileName, const std::wstring& language);

    static std::shared_ptr<ISpxRecognizer> CreateDictationRecognizer();
    static std::shared_ptr<ISpxRecognizer> CreateDictationRecognizer(bool passiveListeningEnaled);
    static std::shared_ptr<ISpxRecognizer> CreateDictationRecognizer(const std::wstring& language);
    static std::shared_ptr<ISpxRecognizer> CreateDictationRecognizerWithFileInput(const std::wstring& fileName);
    static std::shared_ptr<ISpxRecognizer> CreateDictationRecognizerWithFileInput(const std::wstring& fileName, const std::wstring& language);

    static std::shared_ptr<ISpxRecognizer> CreateIntentRecognizer();
    static std::shared_ptr<ISpxRecognizer> CreateIntentRecognizer(bool passiveListeningEnaled);
    static std::shared_ptr<ISpxRecognizer> CreateIntentRecognizer(const std::wstring& language);
    static std::shared_ptr<ISpxRecognizer> CreateIntentRecognizerWithFileInput(const std::wstring& fileName);
    static std::shared_ptr<ISpxRecognizer> CreateIntentRecognizerWithFileInput(const std::wstring& fileName, const std::wstring& language);


private:

    CSpxRecognizerFactory() = delete;
    CSpxRecognizerFactory(const CSpxRecognizerFactory&) = delete;
    CSpxRecognizerFactory(const CSpxRecognizerFactory&&) = delete;
};


}; // CARBON_IMPL_NAMESPACE()

