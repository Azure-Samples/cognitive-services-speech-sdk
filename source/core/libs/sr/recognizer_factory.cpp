#include "stdafx.h"
#include "recognizer_factory.h"
#include "audio_stream_session.h"
#include "interactive_session.h"


namespace CARBON_IMPL_NAMESPACE() {


std::shared_ptr<ISpxRecognizer> CSpxRecognizerFactory::CreateSpeechRecognizer() 
{
    auto session = std::make_shared<CSpxInteractiveSession>();

    auto pISpxSession = std::dynamic_pointer_cast<ISpxSession>(session);
    auto recognizer = std::make_shared<CSpxRecognizer>(pISpxSession);

    auto pISpxRecognizer = std::dynamic_pointer_cast<ISpxRecognizer>(recognizer);
    session->AddRecognizer(recognizer);

    return pISpxRecognizer;
}

std::shared_ptr<ISpxRecognizer> CSpxRecognizerFactory::CreateSpeechRecognizer(bool passiveListeningEnaled)
{
    throw nullptr;
}

std::shared_ptr<ISpxRecognizer> CSpxRecognizerFactory::CreateSpeechRecognizer(const std::wstring& language)
{
    throw nullptr;
}

std::shared_ptr<ISpxRecognizer> CSpxRecognizerFactory::CreateSpeechRecognizerWithFileInput(const std::wstring& fileName)
{
    auto session = std::make_shared<CSpxAudioStreamSession>();

    auto pISpxAudioStreamSessionInit = std::dynamic_pointer_cast<ISpxAudioStreamSessionInit>(session);
    pISpxAudioStreamSessionInit->InitFromFile(fileName.c_str());

    auto pISpxSession = std::dynamic_pointer_cast<ISpxSession>(session);
    auto recognizer = std::make_shared<CSpxRecognizer>(pISpxSession);
    
    auto pISpxRecognizer = std::dynamic_pointer_cast<ISpxRecognizer>(recognizer);
    pISpxSession->AddRecognizer(pISpxRecognizer);

    return pISpxRecognizer;
}

std::shared_ptr<ISpxRecognizer> CSpxRecognizerFactory::CreateSpeechRecognizerWithFileInput(const std::wstring& fileName, const std::wstring& language)
{
    throw nullptr;
}

std::shared_ptr<ISpxRecognizer> CSpxRecognizerFactory::CreateDictationRecognizer() 
{
    throw nullptr;
}

std::shared_ptr<ISpxRecognizer> CSpxRecognizerFactory::CreateDictationRecognizer(bool passiveListeningEnaled)
{
    throw nullptr;
}

std::shared_ptr<ISpxRecognizer> CSpxRecognizerFactory::CreateDictationRecognizer(const std::wstring& language)
{
    throw nullptr;
}

std::shared_ptr<ISpxRecognizer> CSpxRecognizerFactory::CreateDictationRecognizerWithFileInput(const std::wstring& fileName)
{
    throw nullptr;
}

std::shared_ptr<ISpxRecognizer> CSpxRecognizerFactory::CreateDictationRecognizerWithFileInput(const std::wstring& fileName, const std::wstring& language)
{
    throw nullptr;
}

std::shared_ptr<ISpxRecognizer> CSpxRecognizerFactory::CreateIntentRecognizer() 
{
    throw nullptr;
}

std::shared_ptr<ISpxRecognizer> CSpxRecognizerFactory::CreateIntentRecognizer(bool passiveListeningEnaled)
{
    throw nullptr;
}

std::shared_ptr<ISpxRecognizer> CSpxRecognizerFactory::CreateIntentRecognizer(const std::wstring& language)
{
    throw nullptr;
}

std::shared_ptr<ISpxRecognizer> CSpxRecognizerFactory::CreateIntentRecognizerWithFileInput(const std::wstring& fileName)
{
    throw nullptr;
}

std::shared_ptr<ISpxRecognizer> CSpxRecognizerFactory::CreateIntentRecognizerWithFileInput(const std::wstring& fileName, const std::wstring& language)
{
    throw nullptr;
}


}; // CARBON_IMPL_NAMESPACE()

