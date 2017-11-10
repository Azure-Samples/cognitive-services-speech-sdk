#pragma once
#include <spxdebug.h>
#include <speechapi_c_common.h>

SPXAPI RecognizerFactory_CreateSpeechRecognzier_With_Defaults(SPXRECOHANDLE* phreco);
SPXAPI RecognizerFactory_CreateSpeechRecognzier_With_Language(SPXRECOHANDLE* phreco, const wchar_t* pszLanguage);
SPXAPI RecognizerFactory_CreateSpeechRecognzier_With_FileInput(SPXRECOHANDLE* phreco, const wchar_t* pszFileName);
SPXAPI RecognizerFactory_CreateSpeechRecognzier_With_PassiveListening(SPXRECOHANDLE* phreco, bool passive);
SPXAPI RecognizerFactory_CreateSpeechRecognzier(SPXRECOHANDLE* phreco, const wchar_t* pszLanguage, const wchar_t* pszFileName, bool passive);

SPXAPI RecognizerFactory_CreateDictationRecognzier_With_Defaults(SPXRECOHANDLE* phreco);
SPXAPI RecognizerFactory_CreateDictationRecognzier_With_Language(SPXRECOHANDLE* phreco, const wchar_t* pszLanguage);
SPXAPI RecognizerFactory_CreateDictationRecognzier_With_FileInput(SPXRECOHANDLE* phreco, const wchar_t* pszFileName);
SPXAPI RecognizerFactory_CreateDictationRecognzier_With_PassiveListening(SPXRECOHANDLE* phreco, bool passive);
SPXAPI RecognizerFactory_CreateDictationRecognzier(SPXRECOHANDLE* phreco, const wchar_t* pszLanguage, const wchar_t* pszFileName, bool passive);

SPXAPI RecognizerFactory_CreateIntentRecognzier_With_Defaults(SPXRECOHANDLE* phreco);
SPXAPI RecognizerFactory_CreateIntentRecognzier_With_Language(SPXRECOHANDLE* phreco, const wchar_t* pszLanguage);
SPXAPI RecognizerFactory_CreateIntentRecognzier_With_FileInput(SPXRECOHANDLE* phreco, const wchar_t* pszFileName);
SPXAPI RecognizerFactory_CreateIntentRecognzier_With_PassiveListening(SPXRECOHANDLE* phreco, bool passive);
SPXAPI RecognizerFactory_CreateIntentRecognzier(SPXRECOHANDLE* phreco, const wchar_t* pszLanguage, const wchar_t* pszFileName, bool passive);
