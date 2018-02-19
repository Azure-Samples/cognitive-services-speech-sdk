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


namespace CARBON_IMPL_NAMESPACE() {


class CSpxDefaultRecognizerFactory : 
    public ISpxObjectWithSiteInitImpl<ISpxSite>,
    public ISpxServiceProvider,
    public ISpxRecognizerFactory,
    public ISpxSite
{
public:

    // --- ISpxRecognizerFactory
    std::shared_ptr<ISpxRecognizer> CreateSpeechRecognizer() override;
    std::shared_ptr<ISpxRecognizer> CreateSpeechRecognizer(bool passiveListeningEnabled) override { UNUSED(passiveListeningEnabled); throw SPXERR_NOT_IMPL; }
    std::shared_ptr<ISpxRecognizer> CreateSpeechRecognizer(const std::wstring& language) override { UNUSED(language);  throw SPXERR_NOT_IMPL; }
    std::shared_ptr<ISpxRecognizer> CreateSpeechRecognizerWithFileInput(const std::wstring& fileName) override;
    std::shared_ptr<ISpxRecognizer> CreateSpeechRecognizerWithFileInput(const std::wstring& fileName, const std::wstring& language) override { UNUSED(fileName); UNUSED(language); throw SPXERR_NOT_IMPL; }

    // --- IServiceProvider
    SPX_SERVICE_MAP_BEGIN()
    SPX_SERVICE_MAP_ENTRY_SITE(GetSite())
    SPX_SERVICE_MAP_END()

};


} // CARBON_IMPL_NAMESPACE
