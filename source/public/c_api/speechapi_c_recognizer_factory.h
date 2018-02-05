//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speech_c_recognizer_factory.h: Public API declarations for RecognizerFactory related C methods
//

#pragma once
#include <spxdebug.h>
#include <speechapi_c_common.h>

SPXAPI RecognizerFactory_CreateSpeechRecognizer_With_Defaults(SPXRECOHANDLE* phreco);
SPXAPI RecognizerFactory_CreateSpeechRecognizer_With_Language(SPXRECOHANDLE* phreco, const wchar_t* pszLanguage);
SPXAPI RecognizerFactory_CreateSpeechRecognizer_With_FileInput(SPXRECOHANDLE* phreco, const wchar_t* pszFileName);
SPXAPI RecognizerFactory_CreateSpeechRecognizer_With_PassiveListening(SPXRECOHANDLE* phreco, bool passive);
SPXAPI RecognizerFactory_CreateSpeechRecognizer(SPXRECOHANDLE* phreco, const wchar_t* pszLanguage, const wchar_t* pszFileName, bool passive);

SPXAPI RecognizerFactory_CreateDictationRecognizer_With_Defaults(SPXRECOHANDLE* phreco);
SPXAPI RecognizerFactory_CreateDictationRecognizer_With_Language(SPXRECOHANDLE* phreco, const wchar_t* pszLanguage);
SPXAPI RecognizerFactory_CreateDictationRecognizer_With_FileInput(SPXRECOHANDLE* phreco, const wchar_t* pszFileName);
SPXAPI RecognizerFactory_CreateDictationRecognizer_With_PassiveListening(SPXRECOHANDLE* phreco, bool passive);
SPXAPI RecognizerFactory_CreateDictationRecognizer(SPXRECOHANDLE* phreco, const wchar_t* pszLanguage, const wchar_t* pszFileName, bool passive);

SPXAPI RecognizerFactory_CreateIntentRecognizer_With_Defaults(SPXRECOHANDLE* phreco);
SPXAPI RecognizerFactory_CreateIntentRecognizer_With_Language(SPXRECOHANDLE* phreco, const wchar_t* pszLanguage);
SPXAPI RecognizerFactory_CreateIntentRecognizer_With_FileInput(SPXRECOHANDLE* phreco, const wchar_t* pszFileName);
SPXAPI RecognizerFactory_CreateIntentRecognizer_With_PassiveListening(SPXRECOHANDLE* phreco, bool passive);
SPXAPI RecognizerFactory_CreateIntentRecognizer(SPXRECOHANDLE* phreco, const wchar_t* pszLanguage, const wchar_t* pszFileName, bool passive);
