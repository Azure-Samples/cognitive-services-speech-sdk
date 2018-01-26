#include "stdafx.h"
#include "recognizer_factory.h"
#include "audio_stream_session.h"


namespace CARBON_IMPL_NAMESPACE() {


std::shared_ptr<ISpxRecognizer> CSpxRecognizerFactory::CreateSpeechRecognizer() 
{
    // Create the sesion and initialize it
    auto session = SpxMakeShared<CSpxAudioStreamSession, ISpxSession>();
    auto sessionInit = std::dynamic_pointer_cast<ISpxAudioStreamSessionInit>(session);
    sessionInit->InitFromMicrophone();

    // Create the recognizer and add it to the session
    auto recognizer = SpxMakeShared<CSpxRecognizer, ISpxRecognizer>(session);
    session->AddRecognizer(recognizer);

    return recognizer;
}

std::shared_ptr<ISpxRecognizer> CSpxRecognizerFactory::CreateSpeechRecognizer(bool passiveListeningEnaled)
{
    UNUSED(passiveListeningEnaled);
    throw nullptr;
}

std::shared_ptr<ISpxRecognizer> CSpxRecognizerFactory::CreateSpeechRecognizer(const std::wstring& language)
{
    UNUSED(language);
    throw nullptr;
}

std::shared_ptr<ISpxRecognizer> CSpxRecognizerFactory::CreateSpeechRecognizerWithFileInput(const std::wstring& fileName)
{
    // Create the session and initialize it
    auto session = SpxMakeShared<CSpxAudioStreamSession, ISpxSession>();
    auto sessionInit = std::dynamic_pointer_cast<ISpxAudioStreamSessionInit>(session);
    sessionInit->InitFromFile(fileName.c_str());

    // Create the recognizer and add it to the session
    auto recognizer = SpxMakeShared<CSpxRecognizer, ISpxRecognizer>(session);
    session->AddRecognizer(recognizer);

    return recognizer;
}

std::shared_ptr<ISpxRecognizer> CSpxRecognizerFactory::CreateSpeechRecognizerWithFileInput(const std::wstring& fileName, const std::wstring& language)
{
    UNUSED(fileName);
    UNUSED(language);
    throw nullptr;
}

std::shared_ptr<ISpxRecognizer> CSpxRecognizerFactory::CreateDictationRecognizer() 
{
    throw nullptr;
}

std::shared_ptr<ISpxRecognizer> CSpxRecognizerFactory::CreateDictationRecognizer(bool passiveListeningEnaled)
{
    UNUSED(passiveListeningEnaled);
    throw nullptr;
}

std::shared_ptr<ISpxRecognizer> CSpxRecognizerFactory::CreateDictationRecognizer(const std::wstring& language)
{
    UNUSED(language);
    throw nullptr;
}

std::shared_ptr<ISpxRecognizer> CSpxRecognizerFactory::CreateDictationRecognizerWithFileInput(const std::wstring& fileName)
{
    UNUSED(fileName);
    throw nullptr;
}

std::shared_ptr<ISpxRecognizer> CSpxRecognizerFactory::CreateDictationRecognizerWithFileInput(const std::wstring& fileName, const std::wstring& language)
{
    UNUSED(fileName);
    UNUSED(language);
    throw nullptr;
}

std::shared_ptr<ISpxRecognizer> CSpxRecognizerFactory::CreateIntentRecognizer() 
{
    throw nullptr;
}

std::shared_ptr<ISpxRecognizer> CSpxRecognizerFactory::CreateIntentRecognizer(bool passiveListeningEnaled)
{
    UNUSED(passiveListeningEnaled);
    throw nullptr;
}

std::shared_ptr<ISpxRecognizer> CSpxRecognizerFactory::CreateIntentRecognizer(const std::wstring& language)
{
    UNUSED(language);
    throw nullptr;
}

std::shared_ptr<ISpxRecognizer> CSpxRecognizerFactory::CreateIntentRecognizerWithFileInput(const std::wstring& fileName)
{
    UNUSED(fileName);
    throw nullptr;
}

std::shared_ptr<ISpxRecognizer> CSpxRecognizerFactory::CreateIntentRecognizerWithFileInput(const std::wstring& fileName, const std::wstring& language)
{
    UNUSED(fileName);
    UNUSED(language);
    throw nullptr;
}


} // CARBON_IMPL_NAMESPACE()

