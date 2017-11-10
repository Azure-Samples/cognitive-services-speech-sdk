#include "stdafx.h"
#include "recognizer_factory.h"


namespace CARBON_IMPL_NAMESPACE() {


std::shared_ptr<CSpxRecognizer> CSpxRecognizerFactory::CreateSpeechRecognizer() 
{
    return std::make_shared<CSpxRecognizer>();
}

std::shared_ptr<CSpxRecognizer> CSpxRecognizerFactory::CreateSpeechRecognizer(bool passiveListeningEnaled)
{
    throw nullptr;
}

std::shared_ptr<CSpxRecognizer> CSpxRecognizerFactory::CreateSpeechRecognizer(const std::wstring& language)
{
    throw nullptr;
}

std::shared_ptr<CSpxRecognizer> CSpxRecognizerFactory::CreateSpeechRecognizerWithFileInput(const std::wstring& filename)
{
    throw nullptr;
}

std::shared_ptr<CSpxRecognizer> CSpxRecognizerFactory::CreateSpeechRecognizerWithFileInput(const std::wstring& filename, const std::wstring& language)
{
    throw nullptr;
}

std::shared_ptr<CSpxRecognizer> CSpxRecognizerFactory::CreateDictationRecognizer() 
{
    throw nullptr;
}

std::shared_ptr<CSpxRecognizer> CSpxRecognizerFactory::CreateDictationRecognizer(bool passiveListeningEnaled)
{
    throw nullptr;
}

std::shared_ptr<CSpxRecognizer> CSpxRecognizerFactory::CreateDictationRecognizer(const std::wstring& language)
{
    throw nullptr;
}

std::shared_ptr<CSpxRecognizer> CSpxRecognizerFactory::CreateDictationRecognizerWithFileInput(const std::wstring& filename)
{
    throw nullptr;
}

std::shared_ptr<CSpxRecognizer> CSpxRecognizerFactory::CreateDictationRecognizerWithFileInput(const std::wstring& filename, const std::wstring& language)
{
    throw nullptr;
}

std::shared_ptr<CSpxRecognizer> CSpxRecognizerFactory::CreateIntentRecognizer() 
{
    throw nullptr;
}

std::shared_ptr<CSpxRecognizer> CSpxRecognizerFactory::CreateIntentRecognizer(bool passiveListeningEnaled)
{
    throw nullptr;
}

std::shared_ptr<CSpxRecognizer> CSpxRecognizerFactory::CreateIntentRecognizer(const std::wstring& language)
{
    throw nullptr;
}

std::shared_ptr<CSpxRecognizer> CSpxRecognizerFactory::CreateIntentRecognizerWithFileInput(const std::wstring& filename)
{
    throw nullptr;
}

std::shared_ptr<CSpxRecognizer> CSpxRecognizerFactory::CreateIntentRecognizerWithFileInput(const std::wstring& filename, const std::wstring& language)
{
    throw nullptr;
}


}; // CARBON_IMPL_NAMESPACE()

