#include "stdafx.h"


using namespace CARBON_IMPL_NAMESPACE();


/// <summary>
/// Creates a recognizer object using default construction parameters.
/// </summary>
/// <param name="phreco"> Address of pointer variable that receives the recognizer handle. 
///    Upon successful return, <paramref name="*phreco"/> contains the recognizer handle. 
///    Upon failure, <paramref name="*phreco"/> contains nullptr. </param>
/// <returns>This function can return the following values.
/// <list type="table">
/// <listheader>
/// <term> Return code </term>
/// <term> Description </term>
/// </listheader>
/// <item>
/// <term> SPX_NOERROR </term>
/// <term> The recognizer was created successfully. </term>
/// </item>
/// <item>
/// <term> SPXERR_NOT_IMPL </term>
/// <term> This function is not implemented. </term>
/// </item>
/// </list>
/// </returns>
SPXAPI RecognizerFactory_CreateSpeechRecognzier_With_Defaults(SPXRECOHANDLE* phreco)
{
    SPX_INIT_HR(hr);

    try
    {
        *phreco = SPXHANDLE_INVALID;
        auto recognizer = CSpxRecognizerFactory::CreateSpeechRecognizer();
        auto ht = CSpxHandleTableManager::Get<CSpxRecognizer, SPXRECOHANDLE>();
        *phreco = ht->TrackHandle(recognizer);
    }
    catch (std::exception ex)
    {
        SPX_RETURN_HR(SPXERR_UNHANDLED_EXCEPTION);
    }

    SPX_RETURN_HR(hr);
}

SPXAPI RecognizerFactory_CreateSpeechRecognzier_With_Language(SPXRECOHANDLE* phreco, const wchar_t* pszLanguage)
{
    return SPXERR_NOT_IMPL;
}

SPXAPI RecognizerFactory_CreateSpeechRecognzier_With_FileInput(SPXRECOHANDLE* phreco, const wchar_t* pszFileName)
{
    return SPXERR_NOT_IMPL;
}

SPXAPI RecognizerFactory_CreateSpeechRecognzier_With_PassiveListening(SPXRECOHANDLE* phreco, bool passive)
{
    return SPXERR_NOT_IMPL;
}

SPXAPI RecognizerFactory_CreateSpeechRecognzier(SPXRECOHANDLE* phreco, const wchar_t* pszLanguage, const wchar_t* pszFileName, bool passive)
{
    return SPXERR_NOT_IMPL;
}

SPXAPI RecognizerFactory_CreateDictationRecognzier_With_Defaults(SPXRECOHANDLE* phreco)
{
    return SPXERR_NOT_IMPL;
}

SPXAPI RecognizerFactory_CreateDictationRecognzier_With_Language(SPXRECOHANDLE* phreco, const wchar_t* pszLanguage)
{
    return SPXERR_NOT_IMPL;
}

SPXAPI RecognizerFactory_CreateDictationRecognzier_With_FileInput(SPXRECOHANDLE* phreco, const wchar_t* pszFileName)
{
    return SPXERR_NOT_IMPL;
}

SPXAPI RecognizerFactory_CreateDictationRecognzier_With_PassiveListening(SPXRECOHANDLE* phreco, bool passive)
{
    return SPXERR_NOT_IMPL;
}

SPXAPI RecognizerFactory_CreateDictationRecognzier(SPXRECOHANDLE* phreco, const wchar_t* pszLanguage, const wchar_t* pszFileName, bool passive)
{
    return SPXERR_NOT_IMPL;
}

SPXAPI RecognizerFactory_CreateIntentRecognzier_With_Defaults(SPXRECOHANDLE* phreco)
{
    return SPXERR_NOT_IMPL;
}

SPXAPI RecognizerFactory_CreateIntentRecognzier_With_Language(SPXRECOHANDLE* phreco, const wchar_t* pszLanguage)
{
    return SPXERR_NOT_IMPL;
}

SPXAPI RecognizerFactory_CreateIntentRecognzier_With_FileInput(SPXRECOHANDLE* phreco, const wchar_t* pszFileName)
{
    return SPXERR_NOT_IMPL;
}

SPXAPI RecognizerFactory_CreateIntentRecognzier_With_PassiveListening(SPXRECOHANDLE* phreco, bool passive)
{
    return SPXERR_NOT_IMPL;
}

SPXAPI RecognizerFactory_CreateIntentRecognzier(SPXRECOHANDLE* phreco, const wchar_t* pszLanguage, const wchar_t* pszFileName, bool passive)
{
    return SPXERR_NOT_IMPL;
}
