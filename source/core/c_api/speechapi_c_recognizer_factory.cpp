//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_c_recognizer_factory.cpp: Definitions for RecognizerFactory related C methods
//

#include "stdafx.h"
#include "service_helpers.h"
#include "site_helpers.h"


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

SPXAPI RecognizerFactory_CreateSpeechRecognizer(SPXRECOHANDLE* phreco, const wchar_t* pszLanguage, const wchar_t* pszFileName)
{
    UNUSED(phreco);
    UNUSED(pszLanguage);
    UNUSED(pszFileName);
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

SPXAPI RecognizerFactory_CreateIntentRecognizer(SPXRECOHANDLE* phreco, const wchar_t* pszLanguage, const wchar_t* pszFileName)
{
    UNUSED(phreco);
    UNUSED(pszLanguage);
    UNUSED(pszFileName);
    return SPXERR_NOT_IMPL;
}

SPXAPI RecognizerFactory_SetParameter_String(const wchar_t* name, const wchar_t* value)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto namedProperties = SpxQueryService<ISpxNamedProperties>(SpxGetRootSite());
        namedProperties->SetStringValue(name, value);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI RecognizerFactory_GetParameter_String(const wchar_t* name, wchar_t* value, uint32_t cchValue, const wchar_t* defaultValue)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto namedProperties = SpxQueryService<ISpxNamedProperties>(SpxGetRootSite());
        auto tempValue = namedProperties->GetStringValue(name, defaultValue);
        PAL::wcscpy(value, cchValue, tempValue.c_str(), tempValue.size(), true);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI_(bool) RecognizerFactory_HasParameter_String(const wchar_t* name)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto namedProperties = SpxQueryService<ISpxNamedProperties>(SpxGetRootSite());
        return namedProperties->HasStringValue(name);
    }
    SPXAPI_CATCH_AND_RETURN(hr, false)
}

SPXAPI RecognizerFactory_SetParameter_Int32(const wchar_t* name, int32_t value)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto namedProperties = SpxQueryService<ISpxNamedProperties>(SpxGetRootSite());
        namedProperties->SetNumberValue(name, value);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI RecognizerFactory_GetParameter_Int32(const wchar_t* name, int32_t* pvalue, int32_t defaultValue)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto namedProperties = SpxQueryService<ISpxNamedProperties>(SpxGetRootSite());
        auto tempValue = namedProperties->GetNumberValue(name, defaultValue);
        *pvalue = (int32_t)tempValue;
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI_(bool) RecognizerFactory_HasParameter_Int32(const wchar_t* name)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto namedProperties = SpxQueryService<ISpxNamedProperties>(SpxGetRootSite());
        return namedProperties->HasNumberValue(name);
    }
    SPXAPI_CATCH_AND_RETURN(hr, false)
}

SPXAPI RecognizerFactory_SetParameter_Bool(const wchar_t* name, bool value)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto namedProperties = SpxQueryService<ISpxNamedProperties>(SpxGetRootSite());
        namedProperties->SetBooleanValue(name, value);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI RecognizerFactory_GetParameter_Bool(const wchar_t* name, bool* pvalue, bool defaultValue)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto namedProperties = SpxQueryService<ISpxNamedProperties>(SpxGetRootSite());
        auto tempValue = namedProperties->GetBooleanValue(name, defaultValue);
        *pvalue = tempValue;
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI_(bool) RecognizerFactory_HasParameter_Bool(const wchar_t* name)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto namedProperties = SpxQueryService<ISpxNamedProperties>(SpxGetRootSite());
        return namedProperties->HasBooleanValue(name);
    }
    SPXAPI_CATCH_AND_RETURN(hr, false)
}
