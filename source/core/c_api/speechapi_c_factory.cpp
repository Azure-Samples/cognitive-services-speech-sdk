//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_c_factory.cpp: Definitions for SpeechFactory related C methods
//

#include <string>
#include <vector>

#include "stdafx.h"
#include "service_helpers.h"
#include "site_helpers.h"
#include "handle_helpers.h"
#include "resource_manager.h"
#include "mock_controller.h"

using namespace Microsoft::CognitiveServices::Speech;
using namespace Microsoft::CognitiveServices::Speech::Impl;
using namespace std;

static_assert((int)OutputFormat::Simple == (int)SpeechOutputFormat_Simple, "OutputFormat should match between C and C++ layers");
static_assert((int)OutputFormat::Detailed == (int)SpeechOutputFormat_Detailed, "OutputFormat should match between C and C++ layers");

std::shared_ptr<ISpxNamedProperties> GetNamedPropertiesFromFactoryHandle(SPXFACTORYHANDLE hfactory)
{
    std::shared_ptr<ISpxNamedProperties> namedProperties;
    if (hfactory == SPXFACTORYHANDLE_ROOTSITEPARAMETERS_HACK)
    {
        namedProperties = SpxQueryService<ISpxNamedProperties>(SpxGetRootSite());
    }
    else
    {
        auto factoryhandles = CSpxSharedPtrHandleTableManager::Get<ISpxSpeechApiFactory, SPXFACTORYHANDLE>();
        auto factory = (*factoryhandles)[hfactory];
        namedProperties = SpxQueryService<ISpxNamedProperties>(factory);
    }

    return namedProperties;
}

SPXAPI_(bool) SpeechFactory_Handle_IsValid(SPXFACTORYHANDLE hfactory)
{
    return Handle_IsValid<SPXFACTORYHANDLE, ISpxSpeechApiFactory>(hfactory);
}

SPXAPI SpeechFactory_Handle_Close(SPXFACTORYHANDLE hfactory)
{
    return Handle_Close<SPXFACTORYHANDLE, ISpxSpeechApiFactory>(hfactory);
}

SPXAPI SpeechFactory_CreateSpeechRecognizer_With_Defaults(SPXFACTORYHANDLE  hfactory, SPXRECOHANDLE* phreco)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto factoryhandles = CSpxSharedPtrHandleTableManager::Get<ISpxSpeechApiFactory, SPXFACTORYHANDLE>();
        auto factory = (*factoryhandles)[hfactory];
        *phreco = SPXHANDLE_INVALID;

        auto recognizer = factory->CreateSpeechRecognizer();
        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        *phreco = recohandles->TrackHandle(recognizer);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

class SpeechApiAudioInputStreamWrapper : public AudioInputStream
{
public:
    SpeechApiAudioInputStreamWrapper(SpeechApi_AudioInputStream *pstream) :
        m_pstream(pstream)
    {
    }

    virtual int GetFormat(Microsoft::CognitiveServices::Speech::AudioInputStreamFormat* pformat, int cbFormat) override
    {
        struct tAudioInputStreamFormatC format;

        int retValue = 0;

        if (pformat)
        {
            retValue = m_pstream->GetFormat(m_pstream, &format, cbFormat);
            pformat->cbSize = format.cbSize;
            pformat->nAvgBytesPerSec = format.nAvgBytesPerSec;
            pformat->nBlockAlign = format.nBlockAlign;
            pformat->nChannels = format.nChannels;
            pformat->nSamplesPerSec = format.nSamplesPerSec;
            pformat->wBitsPerSample = format.wBitsPerSample;
            pformat->wFormatTag = format.wFormatTag;
        }
        else
        {
            retValue = m_pstream->GetFormat(m_pstream, nullptr, cbFormat);
        }

        return retValue;
    }

    virtual int Read(char* pbuffer, int cbBuffer) override
    {
        return m_pstream->Read(m_pstream, (unsigned char*)pbuffer, cbBuffer);
    }

    virtual void Close() override
    {
        m_pstream->Close(m_pstream);
    }


private:
    SpeechApi_AudioInputStream * m_pstream;
};

