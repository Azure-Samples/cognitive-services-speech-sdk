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

std::shared_ptr<ISpxRecognizer> CSpxRecognizerFactory::CreateSpeechRecognizerWithFileInput(const std::wstring& fileName)
{
    auto factory = GetDefaultFactory();
    return factory->CreateSpeechRecognizerWithFileInput(fileName);
}

std::shared_ptr<ISpxRecognizerFactory> CSpxRecognizerFactory::GetDefaultFactory()
{
    auto factory = CSpxResourceManager::InitService<ISpxRecognizerFactory>("CSpxDefaultRecognizerFactory");
    return factory;
}


} // CARBON_IMPL_NAMESPACE
