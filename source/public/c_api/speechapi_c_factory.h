//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speech_c_recognizer_factory.h: Public API declarations for SpeechFactory related C methods
//

#pragma once
#include <speechapi_c_common.h>
#include <speechapi_c_audioinputstream.h>

typedef enum { SpeechOutputFormat_Simple = 0, SpeechOutputFormat_Detailed = 1 } SpeechOutputFormat;

SPXAPI SpeechFactory_CreateSpeechRecognizer_With_Defaults(SPXFACTORYHANDLE hfactory, SPXRECOHANDLE* phreco);
SPXAPI SpeechFactory_CreateSpeechRecognizer_With_Language(SPXFACTORYHANDLE hfactory, SPXRECOHANDLE* phreco, const char* pszLanguage);
SPXAPI SpeechFactory_CreateSpeechRecognizer_With_LanguageAndFormat(SPXFACTORYHANDLE hfactory, SPXRECOHANDLE* phreco, const char* pszLanguage, SpeechOutputFormat format);

SPXAPI SpeechFactory_CreateSpeechRecognizer_With_FileInput(SPXFACTORYHANDLE hfactory, SPXRECOHANDLE* phreco, const char* pszFileName);
SPXAPI SpeechFactory_CreateSpeechRecognizer_With_FileInputAndLanguage(SPXFACTORYHANDLE hfactory, SPXRECOHANDLE* phreco, const char* pszLanguage, const char* pszFileName);
SPXAPI SpeechFactory_CreateSpeechRecognizer_With_FileInputAndLanguageAndFormat(SPXFACTORYHANDLE hfactory, SPXRECOHANDLE* phreco, const char* pszLanguage, const char* pszFileName, SpeechOutputFormat format);
SPXAPI SpeechFactory_CreateSpeechRecognizer_With_Stream(SPXFACTORYHANDLE hfactory, SPXRECOHANDLE* phreco, SpeechApi_AudioInputStream *stream);
SPXAPI SpeechFactory_CreateSpeechRecognizer_With_StreamAndLanguage(SPXFACTORYHANDLE hfactory, SPXRECOHANDLE* phreco, SpeechApi_AudioInputStream *stream, const char* pszLanguage);
SPXAPI SpeechFactory_CreateSpeechRecognizer_With_StreamAndLanguageAndFormat(SPXFACTORYHANDLE hfactory, SPXRECOHANDLE* phreco, SpeechApi_AudioInputStream *stream, const char* pszLanguage, SpeechOutputFormat format);

SPXAPI SpeechFactory_CreateIntentRecognizer_With_Defaults(SPXFACTORYHANDLE hfactory, SPXRECOHANDLE* phreco);
SPXAPI SpeechFactory_CreateIntentRecognizer_With_Language(SPXFACTORYHANDLE hfactory, SPXRECOHANDLE* phreco, const char* pszLanguage);
SPXAPI SpeechFactory_CreateIntentRecognizer_With_FileInput(SPXFACTORYHANDLE hfactory, SPXRECOHANDLE* phreco, const char* pszFileName);
SPXAPI SpeechFactory_CreateIntentRecognizer_With_FileInputAndLanguage(SPXFACTORYHANDLE hfactory, SPXRECOHANDLE* phreco, const char* pszLanguage, const char* pszFileName);
SPXAPI SpeechFactory_CreateIntentRecognizer_With_Stream(SPXFACTORYHANDLE hfactory, SPXRECOHANDLE* phreco, SpeechApi_AudioInputStream *stream);
SPXAPI SpeechFactory_CreateIntentRecognizer_With_StreamAndLanguage(SPXFACTORYHANDLE hfactory, SPXRECOHANDLE* phreco, SpeechApi_AudioInputStream *stream, const char* pszLanguage);

SPXAPI SpeechFactory_CreateTranslationRecognizer(SPXFACTORYHANDLE hfactory, SPXRECOHANDLE* recoHandlePointer, const char* sourceLanguage, const char* targetLanguages[], size_t numberOfTargetLanguages, const char* voice);
SPXAPI SpeechFactory_CreateTranslationRecognizer_With_FileInput(SPXFACTORYHANDLE hfactory, SPXRECOHANDLE* recoHandlePointer, const char* sourceLanguage, const char* targetLanguages[], size_t numberOfTargetLanguages, const char* voice, const char* fileName);
SPXAPI SpeechFactory_CreateTranslationRecognizer_With_Stream(SPXFACTORYHANDLE hfactory, SPXRECOHANDLE* recoHandlePointer, const char* sourceLanguage, const char* targetLanguages[], size_t numberOfTargetLanguages, const char* voice, SpeechApi_AudioInputStream *stream);

SPXAPI_(bool) SpeechFactory_Handle_IsValid(SPXFACTORYHANDLE hreco);
SPXAPI SpeechFactory_Handle_Close(SPXFACTORYHANDLE hreco);

SPXAPI speech_factory_get_property_bag(SPXFACTORYHANDLE hfactory, SPXPROPERTYBAGHANDLE* hpropbag);

SPXAPI SpeechFactory_FromAuthorizationToken(const char* authToken, const char* region, SPXFACTORYHANDLE* phfactory);
SPXAPI SpeechFactory_FromSubscription(const char* subscriptionKey, const char* region, SPXFACTORYHANDLE* phfactory);
SPXAPI SpeechFactory_FromEndpoint(const char* endpoint, const char* subscription, SPXFACTORYHANDLE* phfactory);
