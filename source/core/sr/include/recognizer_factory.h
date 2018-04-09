#pragma once
#include <spxdebug.h>
#include <spxerror.h>
#include <speechapi_cxx_audioinputstream.h>


namespace CARBON_IMPL_NAMESPACE() {


class CSpxRecognizerFactory
{
public:

    static std::shared_ptr<ISpxRecognizer> CreateSpeechRecognizer();
    static std::shared_ptr<ISpxRecognizer> CreateSpeechRecognizerWithStream(AudioInputStream *stream);
    static std::shared_ptr<ISpxRecognizer> CreateSpeechRecognizer(const std::wstring& language);
    static std::shared_ptr<ISpxRecognizer> CreateSpeechRecognizerWithFileInput(const std::wstring& fileName);
    static std::shared_ptr<ISpxRecognizer> CreateSpeechRecognizerWithFileInput(const std::wstring& fileName, const std::wstring& language);
    
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

    static std::shared_ptr<ISpxRecognizer> CreateTranslationRecognizer(const std::wstring& sourceLanguage, const std::wstring& targetLanguage);
    static std::shared_ptr<ISpxRecognizer> CreateTranslationRecognizerWithFileInput(const std::wstring& fileName, const std::wstring& sourceLanguage, const std::wstring& targetLanguage);
    static std::shared_ptr<ISpxRecognizer> CreateTranslationRecognizerWithStream(AudioInputStream *stream, const std::wstring& sourceLanguage, const std::wstring& targetLanguage);

private:

    CSpxRecognizerFactory() = delete;
    CSpxRecognizerFactory(const CSpxRecognizerFactory&) = delete;
    CSpxRecognizerFactory(const CSpxRecognizerFactory&&) = delete;

    static std::shared_ptr<ISpxRecognizerFactory> GetDefaultFactory();
};


} // CARBON_IMPL_NAMESPACE
