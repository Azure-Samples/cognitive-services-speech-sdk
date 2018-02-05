#include "stdafx.h"


using namespace CARBON_IMPL_NAMESPACE();


SPXAPI RecognizerFactory_CreateSpeechRecognizer_With_Defaults(SPXRECOHANDLE* phreco)
{
    SPXAPI_INIT_TRY(hr)
    {
        *phreco = SPXHANDLE_INVALID;
        auto recognizer = CSpxRecognizerFactory::CreateSpeechRecognizer();
        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        *phreco = recohandles->TrackHandle(recognizer);
    }
    SPXAPI_CATCH_AND_RETURN(hr);
}

SPXAPI RecognizerFactory_CreateSpeechRecognizer_With_Language(SPXRECOHANDLE* phreco, const wchar_t* pszLanguage)
{
    UNUSED(phreco);
    UNUSED(pszLanguage);
    return SPXERR_NOT_IMPL;
}

SPXAPI RecognizerFactory_CreateSpeechRecognizer_With_FileInput(SPXRECOHANDLE* phreco, const wchar_t* pszFileName)
{
    SPXAPI_INIT_TRY(hr)
    {
        *phreco = SPXHANDLE_INVALID;
        auto recognizer = CSpxRecognizerFactory::CreateSpeechRecognizerWithFileInput(pszFileName);
        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        *phreco = recohandles->TrackHandle(recognizer);
    }
    SPXAPI_CATCH_AND_RETURN(hr);
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
    UNUSED(phreco);
    return SPXERR_NOT_IMPL;
}

SPXAPI RecognizerFactory_CreateIntentRecognizer_With_Language(SPXRECOHANDLE* phreco, const wchar_t* pszLanguage)
{
    UNUSED(phreco);
    UNUSED(pszLanguage);
    return SPXERR_NOT_IMPL;
}

SPXAPI RecognizerFactory_CreateIntentRecognizer_With_FileInput(SPXRECOHANDLE* phreco, const wchar_t* pszFileName)
{
    UNUSED(phreco);
    UNUSED(pszFileName);
    return SPXERR_NOT_IMPL;
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
