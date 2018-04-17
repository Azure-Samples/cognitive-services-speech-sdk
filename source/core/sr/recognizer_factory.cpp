#include "stdafx.h"
#include "spxcore_common.h"
#include "create_object_helpers.h"
#include "recognizer_factory.h"
#include "resource_manager.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


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
    auto factory = GetDefaultFactory();
    return factory->CreateTranslationRecognizer(sourceLanguage, targetLanguage);
}

std::shared_ptr<ISpxRecognizer> CSpxRecognizerFactory::CreateTranslationRecognizerWithFileInput(const std::wstring& fileName, const std::wstring& sourceLanguage, const std::wstring& targetLanguage)
{
    auto factory = GetDefaultFactory();
    return factory->CreateTranslationRecognizerWithFileInput(fileName, sourceLanguage, targetLanguage);
}

std::shared_ptr<ISpxRecognizer> CSpxRecognizerFactory::CreateTranslationRecognizerWithStream(AudioInputStream *stream, const std::wstring& sourceLanguage, const std::wstring& targetLanguage)
{
    auto factory = GetDefaultFactory();
    return factory->CreateTranslationRecognizerWithStream(stream, sourceLanguage, targetLanguage);
}



} } } } // Microsoft::CognitiveServices::Speech::Impl
