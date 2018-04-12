//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//


#pragma once
#include <speechapi_c_common.h>


// Todo: Translation recognizer management API.

typedef void(*PTRANSLATIONSYNTHESIS_CALLBACK_FUNC)(SPXRECOHANDLE hreco, SPXEVENTHANDLE hevent, void* pvContext);
SPXAPI TranslationRecognizer_TranslationSynthesis_SetEventCallback(SPXRECOHANDLE hreco, PTRANSLATIONSYNTHESIS_CALLBACK_FUNC pCallback, void* pvContext);