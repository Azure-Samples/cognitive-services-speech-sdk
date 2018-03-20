//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_c_recognizer_factory.cpp: Definitions for RecognizerFactory related C methods
//

#include "stdafx.h"


using namespace CARBON_IMPL_NAMESPACE();


SPXAPI RecognizerFactory_CreateSpeechRecognizer_With_Defaults(SPXRECOHANDLE* phreco)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        *phreco = SPXHANDLE_INVALID;
        auto recognizer = CSpxRecognizerFactory::CreateSpeechRecognizer();
        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        *phreco = recohandles->TrackHandle(recognizer);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI RecognizerFactory_CreateSpeechRecognizer_With_Language(SPXRECOHANDLE* phreco, const wchar_t* pszLanguage)
{
    UNUSED(phreco);
    UNUSED(pszLanguage);
    return SPXERR_NOT_IMPL;
}

SPXAPI RecognizerFactory_CreateSpeechRecognizer_With_FileInput(SPXRECOHANDLE* phreco, const wchar_t* pszFileName)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        *phreco = SPXHANDLE_INVALID;
        auto recognizer = CSpxRecognizerFactory::CreateSpeechRecognizerWithFileInput(pszFileName);
        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        *phreco = recohandles->TrackHandle(recognizer);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI RecognizerFactory_CreateSpeechRecognizer_With_PassiveListening(SPXRECOHANDLE* phreco, bool passive)
{
    UNUSED(phreco);
    UNUSED(passive);
    return SPXERR_NOT_IMPL;
}

SPXAPI RecognizerFactory_CreateSpeechRecognizer(SPXRECOHANDLE* phreco, const wchar_t* pszLanguage, const wchar_t* pszFileName, bool passive)
{
    UNUSED(phreco);
    UNUSED(pszLanguage);
    UNUSED(pszFileName);
    UNUSED(passive);
    return SPXERR_NOT_IMPL;
}

SPXAPI RecognizerFactory_CreateDictationRecognizer_With_Defaults(SPXRECOHANDLE* phreco)
{
    UNUSED(phreco);
    return SPXERR_NOT_IMPL;
}

SPXAPI RecognizerFactory_CreateDictationRecognizer_With_Language(SPXRECOHANDLE* phreco, const wchar_t* pszLanguage)
{
    UNUSED(phreco);
    UNUSED(pszLanguage);
    return SPXERR_NOT_IMPL;
}

SPXAPI RecognizerFactory_CreateDictationRecognizer_With_FileInput(SPXRECOHANDLE* phreco, const wchar_t* pszFileName)
{
    UNUSED(phreco);
    UNUSED(pszFileName);
    return SPXERR_NOT_IMPL;
}

SPXAPI RecognizerFactory_CreateDictationRecognizer_With_PassiveListening(SPXRECOHANDLE* phreco, bool passive)
{
    UNUSED(phreco);
    UNUSED(passive);
    return SPXERR_NOT_IMPL;
}

SPXAPI RecognizerFactory_CreateDictationRecognizer(SPXRECOHANDLE* phreco, const wchar_t* pszLanguage, const wchar_t* pszFileName, bool passive)
{
    UNUSED(phreco);
    UNUSED(pszLanguage);
    UNUSED(pszFileName);
    UNUSED(passive);
    return SPXERR_NOT_IMPL;
}

SPXAPI RecognizerFactory_CreateIntentRecognizer_With_Defaults(SPXRECOHANDLE* phreco)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        *phreco = SPXHANDLE_INVALID;
        auto recognizer = CSpxRecognizerFactory::CreateIntentRecognizer();
        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        *phreco = recohandles->TrackHandle(recognizer);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI RecognizerFactory_CreateIntentRecognizer_With_Language(SPXRECOHANDLE* phreco, const wchar_t* pszLanguage)
{
    UNUSED(phreco);
    UNUSED(pszLanguage);
    return SPXERR_NOT_IMPL;
}

SPXAPI RecognizerFactory_CreateIntentRecognizer_With_FileInput(SPXRECOHANDLE* phreco, const wchar_t* pszFileName)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        *phreco = SPXHANDLE_INVALID;
        auto recognizer = CSpxRecognizerFactory::CreateIntentRecognizerWithFileInput(pszFileName);
        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        *phreco = recohandles->TrackHandle(recognizer);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI RecognizerFactory_CreateIntentRecognizer_With_PassiveListening(SPXRECOHANDLE* phreco, bool passive)
{
    UNUSED(phreco);
    UNUSED(passive);
    return SPXERR_NOT_IMPL;
}

SPXAPI RecognizerFactory_CreateIntentRecognizer(SPXRECOHANDLE* phreco, const wchar_t* pszLanguage, const wchar_t* pszFileName, bool passive)
{
    UNUSED(phreco);
    UNUSED(pszLanguage);
    UNUSED(pszFileName);
    UNUSED(passive);
    return SPXERR_NOT_IMPL;
}
