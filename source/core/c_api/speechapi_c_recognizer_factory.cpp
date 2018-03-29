//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_c_recognizer_factory.cpp: Definitions for RecognizerFactory related C methods
//

#include "stdafx.h"
#include "service_helpers.h"
#include "site_helpers.h"
#include "named_properties_constants.h"


using namespace CARBON_IMPL_NAMESPACE();


SPXAPI RecognizerFactory_CreateSpeechRecognizer_With_Defaults(SPXRECOFACTORYHANDLE hrecofactory, SPXRECOHANDLE* phreco)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        SPX_IFTRUE_THROW_HR(hrecofactory != SPXHANDLE_DEFAULT, SPXERR_INVALID_ARG);
        *phreco = SPXHANDLE_INVALID;

        auto recognizer = CSpxRecognizerFactory::CreateSpeechRecognizer();
        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        *phreco = recohandles->TrackHandle(recognizer);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI RecognizerFactory_CreateSpeechRecognizer_With_Language(SPXRECOFACTORYHANDLE hrecofactory, SPXRECOHANDLE* phreco, const wchar_t* pszLanguage)
{
    SPX_IFTRUE_RETURN_HR(hrecofactory != SPXHANDLE_DEFAULT, SPXERR_INVALID_ARG);

    UNUSED(phreco);
    UNUSED(pszLanguage);
    return SPXERR_NOT_IMPL;
}

SPXAPI RecognizerFactory_CreateSpeechRecognizer_With_FileInput(SPXRECOFACTORYHANDLE hrecofactory, SPXRECOHANDLE* phreco, const wchar_t* pszFileName)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        SPX_IFTRUE_THROW_HR(hrecofactory != SPXHANDLE_DEFAULT, SPXERR_INVALID_ARG);
        *phreco = SPXHANDLE_INVALID;

        auto recognizer = CSpxRecognizerFactory::CreateSpeechRecognizerWithFileInput(pszFileName);
        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        *phreco = recohandles->TrackHandle(recognizer);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI RecognizerFactory_CreateSpeechRecognizer(SPXRECOFACTORYHANDLE hrecofactory, SPXRECOHANDLE* phreco, const wchar_t* pszLanguage, const wchar_t* pszFileName)
{
    SPX_IFTRUE_RETURN_HR(hrecofactory != SPXHANDLE_DEFAULT, SPXERR_INVALID_ARG);

    UNUSED(phreco);
    UNUSED(pszLanguage);
    UNUSED(pszFileName);
    return SPXERR_NOT_IMPL;
}

SPXAPI RecognizerFactory_CreateIntentRecognizer_With_Defaults(SPXRECOFACTORYHANDLE hrecofactory, SPXRECOHANDLE* phreco)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        SPX_IFTRUE_THROW_HR(hrecofactory != SPXHANDLE_DEFAULT, SPXERR_INVALID_ARG);
        *phreco = SPXHANDLE_INVALID;

        auto recognizer = CSpxRecognizerFactory::CreateIntentRecognizer();
        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        *phreco = recohandles->TrackHandle(recognizer);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI RecognizerFactory_CreateIntentRecognizer_With_Language(SPXRECOFACTORYHANDLE hrecofactory, SPXRECOHANDLE* phreco, const wchar_t* pszLanguage)
{
    SPX_IFTRUE_RETURN_HR(hrecofactory != SPXHANDLE_DEFAULT, SPXERR_INVALID_ARG);

    UNUSED(phreco);
    UNUSED(pszLanguage);
    return SPXERR_NOT_IMPL;
}

SPXAPI RecognizerFactory_CreateIntentRecognizer_With_FileInput(SPXRECOFACTORYHANDLE hrecofactory, SPXRECOHANDLE* phreco, const wchar_t* pszFileName)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        SPX_IFTRUE_THROW_HR(hrecofactory != SPXHANDLE_DEFAULT, SPXERR_INVALID_ARG);
        *phreco = SPXHANDLE_INVALID;

        auto recognizer = CSpxRecognizerFactory::CreateIntentRecognizerWithFileInput(pszFileName);
        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        *phreco = recohandles->TrackHandle(recognizer);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI RecognizerFactory_CreateIntentRecognizer(SPXRECOFACTORYHANDLE hrecofactory, SPXRECOHANDLE* phreco, const wchar_t* pszLanguage, const wchar_t* pszFileName)
{
    SPX_IFTRUE_RETURN_HR(hrecofactory != SPXHANDLE_DEFAULT, SPXERR_INVALID_ARG);

    UNUSED(phreco);
    UNUSED(pszLanguage);
    UNUSED(pszFileName);
    return SPXERR_NOT_IMPL;
}

