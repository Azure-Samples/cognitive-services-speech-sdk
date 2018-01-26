#include "stdafx.h"


using namespace CARBON_IMPL_NAMESPACE();


SPXAPI RecognizerFactory_CreateSpeechRecognzier_With_Defaults(SPXRECOHANDLE* phreco)
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

SPXAPI RecognizerFactory_CreateSpeechRecognzier_With_Language(SPXRECOHANDLE* phreco, const wchar_t* pszLanguage)
{
    UNUSED(phreco);
    UNUSED(pszLanguage);
    return SPXERR_NOT_IMPL;
}

SPXAPI RecognizerFactory_CreateSpeechRecognzier_With_FileInput(SPXRECOHANDLE* phreco, const wchar_t* pszFileName)
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

SPXAPI RecognizerFactory_CreateSpeechRecognzier_With_PassiveListening(SPXRECOHANDLE* phreco, bool passive)
{
    UNUSED(phreco);
    UNUSED(passive);
    return SPXERR_NOT_IMPL;
}

SPXAPI RecognizerFactory_CreateSpeechRecognzier(SPXRECOHANDLE* phreco, const wchar_t* pszLanguage, const wchar_t* pszFileName, bool passive)
{
    UNUSED(phreco);
    UNUSED(pszLanguage);
    UNUSED(pszFileName);
    UNUSED(passive);
    return SPXERR_NOT_IMPL;
}

SPXAPI RecognizerFactory_CreateDictationRecognzier_With_Defaults(SPXRECOHANDLE* phreco)
{
    UNUSED(phreco);
    return SPXERR_NOT_IMPL;
}

SPXAPI RecognizerFactory_CreateDictationRecognzier_With_Language(SPXRECOHANDLE* phreco, const wchar_t* pszLanguage)
{
    UNUSED(phreco);
    UNUSED(pszLanguage);
    return SPXERR_NOT_IMPL;
}

SPXAPI RecognizerFactory_CreateDictationRecognzier_With_FileInput(SPXRECOHANDLE* phreco, const wchar_t* pszFileName)
{
    UNUSED(phreco);
    UNUSED(pszFileName);
    return SPXERR_NOT_IMPL;
}

SPXAPI RecognizerFactory_CreateDictationRecognzier_With_PassiveListening(SPXRECOHANDLE* phreco, bool passive)
{
    UNUSED(phreco);
    UNUSED(passive);
    return SPXERR_NOT_IMPL;
}

SPXAPI RecognizerFactory_CreateDictationRecognzier(SPXRECOHANDLE* phreco, const wchar_t* pszLanguage, const wchar_t* pszFileName, bool passive)
{
    UNUSED(phreco);
    UNUSED(pszLanguage);
    UNUSED(pszFileName);
    UNUSED(passive);
    return SPXERR_NOT_IMPL;
}

SPXAPI RecognizerFactory_CreateIntentRecognzier_With_Defaults(SPXRECOHANDLE* phreco)
{
    UNUSED(phreco);
    return SPXERR_NOT_IMPL;
}

SPXAPI RecognizerFactory_CreateIntentRecognzier_With_Language(SPXRECOHANDLE* phreco, const wchar_t* pszLanguage)
{
    UNUSED(phreco);
    UNUSED(pszLanguage);
    return SPXERR_NOT_IMPL;
}

SPXAPI RecognizerFactory_CreateIntentRecognzier_With_FileInput(SPXRECOHANDLE* phreco, const wchar_t* pszFileName)
{
    UNUSED(phreco);
    UNUSED(pszFileName);
    return SPXERR_NOT_IMPL;
}

SPXAPI RecognizerFactory_CreateIntentRecognzier_With_PassiveListening(SPXRECOHANDLE* phreco, bool passive)
{
    UNUSED(phreco);
    UNUSED(passive);
    return SPXERR_NOT_IMPL;
}

SPXAPI RecognizerFactory_CreateIntentRecognzier(SPXRECOHANDLE* phreco, const wchar_t* pszLanguage, const wchar_t* pszFileName, bool passive)
{
    UNUSED(phreco);
    UNUSED(pszLanguage);
    UNUSED(pszFileName);
    UNUSED(passive);
    return SPXERR_NOT_IMPL;
}
