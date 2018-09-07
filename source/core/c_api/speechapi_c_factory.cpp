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

static_assert((int)FactoryParameter_Region == (int)FactoryParameter::Region, "FactoryParameter_* enum values == FactoryParameter::* enum values");
static_assert((int)FactoryParameter_SubscriptionKey == (int)FactoryParameter::SubscriptionKey, "FactoryParameter_* enum values == FactoryParameter::* enum values");
static_assert((int)FactoryParameter_AuthorizationToken == (int)FactoryParameter::AuthorizationToken, "FactoryParameter_* enum values == FactoryParameter::* enum values");
static_assert((int)FactoryParameter_Endpoint == (int)FactoryParameter::Endpoint, "FactoryParameter_* enum values == FactoryParameter::* enum values");

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

    virtual size_t GetFormat(Microsoft::CognitiveServices::Speech::AudioInputStreamFormat* pformat, size_t cbFormat) override
    {
        struct _AudioInputStreamFormatC format;
        if ((size_t)std::numeric_limits<int>().max() < cbFormat)
        {
            throw std::overflow_error("cbFormat");
        }

        int retValue = 0;
        if (pformat)
        {
            retValue = m_pstream->GetFormat(m_pstream, &format, (int)cbFormat);
            pformat->AvgBytesPerSec = format.AvgBytesPerSec;
            pformat->BlockAlign = format.BlockAlign;
            pformat->Channels = format.Channels;
            pformat->SamplesPerSec = format.SamplesPerSec;
            pformat->BitsPerSample = format.BitsPerSample;
            pformat->FormatTag = format.FormatTag;
        }
        else
        {
            retValue = m_pstream->GetFormat(m_pstream, nullptr, (int)cbFormat);
        }

        if (retValue <= 0)
        {
            throw std::runtime_error("Could not get data format from stream, error code" + std::to_string(retValue));
        }

        return (size_t)retValue;
    }

    virtual size_t Read(char* pbuffer, size_t size) override
    {
        if ((size_t)std::numeric_limits<int>().max() < size)
        {
            throw std::overflow_error("size");
        }

        auto res = m_pstream->Read(m_pstream, (unsigned char*)pbuffer, (int)size);
        if (res < 0)
        {
            throw std::runtime_error(("Could not read data from stream, error code" + std::to_string(res)).c_str());
        }

        return res;
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

SPXAPI SpeechFactory_CreateSpeechRecognizer_With_StreamAndLanguage(SPXFACTORYHANDLE hfactory, SPXRECOHANDLE* phreco, SpeechApi_AudioInputStream *pstream, const char* pszLanguage)
{
    return SpeechFactory_CreateSpeechRecognizer_With_StreamAndLanguageAndFormat(hfactory, phreco, pstream, pszLanguage, SpeechOutputFormat_Simple);
}

SPXAPI SpeechFactory_CreateSpeechRecognizer_With_StreamAndLanguageAndFormat(SPXFACTORYHANDLE hfactory, SPXRECOHANDLE* phreco, SpeechApi_AudioInputStream *pstream, const char* pszLanguage, SpeechOutputFormat format)
{
    if (pszLanguage == nullptr)
        return SPXERR_INVALID_ARG;

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto factoryhandles = CSpxSharedPtrHandleTableManager::Get<ISpxSpeechApiFactory, SPXFACTORYHANDLE>();
        auto factory = (*factoryhandles)[hfactory];
        SpeechApiAudioInputStreamWrapper* stream = new SpeechApiAudioInputStreamWrapper(pstream); // TODO: Leaking? Same in other places.

        *phreco = SPXHANDLE_INVALID;
        auto recognizer = factory->CreateSpeechRecognizerWithStream(stream, PAL::ToWString(pszLanguage), (OutputFormat)format);
        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        *phreco = recohandles->TrackHandle(recognizer);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI SpeechFactory_CreateSpeechRecognizer_With_Language(SPXFACTORYHANDLE hfactory, SPXRECOHANDLE* phreco, const char* pszLanguage)
{
    return SpeechFactory_CreateSpeechRecognizer_With_LanguageAndFormat(hfactory, phreco, pszLanguage, SpeechOutputFormat_Simple);
}

SPXAPI SpeechFactory_CreateSpeechRecognizer_With_LanguageAndFormat(SPXFACTORYHANDLE hfactory, SPXRECOHANDLE* phreco, const char* pszLanguage, SpeechOutputFormat format)
{
    if (pszLanguage == nullptr)
        return SPXERR_INVALID_ARG;

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto factoryhandles = CSpxSharedPtrHandleTableManager::Get<ISpxSpeechApiFactory, SPXFACTORYHANDLE>();
        auto factory = (*factoryhandles)[hfactory];
        *phreco = SPXHANDLE_INVALID;

        auto recognizer = factory->CreateSpeechRecognizer(PAL::ToWString(pszLanguage), (OutputFormat)format);
        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        *phreco = recohandles->TrackHandle(recognizer);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI SpeechFactory_CreateSpeechRecognizer_With_FileInput(SPXFACTORYHANDLE hfactory, SPXRECOHANDLE* phreco, const char* pszFileName)
{
    if (pszFileName == nullptr)
        return SPXERR_INVALID_ARG;

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto factoryhandles = CSpxSharedPtrHandleTableManager::Get<ISpxSpeechApiFactory, SPXFACTORYHANDLE>();
        auto factory = (*factoryhandles)[hfactory];
        *phreco = SPXHANDLE_INVALID;

        auto recognizer = factory->CreateSpeechRecognizerWithFileInput(PAL::ToWString(pszFileName));
        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        *phreco = recohandles->TrackHandle(recognizer);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI SpeechFactory_CreateSpeechRecognizer_With_FileInputAndLanguage(SPXFACTORYHANDLE hfactory, SPXRECOHANDLE* phreco, const char* pszLanguage, const char* pszFileName)
{
    return SpeechFactory_CreateSpeechRecognizer_With_FileInputAndLanguageAndFormat(hfactory, phreco, pszLanguage, pszFileName, SpeechOutputFormat_Simple);
}

SPXAPI SpeechFactory_CreateSpeechRecognizer_With_FileInputAndLanguageAndFormat(SPXFACTORYHANDLE hfactory, SPXRECOHANDLE* phreco, const char* pszLanguage, const char* pszFileName, SpeechOutputFormat format)
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

        auto recognizer = factory->CreateSpeechRecognizerWithFileInput(PAL::ToWString(pszFileName), PAL::ToWString(pszLanguage), (OutputFormat)format);
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

SPXAPI SpeechFactory_CreateIntentRecognizer_With_Language(SPXFACTORYHANDLE hfactory, SPXRECOHANDLE* phreco, const char* pszLanguage)
{
    if (pszLanguage == nullptr)
        return SPXERR_INVALID_ARG;

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto factoryhandles = CSpxSharedPtrHandleTableManager::Get<ISpxSpeechApiFactory, SPXFACTORYHANDLE>();
        auto factory = (*factoryhandles)[hfactory];
        *phreco = SPXHANDLE_INVALID;

        auto recognizer = factory->CreateIntentRecognizer(PAL::ToWString(pszLanguage));
        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        *phreco = recohandles->TrackHandle(recognizer);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI SpeechFactory_CreateIntentRecognizer_With_FileInput(SPXFACTORYHANDLE hfactory, SPXRECOHANDLE* phreco, const char* pszFileName)
{
    if (pszFileName == nullptr)
        return SPXERR_INVALID_ARG;

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto factoryhandles = CSpxSharedPtrHandleTableManager::Get<ISpxSpeechApiFactory, SPXFACTORYHANDLE>();
        auto factory = (*factoryhandles)[hfactory];
        *phreco = SPXHANDLE_INVALID;

        auto recognizer = factory->CreateIntentRecognizerWithFileInput(PAL::ToWString(pszFileName));
        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        *phreco = recohandles->TrackHandle(recognizer);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI SpeechFactory_CreateIntentRecognizer_With_FileInputAndLanguage(SPXFACTORYHANDLE hfactory, SPXRECOHANDLE* phreco, const char* pszLanguage, const char* pszFileName)
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

        auto recognizer = factory->CreateIntentRecognizerWithFileInput(PAL::ToWString(pszFileName), PAL::ToWString(pszLanguage));
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

SPXAPI SpeechFactory_CreateIntentRecognizer_With_StreamAndLanguage(SPXFACTORYHANDLE hfactory, SPXRECOHANDLE* phreco, SpeechApi_AudioInputStream *pstream, const char* pszLanguage)
{
    if (pszLanguage == nullptr)
        return SPXERR_INVALID_ARG;

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto factoryhandles = CSpxSharedPtrHandleTableManager::Get<ISpxSpeechApiFactory, SPXFACTORYHANDLE>();
        auto factory = (*factoryhandles)[hfactory];
        SpeechApiAudioInputStreamWrapper* stream = new SpeechApiAudioInputStreamWrapper(pstream);

        *phreco = SPXHANDLE_INVALID;
        auto recognizer = factory->CreateIntentRecognizerWithStream(stream, PAL::ToWString(pszLanguage));
        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        *phreco = recohandles->TrackHandle(recognizer);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

inline vector<wstring> GetVectorFromBuffer(const char* buffer[], size_t entries)
{
    vector<wstring> result;
    if (buffer != nullptr)
    {
        for (size_t i = 0; i < entries; i++)
        {
            if (!buffer[i])
                Impl::ThrowWithCallstack(SPXERR_INVALID_ARG);
            else
                result.push_back(PAL::ToWString(buffer[i]));
        }
    }

    return result;
}

SPXAPI SpeechFactory_CreateTranslationRecognizer(SPXFACTORYHANDLE hfactory, SPXRECOHANDLE* recoHandlePointer, const char* sourceLanguage, const char* targetLanguages[], size_t numberOfTargetLanguages, const char* voice)
{
    if (sourceLanguage == nullptr)
        return SPXERR_INVALID_ARG;

    if (voice == nullptr)
        return SPXERR_INVALID_ARG;

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto factoryhandles = CSpxSharedPtrHandleTableManager::Get<ISpxSpeechApiFactory, SPXFACTORYHANDLE>();
        auto factory = (*factoryhandles)[hfactory];
        *recoHandlePointer = SPXHANDLE_INVALID;

        auto toLangs = GetVectorFromBuffer(targetLanguages, numberOfTargetLanguages);
        auto recognizer = factory->CreateTranslationRecognizer(PAL::ToWString(sourceLanguage), toLangs, PAL::ToWString(voice));
        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        *recoHandlePointer = recohandles->TrackHandle(recognizer);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI SpeechFactory_CreateTranslationRecognizer_With_FileInput(SPXFACTORYHANDLE hfactory, SPXRECOHANDLE* recoHandlePointer, const char* sourceLanguage, const char* targetLanguages[], size_t numberOfTargetLanguages, const char* voice, const char* fileName)
{
    if (sourceLanguage == nullptr)
        return SPXERR_INVALID_ARG;

    if (voice == nullptr)
        return SPXERR_INVALID_ARG;

    if (fileName == nullptr)
        return SPXERR_INVALID_ARG;

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto factoryhandles = CSpxSharedPtrHandleTableManager::Get<ISpxSpeechApiFactory, SPXFACTORYHANDLE>();
        auto factory = (*factoryhandles)[hfactory];
        *recoHandlePointer = SPXHANDLE_INVALID;

        auto toLangs = GetVectorFromBuffer(targetLanguages, numberOfTargetLanguages);
        auto recognizer = factory->CreateTranslationRecognizerWithFileInput(PAL::ToWString(fileName), PAL::ToWString(sourceLanguage), toLangs, PAL::ToWString(voice));
        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        *recoHandlePointer = recohandles->TrackHandle(recognizer);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI SpeechFactory_CreateTranslationRecognizer_With_Stream(SPXFACTORYHANDLE hfactory, SPXRECOHANDLE* recoHandlePointer, const char* sourceLanguage, const char* targetLanguages[], size_t numberOfTargetLanguages, const char* voice, SpeechApi_AudioInputStream *stream)
{
    if (sourceLanguage == nullptr)
        return SPXERR_INVALID_ARG;

    if (voice == nullptr)
        return SPXERR_INVALID_ARG;

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto factoryhandles = CSpxSharedPtrHandleTableManager::Get<ISpxSpeechApiFactory, SPXFACTORYHANDLE>();
        auto factory = (*factoryhandles)[hfactory];
        SpeechApiAudioInputStreamWrapper* streamWrapper = new SpeechApiAudioInputStreamWrapper(stream);

        *recoHandlePointer = SPXHANDLE_INVALID;
        auto toLangs = GetVectorFromBuffer(targetLanguages, numberOfTargetLanguages);
        auto recognizer = factory->CreateTranslationRecognizerWithStream(streamWrapper, PAL::ToWString(sourceLanguage), toLangs, PAL::ToWString(voice));
        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        *recoHandlePointer = recohandles->TrackHandle(recognizer);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}


SPXAPI SpeechFactory_GetParameter_Name(Factory_Parameter parameter, char* name, uint32_t cchName)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        const char* parameterName = "";
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

        PAL::strcpy(name, cchName, parameterName, strlen(parameterName), true);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI SpeechFactory_SetParameter_String(SPXFACTORYHANDLE hfactory, const char* name, const char* value)
{
    if (name == nullptr)
        return SPXERR_INVALID_ARG;

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto namedProperties = GetNamedPropertiesFromFactoryHandle(hfactory);
        namedProperties->SetStringValue(PAL::ToWString(name).c_str(), value ? PAL::ToWString(value).c_str() : nullptr);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI SpeechFactory_GetParameter_String(SPXFACTORYHANDLE hfactory, const char* name, char* value, uint32_t cchValue, const char* defaultValue)
{
    if (name == nullptr)
        return SPXERR_INVALID_ARG;

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto namedProperties = GetNamedPropertiesFromFactoryHandle(hfactory);
        auto tempValue = PAL::ToString(namedProperties->GetStringValue(PAL::ToWString(name).c_str(), defaultValue ? PAL::ToWString(defaultValue).c_str(): nullptr));
        PAL::strcpy(value, cchValue, tempValue.c_str(), tempValue.size(), true);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI_(bool) SpeechFactory_ContainsParameter_String(SPXFACTORYHANDLE hfactory, const char* name)
{
    if (name == nullptr)
        return false;

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto namedProperties = GetNamedPropertiesFromFactoryHandle(hfactory);
        return namedProperties->HasStringValue(PAL::ToWString(name).c_str());
    }
    SPXAPI_CATCH_AND_RETURN(hr, false)
}

SPXAPI SpeechFactory_SetParameter_Int32(SPXFACTORYHANDLE hfactory, const char* name, int32_t value)
{
    if (name == nullptr)
        return SPXERR_INVALID_ARG;

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto namedProperties = GetNamedPropertiesFromFactoryHandle(hfactory);
        namedProperties->SetNumberValue(PAL::ToWString(name).c_str(), value);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI SpeechFactory_GetParameter_Int32(SPXFACTORYHANDLE hfactory, const char* name, int32_t* pvalue, int32_t defaultValue)
{
    if (name == nullptr)
        return SPXERR_INVALID_ARG;

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto namedProperties = GetNamedPropertiesFromFactoryHandle(hfactory);
        auto tempValue = namedProperties->GetNumberValue(PAL::ToWString(name).c_str(), defaultValue);
        *pvalue = (int32_t)tempValue;
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI_(bool) SpeechFactory_ContainsParameter_Int32(SPXFACTORYHANDLE hfactory, const char* name)
{
    if (name == nullptr)
        return false;

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto namedProperties = GetNamedPropertiesFromFactoryHandle(hfactory);
        return namedProperties->HasNumberValue(PAL::ToWString(name).c_str());
    }
    SPXAPI_CATCH_AND_RETURN(hr, false)
}

SPXAPI SpeechFactory_SetParameter_Bool(SPXFACTORYHANDLE hfactory, const char* name, bool value)
{
    if (name == nullptr)
        return SPXERR_INVALID_ARG;

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto namedProperties = GetNamedPropertiesFromFactoryHandle(hfactory);
        namedProperties->SetBooleanValue(PAL::ToWString(name).c_str(), value);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI SpeechFactory_GetParameter_Bool(SPXFACTORYHANDLE hfactory, const char* name, bool* pvalue, bool defaultValue)
{
    if (name == nullptr)
        return SPXERR_INVALID_ARG;

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto namedProperties = GetNamedPropertiesFromFactoryHandle(hfactory);
        auto tempValue = namedProperties->GetBooleanValue(PAL::ToWString(name).c_str(), defaultValue);
        *pvalue = tempValue;
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI_(bool) SpeechFactory_ContainsParameter_Bool(SPXFACTORYHANDLE hfactory, const char* name)
{
    if (name == nullptr)
        return false;

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto namedProperties = GetNamedPropertiesFromFactoryHandle(hfactory);
        return namedProperties->HasBooleanValue(PAL::ToWString(name).c_str());
    }
    SPXAPI_CATCH_AND_RETURN(hr, false)
}

SPXAPI SpeechFactory_FromAuthorizationToken(const char* authToken, const char* region, SPXFACTORYHANDLE* phfactory)
{
    if (region == nullptr)
        return SPXERR_INVALID_ARG;

    if (authToken == nullptr)
        return SPXERR_INVALID_ARG;

    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    SPXAPI_INIT_HR_TRY(hr)
    {
        *phfactory = SPXHANDLE_INVALID;

        auto factory = SpxCreateObjectWithSite<ISpxSpeechApiFactory>("CSpxSpeechApiFactory", SpxGetRootSite());

        auto namedProperties = SpxQueryService<ISpxNamedProperties>(factory);
        namedProperties->SetStringValue(PAL::ToWString(g_SPEECH_AuthToken).c_str(), PAL::ToWString(authToken).c_str());

        if (region != nullptr && *region != L'\0')
        {
            namedProperties->SetStringValue(PAL::ToWString(g_SPEECH_Region).c_str(), PAL::ToWString(region).c_str());
        }

        auto factoryhandles = CSpxSharedPtrHandleTableManager::Get<ISpxSpeechApiFactory, SPXFACTORYHANDLE>();
        *phfactory = factoryhandles->TrackHandle(factory);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI SpeechFactory_FromSubscription(const char* subscriptionKey, const char* region, SPXFACTORYHANDLE* phfactory)
{
    if (region == nullptr)
        return SPXERR_INVALID_ARG;

    if (subscriptionKey == nullptr)
        return SPXERR_INVALID_ARG;

    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    SPXAPI_INIT_HR_TRY(hr)
    {
        *phfactory = SPXHANDLE_INVALID;

        auto factory = SpxCreateObjectWithSite<ISpxSpeechApiFactory>("CSpxSpeechApiFactory", SpxGetRootSite());

        auto namedProperties = SpxQueryService<ISpxNamedProperties>(factory);
        namedProperties->SetStringValue(PAL::ToWString(g_SPEECH_SubscriptionKey).c_str(), PAL::ToWString(subscriptionKey).c_str());

        if (region != nullptr && *region != L'\0')
        {
            namedProperties->SetStringValue(PAL::ToWString(g_SPEECH_Region).c_str(), PAL::ToWString(region).c_str());
        }

        auto factoryhandles = CSpxSharedPtrHandleTableManager::Get<ISpxSpeechApiFactory, SPXFACTORYHANDLE>();
        *phfactory = factoryhandles->TrackHandle(factory);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI SpeechFactory_FromEndpoint(const char* endpoint, const char* subscription, SPXFACTORYHANDLE* phfactory)
{
    if (endpoint == nullptr)
        return SPXERR_INVALID_ARG;

    if (subscription == nullptr)
        return SPXERR_INVALID_ARG;

    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    SPXAPI_INIT_HR_TRY(hr)
    {
        *phfactory = SPXHANDLE_INVALID;

        auto factory = SpxCreateObjectWithSite<ISpxSpeechApiFactory>("CSpxSpeechApiFactory", SpxGetRootSite());

        auto namedProperties = SpxQueryService<ISpxNamedProperties>(factory);
        namedProperties->SetStringValue(PAL::ToWString(g_SPEECH_Endpoint).c_str(), PAL::ToWString(endpoint).c_str());

        if (subscription != nullptr && *subscription != L'\0')
        {
            namedProperties->SetStringValue(PAL::ToWString(g_SPEECH_SubscriptionKey).c_str(), PAL::ToWString(subscription).c_str());
        }

        auto factoryhandles = CSpxSharedPtrHandleTableManager::Get<ISpxSpeechApiFactory, SPXFACTORYHANDLE>();
        *phfactory = factoryhandles->TrackHandle(factory);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}
