//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speech_api_factory.h: Implementation declarations for CSpxSpeechApiFactory C++ class
//

#pragma once
#include <spxdebug.h>
#include <spxerror.h>
#include "service_helpers.h"
#include <speechapi_cxx_audioinputstream.h>
#include "interface_helpers.h"
#include "named_properties_impl.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


class CSpxSpeechApiFactory :
    public ISpxObjectWithSiteInitImpl<ISpxGenericSite>,
    public ISpxServiceProvider,
    public ISpxSpeechApiFactory,
    public ISpxGenericSite,
    public ISpxNamedPropertiesImpl
{
public:

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxInterfaceBase)
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectWithSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxServiceProvider)
        SPX_INTERFACE_MAP_ENTRY(ISpxSpeechApiFactory)
        SPX_INTERFACE_MAP_ENTRY(ISpxGenericSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxNamedProperties)
    SPX_INTERFACE_MAP_END()

    // --- ISpxSpeechApiFactory
    std::shared_ptr<ISpxRecognizer> CreateSpeechRecognizer() override;
    std::shared_ptr<ISpxRecognizer> CreateSpeechRecognizerWithStream(AudioInputStream* audioInputStream) override;
    std::shared_ptr<ISpxRecognizer> CreateSpeechRecognizer(const std::wstring& language) override;
    std::shared_ptr<ISpxRecognizer> CreateSpeechRecognizerWithFileInput(const std::wstring& fileName) override;
    std::shared_ptr<ISpxRecognizer> CreateSpeechRecognizerWithFileInput(const std::wstring& fileName, const std::wstring& language) override;

    std::shared_ptr<ISpxRecognizer> CreateIntentRecognizer() override;
    std::shared_ptr<ISpxRecognizer> CreateIntentRecognizer(const std::wstring& language) override;
    std::shared_ptr<ISpxRecognizer> CreateIntentRecognizerWithFileInput(const std::wstring& fileName) override;
    std::shared_ptr<ISpxRecognizer> CreateIntentRecognizerWithFileInput(const std::wstring& fileName, const std::wstring& language) override;


    std::shared_ptr<ISpxRecognizer> CreateTranslationRecognizer(const std::wstring& sourceLanguage, const std::vector<std::wstring>& targetLanguages, const std::wstring& voice = L"") override;
    std::shared_ptr<ISpxRecognizer> CreateTranslationRecognizerWithFileInput(const std::wstring& fileName, const std::wstring& sourceLanguage, const std::vector<std::wstring>& targetLanguages, const std::wstring& voice = L"") override;
    std::shared_ptr<ISpxRecognizer> CreateTranslationRecognizerWithStream(AudioInputStream *stream, const std::wstring& sourceLanguage, const std::vector<std::wstring>& targetLanguages, const std::wstring& voice = L"") override;

    // --- IServiceProvider
    SPX_SERVICE_MAP_BEGIN()
    SPX_SERVICE_MAP_ENTRY(ISpxNamedProperties)
    SPX_SERVICE_MAP_ENTRY_SITE(GetSite())
    SPX_SERVICE_MAP_END()


protected:

    std::shared_ptr<ISpxRecognizer> CreateRecognizerInternal(const char* sessionClassName, const char* recognizerClassName, const wchar_t* fileName = nullptr, const wchar_t* language = nullptr);
    std::shared_ptr<ISpxNamedProperties> GetParentProperties() override { return SpxQueryService<ISpxNamedProperties>(GetSite()); }

private:

    std::shared_ptr<ISpxRecognizer> CreateTranslationRecognizerInternal(wchar_t const* fileNameStr, const std::wstring& sourceLanguage, const std::vector<std::wstring>& targetLanguages, const std::wstring& voice);
    void SetTranslationParameter(const std::shared_ptr<ISpxNamedProperties>& namedProperties, const std::wstring& sourceLanguage, const std::vector<std::wstring>& targetLanguages, const std::wstring& voice);

};


} } } } // Microsoft::CognitiveServices::Speech::Impl