SPXAPI SpeechFactory_CreateSpeechRecognizer_With_Stream(SPXFACTORYHANDLE hfactory, SPXRECOHANDLE* phreco, SpeechApi_AudioInputStream *pstream)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto factoryhandles = CSpxSharedPtrHandleTableManager::Get<ISpxSpeechApiFactory, SPXFACTORYHANDLE>();
        auto factory = (*factoryhandles)[hfactory];
        SpeechApiAudioInputStreamWrapper* stream = new SpeechApiAudioInputStreamWrapper(pstream);

        *phreco = SPXHANDLE_INVALID;
        auto recognizer = factory->CreateSpeechRecognizerWithStream(stream);
        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        *phreco = recohandles->TrackHandle(recognizer);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI SpeechFactory_CreateSpeechRecognizer_With_StreamAndLanguage(SPXFACTORYHANDLE hfactory, SPXRECOHANDLE* phreco, SpeechApi_AudioInputStream *pstream, const wchar_t* pszLanguage)
{
    return SpeechFactory_CreateSpeechRecognizer_With_StreamAndLanguageAndFormat(hfactory, phreco, pstream, pszLanguage, SpeechOutputFormat_Simple);
}

SPXAPI SpeechFactory_CreateSpeechRecognizer_With_StreamAndLanguageAndFormat(SPXFACTORYHANDLE hfactory, SPXRECOHANDLE* phreco, SpeechApi_AudioInputStream *pstream, const wchar_t* pszLanguage, SpeechOutputFormat format)
{
    if (pszLanguage == nullptr)
        return SPXERR_INVALID_ARG;

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto factoryhandles = CSpxSharedPtrHandleTableManager::Get<ISpxSpeechApiFactory, SPXFACTORYHANDLE>();
        auto factory = (*factoryhandles)[hfactory];
        SpeechApiAudioInputStreamWrapper* stream = new SpeechApiAudioInputStreamWrapper(pstream); // TODO: Leaking? Same in other places.

        *phreco = SPXHANDLE_INVALID;
        auto recognizer = factory->CreateSpeechRecognizerWithStream(stream, pszLanguage, (OutputFormat)format);
        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        *phreco = recohandles->TrackHandle(recognizer);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI SpeechFactory_CreateSpeechRecognizer_With_Language(SPXFACTORYHANDLE hfactory, SPXRECOHANDLE* phreco, const wchar_t* pszLanguage)
{
    return SpeechFactory_CreateSpeechRecognizer_With_LanguageAndFormat(hfactory, phreco, pszLanguage, SpeechOutputFormat_Simple);
}

SPXAPI SpeechFactory_CreateSpeechRecognizer_With_LanguageAndFormat(SPXFACTORYHANDLE hfactory, SPXRECOHANDLE* phreco, const wchar_t* pszLanguage, SpeechOutputFormat format)
{
    if (pszLanguage == nullptr)
        return SPXERR_INVALID_ARG;

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto factoryhandles = CSpxSharedPtrHandleTableManager::Get<ISpxSpeechApiFactory, SPXFACTORYHANDLE>();
        auto factory = (*factoryhandles)[hfactory];
        *phreco = SPXHANDLE_INVALID;

        auto recognizer = factory->CreateSpeechRecognizer(std::wstring(pszLanguage), (OutputFormat)format);
        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        *phreco = recohandles->TrackHandle(recognizer);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI SpeechFactory_CreateSpeechRecognizer_With_FileInput(SPXFACTORYHANDLE hfactory, SPXRECOHANDLE* phreco, const wchar_t* pszFileName)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto factoryhandles = CSpxSharedPtrHandleTableManager::Get<ISpxSpeechApiFactory, SPXFACTORYHANDLE>();
        auto factory = (*factoryhandles)[hfactory];
        *phreco = SPXHANDLE_INVALID;

        auto recognizer = factory->CreateSpeechRecognizerWithFileInput(pszFileName);
        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        *phreco = recohandles->TrackHandle(recognizer);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI SpeechFactory_CreateSpeechRecognizer_With_FileInputAndLanguage(SPXFACTORYHANDLE hfactory, SPXRECOHANDLE* phreco, const wchar_t* pszLanguage, const wchar_t* pszFileName)
{
    return SpeechFactory_CreateSpeechRecognizer_With_FileInputAndLanguageAndFormat(hfactory, phreco, pszLanguage, pszFileName, SpeechOutputFormat_Simple);
}

SPXAPI SpeechFactory_CreateSpeechRecognizer_With_FileInputAndLanguageAndFormat(SPXFACTORYHANDLE hfactory, SPXRECOHANDLE* phreco, const wchar_t* pszLanguage, const wchar_t* pszFileName, SpeechOutputFormat format)
{
    if (pszLanguage == nullptr)
        return SPXERR_INVALID_ARG;

    if (pszFileName == nullptr)
        return SPXERR_INVALID_ARG;

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto factoryhandles = CSpxSharedPtrHandleTableManager::Get<ISpxSpeechApiFactory, SPXFACTORYHANDLE>();
        auto factory = (*factoryhandles)[hfactory];
        *phreco = SPXHANDLE_INVALID;

        auto recognizer = factory->CreateSpeechRecognizerWithFileInput(pszFileName, pszLanguage, (OutputFormat)format);
        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        *phreco = recohandles->TrackHandle(recognizer);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI SpeechFactory_CreateIntentRecognizer_With_Defaults(SPXFACTORYHANDLE hfactory, SPXRECOHANDLE* phreco)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto factoryhandles = CSpxSharedPtrHandleTableManager::Get<ISpxSpeechApiFactory, SPXFACTORYHANDLE>();
        auto factory = (*factoryhandles)[hfactory];
        *phreco = SPXHANDLE_INVALID;

        auto recognizer = factory->CreateIntentRecognizer();
        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        *phreco = recohandles->TrackHandle(recognizer);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI SpeechFactory_CreateIntentRecognizer_With_Language(SPXFACTORYHANDLE hfactory, SPXRECOHANDLE* phreco, const wchar_t* pszLanguage)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto factoryhandles = CSpxSharedPtrHandleTableManager::Get<ISpxSpeechApiFactory, SPXFACTORYHANDLE>();
        auto factory = (*factoryhandles)[hfactory];
        *phreco = SPXHANDLE_INVALID;

        auto recognizer = factory->CreateIntentRecognizer(pszLanguage);
        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        *phreco = recohandles->TrackHandle(recognizer);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI SpeechFactory_CreateIntentRecognizer_With_FileInput(SPXFACTORYHANDLE hfactory, SPXRECOHANDLE* phreco, const wchar_t* pszFileName)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto factoryhandles = CSpxSharedPtrHandleTableManager::Get<ISpxSpeechApiFactory, SPXFACTORYHANDLE>();
        auto factory = (*factoryhandles)[hfactory];
        *phreco = SPXHANDLE_INVALID;

        auto recognizer = factory->CreateIntentRecognizerWithFileInput(pszFileName);
        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        *phreco = recohandles->TrackHandle(recognizer);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI SpeechFactory_CreateIntentRecognizer_With_FileInputAndLanguage(SPXFACTORYHANDLE hfactory, SPXRECOHANDLE* phreco, const wchar_t* pszLanguage, const wchar_t* pszFileName)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto factoryhandles = CSpxSharedPtrHandleTableManager::Get<ISpxSpeechApiFactory, SPXFACTORYHANDLE>();
        auto factory = (*factoryhandles)[hfactory];
        *phreco = SPXHANDLE_INVALID;

        auto recognizer = factory->CreateIntentRecognizerWithFileInput(pszFileName, pszLanguage);
        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        *phreco = recohandles->TrackHandle(recognizer);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI SpeechFactory_CreateIntentRecognizer_With_Stream(SPXFACTORYHANDLE hfactory, SPXRECOHANDLE* phreco, SpeechApi_AudioInputStream *pstream)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto factoryhandles = CSpxSharedPtrHandleTableManager::Get<ISpxSpeechApiFactory, SPXFACTORYHANDLE>();
        auto factory = (*factoryhandles)[hfactory];
        SpeechApiAudioInputStreamWrapper* stream = new SpeechApiAudioInputStreamWrapper(pstream);

        *phreco = SPXHANDLE_INVALID;
        auto recognizer = factory->CreateIntentRecognizerWithStream(stream);
        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        *phreco = recohandles->TrackHandle(recognizer);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI SpeechFactory_CreateIntentRecognizer_With_StreamAndLanguage(SPXFACTORYHANDLE hfactory, SPXRECOHANDLE* phreco, SpeechApi_AudioInputStream *pstream, const wchar_t* pszLanguage)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto factoryhandles = CSpxSharedPtrHandleTableManager::Get<ISpxSpeechApiFactory, SPXFACTORYHANDLE>();
        auto factory = (*factoryhandles)[hfactory];
        SpeechApiAudioInputStreamWrapper* stream = new SpeechApiAudioInputStreamWrapper(pstream);

        *phreco = SPXHANDLE_INVALID;
        auto recognizer = factory->CreateIntentRecognizerWithStream(stream, pszLanguage);
        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        *phreco = recohandles->TrackHandle(recognizer);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

inline vector<wstring> GetVectorFromBuffer(const wchar_t* buffer[], size_t entries)
{
    vector<wstring> result;
    if (buffer != nullptr)
    {
        for (size_t i = 0; i < entries; i++)
        {
            result.push_back(buffer[i]);
        }
    }

    return result;
}

SPXAPI SpeechFactory_CreateTranslationRecognizer(SPXFACTORYHANDLE hfactory, SPXRECOHANDLE* recoHandlePointer, const wchar_t* sourceLanguage, const wchar_t* targetLanguages[], size_t numberOfTargetLanguages, const wchar_t* voice)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto factoryhandles = CSpxSharedPtrHandleTableManager::Get<ISpxSpeechApiFactory, SPXFACTORYHANDLE>();
        auto factory = (*factoryhandles)[hfactory];
        *recoHandlePointer = SPXHANDLE_INVALID;

        auto toLangs = GetVectorFromBuffer(targetLanguages, numberOfTargetLanguages);
        auto recognizer = factory->CreateTranslationRecognizer(sourceLanguage, toLangs, voice);
        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        *recoHandlePointer = recohandles->TrackHandle(recognizer);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI SpeechFactory_CreateTranslationRecognizer_With_FileInput(SPXFACTORYHANDLE hfactory, SPXRECOHANDLE* recoHandlePointer, const wchar_t* sourceLanguage, const wchar_t* targetLanguages[], size_t numberOfTargetLanguages, const wchar_t* voice, const wchar_t* fileName)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto factoryhandles = CSpxSharedPtrHandleTableManager::Get<ISpxSpeechApiFactory, SPXFACTORYHANDLE>();
        auto factory = (*factoryhandles)[hfactory];
        *recoHandlePointer = SPXHANDLE_INVALID;

        auto toLangs = GetVectorFromBuffer(targetLanguages, numberOfTargetLanguages);
        auto recognizer = factory->CreateTranslationRecognizerWithFileInput(fileName, sourceLanguage, toLangs, voice);
        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        *recoHandlePointer = recohandles->TrackHandle(recognizer);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI SpeechFactory_CreateTranslationRecognizer_With_Stream(SPXFACTORYHANDLE hfactory, SPXRECOHANDLE* recoHandlePointer, const wchar_t* sourceLanguage, const wchar_t* targetLanguages[], size_t numberOfTargetLanguages, const wchar_t* voice, SpeechApi_AudioInputStream *stream)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto factoryhandles = CSpxSharedPtrHandleTableManager::Get<ISpxSpeechApiFactory, SPXFACTORYHANDLE>();
        auto factory = (*factoryhandles)[hfactory];
        SpeechApiAudioInputStreamWrapper* streamWrapper = new SpeechApiAudioInputStreamWrapper(stream);

        *recoHandlePointer = SPXHANDLE_INVALID;
        auto toLangs = GetVectorFromBuffer(targetLanguages, numberOfTargetLanguages);
        auto recognizer = factory->CreateTranslationRecognizerWithStream(streamWrapper, sourceLanguage, toLangs, voice);
        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        *recoHandlePointer = recohandles->TrackHandle(recognizer);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}


SPXAPI SpeechFactory_GetParameter_Name(Factory_Parameter parameter, wchar_t* name, uint32_t cchName)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        const wchar_t* parameterName = L"";
        switch (parameter)
        {
            case FactoryParameter_SubscriptionKey:
                parameterName = g_SPEECH_SubscriptionKey;
                break;

            case FactoryParameter_AuthorizationToken:
                parameterName = g_SPEECH_AuthToken;
                break;

            case FactoryParameter_Region:
                parameterName = g_SPEECH_Region;
                break;

            case FactoryParameter_Endpoint:
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

SPXAPI SpeechFactory_SetParameter_String(SPXFACTORYHANDLE hfactory, const wchar_t* name, const wchar_t* value)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto namedProperties = GetNamedPropertiesFromFactoryHandle(hfactory);
        namedProperties->SetStringValue(name, value);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI SpeechFactory_GetParameter_String(SPXFACTORYHANDLE hfactory, const wchar_t* name, wchar_t* value, uint32_t cchValue, const wchar_t* defaultValue)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto namedProperties = GetNamedPropertiesFromFactoryHandle(hfactory);
        auto tempValue = namedProperties->GetStringValue(name, defaultValue);
        PAL::wcscpy(value, cchValue, tempValue.c_str(), tempValue.size(), true);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI_(bool) SpeechFactory_ContainsParameter_String(SPXFACTORYHANDLE hfactory, const wchar_t* name)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto namedProperties = GetNamedPropertiesFromFactoryHandle(hfactory);
        return namedProperties->HasStringValue(name);
    }
    SPXAPI_CATCH_AND_RETURN(hr, false)
}

SPXAPI SpeechFactory_SetParameter_Int32(SPXFACTORYHANDLE hfactory, const wchar_t* name, int32_t value)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto namedProperties = GetNamedPropertiesFromFactoryHandle(hfactory);
        namedProperties->SetNumberValue(name, value);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI SpeechFactory_GetParameter_Int32(SPXFACTORYHANDLE hfactory, const wchar_t* name, int32_t* pvalue, int32_t defaultValue)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto namedProperties = GetNamedPropertiesFromFactoryHandle(hfactory);
        auto tempValue = namedProperties->GetNumberValue(name, defaultValue);
        *pvalue = (int32_t)tempValue;
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI_(bool) SpeechFactory_ContainsParameter_Int32(SPXFACTORYHANDLE hfactory, const wchar_t* name)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto namedProperties = GetNamedPropertiesFromFactoryHandle(hfactory);
        return namedProperties->HasNumberValue(name);
    }
    SPXAPI_CATCH_AND_RETURN(hr, false)
}