SPXAPI RecognizerFactory_GetParameter_Name(Factory_Parameter parameter, wchar_t* name, uint32_t cchName)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        const wchar_t* parameterName = L"";
        switch (parameter)
        {
            case FactoryParameter_SpeechSubscriptionKey:
                parameterName = g_SPEECH_SubscriptionKey;
                break;

            case FactoryParameter_SpeechEndpoint:
                parameterName = g_SPEECH_Endpoint;
                break;

            default:
                hr = SPXERR_INVALID_ARG;
                break;
        }

        PAL::wcscpy(name, cchName, parameterName, wcslen(parameterName), true);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI RecognizerFactory_SetParameter_String(SPXRECOFACTORYHANDLE hrecofactory, const wchar_t* name, const wchar_t* value)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        SPX_IFTRUE_THROW_HR(hrecofactory != SPXHANDLE_DEFAULT, SPXERR_INVALID_ARG);

        auto namedProperties = SpxQueryService<ISpxNamedProperties>(SpxGetRootSite());
        namedProperties->SetStringValue(name, value);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI RecognizerFactory_GetParameter_String(SPXRECOFACTORYHANDLE hrecofactory, const wchar_t* name, wchar_t* value, uint32_t cchValue, const wchar_t* defaultValue)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        SPX_IFTRUE_THROW_HR(hrecofactory != SPXHANDLE_DEFAULT, SPXERR_INVALID_ARG);

        auto namedProperties = SpxQueryService<ISpxNamedProperties>(SpxGetRootSite());
        auto tempValue = namedProperties->GetStringValue(name, defaultValue);
        PAL::wcscpy(value, cchValue, tempValue.c_str(), tempValue.size(), true);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI_(bool) RecognizerFactory_ContainsParameter_String(SPXRECOFACTORYHANDLE hrecofactory, const wchar_t* name)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        SPX_IFTRUE_THROW_HR(hrecofactory != SPXHANDLE_DEFAULT, SPXERR_INVALID_ARG);

        auto namedProperties = SpxQueryService<ISpxNamedProperties>(SpxGetRootSite());
        return namedProperties->HasStringValue(name);
    }
    SPXAPI_CATCH_AND_RETURN(hr, false)
}

SPXAPI RecognizerFactory_SetParameter_Int32(SPXRECOFACTORYHANDLE hrecofactory, const wchar_t* name, int32_t value)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        SPX_IFTRUE_THROW_HR(hrecofactory != SPXHANDLE_DEFAULT, SPXERR_INVALID_ARG);

        auto namedProperties = SpxQueryService<ISpxNamedProperties>(SpxGetRootSite());
        namedProperties->SetNumberValue(name, value);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI RecognizerFactory_GetParameter_Int32(SPXRECOFACTORYHANDLE hrecofactory, const wchar_t* name, int32_t* pvalue, int32_t defaultValue)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        SPX_IFTRUE_THROW_HR(hrecofactory != SPXHANDLE_DEFAULT, SPXERR_INVALID_ARG);

        auto namedProperties = SpxQueryService<ISpxNamedProperties>(SpxGetRootSite());
        auto tempValue = namedProperties->GetNumberValue(name, defaultValue);
        *pvalue = (int32_t)tempValue;
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI_(bool) RecognizerFactory_ContainsParameter_Int32(SPXRECOFACTORYHANDLE hrecofactory, const wchar_t* name)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        SPX_IFTRUE_THROW_HR(hrecofactory != SPXHANDLE_DEFAULT, SPXERR_INVALID_ARG);

        auto namedProperties = SpxQueryService<ISpxNamedProperties>(SpxGetRootSite());
        return namedProperties->HasNumberValue(name);
    }
    SPXAPI_CATCH_AND_RETURN(hr, false)
}

SPXAPI RecognizerFactory_SetParameter_Bool(SPXRECOFACTORYHANDLE hrecofactory, const wchar_t* name, bool value)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        SPX_IFTRUE_THROW_HR(hrecofactory != SPXHANDLE_DEFAULT, SPXERR_INVALID_ARG);

        auto namedProperties = SpxQueryService<ISpxNamedProperties>(SpxGetRootSite());
        namedProperties->SetBooleanValue(name, value);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI RecognizerFactory_GetParameter_Bool(SPXRECOFACTORYHANDLE hrecofactory, const wchar_t* name, bool* pvalue, bool defaultValue)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        SPX_IFTRUE_THROW_HR(hrecofactory != SPXHANDLE_DEFAULT, SPXERR_INVALID_ARG);

        auto namedProperties = SpxQueryService<ISpxNamedProperties>(SpxGetRootSite());
        auto tempValue = namedProperties->GetBooleanValue(name, defaultValue);
        *pvalue = tempValue;
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI_(bool) RecognizerFactory_ContainsParameter_Bool(SPXRECOFACTORYHANDLE hrecofactory, const wchar_t* name)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        SPX_IFTRUE_THROW_HR(hrecofactory != SPXHANDLE_DEFAULT, SPXERR_INVALID_ARG);

        auto namedProperties = SpxQueryService<ISpxNamedProperties>(SpxGetRootSite());
        return namedProperties->HasBooleanValue(name);
    }
    SPXAPI_CATCH_AND_RETURN(hr, false)
}
