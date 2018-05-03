//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speech_c_recognizer_factory.h: Public API declarations for SpeechFactory related C methods
//

#pragma once
#include <speechapi_c_common.h>
#include <speechapi_c_audioinputstream.h>

SPXAPI SpeechFactory_CreateSpeechRecognizer_With_Defaults(SPXFACTORYHANDLE hfactory, SPXRECOHANDLE* phreco);
SPXAPI SpeechFactory_CreateSpeechRecognizer_With_Language(SPXFACTORYHANDLE hfactory, SPXRECOHANDLE* phreco, const wchar_t* pszLanguage);
SPXAPI SpeechFactory_CreateSpeechRecognizer_With_FileInput(SPXFACTORYHANDLE hfactory, SPXRECOHANDLE* phreco, const wchar_t* pszFileName);
SPXAPI SpeechFactory_CreateSpeechRecognizer_With_FileInputAndLanguage(SPXFACTORYHANDLE hfactory, SPXRECOHANDLE* phreco, const wchar_t* pszLanguage, const wchar_t* pszFileName);

SPXAPI SpeechFactory_CreateIntentRecognizer_With_Defaults(SPXFACTORYHANDLE hfactory, SPXRECOHANDLE* phreco);
SPXAPI SpeechFactory_CreateIntentRecognizer_With_Language(SPXFACTORYHANDLE hfactory, SPXRECOHANDLE* phreco, const wchar_t* pszLanguage);
SPXAPI SpeechFactory_CreateIntentRecognizer_With_FileInput(SPXFACTORYHANDLE hfactory, SPXRECOHANDLE* phreco, const wchar_t* pszFileName);
SPXAPI SpeechFactory_CreateIntentRecognizer_With_FileInputAndLanguage(SPXFACTORYHANDLE hfactory, SPXRECOHANDLE* phreco, const wchar_t* pszLanguage, const wchar_t* pszFileName);

SPXAPI SpeechFactory_CreateTranslationRecognizer(SPXFACTORYHANDLE hfactory, SPXRECOHANDLE* recoHandlePointer, const wchar_t* sourceLanguage, const wchar_t* targetLanguages[], size_t numberOfTargetLanguages, const wchar_t* voice);
SPXAPI SpeechFactory_CreateTranslationRecognizer_With_FileInput(SPXFACTORYHANDLE hfactory, SPXRECOHANDLE* recoHandlePointer, const wchar_t* sourceLanguage, const wchar_t* targetLanguages[], size_t numberOfTargetLanguages, const wchar_t* voice, const wchar_t* fileName);

enum Factory_Parameter { FactoryParameter_Region = 1, FactoryParameter_SubscriptionKey = 2, FactoryParameter_AuthorizationToken = 3, FactoryParameter_Endpoint = 4 };
typedef enum Factory_Parameter Factory_Parameter;
SPXAPI SpeechFactory_GetParameter_Name(Factory_Parameter parameter, wchar_t* name, uint32_t cchName);

SPXAPI SpeechFactory_SetParameter_String(SPXFACTORYHANDLE hfactory, const wchar_t* name, const wchar_t* value);
SPXAPI SpeechFactory_GetParameter_String(SPXFACTORYHANDLE hfactory, const wchar_t* name, wchar_t* value, uint32_t cchValue, const wchar_t* defaultValue);
SPXAPI_(bool) SpeechFactory_ContainsParameter_String(SPXFACTORYHANDLE hfactory, const wchar_t* name);

SPXAPI SpeechFactory_SetParameter_Int32(SPXFACTORYHANDLE hfactory, const wchar_t* name, int32_t value);
SPXAPI SpeechFactory_GetParameter_Int32(SPXFACTORYHANDLE hfactory, const wchar_t* name, int32_t* pvalue, int32_t defaultValue);
SPXAPI_(bool) SpeechFactory_ContainsParameter_Int32(SPXFACTORYHANDLE hfactory, const wchar_t* name);

SPXAPI SpeechFactory_SetParameter_Bool(SPXFACTORYHANDLE hfactory, const wchar_t* name, bool value);
SPXAPI SpeechFactory_GetParameter_Bool(SPXFACTORYHANDLE hfactory, const wchar_t* name, bool* pvalue, bool defaultValue);
SPXAPI_(bool) SpeechFactory_ContainsParameter_Bool(SPXFACTORYHANDLE hfactory, const wchar_t* name);

SPXAPI_(bool) SpeechFactory_Handle_IsValid(SPXFACTORYHANDLE hreco);
SPXAPI SpeechFactory_Handle_Close(SPXFACTORYHANDLE hreco);

SPXAPI SpeechFactory_FromAuthorizationToken(const wchar_t* authToken, const wchar_t* region, SPXFACTORYHANDLE* phfactory);
SPXAPI SpeechFactory_FromSubscription(const wchar_t* subscriptionKey, const wchar_t* region, SPXFACTORYHANDLE* phfactory);
SPXAPI SpeechFactory_FromEndpoint(const wchar_t* endpoint, const wchar_t* subscription, SPXFACTORYHANDLE* phfactory);
