#include "stdafx.h"
#include "spxcore_common.h"
#include "create_object_helpers.h"
#include "recognizer_factory.h"
#include "resource_manager.h"


namespace CARBON_IMPL_NAMESPACE() {


std::shared_ptr<ISpxRecognizer> CSpxRecognizerFactory::CreateSpeechRecognizer() 
{
    auto factory = GetDefaultFactory();
    return factory->CreateSpeechRecognizer();
}

std::shared_ptr<ISpxRecognizer> CSpxRecognizerFactory::CreateSpeechRecognizer(const std::wstring& language)
{
    auto factory = GetDefaultFactory();
    return factory->CreateSpeechRecognizer(language);
}

std::shared_ptr<ISpxRecognizer> CSpxRecognizerFactory::CreateSpeechRecognizerWithFileInput(const std::wstring& fileName)
{
    auto factory = GetDefaultFactory();
    return factory->CreateSpeechRecognizerWithFileInput(fileName);
}

std::shared_ptr<ISpxRecognizer> CSpxRecognizerFactory::CreateSpeechRecognizerWithFileInput(const std::wstring& fileName, const std::wstring& language)
{
    auto factory = GetDefaultFactory();
    return factory->CreateSpeechRecognizerWithFileInput(fileName, language);
}

std::shared_ptr<ISpxRecognizer> CSpxRecognizerFactory::CreateIntentRecognizer() 
{
    auto factory = GetDefaultFactory();
    return factory->CreateIntentRecognizer();
}

std::shared_ptr<ISpxRecognizer> CSpxRecognizerFactory::CreateIntentRecognizerWithFileInput(const std::wstring& fileName)
{
    auto factory = GetDefaultFactory();
    return factory->CreateIntentRecognizerWithFileInput(fileName);
}

std::shared_ptr<ISpxRecognizer> CSpxRecognizerFactory::CreateSpeechRecognizerWithStream(AudioInputStream* audioInputStream)
{
    auto factory = GetDefaultFactory();
    return factory->CreateSpeechRecognizerWithStream(audioInputStream);
}

std::shared_ptr<ISpxRecognizerFactory> CSpxRecognizerFactory::GetDefaultFactory()
{
    auto factory = CSpxResourceManager::InitService<ISpxRecognizerFactory>("CSpxDefaultRecognizerFactory");
    return factory;
}

std::shared_ptr<ISpxRecognizer> CSpxRecognizerFactory::CreateTranslationRecognizer(const std::wstring& sourceLanguage, const std::wstring& targetLanguage)
{
    UNUSED(sourceLanguage);
    UNUSED(targetLanguage);
    SPX_THROW_HR(SPXERR_NOT_IMPL);
    auto factory = GetDefaultFactory();
    return  nullptr; //factory->CreateSpeechRecognizer(language);

}

std::shared_ptr<ISpxRecognizer> CSpxRecognizerFactory::CreateTranslationRecognizerWithFileInput(const std::wstring& fileName, const std::wstring& sourceLanguage, const std::wstring& targetLanguage)
{
    UNUSED(fileName);
    UNUSED(sourceLanguage);
    UNUSED(targetLanguage);
    SPX_THROW_HR(SPXERR_NOT_IMPL);
    auto factory = GetDefaultFactory();
    return nullptr; // factory->CreateSpeechRecognizerWithFileInput(fileName);
}

std::shared_ptr<ISpxRecognizer> CreateTranslationRecognizerWithStream(AudioInputStream *stream, const std::wstring& sourceLanguage, const std::wstring& targetLanguage)
{
    UNUSED(sourceLanguage);
    UNUSED(targetLanguage);
    UNUSED(stream);
    SPX_THROW_HR(SPXERR_NOT_IMPL);
    return nullptr;
}



} // CARBON_IMPL_NAMESPACE
