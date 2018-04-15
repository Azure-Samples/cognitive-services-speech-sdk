//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// default_recognizer_factory.h: Implementation declarations for CSpxDefaultRecognizerFactory C++ class
//

#pragma once
#include <spxdebug.h>
#include <spxerror.h>
#include "service_helpers.h"
#include <speechapi_cxx_audioinputstream.h>
#include "interface_helpers.h"


namespace CARBON_IMPL_NAMESPACE() {


class CSpxDefaultRecognizerFactory :
    public ISpxObjectWithSiteInitImpl<ISpxGenericSite>,
    public ISpxServiceProvider,
    public ISpxRecognizerFactory,
    public ISpxGenericSite
{
public:

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxInterfaceBase)
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectWithSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxServiceProvider)
        SPX_INTERFACE_MAP_ENTRY(ISpxRecognizerFactory)
        SPX_INTERFACE_MAP_ENTRY(ISpxGenericSite)
    SPX_INTERFACE_MAP_END()

    // --- ISpxRecognizerFactory
    std::shared_ptr<ISpxRecognizer> CreateSpeechRecognizer() override;
    std::shared_ptr<ISpxRecognizer> CreateSpeechRecognizerWithStream(AudioInputStream* audioInputStream) override;
    std::shared_ptr<ISpxRecognizer> CreateSpeechRecognizer(const std::wstring& language) override;
    std::shared_ptr<ISpxRecognizer> CreateSpeechRecognizer(bool passiveListeningEnabled) override { UNUSED(passiveListeningEnabled); throw SPXERR_NOT_IMPL; }
    std::shared_ptr<ISpxRecognizer> CreateSpeechRecognizerWithFileInput(const std::wstring& fileName) override;
    std::shared_ptr<ISpxRecognizer> CreateSpeechRecognizerWithFileInput(const std::wstring& fileName, const std::wstring& language) override;

    std::shared_ptr<ISpxRecognizer> CreateIntentRecognizer() override;
    std::shared_ptr<ISpxRecognizer> CreateIntentRecognizer(bool passiveListeningEnabled) override { UNUSED(passiveListeningEnabled); throw SPXERR_NOT_IMPL; }
    std::shared_ptr<ISpxRecognizer> CreateIntentRecognizer(const std::wstring& language) override { UNUSED(language); throw SPXERR_NOT_IMPL; };
    std::shared_ptr<ISpxRecognizer> CreateIntentRecognizerWithFileInput(const std::wstring& fileName) override;
    std::shared_ptr<ISpxRecognizer> CreateIntentRecognizerWithFileInput(const std::wstring& fileName, const std::wstring& language) override { UNUSED(fileName); UNUSED(language); throw SPXERR_NOT_IMPL; }

    std::shared_ptr<ISpxRecognizer> CreateTranslationRecognizer(const std::wstring& sourceLanguage, const std::wstring& targetLanguage) override;
    std::shared_ptr<ISpxRecognizer> CreateTranslationRecognizerWithFileInput(const std::wstring& fileName, const std::wstring& sourceLanguage, const std::wstring& targetLanguage) override;
    std::shared_ptr<ISpxRecognizer> CreateTranslationRecognizerWithStream(AudioInputStream *stream, const std::wstring& sourceLanguage, const std::wstring& targetLanguage) override;

    // --- IServiceProvider
    SPX_SERVICE_MAP_BEGIN()
    SPX_SERVICE_MAP_ENTRY_SITE(GetSite())
    SPX_SERVICE_MAP_END()


protected:

    std::shared_ptr<ISpxRecognizer> CreateRecognizerInternal(const char* sessionClassName, const char* recognizerClassName, wchar_t const* fileName = nullptr, wchar_t const* language = nullptr);

private:

    std::shared_ptr<ISpxRecognizer> CreateTranslationRecognizerInternal(wchar_t const* fileNameStr, const std::wstring& sourceLanguage, const std::wstring& targetLanguage);

};


} // CARBON_IMPL_NAMESPACE
