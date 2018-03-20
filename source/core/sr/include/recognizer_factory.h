#pragma once
#include <spxdebug.h>
#include <spxerror.h>


namespace CARBON_IMPL_NAMESPACE() {


class CSpxRecognizerFactory
{
public:

    static std::shared_ptr<ISpxRecognizer> CreateSpeechRecognizer();
    static std::shared_ptr<ISpxRecognizer> CreateSpeechRecognizer(bool passiveListeningEnabled) { UNUSED(passiveListeningEnabled); throw SPXERR_NOT_IMPL; }
    static std::shared_ptr<ISpxRecognizer> CreateSpeechRecognizer(const std::wstring& language) { UNUSED(language); throw SPXERR_NOT_IMPL; }
    static std::shared_ptr<ISpxRecognizer> CreateSpeechRecognizerWithFileInput(const std::wstring& fileName);
    static std::shared_ptr<ISpxRecognizer> CreateSpeechRecognizerWithFileInput(const std::wstring& fileName, const std::wstring& language) { UNUSED(fileName); UNUSED(language); throw SPXERR_NOT_IMPL; }

    static std::shared_ptr<ISpxRecognizer> CreateDictationRecognizer() { throw SPXERR_NOT_IMPL; }
    static std::shared_ptr<ISpxRecognizer> CreateDictationRecognizer(bool passiveListeningEnabled) { UNUSED(passiveListeningEnabled);  throw SPXERR_NOT_IMPL; }
    static std::shared_ptr<ISpxRecognizer> CreateDictationRecognizer(const std::wstring& language) { UNUSED(language); throw SPXERR_NOT_IMPL; }
    static std::shared_ptr<ISpxRecognizer> CreateDictationRecognizerWithFileInput(const std::wstring& fileName) { UNUSED(fileName); throw SPXERR_NOT_IMPL; }
    static std::shared_ptr<ISpxRecognizer> CreateDictationRecognizerWithFileInput(const std::wstring& fileName, const std::wstring& language) { UNUSED(fileName); UNUSED(language); throw SPXERR_NOT_IMPL; }

    static std::shared_ptr<ISpxRecognizer> CreateIntentRecognizer();
    static std::shared_ptr<ISpxRecognizer> CreateIntentRecognizer(bool passiveListeningEnabled) { UNUSED(passiveListeningEnabled); throw SPXERR_NOT_IMPL; }
    static std::shared_ptr<ISpxRecognizer> CreateIntentRecognizer(const std::wstring& language) { UNUSED(language);  throw SPXERR_NOT_IMPL; }
    static std::shared_ptr<ISpxRecognizer> CreateIntentRecognizerWithFileInput(const std::wstring& fileName);
    static std::shared_ptr<ISpxRecognizer> CreateIntentRecognizerWithFileInput(const std::wstring& fileName, const std::wstring& language) { UNUSED(fileName); UNUSED(language); throw SPXERR_NOT_IMPL; }


private:

    CSpxRecognizerFactory() = delete;
    CSpxRecognizerFactory(const CSpxRecognizerFactory&) = delete;
    CSpxRecognizerFactory(const CSpxRecognizerFactory&&) = delete;

    static std::shared_ptr<ISpxRecognizerFactory> GetDefaultFactory();
};


} // CARBON_IMPL_NAMESPACE
