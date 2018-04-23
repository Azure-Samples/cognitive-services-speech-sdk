//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speech_c_recognizer_factory.h: Public API declarations for RecognizerFactory related C methods
//

#pragma once
#include <speechapi_c_common.h>
#include <speechapi_c_audioinputstream.h>

SPXAPI RecognizerFactory_CreateSpeechRecognizer_With_Defaults(SPXRECOFACTORYHANDLE hrecofactory, SPXRECOHANDLE* phreco);
SPXAPI RecognizerFactory_CreateSpeechRecognizer_With_Language(SPXRECOFACTORYHANDLE hrecofactory, SPXRECOHANDLE* phreco, const wchar_t* pszLanguage);
SPXAPI RecognizerFactory_CreateSpeechRecognizer_With_FileInput(SPXRECOFACTORYHANDLE hrecofactory, SPXRECOHANDLE* phreco, const wchar_t* pszFileName);
SPXAPI RecognizerFactory_CreateSpeechRecognizer_With_FileInputAndLanguage(SPXRECOFACTORYHANDLE hrecofactory, SPXRECOHANDLE* phreco, const wchar_t* pszLanguage, const wchar_t* pszFileName);
SPXAPI RecognizerFactory_CreateSpeechRecognizer_With_Stream(SPXRECOFACTORYHANDLE hrecofactory, SPXRECOHANDLE* phreco, SpeechApi_AudioInputStream *stream);

SPXAPI RecognizerFactory_CreateIntentRecognizer_With_Defaults(SPXRECOFACTORYHANDLE hrecofactory, SPXRECOHANDLE* phreco);
SPXAPI RecognizerFactory_CreateIntentRecognizer_With_Language(SPXRECOFACTORYHANDLE hrecofactory, SPXRECOHANDLE* phreco, const wchar_t* pszLanguage);
SPXAPI RecognizerFactory_CreateIntentRecognizer_With_FileInput(SPXRECOFACTORYHANDLE hrecofactory, SPXRECOHANDLE* phreco, const wchar_t* pszFileName);
SPXAPI RecognizerFactory_CreateIntentRecognizer(SPXRECOFACTORYHANDLE hrecofactory, SPXRECOHANDLE* phreco, const wchar_t* pszLanguage, const wchar_t* pszFileName);

SPXAPI RecognizerFactory_CreateTranslationRecognizer(SPXRECOFACTORYHANDLE hrecofactory, SPXRECOHANDLE* phreco, const wchar_t* sourceLanguage, const wchar_t* targetLanguage);
SPXAPI RecognizerFactory_CreateTranslationRecognizer_With_FileInput(SPXRECOFACTORYHANDLE hrecofactory, SPXRECOHANDLE* phreco, const wchar_t* sourceLanguage, const wchar_t* targetLanguage, const wchar_t* pszFileName);
SPXAPI RecognizerFactory_CreateTranslationRecognizer_With_Stream(SPXRECOFACTORYHANDLE hrecofactory, SPXRECOHANDLE* phreco, const wchar_t* sourceLanguage, const wchar_t* targetLanguage, SpeechApi_AudioInputStream *stream);

enum Factory_Parameter { FactoryParameter_Region = 1, FactoryParameter_SubscriptionKey = 2, FactoryParameter_AuthorizationToken = 3, FactoryParameter_Endpoint = 4 };
typedef enum Factory_Parameter Factory_Parameter;
SPXAPI RecognizerFactory_GetParameter_Name(Factory_Parameter parameter, wchar_t* name, uint32_t cchName);

SPXAPI RecognizerFactory_SetParameter_String(SPXRECOFACTORYHANDLE hrecofactory, const wchar_t* name, const wchar_t* value);
SPXAPI RecognizerFactory_GetParameter_String(SPXRECOFACTORYHANDLE hrecofactory, const wchar_t* name, wchar_t* value, uint32_t cchValue, const wchar_t* defaultValue);
SPXAPI_(bool) RecognizerFactory_ContainsParameter_String(SPXRECOFACTORYHANDLE hrecofactory, const wchar_t* name);

SPXAPI RecognizerFactory_SetParameter_Int32(SPXRECOFACTORYHANDLE hrecofactory, const wchar_t* name, int32_t value);
SPXAPI RecognizerFactory_GetParameter_Int32(SPXRECOFACTORYHANDLE hrecofactory, const wchar_t* name, int32_t* pvalue, int32_t defaultValue);
SPXAPI_(bool) RecognizerFactory_ContainsParameter_Int32(SPXRECOFACTORYHANDLE hrecofactory, const wchar_t* name);

SPXAPI RecognizerFactory_SetParameter_Bool(SPXRECOFACTORYHANDLE hrecofactory, const wchar_t* name, bool value);
SPXAPI RecognizerFactory_GetParameter_Bool(SPXRECOFACTORYHANDLE hrecofactory, const wchar_t* name, bool* pvalue, bool defaultValue);
SPXAPI_(bool) RecognizerFactory_ContainsParameter_Bool(SPXRECOFACTORYHANDLE hrecofactory, const wchar_t* name);
