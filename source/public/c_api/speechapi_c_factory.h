//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speech_c_recognizer_factory.h: Public API declarations for SpeechFactory related C methods
//

#pragma once
#include <speechapi_c_common.h>


typedef enum { SpeechOutputFormat_Simple = 0, SpeechOutputFormat_Detailed = 1 } SpeechOutputFormat;

SPXAPI_(bool) speech_factory_handle_is_valid(SPXFACTORYHANDLE hreco);

SPXAPI speech_factory_from_authorization_token(const char* authToken, const char* region, SPXFACTORYHANDLE* phfactory);
SPXAPI speech_factory_from_subscription(const char* subscriptionKey, const char* region, SPXFACTORYHANDLE* phfactory);
SPXAPI speech_factory_from_endpoint(const char* endpoint, const char* subscription, SPXFACTORYHANDLE* phfactory);

SPXAPI speech_factory_get_property_bag(SPXFACTORYHANDLE hfactory, SPXPROPERTYBAGHANDLE* hpropbag);

SPXAPI speech_factory_create_speech_recognizer_from_config(SPXFACTORYHANDLE hfactory, SPXRECOHANDLE* phreco, const char* pszLanguage, SpeechOutputFormat format, SPXAUDIOCONFIGHANDLE haudioInput);
SPXAPI speech_factory_create_intent_recognizer_from_config(SPXFACTORYHANDLE hfactory, SPXRECOHANDLE* phreco, const char* pszLanguage, SpeechOutputFormat format, SPXAUDIOCONFIGHANDLE haudioInput);
SPXAPI speech_factory_create_translation_recognizer_from_config(SPXFACTORYHANDLE hfactory, SPXRECOHANDLE* phreco, const char* sourceLanguage, const char* targetLanguages[], size_t numberOfTargetLanguages, const char* voice, SPXAUDIOCONFIGHANDLE haudioInput);

SPXAPI speech_factory_handle_close(SPXFACTORYHANDLE hreco);