SPXAPI SpeechFactory_SetParameter_Bool(SPXFACTORYHANDLE hfactory, const wchar_t* name, bool value)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto namedProperties = GetNamedPropertiesFromFactoryHandle(hfactory);
        namedProperties->SetBooleanValue(name, value);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI SpeechFactory_GetParameter_Bool(SPXFACTORYHANDLE hfactory, const wchar_t* name, bool* pvalue, bool defaultValue)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto namedProperties = GetNamedPropertiesFromFactoryHandle(hfactory);
        auto tempValue = namedProperties->GetBooleanValue(name, defaultValue);
        *pvalue = tempValue;
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI_(bool) SpeechFactory_ContainsParameter_Bool(SPXFACTORYHANDLE hfactory, const wchar_t* name)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto namedProperties = GetNamedPropertiesFromFactoryHandle(hfactory);
        return namedProperties->HasBooleanValue(name);
    }
    SPXAPI_CATCH_AND_RETURN(hr, false)
}

SPXAPI SpeechFactory_FromAuthorizationToken(const wchar_t* authToken, const wchar_t* region, SPXFACTORYHANDLE* phfactory)
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    SPXAPI_INIT_HR_TRY(hr)
    {
        *phfactory = SPXHANDLE_INVALID;

        auto factory = SpxCreateObjectWithSite<ISpxSpeechApiFactory>("CSpxSpeechApiFactory", SpxGetRootSite());

        auto namedProperties = SpxQueryService<ISpxNamedProperties>(factory);
        namedProperties->SetStringValue(g_SPEECH_AuthToken, authToken);

        if (region != nullptr && *region != L'\0')
        {
            namedProperties->SetStringValue(g_SPEECH_Region, region);
        }

        auto factoryhandles = CSpxSharedPtrHandleTableManager::Get<ISpxSpeechApiFactory, SPXFACTORYHANDLE>();
        *phfactory = factoryhandles->TrackHandle(factory);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI SpeechFactory_FromSubscription(const wchar_t* subscriptionKey, const wchar_t* region, SPXFACTORYHANDLE* phfactory)
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    SPXAPI_INIT_HR_TRY(hr)
    {
        *phfactory = SPXHANDLE_INVALID;

        auto factory = SpxCreateObjectWithSite<ISpxSpeechApiFactory>("CSpxSpeechApiFactory", SpxGetRootSite());

        auto namedProperties = SpxQueryService<ISpxNamedProperties>(factory);
        namedProperties->SetStringValue(g_SPEECH_SubscriptionKey, subscriptionKey);

        if (region != nullptr && *region != L'\0')
        {
            namedProperties->SetStringValue(g_SPEECH_Region, region);
        }

        auto factoryhandles = CSpxSharedPtrHandleTableManager::Get<ISpxSpeechApiFactory, SPXFACTORYHANDLE>();
        *phfactory = factoryhandles->TrackHandle(factory);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI SpeechFactory_FromEndpoint(const wchar_t* endpoint, const wchar_t* subscription, SPXFACTORYHANDLE* phfactory)
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    SPXAPI_INIT_HR_TRY(hr)
    {
        *phfactory = SPXHANDLE_INVALID;

        auto factory = SpxCreateObjectWithSite<ISpxSpeechApiFactory>("CSpxSpeechApiFactory", SpxGetRootSite());

        auto namedProperties = SpxQueryService<ISpxNamedProperties>(factory);
        namedProperties->SetStringValue(g_SPEECH_Endpoint, endpoint);

        if (subscription != nullptr && *subscription != L'\0')
        {
            namedProperties->SetStringValue(g_SPEECH_SubscriptionKey, subscription);
        }

        auto factoryhandles = CSpxSharedPtrHandleTableManager::Get<ISpxSpeechApiFactory, SPXFACTORYHANDLE>();
        *phfactory = factoryhandles->TrackHandle(factory);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}
